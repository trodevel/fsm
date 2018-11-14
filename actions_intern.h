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

// $Revision: 9954 $ $Date:: 2018-11-13 #$ $Author: serge $

#ifndef LIB_FSM__ACTIONS_INTERN_H
#define LIB_FSM__ACTIONS_INTERN_H

#include <vector>               // std::vector

#include "elements.h"           // Element

namespace fsm {

namespace intern {

class SendSignal: public Element
{
    SendSignal( const std::string & name, const std::vector<Argument> & arguments );
};

class SetTimer: public Element
{
    SetTimer( element_id_t id, const Argument & delay );
};

class FunctionCall: public Element
{
    FunctionCall( const std::string & name, const std::vector<Argument> & arguments );
};

class If: public Element
{
    If( const Expression & expr, action_id_t true_id, action_id_t false_id  );
};

class NextState: public Element
{
    NextState( element_id_t id );
};

class Exit: public Element
{
    Exit( const std::vector<Argument> & arguments );
};

} // namespace intern

} // namespace fsm

#endif // LIB_FSM__ACTIONS_INTERN_H
