
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


#include "core/predictor.h"

// REVISIT: including SmoothedUniBiTrigramPlugin here because
// PluginManager and Plump are temporarily disabled. Plugin lifetime
// management is currently disabled. Only one plugin is used:
// SmoothedUniBiTrigramPlugin.
//
// IMPORTANT: remove following include when moving back to dynamically
// loaded plugins
//
#include "plugins/smoothedUniBiTrigramPlugin.h"
#include "plugins/abbreviationExpansionPlugin.h"
#include "plugins/dummyPlugin.h"
#include "plugins/dictionaryPlugin.h"
#include "plugins/smoothedCountPlugin.h"

#ifdef DEBUG
# define LOG(x) std::cout << x << std::endl
#else
# define LOG(x) /* x */
#endif


Predictor::Predictor(Profile* prof, ContextTracker* ct)
    : profile(prof), contextTracker(ct)
{
    combiner = 0;

    // read config values
    Variable variable;
    variable.push_back("Soothsayer");
    variable.push_back("Predictor");

    Value value;

    try {
	variable.push_back("PREDICT_TIME");
	value = profile->getConfig(variable);
	LOG("[Predictor] PREDICT_TIME: " + value);
	setPredictTime(toInt(value));
	variable.pop_back();

	variable.push_back("COMBINATION_POLICY");
	value = profile->getConfig(variable);
	LOG("[Predictor] COMBINATION_POLICY: " + value);
	setCombinationPolicy(value);
	variable.pop_back();

	variable.push_back("PLUGINS");
	value = profile->getConfig(variable);
	LOG("[Predictor] PLUGINS: " + value);
	setPlugins(value);
	variable.pop_back();

    } catch (Profile::ProfileException ex) {
	std::cerr << "[Predictor] Caught ProfileException: " << ex.what() << std::endl;
    }
}


Predictor::~Predictor()
{
    removePlugins();
    delete combiner;
}

void Predictor::setPlugins(const std::string& pluginList)
{
    std::stringstream ss(pluginList);
    std::string pluginName;
    while (ss >> pluginName) {
	LOG("[Predictor] Initializing predictive plugin: " + pluginName);
	addPlugin(pluginName);
    }
}

void Predictor::addPlugin(const std::string& pluginName)
{
    // TODO: this will have to do for now, until a proper plugin
    // framework (i.e. plump) is integrated into soothsayer. Until
    // then, all known plugins have to be listed here and explicitly
    // created based on their name.
    //
    Plugin* plugin = 0;
    if (pluginName == "SmoothedUniBiTrigramPlugin") {
	plugin = new SmoothedUniBiTrigramPlugin(profile, contextTracker);
    } else if (pluginName == "AbbreviationExpansionPlugin") {
	plugin = new AbbreviationExpansionPlugin(profile, contextTracker);
    } else if (pluginName == "DummyPlugin") {
	plugin = new DummyPlugin(profile, contextTracker);
    } else if (pluginName == "DictionaryPlugin" ) {
	plugin = new DictionaryPlugin(profile, contextTracker);
    } else if (pluginName == "SmoothedCountPlugin") {
	plugin = new SmoothedCountPlugin(profile, contextTracker);
    } else {
	// TODO: raise exception
	std::cerr << "[Predictor] Error: unable to add plugin: " 
		  << pluginName << std::endl;
	abort();
    }
    
    if (plugin != 0) {
	plugins.push_back (plugin);
	LOG("[Predictor] Activated predictive plugin: " + pluginName);
    }
}

void Predictor::removePlugins()
{
    for (int i = 0; i < plugins.size(); i++) {
	delete plugins[i];
    }
    plugins.clear();
}


// PLUMP callback
//void callback_predict(plump::PluginInstance* plugin, void* data)
//{
//    Prediction* p = reinterpret_cast<Prediction*>(data);
//    Plugin* predictivePlugin = reinterpret_cast<Plugin*>(plugin->instance());
//    *p = predictivePlugin->predict();
//}


Prediction Predictor::predict()
{
    Prediction result;

    // Here goes code to instantiate a separate thread for each Plugin;
    // code can be taken from test file
    // /home/matt/word_predictor/programming_test/threads.cpp
    //

    // All threads need to be synched together. One thread makes sure that
    // we are not exceeding the maximum time allowed.
    //

    // Now that the all threads have exited or have been cancelled,
    // the predictions returned by each of them are combined.
    //

    predictions.clear();
    // for each active predictive plugin...
    for(std::vector<Plugin*>::const_iterator i = plugins.begin();
	i != plugins.end();
	i++) {
	// ...generate and store prediction...
	predictions.push_back ((*i)->predict());
    }

    // ...then merge predictions into a single one...
    result = combiner->combine(predictions);

    // ...and return final prediction
    return result;

    ////////
    // PLUMP
    //
    //plump.registerCallback(callback_predict, &p);
    //plump.run();
}


bool Predictor::setPredictTime( const int predictTime )
{
    // handle exception where predictTime is less than zero
    if( predictTime < 0 ) {
        std::cerr << "Error: attempted to set PREDICT_TIME option to "
                  << "a negative integer value. Please make sure that "
                  << "PREDICT_TIME option is set to a value greater "
                  << "than or equal to zero.\a" << std::endl;
        return false;
    } else {
        LOG("[Predictor] Setting PREDICT_TIME to " << predictTime);
        PREDICT_TIME = predictTime;
        return true;
    }
}


int Predictor::getPredictTime() const
{
    return PREDICT_TIME;
}


void Predictor::setCombinationPolicy(const std::string cp)
{
    LOG("[Predictor] Setting COMBINATION_POLICY to " << cp);
    delete combiner;
    combinationPolicy = cp;

    std::string policy = strtolower (cp);
    if (policy == "meritocracy") {
	combiner = new MeritocracyCombiner();
    } else {
	// TODO: throw exception
	std::cerr << "[Predictor] Error - unknown combination policy: "
		  << cp << std::endl;
    }
}


std::string Predictor::getCombinationPolicy() const
{
    return combinationPolicy;
}

void Predictor::setProfile(Profile* prof)
{
    delete profile;
    profile = prof;
}
