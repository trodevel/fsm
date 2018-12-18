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

// $Revision: 10276 $ $Date:: 2018-12-18 #$ $Author: serge $

#include "fsm_manager.h"        // self

#include <cassert>              // assert
#include <typeindex>            // std::type_index
#include <typeinfo>
#include <unordered_map>

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
    for( auto & e : map_id_to_process_ )
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

void FsmManager::consume( const Object * req )
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

uint32_t FsmManager::create_process()
{
    MUTEX_SCOPE_LOCK( mutex_ );

    auto id = req_id_gen_.get_next_request_id();

    auto fsm = new Process( id, log_id_fsm_, this, callback_, scheduler_, & req_id_gen_ );

    dummy_log_info( log_id_, "new fsm %u", id );

    auto b = map_id_to_process_.insert( std::make_pair( id, fsm ) ).second;

    assert( b );(void)b;

    return id;
}

void FsmManager::start_process( uint32_t process_id )
{
    dummy_log_info( log_id_, "start process %u", process_id );

    consume( new StartProcess( process_id ) );
}

// must be called in the locked state
Process* FsmManager::find_process( uint32_t process_id )
{
    auto it = map_id_to_process_.find( process_id );

    if( it != map_id_to_process_.end() )
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

void FsmManager::handle( const Object * req )
{
    typedef FsmManager Type;

    typedef void (Type::*PPMF)( const Object & r );

#define MAP_ENTRY(_v)       { typeid( _v ),        & Type::handle_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( Signal ),
        MAP_ENTRY( StartProcess ),
    };

#undef MAP_ENTRY

    auto it = funcs.find( typeid( * req ) );

    if( it == funcs.end() )
    {
        dummy_log_fatal( log_id_, "unsupported object %s", typeid( * req ).name() );
        assert( 0 );
        throw std::runtime_error( "unsupported object " + std::string( typeid( * req ).name() ) );
    }

    (this->*it->second)( * req );

    release( req );
}

void FsmManager::handle_Signal( const Object & rreq )
{
    auto & req = dynamic_cast< const Signal &>( rreq );

    {
        MUTEX_SCOPE_LOCK( mutex_ );

        auto process_id = req.process_id;

        auto it = map_id_to_process_.find( process_id );

        if( it != map_id_to_process_.end() )
        {
            it->second->handle( req );

            check_process_end( it );
        }
        else
        {
            dummy_log_error( log_id_, "process id %u: wrong process id or process ended", process_id );
        }
    }
}

void FsmManager::handle_StartProcess( const Object & rreq )
{
    auto & req = dynamic_cast< const StartProcess &>( rreq );

    dummy_log_trace( log_id_, "handle %s, process id %u", typeid( req ).name(), req.process_id );

    {
        MUTEX_SCOPE_LOCK( mutex_ );

        auto process_id = req.process_id;

        auto it = map_id_to_process_.find( process_id );

        if( it != map_id_to_process_.end() )
        {
            it->second->start();

            check_process_end( it );
        }
        else
        {
            dummy_log_error( log_id_, "process id %u: wrong process id or process ended", process_id );
        }
    }
}

void FsmManager::release( const Object * req ) const
{
    delete req;
}

void FsmManager::check_process_end( MapIdToProcess::iterator it )
{
    if( it->second->is_ended() )
    {
        delete it->second;

        map_id_to_process_.erase( it );
    }
}

element_id_t FsmManager::get_next_id()
{
    return req_id_gen_.get_next_request_id();
}

} // namespace fsm
