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

#include <cassert>

#include "chat.h"
#include "inventorywindow.h"
#include "itemamount.h"
#include "popupmenu.h"
#include "storagewindow.h"

#include "../item.h"
#include "../playerrelations.h"

#include "../bindings/guichan/graphics.h"

#include "../bindings/guichan/widgets/browserbox.h"

#include "../net/messageout.h"
#include "../net/protocol.h"

#include "../resources/beingmanager.h"

#include "../resources/db/itemdb.h"

#include "../resources/sprite/being.h"
#include "../resources/sprite/flooritem.h"
#include "../resources/sprite/localplayer.h"
#include "../resources/sprite/npc.h"

#include "../utils/gettext.h"
#include "../utils/stringutils.h"

extern std::string tradePartnerName;

PopupMenu::PopupMenu(MenuType type):
    Popup("PopupMenu"),
    mBeingId(NULL),
    mFloorItem(NULL),
    mItem(NULL),
    mType(type)
{
    mBrowserBox = new BrowserBox();
    mBrowserBox->setPosition(getPadding(), getPadding());
    mBrowserBox->setOpaque(false);
    mBrowserBox->setLinkHandler(this);
    add(mBrowserBox);

    loadPopupConfiguration();
}

void PopupMenu::showPopup(int x, int y, Being *being)
{
    mBeingId = being->getId();
    mBrowserBox->clearRows();

    // Any being's name can be added to chat
    mBrowserBox->addRow(_("@@name|Add name to chat@@"));

    const std::string &name = being->getType() == Being::NPC ?
                              being->getName().substr(0, being->getName().size()
                              - 6) : being->getName();

    switch (being->getType())
    {
        case Being::PLAYER:
            {
                // Players can be traded with. Later also follow and
                // add as buddy will be options in this menu.
                mBrowserBox->addRow(strprintf(_("@@trade|Trade With %s@@"), name.c_str()));
                mBrowserBox->addRow(strprintf(_("@@attack|Attack %s@@"), name.c_str()));

                mBrowserBox->addRow("##3---");

                switch (player_relations.getRelation(name)) {
                case PlayerRelation::NEUTRAL:
                    mBrowserBox->addRow(strprintf(_("@@friend|Befriend %s@@"), name.c_str()));

                case PlayerRelation::FRIEND:
                    mBrowserBox->addRow(strprintf(_("@@disregard|Disregard %s@@"), name.c_str()));
                    mBrowserBox->addRow(strprintf(_("@@ignore|Ignore %s@@"), name.c_str()));
                    break;

                case PlayerRelation::DISREGARDED:
                    mBrowserBox->addRow(strprintf(_("@@unignore|Un-Ignore %s@@"), name.c_str()));
                    mBrowserBox->addRow(strprintf(_("@@ignore|Completely ignore %s@@"), name.c_str()));
                    break;

                case PlayerRelation::IGNORED:
                    mBrowserBox->addRow(strprintf(_("@@unignore|Un-Ignore %s@@"), name.c_str()));
                    break;
                }

                //mBrowserBox->addRow(_(strprintf("@@follow|Follow %s@@"), name.c_str()));

                
                /*mBrowserBox->addRow("##3---");
                mBrowserBox->addRow(strprintf(_("@@party|Invite %s to party@@"), name.c_str()));*/
            }
            break;

        case Being::NPC:
            // NPCs can be talked to (single option, candidate for removal
            // unless more options would be added)
            mBrowserBox->addRow(strprintf(_("@@talk|Talk To %s@@"), name.c_str()));
            break;

        case Being::MONSTER:
            // Monsters can be attacked
            mBrowserBox->addRow(strprintf(_("@@attack|Attack %s@@"), name.c_str()));
            break;

        default:
            /* Other beings aren't interesting... */
            return;
    }

    //browserBox->addRow("@@look|Look To@@");
    mBrowserBox->addRow("##3---");
    mBrowserBox->addRow(_("@@cancel|Cancel@@"));

    showPopup(x, y);
}

void PopupMenu::showPopup(int x, int y, FloorItem *floorItem)
{
    mFloorItem = floorItem;
    mItem = floorItem->getItem();
    mBrowserBox->clearRows();

    // Floor item can be picked up (single option, candidate for removal)
    std::string name = ItemDB::get(mFloorItem->getItemId()).getName();
    mBrowserBox->addRow(strprintf(_("@@pickup|Pick Up %s@@"), name.c_str()));
    mBrowserBox->addRow(_("@@chat|Add to Chat@@"));

    //browserBox->addRow("@@look|Look To@@");
    mBrowserBox->addRow("##3---");
    mBrowserBox->addRow(_("@@cancel|Cancel@@"));

    showPopup(x, y);
}

void PopupMenu::handleLink(const std::string& link)
{
    Being *being = beingManager->findBeing(mBeingId);

    // Talk To action
    if (link == "talk" && being && being->getType() == Being::NPC &&
        current_npc == 0)
    {
        dynamic_cast<NPC*>(being)->talk();
    }

    // Trade action
    else if (link == "trade" && being && being->getType() == Being::PLAYER)
    {
        player_node->trade(being);
        tradePartnerName = being->getName();
    }

    // Attack action
    else if (link == "attack" && being)
    {
        player_node->attack(being, true);
    }

    else if (link == "unignore" && being && being->getType() == Being::PLAYER)
    {
        player_relations.setRelation(being->getName(), PlayerRelation::NEUTRAL);
    }

    else if (link == "ignore" && being && being->getType() == Being::PLAYER)
    {
        player_relations.setRelation(being->getName(), PlayerRelation::IGNORED);
    }

    else if (link == "disregard" && being && being->getType() == Being::PLAYER)
    {
        player_relations.setRelation(being->getName(), PlayerRelation::DISREGARDED);
    }

    else if (link == "friend" && being && being->getType() == Being::PLAYER)
    {
        player_relations.setRelation(being->getName(), PlayerRelation::FRIEND);
    }

    /*
    // Follow Player action
    else if (link == "follow")
    {
    }*/

    // Pick Up Floor Item action
    else if ((link == "pickup") && mFloorItem)
    {
        player_node->pickUp(mFloorItem);
    }

    // Look To action
    else if (link == "look")
    {
    }

    else if (link == "use")
    {
        assert(mItem);
        if (mItem->isEquipment())
        {
            if (mItem->isEquipped())
                player_node->unequipItem(mItem);
            else
                player_node->equipItem(mItem);
        }
        else
        {
            player_node->useItem(mItem);
        }
    }

    else if (link == "chat")
    {
        chatWindow->addItemText(mItem->getInfo().getName());
    }

    else if (link == "drop")
    {
        new ItemAmountWindow(AMOUNT_ITEM_DROP, inventoryWindow, mItem);
    }

    else if (link == "store")
    {
        if (storageWindow && storageWindow->isVisible())
            new ItemAmountWindow(AMOUNT_STORE_ADD, inventoryWindow, mItem);
    }

    else if (link == "retrieve")
    {
        new ItemAmountWindow(AMOUNT_STORE_REMOVE, storageWindow, mItem);
    }

    else if (link == "party" && being && being->getType() == Being::PLAYER)
    {
        MessageOut outMsg(CMSG_PARTY_INVITE);
        outMsg.writeInt32(being->getId());
    }

    else if (link == "name" && being)
    {
        const std::string &name = being->getType() == Being::NPC ?
                                  being->getName().substr(0,
                                  being->getName().size() - 6) : being->getName();
        chatWindow->addInputText(name);
    }

    // Unknown actions
    else if (link != "cancel")
    {
        std::cout << link << std::endl;
    }

    setVisible(false);

    mBeingId = NULL;
    mFloorItem = NULL;
    mItem = NULL;
}

void PopupMenu::showPopup(int x, int y, Item *item)
{
    assert(item);
    mItem = item;
    mBrowserBox->clearRows();

    if (mType == INVENTORY)
    {
        if (item->isEquipment())
        {
            if (item->isEquipped())
                mBrowserBox->addRow(_("@@use|Unequip@@"));
            else
                mBrowserBox->addRow(_("@@use|Equip@@"));
        }
        else
            mBrowserBox->addRow(_("@@use|Use@@"));

        mBrowserBox->addRow(_("@@drop|Drop@@"));

        if (storageWindow && storageWindow->isVisible())
            mBrowserBox->addRow(_("@@store|Store@@"));
    }
    else if (mType == STORAGE)
    {
        mBrowserBox->addRow(_("@@retrieve|Retrieve@@"));
    }

    mBrowserBox->addRow(_("@@chat|Add to Chat@@"));
    mBrowserBox->addRow("##3---");
    mBrowserBox->addRow(_("@@cancel|Cancel@@"));

    showPopup(x, y);
}

void PopupMenu::showPopup(int x, int y)
{
    setContentSize(mBrowserBox->getWidth() + (2 * getPadding()),
                   mBrowserBox->getHeight() + getPadding());
    if (graphics->getWidth() < (x + getWidth()))
        x = graphics->getWidth() - getWidth();
    if (graphics->getHeight() < (y + getHeight()))
        y = graphics->getHeight() - getHeight();
    setPosition(x, y);
    setVisible(true);
    requestMoveToTop();
}
