/*

FSM.

Copyright (C) 2018 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Revision: 11612 $ $Date:: 2019-05-24 #$ $Author: serge $

#include "process.h"            // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "utils/dummy_logger.h"     // dummy_logi_debug
#include "scheduler/timeout_job_aux.h"      // create_and_insert_timeout_job
#include "anyvalue/value_operations.h"      // compare_values
#include "anyvalue/str_helper.h"    // anyvalue::StrHelper

#include "str_helper.h"             // StrHelper
#include "syntax_error.h"           // SyntaxError

namespace fsm {

Process::Process(
        uint32_t                id,
        uint32_t                log_id,
        IFsm                    * parent,
        ICallback               * callback,
        scheduler::IScheduler   * scheduler ):
        id_( id ),
        log_id_( log_id ),
        parent_( parent ),
        callback_( callback ),
        scheduler_( scheduler ),
        internal_state_( internal_state_e::IDLE ),
        current_state_( 0 ),
        start_action_connector_( 0 ),
        matched_switch_condition_( 0 ),
        names_( id, log_id ),
        mem_( id, log_id, & req_id_gen_, & names_ )

{
    req_id_gen_.init( 1, 1 );

    dummy_logi_info( log_id_, id_, "created" );
}

Process::~Process()
{
    for( auto & e : map_id_to_state_ )
    {
        delete e.second;
    }

    dummy_logi_info( log_id_, id_, "destructed" );
}

void Process::start()
{
    dummy_logi_trace( log_id_, id_, "start" );

    assert( internal_state_ == internal_state_e::IDLE );

    internal_state_ = internal_state_e::ACTIVE;

    dummy_logi_debug( log_id_, id_, "start: start_action_connector %u", start_action_connector_ );

    if( start_action_connector_ == 0 )
    {
        dummy_logi_fatal( log_id_, id_, "start: start_action_connector is not set" );
        throw SyntaxError( "start: start_action_connector is not set" );
    }

    execute_action_connector_id( start_action_connector_ );
}

void Process::handle_signal_handler( element_id_t signal_handler_id, const std::vector<element_id_t> & arguments )
{
    dummy_logi_trace( log_id_, id_, "handle_signal_handler: signal handler id %u", signal_handler_id );

    auto it = map_id_to_signal_handler_.find( signal_handler_id );

    if( it == map_id_to_signal_handler_.end() )
    {
        dummy_logi_fatal( log_id_, id_, "handle_signal_handler: cannot find signal handler id %u", signal_handler_id );
        throw SyntaxError( "signal handler id " + std::to_string( signal_handler_id ) + " not found" );
        return;
    }

    auto & h = it->second;

    auto first_action_id = h->get_first_action_id();

    dummy_logi_debug( log_id_, id_, "handle_signal_handler: signal handler id %u, first action id %u", signal_handler_id, first_action_id );

    if( first_action_id )
    {
        execute_action_connector_id( first_action_id );
    }
}

bool Process::is_ended() const
{
    return internal_state_ == internal_state_e::FINISHED;
}

element_id_t Process::create_add_start_action_connector( Action * action )
{
    if( start_action_connector_ != 0 )
    {
        dummy_logi_fatal( log_id_, id_, "start_action_connector is already defined (%u)", start_action_connector_ );
        throw SyntaxError( "start_action_connector is already defined (" + std::to_string( start_action_connector_ ) + ")" );
    }

    auto id = create_action_connector( action );

    start_action_connector_ = id;

    dummy_logi_trace( log_id_, id_, "create_add_start_action_connector: start_action_connector %u", start_action_connector_ );

    return id;
}

element_id_t Process::create_state( const std::string & name )
{
    auto id = get_next_id();

    auto state = new State( log_id_, id, id_, name, this );

    auto b = map_id_to_state_.insert( std::make_pair( id, state ) ).second;

    assert( b );

    dummy_logi_debug( log_id_, id_, "create_state: %s (%u)", name.c_str(), id );

    names_.add_name( id, name );

    return id;
}

element_id_t Process::create_add_signal_handler( element_id_t state_id, const std::string & signal_name )
{
    dummy_logi_trace( log_id_, id_, "create_add_signal_handler: state id %u, signal name %s", state_id, signal_name.c_str() );

    auto state = find_state( state_id );

    if( state == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "create_add_signal_handler: cannot find state id %u", state_id );
        throw SyntaxError( "state id " + std::to_string( state_id ) + " not found" );
        return 0;
    }

    auto name = signal_name + " in " + state->get_name();

    auto id = create_signal_handler( name );

    state->add_signal_handler( signal_name, id );

    return id;
}

void Process::set_first_action_connector( element_id_t signal_handler_id, element_id_t action_connector_id )
{
    dummy_logi_trace( log_id_, id_, "set_first_action_connector: signal handler id %u, action_connector_id %u", signal_handler_id, action_connector_id );

    auto it = map_id_to_signal_handler_.find( signal_handler_id );

    if( it == map_id_to_signal_handler_.end() )
    {
        dummy_logi_fatal( log_id_, id_, "set_first_action_connector: cannot find signal handler id %u", signal_handler_id );
        throw SyntaxError( "signal handler id " + std::to_string( signal_handler_id ) + " not found" );
        return;
    }

    it->second->set_first_action_id( action_connector_id );
}

element_id_t Process::create_set_first_action_connector( element_id_t signal_handler_id, Action * action )
{
    auto id = create_action_connector( action );

    set_first_action_connector( signal_handler_id, id );

    return id;
}

void Process::set_next_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id )
{
    set_next_action_connector_intern( action_connector_id, next_action_connector_id, next_action_type_e::MAIN );
}

void Process::set_alt_next_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id )
{
    set_next_action_connector_intern( action_connector_id, next_action_connector_id, next_action_type_e::ALT );
}

void Process::set_default_switch_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id )
{
    set_next_action_connector_intern( action_connector_id, next_action_connector_id, next_action_type_e::SWITCH_DEFAULT );
}

void Process::add_switch_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id )
{
    set_next_action_connector_intern( action_connector_id, next_action_connector_id, next_action_type_e::SWITCH_NEXT );
}

void Process::set_next_action_connector_intern( element_id_t action_connector_id, element_id_t next_action_connector_id, next_action_type_e type )
{
    dummy_logi_trace( log_id_, id_, "set_next_action_connector_intern: action_connector_id %u, next_action_connector_id %u, type %u", action_connector_id, next_action_connector_id, unsigned( type ) );

    auto action_connector = find_action_connector( action_connector_id );

    if( action_connector == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "set_next_action_connector_intern: cannot find action_connector_id %u", action_connector_id );
        assert( 0 );
        throw SyntaxError( "action connector id " + std::to_string( action_connector_id ) + " not found" );
        return;
    }

    switch( type )
    {
    case next_action_type_e::MAIN:
        action_connector->set_next_id( next_action_connector_id );
        break;

    case next_action_type_e::ALT:
        action_connector->set_alt_next_id( next_action_connector_id );
        break;

    case next_action_type_e::SWITCH_DEFAULT:
        action_connector->set_default_switch_action( next_action_connector_id );
        break;

    case next_action_type_e::SWITCH_NEXT:
        action_connector->add_switch_action( next_action_connector_id );
        break;

    default:
        dummy_logi_fatal( log_id_, id_, "set_next_action_connector_intern: unsupported type %u", unsigned( type ) );
        assert( 0 );
        throw SyntaxError( "unsupported type " + std::to_string( unsigned( type ) ) );
    }
}

element_id_t Process::create_set_next_action_connector( element_id_t action_connector_id, Action * action )
{
    return create_set_next_action_connector_intern( action_connector_id, action, next_action_type_e::MAIN );
}

element_id_t Process::create_set_alt_next_action_connector( element_id_t action_connector_id, Action * action )
{
    return create_set_next_action_connector_intern( action_connector_id, action, next_action_type_e::ALT );
}

element_id_t Process::create_set_default_switch_action_connector( element_id_t action_connector_id, Action * action )
{
    return create_set_next_action_connector_intern( action_connector_id, action, next_action_type_e::SWITCH_DEFAULT );
}

element_id_t Process::create_add_switch_action_connector( element_id_t action_connector_id, Action * action )
{
    return create_set_next_action_connector_intern( action_connector_id, action, next_action_type_e::SWITCH_NEXT );
}

element_id_t Process::create_set_next_action_connector_intern( element_id_t action_connector_id, Action * action, next_action_type_e type )
{
    auto id = create_action_connector( action );

    set_next_action_connector_intern( action_connector_id, id, type );

    return id;
}

element_id_t Process::create_add_timer( const std::string & name )
{
    auto id = get_next_id();

    auto timer = new Timer( log_id_, id, name );

    auto b = map_id_to_timer_.insert( std::make_pair( id, timer ) ).second;

    assert( b );

    dummy_logi_debug( log_id_, id_, "create_add_timer: created timer %s (%u)", name.c_str(), id );

    names_.add_name( id, name );

    return id;
}

element_id_t Process::create_signal_handler( const std::string & name )
{
    auto id = get_next_id();

    auto signal_handler = new SignalHandler( log_id_, id, name );

    auto b = map_id_to_signal_handler_.insert( std::make_pair( id, signal_handler ) ).second;

    assert( b );

    dummy_logi_debug( log_id_, id_, "create_signal_handler: created signal handler '%s' (%u)", name.c_str(), id );

    names_.add_name( id, name );

    return id;
}

element_id_t Process::create_action_connector( Action * action )
{
    auto id = get_next_id();

    auto obj = new ActionConnector( log_id_, id, action );

    auto b = map_id_to_action_connector_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_logi_debug( log_id_, id_, "create_action_connector: created action connector %u", id );

    return id;
}

element_id_t Process::create_add_variable( const std::string & name, data_type_e type )
{
    return mem_.create_add_variable( name, type );
}

element_id_t Process::create_add_variable( const std::string & name, data_type_e type, const Value & value )
{
    return mem_.create_add_variable( name, type, value );
}

element_id_t Process::create_add_constant( const std::string & name, data_type_e type, const Value & value )
{
    return mem_.create_add_constant( name, type, value );
}

void Process::set_initial_state( element_id_t state_id )
{
    dummy_logi_trace( log_id_, id_, "set_initial_state: %u", state_id );

    assert( current_state_ == 0 );

    current_state_  = state_id;
}

void Process::handle( const ev::Signal & req )
{
    dummy_logi_trace( log_id_, id_, "handle: %s", typeid( req ).name() );

    if( is_ended() == true )
    {
        dummy_logi_info( log_id_, id_, "process finished, ignoring" );

        return;
    }

    assert( internal_state_ == internal_state_e::ACTIVE );

    auto state = find_state( current_state_ );

    assert( state != nullptr );

    std::vector<element_id_t> arguments;

    mem_.init_temp_variables_from_signal( req, & arguments );

    state->handle_signal( req.name, arguments );
}

void Process::handle( const ev::Timer & req )
{
    dummy_logi_trace( log_id_, id_, "handle: %s", typeid( req ).name() );

    if( is_ended() == true )
    {
        dummy_logi_info( log_id_, id_, "process finished, ignoring" );

        return;
    }

    assert( internal_state_ == internal_state_e::ACTIVE );

    auto timer = find_timer( req.timer_id );

    assert( timer != nullptr );

    auto job_id = timer->get_job_id();

    if( job_id == 0 )
    {
        dummy_logi_info( log_id_, id_, "timer %u is already cancelled", req.timer_id );
        return;
    }

    timer->set_job_id( 0 ); // cancel scheduler job id

    auto & name = timer->get_name();

    std::vector<Value> dummy;

    ev::Signal signal( id_, name, dummy );

    handle( signal );
}

State* Process::find_state( element_id_t id )
{
    {
        auto it = map_id_to_state_.find( id );

        if( it != map_id_to_state_.end() )
        {
            return it->second;
        }
    }

    return nullptr;
}

Timer* Process::find_timer( element_id_t id )
{
    {
        auto it = map_id_to_timer_.find( id );

        if( it != map_id_to_timer_.end() )
        {
            return it->second;
        }
    }

    return nullptr;
}

ActionConnector* Process::find_action_connector( element_id_t id )
{
    auto it = map_id_to_action_connector_.find( id );

    if( it != map_id_to_action_connector_.end() )
    {
        return it->second;
    }

    return nullptr;
}

const ActionConnector* Process::find_action_connector( element_id_t id ) const
{
    auto it = map_id_to_action_connector_.find( id );

    if( it != map_id_to_action_connector_.end() )
    {
        return it->second;
    }

    return nullptr;
}

void Process::set_timer( Timer * timer, const Value & delay )
{
    dummy_logi_trace( log_id_, id_, "set_timer: timer %s (%u), %.2f sec", timer->get_name().c_str(), timer->get_id(), delay.arg_d );

    auto timer_id   = timer->get_id();
    auto job_id     = timer->get_job_id();

    if( job_id != 0 )
    {
        dummy_logi_fatal( log_id_, id_, "timer id %u is active (job id %u)", timer_id, job_id );
        assert( 0 );
        throw SyntaxError( "timer " + std::to_string( timer_id ) + " is active (job id " + std::to_string( job_id ) + ")" );
        return;
    }

    auto & name = timer->get_name();

    std::vector<Value> dummy;

    auto signal = new ev::Timer( id_, timer_id );

    std::string error_msg;

    scheduler::job_id_t sched_job_id;

    auto b = scheduler::create_and_insert_timeout_job(
            & sched_job_id,
            & error_msg,
            * scheduler_,
            "timer_job",
            scheduler::Duration( delay.arg_d ),
            std::bind( static_cast<void (IFsm::*)(const ev::Object * )>(&IFsm::consume), parent_, signal ) );

    if( b == false )
    {
        dummy_logi_error( log_id_, id_, "cannot set timer: %s", error_msg.c_str() );

        timer->set_job_id( 0 );

        delete signal;
    }
    else
    {
        dummy_logi_debug( log_id_, id_, "timer %s, process %u, scheduled execution in: %.2f sec", name.c_str(), signal->process_id, delay.arg_d );

        timer->set_job_id( sched_job_id );
    }

}

void Process::reset_timer( Timer * timer )
{
    dummy_logi_trace( log_id_, id_, "reset_timer: timer %s (%u)", timer->get_name().c_str(), timer->get_id() );

    auto & name = timer->get_name();

    auto timer_id       = timer->get_id();
    auto sched_job_id   = timer->get_job_id();

    if( sched_job_id == 0 )
    {
        dummy_logi_debug( log_id_, id_, "reset_timer: timer id %u, job id is 0", timer_id );

        return;
    }

    std::string error_msg;

    auto b = scheduler_->delete_job( & error_msg, sched_job_id );

    dummy_logi_trace( log_id_, id_, "reset_timer: job id %u", sched_job_id );

    if( b == false )
    {
        dummy_logi_error( log_id_, id_, "cannot reset timer: %s", error_msg.c_str() );
    }
    else
    {
        dummy_logi_debug( log_id_, id_, "reset timer %s", name.c_str() );
    }

    timer->set_job_id( 0 );
}

void Process::convert_values_to_value_pointers( std::vector<Value*> * value_pointers, std::vector<Value> & values )
{
    for( auto & e : values )
    {
        auto * p = & e;

        value_pointers->push_back( p );
    }
}

void Process::set_matched_switch_condition( int matched_switch_condition )
{
    assert( matched_switch_condition != 0 );
    assert( matched_switch_condition_ == 0 );

    matched_switch_condition_   = matched_switch_condition;
}

int Process::get_matched_switch_condition_and_clear()
{
    assert( matched_switch_condition_ != 0 );

    auto res    = matched_switch_condition_;

    matched_switch_condition_   = 0;

    return res;
}

void Process::execute_action_connector_id( element_id_t action_connector_id )
{
    dummy_logi_trace( log_id_, id_, "execute_action_connector_id: action_connector_id %u", action_connector_id );

    auto action_connector = find_action_connector( action_connector_id );

    if( action_connector == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find action connector %u", action_connector_id );
        assert( 0 );
        throw SyntaxError( "cannot find action connector " + std::to_string( action_connector_id ) );
    }

    execute_action_connector( * action_connector );
}

void Process::execute_action_connector( const ActionConnector & action_connector )
{
    auto & action = * action_connector.get_action();

    auto flow_control = handle_action( action );

    if( flow_control == flow_control_e::NEXT )
    {
        execute_action_connector_id( action_connector.get_next_id() );
    }
    else if( flow_control == flow_control_e::ALT_NEXT )
    {
        execute_action_connector_id( action_connector.get_alt_next_id() );
    }
    else if( flow_control == flow_control_e::CHECK_SWITCH )
    {
        auto matched_switch_condition = get_matched_switch_condition_and_clear();

        auto id = action_connector.get_switch_action( matched_switch_condition );

        execute_action_connector_id( id );
    }
    else // if( flow_control == flow_control_e::STOP )
    {
        // do nothing, just exit
    }
}

Process::flow_control_e Process::handle_action( const Action & action )
{
    typedef Process Type;

    typedef flow_control_e (Type::*PPMF)( const Action & r );

#define MAP_ENTRY(_v)       { typeid( _v ),        & Type::handle_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( SendSignal ),
        MAP_ENTRY( SetTimer ),
        MAP_ENTRY( ResetTimer ),
        MAP_ENTRY( FunctionCall ),
        MAP_ENTRY( Task ),
        MAP_ENTRY( Condition ),
        MAP_ENTRY( SwitchCondition ),
        MAP_ENTRY( NextState ),
        MAP_ENTRY( Exit ),
    };

#undef MAP_ENTRY

    auto it = funcs.find( typeid( action ) );

    if( it == funcs.end() )
    {
        dummy_logi_fatal( log_id_, id_, "unsupported action type %s", typeid( action ).name() );
        assert( 0 );
        throw SyntaxError( "unsupported action type " + std::string( typeid( action ).name() ) );
    }

    auto flow_control = (this->*it->second)( action );

    return flow_control;
}

Process::flow_control_e Process::handle_SendSignal( const Action & aa )
{
    auto & a = dynamic_cast< const SendSignal &>( aa );

    std::vector<Value> values;

    mem_.evaluate_expressions( & values, a.arguments );

    callback_->handle_send_signal( id_, a.name, values );

    return flow_control_e::NEXT;
}

Process::flow_control_e Process::handle_SetTimer( const Action & aa )
{
    auto & a = dynamic_cast< const SetTimer &>( aa );

    auto timer = find_timer( a.timer_id );

    if( timer == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find timer %u", a.timer_id );
        assert( 0 );
        throw SyntaxError( "cannot find timer " + std::to_string( a.timer_id ) );
    }

    Value delay;

    mem_.evaluate_expression( & delay, a.delay );

    set_timer( timer, delay );

    return flow_control_e::NEXT;
}

Process::flow_control_e Process::handle_ResetTimer( const Action & aa )
{
    auto & a = dynamic_cast< const ResetTimer &>( aa );

    auto timer = find_timer( a.timer_id );

    if( timer == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find timer %u", a.timer_id );
        assert( 0 );
        throw SyntaxError( "cannot find timer " + std::to_string( a.timer_id ) );
    }

    reset_timer( timer );

    return flow_control_e::NEXT;
}

Process::flow_control_e Process::handle_FunctionCall( const Action & aa )
{
    auto & a = dynamic_cast< const FunctionCall &>( aa );

    std::vector<Value> values;

    mem_.evaluate_expressions( & values, a.arguments );

    std::vector<Value*> value_pointers;

    convert_values_to_value_pointers( & value_pointers, values );

    callback_->handle_function_call( id_, a.name, value_pointers );

    dummy_logi_debug( log_id_, id_, "values: %s", StrHelper::to_string( values ).c_str() );

    mem_.import_values_into_variables( a.arguments, values );

    return flow_control_e::NEXT;
}

Process::flow_control_e Process::handle_Task( const Action & aa )
{
    auto & a = dynamic_cast< const Task &>( aa );

    auto variable = mem_.find_variable( a.variable_id );

    if( variable == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find variable %u", a.variable_id );
        assert( 0 );
        throw SyntaxError( "cannot find variable " + std::to_string( a.variable_id ) );
    }

    Value res;

    mem_.evaluate_expression( & res, a.expr );

    variable->assign( res );

    dummy_logi_debug( log_id_, id_, "task: %s (%i) = %s",
            variable->get_name().c_str(),
            variable->get_id(),
            anyvalue::StrHelper::to_string( res ).c_str() );

    return flow_control_e::NEXT;
}

Process::flow_control_e Process::handle_Condition( const Action & aa )
{
    auto & a = dynamic_cast< const Condition &>( aa );

    if( a.type == comparison_type_e::NOT )
    {
        // unary expression

        Value val;

        mem_.evaluate_expression( & val, a.lhs );

        auto b = ! val.arg_b;

        dummy_logi_debug( log_id_, id_, "condition ( %s %s ) evaluated to %s",
                anyvalue::StrHelper::to_string_short( a.type ).c_str(),
                anyvalue::StrHelper::to_string( val ).c_str(),
                b ? "TRUE" : "FALSE" );

        return b ? flow_control_e::NEXT : flow_control_e::ALT_NEXT;
    }

    Value lhs;
    mem_.evaluate_expression( & lhs, a.lhs );

    Value rhs;
    mem_.evaluate_expression( & rhs, a.rhs );

    auto b = anyvalue::compare_values( a.type, lhs, rhs );

    dummy_logi_debug( log_id_, id_, "condition ( %s %s %s ) evaluated to %s",
            anyvalue::StrHelper::to_string( lhs ).c_str(),
            anyvalue::StrHelper::to_string_short( a.type ).c_str(),
            anyvalue::StrHelper::to_string( rhs ).c_str(),
            b ? "TRUE" : "FALSE" );

    return b ? flow_control_e::NEXT : flow_control_e::ALT_NEXT;
}

Process::flow_control_e Process::handle_SwitchCondition( const Action & aa )
{
    auto & a = dynamic_cast< const SwitchCondition &>( aa );

    Value lhs;

    mem_.evaluate_expression( & lhs, a.var );

    int i = 0;

    for( auto & e : a.values )
    {
        ++i;

        Value rhs;

        mem_.evaluate_expression( & rhs, e );

        auto b = anyvalue::compare_values( comparison_type_e::EQ, lhs, rhs );

        if( b == true )
        {
            set_matched_switch_condition( i );

            dummy_logi_debug( log_id_, id_, "switch variable %s matched %s, executing case %u",
                    anyvalue::StrHelper::to_string( lhs ).c_str(),
                    anyvalue::StrHelper::to_string( rhs ).c_str(),
                    i );

            return flow_control_e::CHECK_SWITCH;
        }
    }

    set_matched_switch_condition( -1 );  // default

    dummy_logi_debug( log_id_, id_, "switch variable %s didn't match anything, executing default case",
            anyvalue::StrHelper::to_string( lhs ).c_str() );

    return flow_control_e::CHECK_SWITCH;
}

Process::flow_control_e Process::handle_NextState( const Action & aa )
{
    auto & a = dynamic_cast< const NextState &>( aa );

    auto state = find_state( a.state_id );

    if( state == nullptr )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find state %u", a.state_id );
        assert( 0 );
        throw SyntaxError( "cannot find state " + std::to_string( a.state_id ) );
    }

    next_state( a.state_id );

    return flow_control_e::STOP;
}
Process::flow_control_e Process::handle_Exit( const Action & /* aa */ )
{
    assert( internal_state_ == internal_state_e::ACTIVE );

    internal_state_ = internal_state_e::FINISHED;

    return flow_control_e::STOP;
}

void Process::next_state( element_id_t state )
{
    if( state == current_state_ )
    {
        dummy_logi_debug( log_id_, id_, "remained in state %s (%u)", names_.get_name( current_state_ ).c_str(), current_state_ );
    }
    else
    {
        dummy_logi_debug( log_id_, id_, "switched state %s (%u) --> %s (%u)", names_.get_name( current_state_ ).c_str(), current_state_, names_.get_name( state ).c_str(), state );
    }

    current_state_  = state;
}

element_id_t Process::get_next_id()
{
    return req_id_gen_.get_next_request_id();
}

} // namespace fsm
