/*
 *  Extended support for activating emotes
 *  Copyright (C) 2009  Aethyra Development Team
 *
 *  This file is part of Aethyra derived from original code
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

#include <guichan/focushandler.hpp>
#include <guichan/font.hpp>
#include <guichan/mouseinput.hpp>
#include <guichan/selectionlistener.hpp>

#include "emotecontainer.h"

#include "../db/emotedb.h"

#include "../gui/popupmenu.h"

#include "../../bindings/guichan/graphics.h"
#include "../../bindings/guichan/gui.h"
#include "../../bindings/guichan/protectedfocuslistener.h"
#include "../../bindings/guichan/skin.h"

#include "../../bindings/guichan/sdl/sdlinput.h"

#include "../../core/log.h"
#include "../../core/resourcemanager.h"

#include "../../core/image/image.h"

#include "../../core/map/sprite/animatedsprite.h"
#include "../../core/map/sprite/localplayer.h"

const int EmoteContainer::gridWidth = 34;  // emote icon width + 4
const int EmoteContainer::gridHeight = 36; // emote icon height + 4

static const int NO_EMOTE = -1;

std::vector<const AnimatedSprite*> EmoteContainer::mEmoteImg;
Image *EmoteContainer::mSelImg = NULL;
int EmoteContainer::mInstances = 0;
int EmoteContainer::mMaxEmote = 0;
PopupMenu *EmoteContainer::mPopupMenu = NULL;

EmoteContainer::EmoteContainer(const std::string &actionEventId,
                               gcn::ActionListener *listener):
    mSelectedEmoteIndex(NO_EMOTE)
{
    if (!actionEventId.empty())
        setActionEventId(actionEventId);

    if (listener && !actionEventId.empty())
        addActionListener(listener);

    setFocusable(true);

    if (mInstances == 0)
    {
        mPopupMenu = new PopupMenu(EMOTE);

        ResourceManager *resman = ResourceManager::getInstance();

        // Setup emote sprites
        for (int i = 0; i <= EmoteDB::getLast(); i++)
            mEmoteImg.push_back(EmoteDB::getAnimation(i));

        if (mEmoteImg.size() == 0)
            setEnabled(false);

        mSelImg = resman->getImage("graphics/gui/selection.png");
        mSelImg->setAlpha(Skin::getAlpha());

        if (!mSelImg)
            logger->error("Unable to load selection.png");

        mMaxEmote = EmoteDB::getLast() + 1;
    }

    mProtFocusListener = new ProtectedFocusListener();

    addFocusListener(mProtFocusListener);

    mProtFocusListener->blockKey(SDLK_LEFT);
    mProtFocusListener->blockKey(SDLK_RIGHT);
    mProtFocusListener->blockKey(SDLK_UP);
    mProtFocusListener->blockKey(SDLK_DOWN);
    mProtFocusListener->blockKey(SDLK_SPACE);
    mProtFocusListener->blockKey(SDLK_RETURN);

    mInstances++;

    addKeyListener(this);
    addMouseListener(this);
    addWidgetListener(this);
}

EmoteContainer::~EmoteContainer()
{
    mInstances--;

    if (mInstances == 0)
    {
        if (mSelImg)
            mSelImg->decRef();

        delete mPopupMenu;
    }

    if (mFocusHandler && mFocusHandler->isFocused(this))
        mFocusHandler->focusNone();

    removeFocusListener(mProtFocusListener);
    delete mProtFocusListener;
}

void EmoteContainer::draw(gcn::Graphics *graphics)
{
    const int columns = std::max(1, getWidth() / gridWidth);
    int emoteCount = 0;

    for (int i = 0; i < mMaxEmote ; i++)
    {
        const int emoteX = ((i) % columns) * gridWidth;
        const int emoteY = ((i) / columns) * gridHeight;

        // Draw selection image below selected emote
        if (mSelectedEmoteIndex == i)
        {
            static_cast<Graphics*>(graphics)->drawImage(mSelImg, emoteX,
                                                        emoteY + 8);
        }

        // Draw emote icon
        if (mEmoteImg[i])
        {
            emoteCount++;
            mEmoteImg[i]->draw(static_cast<Graphics*>(graphics), emoteX, emoteY);
        }
    }

    if (!emoteCount)
        selectNone();
}

void EmoteContainer::widgetResized(const gcn::Event &event)
{
    recalculateHeight();
}

void EmoteContainer::recalculateHeight()
{
    const int cols = std::max(1, getWidth() / gridWidth);
    const int rows = (mMaxEmote / cols) + (mMaxEmote % cols > 0 ? 1 : 0);
    const int height = rows * gridHeight + 8;

    if (height != getHeight())
        setHeight(height);
}

int EmoteContainer::getSelectedEmote()
{
    return 1 + mSelectedEmoteIndex;
}

void EmoteContainer::selectNone()
{
    setSelectedEmoteIndex(NO_EMOTE);
}

void EmoteContainer::setSelectedEmoteIndex(int index)
{
    if (index < 0 || index >= mMaxEmote )
        mSelectedEmoteIndex = NO_EMOTE;
    else
        mSelectedEmoteIndex = index;

    gcn::Rectangle scroll;

    const int columns = std::max(1, getWidth() / gridWidth);
    const int emoteY = mSelectedEmoteIndex / columns;

    if (mSelectedEmoteIndex == NO_EMOTE)
        scroll.y = 0;
    else
        scroll.y = gridHeight * emoteY;

    scroll.height = gridHeight;
    showPart(scroll);

    distributeValueChangedEvent();
}

void EmoteContainer::distributeValueChangedEvent()
{
    gcn::SelectionEvent event(this);
    std::list<gcn::SelectionListener*>::iterator i_end = mListeners.end();
    std::list<gcn::SelectionListener*>::iterator i;

    for (i = mListeners.begin(); i != i_end; ++i)
        (*i)->valueChanged(event);
}

void EmoteContainer::showPopup(bool useMouseCoordinates)
{
    const int emote = mSelectedEmoteIndex;
    int x = gui->getMouseX();
    int y = gui->getMouseY();

    if (emote == NO_EMOTE)
        return;

    if (!useMouseCoordinates)
    {
        const int columns = std::max(1, getWidth() / gridWidth);
        const int emoteX = emote % columns;
        const int emoteY = emote / columns;
        const int xPos = emoteX * gridWidth + (gridWidth / 2);
        const int yPos = emoteY * gridHeight + (gridHeight / 2) +
                         gui->getFont()->getHeight();

        x = xPos;
        y = yPos;

        gcn::Widget *widget = this;

        while (widget)
        {
            x += widget->getX();
            y += widget->getY();

            widget = widget->getParent();
        }
    }

    mPopupMenu->setEmote(getSelectedEmote());
    mPopupMenu->showPopup(x, y);
}

void EmoteContainer::keyPressed(gcn::KeyEvent &event)
{
    int columns = std::max(1, getWidth() / gridWidth);
    const int rows = mMaxEmote / columns;
    const int emoteX = mSelectedEmoteIndex % columns;
    const int emoteY = mSelectedEmoteIndex / columns;

    if (columns > mMaxEmote)
        columns = mMaxEmote;

    switch (event.getKey().getValue())
    {
        case Key::LEFT:
            if (emoteX != 0)
                setSelectedEmoteIndex((emoteY * columns) + emoteX - 1);

            event.consume();
            break;
        case Key::RIGHT:
            if (emoteX < (columns - 1) &&
               ((emoteY * columns) + emoteX + 1) < mMaxEmote)
                setSelectedEmoteIndex((emoteY * columns) + emoteX + 1);

            event.consume();
            break;
        case Key::UP:
            if (emoteY != 0)
                setSelectedEmoteIndex(((emoteY - 1) * columns) + emoteX);

            event.consume();
            break;
        case Key::DOWN:
            if (emoteY < rows &&
               (((emoteY + 1) * columns) + emoteX) < mMaxEmote)
                setSelectedEmoteIndex(((emoteY + 1) * columns) + emoteX);

            event.consume();
            break;
        case Key::ENTER:
        case Key::SPACE:
            if (event.isShiftPressed())
            {
                const std::string actionEventId = getActionEventId();
                setActionEventId("default");
                distributeActionEvent();
                setActionEventId(actionEventId);
            }
            else
                distributeActionEvent();

            event.consume();
            break;
    }
}

void EmoteContainer::mousePressed(gcn::MouseEvent &event)
{
    int button = event.getButton();

    if (button == gcn::MouseEvent::LEFT || button == gcn::MouseEvent::RIGHT)
    {
        const int columns = std::max(1, getWidth() / gridWidth);
        const int mx = event.getX();
        const int my = event.getY();
        const int index = mx / gridWidth + ((my / gridHeight) * columns);

        if (index > -1 && index < mMaxEmote)
            setSelectedEmoteIndex(index);
        else
            setSelectedEmoteIndex(NO_EMOTE);
    }
}
