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

// $Revision: 10189 $ $Date:: 2018-12-14 #$ $Author: serge $

#ifndef LIB_FSM__PROCESS_H
#define LIB_FSM__PROCESS_H

#include <map>                  // std::map

#include "workt/worker_t.h"         // WorkerT
#include "utils/request_id_gen.h"   // utils::RequestIdGen
#include "scheduler/i_scheduler.h"  // IScheduler

#include "argument.h"           // Argument
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

namespace fsm {

class Process:
        public ISignalHandler
{
public:
    Process(    uint32_t                id,
            uint32_t                log_id,
            IFsm                    * parent,
            ICallback               * callback,
            scheduler::IScheduler   * scheduler,
            utils::IRequestIdGen    * req_id_gen );
    ~Process();

    void handle( const Signal * req );

    void handle_signal_handler( element_id_t signal_handler_id, const std::vector<element_id_t> & arguments ) override;

    element_id_t create_state( const std::string & name );
    element_id_t create_add_signal_handler( element_id_t state_id, const std::string & signal_name );
    element_id_t create_add_first_action_connector( element_id_t signal_handler_id, Action * action );
    element_id_t create_add_next_action_connector( element_id_t action_connector_id, Action * action );
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
    typedef std::map<element_id_t,std::string>      MapIdToString;
    typedef std::map<std::string,element_id_t>      MapStringToId;

    enum class flow_control_e
    {
        STOP,
        NEXT,
        ALT_NEXT
    };

private:
    Process( const Process & )              = delete;
    Process & operator=( const Process & )  = delete;

    void add_name( element_id_t id, const std::string & name );
    const std::string & get_name( element_id_t id );
    element_id_t find_element( const std::string & name ) const;
    bool delete_name( element_id_t id );

    void clear_temp_variables();
    void init_temp_variables_from_signal( const Signal & s, std::vector<element_id_t> * arguments );
    element_id_t create_temp_variable( const Value & v, unsigned n );

    State* find_state( element_id_t id );
    Variable* find_variable( element_id_t id );
    Variable* find_variable( const std::string & name );
    Timer* find_timer( element_id_t id );

    void convert_arguments_to_values( std::vector<Value> * values, const std::vector<Argument> & arguments );
    void convert_argument_to_value( Value * value, const Argument & argument );
    void convert_variable_to_value( Value * value, element_id_t variable_id );
    void convert_values_to_value_pointers( std::vector<Value*> * value_pointers, std::vector<Value> & values );

    void evaluate_expression( Value * value, const Expression & expr );
    void evaluate_expression_ExpressionArgument( Value * value, const Expression & expr );
    void evaluate_expression_UnaryExpression( Value * value, const Expression & expr );
    void evaluate_expression_BinaryExpression( Value * value, const Expression & expr );

    void import_values_into_arguments( const std::vector<Argument> & arguments, const std::vector<Value> & values );
    void import_value_into_variable( const std::string & variable_name, const Value & value );
    void import_value_into_variable( element_id_t variable_id, const Value & value );

    void set_timer( Timer * timer, const Value & delay );
    void reset_timer( Timer * timer );

    void execute_action_connector_id( element_id_t action_connector_id );
    void execute_action_connector( const ActionConnector & action_connector );

    flow_control_e handle_action( const Action & a );
    flow_control_e handle_SendSignal( const Action & a );
    flow_control_e handle_SetTimer( const Action & a );
    flow_control_e handle_ResetTimer( const Action & a );
    flow_control_e handle_FunctionCall( const Action & a );
    flow_control_e handle_Condition( const Action & a );
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
    utils::IRequestIdGen        * req_id_gen_;

    bool                        id_ended_;
    element_id_t                current_state_;

    MapIdToState                map_id_to_state_;
    MapIdToSignalHandler        map_id_to_signal_handler_;
    MapIdToActionConnector      map_id_to_action_connector_;
    MapIdToVariable             map_id_to_variable_;
    MapIdToVariable             map_id_to_temp_variable_;
    MapIdToConstant             map_id_to_constant_;
    MapIdToTimer                map_id_to_timer_;
    MapIdToString               map_id_to_name_;
    MapStringToId               map_name_to_id_;
};

} // namespace fsm

#endif // LIB_FSM__PROCESS_H