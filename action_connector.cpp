/*

FSM action connectors.

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

// $Revision: 10404 $ $Date:: 2019-01-05 #$ $Author: serge $

#include "action_connector.h"       // self

#include <cassert>              // assert

namespace fsm {

ActionConnector::ActionConnector( uint32_t log_id, element_id_t id, Action * action ):
        id_( id ),
        next_id_( 0 ),
        alt_next_id_( 0 ),
        default_switch_action_( 0 ),
        action_( action )
{
}

void ActionConnector::set_next_id( element_id_t id )
{
    assert( switch_actions_.empty() );

    next_id_    = id;
}

void ActionConnector::set_alt_next_id( element_id_t id )
{
    assert( switch_actions_.empty() );

    alt_next_id_    = id;
}

void ActionConnector::set_default_switch_action( element_id_t id )
{
    assert( next_id_ == 0 );
    assert( alt_next_id_ == 0 );

    default_switch_action_  = id ;
}

void ActionConnector::add_switch_action( element_id_t id )
{
    assert( next_id_ == 0 );
    assert( alt_next_id_ == 0 );

    switch_actions_.push_back( id );
}

element_id_t ActionConnector::get_id() const
{
    return id_;
}

element_id_t ActionConnector::get_next_id() const
{
    return next_id_;
}

element_id_t ActionConnector::get_alt_next_id() const
{
    return alt_next_id_;
}

element_id_t ActionConnector::get_switch_action( int switch_case_num ) const
{
    assert( ( switch_case_num == -1 ) || ( ( switch_case_num > 0 ) && ( unsigned( switch_case_num ) <= switch_actions_.size() ) ) );

    if( switch_case_num == -1 )
        return default_switch_action_;

    return switch_actions_.at( switch_case_num - 1 );
}

const Action* ActionConnector::get_action() const
{
    return action_.get();
}

} // namespace fsm
