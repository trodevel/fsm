/*

FSM action connectors.

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

// $Revision: 10171 $ $Date:: 2018-12-14 #$ $Author: serge $

#ifndef LIB_FSM__ACTION_CONNECTOR_H
#define LIB_FSM__ACTION_CONNECTOR_H

#include <memory>               // std::unique_ptr

#include "actions.h"            // Action

namespace fsm {

class ActionConnector: public Element
{
public:
    ActionConnector( uint32_t log_id, element_id_t id, Action * action );

    void set_next_id( element_id_t id );
    void set_alt_next_id( element_id_t id );

    element_id_t get_next_id() const;
    element_id_t get_alt_next_id() const;

    const Action* get_action() const;

private:
    ActionConnector( const ActionConnector & )              = delete;
    ActionConnector & operator=( const ActionConnector & )  = delete;

private:

    element_id_t    id_;
    element_id_t    next_id_;
    element_id_t    alt_next_id_;

    std::unique_ptr<Action> action_;
};

} // namespace fsm

#endif // LIB_FSM__ACTION_CONNECTOR_H
