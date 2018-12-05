/*

FSM manager.

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

// $Revision: 10059 $ $Date:: 2018-12-06 #$ $Author: serge $

#include "fsm_manager.h"        // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug

namespace fsm {

FsmManager::FsmManager( uint32_t log_id ):
        WorkerBase( this ),
        log_id_( log_id ),
        callback_( nullptr )
{
    req_id_gen_.init( 1, 1 );
}

FsmManager::~FsmManager()
{
}

bool FsmManager::init(
        ICallback * callback )
{
    assert( callback );

    callback_   = callback;

    return true;
}

void FsmManager::consume( uint32_t fsm_id, const Signal * req )
{
    WorkerBase::consume( req );
}

void FsmManager::start()
{
    WorkerBase::start();
}

void FsmManager::shutdown()
{
    WorkerBase::shutdown();
}

element_id_t FsmManager::create_state( const std::string & name )
{
}

element_id_t FsmManager::create_add_signal_handler( element_id_t state_id, const std::string & signal_name )
{
}

element_id_t FsmManager::create_add_first_action_connector( element_id_t signal_handler_id, Action * action )
{
}

element_id_t FsmManager::create_add_next_action_connector( element_id_t action_connector_id, Action * action )
{
}

element_id_t FsmManager::create_signal_handler( const std::string & name )
{
}

element_id_t FsmManager::create_action_connector( Action * action )
{
}

void FsmManager::handle( const Signal * req )
{
}

element_id_t FsmManager::get_next_id()
{
    return req_id_gen_.get_next_request_id();
}

} // namespace fsm
