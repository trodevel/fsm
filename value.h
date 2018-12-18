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

// $Revision: 10258 $ $Date:: 2018-12-18 #$ $Author: serge $

#ifndef LIB_FSM__VALUE_H
#define LIB_FSM__VALUE_H

#include "elements.h"           // Element

namespace fsm {

struct Value
{
    Value();
    Value( bool b );
    Value( int i );
    Value( double d );
    Value( const std::string & s );

    data_type_e     type;

    bool            arg_b;
    int             arg_i;
    double          arg_d;
    std::string     arg_s;
};

void harmonize( Value * v );

void assign( Value * lhs, const Value & rhs );

} // namespace fsm

#endif // LIB_FSM__VALUE_H
