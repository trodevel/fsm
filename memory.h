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

// $Revision: 10204 $ $Date:: 2018-12-15 #$ $Author: serge $

#ifndef LIB_FSM__MEMORY_H
#define LIB_FSM__MEMORY_H

#include <map>                  // std::map

#include "utils/request_id_gen.h"   // utils::RequestIdGen

#include "argument.h"           // Argument
#include "variable.h"           // Variable
#include "constant.h"           // Constant
#include "signal.h"             // Signal
#include "expression.h"         // Expression
#include "names_db.h"           // NamesDb

namespace fsm {

class Memory
{
public:
    Memory(
            uint32_t                id,
            uint32_t                log_id,
            utils::IRequestIdGen    * req_id_gen,
            NamesDb                 * names );
    ~Memory();

    element_id_t create_add_variable( const std::string & name, data_type_e type );
    element_id_t create_add_variable( const std::string & name, data_type_e type, const Value & value );
    element_id_t create_add_constant( const std::string & name, data_type_e type, const Value & value );

    void clear_temp_variables();
    void init_temp_variables_from_signal( const Signal & s, std::vector<element_id_t> * arguments );
    element_id_t create_temp_variable( const Value & v, unsigned n );

    Variable* find_variable( element_id_t id );
    Variable* find_variable( const std::string & name );

    void convert_arguments_to_values( std::vector<Value> * values, const std::vector<Argument> & arguments );
    void convert_argument_to_value( Value * value, const Argument & argument );
    void convert_variable_to_value( Value * value, element_id_t variable_id );
    void convert_values_to_value_pointers( std::vector<Value*> * value_pointers, std::vector<Value> & values );

    void import_values_into_arguments( const std::vector<Argument> & arguments, const std::vector<Value> & values );
    void import_value_into_variable( const std::string & variable_name, const Value & value );
    void import_value_into_variable( element_id_t variable_id, const Value & value );

    void evaluate_expression( Value * value, const Expression & expr );

private:
    typedef std::map<element_id_t,Variable*>        MapIdToVariable;
    typedef std::map<element_id_t,Constant*>        MapIdToConstant;

private:
    Memory( const Memory & )              = delete;
    Memory & operator=( const Memory & )  = delete;

    void evaluate_expression_ExpressionArgument( Value * value, const Expression & expr );
    void evaluate_expression_UnaryExpression( Value * value, const Expression & expr );
    void evaluate_expression_BinaryExpression( Value * value, const Expression & expr );

    element_id_t get_next_id();

private:

    uint32_t                    id_;
    uint32_t                    log_id_;
    utils::IRequestIdGen        * req_id_gen_;
    NamesDb                     * names_;

    MapIdToVariable             map_id_to_variable_;
    MapIdToVariable             map_id_to_temp_variable_;
    MapIdToConstant             map_id_to_constant_;
};

} // namespace fsm

#endif // LIB_FSM__MEMORY_H
