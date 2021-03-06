/*

StartProcess.

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

// $Revision: 10288 $ $Date:: 2018-12-20 #$ $Author: serge $

#ifndef LIB_FSM__OBJECTS_H
#define LIB_FSM__OBJECTS_H

#include <cstdint>              // uint32_t

#include "object.h"             // Object

namespace fsm {

namespace ev {

struct StartProcess: public Object
{
    StartProcess( uint32_t process_id ):
        process_id( process_id )
    {
    }

    uint32_t                        process_id;
};

struct Timer: public Object
{
    Timer( uint32_t process_id, element_id_t timer_id ):
        process_id( process_id ),
        timer_id( timer_id )
    {
    }

    uint32_t                        process_id;
    element_id_t                    timer_id;
};

} // namespace ev

} // namespace fsm

#endif // LIB_FSM__OBJECTS_H
