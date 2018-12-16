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

// $Revision: 10212 $ $Date:: 2018-12-16 #$ $Author: serge $

#ifndef LIB_FSM__EXPRESSION_H
#define LIB_FSM__EXPRESSION_H

#include <memory>               // std::shared_ptr

#include "elements.h"           // element_id_t
#include "value.h"              // Value

namespace fsm {

struct Expression
{
    virtual ~Expression() {}
};

typedef std::shared_ptr<Expression> ExpressionPtr;

struct ExpressionVariable: public Expression
{
    element_id_t        variable_id;
};

struct ExpressionVariableName: public Expression
{
    std::string         variable_name;
};

struct ExpressionValue: public Expression
{
    Value               value;
};

struct UnaryExpression: public Expression
{
    unary_operation_type_e      type;
    ExpressionPtr               op;
};

struct BinaryExpression: public Expression
{
    binary_operation_type_e     type;

    ExpressionPtr               lhs;
    ExpressionPtr               rhs;
};

} // namespace fsm

#endif // LIB_FSM__EXPRESSION_H
