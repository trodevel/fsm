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

// $Revision: 10119 $ $Date:: 2018-12-10 #$ $Author: serge $

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

void Fsm::schedule_signal( const Signal * s, double duration )
{
    dummy_logi_trace( log_id_, id_, "schedule_signal: %.2f sec, %s", duration, s->name.c_str() );

    std::string error_msg;

    scheduler::job_id_t sched_job_id;

    auto b = scheduler::create_and_insert_timeout_job(
            & sched_job_id,
            & error_msg,
            * scheduler_,
            "timer_job",
            scheduler::Duration( duration ),
            std::bind( static_cast<void (IFsm::*)(const Signal * )>(&IFsm::consume), parent_, s ) );

    if( b == false )
    {
        dummy_logi_error( log_id_, id_, "cannot set timer: %s", error_msg.c_str() );
    }
    else
    {
        dummy_logi_debug( log_id_, id_, "scheduled execution in: %.2f sec", duration );
    }
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

void Fsm::convert_arguments_to_values( std::vector<Value> * values, const std::vector<Argument> & arguments )
{
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

    return flow_control_e::NEXT;
}
Fsm::flow_control_e Fsm::handle_FunctionCall( const Action & aa )
{
    auto & a = dynamic_cast< const FunctionCall &>( aa );

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

    return flow_control_e::STOP;
}
Fsm::flow_control_e Fsm::handle_Exit( const Action & aa )
{
    auto & a = dynamic_cast< const Exit &>( aa );

    return flow_control_e::STOP;
}

element_id_t Fsm::get_next_id()
{
    return req_id_gen_->get_next_request_id();
}

} // namespace fsm
