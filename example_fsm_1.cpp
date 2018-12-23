#include <iostream>         // cout

#include "process.h"        // Process

void init_fsm_1( fsm::Process * fsm )
{
    auto timer              = fsm->create_add_timer( "T" );
    auto E_DONE             = fsm->create_add_constant( "DONE",     fsm::data_type_e::INT, fsm::Value( 0 ) );
    auto E_CANCELLED        = fsm->create_add_constant( "CANCELLED", fsm::data_type_e::INT, fsm::Value( 1 ) );
    auto E_FAILED           = fsm->create_add_constant( "FAILED",   fsm::data_type_e::INT, fsm::Value( 2 ) );
    auto E_ABORTED          = fsm->create_add_constant( "ABORTED",  fsm::data_type_e::INT, fsm::Value( 3 ) );

    auto IDLE               = fsm->create_state( "IDLE" );
    auto PLAYING_MESSAGE    = fsm->create_state( "PLAYING_MESSAGE" );

    auto st_ac1             = fsm->create_add_start_action_connector( new fsm::SetTimer( timer, fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 1 ) ) ) ) );
    auto st_ac2             = fsm->create_set_next_action_connector( st_ac1, new fsm::NextState( IDLE ) );
    (void)st_ac2;   // not needed

    auto IDLE__T            = fsm->create_add_signal_handler( IDLE, "T" );
    auto IDLE__T__ac1       = fsm->create_set_first_action_connector( IDLE__T, new fsm::SendSignal( "ScenPlayMessage", { fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 1 ) ) )} ));
    auto IDLE__T__ac2       = fsm->create_set_next_action_connector( IDLE__T__ac1, new fsm::NextState( PLAYING_MESSAGE ) );
    (void)IDLE__T__ac2;  // not needed

    auto IDLE__Cancel               = fsm->create_add_signal_handler( IDLE, "Cancel" );
    auto IDLE__Cancel__ac1          = fsm->create_set_first_action_connector( IDLE__Cancel,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CANCELLED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cancelled before announcement" ) ) )
            } ));
    fsm->create_set_next_action_connector( IDLE__Cancel__ac1, new fsm::Exit() );

    auto IDLE_ConnectionLost        = fsm->create_add_signal_handler( IDLE, "ConnectionLost" );
    auto IDLE_ConnectionLost__ac1   = fsm->create_set_first_action_connector( IDLE_ConnectionLost,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_ABORTED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "connection lost before announcement" ) ) )
            } ));
    fsm->create_set_next_action_connector( IDLE_ConnectionLost__ac1, new fsm::Exit() );

    auto PLAYING_MESSAGE__Cancel                = fsm->create_add_signal_handler( PLAYING_MESSAGE, "Cancel" );
    auto PLAYING_MESSAGE__Cancel__ac1           = fsm->create_set_first_action_connector( PLAYING_MESSAGE__Cancel,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CANCELLED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cancelled during announcement" ) ) )
            } ));
    fsm->create_set_next_action_connector( PLAYING_MESSAGE__Cancel__ac1, new fsm::Exit() );

    auto PLAYING_MESSAGE__ConnectionLost        = fsm->create_add_signal_handler( PLAYING_MESSAGE, "ConnectionLost" );
    auto PLAYING_MESSAGE__ConnectionLost__ac1   = fsm->create_set_first_action_connector( PLAYING_MESSAGE__ConnectionLost,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_ABORTED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "connection lost during announcement" ) ) )
            } ));
    fsm->create_set_next_action_connector( PLAYING_MESSAGE__ConnectionLost__ac1, new fsm::Exit() );

    auto PLAYING_MESSAGE__PlayFinished          = fsm->create_add_signal_handler( PLAYING_MESSAGE, "PlayFinished" );
    auto PLAYING_MESSAGE__PlayFinished__ac1     = fsm->create_set_first_action_connector( PLAYING_MESSAGE__PlayFinished,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_DONE ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "done" ) ) )
            } ));
    fsm->create_set_next_action_connector( PLAYING_MESSAGE__PlayFinished__ac1, new fsm::Exit() );

    auto PLAYING_MESSAGE__PlayFailed             = fsm->create_add_signal_handler( PLAYING_MESSAGE, "PlayFailed" );
    auto PLAYING_MESSAGE__PlayFailed__ac1        = fsm->create_set_first_action_connector( PLAYING_MESSAGE__PlayFailed,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_FAILED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cannot play sound file" ) ) )
            } ));
    fsm->create_set_next_action_connector( PLAYING_MESSAGE__PlayFailed__ac1, new fsm::Exit() );
}
