#include <iostream>         // cout
#include <memory>
#include <sstream>          // stringstream

#include "utils/dummy_logger.h"     // dummy_logger::set_log_level
#include "scheduler/scheduler.h"    // Scheduler

#include "fsm_manager.h"    // FsmManager
#include "parser.h"         // Parser

class Callback: virtual public fsm::ICallback
{
public:
    Callback( fsm::IFsm * fsm_man ):
        fsm_man_( fsm_man )
    {
    }

    void handle_send_signal( uint32_t fsm_id, const std::string & name, const std::vector<fsm::Value> & arguments ) override
    {
    }

    void handle_function_call( uint32_t fsm_id, const std::string & name, const std::vector<fsm::Value*> & arguments ) override
    {
    }

    void control_thread()
    {
        std::cout << "type exit or quit to quit: " << std::endl;
        std::cout << "send <signal_name> [<arg_type> <arg_val> [<arg_type> <arg_val> [...]]]" << std::endl;

        std::string input;

        while( true )
        {
            std::cout << "enter command: ";

            std::getline( std::cin, input );

            std::cout << "your input: " << input << std::endl;

            bool b = process_input( input );

            if( b == false )
                break;
        };

        std::cout << "exiting ..." << std::endl;
    }

private:
    bool process_input( const std::string & input )
    {
        std::string cmd;

        std::stringstream stream( input );
        if( stream >> cmd )
        {
            if( cmd == "exit" || cmd == "quit" )
            {
                return false;
            }
            else if( cmd == "send" )
            {
                uint32_t    fsm_id;
                std::string name;

                stream >> fsm_id;
                if( fsm_id == 0 )
                {
                    std::cout << "ERROR: fsm id is 0" << std::endl;
                    return true;
                }

                stream >> name;

                if( name.empty() )
                {
                    std::cout << "ERROR: signal name is empty" << std::endl;
                    return true;
                }

                std::vector<fsm::Value> arguments;

                while( true )
                {
                    std::string t;
                    std::string v;

                    auto read = static_cast<bool>( stream >> t );
                    if( !read )
                        break;
                    read = static_cast<bool>( stream >> v );
                    if( !read )
                        break;

                    //std::cout << "t " << t << " v " << v << std::endl;

                    fsm::Value val;

                    auto b = fsm::Parser::to_value( & val, t, v, false );

                    if( b )
                    {
                        arguments.push_back( val );
                    }
                    else
                    {
                        std::cout << "ERROR: invalid arguments: t " << t << " v " << v << std::endl;
                    }
                }

                fsm_man_->consume( new fsm::Signal( fsm_id, name, arguments ) );
            }
            else
            {
                std::cout << "ERROR: unknown command '" << cmd << "'" << std::endl;
            }
        }
        else
        {
            std::cout << "ERROR: cannot read command" << std::endl;
        }
        return true;
    }

private:
    fsm::IFsm                   * fsm_man_;
};

void create_fsm_1( fsm::FsmManager * fsm_man )
{

}

void create_fsm_2( fsm::FsmManager * fsm_man )
{

}

int main( int argc, char **argv )
{
    std::cout << "Hello, world" << std::endl;

    dummy_logger::set_log_level( log_levels_log4j::DEBUG );

    auto log_id     = dummy_logger::register_module( "fsm_man" );
    auto log_id_fsm = dummy_logger::register_module( "fsm" );

    fsm::FsmManager fsm_man;

    Callback test( & fsm_man );

    std::string error_msg;

    scheduler::Scheduler sched( scheduler::Duration( std::chrono::milliseconds( 1 ) ) );

    dummy_logger::set_log_level( log_id,        log_levels_log4j::TRACE );

    bool b = fsm_man.init( log_id, log_id_fsm, & test, & sched, & error_msg );
    if( b == false )
    {
        std::cout << "cannot initialize fsm manager: " << error_msg << std::endl;
        return EXIT_FAILURE;
    }

    if( argc <= 1 )
    {
        std::cout << "USAGE: ./example <fsm_num>, where fsm_num is 1 or 2" << std::endl;
        return EXIT_SUCCESS;
    }

    std::string param( argv[1] );

    if( param == "1" )
        create_fsm_1( & fsm_man );
    else if( param == "2" )
        create_fsm_1( & fsm_man );
    else
    {
        std::cout << "ERROR: unsupported fsm_num = " << param << std::endl;
        return EXIT_FAILURE;
    }

    sched.run();
    fsm_man.start();

    std::vector< std::thread > tg;

    tg.push_back( std::thread( std::bind( & Callback::control_thread, & test ) ) );

    for( auto & t : tg )
        t.join();

    fsm_man.shutdown();

    sched.shutdown();

    std::cout << "Done! =)" << std::endl;

    return EXIT_SUCCESS;
}
