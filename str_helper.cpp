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

// $Revision: 11612 $ $Date:: 2019-05-24 #$ $Author: serge $

#include "str_helper.h"             // self

#include "anyvalue/str_helper.h"    // anyvalue::StrHelper

namespace fsm {

std::ostream & StrHelper::write( std::ostream & os, const std::vector<fsm::Value> & l )
{
    for( auto & e : l )
    {
        anyvalue::StrHelper::write( os, e );
        os << " ";
    }

    return os;
}

std::ostream & StrHelper::write( std::ostream & os, const std::vector<fsm::Value*> & l )
{
    for( auto & e : l )
    {
        anyvalue::StrHelper::write( os, * e );
        os << " ";
    }

    return os;
}

} // namespace fsm

