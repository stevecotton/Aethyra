/*
 *  Aethyra
 *  Copyright (C) 2004  The Mana World Development Team
 *
 *  This file is part of Aethyra based on original code
 *  from The Mana World.
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include <map>
#include <string>

#include "iteminfo.h"

class ItemInfo;

/**
 * The namespace that holds the item information.
 */
namespace ItemDB
{
    /**
     * Loads the item data from <code>items.xml</code>.
     */
    void load();

    /**
     * Frees item data.
     */
    void unload();

    const ItemInfo& get(const int id);
    const ItemInfo& get(const std::string &name);

    // Items database
    typedef std::map<int, ItemInfo*> ItemInfos;
    typedef std::map<std::string, ItemInfo*> NamedItemInfos;
    typedef ItemInfos::iterator ItemInfoIterator;
    typedef NamedItemInfos::iterator NamedItemInfoIterator;

    // Tags overview (gets the list of all tags used by items)
    std::size_t getTagCount();
    const std::string& getTag(int n);
}

#endif
