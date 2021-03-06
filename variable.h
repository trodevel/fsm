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

// $Revision: 11611 $ $Date:: 2019-05-24 #$ $Author: serge $

#ifndef LIB_FSM__VARIABLE_H
#define LIB_FSM__VARIABLE_H

#include "elements.h"      // Value

namespace fsm {

class Variable: public NamedElement
{
public:
    Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type );
    Variable( uint32_t log_id, element_id_t id, const std::string & name, data_type_e type, const Value & value );

    data_type_e get_type() const;
    bool is_inited() const;
    const Value & get() const;
    void set( const Value & v );
    void assign( const Value & v );

private:
    Variable( const Variable & )              = delete;
    Variable & operator=( const Variable & )  = delete;

protected:

    uint32_t                                log_id_;
    data_type_e                             type_;
    bool                                    is_inited_;
    Value                                   value_;
};

} // namespace fsm

#endif // LIB_FSM__VARIABLE_H
