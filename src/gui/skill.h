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

#ifndef _SKILL_H
#define _SKILL_H

#include <allegro.h>
#include "button.h"
#include "../main.h"

struct SKILL {
    short id; //index into "skill_db" array
    short lv, sp;
};

class SkillListModel : public gcn::ListModel
{
    std::vector<SKILL*> skillList;
    public:
        SkillListModel();
        ~SkillListModel();

        int getNumberOfElements();
        std::string getElementAt(int);

        bool hasSkill(int id);
        void addSkill(int id, int lv, int sp);
        void setSkill(int id, int lv, int sp);
};

class SkillDialog : public Window, public gcn::ActionListener
{
    gcn::ListBox *skillListBox;
    SkillListModel *skills;
    gcn::Label *pointsLabel;

    Button *incButton;
    Button *closeButton;
    public:
        SkillDialog(gcn::Container *);
        ~SkillDialog();

        void action(const std::string&);
        SkillListModel* getModel() { return skills; }

        void setPoints(int i);
};

#endif
