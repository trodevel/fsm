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

// $Revision: 10027 $ $Date:: 2018-12-04 #$ $Author: serge $

#ifndef LIB_FSM__FSM_H
#define LIB_FSM__FSM_H

#include <map>                  // std::map

#include "workt/worker_t.h"         // WorkerT
#include "utils/request_id_gen.h"   // utils::RequestIdGen

#include "argument.h"           // Argument
#include "actions.h"            // Actions
#include "state.h"              // State
#include "signal_handler.h"     // SignalHandler
#include "action_connector.h"   // ActionConnector
#include "i_signal_handler.h"   // ISignalHandler
#include "signal.h"             // Signal
#include "i_fsm.h"              // IFsm

namespace fsm {

class Fsm;

typedef workt::WorkerT< const Signal *, Fsm> WorkerBase;

class Fsm:
        public WorkerBase,
        public IFsm,
        public ISignalHandler
{
    friend WorkerBase;

public:
    Fsm( uint32_t log_id );
    ~Fsm();

    void consume( const Signal * req ) override;

    void start();

    void shutdown();

    void handle_signal_handler( element_id_t signal_handler_id, const std::vector<Argument> & arguments ) override;

    element_id_t create_state( const std::string & name );
    element_id_t create_add_signal_handler( element_id_t state_id, const std::string & signal_name );
    element_id_t create_add_first_action_connector( element_id_t signal_handler_id, Action * action );
    element_id_t create_add_next_action_connector( element_id_t action_connector_id, Action * action );

    element_id_t create_signal_handler( const std::string & name );
    element_id_t create_action_connector( Action * action );

private:
    typedef std::map<element_id_t,State*>           MapIdToState;
    typedef std::map<element_id_t,SignalHandler*>   MapIdToSignalHandler;
    typedef std::map<element_id_t,ActionConnector*> MapIdToActionConnector;
    typedef std::map<element_id_t,std::string>      MapIdToString;

private:
    Fsm( const Fsm & )              = delete;
    Fsm & operator=( const Fsm & )  = delete;

    void handle( const Signal * req );

    void add_name( element_id_t id, const std::string & name );
    const std::string & find_name( element_id_t id );

    void execute_action_flow( element_id_t action_connector_id );

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
