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

// $Revision: 9997 $ $Date:: 2018-11-22 #$ $Author: serge $

#ifndef LIB_FSM__FSM_H
#define LIB_FSM__FSM_H

#include <map>                  // std::map

#include "argument.h"           // Argument
#include "actions.h"            // Actions
#include "state.h"              // State
#include "signal_handler.h"     // SignalHandler
#include "action_connector.h"   // ActionConnector
#include "i_signal_handler.h"   // ISignalHandler
#include "utils/request_id_gen.h"   // utils::RequestIdGen

namespace fsm {

class Fsm: public ISignalHandler
{
public:
    Fsm( uint32_t log_id );
    ~Fsm();

    void handle_signal( element_id_t signal_id, const std::vector<Argument> & arguments ) override;

    element_id_t create_state( const std::string & name );
    element_id_t create_add_signal_handler( element_id_t state_id, const std::string & name );

private:
    typedef std::map<element_id_t,State*>           MapIdToState;
    typedef std::map<element_id_t,SignalHandler*>   MapIdToSignalHandler;
    typedef std::map<element_id_t,ActionConnector*> MapIdToActionConnector;
    typedef std::map<element_id_t,std::string>      MapIdToString;

private:
    Fsm( const Fsm & )              = delete;
    Fsm & operator=( const Fsm & )  = delete;

    void add_name( element_id_t id, const std::string & name );
    const std::string & find_name( element_id_t id );

    element_id_t get_next_id();

private:

    uint32_t                    log_id_;

    MapIdToState                map_id_to_state_;
    MapIdToSignalHandler        map_id_to_signal_handler_;
    MapIdToActionConnector      map_id_to_action_connector_;
    MapIdToString               map_id_to_name_;

    utils::RequestIdGen         req_id_gen_;
};

} // namespace fsm

#endif // LIB_FSM__FSM_H
