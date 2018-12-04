/*

FSM signal/function calls handler.

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

// $Revision: 10038 $ $Date:: 2018-12-04 #$ $Author: serge $

#ifndef LIB_FSM__I_CALLBACK_H
#define LIB_FSM__I_CALLBACK_H

#include <vector>               // std::vector

#include "outer_argument.h"     // SignalArgument

namespace fsm {

struct ICallback
{
    virtual ~ICallback() {}

    virtual void handle_send_signal( const std::string & name, const std::vector<outer::Argument> & arguments )       = 0;
    virtual void handle_function_call( const std::string & name, const std::vector<outer::FunctionArgument> & arguments )   = 0;
};

} // namespace fsm

#endif // LIB_FSM__I_CALLBACK_H
