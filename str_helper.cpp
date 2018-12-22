/*

String Helper. Provides to_string() function.

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

// $Revision: 10315 $ $Date:: 2018-12-21 #$ $Author: serge $

#include "str_helper.h"             // self

#include <iomanip>                  // std::setfill
#include <map>

namespace fsm {

#define TUPLE_VAL_STR(_x_)  _x_,#_x_
#define TUPLE_STR_VAL(_x_)  #_x_,_x_

const std::string & StrHelper::to_string( const data_type_e s )
{
    typedef data_type_e Type;
    static const std::map< Type, std::string > m =
    {
        { Type:: TUPLE_VAL_STR( UNDEF ) },
        { Type:: TUPLE_VAL_STR( BOOL ) },
        { Type:: TUPLE_VAL_STR( INT ) },
        { Type:: TUPLE_VAL_STR( DOUBLE ) },
        { Type:: TUPLE_VAL_STR( STRING ) },
    };

    auto it = m.find( s );

    static const std::string undef( "undef" );

    if( it == m.end() )
        return undef;

    return it->second;
}

const std::string & StrHelper::to_string_short( const data_type_e s )
{
    typedef data_type_e Type;
    static const std::map< Type, std::string > m =
    {
        { Type:: UNDEF,     "u" },
        { Type:: BOOL,      "b" },
        { Type:: INT,       "i" },
        { Type:: DOUBLE,    "d" },
        { Type:: STRING,    "s" },
    };

    auto it = m.find( s );

    static const std::string undef( "undef" );

    if( it == m.end() )
        return undef;

    return it->second;
}

std::ostream & StrHelper::write( std::ostream & os, const Value & l )
{
    os << to_string_short( l.type ) << " ";

    switch( l.type )
    {
    case data_type_e::UNDEF:
        os << "?";
        break;
    case data_type_e::BOOL:
        os << (unsigned)l.arg_b;
        break;
    case data_type_e::INT:
        os << l.arg_i;
        break;
    case data_type_e::DOUBLE:
        os << l.arg_d;
        break;
    case data_type_e::STRING:
        os << "'" << l.arg_s << "'";
        break;
    default:
        break;
    }

    return os;
}

} // namespace fsm

