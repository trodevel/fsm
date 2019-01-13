/*

FSM. Process Memory.

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

// $Revision: 10479 $ $Date:: 2019-01-13 #$ $Author: serge $

#include "memory.h"            // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "utils/dummy_logger.h"     // dummy_log_debug
#include "value_operations.h"               // compare_values
#include "syntax_error.h"           // SyntaxError
#include "str_helper.h"             // StrHelper

namespace fsm {

Memory::Memory(
        uint32_t                id,
        uint32_t                log_id,
        utils::IRequestIdGen    * req_id_gen,
        NamesDb                 * names ):
        id_( id ),
        log_id_( log_id ),
        req_id_gen_( req_id_gen ),
        names_( names )
{
//    dummy_logi_info( log_id_, id_, "created" );
}

Memory::~Memory()
{
//    dummy_logi_info( log_id_, id_, "destructed" );
}

element_id_t Memory::create_add_variable( const std::string & name, data_type_e type )
{
    Value dummy;

    return create_add_variable_core( name, type, dummy, false );
}

element_id_t Memory::create_add_variable( const std::string & name, data_type_e type, const Value & value )
{
    return create_add_variable_core( name, type, value, true );
}

element_id_t Memory::create_add_variable_core( const std::string & name, data_type_e type, const Value & value, bool is_inited )
{
    auto id = get_next_id();

    auto obj = is_inited ? new Variable( log_id_, id, name, type, value ) : new Variable( log_id_, id, name, type );

    auto b = map_id_to_variable_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_variable: created variable %s (%u)", name.c_str(), id );

    names_->add_name( id, name );

    return id;
}

element_id_t Memory::create_add_constant( const std::string & name, data_type_e type, const Value & value )
{
    auto id = get_next_id();

    auto obj = new Constant( log_id_, id, name, type, value );

    auto b = map_id_to_constant_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_constant: created constant %s (%u)", name.c_str(), id );

    names_->add_name( id, name );

    return id;
}

void Memory::clear_temp_variables()
{
    for( auto e : map_id_to_temp_variable_ )
    {
        auto id = e.first;
        auto b = names_->delete_name( id );

        assert( b ); (void)b;
    }

    dummy_logi_debug( log_id_, id_, "clear_temp_variables: %u variables deleted", map_id_to_temp_variable_.size() );

    map_id_to_temp_variable_.clear();
}

void Memory::init_temp_variables_from_signal( const ev::Signal & s, std::vector<element_id_t> * arguments )
{
    clear_temp_variables();

    auto i = 0;

    for( auto v : s.arguments )
    {
        ++i;

        auto id = create_temp_variable( v, i );

        arguments->push_back( id );
    }

    dummy_logi_debug( log_id_, id_, "created %u temp variables", i );
}

element_id_t Memory::create_temp_variable( const Value & v, unsigned n )
{
    auto id = get_next_id();

    auto name = "$" + std::to_string( n );

    auto obj = new Variable( log_id_, id, name, v.type, v );

    auto b = map_id_to_temp_variable_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_temp_variable: created variable %s (%u)", name.c_str(), id );

    names_->add_name( id, name );

    return id;
}

Variable* Memory::find_variable( element_id_t id )
{
    {
        auto it = map_id_to_variable_.find( id );

        if( it != map_id_to_variable_.end() )
        {
            return it->second;
        }
    }

    {
        auto it = map_id_to_temp_variable_.find( id );

        if( it != map_id_to_temp_variable_.end() )
        {
            return it->second;
        }
    }

    return nullptr;
}

const Variable* Memory::find_variable( element_id_t id ) const
{
    {
        auto it = map_id_to_variable_.find( id );

        if( it != map_id_to_variable_.end() )
        {
            return it->second;
        }
    }

    {
        auto it = map_id_to_temp_variable_.find( id );

        if( it != map_id_to_temp_variable_.end() )
        {
            return it->second;
        }
    }

    return nullptr;
}

Variable* Memory::find_variable( const std::string & name )
{
    auto id = names_->find_element( name );

    return find_variable( id );
}

const Constant* Memory::find_constant( element_id_t id ) const
{
    auto it = map_id_to_constant_.find( id );

    if( it != map_id_to_constant_.end() )
    {
        return it->second;
    }

    return nullptr;
}

void Memory::evaluate_expressions( std::vector<Value> * values, const std::vector<ExpressionPtr> & arguments )
{
    dummy_log_trace( log_id_, id_, "evaluate_expressions: convert %u arguments", arguments.size() );

    for( auto & e : arguments )
    {
        Value v;

        evaluate_expression( & v, e );

        values->push_back( v );
    }
}

void Memory::evaluate_expressions( std::vector<Value> * values, const std::vector<std::pair<bool,ExpressionPtr>> & arguments )
{
    dummy_log_trace( log_id_, id_, "evaluate_expressions: convert %u arguments", arguments.size() );

    for( auto & e : arguments )
    {
        Value v;

        evaluate_expression( & v, e.second );

        values->push_back( v );
    }
}

void Memory::convert_variable_to_value( Value * value, element_id_t variable_id )
{
    {
        auto it = map_id_to_variable_.find( variable_id );

        if( it != map_id_to_variable_.end() )
        {
            assign( value, it->second->get() );
            return;
        }
    }

    {
        auto it = map_id_to_temp_variable_.find( variable_id );

        if( it != map_id_to_temp_variable_.end() )
        {
            assign( value, it->second->get() );
            return;
        }
    }

    {
        auto it = map_id_to_constant_.find( variable_id );

        if( it != map_id_to_constant_.end() )
        {
            assign( value, it->second->get() );
            return;
        }
    }

    dummy_log_fatal( log_id_, id_, "convert_variable_to_value: variable_id %u not found in the list of variables, temp variables, and constants", variable_id );
    assert( 0 );
    throw SyntaxError( "convert_variable_to_value: variable_id " + std::to_string( variable_id ) + " not found in the list of variables, temp variables, and constants" );
}

void Memory::import_values_into_variables( const std::vector<std::pair<bool,ExpressionPtr>> & arguments, const std::vector<Value> & values )
{
    dummy_log_trace( log_id_, id_, "import_values_into_variables: %u arguments", arguments.size() );

    assert( arguments.size() == values.size() );

    unsigned i = 0;
    unsigned imported = 0;

    for( auto & e : arguments )
    {
        // ignore all variables except output variables
        if( e.first == false )
        {
            ++i;
            continue;
        }

        auto & eexpr = * e.second.get();

        if( typeid( eexpr ) == typeid( ExpressionVariable ) )
        {
            auto & a = dynamic_cast< const ExpressionVariable &>( eexpr );

            assert( a.variable_id );

            import_value_into_variable( a.variable_id, values[i] );
        }
        else if( typeid( eexpr ) == typeid( ExpressionVariableName ) )
        {
            auto & a = dynamic_cast< const ExpressionVariableName &>( eexpr );

            assert( ! a.variable_name.empty() );

            import_value_into_variable( a.variable_name, values[i] );
        }
        else
        {
            dummy_logi_fatal( log_id_, id_, "argument is not a variable %s", typeid( eexpr ).name() );
            assert( 0 );
            throw SyntaxError( "argument is not a variable " + std::string( typeid( eexpr ).name() ) );
        }

        ++i;
        ++imported;
    }

    dummy_log_trace( log_id_, id_, "import_values_into_variables: imported %u values", imported );
}

void Memory::import_value_into_variable( const std::string & variable_name, const Value & value )
{
    auto id = names_->find_element( variable_name );

    import_value_into_variable( id, value );
}

void Memory::import_value_into_variable( element_id_t variable_id, const Value & value )
{
    auto variable = find_variable( variable_id );

    if( variable == nullptr )
    {
        dummy_log_fatal( log_id_, id_, "import_value_into_variable: variable_id %u not found in the list of variables and temp variables", variable_id );
        assert( 0 );
        throw SyntaxError( "import_values_into_variables: variable_id " + std::to_string( variable_id ) + " not found in the list of variables and temp variables" );
    }

    dummy_logi_debug( log_id_, id_, "import_value_into_variable: %s (%i) = %s", variable->get_name().c_str(), variable->get_id(), StrHelper::to_string( value ).c_str() );

    variable->set( value );
}

void Memory::evaluate_expression( Value * value, ExpressionPtr expr )
{
    evaluate_expression( value, * expr.get() );
}

void Memory::evaluate_expression( Value * value, const Expression & expr )
{
    typedef Memory Type;

    typedef void (Type::*PPMF)( Value *, const Expression & );

#define MAP_ENTRY(_v)       { typeid( _v ),        & Type::evaluate_expression_##_v }

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
        dummy_logi_fatal( log_id_, id_, "unsupported expression type %s", typeid( expr ).name() );
        assert( 0 );
        throw SyntaxError( "unsupported expression type " + std::string( typeid( expr ).name() ) );
    }

    (this->*it->second)( value, expr );
}

void Memory::evaluate_expression_ExpressionValue( Value * value, const Expression & eexpr )
{
    auto & a = dynamic_cast< const ExpressionValue&>( eexpr );

    assign( value, a.value );
}

void Memory::evaluate_expression_ExpressionVariable( Value * value, const Expression & eexpr )
{
    auto & a = dynamic_cast< const ExpressionVariable &>( eexpr );

    convert_variable_to_value( value, a.variable_id );
}

void Memory::evaluate_expression_ExpressionVariableName( Value * value, const Expression & eexpr )
{
    auto & a = dynamic_cast< const ExpressionVariableName &>( eexpr );

    auto variable_id = names_->find_element( a.variable_name );

    assert( variable_id );

    convert_variable_to_value( value, variable_id );
}

void Memory::evaluate_expression_UnaryExpression( Value * value, const Expression & eexpr )
{
    auto & a = dynamic_cast< const UnaryExpression &>( eexpr );

    Value temp;

    evaluate_expression( & temp, a.op );

    unary_operation( value, a.type, temp );
}

void Memory::evaluate_expression_BinaryExpression( Value * value, const Expression & eexpr )
{
    auto & a = dynamic_cast< const BinaryExpression &>( eexpr );

    Value lhs;
    Value rhs;

    evaluate_expression( & lhs, a.lhs );
    evaluate_expression( & rhs, a.rhs );

    binary_operation( value, a.type, lhs, rhs );
}

element_id_t Memory::get_next_id()
{
    return req_id_gen_->get_next_request_id();
}

} // namespace fsm
