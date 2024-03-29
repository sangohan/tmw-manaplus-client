/*
 *  The ManaPlus Client
 *  Copyright (C) 2009  The Mana World Development Team
 *  Copyright (C) 2009-2010  The Mana Developers
 *  Copyright (C) 2011-2013  The ManaPlus Developers
 *
 *  This file is part of The ManaPlus Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NET_WORLDINFO_H
#define NET_WORLDINFO_H

#include <string>
#include <vector>

struct WorldInfo final
{
    WorldInfo() :
        address(0),
        name(),
        port(0),
        online_users(0),
        updateHost()
    {
    }

    int address;
    std::string name;
    uint16_t port;
    int online_users;
    std::string updateHost;
};

typedef std::vector<WorldInfo*> Worlds;

#endif  // NET_WORLDINFO_H
