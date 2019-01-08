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

// $Revision: 10422 $ $Date:: 2019-01-08 #$ $Author: serge $

#ifndef LIB_FSM__SDL_GR_HELPER_H
#define LIB_FSM__SDL_GR_HELPER_H

#include <string>
#include <vector>
#include <sstream>              // std::ostringstream

#include "value.h"              // Value
#include "state.h"              // State
#include "action_connector.h"   // ActionConnector
#include "signal_handler.h"     // SignalHandler

namespace fsm {

class Process;

class SdlGrHelper
{
public:
    static std::ostream & write_element_name( std::ostream & os, const std::string & prefix, element_id_t id );
    static std::ostream & write_action_connector_name( std::ostream & os, element_id_t id );
    static std::ostream & write_signal_handler_name( std::ostream & os, element_id_t id );
    static std::ostream & write_state_name( std::ostream & os, element_id_t id );
    static std::ostream & write_signal_handler_def( std::ostream & os, element_id_t id, const std::string & signal_name );
    static std::ostream & write_name( std::ostream & os, const State & l );
    static std::ostream & write( std::ostream & os, const State & l );

    static std::ostream & write( std::ostream & os, const Action & l, const ActionConnector & ac );

    static std::ostream & write_SendSignal( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_SetTimer( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_ResetTimer( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_FunctionCall( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_Task( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_Condition( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_SwitchCondition( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_NextState( std::ostream & os, const Action & l, const ActionConnector & ac );
    static std::ostream & write_Exit( std::ostream & os, const Action & l, const ActionConnector & ac );

    static std::ostream & write_name( std::ostream & os, const ActionConnector & l );
    static std::ostream & write( std::ostream & os, const ActionConnector & l );

    static std::ostream & write( std::ostream & os, const SignalHandler & l );

    static std::ostream & write( std::ostream & os, const Process & l );

    template<class T>
    static std::string to_string( const T & l )
    {
        std::ostringstream os;

        write( os, l );

        return os.str();
    }

private:

    static std::ostream & write_edge( std::ostream & os, element_id_t action_connector_id_1, element_id_t action_connector_id_2, const std::string & comment = std::string() );
};

} // namespace fsm

#endif // LIB_FSM__SDL_GR_HELPER_H
