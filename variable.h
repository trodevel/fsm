/*

FSM Variable.

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

// $Revision: 10240 $ $Date:: 2018-12-17 #$ $Author: serge $

#ifndef LIB_FSM__VARIABLE_H
#define LIB_FSM__VARIABLE_H

#include "value.h"      // Value

namespace fsm {

class Variable: public Element
{
public:
    Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type );
    Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type, const Value & value );

    const Value & get() const;
    void set( const Value & v );
    void assign( const Value & v );

private:
    Variable( const Variable & )              = delete;
    Variable & operator=( const Variable & )  = delete;

protected:

    uint32_t                                log_id_;
    element_id_t                            id_;
    std::string                             name_;
    data_type_e                             type_;
    Value                                   value_;
};

} // namespace fsm

#endif // LIB_FSM__VARIABLE_H
