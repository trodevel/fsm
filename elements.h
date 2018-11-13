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

// $Revision: 9952 $ $Date:: 2018-11-12 #$ $Author: serge $

#ifndef LIB_FSM__ELEMENTS_H
#define LIB_FSM__ELEMENTS_H

#include <cstdint>              // uint32_t
#include <map>                  // std::map
#include <vector>               // std::vector

namespace fsm {

typedef unsigned state_id_t;
typedef unsigned action_id_t;

class Element
{
    virtual ~Element() {}
};

class Action: public Element
{
};

enum class data_type_e
{
    UNDEF = 0,
    BOOL,
    INT,
    DOUBLE,
    STRING,
};

struct Value
{
    data_type_e     type;

    bool            arg_b;
    int             arg_i;
    double          arg_d;
    std::string     arg_s;
};

enum class argument_type_e
{
    VALUE = 0,
    VARIABLE,
};

struct Argument
{
    argument_type_e type;
    std::string     variable_name;
    Value           value;
};

enum class comparison_type_e
{
    EQ = 0,
    NEQ,
    LT,
    LE,
    GT,
    GE,
    NOT
};

struct Expression
{
    bool                is_unary;
    comparison_type_e   type;
    Argument            lhs;
    Argument            rhs;
};

class SendSignal: public Element
{
    SendSignal( const std::string & name, const std::vector<Argument> & arguments );
};

class SetTimer: public Element
{
    SetTimer( const std::string & name, const Argument & delay );
};

class FunctionCall: public Element
{
    FunctionCall( const std::string & name, const std::vector<Argument> & arguments );
};

class If: public Element
{
    If( const Expression & expr, action_id_t true_id, action_id_t false_id  );
};

class NextState: public Element
{
    NextState( const std::string & name );
};

class Exit: public Element
{
    Exit( const std::vector<Argument> & arguments );
};

} // namespace fsm

#endif // LIB_FSM__ELEMENTS_H
