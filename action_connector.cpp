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

// $Revision: 10010 $ $Date:: 2018-11-23 #$ $Author: serge $

#include "action_connector.h"       // self

namespace fsm {

ActionConnector::ActionConnector( uint32_t log_id, element_id_t id, Action * action ):
        id_( id ),
        next_id_( 0 ),
        alt_next_id_( 0 ),
        action_( action )
{
}

void ActionConnector::set_next_id( element_id_t id )
{
    next_id_    = id;
}

void ActionConnector::set_alt_next_id( element_id_t id )
{
    alt_next_id_    = id;
}

element_id_t ActionConnector::get_next_id( element_id_t id ) const
{
    return next_id_;
}

element_id_t ActionConnector::get_alt_next_id( element_id_t id ) const
{
    return alt_next_id_;
}

} // namespace fsm
