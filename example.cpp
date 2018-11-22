#include <iostream>         // cout
#include <memory>

#include "utils/dummy_logger.h"         // dummy_logger::set_log_level

#include "fsm.h"            // Fsm

int main()
{
    std::cout << "Hello, world" << std::endl;

    dummy_logger::set_log_level( log_levels_log4j::DEBUG );

    auto log_id = dummy_logger::register_module( "fsm" );

    std::unique_ptr<fsm::Fsm> fsm( new fsm::Fsm( log_id ) );

    return 0;
}
