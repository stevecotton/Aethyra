/*
 *  Aethyra
 *  Copyright (C) 2004  The Mana World Development Team
 *
 *  This file is part of Aethyra based on original code
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

#include "status.h"

#include "../../bindings/guichan/layout.h"

#include "../../bindings/guichan/widgets/button.h"
#include "../../bindings/guichan/widgets/label.h"
#include "../../bindings/guichan/widgets/progressbar.h"
#include "../../bindings/guichan/widgets/windowcontainer.h"

#include "../../core/image/sprite/localplayer.h"

#include "../../core/utils/gettext.h"
#include "../../core/utils/stringutils.h"

StatusWindow::StatusWindow(LocalPlayer *player):
    Window(player->getName()),
    mPlayer(player)
{
    setWindowName("Status");
    setCloseButton(true);
    setDefaultSize(400, 315, ImageRect::CENTER);

    // ----------------------
    // Status Part
    // ----------------------

    mLvlLabel = new Label(strprintf(_("Level: %d"), 0));
    mJobLvlLabel = new Label(strprintf(_("Job: %d"), 0));
    mGpLabel = new Label(strprintf(_("Money: %d GP"), 0));

    mHpLabel = new Label(_("HP:"));
    mHpBar = new ProgressBar(0.5f, 80, 15, gcn::Color(223, 32, 32));
    mHpBar->addColor(230, 171, 34);
    mHpBar->addColor(0, 171, 34);

    mXpLabel = new Label(_("Exp:"));
    mXpBar = new ProgressBar(0.0f, 80, 15, gcn::Color(143, 192, 211));

    mMpLabel = new Label(_("MP:"));
    mMpBar = new ProgressBar(0.0f, 100, 20, gcn::Color(140, 70, 230));
    mMpBar->addColor(110, 85, 230);
    mMpBar->addColor(26, 100, 230);

    mJobLabel = new Label(_("Job:"));
    mJobBar = new ProgressBar(0.0f, 80, 15, gcn::Color(220, 135, 203));

    mHpBar->setProgress((float) mPlayer->mHp / mPlayer->mMaxHp);
    mMpBar->setProgress((float) mPlayer->mMp / mPlayer->mMaxMp);

    mXpBar->setProgress((float) mPlayer->getXp() / mPlayer->mXpForNextLevel);
    mJobBar->setProgress((float) mPlayer->mJobXp / mPlayer->mJobXpForNextLevel);

    // ----------------------
    // Stats Part
    // ----------------------

    // Static Labels
    gcn::Label *mStatsTitleLabel = new Label(_("Stats"));
    gcn::Label *mStatsTotalLabel = new Label(_("Total"));
    gcn::Label *mStatsCostLabel = new Label(_("Cost"));
    mStatsTotalLabel->setAlignment(gcn::Graphics::CENTER);

    // Derived Stats
    mStatsAttackLabel = new Label(_("Attack:"));
    mStatsDefenseLabel= new Label(_("Defense:"));
    mStatsMagicAttackLabel = new Label(_("M.Attack:"));
    mStatsMagicDefenseLabel = new Label(_("M.Defense:"));
    mStatsAccuracyLabel = new Label(_("% Accuracy:"));
    mStatsEvadeLabel = new Label(_("% Evade:"));
    mStatsReflexLabel = new Label(_("% Reflex:"));

    mStatsAttackPoints = new Label;
    mStatsDefensePoints = new Label;
    mStatsMagicAttackPoints = new Label;
    mStatsMagicDefensePoints = new Label;
    mStatsAccuracyPoints = new Label;
    mStatsEvadePoints = new Label;
    mStatsReflexPoints = new Label;

    // New labels
    for (int i = 0; i < 6; i++)
    {
        mStatsLabel[i] = new Label("0");
        mStatsLabel[i]->setAlignment(gcn::Graphics::CENTER);
        mStatsDisplayLabel[i] = new Label;
        mPointsLabel[i] = new Label("0");
        mPointsLabel[i]->setAlignment(gcn::Graphics::CENTER);
    }
    mRemainingStatsPointsLabel = new Label;

    // Set button events Id
    mStatsButton[0] = new Button("+", "STR", this);
    mStatsButton[1] = new Button("+", "AGI", this);
    mStatsButton[2] = new Button("+", "VIT", this);
    mStatsButton[3] = new Button("+", "INT", this);
    mStatsButton[4] = new Button("+", "DEX", this);
    mStatsButton[5] = new Button("+", "LUK", this);

    // Assemble
    ContainerPlacer place;
    place = getPlacer(0, 0);

    place(0, 0, mLvlLabel, 3);
    place(5, 0, mJobLvlLabel, 3);
    place(8, 0, mGpLabel, 3);
    place(1, 1, mHpLabel).setPadding(3);
    place(2, 1, mHpBar, 3);
    place(6, 1, mXpLabel).setPadding(3);
    place(7, 1, mXpBar, 3);
    place(1, 2, mMpLabel).setPadding(3);
    place(2, 2, mMpBar, 3);
    place(6, 2, mJobLabel).setPadding(3);
    place(7, 2, mJobBar, 3);
    place.getCell().matchColWidth(0, 1);
    place = getPlacer(0, 3);
    place(0, 1, mStatsTitleLabel, 5).setPadding(5);
    place(5, 1, mStatsTotalLabel, 5).setPadding(5);
    place(12, 1, mStatsCostLabel, 5).setPadding(5);
    for (int i = 0; i < 6; i++)
    {
        place(0, 2 + i, mStatsLabel[i], 7).setPadding(5);
        place(7, 2 + i, mStatsDisplayLabel[i]).setPadding(5);
        place(10, 2 + i, mStatsButton[i]);
        place(12, 2 + i, mPointsLabel[i]).setPadding(5);
    }
    place(14, 2, mStatsAttackLabel, 7).setPadding(5);
    place(14, 3, mStatsDefenseLabel, 7).setPadding(5);
    place(14, 4, mStatsMagicAttackLabel, 7).setPadding(5);
    place(14, 5, mStatsMagicDefenseLabel, 7).setPadding(5);
    place(14, 6, mStatsAccuracyLabel, 7).setPadding(5);
    place(14, 7, mStatsEvadeLabel, 7).setPadding(5);
    place(14, 8, mStatsReflexLabel, 7).setPadding(5);
    place(21, 2, mStatsAttackPoints, 3).setPadding(5);
    place(21, 3, mStatsDefensePoints, 3).setPadding(5);
    place(21, 4, mStatsMagicAttackPoints, 3).setPadding(5);
    place(21, 5, mStatsMagicDefensePoints, 3).setPadding(5);
    place(21, 6, mStatsAccuracyPoints, 3).setPadding(5);
    place(21, 7, mStatsEvadePoints, 3).setPadding(5);
    place(21, 8, mStatsReflexPoints, 3).setPadding(5);
    place(0, 8, mRemainingStatsPointsLabel, 3).setPadding(5);

    Layout &layout = getLayout();
    layout.setRowHeight(0, Layout::AUTO_SET);

    loadWindowState();
}

void StatusWindow::update()
{
    // Status Part
    // -----------
    mLvlLabel->setCaption(strprintf(_("Level: %d"), mPlayer->mLevel));
    mLvlLabel->adjustSize();

    mJobLvlLabel->setCaption(strprintf(_("Job: %d"), mPlayer->mJobLevel));
    mJobLvlLabel->adjustSize();

    mGpLabel->setCaption(strprintf(_("Money: %d GP"), mPlayer->mGp));
    mGpLabel->adjustSize();

    mHpBar->setText(toString(mPlayer->mHp) + "/" + toString(mPlayer->mMaxHp));

    mMpBar->setText(toString(mPlayer->mMp) + "/" + toString(mPlayer->mMaxMp));

    mXpBar->setText(toString(mPlayer->getXp()) + "/" + 
                    toString(mPlayer->mXpForNextLevel));

    mJobBar->setText(toString(mPlayer->mJobXp) + "/" +
                    toString(mPlayer->mJobXpForNextLevel));

    mHpBar->setProgress((float) mPlayer->mHp / mPlayer->mMaxHp);
    mMpBar->setProgress((float) mPlayer->mMp / mPlayer->mMaxMp);

    mXpBar->setProgress((float) mPlayer->getXp() / mPlayer->mXpForNextLevel);
    mJobBar->setProgress((float) mPlayer->mJobXp / mPlayer->mJobXpForNextLevel);

    // Stats Part
    // ----------
    static const char *attrNames[6] = {
        N_("Strength"),
        N_("Agility"),
        N_("Vitality"),
        N_("Intelligence"),
        N_("Dexterity"),
        N_("Luck")
    };
    int statusPoints = mPlayer->mStatsPointsToAttribute;

    // Update labels
    for (int i = 0; i < 6; i++)
    {
        mStatsLabel[i]->setCaption(gettext(attrNames[i]));
        mStatsDisplayLabel[i]->setCaption(toString((int) mPlayer->mAttr[i]));
        mPointsLabel[i]->setCaption(toString((int) mPlayer->mAttrUp[i]));

        mStatsLabel[i]->adjustSize();
        mStatsDisplayLabel[i]->adjustSize();
        mPointsLabel[i]->adjustSize();

        mStatsButton[i]->setEnabled(mPlayer->mAttrUp[i] <= statusPoints);
    }
    mRemainingStatsPointsLabel->setCaption(
            strprintf(_("Remaining Status Points: %d"), statusPoints));
    mRemainingStatsPointsLabel->adjustSize();

    // Derived Stats Points

    // Attack TODO: Count equipped Weapons and items attack bonuses
    mStatsAttackPoints->setCaption(
            toString(mPlayer->ATK + mPlayer->ATK_BONUS));
    mStatsAttackPoints->adjustSize();

    // Defense TODO: Count equipped Armors and items defense bonuses
    mStatsDefensePoints->setCaption(
            toString(mPlayer->DEF + mPlayer->DEF_BONUS));
    mStatsDefensePoints->adjustSize();

    // Magic Attack TODO: Count equipped items M.Attack bonuses
    mStatsMagicAttackPoints->setCaption(
            toString(mPlayer->MATK + mPlayer->MATK_BONUS));
    mStatsMagicAttackPoints->adjustSize();

    // Magic Defense TODO: Count equipped items M.Defense bonuses
    mStatsMagicDefensePoints->setCaption(
            toString(mPlayer->MDEF + mPlayer->MDEF_BONUS));
    mStatsMagicDefensePoints->adjustSize();

    // Accuracy %
    mStatsAccuracyPoints->setCaption(toString(mPlayer->HIT));
    mStatsAccuracyPoints->adjustSize();

    // Evasion %
    mStatsEvadePoints->setCaption(toString(mPlayer->FLEE));
    mStatsEvadePoints->adjustSize();

    // Reflex %
    mStatsReflexPoints->setCaption(toString(mPlayer->DEX / 4)); // + counter
    mStatsReflexPoints->adjustSize();
}

void StatusWindow::draw(gcn::Graphics *g)
{
    update();
    Window::draw(g);
}

void StatusWindow::action(const gcn::ActionEvent &event)
{
    // Stats Part
    if (event.getId().length() == 3)
    {
        if (event.getId() == "STR")
            player_node->raiseAttribute(LocalPlayer::STR);
        if (event.getId() == "AGI")
            player_node->raiseAttribute(LocalPlayer::AGI);
        if (event.getId() == "VIT")
            player_node->raiseAttribute(LocalPlayer::VIT);
        if (event.getId() == "INT")
            player_node->raiseAttribute(LocalPlayer::INT);
        if (event.getId() == "DEX")
            player_node->raiseAttribute(LocalPlayer::DEX);
        if (event.getId() == "LUK")
            player_node->raiseAttribute(LocalPlayer::LUK);
    }
}

void StatusWindow::widgetShown(const gcn::Event& event)
{
    Window::widgetShown(event);

    mHpBar->reset((float) mPlayer->mHp / mPlayer->mMaxHp);
    mMpBar->reset((float) mPlayer->mMp / mPlayer->mMaxMp);
    mXpBar->reset((float) mPlayer->getXp() / mPlayer->mXpForNextLevel);
    mJobBar->reset((float) mPlayer->mJobXp / mPlayer->mJobXpForNextLevel);
}
