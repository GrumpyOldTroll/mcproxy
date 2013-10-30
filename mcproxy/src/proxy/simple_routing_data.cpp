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

#include "include/hamcast_logging.h"
#include "include/proxy/simple_routing_data.hpp"
#include "include/proxy/message_format.hpp"
#include "include/utils/mroute_socket.hpp"

simple_routing_data::simple_routing_data(group_mem_protocol group_mem_protocol, std::shared_ptr<mroute_socket> mrt_sock)
    : m_group_mem_protocol(group_mem_protocol)
    , m_mrt_sock(mrt_sock)
{
    HC_LOG_TRACE("");
}

unsigned long simple_routing_data::get_current_packet_count(const addr_storage& gaddr, const addr_storage& saddr)
{
    HC_LOG_TRACE("");

    if (is_IPv4(m_group_mem_protocol)) {
        struct sioc_sg_req tmp_stat;
        if (m_mrt_sock->get_mroute_stats(saddr, gaddr, &tmp_stat, nullptr)) {
            return tmp_stat.pktcnt;
        } else {
            return true;
        }
    } else if (is_IPv6(m_group_mem_protocol)) {
        struct sioc_sg_req6 tmp_stat;
        if (m_mrt_sock->get_mroute_stats(saddr, gaddr, nullptr, &tmp_stat)) {
            return tmp_stat.pktcnt;
        } else {
            return true;
        }
    } else {
        HC_LOG_ERROR("unknown IP version");
        return true;
    }
}

void simple_routing_data::set_source(const addr_storage& gaddr, const source& saddr)
{
    HC_LOG_TRACE("");
    auto gaddr_it = m_data.find(gaddr);
    if (gaddr_it != std::end(m_data)) {
        auto result = gaddr_it->second.insert(saddr);
        if (!result.second) {
            saddr.retransmission_count = get_current_packet_count(gaddr, saddr.saddr);
            gaddr_it->second.erase(result.first);
            gaddr_it->second.insert(saddr);
        }
    } else {
        m_data.insert(s_routing_data_pair(gaddr, {saddr} ));
    }
}

void simple_routing_data::del_source(const addr_storage& gaddr, const addr_storage& saddr)
{
    HC_LOG_TRACE("");
    auto gaddr_it = m_data.find(gaddr);
    if (gaddr_it != std::end(m_data)) {
        gaddr_it->second.erase(saddr);
        if (gaddr_it->second.empty()) {
            m_data.erase(gaddr_it);
        }
    }

}

std::pair<source_list<source>::iterator, bool> simple_routing_data::refresh_source_or_del_it_if_unused(const addr_storage& gaddr, const addr_storage& saddr)
{
    HC_LOG_TRACE("");
    auto gaddr_it = m_data.find(gaddr);
    if (gaddr_it != std::end(m_data)) {

        auto saddr_it = gaddr_it->second.find(saddr);
        if (saddr_it != std::end(gaddr_it->second)) {

            auto cnt = get_current_packet_count(gaddr, saddr);
            if (static_cast<unsigned long>(saddr_it->retransmission_count) == cnt) {
                gaddr_it->second.erase(saddr_it);
            } else {
                saddr_it->retransmission_count = cnt;
                return std::pair<source_list<source>::iterator, bool>(saddr_it, true);
            }

        }

        gaddr_it->second.erase(saddr);
        if (gaddr_it->second.empty()) {
            m_data.erase(gaddr_it);
        }
    }

    return std::pair<source_list<source>::iterator, bool>(source_list<source>::iterator(), false);
}

source_list<source> simple_routing_data::get_available_sources(const addr_storage& gaddr, const source_list<source>& slist) const
{
    HC_LOG_TRACE("");
    source_list<source> rt;

    auto gaddr_it = m_data.find(gaddr);
    if (gaddr_it != std::end(m_data)) {
        rt = gaddr_it->second * slist;
    }

    return rt;
}

std::string simple_routing_data::to_string() const
{
    using  namespace std;
    HC_LOG_TRACE("");
    ostringstream s;
    s << "##-- simple multicast routing information base --##";

    for (auto &  b : m_data) {
        s << endl << "\tgroup: " << b.first;
        s << endl << "\t\tsources: " << b.second;
    }

    return s.str();
}

std::ostream& operator<<(std::ostream& stream, const simple_routing_data& rm)
{
    return stream << rm.to_string();
}

void simple_routing_data::test_simple_routing_data()
{
    using namespace std;
    //simple_routing_data srd;
    //cout << srd << endl;
    //srd.set_source(1, addr_storage("10.1.1.1"), addr_storage("1.1.1.1"));
    //srd.set_source(1, addr_storage("10.1.1.1"), addr_storage("1.1.1.2"));
    //srd.set_source(1, addr_storage("10.1.1.1"), addr_storage("1.1.1.3"));
    //srd.set_source(1, addr_storage("10.1.1.2"), addr_storage("1.1.1.1"));
    //srd.set_source(1, addr_storage("10.1.1.2"), addr_storage("1.1.1.2"));
    //srd.set_source(1, addr_storage("10.1.1.3"), addr_storage("1.1.1.1"));
    //srd.set_source(0, addr_storage("10.1.1.1"), addr_storage("1.1.1.1"));
    //srd.set_source(0, addr_storage("10.1.1.1"), addr_storage("1.1.1.2"));
    //cout << srd << endl;
    //srd.del_source(1, addr_storage("10.1.1.2"), addr_storage("1.1.1.1"));
    //srd.del_source(1, addr_storage("10.1.1.2"), addr_storage("1.1.1.1"));
    //srd.del_source(0, addr_storage("10.1.1.1"), addr_storage("1.1.1.1"));
    //srd.del_source(0, addr_storage("10.1.1.1"), addr_storage("1.1.1.2"));
    //cout << srd << endl;
}