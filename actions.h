/*

FSM objects.

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

// $Revision: 9986 $ $Date:: 2018-11-19 #$ $Author: serge $

#ifndef LIB_FSM__ACTIONS_H
#define LIB_FSM__ACTIONS_H

#include <vector>               // std::vector

#include "expression.h"         // Expression, Action

namespace fsm {

class SendSignal: public Action
{
    SendSignal( const std::string & name, const std::vector<Argument> & arguments );
};

class SetTimer: public Action
{
    SetTimer( element_id_t id, const Argument & delay );
};

class FunctionCall: public Action
{
    FunctionCall( const std::string & name, const std::vector<Argument> & arguments );
};

class If: public Action
{
    If( const Expression & expr, action_id_t true_id, action_id_t false_id  );
};

class NextState: public Action
{
    NextState( element_id_t id );
};

class Exit: public Action
{
    Exit( const std::vector<Argument> & arguments );
};

} // namespace fsm

#endif // LIB_FSM__ACTIONS_H
