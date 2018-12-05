#include <iostream>         // cout
#include <memory>
#include <sstream>          // stringstream

#include "utils/dummy_logger.h"         // dummy_logger::set_log_level

#include "fsm.h"            // Fsm
#include "parser.h"         // Parser

class Callback: virtual public fsm::ICallback
{
public:
    Callback( fsm::IFsm * dummy ):
        fsm_( dummy )
    {
    }

    void handle_send_signal( const std::string & name, const std::vector<fsm::Value> & arguments ) override
    {
    }

    void handle_function_call( const std::string & name, const std::vector<fsm::Value*> & arguments ) override
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
                std::string name;
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

                    bool read = ( stream >> t );
                    if( !read )
                        break;
                    read = ( stream >> v );
                    if( !read )
                        break;

                    std::cout << "t " << t << " v " << v << std::endl;

                    fsm::Value val;

                    fsm::Parser::to_value( & val, t, v );

                    arguments.push_back( val );
                }

                fsm_->consume( new fsm::Signal( name, arguments ) );
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
    fsm::IFsm                   * fsm_;
};

int main()
{
    std::cout << "Hello, world" << std::endl;

    dummy_logger::set_log_level( log_levels_log4j::DEBUG );

    auto log_id = dummy_logger::register_module( "fsm" );

    std::unique_ptr<fsm::Fsm> fsm( new fsm::Fsm( log_id ) );

    Callback test( fsm.get() );

    fsm->init( & test );

    std::string error_msg;

//    scheduler::Scheduler sched( scheduler::Duration( std::chrono::milliseconds( 1 ) ) );

    dummy_logger::set_log_level( log_id,        log_levels_log4j::TRACE );

//    sched.run();
    fsm->start();

    std::vector< std::thread > tg;

    tg.push_back( std::thread( std::bind( &Callback::control_thread, &test ) ) );

    for( auto & t : tg )
        t.join();

    fsm->shutdown();

//    sched.shutdown();

    std::cout << "Done! =)" << std::endl;

    return 0;
}
