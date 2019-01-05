/*

SDL/GR graph generator for graphviz. Provides to_string() function.

Copyright (C) 2019 Sergey Kolevatov

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

// $Revision: 10406 $ $Date:: 2019-01-05 #$ $Author: serge $

#include "sdl_gr_helper.h"             // self

#include <iomanip>                  // std::setfill
#include <map>

#include "process.h"                // Process

namespace fsm {

#define TUPLE_VAL_STR(_x_)  _x_,#_x_
#define TUPLE_STR_VAL(_x_)  #_x_,_x_

std::ostream & SdlGrHelper::write_element_name( std::ostream & os, const std::string & prefix, element_id_t id )
{
    os << prefix << "_" << id;

    return os;
}

std::ostream & SdlGrHelper::write_action_connector_name( std::ostream & os, element_id_t id )
{
    return write_element_name( os, "ACTION", id );
}

std::ostream & SdlGrHelper::write_signal_handler_name( std::ostream & os, element_id_t id )
{
    return write_element_name( os, "SIGNAL_HANDLER", id );
}

std::ostream & SdlGrHelper::write_signal_handler_def( std::ostream & os, element_id_t id, const std::string & signal_name )
{
    write_signal_handler_name( os, id );

    os << " [ label=\"" << signal_name << "\" shape=sdl_input_from_right fillcolor=orange ]";

    return os;

}

std::ostream & SdlGrHelper::write_name( std::ostream & os, const State & l )
{
    return write_element_name( os, "STATE", l.get_id() );
}

std::ostream & SdlGrHelper::write( std::ostream & os, const State & l )
{
    write_name( os, l );

    os << " [ label=\"" << l.get_name() << "\" shape=sdl_state fillcolor=yellow ]";
    os << "\n";

    for( auto & e : l.map_signal_name_to_signal_handler_ids_ )
    {
        write_signal_handler_def( os, e.second, e.first );
        os << "\n";
    }

    for( auto & e : l.map_signal_name_to_signal_handler_ids_ )
    {
        write_name( os, l );
        os << " -> ";
        write_signal_handler_name( os, e.second );
        os << "\n";
    }

    return os;
}

std::ostream & SdlGrHelper::write_name( std::ostream & os, const ActionConnector & l )
{
    return write_action_connector_name( os, l.get_id() );
}

std::ostream & SdlGrHelper::write( std::ostream & os, const ActionConnector & l )
{
    write_name( os, l );

    os << "\n";

    return os;
}

std::ostream & SdlGrHelper::write( std::ostream & os, const SignalHandler & l )
{
    write_signal_handler_name( os, l.get_id() );

    os << " -> ";

    write_action_connector_name( os, l.get_first_action_id() );

    return os;
}

std::ostream & SdlGrHelper::write( std::ostream & os, const Process & l )
{
    os <<
    "digraph Process\n"
    "{\n"
    "\n"
    "label=\"Process\";\n"
    "\n"
    "size=\"7,7\";\n"
    "ratio=fill;\n"
    "splines=true;\n"
    "node [ peripheries=0 style=filled fillcolor=white ]\n"
    "\n";

    // generate start action
    os << "START [ shape=sdl_start ]\n";
    os << "START -> ";
    write_action_connector_name( os, l.start_action_connector_ );
    os << "\n";

    for( auto & e : l.map_id_to_state_ )
    {
        write( os, * e.second );
        os << "\n";
    }

    for( auto & e : l.map_id_to_action_connector_ )
    {
        write( os, * e.second );
        os << "\n";
    }

    for( auto & e : l.map_id_to_signal_handler_ )
    {
        write( os, * e.second );
        os << "\n";
    }

    os << "}\n";

    return os;
}

} // namespace fsm

