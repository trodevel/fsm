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

// $Revision: 10640 $ $Date:: 2019-03-13 #$ $Author: serge $

#include "state.h"              // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_logi_debug

namespace fsm {

State::State( uint32_t log_id, element_id_t id, uint32_t process_id, const std::string & name, ISignalHandler * handler ):
        NamedElement( id, name ),
        log_id_( log_id ),
        process_id_( process_id ),
        handler_( handler )
{
    assert( id );
    assert( handler );
}

void State::add_signal_handler( const std::string & signal_name, element_id_t signal_handler_id )
{
    dummy_logi_trace( log_id_, process_id_, "add_signal_handler: id %u, %s", signal_handler_id, signal_name.c_str() );

    auto b = map_signal_name_to_signal_handler_ids_.insert( std::make_pair( signal_name, signal_handler_id ) ).second;

    if( b )
    {
        dummy_logi_debug( log_id_, process_id_, "added signal handler: state %s (%u), signal handler %u", name_.c_str(), id_, signal_handler_id );
    }
    else
    {
        dummy_logi_error( log_id_, process_id_, "signal handler already exists: state %s (%u), %u", name_.c_str(), id_, signal_handler_id );
        assert( b );
    }
}

void State::handle_signal( const std::string & signal_name, const std::vector<element_id_t> & arguments )
{
    auto it = map_signal_name_to_signal_handler_ids_.find( signal_name );

    if( it != map_signal_name_to_signal_handler_ids_.end() )
    {
        auto signal_handler_id = it->second;

        dummy_logi_debug( log_id_, process_id_, "handler_signal: state %s (%u), signal %s (%u)", name_.c_str(), id_, signal_name.c_str(), signal_handler_id );

        handler_->handle_signal_handler( signal_handler_id, arguments );
    }
    else
    {
        // signal not found

        dummy_logi_info( log_id_, process_id_, "handler_signal: state %s (%u), signal %s - not handled", name_.c_str(), id_, signal_name.c_str() );
    }
}

} // namespace fsm
