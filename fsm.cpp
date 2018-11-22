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

// $Revision: 9996 $ $Date:: 2018-11-20 #$ $Author: serge $

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

void Fsm::handle_signal( element_id_t signal_id, const std::vector<Argument> & arguments )
{
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
