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

// $Revision: 9969 $ $Date:: 2018-11-14 #$ $Author: serge $

#ifndef LIB_FSM__ARGUMENT_H
#define LIB_FSM__ARGUMENT_H

#include "value.h"          // Value

namespace fsm {

struct Argument
{
    argument_type_e type;
    std::string     variable_name;
    Value           value;
};

namespace intern {

struct Argument
{
    argument_type_e type;
    element_id_t    variable_id;
    Value           value;
};

} // namespace intern

} // namespace fsm

#endif // LIB_FSM__ARGUMENT_H
