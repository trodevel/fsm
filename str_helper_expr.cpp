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

// $Revision: 10466 $ $Date:: 2019-01-11 #$ $Author: serge $

#include "str_helper_expr.h"    // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>
#include <sstream>              // std::ostringstream

#include "value_operations.h"       // compare_values
#include "syntax_error.h"           // SyntaxError
#include "str_helper.h"             // StrHelper

namespace fsm {

StrHelperExpr::StrHelperExpr(
        const Memory                & memory ):
        memory_( memory )
{
}

std::string StrHelperExpr::to_string( ExpressionPtr expr ) const
{
    return to_string( * expr.get() );
}

std::string StrHelperExpr::to_string( const std::vector<ExpressionPtr> expr ) const
{
    std::ostringstream os;

    bool b = false;

    for( auto & e : expr )
    {
        if( b )
        {
            os << ", ";
        }
        else
        {
            b = true;
        }

        os << to_string( e );
    }

    return os.str();
}

std::string StrHelperExpr::to_string( const std::vector<std::pair<bool,ExpressionPtr>> expr ) const
{
    std::ostringstream os;

    bool b = false;

    for( auto & e : expr )
    {
        if( b )
        {
            os << ", ";
        }
        else
        {
            b = true;
        }

        if( e.first )
        {
            os << "& ";
        }

        os << to_string( e.second );
    }

    return os.str();
}

std::string StrHelperExpr::to_string( const Expression & expr ) const
{
    typedef StrHelperExpr Type;

    typedef std::string (Type::*PPMF)( const Expression & ) const;

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

std::string StrHelperExpr::to_string_ExpressionValue( const Expression & eexpr ) const
{
    auto & a = dynamic_cast< const ExpressionValue&>( eexpr );

    auto res = StrHelper::to_string_short( a.value );

    if( res.size() > 8 )
        return res.substr( 0, 8 ) + "...";

    return res;
}

std::string StrHelperExpr::to_string_ExpressionVariable( const Expression & eexpr ) const
{
    auto & a = dynamic_cast< const ExpressionVariable &>( eexpr );

    auto variable = memory_.find_variable( a.variable_id );

    if( variable != nullptr )
    {
        return variable->get_name();
    }

    auto constant = memory_.find_constant( a.variable_id );

    if( constant != nullptr )
    {
        return constant->get_name();
    }

    return "?";
}

std::string StrHelperExpr::to_string_ExpressionVariableName( const Expression & eexpr ) const
{
    auto & a = dynamic_cast< const ExpressionVariableName &>( eexpr );

    return a.variable_name;
}

std::string StrHelperExpr::to_string_UnaryExpression( const Expression & eexpr ) const
{
    auto & a = dynamic_cast< const UnaryExpression &>( eexpr );

    return StrHelper::to_string_short( a.type ) + " ( " + to_string( a.op ) + " )";
}

std::string StrHelperExpr::to_string_BinaryExpression( const Expression & eexpr ) const
{
    auto & a = dynamic_cast< const BinaryExpression &>( eexpr );

    return "( " + to_string( a.lhs ) + StrHelper::to_string_short( a.type ) + to_string( a.rhs ) + " )";
}

} // namespace fsm
