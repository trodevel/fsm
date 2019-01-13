/*

SDL/GR graph generator for graphviz. Provides to_string() function.

Copyright (C) 2019 Sergey Kolevatov

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

// $Revision: 10480 $ $Date:: 2019-01-13 #$ $Author: serge $

#ifndef LIB_FSM__SDL_GR_HELPER_H
#define LIB_FSM__SDL_GR_HELPER_H

#include <string>
#include <vector>
#include <sstream>              // std::ostringstream

#include "value.h"              // Value
#include "state.h"              // State
#include "constant.h"           // Constant
#include "variable.h"           // Variable
#include "timer.h"              // Timer
#include "action_connector.h"   // ActionConnector
#include "signal_handler.h"     // SignalHandler

namespace fsm {

class Process;

class SdlGrHelper
{
public:

    SdlGrHelper( const Process * l );

    static std::ostream & write_element_name( std::ostream & os, const std::string & prefix, element_id_t id );
    static std::ostream & write_action_connector_name( std::ostream & os, element_id_t id );
    static std::ostream & write_signal_handler_name( std::ostream & os, element_id_t id );
    static std::ostream & write_state_name( std::ostream & os, element_id_t id );
    static std::ostream & write_signal_handler_def( std::ostream & os, element_id_t id, const std::string & signal_name );
    static std::ostream & write_name( std::ostream & os, const State & l );
    static std::ostream & write( std::ostream & os, const State & l );

    static std::ostream & write( std::ostream & os, const Constant & l );
    static std::ostream & write( std::ostream & os, const Variable & l );
    static std::ostream & write( std::ostream & os, const Timer & l );

    std::ostream & write( std::ostream & os, const Action & l, const ActionConnector & ac );

    std::ostream & write_SendSignal( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_SetTimer( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_ResetTimer( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_FunctionCall( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_Task( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_Condition( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_SwitchCondition( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_NextState( std::ostream & os, const Action & l, const ActionConnector & ac );
    std::ostream & write_Exit( std::ostream & os, const Action & l, const ActionConnector & ac );

    static std::ostream & write_name( std::ostream & os, const ActionConnector & l );
    std::ostream & write( std::ostream & os, const ActionConnector & l );

    static std::ostream & write( std::ostream & os, const SignalHandler & l );

    std::ostream & write( std::ostream & os );

    template<class T>
    static std::string to_string( const T & l )
    {
        std::ostringstream os;

        write( os, l );

        return os.str();
    }

private:

    typedef std::map<element_id_t,element_id_t> MapIdToId;

private:

    void write_states( std::ostream & os );
    void write_action_connectors( std::ostream & os );
    void write_signal_handlers( std::ostream & os );
    void write_data_members( std::ostream & os );
    void write_constants( std::ostream & os );
    void write_variables( std::ostream & os );
    void write_timer( std::ostream & os );

    std::ostream & write_edge( std::ostream & os, element_id_t action_connector_id_1, element_id_t action_connector_id_2, const std::string & comment = std::string() );

    void generate_map_of_next_state_actions();

    bool is_action_next_state( element_id_t action_connector_id, element_id_t * state_id ) const;

private:

    const Process   * process_;

    MapIdToId       map_next_state_action_to_state_id_;
};

} // namespace fsm

#endif // LIB_FSM__SDL_GR_HELPER_H
