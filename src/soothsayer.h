
/*****************************************************************************\
 *                                                                           *
 * Soothsayer, an extensible predictive text entry system                    *
 * ------------------------------------------------------                    *
 *                                                                           *
 * Copyright (C) 2004  Matteo Vescovi <matteo.vescovi@yahoo.co.uk>           *
 *                                                                           *
 * This program is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU General Public License               *
 * as published by the Free Software Foundation; either version 2            *
 * of the License, or (at your option) any later version.                    *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*
 *                                                                           *
\*****************************************************************************/


#ifndef SOOTHSAYER
#define SOOTHSAYER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>

class ProfileManager;
class Profile;
class HistoryTracker;
class Predictor;
class Selector;

/** Soothsayer, the intelligent predictive text entry platform.
 *
 */
class Soothsayer {
  public:
    Soothsayer();
    ~Soothsayer();
	
    std::vector<std::string> predict(std::string);
    std::vector<std::string> predict(char);

    void update(std::string);
    void update(char);

    void complete(std::string);

    std::string history() const;

  private:
    ProfileManager* profileManager;
    Profile*        profile;
    HistoryTracker* historyTracker;
    Predictor*      predictor;
    Selector*       selector;

};


#endif // SOOTHSAYER
