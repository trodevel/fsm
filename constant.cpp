/*

FSM constant.

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

// $Revision: 10369 $ $Date:: 2018-12-31 #$ $Author: serge $

#include "constant.h"           // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

Constant::Constant( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type, const Value & value ):
        NamedElement( id, name ),
        log_id_( log_id ),
        type_( type )
{
    assert( id );

    value_.type = type;

    assign( & value_, value );
}

const Value & Constant::get() const
{
    return value_;
}

} // namespace fsm
