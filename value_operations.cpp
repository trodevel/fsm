/*

FSM. Value Operations.

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

// $Revision: 10191 $ $Date:: 2018-12-14 #$ $Author: serge $

#include "value_operations.h"   // self

#include <cassert>              // assert
#include <stdexcept>            // std::runtime_error

namespace fsm {

template <class T>
bool compare_values_t( comparison_type_e type, const T & lhs, const T & rhs )
{
    switch( type )
    {
    case comparison_type_e::EQ:
        return lhs == rhs;

    case comparison_type_e::NEQ:
        return lhs != rhs;

    case comparison_type_e::LT:
        return lhs < rhs;

    case comparison_type_e::LE:
        return lhs <= rhs;

    case comparison_type_e::GT:
        return lhs > rhs;

    case comparison_type_e::GE:
        return lhs >= rhs;

    default:
        break;
    }

    //dummy_logi_fatal( log_id_, id_, "illegal comparison type %u", unsigned( type ) );
    assert( 0 );
    throw std::runtime_error( "illegal comparison type  " + std::to_string( unsigned( type ) ) );

    return false;
}


bool compare_values( comparison_type_e type, const Value & lhs, const Value & rhs )
{
    switch( lhs.type )
    {
    case data_type_e::BOOL:
        return compare_values_t( type, lhs.arg_b, rhs.arg_b );

    case data_type_e::INT:
        return compare_values_t( type, lhs.arg_i, rhs.arg_i );

    case data_type_e::DOUBLE:
        return compare_values_t( type, lhs.arg_d, rhs.arg_d );

    case data_type_e::STRING:
        return compare_values_t( type, lhs.arg_s, rhs.arg_s );

    default:
        break;
    }

    //dummy_logi_fatal( log_id_, id_, "illegal data type %u", unsigned( lhs.type ) );
    assert( 0 );
    throw std::runtime_error( "illegal data type  " + std::to_string( unsigned( lhs.type ) ) );

    return false;
}

void unary_operation( Value * res, unary_operation_type_e type, const Value & op )
{
}

void binary_operation( Value * res, binary_operation_type_e type, const Value & lhs, const Value & rhs )
{
}

} // namespace fsm
