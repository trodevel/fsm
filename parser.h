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

// $Revision: 11611 $ $Date:: 2019-05-24 #$ $Author: serge $

#include "elements.h"      // Value...

namespace fsm
{

class Parser
{
public:

    static data_type_e to_data_type( const std::string & s, bool throw_on_error = true );
    static data_type_e to_data_type_short( const std::string & s, bool throw_on_error = true );
    static bool to_value( Value * res, const std::string & type, const std::string & value_raw, bool throw_on_error = true );

private:

    static bool to_typed_value( Value * res, data_type_e data_type, const std::string & value_raw, bool throw_on_error = true );
};

} // namespace fsm
