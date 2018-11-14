#include <iostream>         // cout

#include "utils/dummy_logger.h"         // dummy_logger::set_log_level

int main()
{
    std::cout << "Hello, world" << std::endl;

    dummy_logger::set_log_level( log_levels_log4j::DEBUG );

    auto log_id = dummy_logger::register_module( "fsm" );

    return 0;
}
