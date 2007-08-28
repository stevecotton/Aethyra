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

#ifndef _TMW_INVENTORYWINDOW_H
#define _TMW_INVENTORYWINDOW_H

#include <guichan/actionlistener.hpp>

#include "selectionlistener.h"
#include "window.h"
#include "windowlistener.h"

#include "../guichanfwd.h"

class Item;
class ItemContainer;

/**
 * Inventory dialog.
 *
 * \ingroup Interface
 */
class InventoryWindow : public Window, gcn::ActionListener, SelectionListener,
    WindowListener
{
    public:
        /**
         * Constructor.
         */
        InventoryWindow();

        /**
         * Logic (updates buttons and weight information).
         */
        void logic();

        /**
         * Called when receiving actions from the widgets.
         */
        void action(const gcn::ActionEvent &event);

        void mouseClicked(gcn::MouseEvent &event);

        Item* getItem();

        /**
         * Updates labels to currently selected item.
         *
         * @see SelectionListener::selectionChanged.
         */
        void selectionChanged(const SelectionEvent &event);

        /**
         * Called whenever the window is resized.
         */
        void windowResized(const WindowEvent &event);

    private:
        void updateButtons();    /**< Updates button states. */

        ItemContainer *mItems;

        gcn::Button *mUseButton, *mDropButton;
        gcn::ScrollArea *mInvenScroll;
        gcn::Label *mItemNameLabel;
        gcn::Label *mItemDescriptionLabel;
        gcn::Label *mItemEffectLabel;
        gcn::Label *mWeightLabel;
};

extern InventoryWindow *inventoryWindow;

#endif
