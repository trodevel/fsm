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

// $Revision: 11611 $ $Date:: 2019-05-24 #$ $Author: serge $

#include "variable.h"           // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

Variable::Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type ):
        NamedElement( id, name ),
        log_id_( log_id ),
        type_( type ),
        is_inited_( false )
{
    assert( id );

    value_.type = type;
}

Variable::Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type, const Value & value ):
        NamedElement( id, name ),
        log_id_( log_id ),
        type_( type ),
        is_inited_( true )
{
    assert( id );

    value_.type = type;

    anyvalue::assign( & value_, value );
}

data_type_e Variable::get_type() const
{
    return type_;
}

bool Variable::is_inited() const
{
    return is_inited_;
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
    anyvalue::assign( & value_, v );
}
} // namespace fsm
