
/******************************************************
 *  Presage, an extensible predictive text entry system
 *  ---------------------------------------------------
 *
 *  Copyright (C) 2008  Matteo Vescovi <matteo.vescovi@yahoo.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
                                                                             *
                                                                **********(*)*/


#ifndef PRESAGE_PROFILEMANAGER
#define PRESAGE_PROFILEMANAGER

#include "core/profile.h"
#include "tinyxml/tinyxml.h"
#include "core/combiner.h"
#include "core/logger.h"

#include <string>
#include <list>
#include <sstream> // for std::ostringstream
#include <stdio.h> // for int remove( const char* ) to remove files/dirs


/** Juggles configuration files and presage system initialization.
 *
 * ProfileManager loads up a bunch of xml files containing
 * configuration data and initializes a configuration object.
 * 
 * 1. write config with data from /etc/presage.xml if it exists
 * 2. overwrite config with data from  <sysconfdir>/presage.xml if it exists
 * 3. overwrite config with data from  ~/.presage.xml if it exists
 * 4. overwrite config with data from profile passed to constructor (if non-empty string)
 *
 */
class ProfileManager {
public:
    ProfileManager(const std::string = "");
    ~ProfileManager();

    TiXmlDocument* buildProfile(const std::string = DEFAULT_PROFILE_FILENAME);
    void saveProfile() const;

    Configuration* get_configuration();
    
    static const char*        DEFAULT_PROFILE_FILENAME;
    static const std::string  DEFAULT_LOGGER_LEVEL;
    static const int          DEFAULT_PREDICT_TIME;
    static const int          DEFAULT_MAX_PARTIAL_PREDICTION_SIZE;
    static const std::string  DEFAULT_COMBINATION_POLICY;
    static const std::string  DEFAULT_PREDICTIVE_PLUGINS;
    static const int          DEFAULT_SLIDING_WINDOW_SIZE;
    static const size_t       DEFAULT_SUGGESTIONS;
    static const bool         DEFAULT_REPEAT_SUGGESTION;
    static const size_t       DEFAULT_GREEDY_SUGGESTION_THRESHOLD;
    static const std::string  DEFAULT_PLUGINS;

private:
    static const char* LOGGER;

    void init_profiles (const std::string& profilename);

    void read_profiles_into_configuration ();

    std::string get_user_home_dir() const;
    
    /** Cache log message until logger level is read from configuration.
     */
    void cache_log_message(Logger<char>::Level level, const std::string& message);
    /** Flush cached log messages.
     */
    void flush_cached_log_messages();
    /** Refresh configuration variables
     */
    void refresh_config();

    struct CachedLogMessage {
	// Level is commented out because it gets translated to
	// numeric value of enum, instead of acting as templatized
	// function type switching the logger level as intended.  This
	// is not an issue however since all cached log messages need
	// to be logged at same level, so we can hardcode level in
	// flush method.
	//
	//Logger<char>::Level level;
	std::string         message;
    };

    std::list<CachedLogMessage> cached_log_messages;

    std::list<std::string> profiles;

    Configuration* configuration;

    Logger<char>    logger;
};


#endif // PRESAGE_PROFILEMANAGER
