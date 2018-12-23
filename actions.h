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

// $Revision: 10339 $ $Date:: 2018-12-23 #$ $Author: serge $

#ifndef LIB_FSM__ACTIONS_H
#define LIB_FSM__ACTIONS_H

#include <vector>               // std::vector

#include "expression.h"         // Expression, Action

namespace fsm {

struct SendSignal: public Action
{
    SendSignal( const std::string & name, const std::vector<ExpressionPtr> & arguments ):
        name( name ),
        arguments( arguments )
    {
    }

    std::string                 name;
    std::vector<ExpressionPtr>  arguments;
};

struct SetTimer: public Action
{
    SetTimer( element_id_t timer_id, const ExpressionPtr & delay ):
        timer_id( timer_id ),
        delay( delay )
    {
    }

    element_id_t            timer_id;
    ExpressionPtr           delay;
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
    FunctionCall( const std::string & name, const std::vector<std::pair<bool,ExpressionPtr>> & arguments ):
        name( name ),
        arguments( arguments )
    {
    }

    std::string                                 name;
    std::vector<std::pair<bool,ExpressionPtr>>  arguments;
};

struct Task: public Action
{
    Task( element_id_t variable_id, const ExpressionPtr & expr ):
        variable_id( variable_id ),
        expr( expr )
    {
    }

    element_id_t            variable_id;
    ExpressionPtr           expr;
};

struct Condition: public Action
{
    Condition(
            comparison_type_e           type,
            ExpressionPtr               lhs,
            ExpressionPtr               rhs ):
        type( type ),
        lhs( lhs ),
        rhs( rhs )
    {
    }

    comparison_type_e           type;
    ExpressionPtr               lhs;
    ExpressionPtr               rhs;
};

struct SwitchCondition: public Action
{
    SwitchCondition(
            Expression                          * var,
            const std::vector<ExpressionPtr>    & values ):
        var( var ),
        values( values )
    {
    }

    ExpressionPtr               var;
    std::vector<ExpressionPtr>  values;
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
    Exit()
    {
    }
};

} // namespace fsm

#endif // LIB_FSM__ACTIONS_H
