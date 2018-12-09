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

// $Revision: 10093 $ $Date:: 2018-12-07 #$ $Author: serge $

#ifndef LIB_FSM__STATE_H
#define LIB_FSM__STATE_H

#include <map>                  // std::map

#include "argument.h"           // Argument
#include "actions.h"            // Actions
#include "i_signal_handler.h"   // ISignalHandler

namespace fsm {

class State: public Element
{
public:
    State( uint32_t log_id, element_id_t id, const std::string & name, ISignalHandler * handler );

    void add_signal_handler( const std::string & signal_name, element_id_t signal_handler_id );

    void handle_signal( const std::string & signal_name );

    const std::string & get_name() const;

private:
    State( const State & )              = delete;
    State & operator=( const State & )  = delete;

private:

    uint32_t                                log_id_;
    element_id_t                            id_;
    std::string                             name_;

    ISignalHandler                          * handler_;

    std::map<std::string,element_id_t>      map_signal_name_to_signal_handler_ids_;
};

} // namespace fsm

#endif // LIB_FSM__STATE_H
