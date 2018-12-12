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

// $Revision: 10144 $ $Date:: 2018-12-12 #$ $Author: serge $

#include "fsm.h"                // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "utils/dummy_logger.h"     // dummy_log_debug
#include "scheduler/timeout_job_aux.h"      // create_and_insert_timeout_job

namespace fsm {

Fsm::Fsm(
        uint32_t                id,
        uint32_t                log_id,
        IFsm                    * parent,
        ICallback               * callback,
        scheduler::IScheduler   * scheduler,
        utils::IRequestIdGen    * req_id_gen ):
        id_( id ),
        log_id_( log_id ),
        parent_( parent ),
        callback_( callback ),
        scheduler_( scheduler ),
        req_id_gen_( req_id_gen ),
        id_ended_( false ),
        current_state_( 0 )

{
    dummy_logi_info( log_id_, id_, "created" );
}

Fsm::~Fsm()
{
    for( auto & e : map_id_to_state_ )
    {
        delete e.second;
    }

    dummy_logi_info( log_id_, id_, "destructed" );
}

void Fsm::handle_signal_handler( element_id_t signal_handler_id, const std::vector<element_id_t> & arguments )
{
    dummy_log_trace( log_id_, id_, "handle_signal_handler: signal handler id %u", signal_handler_id );

    auto it = map_id_to_signal_handler_.find( signal_handler_id );

    if( it == map_id_to_signal_handler_.end() )
    {
        dummy_log_fatal( log_id_, id_, "handle_signal_handler: cannot find signal handler id %u", signal_handler_id );
        throw std::runtime_error( "signal handler id " + std::to_string( signal_handler_id ) + " not found" );
        return;
    }

    auto & h = it->second;

    auto first_action_id = h->get_first_action_id();

    dummy_log_debug( log_id_, id_, "handle_signal_handler: signal handler id %u, first action id %u", signal_handler_id, first_action_id );

    if( first_action_id )
    {
        execute_action_connector_id( first_action_id );
    }
}

bool Fsm::is_ended() const
{
    return id_ended_;
}

element_id_t Fsm::create_state( const std::string & name )
{
    auto id = get_next_id();

    auto state = new State( log_id_, id, name, this );

    auto b = map_id_to_state_.insert( std::make_pair( id, state ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_state: %s (%u)", name.c_str(), id );

    add_name( id, name );

    return id;
}

element_id_t Fsm::create_add_signal_handler( element_id_t state_id, const std::string & signal_name )
{
    dummy_log_trace( log_id_, id_, "create_add_signal_handler: state id %u, signal name %s", state_id, signal_name.c_str() );

    auto it = map_id_to_state_.find( state_id );

    if( it == map_id_to_state_.end() )
    {
        dummy_log_fatal( log_id_, id_, "create_add_signal_handler: cannot find state id %u", state_id );
        throw std::runtime_error( "state id " + std::to_string( state_id ) + " not found" );
        return 0;
    }

    auto name = signal_name + " in " + it->second->get_name();

    auto id = create_signal_handler( name );

    it->second->add_signal_handler( signal_name, id );

    return id;
}

element_id_t Fsm::create_add_first_action_connector( element_id_t signal_handler_id, Action * action )
{
    dummy_log_trace( log_id_, id_, "create_add_first_action_connector: signal handler id %u", signal_handler_id );

    auto it = map_id_to_signal_handler_.find( signal_handler_id );

    if( it == map_id_to_signal_handler_.end() )
    {
        dummy_log_fatal( log_id_, id_, "create_add_first_action_connector: cannot find signal handler id %u", signal_handler_id );
        throw std::runtime_error( "signal handler id " + std::to_string( signal_handler_id ) + " not found" );
        return 0;
    }

    auto id = create_action_connector( action );

    it->second->set_first_action_id( id );

    return id;
}

element_id_t Fsm::create_add_next_action_connector( element_id_t action_connector_id, Action * action )
{
    dummy_log_trace( log_id_, id_, "create_add_next_action_connector: action_connector_id %u", action_connector_id );

    auto it = map_id_to_action_connector_.find( action_connector_id );

    if( it == map_id_to_action_connector_.end() )
    {
        dummy_log_fatal( log_id_, id_, "create_add_next_action_connector: cannot find action_connector_id %u", action_connector_id );
        assert( 0 );
        throw std::runtime_error( "signal handler id " + std::to_string( action_connector_id ) + " not found" );
        return 0;
    }

    auto id = create_action_connector( action );

    it->second->set_next_id( id );

    return id;
}

element_id_t Fsm::create_add_timer( const std::string & name )
{
    auto id = get_next_id();

    auto timer = new Timer( log_id_, id, name );

    auto b = map_id_to_timer_.insert( std::make_pair( id, timer ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_timer: created timer %s (%u)", name.c_str(), id );

    add_name( id, name );

    return id;
}

element_id_t Fsm::create_signal_handler( const std::string & name )
{
    auto id = get_next_id();

    auto signal_handler = new SignalHandler( log_id_, id, name );

    auto b = map_id_to_signal_handler_.insert( std::make_pair( id, signal_handler ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_signal_handler: created signal handler %s (%u)", name.c_str(), id );

    add_name( id, name );

    return id;
}

element_id_t Fsm::create_action_connector( Action * action )
{
    auto id = get_next_id();

    auto obj = new ActionConnector( log_id_, id, action );

    auto b = map_id_to_action_connector_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_action_connector: created action connector %u", id );

    return id;
}

element_id_t Fsm::create_add_variable( const std::string & name, data_type_e type )
{
    Value dummy;

    dummy.type  = type;

    return create_add_variable( name, type, dummy );
}

element_id_t Fsm::create_add_variable( const std::string & name, data_type_e type, const Value & value )
{
    auto id = get_next_id();

    auto obj = new Variable( log_id_, id, name, type, value );

    auto b = map_id_to_variable_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_variable: created variable %u", id );

    add_name( id, name );

    return id;
}

element_id_t Fsm::create_add_constant( const std::string & name, data_type_e type, const Value & value )
{
    auto id = get_next_id();

    auto obj = new Constant( log_id_, id, name, type, value );

    auto b = map_id_to_constant_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_constant: created constant %u", id );

    add_name( id, name );

    return id;
}

void Fsm::set_initial_state( element_id_t state_id )
{
    dummy_log_trace( log_id_, id_, "set_initial_state: %u", state_id );

    assert( current_state_ == 0 );

    current_state_  = state_id;
}

void Fsm::handle( const Signal * req )
{
    dummy_log_trace( log_id_, id_, "handle: %s", typeid( *req ).name() );

    auto it = map_id_to_state_.find( current_state_ );

    assert( it != map_id_to_state_.end() );

    auto state = it->second;

    std::vector<element_id_t> arguments;

    init_temp_variables_from_signal( * req, & arguments );

    state->handle_signal( req->name, arguments );

    delete req;
}

void Fsm::add_name( element_id_t id, const std::string & name )
{
    map_id_to_name_.insert( std::make_pair( id, name ) );

    auto p = map_name_to_id_.insert( std::make_pair( name, id ) );

    if( p.second == false )
    {
        dummy_log_fatal( log_id_, id_, "add_name: cannot add element %s (%u), another element exists %s (%u)", name.c_str(), id, name.c_str(), p.first->second );
        assert( 0 );
        throw std::runtime_error( "cannot add element " + name + " " + std::to_string( id ) );
    }
}

const std::string & Fsm::get_name( element_id_t id )
{
    static const std::string unk( "?" );

    auto it = map_id_to_name_.find( id );

    if( it != map_id_to_name_.end() )
        return it->second;

    return unk;
}

element_id_t Fsm::find_element( const std::string & name ) const
{
    auto it = map_name_to_id_.find( name );

    if( it == map_id_to_name_.end() )
        return 0;

    return it->second;
}

bool Fsm::delete_name( element_id_t id )
{
    auto it = map_id_to_name_.find( id );

    if( it == map_id_to_name_.end() )
        return false;

    auto & name = it->second;

    map_name_to_id_.erase( name );

    map_id_to_name_.erase( it );

    return true;
}

void Fsm::clear_temp_variables()
{
    for( auto e : map_id_to_temp_variable_ )
    {
        auto id = e.first;
        auto b = delete_name( id );

        assert( b ); (void)b;
    }

    dummy_logi_debug( log_id_, id_, "clear_temp_variables: %u variables deleted", map_id_to_temp_variable_.size() );

    map_id_to_temp_variable_.clear();
}

void Fsm::init_temp_variables_from_signal( const Signal & s, std::vector<element_id_t> * arguments )
{
    clear_temp_variables();

    auto i = 0;

    for( auto v : s.arguments )
    {
        ++i;

        auto id = create_temp_variable( v, i );

        arguments->push_back( id );
    }

    dummy_logi_debug( log_id_, id_, "created %u temp variables", i );
}

element_id_t Fsm::create_temp_variable( const Value & v, unsigned n )
{
    auto id = get_next_id();

    auto name = "$" + std::to_string( n );

    auto obj = new Variable( log_id_, id, name, v.type, v );

    auto b = map_id_to_temp_variable_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_temp_variable: created variable %u", id );

    add_name( id, name );

    return id;
}

Variable* Fsm::find_variable( element_id_t id )
{
    {
        auto it = map_id_to_variable_.find( id );

        if( it != map_id_to_variable_.end() )
        {
            return it->second;
        }
    }

    {
        auto it = map_id_to_temp_variable_.find( id );

        if( it != map_id_to_temp_variable_.end() )
        {
            return it->second;
        }
    }

    return nullptr;
}

void Fsm::convert_arguments_to_values( std::vector<Value> * values, const std::vector<Argument> & arguments )
{
    dummy_log_trace( log_id_, id_, "convert_arguments_to_values: convert %u arguments", arguments.size() );

    for( auto & e : arguments )
    {
        Value v;

        convert_argument_to_value( & v, e );

        values->push_back( v );
    }
}

void Fsm::convert_argument_to_value( Value * value, const Argument & argument )
{
    if( argument.type == argument_type_e::VALUE )
    {
        assign( value, argument.value );
    }
    else // if( argument.type == argument_type_e::VARIABLE_IN ) // or VARIABLE_OUT
    {
        if( argument.variable_id != 0 && argument.variable_name.empty() )
        {
            convert_variable_to_value( value, argument.variable_id );
        }
        else if( argument.variable_id == 0 && argument.variable_name.empty() == false )
        {
            auto variable_id = find_element( argument.variable_name );

            assert( variable_id );

            convert_variable_to_value( value, argument.variable_id );
        }
        else
        {
            dummy_log_fatal( log_id_, id_, "convert_argument_to_value: illegal combination: variable_id %u, variable_name '%s'", argument.variable_id, argument.variable_name.c_str() );
            assert( 0 );
            throw std::runtime_error( "convert_argument_to_value: illegal combination" );
        }
    }
}

void Fsm::convert_variable_to_value( Value * value, element_id_t variable_id )
{
    {
        auto it = map_id_to_variable_.find( variable_id );

        if( it != map_id_to_variable_.end() )
        {
            assign( value, it->second->get() );
            return;
        }
    }

    {
        auto it = map_id_to_temp_variable_.find( variable_id );

        if( it != map_id_to_temp_variable_.end() )
        {
            assign( value, it->second->get() );
            return;
        }
    }

    {
        auto it = map_id_to_constant_.find( variable_id );

        if( it != map_id_to_constant_.end() )
        {
            assign( value, it->second->get() );
            return;
        }
    }

    dummy_log_fatal( log_id_, id_, "convert_variable_to_value: variable_id %u not found in the list of variables, temp variables, and constants", variable_id );
    assert( 0 );
    throw std::runtime_error( "convert_variable_to_value: variable_id " + std::to_string( variable_id ) + " not found in the list of variables, temp variables, and constants" );
}

void Fsm::convert_values_to_value_pointers( std::vector<Value*> * value_pointers, std::vector<Value> & values )
{
    for( auto & e : values )
    {
        auto * p = & e;

        value_pointers->push_back( p );
    }
}

void Fsm::import_values_into_arguments( const std::vector<Argument> & arguments, const std::vector<Value> & values )
{
    dummy_log_trace( log_id_, id_, "import_values_into_arguments: %u arguments", arguments.size() );

    assert( arguments.size() == values.size() );

    unsigned i = 0;
    unsigned imported = 0;

    for( auto & e : arguments )
    {
        // ignore all variables except output variables
        if( e.type != argument_type_e::VARIABLE_OUT )
        {
            ++i;
            continue;
        }

        ++imported;
    }

    dummy_log_trace( log_id_, id_, "import_values_into_arguments: imported %u values", imported );
}

void Fsm::set_timer( Timer * timer, const Value & delay )
{
    dummy_log_trace( log_id_, id_, "set_timer: timer %s (%u), %.2f sec", timer->get_name().c_str(), timer->get_id(), delay.arg_d );

    auto d = delay.arg_d;

    auto & name = timer->get_name();

    std::vector<Argument> dummy;

    auto signal = new Signal( id_, name, dummy );

    std::string error_msg;

    scheduler::job_id_t sched_job_id;

    auto b = scheduler::create_and_insert_timeout_job(
            & sched_job_id,
            & error_msg,
            * scheduler_,
            "timer_job",
            scheduler::Duration( delay.arg_d ),
            std::bind( static_cast<void (IFsm::*)(const Signal * )>(&IFsm::consume), parent_, signal ) );

    if( b == false )
    {
        dummy_logi_error( log_id_, id_, "cannot set timer: %s", error_msg.c_str() );

        timer->set_job_id( 0 );

        delete signal;
    }
    else
    {
        dummy_logi_debug( log_id_, id_, "timer %s, scheduled execution in: %.2f sec", name.c_str(), delay.arg_d );

        timer->set_job_id( sched_job_id );
    }

}

void Fsm::reset_timer( Timer * timer )
{
    dummy_log_trace( log_id_, id_, "reset_timer: timer %s (%u)", timer->get_name().c_str(), timer->get_id() );

    auto & name = timer->get_name();

    auto sched_job_id = timer->get_job_id();

    if( sched_job_id == 0 )
    {
        dummy_log_trace( log_id_, id_, "reset_timer: job id is 0" );

        return;
    }

    std::string error_msg;

    auto b = scheduler_->delete_job( & error_msg, sched_job_id );

    dummy_log_trace( log_id_, id_, "reset_timer: job id %u", sched_job_id );

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

void Fsm::execute_action_connector_id( element_id_t action_connector_id )
{
    dummy_log_trace( log_id_, id_, "execute_action_connector_id: action_connector_id %u", action_connector_id );

    auto it = map_id_to_action_connector_.find( action_connector_id );

    if( it == map_id_to_action_connector_.end() )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find action connector %u", action_connector_id );
        assert( 0 );
        throw std::runtime_error( "cannot find action connector " + std::to_string( action_connector_id ) );
    }

    auto action_connector = it->second;

    execute_action_connector( * action_connector );
}

void Fsm::execute_action_connector( const ActionConnector & action_connector )
{
    auto & action = * action_connector.action_;

    auto flow_control = handle_action( action );

    if( flow_control == flow_control_e::NEXT )
    {
        execute_action_connector_id( action_connector.next_id_ );
    }
    else if( flow_control == flow_control_e::ALT_NEXT )
    {
        execute_action_connector_id( action_connector.alt_next_id_ );
    }
    else // if( flow_control == flow_control_e::STOP )
    {
        // do nothing, just exit
    }
}

Fsm::flow_control_e Fsm::handle_action( const Action & action )
{
    typedef Fsm Type;

    typedef void (Type::*PPMF)( const Action & r );

#define MAP_ENTRY(_v)       { typeid( _v ),        & Type::handle_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( SendSignal ),
        MAP_ENTRY( SetTimer ),
        MAP_ENTRY( ResetTimer ),
        MAP_ENTRY( FunctionCall ),
        MAP_ENTRY( If ),
        MAP_ENTRY( NextState ),
        MAP_ENTRY( Exit ),
    };

#undef MAP_ENTRY

    auto it = funcs.find( typeid( action ) );

    if( it == funcs.end() )
    {
        dummy_logi_fatal( log_id_, id_, "unsupported action type %s", typeid( action ).name() );
        assert( 0 );
        throw std::runtime_error( "unsupported action type " + std::string( typeid( action ).name() ) );
    }

    auto handler = this->*it->second;

    auto flow_control = handler( action );

    return flow_control;
}

Fsm::flow_control_e Fsm::handle_SendSignal( const Action & aa )
{
    auto & a = dynamic_cast< const SendSignal &>( aa );

    std::vector<Value> values;

    convert_arguments_to_values( & values, a.arguments );

    callback_->handle_send_signal( id_, a.name, values );

    return flow_control_e::NEXT;
}

Fsm::flow_control_e Fsm::handle_SetTimer( const Action & aa )
{
    auto & a = dynamic_cast< const SetTimer &>( aa );

    auto it = map_id_to_timer_.find( a.timer_id );

    if( it == map_id_to_timer_.end() )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find timer %u", a.timer_id );
        assert( 0 );
        throw std::runtime_error( "cannot find timer " + std::to_string( a.timer_id ) );
    }

    Value delay;

    convert_argument_to_value( & delay, a.delay );

    set_timer( it->second, delay );

    return flow_control_e::NEXT;
}

Fsm::flow_control_e Fsm::handle_ResetTimer( const Action & aa )
{
    auto & a = dynamic_cast< const ResetTimer &>( aa );

    auto it = map_id_to_timer_.find( a.timer_id );

    if( it == map_id_to_timer_.end() )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find timer %u", a.timer_id );
        assert( 0 );
        throw std::runtime_error( "cannot find timer " + std::to_string( a.timer_id ) );
    }

    reset_timer( it->second );

    return flow_control_e::NEXT;
}

Fsm::flow_control_e Fsm::handle_FunctionCall( const Action & aa )
{
    auto & a = dynamic_cast< const FunctionCall &>( aa );

    std::vector<Value> values;

    convert_arguments_to_values( & values, a.arguments );

    std::vector<Value*> value_pointers;

    convert_values_to_value_pointers( & value_pointers, values );

    callback_->handle_function_call( id_, a.name, value_pointers );

    import_values_into_arguments( a.arguments, values );

    return flow_control_e::NEXT;
}

Fsm::flow_control_e Fsm::handle_If( const Action & aa )
{
    auto & a = dynamic_cast< const If &>( aa );

    return flow_control_e::NEXT;
}
Fsm::flow_control_e Fsm::handle_NextState( const Action & aa )
{
    auto & a = dynamic_cast< const NextState &>( aa );

    auto it = map_id_to_timer_.find( a.state_id );

    if( it == map_id_to_timer_.end() )
    {
        dummy_logi_fatal( log_id_, id_, "cannot find state %u", a.state_id );
        assert( 0 );
        throw std::runtime_error( "cannot find state " + std::to_string( a.state_id ) );
    }

    next_state( a.state_id );

    return flow_control_e::STOP;
}
Fsm::flow_control_e Fsm::handle_Exit( const Action & aa )
{
    auto & a = dynamic_cast< const Exit &>( aa );

    return flow_control_e::STOP;
}

void Fsm::next_state( element_id_t state )
{
    dummy_logi_debug( log_id_, id_, "switched state %s (%u) --> %s (%u)", get_name( current_state_ ).c_str(), current_state_, get_name( state ).c_str(), state );

    current_state_  = state;
}

element_id_t Fsm::get_next_id()
{
    return req_id_gen_->get_next_request_id();
}

} // namespace fsm
