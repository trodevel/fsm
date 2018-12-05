/*

Parser.

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

// $Revision: 10051 $ $Date:: 2018-12-05 #$ $Author: serge $

#include "parser.h"             // self

#include <map>                  // std::map
#include <stdexcept>            // std::invalid_argument

#include "utils/to_value.h"     // to_value

namespace fsm
{

#define TUPLE_VAL_STR(_x_)  _x_,#_x_
#define TUPLE_STR_VAL(_x_)  #_x_,_x_

template< typename _U, typename _V >
std::pair<_V,_U> make_inverse_pair( _U first, _V second )
{
    return std::make_pair( second, first );
}

data_type_e Parser::to_data_type( const std::string & s, bool throw_on_error )
{
    typedef data_type_e Type;

    typedef std::map< std::string, Type > Map;
    static const Map m =
    {
        make_inverse_pair( Type:: TUPLE_VAL_STR( UNDEF ) ),
        make_inverse_pair( Type:: TUPLE_VAL_STR( BOOL ) ),
        make_inverse_pair( Type:: TUPLE_VAL_STR( INT ) ),
        make_inverse_pair( Type:: TUPLE_VAL_STR( DOUBLE ) ),
        make_inverse_pair( Type:: TUPLE_VAL_STR( STRING ) ),
    };

    auto it = m.find( s );

    if( it == m.end() )
    {
        if( throw_on_error )
            throw std::invalid_argument( "to_data_type: cannot parse " + s );
        else
            return Type:: UNDEF;
    }

    return it->second;
}

data_type_e Parser::to_data_type_short( const std::string & s, bool throw_on_error )
{
    typedef data_type_e Type;

    typedef std::map< std::string, Type > Map;
    static const Map m =
    {
            { "U", Type:: UNDEF  },
            { "B", Type:: BOOL  },
            { "I", Type:: INT  },
            { "F", Type:: DOUBLE  },
            { "S", Type:: STRING  },
    };

    auto it = m.find( s );

    if( it == m.end() )
    {
        if( throw_on_error )
            throw std::invalid_argument( "to_data_type_short: cannot parse " + s );
        else
            return Type:: UNDEF;
    }

    return it->second;
}

bool Parser::to_value( Value * res, const std::string & type_raw, const std::string & value_raw, bool throw_on_error )
{
    auto data_type  = to_data_type_short( type_raw, throw_on_error );

    if( data_type == data_type_e::UNDEF )
        return false;

    auto b = to_typed_value( res, data_type, value_raw, throw_on_error );

    return b;
}

bool Parser::to_typed_value( Value * res, data_type_e data_type, const std::string & value_raw, bool throw_on_error )
{
    if( data_type == data_type_e::UNDEF )
    {
        if( throw_on_error )
            throw std::invalid_argument( "to_typed_value: undefined data type" );
        else
            return false;
    }

    res->type   = data_type;

    switch( data_type )
    {
    case data_type_e::BOOL:
        utils::to_value( & res->arg_b, value_raw );
        break;
    case data_type_e::INT:
        utils::to_value( & res->arg_i, value_raw );
        break;
    case data_type_e::DOUBLE:
        utils::to_value( & res->arg_d, value_raw );
        break;
    case data_type_e::STRING:
        res->arg_s  = value_raw;
        break;
    default:
        throw std::invalid_argument( "to_typed_value: should not happen" );
        break;
    }

    harmonize( res );

    return true;
}

} // namespace fsm
