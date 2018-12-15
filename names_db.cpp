/*

FSM. Names DB.

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

// $Revision: 10198 $ $Date:: 2018-12-15 #$ $Author: serge $

#include "names_db.h"           // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

NamesDb::NamesDb(
        uint32_t                id,
        uint32_t                log_id ):
        id_( id ),
        log_id_( log_id )
{
//    dummy_logi_info( log_id_, id_, "created" );
}

NamesDb::~NamesDb()
{
//    dummy_logi_info( log_id_, id_, "destructed" );
}

void NamesDb::add_name( element_id_t id, const std::string & name )
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

const std::string & NamesDb::get_name( element_id_t id )
{
    static const std::string unk( "?" );

    auto it = map_id_to_name_.find( id );

    if( it != map_id_to_name_.end() )
        return it->second;

    return unk;
}

element_id_t NamesDb::find_element( const std::string & name ) const
{
    auto it = map_name_to_id_.find( name );

    if( it == map_name_to_id_.end() )
        return 0;

    return it->second;
}

bool NamesDb::delete_name( element_id_t id )
{
    auto it = map_id_to_name_.find( id );

    if( it == map_id_to_name_.end() )
        return false;

    auto & name = it->second;

    map_name_to_id_.erase( name );

    map_id_to_name_.erase( it );

    return true;
}

} // namespace fsm
