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

#include "inventory.h"

#include "equipment.h"
#include "item.h"

#include "net/messageout.h"
#include "net/network.h"
#include "net/protocol.h"

Inventory::Inventory()
{
    items = new Item[INVENTORY_SIZE];
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        items[i].setInvIndex(i);
    }
}

Inventory::~Inventory()
{
    delete [] items;
}

Item* Inventory::getItem(int index)
{
    return &items[index];
}

void Inventory::addItem(int id, int quantity, bool equipment)
{
    addItem(getFreeSlot(), id, quantity, equipment);
}

void Inventory::addItem(int index, int id, int quantity, bool equipment)
{
    items[index].setId(id);
    items[index].increaseQuantity(quantity);
    items[index].setEquipment(equipment);
}


void Inventory::resetItems()
{
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        items[i].setId(-1);
        items[i].setQuantity(0);
        items[i].setEquipped(false);
    }
}

void Inventory::removeItem(int id)
{
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (items[i].getId() == id) {
            items[i].setId(-1);
            items[i].setQuantity(0);
        }
    }
}

bool Inventory::contains(Item *item)
{
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (items[i].getId() == item->getId()) {
            return true;
        }
    }

    return false;
}

int Inventory::useItem(Item *item)
{
    MessageOut outMsg;
    outMsg.writeShort(CMSG_PLAYER_INVENTORY_USE);
    outMsg.writeShort(item->getInvIndex());
    outMsg.writeLong(item->getId());
    // Note: id is dest of item, usually player_node->account_ID ??
    writeSet(8);
    return 0;
}

int Inventory::dropItem(Item *item, int quantity)
{
    // TODO: Fix wrong coordinates of drops, serverside?
    MessageOut outMsg;
    outMsg.writeShort(CMSG_PLAYER_INVENTORY_DROP);
    outMsg.writeShort(item->getInvIndex());
    outMsg.writeShort(quantity);
    writeSet(6);
    return 0;
}

void Inventory::equipItem(Item *item)
{
    MessageOut outMsg;
    outMsg.writeShort(CMSG_PLAYER_EQUIP);
    outMsg.writeShort(item->getInvIndex());
    outMsg.writeShort(0);
    writeSet(6);
}

void Inventory::unequipItem(Item *item)
{
    MessageOut outMsg;
    outMsg.writeShort(CMSG_PLAYER_UNEQUIP);
    outMsg.writeShort(item->getInvIndex());
    writeSet(4);

    // Tidy equipment directly to avoid weapon still shown bug, by instance
    Equipment::getInstance()->removeEquipment(item);
}

int Inventory::getFreeSlot()
{
    for (int i = 2; i < INVENTORY_SIZE; i++) {
        if (items[i].getId() == -1) {
            return i;
        }
    }
    return -1;
}

int Inventory::getNumberOfSlotsUsed()
{
    int NumberOfFilledSlot = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (items[i].getId() > -1 || items[i].getQuantity() > 0) 
        {
            NumberOfFilledSlot++;
        }
    }

    return NumberOfFilledSlot;
}

int Inventory::getLastUsedSlot()
{
    int i;

    for (i = INVENTORY_SIZE - 1; i >= 0; i--) {
        if ((items[i].getId() != -1) && (items[i].getQuantity() > 0)) {
            break;
        }
    }

    return --i;
}
