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

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <vector>



class scanner
{
private:
    std::vector<std::pair<unsigned int, std::string>> m_cmds;
    unsigned int m_vec_cmd_pos;

    unsigned int m_pos; //current position
    unsigned int m_vec_pos;
    char m_current_char;

    std::string load_file(const std::string& path);
    std::string delete_comments(std::string&& script_file);
    std::vector<std::pair<unsigned int, std::string>> separate_commands(std::string&& script_file);
public:
    scanner(const std::string& path);
    scanner();
    static void test_scanner();

};


#endif // SCANNER_HPP
