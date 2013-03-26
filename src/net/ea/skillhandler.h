/*
 *  The ManaPlus Client
 *  Copyright (C) 2004-2009  The Mana World Development Team
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

#ifndef NET_EA_SKILLHANDLER_H
#define NET_EA_SKILLHANDLER_H

#include "net/messagein.h"
#include "net/net.h"
#include "net/skillhandler.h"

namespace Ea
{

class SkillHandler : public Net::SkillHandler
{
    public:
        SkillHandler();

        A_DELETE_COPY(SkillHandler)

        void handleMessage(Net::MessageIn &msg);

        void use(int id);

        void processPlayerSkills(Net::MessageIn &msg);

        void processPlayerSkillUp(Net::MessageIn &msg);

        void processSkillFailed(Net::MessageIn &msg);
};

} // namespace Ea

#endif // NET_EA_SKILLHANDLER_H