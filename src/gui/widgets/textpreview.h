/*
 *  The Mana World
 *  Copyright (C) 2006  The Mana World Development Team
 *
 *  This file is part of The Mana World.
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

#ifndef TEXTPREVIEW_H
#define TEXTPREVIEW_H

#include <guichan/color.hpp>
#include <guichan/font.hpp>
#include <guichan/widget.hpp>

/**
 * Preview widget for particle colors, etc.
 */
class TextPreview : public gcn::Widget
{
    public:
        TextPreview(const std::string* text);

        /**
         * Sets the color the text is printed in.
         *
         * @param color the color to set
         */
        inline void setTextColor(const gcn::Color* color)
        {
            mTextColor = color;
        }

        /**
         * Sets the color the text background is drawn in. This is only the
         * rectangle directly behind the text, not to full widget.
         *
         * @param color the color to set
         */
        inline void setTextBGColor(const gcn::Color* color)
        {
            mTextBGColor = color;
        }

        /**
         * Sets the background color of the widget.
         *
         * @param color the color to set
         */
        inline void setBGColor(const gcn::Color* color)
        {
            mBGColor = color;
        }

        /**
         * Sets the font to render the text in.
         *
         * @param font the font to use.
         */
        inline void setFont(gcn::Font *font)
        {
            mFont = font;
        }

        /**
         * Sets whether to use a shadow while rendering.
         *
         * @param shadow true, if a shadow is wanted, false else
         */
        inline void setShadow(bool shadow)
        {
            mShadow = shadow;
        }

        /**
         * Sets whether to use an outline while rendering.
         *
         * @param outline true, if an outline is wanted, false else
         */
        inline void setOutline(bool outline)
        {
            mOutline = outline;
        }

        /**
         * Widget's draw method. Does the actual job.
         *
         * @param graphics graphics to draw into
         */
        void draw(gcn::Graphics *graphics);

    private:
        gcn::Font *mFont;
        const std::string* mText;
        const gcn::Color* mTextColor;
        const gcn::Color* mBGColor;
        const gcn::Color* mTextBGColor;
        bool mShadow;
        bool mOutline;
};

#endif
