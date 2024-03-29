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

#include "gui/widgets/skilldata.h"

#include "configuration.h"

#include "gui/theme.h"

#include "resources/image.h"
#include "resources/resourcemanager.h"

#include "debug.h"

SkillData::SkillData() :
    name(),
    shortName(),
    dispName(),
    description(),
    icon(nullptr),
    particle(),
    soundHit(std::string(), 0),
    soundMiss(std::string(), 0)
{
}

SkillData::~SkillData()
{
    if (icon)
    {
        icon->decRef();
        icon = nullptr;
    }
}

void SkillData::setIcon(const std::string &iconPath)
{
    ResourceManager *const res = ResourceManager::getInstance();
    if (!iconPath.empty())
        icon = res->getImage(iconPath);

    if (!icon)
    {
        icon = Theme::getImageFromTheme(
            paths.getStringValue("unknownItemFile"));
    }
}
