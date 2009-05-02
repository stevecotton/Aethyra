/*
 *  Aethyra
 *  Copyright (C) 2008, The Legend of Mazzeroth Development Team
 *
 *  This file is part of Aethyra based on original code
 *  from The Legend of Mazzeroth.
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

#include <guichan/font.hpp>

#include <guichan/widgets/label.hpp>

#include "speechbubble.h"

#include "../bindings/guichan/gui.h"
#include "../bindings/guichan/graphics.h"

#include "../bindings/guichan/widgets/textbox.h"

SpeechBubble::SpeechBubble():
    Popup("Speech", "graphics/gui/speechbubble.xml"),
    mText("")
{
    setContentSize(140, 46);
    setMinWidth(29);
    setMinHeight(29);

    mCaption = new gcn::Label("");
    mCaption->setFont(gui->getBoldFont());

    mSpeechBox = new TextBox();
    mSpeechBox->setEditable(false);
    mSpeechBox->setOpaque(false);
    mSpeechBox->setTextColor(&guiPalette->getColor(Palette::CHAT));

    add(mCaption);
    add(mSpeechBox);

    loadPopupConfiguration();
}

void SpeechBubble::setCaption(const std::string &name, const gcn::Color *color)
{
    mCaption->setFont(gui->getBoldFont());
    mCaption->setCaption(name);
    mCaption->adjustSize();
    mCaption->setForegroundColor(*color);
}

void SpeechBubble::setText(std::string text, bool showName)
{
    if (text == mText && (mCaption->getWidth() <= mSpeechBox->getMinWidth()))
        return;

    mSpeechBox->setTextColor(&guiPalette->getColor(Palette::TEXT));

    int width = mCaption->getWidth() + 2 * getPadding();
    mSpeechBox->setTextWrapped(text, 130 > width ? 130 : width);
    const int speechWidth = mSpeechBox->getMinWidth() + 2 * getPadding();

    const int fontHeight = getFont()->getHeight();
    const int nameHeight = showName ? mCaption->getHeight() + 
                           (getPadding() / 2) : 0;
    const int numRows = mSpeechBox->getNumberOfRows();
    const int height = (numRows * fontHeight) + nameHeight + getPadding();

    if (width < speechWidth)
        width = speechWidth;

    width += 2 * getPadding();

    setContentSize(width, height);

    const int xPos = ((getWidth() - width) / 2);
    const int yPos = ((getHeight() - height) / 2) + nameHeight;

    mCaption->setPosition(xPos, getPadding());
    mSpeechBox->setPosition(xPos, yPos);
}

