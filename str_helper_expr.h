/*

FSM. StrHelper for Expressions.

Copyright (C) 2019 Sergey Kolevatov

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

// $Revision: 10440 $ $Date:: 2019-01-09 #$ $Author: serge $

#ifndef LIB_FSM__STR_HELPER_EXPR_H
#define LIB_FSM__STR_HELPER_EXPR_H

#include "memory.h"             // Memory

namespace fsm {

class StrHelperExpr
{
public:
    StrHelperExpr(
            Memory                  * memory );

    std::string to_string( ExpressionPtr expr ) const;
    std::string to_string( const Expression & expr ) const;

private:
    StrHelperExpr( const StrHelperExpr & )              = delete;
    StrHelperExpr & operator=( const StrHelperExpr & )  = delete;

    void convert_variable_to_value( element_id_t variable_id );

    std::string to_string_ExpressionValue( const Expression & expr );
    std::string to_string_ExpressionVariable( const Expression & expr );
    std::string to_string_ExpressionVariableName( const Expression & expr );
    std::string to_string_UnaryExpression( const Expression & expr );
    std::string to_string_BinaryExpression( const Expression & expr );

private:

    Memory                     * memory_;
};

} // namespace fsm

#endif // LIB_FSM__STR_HELPER_EXPR_H
