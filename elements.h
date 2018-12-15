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

// $Revision: 10184 $ $Date:: 2018-12-14 #$ $Author: serge $

#ifndef LIB_FSM__ELEMENTS_H
#define LIB_FSM__ELEMENTS_H

#include <cstdint>              // uint32_t
#include <string>               // std::string

namespace fsm {

typedef unsigned element_id_t;
typedef unsigned state_id_t;
typedef unsigned action_id_t;

struct Element
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

enum class argument_type_e
{
    VALUE = 0,
    VARIABLE_IN,
    VARIABLE_OUT,
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

enum class unary_operation_type_e
{
    NOT    = 0,
};

enum class binary_operation_type_e
{
    PLUS    = 0,
    MINUS,
    MUL,
    DIV
};

} // namespace fsm

#endif // LIB_FSM__ELEMENTS_H
