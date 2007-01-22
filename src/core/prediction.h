
/******************************************************************************\
 *                                                                            *
 * Soothsayer, an extensible predictive text entry system                     *
 * ------------------------------------------------------                     *
 *                                                                            *
 * Copyright (C) 2004  Matteo Vescovi <matteo.vescovi@tiscali.it>             *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 *                                                                            *
\******************************************************************************/        


#ifndef SOOTH_PREDICTION
#define SOOTH_PREDICTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>

#include "suggestion.h"


/** Prediction
 *
 * A prediction contains a set of Suggestion objects.  More precisely,
 * a Prediction is a list of Suggestion object, ordered by decreasing
 * probability.
 *
 * A Prediction object is returned by the predictive plugins and by a
 * combiner object.
 * 
 */
class Prediction {
    friend std::ostream &operator<<( std::ostream &, const Prediction & );

public:
    Prediction();
    ~Prediction();

    const Prediction &operator=( const Prediction & );

    int getSize() const;                   // returns number of suggestions
    Suggestion getSuggestion(int=0) const; // returns nth most
    // probable suggestion

    /** Inserts a new suggestion, preserves the ordering.
     * 
     * The suggestion object to be inserted is compared against the
     * suggestions already contained in the prediction and inserted in an
     * ordered fashion.
     *
     * Comparison between suggestion objects uses the overloaded operator<
     *
     */
    void addSuggestion( Suggestion );
    
    /** Returns a string representation of the prediction.
     */
    std::string toString() const;
private:
    std::vector<Suggestion> suggestions;

};


#endif // SOOTH_PREDICTION
