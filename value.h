/*

FSM objects.

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

// $Revision: 10572 $ $Date:: 2019-02-11 #$ $Author: serge $

#ifndef LIB_FSM__VALUE_H
#define LIB_FSM__VALUE_H

#include "elements.h"           // Element
#include "data_types.h"         // Int

namespace fsm {

struct Value
{
    Value();
    Value( bool b );
    Value( int i );
    Value( double d );
    Value( const char * s );
    Value( const std::string & s );

    data_type_e     type;

    Bool            arg_b;
    Int             arg_i;
    Double          arg_d;
    String          arg_s;
};

void harmonize( Value * v );

void assign( Value * lhs, const Value & rhs );

} // namespace fsm

#endif // LIB_FSM__VALUE_H
