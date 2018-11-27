/*

FSM state.

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

// $Revision: 10013 $ $Date:: 2018-11-26 #$ $Author: serge $

#include "state.h"              // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

State::State( uint32_t log_id, element_id_t id, const std::string & name, ISignalHandler * handler ):
        log_id_( log_id ),
        id_( id ),
        name_( name ),
        handler_( handler )
{
    assert( id );
    assert( handler );
}

void State::add_signal_handler( const std::string & signal_name, element_id_t signal_handler_id )
{
    dummy_log_trace( log_id_, "add_signal_handler: id %u, %s", signal_handler_id );

    auto b = map_signal_name_to_signal_handler_ids_.insert( std::make_pair( signal_name, signal_handler_id ) ).second;

    if( b )
    {
        dummy_log_debug( log_id_, "added signal handler: state %s (%u), signal %u", id_, name_.c_str(), signal_handler_id );
    }
    else
    {
        dummy_log_error( log_id_, "signal handler already exists: state %s (%u), %u", id_, name_.c_str(), signal_handler_id );
        assert( b );
    }
}

void State::handle_signal( element_id_t signal_id, const std::vector<Argument> & arguments )
{
    auto it = map_signal_name_to_signal_handler_ids_.find( signal_id );

    if( it != map_signal_name_to_signal_handler_ids_.end() )
    {
        dummy_log_debug( log_id_, "handler_signal: state %s (%u), signal %u", id_, name_.c_str(), signal_id );

        handler_->handle_signal( signal_id, arguments );
    }
    else
    {
        // signal not found

        dummy_log_info( log_id_, "handler_signal: state %s (%u), signal %u - not handled", id_, name_.c_str(), signal_id );
    }
}

} // namespace fsm
