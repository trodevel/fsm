/*

Signal.

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

#ifndef LIB_FSM__SIGNAL_H
#define LIB_FSM__SIGNAL_H

#include <vector>               // std::vector

#include "object.h"             // Object
#include "elements.h"              // Value

namespace fsm {

namespace ev {

struct Signal: public Object
{
    Signal( uint32_t process_id, const std::string & name, const std::vector<Value> & arguments ):
        process_id( process_id ),
        name( name ),
        arguments( arguments )
    {
    }

    uint32_t                        process_id;
    std::string                     name;
    std::vector<Value>              arguments;
};

} // namespace ev

} // namespace fsm

#endif // LIB_FSM__SIGNAL_H
