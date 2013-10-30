/*
 * This file is part of mcproxy.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * written by Sebastian Woelke, in cooperation with:
 * INET group, Hamburg University of Applied Sciences,
 * Website: http://mcproxy.realmv6.org/
 */

#ifndef SIMPLE_ROUTING_DATA_HPP 
#define SIMPLE_ROUTING_DATA_HPP

#include "include/proxy/def.hpp"
#include <map>
#include <memory>
#include <string>
#include <set>

class addr_storage;
struct source;
struct timer_msg;
class mroute_socket;


using s_routing_data = std::map<addr_storage,source_list<source>>;
using s_routing_data_pair = std::pair<addr_storage,source_list<source>>;

class simple_routing_data 
{
private:
    s_routing_data m_data;
    group_mem_protocol m_group_mem_protocol;
    std::shared_ptr<mroute_socket> m_mrt_sock;
    unsigned long get_current_packet_count(const addr_storage& gaddr, const addr_storage& saddr);
public:
    simple_routing_data(group_mem_protocol group_mem_protocol, std::shared_ptr<mroute_socket> mrt_sock);

    void set_source(const addr_storage& gaddr, const source& saddr);

    void del_source(const addr_storage& gaddr, const addr_storage& saddr);

    //return true if the source has been refreshed 
    //iterator of the refrehed source
    std::pair<source_list<source>::iterator, bool> refresh_source_or_del_it_if_unused(const addr_storage& gaddr, const addr_storage& saddr);

    source_list<source> get_available_sources(const addr_storage& gaddr, const source_list<source>& slist) const;

    std::string to_string() const;
    friend std::ostream& operator<<(std::ostream& stream, const simple_routing_data& srd); 

    static void test_simple_routing_data();
};



#endif // SIMPLE_ROUTING_DATA_HPP