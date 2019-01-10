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

// $Revision: 10444 $ $Date:: 2019-01-09 #$ $Author: serge $

#include "str_helper_expr.h"    // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "value_operations.h"       // compare_values
#include "syntax_error.h"           // SyntaxError
#include "str_helper.h"             // StrHelper

namespace fsm {

StrHelperExpr::StrHelperExpr(
        Memory                  * memory ):
        memory_( memory )
{
}

std::string StrHelperExpr::to_string( ExpressionPtr expr ) const
{
    return to_string( * expr.get() );
}

std::string StrHelperExpr::to_string( const Expression & expr ) const
{
    typedef StrHelperExpr Type;

    typedef void (Type::*PPMF)( Value *, const Expression & );

#define MAP_ENTRY(_v)       { typeid( _v ),        & Type::to_string_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( ExpressionValue ),
        MAP_ENTRY( ExpressionVariable ),
        MAP_ENTRY( ExpressionVariableName ),
        MAP_ENTRY( UnaryExpression ),
        MAP_ENTRY( BinaryExpression ),
    };

#undef MAP_ENTRY

    auto it = funcs.find( typeid( expr ) );

    if( it == funcs.end() )
    {
        //dummy_logi_fatal( log_id_, id_, "unsupported expression type %s", typeid( expr ).name() );
        assert( 0 );
        throw SyntaxError( "unsupported expression type " + std::string( typeid( expr ).name() ) );
    }

    return (this->*it->second)( expr );
}

std::string StrHelperExpr::to_string_ExpressionValue( const Expression & eexpr )
{
    auto & a = dynamic_cast< const ExpressionValue&>( eexpr );

    return StrHelper::to_string( a.value );
}

std::string StrHelperExpr::to_string_ExpressionVariable( const Expression & eexpr )
{
    auto & a = dynamic_cast< const ExpressionVariable &>( eexpr );

    auto variable = memory_->find_variable( a.variable_id );

    if( variable == nullptr )
    {
        return "?";
    }

    return variable->get_name();
}

std::string StrHelperExpr::to_string_ExpressionVariableName( const Expression & eexpr )
{
    auto & a = dynamic_cast< const ExpressionVariableName &>( eexpr );

    return a.variable_name;
}

std::string StrHelperExpr::to_string_UnaryExpression( const Expression & eexpr )
{
    auto & a = dynamic_cast< const UnaryExpression &>( eexpr );

    Value temp;

    evaluate_expression( & temp, a.op );

    unary_operation( value, a.type, temp );
}

std::string StrHelperExpr::to_string_BinaryExpression( const Expression & eexpr )
{
    auto & a = dynamic_cast< const BinaryExpression &>( eexpr );

    Value lhs;
    Value rhs;

    evaluate_expression( & lhs, a.lhs );
    evaluate_expression( & rhs, a.rhs );

    binary_operation( value, a.type, lhs, rhs );
}

} // namespace fsm
