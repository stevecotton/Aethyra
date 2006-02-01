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

#include "npc.h"

#include "graphics.h"

#include "graphic/spriteset.h"

#include "net/messageout.h"
#include "net/protocol.h"

extern Spriteset *npcset;

NPC *current_npc = 0;

NPC::NPC(Uint32 id, Uint16 job, Map *map, Network *network):
    Being(id, job, map), mNetwork(network)
{
}

Being::Type NPC::getType() const
{
    return Being::NPC;
}

void NPC::draw(Graphics *graphics, int offsetX, int offsetY)
{
    int px = mPx + offsetX;
    int py = mPy + offsetY;

    graphics->drawImage(npcset->spriteset[job - 100], px - 8, py - 52);

    Being::draw(graphics, offsetX, offsetY);
}

void NPC::talk()
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_NPC_TALK);
    outMsg.writeInt32(mId);
    outMsg.writeInt8(0);
    current_npc = this;
}

void NPC::nextDialog()
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_NPC_NEXT_REQUEST);
    outMsg.writeInt32(mId);
}

void NPC::dialogChoice(char choice)
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_NPC_LIST_CHOICE);
    outMsg.writeInt32(mId);
    outMsg.writeInt8(choice);
}

/*
 * TODO Unify the buy() and sell() methods, without sacrificing readability of
 * the code calling the method. buy(bool buySell) would be bad...
 */
void NPC::buy()
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_NPC_BUY_SELL_REQUEST);
    outMsg.writeInt32(mId);
    outMsg.writeInt8(0);
}

void NPC::sell()
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_NPC_BUY_SELL_REQUEST);
    outMsg.writeInt32(mId);
    outMsg.writeInt8(1);
}