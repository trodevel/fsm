/*

FSM state.

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

// $Revision: 10004 $ $Date:: 2018-11-23 #$ $Author: serge $

#ifndef LIB_FSM__STATE_H
#define LIB_FSM__STATE_H

#include <set>                  // std::set

#include "argument.h"           // Argument
#include "actions.h"            // Actions
#include "i_signal_handler.h"   // ISignalHandler

namespace fsm {

class State: public Element
{
public:
    State( uint32_t log_id, element_id_t id, const std::string & name, ISignalHandler * handler );

    void add_signal_handler( element_id_t signal_id );

    void handle_signal( element_id_t signal_id, const std::vector<Argument> & arguments );

private:
    State( const State & )              = delete;
    State & operator=( const State & )  = delete;

private:

    uint32_t                                log_id_;
    element_id_t                            id_;
    std::string                             name_;

    ISignalHandler                          * handler_;

    std::set<element_id_t>                  set_signal_ids_;
};

} // namespace fsm

#endif // LIB_FSM__STATE_H
