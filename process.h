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

// $Revision: 10395 $ $Date:: 2019-01-05 #$ $Author: serge $

#ifndef LIB_FSM__PROCESS_H
#define LIB_FSM__PROCESS_H

#include <map>                  // std::map

#include "workt/worker_t.h"         // WorkerT
#include "utils/request_id_gen.h"   // utils::RequestIdGen
#include "scheduler/i_scheduler.h"  // IScheduler

#include "actions.h"            // Actions
#include "state.h"              // State
#include "signal_handler.h"     // SignalHandler
#include "action_connector.h"   // ActionConnector
#include "variable.h"           // Variable
#include "constant.h"           // Constant
#include "timer.h"              // Timer
#include "i_signal_handler.h"   // ISignalHandler
#include "signal.h"             // Signal
#include "i_fsm.h"              // IFsm
#include "i_callback.h"         // ICallback
#include "names_db.h"           // NamesDb
#include "memory.h"             // Memory
#include "objects.h"            // ev::Timer

namespace fsm {

class Process:
        public ISignalHandler
{
    friend class SdlGrHelper;

public:
    Process(
            uint32_t                id,
            uint32_t                log_id,
            IFsm                    * parent,
            ICallback               * callback,
            scheduler::IScheduler   * scheduler );
    ~Process();

    void start();
    void handle( const ev::Signal & req );
    void handle( const ev::Timer & req );

    void handle_signal_handler( element_id_t signal_handler_id, const std::vector<element_id_t> & arguments ) override;

    void set_first_action_connector( element_id_t signal_handler_id, element_id_t action_connector_id );
    void set_next_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id );
    void set_alt_next_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id );
    void set_default_switch_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id );
    void add_switch_action_connector( element_id_t action_connector_id, element_id_t next_action_connector_id );

    element_id_t create_add_start_action_connector( Action * action );
    element_id_t create_state( const std::string & name );
    element_id_t create_add_signal_handler( element_id_t state_id, const std::string & signal_name );
    element_id_t create_set_first_action_connector( element_id_t signal_handler_id, Action * action );
    element_id_t create_set_next_action_connector( element_id_t action_connector_id, Action * action );
    element_id_t create_set_alt_next_action_connector( element_id_t action_connector_id, Action * action );
    element_id_t create_set_default_switch_action_connector( element_id_t action_connector_id, Action * action );
    element_id_t create_add_switch_action_connector( element_id_t action_connector_id, Action * action );
    element_id_t create_add_timer( const std::string & name );

    element_id_t create_signal_handler( const std::string & name );
    element_id_t create_action_connector( Action * action );

    element_id_t create_add_variable( const std::string & name, data_type_e type );
    element_id_t create_add_variable( const std::string & name, data_type_e type, const Value & value );
    element_id_t create_add_constant( const std::string & name, data_type_e type, const Value & value );

    void set_initial_state( element_id_t state_id );

    bool is_ended() const;

private:
    typedef std::map<element_id_t,State*>           MapIdToState;
    typedef std::map<element_id_t,SignalHandler*>   MapIdToSignalHandler;
    typedef std::map<element_id_t,ActionConnector*> MapIdToActionConnector;
    typedef std::map<element_id_t,Variable*>        MapIdToVariable;
    typedef std::map<element_id_t,Constant*>        MapIdToConstant;
    typedef std::map<element_id_t,Timer*>           MapIdToTimer;

    enum class flow_control_e
    {
        STOP,
        NEXT,
        ALT_NEXT,
        CHECK_SWITCH
    };

    enum class internal_state_e
    {
        IDLE,
        ACTIVE,
        FINISHED
    };

    enum class next_action_type_e
    {
        MAIN,
        ALT,
        SWITCH_DEFAULT,
        SWITCH_NEXT
    };

private:
    Process( const Process & )              = delete;
    Process & operator=( const Process & )  = delete;

    State* find_state( element_id_t id );
    Timer* find_timer( element_id_t id );
    ActionConnector* find_action_connector( element_id_t id );
    const ActionConnector* find_action_connector( element_id_t id ) const;

    void set_timer( Timer * timer, const Value & delay );
    void reset_timer( Timer * timer );

    static void convert_values_to_value_pointers( std::vector<Value*> * value_pointers, std::vector<Value> & values );

    void set_matched_switch_condition( int matched_switch_condition );
    int get_matched_switch_condition_and_clear();

    void set_next_action_connector_intern( element_id_t action_connector_id, element_id_t next_action_connector_id, next_action_type_e type );
    element_id_t create_set_next_action_connector_intern( element_id_t action_connector_id, Action * action, next_action_type_e type );

    void execute_action_connector_id( element_id_t action_connector_id );
    void execute_action_connector( const ActionConnector & action_connector );

    flow_control_e handle_action( const Action & a );
    flow_control_e handle_SendSignal( const Action & a );
    flow_control_e handle_SetTimer( const Action & a );
    flow_control_e handle_ResetTimer( const Action & a );
    flow_control_e handle_FunctionCall( const Action & a );
    flow_control_e handle_Task( const Action & a );
    flow_control_e handle_Condition( const Action & a );
    flow_control_e handle_SwitchCondition( const Action & a );
    flow_control_e handle_NextState( const Action & a );
    flow_control_e handle_Exit( const Action & a );

    void next_state( element_id_t state );

    element_id_t get_next_id();

private:

    uint32_t                    id_;
    uint32_t                    log_id_;
    IFsm                        * parent_;
    ICallback                   * callback_;
    scheduler::IScheduler       * scheduler_;

    internal_state_e            internal_state_;
    element_id_t                current_state_;
    element_id_t                start_action_connector_;

    int                         matched_switch_condition_;

    MapIdToState                map_id_to_state_;
    MapIdToSignalHandler        map_id_to_signal_handler_;
    MapIdToActionConnector      map_id_to_action_connector_;
    MapIdToTimer                map_id_to_timer_;

    utils::RequestIdGen         req_id_gen_;

    NamesDb                     names_;
    Memory                      mem_;
};

} // namespace fsm

#endif // LIB_FSM__PROCESS_H
