/*

FSM signal handler.

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

// $Revision: 9988 $ $Date:: 2018-11-19 #$ $Author: serge $

#ifndef LIB_FSM__SIGNAL_HANDLER_H
#define LIB_FSM__SIGNAL_HANDLER_H

#include "elements.h"       // Element

namespace fsm {

class SignalHandler: public Element
{
public:
    SignalHandler( uint32_t log_id, element_id_t id, const std::string & name );

    void set_first_action_id( element_id_t id );

private:
    SignalHandler( const SignalHandler & )              = delete;
    SignalHandler & operator=( const SignalHandler & )  = delete;

private:

    uint32_t                                log_id_;
    element_id_t                            id_;
    std::string                             name_;

    element_id_t                            first_action_id_;
};

} // namespace fsm

#endif // LIB_FSM__SIGNAL_HANDLER_H
