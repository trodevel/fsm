/*

FSM timer.

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

// $Revision: 10370 $ $Date:: 2018-12-31 #$ $Author: serge $

#ifndef LIB_FSM__TIMER_H
#define LIB_FSM__TIMER_H

#include "elements.h"           // Element
#include "scheduler/job_id_t.h" // scheduler::job_id_t

namespace fsm {

class Timer: public NamedElement
{
public:
    Timer( uint32_t log_id, element_id_t id, const std::string & name );

    void set_job_id( scheduler::job_id_t id );
    scheduler::job_id_t get_job_id() const;

private:
    Timer( const Timer & )              = delete;
    Timer & operator=( const Timer & )  = delete;

private:

    uint32_t                                log_id_;

    scheduler::job_id_t                     sched_job_id_;
};

} // namespace fsm

#endif // LIB_FSM__TIMER_H
