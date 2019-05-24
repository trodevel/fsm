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

// $Revision: 11611 $ $Date:: 2019-05-24 #$ $Author: serge $

#ifndef LIB_FSM__ELEMENTS_H
#define LIB_FSM__ELEMENTS_H

#include <cstdint>              // uint32_t
#include <string>               // std::string
#include "anyvalue/value.h"     // Value
#include "anyvalue/operations.h"    // comparison_type_e

namespace fsm {

typedef unsigned element_id_t;
typedef unsigned state_id_t;
typedef unsigned action_id_t;

struct Element
{
    virtual ~Element() {}
};

struct NamedElement: public Element
{
    NamedElement( element_id_t id, const std::string & name ):
        id_( id ),
        name_( name )
    {
    }

    element_id_t get_id() const
    {
        return id_;
    }

    const std::string & get_name() const
    {
        return name_;
    }

protected:

    element_id_t    id_;
    std::string     name_;
};

class Action: public Element
{
};

enum class argument_type_e
{
    VALUE = 0,
    VARIABLE_IN,
    VARIABLE_OUT,
};

typedef anyvalue::data_type_e data_type_e;
typedef anyvalue::comparison_type_e comparison_type_e;
typedef anyvalue::unary_operation_type_e unary_operation_type_e;
typedef anyvalue::binary_operation_type_e binary_operation_type_e;

typedef anyvalue::Value Value;

} // namespace fsm

#endif // LIB_FSM__ELEMENTS_H
