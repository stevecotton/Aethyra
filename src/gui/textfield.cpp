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
 *  $Id: textfield.cpp 4045 2008-04-07 15:23:07Z b_lindeijer $
 */

#include <algorithm>

#include <guichan/font.hpp>

#include "textfield.h"

#include "../graphics.h"

#include "../resources/image.h"
#include "../resources/resourcemanager.h"

#include "../utils/dtor.h"

int TextField::instances = 0;
ImageRect TextField::skin;

TextField::TextField(const std::string& text):
    gcn::TextField(text),
    mNumeric(false),
    mListener(0)
{
    setFrameSize(2);

    if (instances == 0)
    {
        // Load the skin
        ResourceManager *resman = ResourceManager::getInstance();
        Image *textbox = resman->getImage("graphics/gui/deepbox.png");
        int gridx[4] = {0, 3, 28, 31};
        int gridy[4] = {0, 3, 28, 31};
        //Image *textbox = resman->getImage("graphics/gui/textbox.png");
        //int gridx[4] = {0, 5, 26, 31};
        //int gridy[4] = {0, 5, 26, 31};
        int a = 0, x, y;

        for (y = 0; y < 3; y++) {
            for (x = 0; x < 3; x++) {
                skin.grid[a] = textbox->getSubImage(
                        gridx[x], gridy[y],
                        gridx[x + 1] - gridx[x] + 1,
                        gridy[y + 1] - gridy[y] + 1);
                a++;
            }
        }

        textbox->decRef();
    }

    instances++;
}

TextField::~TextField()
{
    instances--;

    if (instances == 0)
    {
        for_each(skin.grid, skin.grid + 9, dtor<Image*>());
    }
}

void TextField::draw(gcn::Graphics *graphics)
{
    if (isFocused()) {
        drawCaret(graphics,
                getFont()->getWidth(mText.substr(0, mCaretPosition)) -
                mXScroll);
    }

    graphics->setColor(getForegroundColor());
    graphics->setFont(getFont());
    graphics->drawText(mText, 1 - mXScroll, 1);
}

void TextField::drawFrame(gcn::Graphics *graphics)
{
    int w, h, bs;
    bs = getFrameSize();
    w = getWidth() + bs * 2;
    h = getHeight() + bs * 2;

    static_cast<Graphics*>(graphics)->drawImageRect(0, 0, w, h, skin);
}

void TextField::setNumeric(bool numeric)
{
    mNumeric = numeric;
    if (!numeric)
    {
        return;
    }
    const char *text = mText.c_str();
    for (const char *textPtr = text; *textPtr; ++textPtr)
    {
        if (*textPtr < '0' || *textPtr > '9')
        {
            setText(mText.substr(0, textPtr - text));
            return;
        }
    }
}

void TextField::keyPressed(gcn::KeyEvent &keyEvent)
{
    if (mNumeric)
    {
        while (true)
        {
            const gcn::Key &key = keyEvent.getKey();
            if (key.isNumber())
            {
                break;
            }
            int value = key.getValue();
            if (value == gcn::Key::LEFT || value == gcn::Key::RIGHT ||
                value == gcn::Key::HOME || value == gcn::Key::END ||
                value == gcn::Key::BACKSPACE || value == gcn::Key::DELETE)
            {
                break;
            }
            return;
        }
    }
    gcn::TextField::keyPressed(keyEvent);
    if (mListener)
    {
        mListener->listen(this);
    }
}

int TextField::getValue() const
{
    if (!mNumeric)
    {
        return 0;
    }
    int value = atoi(mText.c_str());
    if (value < mMinimum)
    {
        return mMinimum;
    }
    if (value > mMaximum)
    {
        return mMaximum;
    }
    return value;
}
