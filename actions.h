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

// $Revision: 10176 $ $Date:: 2018-12-14 #$ $Author: serge $

#ifndef LIB_FSM__ACTIONS_H
#define LIB_FSM__ACTIONS_H

#include <vector>               // std::vector

#include "expression.h"         // Expression, Action

namespace fsm {

struct SendSignal: public Action
{
    SendSignal( const std::string & name, const std::vector<Argument> & arguments ):
        name( name ),
        arguments( arguments )
    {
    }

    std::string             name;
    std::vector<Argument>   arguments;
};

struct SetTimer: public Action
{
    SetTimer( element_id_t timer_id, const Argument & delay ):
        timer_id( timer_id ),
        delay( delay )
    {
    }

    element_id_t            timer_id;
    Argument                delay;
};

struct ResetTimer: public Action
{
    ResetTimer( element_id_t timer_id ):
        timer_id( timer_id )
    {
    }

    element_id_t            timer_id;
};

struct FunctionCall: public Action
{
    FunctionCall( const std::string & name, const std::vector<Argument> & arguments ):
        name( name ),
        arguments( arguments )
    {
    }

    std::string             name;
    std::vector<Argument>   arguments;
};

struct Condition: public Action
{
    Condition( const Expression & expr ):
        expr( expr )
    {
    }

    Expression  expr;
};

struct NextState: public Action
{
    NextState( element_id_t state_id ):
        state_id( state_id )
    {
    }

    element_id_t            state_id;
};

struct Exit: public Action
{
    Exit( const std::vector<Argument> & arguments )
    {
    }
};

} // namespace fsm

#endif // LIB_FSM__ACTIONS_H
