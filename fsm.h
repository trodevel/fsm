/*

FSM.

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

// $Revision: 9991 $ $Date:: 2018-11-19 #$ $Author: serge $

#ifndef LIB_FSM__FSM_H
#define LIB_FSM__FSM_H

#include <map>                  // std::map

#include "argument.h"           // Argument
#include "actions.h"            // Actions
#include "state.h"              // State
#include "signal_handler.h"     // SignalHandler
#include "action_connector.h"   // ActionConnector
#include "i_signal_handler.h"   // ISignalHandler

namespace fsm {

class Fsm: public ISignalHandler
{
public:
    Fsm( uint32_t log_id );

    void handle_signal( element_id_t signal_id, const std::vector<Argument> & arguments ) override;

private:
    typedef std::map<element_id_t,State>            MapIdToState;
    typedef std::map<element_id_t,SignalHandler>    MapIdToSignalHandler;
    typedef std::map<element_id_t,ActionConnector>  MapIdToActionConnector;

private:
    Fsm( const Fsm & )              = delete;
    Fsm & operator=( const Fsm & )  = delete;

private:

    uint32_t                    log_id_;

    MapIdToState                map_id_to_state_;
    MapIdToSignalHandler        map_id_to_signal_handler_;

};

} // namespace fsm

#endif // LIB_FSM__FSM_H
