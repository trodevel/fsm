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

// $Revision: 10129 $ $Date:: 2018-12-12 #$ $Author: serge $

#include "timer.h"              // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

Timer::Timer( uint32_t log_id, element_id_t id, const std::string & name ):
        log_id_( log_id ),
        id_( id ),
        name_( name ),
        sched_job_id_( 0 )
{
    assert( id );
}

uint32_t Timer::get_id() const
{
    return id_;
}

const std::string & Timer::get_name() const
{
    return name_;
}

void Timer::set_job_id( scheduler::job_id_t id )
{
    sched_job_id_   = id;
}

scheduler::job_id_t Timer::get_job_id() const
{
    return sched_job_id_;
}

} // namespace fsm
