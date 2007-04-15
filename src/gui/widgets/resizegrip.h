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

#ifndef _TMW_RESIZEGRIP_H
#define _TMW_RESIZEGRIP_H

#include <guichan/widget.hpp>

class Image;

/**
 * Resize grip. The resize grip is part of a resizable Window. It relies on the
 * fact that uncaught mouse events are automatically routed to the parent
 * window.
 *
 * \ingroup GUI
 */
class ResizeGrip : public gcn::Widget
{
    public:
        /**
         * Constructor.
         */
        ResizeGrip();

        /**
         * Destructor.
         */
        ~ResizeGrip();

        /**
         * Draws the resize grip.
         */
        void draw(gcn::Graphics *graphics);

    private:
        static Image *gripImage;   /**< Resize grip image */
        static int mInstances;     /**< Number of resize grip instances */
};

#endif
