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

// $Revision: 10020 $ $Date:: 2018-12-03 #$ $Author: serge $

#include "fsm.h"                // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

Fsm::Fsm( uint32_t log_id ):
        log_id_( log_id )
{
    req_id_gen_.init( 1, 1 );
}

Fsm::~Fsm()
{
    for( auto & e : map_id_to_state_ )
    {
        delete e.second;
    }
}

void Fsm::handle_signal_handler( element_id_t signal_handler_id, const std::vector<Argument> & arguments )
{
    dummy_log_trace( log_id_, "handle_signal_handler: signal handler id %u", signal_handler_id );

    auto it = map_id_to_signal_handler_.find( signal_handler_id );

    if( it == map_id_to_signal_handler_.end() )
    {
        dummy_log_fatal( log_id_, "handle_signal_handler: cannot find signal handler id %u", signal_handler_id );
        throw std::runtime_error( "signal handler id " + std::to_string( signal_handler_id ) + " not found" );
        return;
    }

    auto & h = it->second;

    auto first_action_id = h->get_first_action_id();

    dummy_log_debug( log_id_, "handle_signal_handler: signal handler id %u, first action id %u", signal_handler_id, first_action_id );

    if( first_action_id )
    {
        execute_action_flow( first_action_id );
    }
}

element_id_t Fsm::create_state( const std::string & name )
{
    auto id = get_next_id();

    auto state = new State( log_id_, id, name, this );

    auto b = map_id_to_state_.insert( std::make_pair( id, state ) ).second;

    assert( b );

    dummy_log_debug( log_id_, "create_state: %s (%u)", name.c_str(), id );

    add_name( id, name );

    return id;
}

element_id_t Fsm::create_add_signal_handler( element_id_t state_id, const std::string & signal_name )
{
    dummy_log_trace( log_id_, "create_add_signal_handler: state id %u, signal name %s", state_id, signal_name.c_str() );

    auto it = map_id_to_state_.find( state_id );

    if( it == map_id_to_state_.end() )
    {
        dummy_log_fatal( log_id_, "create_add_signal_handler: cannot find state id %u", state_id );
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
    dummy_log_trace( log_id_, "create_add_first_action_connector: signal handler id %u", signal_handler_id );

    auto it = map_id_to_signal_handler_.find( signal_handler_id );

    if( it == map_id_to_signal_handler_.end() )
    {
        dummy_log_fatal( log_id_, "create_add_first_action_connector: cannot find signal handler id %u", signal_handler_id );
        throw std::runtime_error( "signal handler id " + std::to_string( signal_handler_id ) + " not found" );
        return 0;
    }

    auto id = create_action_connector( action );

    it->second->set_first_action_id( id );

    return id;
}

element_id_t Fsm::create_add_next_action_connector( element_id_t action_connector_id, Action * action )
{
    dummy_log_trace( log_id_, "create_add_next_action_connector: action_connector_id %u", action_connector_id );

    auto it = map_id_to_action_connector_.find( action_connector_id );

    if( it == map_id_to_action_connector_.end() )
    {
        dummy_log_fatal( log_id_, "create_add_next_action_connector: cannot find action_connector_id %u", action_connector_id );
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

    dummy_log_debug( log_id_, "create_signal_handler: created signal handler %s (%u)", name.c_str(), id );

    add_name( id, name );

    return id;
}

element_id_t Fsm::create_action_connector( Action * action )
{
    auto id = get_next_id();

    auto obj = new ActionConnector( log_id_, id, action );

    auto b = map_id_to_action_connector_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, "create_action_connector: created action connector %u", id );

    return id;
}

void Fsm::add_name( element_id_t id, const std::string & name )
{
    map_id_to_name_.insert( std::make_pair( id, name ) );
}

const std::string & Fsm::find_name( element_id_t id )
{
    static const std::string unk( "?" );

    auto it = map_id_to_name_.find( id );

    if( it != map_id_to_name_.end() )
        return it->second;

    return unk;
}

element_id_t Fsm::get_next_id()
{
    return req_id_gen_.get_next_request_id();
}

} // namespace fsm
