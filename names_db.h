/*

FSM. Names DB.

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

// $Revision: 10198 $ $Date:: 2018-12-15 #$ $Author: serge $

#ifndef LIB_FSM__NAMES_DB_H
#define LIB_FSM__NAMES_DB_H

#include <map>                  // std::map

#include "elements.h"           // element_id_t

namespace fsm {

class NamesDb
{
public:
    NamesDb(
            uint32_t                id,
            uint32_t                log_id );
    ~NamesDb();

    void add_name( element_id_t id, const std::string & name );
    const std::string & get_name( element_id_t id );
    element_id_t find_element( const std::string & name ) const;
    bool delete_name( element_id_t id );

private:
    typedef std::map<element_id_t,std::string>      MapIdToString;
    typedef std::map<std::string,element_id_t>      MapStringToId;

private:
    NamesDb( const NamesDb & )              = delete;
    NamesDb & operator=( const NamesDb & )  = delete;

private:

    uint32_t                    id_;
    uint32_t                    log_id_;

    MapIdToString               map_id_to_name_;
    MapStringToId               map_name_to_id_;
};

} // namespace fsm

#endif // LIB_FSM__NAMES_DB_H
