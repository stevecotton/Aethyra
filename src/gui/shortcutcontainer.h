/*
 *  The Mana World
 *  Copyright 2007 The Mana World Development Team
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

#ifndef _TMW_SHORTCUTCONTAINER_H__
#define _TMW_SHORTCUTCONTAINER_H__

#include <guichan/mouselistener.hpp>
#include <guichan/widget.hpp>
#include <guichan/widgetlistener.hpp>

#include "../guichanfwd.h"

class Image;
class Item;

/**
 * An item shortcut container. Used to quickly use items.
 *
 * \ingroup GUI
 */
class ShortcutContainer : public gcn::Widget,
                              public gcn::WidgetListener,
                              public gcn::MouseListener
{
    public:
        /**
         * Constructor. Initializes the graphic.
         */
        ShortcutContainer();

        /**
         * Destructor.
         */
        ~ShortcutContainer(){}

        /**
         * Draws the items.
         */
        virtual void draw(gcn::Graphics *graphics)=0;

        /**
         * Invoked when a widget changes its size. This is used to determine
         * the new height of the container.
         */
        virtual void widgetResized(const gcn::Event &event);

        /**
         * Handles mouse when dragged.
         */
        virtual void mouseDragged(gcn::MouseEvent &event)=0;

        /**
         * Handles mouse when pressed.
         */
        virtual void mousePressed(gcn::MouseEvent &event)=0;

        /**
         * Handles mouse release.
         */
        virtual void mouseReleased(gcn::MouseEvent &event)=0;

        virtual int getMaxItems()
        { return mMaxItems; }

        virtual int getBoxWidth()
        { return mBoxWidth; }

        virtual int getBoxHeight()
        { return mBoxHeight; }

    protected:
        /**
         * Gets the index from the grid provided the point is in an item box.
         *
         * @param pointX X coordinate of the point.
         * @param pointY Y coordinate of the point.
         * @return index on success, -1 on failure.
         */
        int getIndexFromGrid(int pointX, int pointY) const;

        Image *mBackgroundImg;

        int mMaxItems;
        int mBoxWidth;
        int mBoxHeight;
        int mCursorPosX, mCursorPosY;
        int mGridWidth, mGridHeight;
};

#endif
