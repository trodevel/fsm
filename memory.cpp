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

// $Revision: 10202 $ $Date:: 2018-12-15 #$ $Author: serge $

#include "memory.h"            // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

#include "utils/dummy_logger.h"     // dummy_log_debug
#include "value_operations.h"               // compare_values

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

    dummy.type  = type;

    return create_add_variable( name, type, dummy );
}

element_id_t Memory::create_add_variable( const std::string & name, data_type_e type, const Value & value )
{
    auto id = get_next_id();

    auto obj = new Variable( log_id_, id, name, type, value );

    auto b = map_id_to_variable_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_variable: created variable %u", id );

    names_->add_name( id, name );

    return id;
}

element_id_t Memory::create_add_constant( const std::string & name, data_type_e type, const Value & value )
{
    auto id = get_next_id();

    auto obj = new Constant( log_id_, id, name, type, value );

    auto b = map_id_to_constant_.insert( std::make_pair( id, obj ) ).second;

    assert( b );

    dummy_log_debug( log_id_, id_, "create_add_constant: created constant %u", id );

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

void Memory::init_temp_variables_from_signal( const Signal & s, std::vector<element_id_t> * arguments )
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

    dummy_log_debug( log_id_, id_, "create_temp_variable: created variable %u", id );

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

Variable* Memory::find_variable( const std::string & name )
{
    auto id = names_->find_element( name );

    return find_variable( id );
}

void Memory::convert_arguments_to_values( std::vector<Value> * values, const std::vector<Argument> & arguments )
{
    dummy_log_trace( log_id_, id_, "convert_arguments_to_values: convert %u arguments", arguments.size() );

    for( auto & e : arguments )
    {
        Value v;

        convert_argument_to_value( & v, e );

        values->push_back( v );
    }
}

void Memory::convert_argument_to_value( Value * value, const Argument & argument )
{
    if( argument.type == argument_type_e::VALUE )
    {
        assign( value, argument.value );
    }
    else // if( argument.type == argument_type_e::VARIABLE_IN ) // or VARIABLE_OUT
    {
        if( argument.variable_id != 0 && argument.variable_name.empty() )
        {
            convert_variable_to_value( value, argument.variable_id );
        }
        else if( argument.variable_id == 0 && argument.variable_name.empty() == false )
        {
            auto variable_id = names_->find_element( argument.variable_name );

            assert( variable_id );

            convert_variable_to_value( value, argument.variable_id );
        }
        else
        {
            dummy_log_fatal( log_id_, id_, "convert_argument_to_value: illegal combination: variable_id %u, variable_name '%s'", argument.variable_id, argument.variable_name.c_str() );
            assert( 0 );
            throw std::runtime_error( "convert_argument_to_value: illegal combination" );
        }
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
    throw std::runtime_error( "convert_variable_to_value: variable_id " + std::to_string( variable_id ) + " not found in the list of variables, temp variables, and constants" );
}

void Memory::convert_values_to_value_pointers( std::vector<Value*> * value_pointers, std::vector<Value> & values )
{
    for( auto & e : values )
    {
        auto * p = & e;

        value_pointers->push_back( p );
    }
}

void Memory::import_values_into_arguments( const std::vector<Argument> & arguments, const std::vector<Value> & values )
{
    dummy_log_trace( log_id_, id_, "import_values_into_arguments: %u arguments", arguments.size() );

    assert( arguments.size() == values.size() );

    unsigned i = 0;
    unsigned imported = 0;

    for( auto & e : arguments )
    {
        ++i;

        // ignore all variables except output variables
        if( e.type != argument_type_e::VARIABLE_OUT )
        {
            continue;
        }

        if( e.variable_id != 0 )
        {
            import_value_into_variable( e.variable_id, values[i] );
        }
        else if( e.variable_name.empty() != false )
        {
            import_value_into_variable( e.variable_name, values[i] );
        }
        else
        {
            dummy_log_fatal( log_id_, id_, "import_values_into_arguments: illegal combination: variable_id %u, variable_name '%s'", e.variable_id, e.variable_name.c_str() );
            assert( 0 );
            throw std::runtime_error( "import_values_into_arguments: illegal combination" );
        }

        ++imported;
    }

    dummy_log_trace( log_id_, id_, "import_values_into_arguments: imported %u values", imported );
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
        throw std::runtime_error( "import_values_into_arguments: variable_id " + std::to_string( variable_id ) + " not found in the list of variables and temp variables" );
    }

    variable->set( value );
}

element_id_t Memory::get_next_id()
{
    return req_id_gen_->get_next_request_id();
}

} // namespace fsm
