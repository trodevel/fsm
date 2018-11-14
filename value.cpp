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

// $Revision: 9978 $ $Date:: 2018-11-14 #$ $Author: serge $

#include "value.h"          // self

#include <limits>           // quiet_NaN()

namespace fsm {

void harmonize( Value * v )
{
    switch( v->type )
    {
    case data_type_e::BOOL:
    {
        v->arg_i    = (int)v->arg_b;
        v->arg_d    = (double)v->arg_b;
        v->arg_s.clear();
    }
    break;

    case data_type_e::INT:
    {
        v->arg_b    = (int)v->arg_i;
        v->arg_d    = (double)v->arg_i;
        v->arg_s.clear();
    }
    break;

    case data_type_e::DOUBLE:
    {
        v->arg_b    = (double)v->arg_d;
        v->arg_i    = (int)v->arg_d;
        v->arg_s.clear();
    }
    break;

    case data_type_e::STRING:
    {
        v->arg_b    = false;
        v->arg_i    = 0;
        v->arg_d    = std::numeric_limits<double>::quiet_NaN();
    }
    break;

    default:
        break;
    }
}

void assign( Value * lhs, const Value & rhs )
{
    switch( lhs->type )
    {
    case data_type_e::BOOL:
    {
        lhs->arg_b  = rhs.arg_b;
    }
    break;

    case data_type_e::INT:
    {
        lhs->arg_i  = rhs.arg_i;
    }
    break;

    case data_type_e::DOUBLE:
    {
        lhs->arg_d  = rhs.arg_d;
    }
    break;

    case data_type_e::STRING:
    {
        lhs->arg_s  = rhs.arg_s;
    }
    break;

    default:
        break;
    }

    harmonize( lhs );
}

} // namespace fsm
