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

// $Revision: 10048 $ $Date:: 2018-12-05 #$ $Author: serge $

#include "parser.h"             // self

#include <map>                  // std::map
#include <stdexcept>            // std::invalid_argument

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

bool Parser::to_value( Value * res, const std::string & type, const std::string value_raw, bool throw_on_error )
{
}

} // namespace fsm
