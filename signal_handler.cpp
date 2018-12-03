/*

FSM signal handler.

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

// $Revision: 10017 $ $Date:: 2018-12-03 #$ $Author: serge $

#include "signal_handler.h"         // self

#include <cassert>                  // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

SignalHandler::SignalHandler( uint32_t log_id, element_id_t id, const std::string & name ):
        log_id_( log_id ),
        id_( id ),
        name_( name ),
        first_action_id_( 0 )
{
    assert( id );
}

void SignalHandler::set_first_action_id( element_id_t id )
{
    dummy_log_trace( log_id_, "set_first_action_id: id %u", id );

    assert( id );

    first_action_id_    = id;

    dummy_log_trace( log_id_, "set_first_action_id: set first action connector id %u, for signal handler %s (%u)", id, name_.c_str(), id_ );
}

element_id_t SignalHandler::get_first_action_id() const
{
    return first_action_id_;
}

const std::string & SignalHandler::get_name() const
{
    return name_;
}

} // namespace fsm
