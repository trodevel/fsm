#include <iostream>         // cout
#include <memory>
#include <sstream>          // stringstream
#include <cassert>          // assert
#include <fstream>          // ofstream

#include "utils/mutex_helper.h"     // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"     // dummy_logger::set_log_level
#include "scheduler/scheduler.h"    // Scheduler

#include "fsm_manager.h"    // FsmManager
#include "parser.h"         // Parser
#include "str_helper.h"     // StrHelper
#include "sdl_gr_helper.h"  // SdlGrHelper

class Callback: virtual public fsm::ICallback
{
public:
    Callback( fsm::IFsm * fsm_man ):
        fsm_man_( fsm_man )
    {
    }

    void handle_send_signal( uint32_t process_id, const std::string & name, const std::vector<fsm::Value> & arguments ) override
    {
        std::cout << "got signal from process " << process_id << " " << name << " "
                << fsm::StrHelper::to_string( arguments ) << std::endl;
    }

    void handle_function_call( uint32_t process_id, const std::string & name, const std::vector<fsm::Value*> & arguments ) override
    {
        std::cout << "got function call from process " << process_id << " " << name << " "
                << fsm::StrHelper::to_string( arguments ) << std::endl;

        if( name == "convert_tone_to_action" )
        {
            convert_tone_to_action__wrap( arguments );
        }
        else
        {
            std::cout << "unhandled function " << name << std::endl;
        }
    }

    void control_thread()
    {
        std::cout << "type exit or q[uit] to quit: " << std::endl;
        std::cout << "s[end] <process_id> <signal_name> [<arg_type> <arg_val> [<arg_type> <arg_val> [...]]]" << std::endl;

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
            if( cmd == "exit" || cmd == "quit" || cmd == "q" )
            {
                return false;
            }
            else if( cmd == "send" || cmd == "s" )
            {
                uint32_t    process_id;
                std::string name;

                stream >> process_id;
                if( process_id == 0 )
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

                fsm_man_->consume( new fsm::ev::Signal( process_id, name, arguments ) );
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

    void convert_tone_to_action__wrap( const std::vector<fsm::Value*> & arguments )
    {
        assert( arguments.size() == 3 );

        int tone = arguments.at( 0 )->arg_i;
        int action;
        int action_message;

        convert_tone_to_action( tone, & action, & action_message );

        anyvalue::assign( arguments.at( 1 ), fsm::Value( action ) );
        anyvalue::assign( arguments.at( 2 ), fsm::Value( action_message ) );
    }

    void convert_tone_to_action( int tone, int * action, int * action_message )
    {
        static const int NONE   = 0;
        static const int REPEAT = 1;
        static const int DROP   = 2;

        typedef int Type;
        static const std::map< Type, std::pair<int,int> > m =
        {
            { 1, std::make_pair( DROP, 1 ) },
            { 2, std::make_pair( DROP, 2 ) },
            { 3, std::make_pair( DROP, 3 ) },
            { 4, std::make_pair( REPEAT, 0 ) },
        };

        auto it = m.find( tone );

        if( it != m.end() )
        {
            * action            = it->second.first;
            * action_message    = it->second.second;
            return;
        }

        * action            = NONE;
        * action_message    = 0;
    }

private:
    fsm::IFsm                   * fsm_man_;
};

void init_fsm_1( fsm::Process * fsm );
void init_fsm_2( fsm::Process * fsm );
void init_fsm_3( fsm::Process * fsm );
void init_fsm_4( fsm::Process * fsm );

bool init_fsm( fsm::Process * fsm, unsigned fsm_num )
{
    switch( fsm_num )
    {
    case 1:
        init_fsm_1( fsm );
        break;

    case 2:
        init_fsm_2( fsm );
        break;

    case 3:
        init_fsm_3( fsm );
        break;

    case 4:
        init_fsm_4( fsm );
        break;

    default:
        return false;
        break;
    }

    return true;
}

bool create_fsm( fsm::FsmManager * fsm_man, uint32_t * process_id, unsigned fsm_num )
{
    auto id = fsm_man->create_process();

    auto & mutex = fsm_man->get_mutex();

    MUTEX_SCOPE_LOCK( mutex );

    auto fsm = fsm_man->find_process( id );

    assert( fsm );

    * process_id    = id;

    return init_fsm( fsm, fsm_num );
}

int main( int argc, char **argv )
{
    std::cout << "Hello, world" << std::endl;

    dummy_logger::set_log_level( log_levels_log4j::Debug );

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
        std::cout << "USAGE: ./example <fsm_num> [--sdlgr], where fsm_num is 1, 2 or 3" << std::endl;
        return EXIT_SUCCESS;
    }

    bool make_sdl_graph = false;

    if( argc == 3 )
    {
        std::string arg2    = argv[2];

        if( arg2 == "--sdlgr" )
        {
            make_sdl_graph  = true;
        }
        else
        {
            std::cout << "ERROR: unsupported param = " << arg2 << std::endl;
            return EXIT_FAILURE;
        }
    }

    unsigned fsm_num = std::stoi( argv[1] );

    uint32_t process_id;

    b = create_fsm( & fsm_man, & process_id, fsm_num );

    if( b == false )
    {
        std::cout << "ERROR: unsupported fsm_num = " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    if( make_sdl_graph )
    {
        std::string name        = "process_" + std::to_string( fsm_num );
        std::string file_name   = name + ".gv";

        std::cout << "generating SDL/GR graph - " << file_name << std::endl;

        std::ofstream out( file_name );

        out << "# generated by fsm\n";
        out << "# execute: FL=" << name << "; dot -l sdl.ps -Tps $FL.gv -o $FL.ps; ps2pdf $FL.ps $FL.pdf\n";
        out << "\n";

        fsm::SdlGrHelper( fsm_man.find_process( process_id ) ).write( out );

        out.close();

        return EXIT_SUCCESS;
    }

    sched.run();
    fsm_man.start();

    fsm_man.start_process( process_id );

    std::vector< std::thread > tg;

    tg.push_back( std::thread( std::bind( & Callback::control_thread, & test ) ) );

    for( auto & t : tg )
        t.join();

    fsm_man.shutdown();

    sched.shutdown();

    std::cout << "Done! =)" << std::endl;

    return EXIT_SUCCESS;
}
