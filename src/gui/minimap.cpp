/*
 *  The Mana World
 *  Copyright 2004-2005 The Mana World Development Team
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
 */

#include "minimap.h"

#include "../being.h"
#include "../beingmanager.h"
#include "../graphics.h"
#include "../localplayer.h"

#include "../resources/image.h"

#include "../utils/gettext.h"

Minimap::Minimap():
    Window(_("Map")),
    mMapImage(NULL),
    mProportion(0.5)
{
    setCloseButton(true);
    setWindowName(_("MiniMap"));
}

Minimap::~Minimap()
{
    if (mMapImage)
        mMapImage->decRef();
}

void Minimap::setMapImage(Image *img)
{
    if (mMapImage)
        mMapImage->decRef();

    mMapImage = img;

    if (mMapImage)
    {
        int offsetX = getPadding() + 4;
        int offsetY = getTitleBarHeight() + 4;
        mMapImage->setAlpha(0.7);
        setDefaultSize(offsetX, offsetY, 
                       mMapImage->getWidth() < 100 ? 
                           mMapImage->getWidth() + offsetX : 100, 
                       mMapImage->getHeight() < 100 ? 
                           mMapImage->getHeight() + offsetY : 100);
        loadWindowState();
    }
    else
    {
        setVisible(false);
    }
}

void Minimap::draw(gcn::Graphics *graphics)
{
    Window::draw(graphics);

    const gcn::Rectangle a = getChildrenArea();

    graphics->pushClipArea(a);

    int mapOriginX = 0;
    int mapOriginY = 0;

    if (mMapImage)
    {
        if (mMapImage->getWidth() > a.width ||
            mMapImage->getHeight() > a.height)
        {
            mapOriginX = (int) (((a.width) / 2) - (player_node->mX * mProportion));
            mapOriginY = (int) (((a.height) / 2) - (player_node->mY * mProportion));

            const int minOriginX = a.width - mMapImage->getWidth();
            const int minOriginY = a.height - mMapImage->getHeight();

            if (mapOriginX < minOriginX)
                mapOriginX = minOriginX;
            if (mapOriginY < minOriginY)
                mapOriginY = minOriginY;
            if (mapOriginX > 0)
                mapOriginX = 0;
            if (mapOriginY > 0)
                mapOriginY = 0;
        }

        static_cast<Graphics*>(graphics)->
            drawImage(mMapImage, mapOriginX, mapOriginY);
    }

    const Beings &beings = beingManager->getAll();
    Beings::const_iterator bi;

    for (bi = beings.begin(); bi != beings.end(); bi++)
    {
        const Being *being = (*bi);
        int dotSize = 2;

        switch (being->getType()) {
            case Being::PLAYER:
                if (being == player_node)
                {
                    dotSize = 3;
                    graphics->setColor(gcn::Color(61, 209, 52));
                    break;
                }
                graphics->setColor(gcn::Color(61, 52, 209));
                break;

            case Being::MONSTER:
                graphics->setColor(gcn::Color(209, 52, 61));
                break;

            case Being::NPC:
                graphics->setColor(gcn::Color(255, 255, 0));
                break;

            default:
                continue;
        }

        const int offset = (int) ((dotSize - 1) * mProportion);

        graphics->fillRectangle(gcn::Rectangle(
                    (int) (being->mX * mProportion) + mapOriginX - offset,
                    (int) (being->mY * mProportion) + mapOriginY - offset,
                    dotSize, dotSize));
    }

    graphics->popClipArea();
}
