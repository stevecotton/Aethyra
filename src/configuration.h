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

#ifndef __INIREAD_H
#define __INIREAD_H

#define INI_DELIMITER "="
#define INI_COMMENTER "#"

#include <map>
#include <string>

/**
 * INI configuration handler for reading (and writing).
 *
 * \ingroup CORE
 */
class Configuration {
    public:
        /**
         * \brief Reads INI file and parse all options into memory.
         * \param filename Full path to INI file (~/.manaworld/tmw.ini)
         */
        void init(std::string filename);

        /**
         * \brief Writes the current settings back to an ini-file.
         * \param filename Full path to INI file (~/.manaworld/tmw.ini)
         */
        bool write(std::string filename);

        /**
         * \brief Sets an option using a string value.
         * \param key Option identifier.
         * \param value Value.
         */
        void setValue(std::string key, std::string value);

        /**
         * \brief Sets an option using a numeric value.
         * \param key Option identifier.
         * \param value Value.
         */
        void setValue(std::string key, float value);

        /**
         * \brief Gets a value as string.
         * \param key Option identifier.
         * \param deflt Default option if not there or error.
         */
        std::string getValue(std::string key, std::string deflt);

        /**
         * \brief Gets a value as numeric (float).
         * \param key Option identifier.
         * \param deflt Default option if not there or error.
         */
        float getValue(std::string key, float delflt);
    private:
        /**
         * Returns wether they given key exists.
         */
        bool keyExists(std::string key);

        /**
         * A simple data structure to store the value of a configuration
         * option.
         */
        class OptionValue {
            public:
                /**
                 * Constructor.
                 */
                OptionValue();

                std::string stringValue;
                float numericValue;
        };

        std::map<std::string, OptionValue> iniOptions;
        std::map<std::string, OptionValue>::iterator iter;
};

#endif
