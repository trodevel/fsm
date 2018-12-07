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

// $Revision: 10080 $ $Date:: 2018-12-07 #$ $Author: serge $

#include "fsm_manager.h"        // self

#include <cassert>              // assert

#include "utils/dummy_logger.h"     // dummy_log_debug
#include "utils/mutex_helper.h"     // MUTEX_SCOPE_LOCK

namespace fsm {

FsmManager::FsmManager():
        WorkerBase( this ),
        log_id_( 0 ),
        log_id_fsm_( 0 ),
        callback_( nullptr ),
        scheduler_( nullptr )
{
    req_id_gen_.init( 1, 1 );
}

FsmManager::~FsmManager()
{
    for( auto & e : map_id_to_fsm_ )
    {
        delete e.second;
    }

    dummy_log_info( log_id_, "destructed" );
}

bool FsmManager::init(
        uint32_t                            log_id,
        uint32_t                            log_id_fsm,
        ICallback                           * callback,
        scheduler::IScheduler               * scheduler,
        std::string                         * error_msg )
{
    * error_msg   = "FsmManager";

    assert( callback );
    assert( scheduler );

    log_id_     = log_id;
    log_id_fsm_ = log_id_fsm;
    callback_   = callback;
    scheduler_  = scheduler;

    dummy_log_info( log_id_, "init OK" );

    return true;
}

void FsmManager::consume( const Signal * req )
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

element_id_t FsmManager::create_fsm()
{
    MUTEX_SCOPE_LOCK( mutex_ );

    auto id = req_id_gen_.get_next_request_id();

    auto fsm = new Fsm( id, log_id_fsm_, this, callback_, scheduler_, & req_id_gen_ );

    dummy_log_info( log_id_, "new fsm %u", id );

    auto b = map_id_to_fsm_.insert( std::make_pair( id, fsm ) ).second;

    assert( b );(void)b;

    return id;
}

// must be called in the locked state
Fsm* FsmManager::find_fsm( element_id fsm_id )
{
    auto it = map_id_to_fsm_.find( fsm_id );

    if( it != map_id_to_fsm_.end() )
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

std::mutex & FsmManager::get_mutex() const
{
    return mutex_;
}

void FsmManager::handle( const Signal * req )
{
    {
        MUTEX_SCOPE_LOCK( mutex_ );

        auto fsm_id = req->fsm_id;

        auto it = map_id_to_fsm_.find( fsm_id );

        if( it != map_id_to_fsm_.end() )
        {
            it->second->handle( req );

            check_fsm_end( it );
        }
        else
        {
            dummy_log_error( log_id_, "fsm id %u: wrong fsm id or fsm ended", fsm_id );
        }
    }

    release( req );
}

void FsmManager::release( const Signal * req ) const
{
    delete req;
}

void FsmManager::check_fsm_end( MapIdToFsm::iterator it )
{
    if( it->second->is_ended() )
    {
        delete it->second;

        map_id_to_call_.erase( it );
    }
}

element_id_t FsmManager::get_next_id()
{
    return req_id_gen_.get_next_request_id();
}

} // namespace fsm
