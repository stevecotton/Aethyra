/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id$
 */

#include "monsterdb.h"

#include "resourcemanager.h"

#include "../log.h"

#include "../utils/dtor.h"
#include "../utils/xml.h"

namespace
{
    MonsterDB::MonsterInfos mMonsterInfos;
    MonsterInfo mUnknown;
    bool mLoaded = false;
}

void
MonsterDB::load()
{
    if (mLoaded)
        return;

    mUnknown.setSprite("error.xml");
    mUnknown.setName("unnamed");

    logger->log("Initializing monster database...");

    ResourceManager *resman = ResourceManager::getInstance();
    int size;
    char *data = (char*)resman->loadFile("monsters.xml", size);

    if (!data)
    {
        logger->error("Monster Database: Could not find monsters.xml!");
    }

    xmlDocPtr doc = xmlParseMemory(data, size);
    free(data);

    if (!doc)
    {
        logger->error("Monster Database: Error while parsing monster database (monsters.xml)!");
    }

    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    if (!rootNode || !xmlStrEqual(rootNode->name, BAD_CAST "monsters"))
    {
        logger->error("Monster Database: monster.xml is not a valid database file!");
    }

    //iterate <monster>s
    for_each_xml_child_node(monsterNode, rootNode)
    {
        if (!xmlStrEqual(monsterNode->name, BAD_CAST "monster"))
        {
            continue;
        }

        MonsterInfo *currentInfo = new MonsterInfo();

        currentInfo->setName (XML::getProperty(monsterNode, "name", "unnamed"));

        //iterate <sprite>s and <sound>s
        for_each_xml_child_node(spriteNode, monsterNode)
        {
            if (xmlStrEqual(spriteNode->name, BAD_CAST "sprite"))
            {
                currentInfo->setSprite((const char*) spriteNode->xmlChildrenNode->content);
            }

            if (xmlStrEqual(spriteNode->name, BAD_CAST "sound"))
            {
                std::string event = XML::getProperty(spriteNode, "event", "");
                const char *filename;
                filename = (const char*) spriteNode->xmlChildrenNode->content;

                if (event == "hit")
                {
                    currentInfo->addSound(EVENT_HIT, filename);
                }
                else if (event == "miss")
                {
                    currentInfo->addSound(EVENT_MISS, filename);
                }
                else if (event == "hurt")
                {
                    currentInfo->addSound(EVENT_HURT, filename);
                }
                else if (event == "die")
                {
                    currentInfo->addSound(EVENT_DIE, filename);
                }
                else
                {
                    logger->log("MonsterDB: Warning, sound effect %s for unknown event %s of monster %s",
                                filename, event.c_str(), currentInfo->getName().c_str());
                }
            }
        }
        mMonsterInfos[XML::getProperty(monsterNode, "id", 0)] = currentInfo;
    }

    mLoaded = true;
}

void
MonsterDB::unload()
{
    for_each (  mMonsterInfos.begin(), mMonsterInfos.end(),
                make_dtor(mMonsterInfos));
    mMonsterInfos.clear();

    mLoaded = false;
}


const MonsterInfo&
MonsterDB::get (int id)
{
    MonsterInfoIterator i = mMonsterInfos.find(id);

    if (i == mMonsterInfos.end())
    {
        logger->log("MonsterDB: Warning, unknown monster ID %d requested", id);
        return mUnknown;
    }
    else
    {
        return *(i->second);
    }
}
