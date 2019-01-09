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

// $Revision: 10434 $ $Date:: 2019-01-09 #$ $Author: serge $

#include "sdl_gr_helper.h"             // self

#include <iomanip>                  // std::setfill
#include <typeindex>                // std::type_index
#include <typeinfo>
#include <unordered_map>
#include <map>
#include <cassert>

#include "process.h"                // Process

#include "str_helper.h"             // StrHelper

namespace fsm {

#define TUPLE_VAL_STR(_x_)  _x_,#_x_
#define TUPLE_STR_VAL(_x_)  #_x_,_x_

SdlGrHelper::SdlGrHelper( const Process * l ):
        process_( l )
{
}

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

std::ostream & SdlGrHelper::write_state_name( std::ostream & os, element_id_t id )
{
    return write_element_name( os, "STATE", id );
}

std::ostream & SdlGrHelper::write_signal_handler_def( std::ostream & os, element_id_t id, const std::string & signal_name )
{
    write_signal_handler_name( os, id );

    os << " [ label=\"" << signal_name << "\" shape=sdl_input_from_right fillcolor=cyan ]";

    return os;
}

std::ostream & SdlGrHelper::write_name( std::ostream & os, const State & l )
{
    return write_state_name( os, l.get_id() );
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

std::ostream & SdlGrHelper::write( std::ostream & os, const Action & l, const ActionConnector & ac )
{
    typedef SdlGrHelper Type;

    typedef std::ostream & (Type::*PPMF)( std::ostream & os, const Action & l, const ActionConnector & ac );

#define MAP_ENTRY(_v)       { typeid( _v ),        & Type::write_##_v }

    static const std::unordered_map<std::type_index, PPMF> funcs =
    {
        MAP_ENTRY( SendSignal ),
        MAP_ENTRY( SetTimer ),
        MAP_ENTRY( ResetTimer ),
        MAP_ENTRY( FunctionCall ),
        MAP_ENTRY( Task ),
        MAP_ENTRY( Condition ),
        MAP_ENTRY( SwitchCondition ),
        MAP_ENTRY( NextState ),
        MAP_ENTRY( Exit ),
    };

#undef MAP_ENTRY

    auto it = funcs.find( typeid( l ) );

    if( it == funcs.end() )
    {
        os << "# unsupported action " << typeid( l ).name() << ", action connector id " << ac.get_id() << "\n";
        return os;
    }

    return (this->*it->second)( os, l, ac );
}

std::ostream & SdlGrHelper::write_SendSignal( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const SendSignal &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << a.name << "\" shape=sdl_output_to_left fillcolor=orange ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_next_id() );

    return os;
}

std::ostream & SdlGrHelper::write_SetTimer( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const SetTimer &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << a.timer_id << "\" shape=sdl_set ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_next_id() );

    return os;
}

std::ostream & SdlGrHelper::write_ResetTimer( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const ResetTimer &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << a.timer_id << "\" shape=sdl_reset ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_next_id() );

    return os;
}

std::ostream & SdlGrHelper::write_FunctionCall( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const FunctionCall &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << a.name << "\" shape=sdl_call ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_next_id() );

    return os;
}

std::ostream & SdlGrHelper::write_Task( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const Task &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << a.variable_id << "\" shape=sdl_task ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_next_id() );

    return os;
}

std::ostream & SdlGrHelper::write_Condition( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const Condition &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << StrHelper::to_string_short( a.type ) << "\" shape=diamond peripheries=1 ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_next_id(), "Y" );
    os << "\n";
    write_edge( os, ac.get_id(), ac.get_alt_next_id(), "N" );

    return os;
}

std::ostream & SdlGrHelper::write_SwitchCondition( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    auto & a = dynamic_cast< const SwitchCondition &>( aa );

    write_name( os, ac );

    os << " [ label=\"" << "cond" << "\" shape=diamond peripheries=1 ]" << "\n";

    write_edge( os, ac.get_id(), ac.get_default_switch_action(), "default" );
    os << "\n";

    auto & actions = ac.get_switch_actions();

    for( auto e : actions )
    {
        write_edge( os, ac.get_id(), e );
        os << "\n";
    }

    return os;
}

std::ostream & SdlGrHelper::write_NextState( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    // do nothing, case is handled by write_edge()

    return os;
}

std::ostream & SdlGrHelper::write_Exit( std::ostream & os, const Action & aa, const ActionConnector & ac )
{
    write_name( os, ac );

    os << " [ label=\"\" shape=sdl_stop peripheries=1 ]" << "\n";

    return os;
}

std::ostream & SdlGrHelper::write( std::ostream & os, const ActionConnector & l )
{
    write( os, * l.get_action(), l );

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

std::ostream & SdlGrHelper::write( std::ostream & os )
{
    generate_map_of_next_state_actions();

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
    write_action_connector_name( os, process_->start_action_connector_ );
    os << "\n";

    for( auto & e : process_->map_id_to_state_ )
    {
        write( os, * e.second );
        os << "\n";
    }

    for( auto & e : process_->map_id_to_action_connector_ )
    {
        write( os, * e.second );
        os << "\n";
    }

    for( auto & e : process_->map_id_to_signal_handler_ )
    {
        write( os, * e.second );
        os << "\n";
    }

    os << "}\n";

    return os;
}

std::ostream & SdlGrHelper::write_edge( std::ostream & os, element_id_t action_connector_id_1, element_id_t action_connector_id_2, const std::string & comment )
{
    write_action_connector_name( os, action_connector_id_1 );

    os << " -> ";

    element_id_t state_id;

    if( is_action_next_state( action_connector_id_2, & state_id ) )
    {
        write_state_name( os, state_id );
    }
    else
    {
        write_action_connector_name( os, action_connector_id_2 );
    }

    if( ! comment.empty() )
    {
        os << " [ label=\"" << comment << "\" ]";
    }

    return os;
}

void SdlGrHelper::generate_map_of_next_state_actions()
{
    for( auto & e : process_->map_id_to_action_connector_ )
    {
        auto & ac   = * e.second;

        auto & aa = * ac.get_action();

        if( typeid( aa ) == typeid( NextState ) )
        {
            auto & a = dynamic_cast< const NextState &>( aa );

            auto state_id = a.state_id;

            auto b = map_next_state_action_to_state_id_.insert( std::make_pair( e.first, state_id ) ).second;

            assert( b );
        }
    }
}

bool SdlGrHelper::is_action_next_state( element_id_t action_connector_id, element_id_t * state_id ) const
{
    auto it = map_next_state_action_to_state_id_.find( action_connector_id );

    if( it != map_next_state_action_to_state_id_.end() )
    {
        * state_id = it->second;

        return true;
    }

    return false;
}


} // namespace fsm

