/*

FSM manager.

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

// $Revision: 10276 $ $Date:: 2018-12-18 #$ $Author: serge $

#ifndef LIB_FSM__FSM_MANAGER_H
#define LIB_FSM__FSM_MANAGER_H

#include <map>                  // std::map
#include <mutex>                // std::mutex

#include "workt/worker_t.h"         // WorkerT
#include "utils/request_id_gen.h"   // utils::RequestIdGen
#include "scheduler/i_scheduler.h"  // IScheduler

#include "actions.h"            // Actions
#include "state.h"              // State
#include "signal_handler.h"     // SignalHandler
#include "action_connector.h"   // ActionConnector
#include "i_signal_handler.h"   // ISignalHandler
#include "signal.h"             // Signal
#include "objects.h"            // StartProcess
#include "i_fsm.h"              // IFsm
#include "i_callback.h"         // ICallback
#include "process.h"            // Process

namespace fsm {

class FsmManager;

typedef workt::WorkerT< const Object *, FsmManager> WorkerBase;

class FsmManager:
        public WorkerBase,
        public IFsm
{
    friend WorkerBase;

public:
    FsmManager();
    ~FsmManager();

    bool init(
            unsigned int                        log_id,
            unsigned int                        log_id_fsm,
            ICallback                           * callback,
            scheduler::IScheduler               * scheduler,
            std::string                         * error_msg );

    void consume( const Object * req ) override;

    void start();

    void shutdown();

    uint32_t create_process();

    void start_process( uint32_t process_id );

    // must be called in the locked state
    Process* find_process( uint32_t process_id );

    std::mutex      & get_mutex() const;

private:

    typedef std::map<uint32_t,Process*>    MapIdToProcess;

private:
    FsmManager( const FsmManager & )              = delete;
    FsmManager & operator=( const FsmManager & )  = delete;

    void handle( const Object * req );
    void handle_Signal( const Object & req );
    void handle_StartProcess( const Object & req );
    void release( const Object * req ) const;

    element_id_t get_next_id();

    void check_process_end( MapIdToProcess::iterator it );

private:

    mutable std::mutex          mutex_;

    uint32_t                    log_id_;
    uint32_t                    log_id_fsm_;
    ICallback                   * callback_;
    scheduler::IScheduler       * scheduler_;


    MapIdToProcess                  map_id_to_process_;

    utils::RequestIdGen         req_id_gen_;
};

} // namespace fsm

#endif // LIB_FSM__FSM_MANAGER_H
