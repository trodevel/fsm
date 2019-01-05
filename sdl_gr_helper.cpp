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

// $Revision: 10390 $ $Date:: 2019-01-04 #$ $Author: serge $

#include "sdl_gr_helper.h"             // self

#include <iomanip>                  // std::setfill
#include <map>

namespace fsm {

#define TUPLE_VAL_STR(_x_)  _x_,#_x_
#define TUPLE_STR_VAL(_x_)  #_x_,_x_

std::ostream & SdlGrHelper::write_name( std::ostream & os, const State & l )
{
    os << "STATE_" << l.get_id();

    return os;
}

std::ostream & SdlGrHelper::write( std::ostream & os, const State & l )
{
    write_name( os, l );

    os << " [ label=\"" << l.get_name() << "\" shape=sdl_state fillcolor=yellow ]";

    return os;
}

} // namespace fsm

