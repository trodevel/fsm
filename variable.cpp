/*

FSM Variable.

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

// $Revision: 10242 $ $Date:: 2018-12-17 #$ $Author: serge $

#include "variable.h"           // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

Variable::Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type ):
        log_id_( log_id ),
        id_( id ),
        name_( name ),
        type_( type )
{
    assert( id );

    value_.type = type;
}

Variable::Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type, const Value & value ):
        log_id_( log_id ),
        id_( id ),
        name_( name ),
        type_( type )
{
    assert( id );

    value_.type = type;

    fsm::assign( & value_, value );
}

const Value & Variable::get() const
{
    return value_;
}

void Variable::set( const Value & v )
{
    value_  = v;
}

void Variable::assign( const Value & v )
{
    fsm::assign( & value_, v );
}
} // namespace fsm
