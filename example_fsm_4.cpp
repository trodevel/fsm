#include <iostream>         // cout

#include "process.h"        // Process

void init_fsm_4( fsm::Process * fsm )
{
    auto timer              = fsm->create_add_timer( "T" );
    auto E_DONE             = fsm->create_add_constant( "DONE",         fsm::data_type_e::INT, fsm::Value( 0 ) );
    auto E_CANCELLED        = fsm->create_add_constant( "CANCELLED",    fsm::data_type_e::INT, fsm::Value( 1 ) );
    auto E_IO_ERROR         = fsm->create_add_constant( "IO_ERROR",     fsm::data_type_e::INT, fsm::Value( 2 ) );
    auto E_CONN_LOST        = fsm->create_add_constant( "CONN_LOST",    fsm::data_type_e::INT, fsm::Value( 3 ) );

    auto A_NONE             = fsm->create_add_constant( "NONE",     fsm::data_type_e::INT, fsm::Value( 0 ) );
    auto A_REPEAT           = fsm->create_add_constant( "REPEAT",   fsm::data_type_e::INT, fsm::Value( 1 ) );
    auto A_DROP             = fsm->create_add_constant( "DROP",     fsm::data_type_e::INT, fsm::Value( 2 ) );

    auto response           = fsm->create_add_variable( "response",         fsm::data_type_e::INT );
    auto action             = fsm->create_add_variable( "action",           fsm::data_type_e::INT );
    auto action_message     = fsm->create_add_variable( "action_message",   fsm::data_type_e::INT );

    auto IDLE               = fsm->create_state( "IDLE" );
    auto PLAYING_MESSAGE_1  = fsm->create_state( "PLAYING_MESSAGE_1" );
    auto PLAYING_MESSAGE_2  = fsm->create_state( "PLAYING_MESSAGE_2" );
    auto WAITING_ACTION     = fsm->create_state( "WAITING_ACTION" );
    auto PLAYING_MESSAGE_ACTION  = fsm->create_state( "PLAYING_MESSAGE_ACTION" );

    auto st_ac1             = fsm->create_add_start_action_connector( new fsm::SetTimer( timer, fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 1 ) ) ) ) );
    fsm->create_set_next_action_connector( st_ac1, new fsm::NextState( IDLE ) );

    // IDLE

    auto IDLE__T            = fsm->create_add_signal_handler( IDLE, "T" );
    auto IDLE__T__ac1       = fsm->create_set_first_action_connector( IDLE__T, new fsm::SendSignal( "ScenPlayMessage", { fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 1 ) ) )} ));
    fsm->create_set_next_action_connector( IDLE__T__ac1, new fsm::NextState( PLAYING_MESSAGE_1 ) );

    auto IDLE__Cancel               = fsm->create_add_signal_handler( IDLE, "Cancel" );
    auto IDLE__Cancel__ac1          = fsm->create_set_first_action_connector( IDLE__Cancel,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CANCELLED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cancelled before announcement" ) ) )
            } ));
    auto IDLE__Cancel__ac2          = fsm->create_set_next_action_connector( IDLE__Cancel__ac1, new fsm::Exit() );

    auto IDLE_ConnectionLost        = fsm->create_add_signal_handler( IDLE, "ConnectionLost" );
    auto IDLE_ConnectionLost__ac1   = fsm->create_set_first_action_connector( IDLE_ConnectionLost,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CONN_LOST ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "connection lost before announcement" ) ) )
            } ));
    fsm->set_next_action_connector( IDLE_ConnectionLost__ac1, IDLE__Cancel__ac2 );

    // PLAYING_MESSAGE_1

    auto PLAYING_MESSAGE_1__Cancel                = fsm->create_add_signal_handler( PLAYING_MESSAGE_1, "Cancel" );
    auto PLAYING_MESSAGE_1__Cancel__ac1           = fsm->create_set_first_action_connector( PLAYING_MESSAGE_1__Cancel,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CANCELLED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cancelled during announcement" ) ) )
            } ));
    auto PLAYING_MESSAGE_1__Cancel__ac2             = fsm->create_set_next_action_connector( PLAYING_MESSAGE_1__Cancel__ac1, new fsm::Exit() );

    auto PLAYING_MESSAGE_1__PlayFinished            = fsm->create_add_signal_handler( PLAYING_MESSAGE_1, "PlayFinished" );
    auto PLAYING_MESSAGE_1__PlayFinished__ac1       = fsm->create_set_first_action_connector( PLAYING_MESSAGE_1__PlayFinished, new fsm::SetTimer( timer, fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 1 ) ) ) ) );
    fsm->create_set_next_action_connector( PLAYING_MESSAGE_1__PlayFinished__ac1, new fsm::NextState( PLAYING_MESSAGE_1 ) );

    auto PLAYING_MESSAGE_1__PlayFailed             = fsm->create_add_signal_handler( PLAYING_MESSAGE_1, "PlayFailed" );
    auto PLAYING_MESSAGE_1__PlayFailed__ac1        = fsm->create_set_first_action_connector( PLAYING_MESSAGE_1__PlayFailed,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_IO_ERROR ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cannot play sound file" ) ) )
            } ));
    fsm->set_next_action_connector( PLAYING_MESSAGE_1__PlayFailed__ac1, PLAYING_MESSAGE_1__Cancel__ac2 );

    auto PLAYING_MESSAGE_1__ConnectionLost        = fsm->create_add_signal_handler( PLAYING_MESSAGE_1, "ConnectionLost" );
    auto PLAYING_MESSAGE_1__ConnectionLost__ac1   = fsm->create_set_first_action_connector( PLAYING_MESSAGE_1__ConnectionLost,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CONN_LOST ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "connection lost during announcement" ) ) )
            } ));
    fsm->set_next_action_connector( PLAYING_MESSAGE_1__ConnectionLost__ac1, PLAYING_MESSAGE_1__Cancel__ac2 );

    auto PLAYING_MESSAGE_1__T            = fsm->create_add_signal_handler( PLAYING_MESSAGE_1, "T" );
    auto PLAYING_MESSAGE_1__T__ac1       = fsm->create_set_first_action_connector( PLAYING_MESSAGE_1__T, new fsm::SendSignal( "ScenPlayMessage", { fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 2 ) ) )} ));
    fsm->create_set_next_action_connector( PLAYING_MESSAGE_1__T__ac1, new fsm::NextState( PLAYING_MESSAGE_2 ) );

    // PLAYING_MESSAGE_2

    auto PLAYING_MESSAGE_2__Cancel              = fsm->create_add_signal_handler( PLAYING_MESSAGE_2, "Cancel" );
    auto PLAYING_MESSAGE_2__Cancel__ac1         = fsm->create_set_first_action_connector( PLAYING_MESSAGE_2__Cancel,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CANCELLED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cancelled during announcement" ) ) )
            } ));
    auto PLAYING_MESSAGE_2__Cancel__ac2         = fsm->create_set_next_action_connector( PLAYING_MESSAGE_2__Cancel__ac1, new fsm::Exit() );

    auto PLAYING_MESSAGE_2__PlayFinished        = fsm->create_add_signal_handler( PLAYING_MESSAGE_2, "PlayFinished" );
    auto PLAYING_MESSAGE_2__PlayFinished__ac1   = fsm->create_set_first_action_connector( PLAYING_MESSAGE_2__PlayFinished, new fsm::SetTimer( timer, fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 1 ) ) ) ) );
    fsm->create_set_next_action_connector( PLAYING_MESSAGE_2__PlayFinished__ac1, new fsm::NextState( PLAYING_MESSAGE_2 ) );

    auto PLAYING_MESSAGE_2__PlayFailed          = fsm->create_add_signal_handler( PLAYING_MESSAGE_2, "PlayFailed" );
    auto PLAYING_MESSAGE_2__PlayFailed__ac1     = fsm->create_set_first_action_connector( PLAYING_MESSAGE_2__PlayFailed,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_IO_ERROR ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cannot play sound file" ) ) )
            } ));
    fsm->set_next_action_connector( PLAYING_MESSAGE_2__PlayFailed__ac1, PLAYING_MESSAGE_2__Cancel__ac2 );

    auto PLAYING_MESSAGE_2__ConnectionLost      = fsm->create_add_signal_handler( PLAYING_MESSAGE_2, "ConnectionLost" );
    auto PLAYING_MESSAGE_2__ConnectionLost__ac1 = fsm->create_set_first_action_connector( PLAYING_MESSAGE_2__ConnectionLost,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CONN_LOST ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "connection lost during announcement" ) ) )
            } ));
    fsm->set_next_action_connector( PLAYING_MESSAGE_2__ConnectionLost__ac1, PLAYING_MESSAGE_2__Cancel__ac2 );

    auto PLAYING_MESSAGE_2__T           = fsm->create_add_signal_handler( PLAYING_MESSAGE_2, "T" );
    auto PLAYING_MESSAGE_2__T__ac1      = fsm->create_set_first_action_connector( PLAYING_MESSAGE_2__T, new fsm::SetTimer( timer, fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( 15 ) ) ) ) );
    fsm->create_set_next_action_connector( PLAYING_MESSAGE_2__T__ac1, new fsm::NextState( WAITING_ACTION ) );

    // WAITING_ACTION

    auto WAITING_ACTION__Cancel         = fsm->create_add_signal_handler( WAITING_ACTION, "Cancel" );
    auto WAITING_ACTION__Cancel__ac1    = fsm->create_set_first_action_connector( WAITING_ACTION__Cancel,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CANCELLED ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "cancelled during waiting" ) ) )
            } ));
    auto WAITING_ACTION__Cancel__ac2    = fsm->create_set_next_action_connector( WAITING_ACTION__Cancel__ac1, new fsm::Exit() );

    auto WAITING_ACTION__ConnectionLost         = fsm->create_add_signal_handler( WAITING_ACTION, "ConnectionLost" );
    auto WAITING_ACTION__ConnectionLost__ac1    = fsm->create_set_first_action_connector( WAITING_ACTION__ConnectionLost,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_CONN_LOST ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "connection lost during waiting" ) ) )
            } ));
    fsm->set_next_action_connector( WAITING_ACTION__ConnectionLost__ac1, WAITING_ACTION__Cancel__ac2 );

    auto WAITING_ACTION__TONE           = fsm->create_add_signal_handler( WAITING_ACTION, "TONE" );
    auto WAITING_ACTION__TONE__ac1      = fsm->create_set_first_action_connector( WAITING_ACTION__TONE,
            new fsm::Task(
                    response,
                    fsm::ExpressionPtr( new fsm::ExpressionVariableName( "$1" ) ) ) );

    auto WAITING_ACTION__TONE__ac2      = fsm->create_set_next_action_connector( WAITING_ACTION__TONE__ac1,
            new fsm::FunctionCall(
                    "convert_tone_to_action",
                    {
                            std::make_pair( false, fsm::ExpressionPtr( new fsm::ExpressionVariable( response ) ) ),
                            std::make_pair( true, fsm::ExpressionPtr( new fsm::ExpressionVariable( action ) ) ),
                            std::make_pair( true, fsm::ExpressionPtr( new fsm::ExpressionVariable( action_message ) ) ),
                    }
    ) );

    auto WAITING_ACTION__TONE__ac3      = fsm->create_set_next_action_connector( WAITING_ACTION__TONE__ac2,
            new fsm::SwitchCondition(
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( action ) ),
                    {
                        fsm::ExpressionPtr( new fsm::ExpressionVariable( A_NONE ) ),
                        fsm::ExpressionPtr( new fsm::ExpressionVariable( A_REPEAT ) ),
                        fsm::ExpressionPtr( new fsm::ExpressionVariable( A_DROP ) )
                    }
    ) );

    fsm->create_set_default_switch_action_connector( WAITING_ACTION__TONE__ac3, new fsm::NextState( WAITING_ACTION ) );
    fsm->create_add_switch_action_connector( WAITING_ACTION__TONE__ac3, new fsm::NextState( WAITING_ACTION ) );
    fsm->create_add_switch_action_connector( WAITING_ACTION__TONE__ac3, new fsm::NextState( WAITING_ACTION ) );

    auto WAITING_ACTION__TONE__ac4      = fsm->create_add_switch_action_connector( WAITING_ACTION__TONE__ac3, new fsm::SendSignal( "ScenPlayMessage", { fsm::ExpressionPtr( new fsm::ExpressionVariable( action_message ) )} ));
    fsm->create_set_next_action_connector( WAITING_ACTION__TONE__ac4, new fsm::NextState( PLAYING_MESSAGE_ACTION ) );

    auto WAITING_ACTION__T          = fsm->create_add_signal_handler( WAITING_ACTION, "T" );
    auto WAITING_ACTION__T__ac1     = fsm->create_set_first_action_connector( WAITING_ACTION__T,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_DONE ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "timeout" ) ) )
            } ));
    fsm->set_next_action_connector( WAITING_ACTION__T__ac1, WAITING_ACTION__Cancel__ac2 );

    // PLAYING_MESSAGE_ACTION

    auto PLAYING_MESSAGE_ACTION__PlayFinished           = fsm->create_add_signal_handler( PLAYING_MESSAGE_ACTION, "PlayFinished" );
    auto PLAYING_MESSAGE_ACTION__PlayFinished__ac1      = fsm->create_set_first_action_connector( PLAYING_MESSAGE_ACTION__PlayFinished,
            new fsm::SendSignal( "ScenFeedbackInt",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( response ) )
            } ));
    auto PLAYING_MESSAGE_ACTION__PlayFinished__ac2      = fsm->create_set_next_action_connector( PLAYING_MESSAGE_ACTION__PlayFinished__ac1,
            new fsm::SendSignal( "ScenExit",
            {
                    fsm::ExpressionPtr( new fsm::ExpressionVariable( E_DONE ) ),
                    fsm::ExpressionPtr( new fsm::ExpressionValue( fsm::Value( "done" ) ) )
            } ));
    fsm->create_set_next_action_connector( PLAYING_MESSAGE_ACTION__PlayFinished__ac2, new fsm::Exit() );

    // despite failed play we still proceed like it was a PlayFinished
    auto PLAYING_MESSAGE_ACTION__PlayFailed             = fsm->create_add_signal_handler( PLAYING_MESSAGE_ACTION, "PlayFailed" );
    fsm->set_first_action_connector( PLAYING_MESSAGE_ACTION__PlayFailed, PLAYING_MESSAGE_ACTION__PlayFinished__ac1 );

    auto PLAYING_MESSAGE_ACTION__Cancel         = fsm->create_add_signal_handler( PLAYING_MESSAGE_ACTION, "Cancel" );
    fsm->set_first_action_connector( PLAYING_MESSAGE_ACTION__Cancel, PLAYING_MESSAGE_ACTION__PlayFinished__ac1 );

    auto PLAYING_MESSAGE_ACTION__ConnectionLost         = fsm->create_add_signal_handler( PLAYING_MESSAGE_ACTION, "ConnectionLost" );
    fsm->set_first_action_connector( PLAYING_MESSAGE_ACTION__ConnectionLost, PLAYING_MESSAGE_ACTION__PlayFinished__ac1 );

}
