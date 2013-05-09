/** Definitions for lightcurveMC supporting infrastructure
 * @file statsupport.cpp
 * @author Krzysztof Findeisen
 * @date Created May 9, 2013
 * @date Last modified May 9, 2013
 * 
 * The functions defined here act as an interface between the StatType 
 * parameters and the parser.
 */

#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "binstats.h"
#include "statparser.h"

using std::string;
using std::vector;

namespace lcmc { namespace stats {

/** Helper type for streamlining the implementation of functions that 
 * construct or select statistics
 */
typedef  std::map<string, const StatType> StatRegistry;
/** Helper type for streamlining the implementation of functions that 
 * construct or select statistics
 */
typedef std::pair<string, const StatType> StatEntry;

/** Implements a global registry of statistics.
 */
const StatRegistry& getStatRegistry() {
	static StatRegistry registry;
	static bool called = false;
	if (called == false) {
		registry.clear();
		
		// Original waveforms
		registry.insert(StatEntry(     "C1" , C1         ));
		registry.insert(StatEntry( "period" , PERIOD     ));
		registry.insert(StatEntry("periplot", PERIODOGRAM));
		registry.insert(StatEntry("dmdtcut" , DMDTCUT    ));
		registry.insert(StatEntry("dmdtplot", DMDT       ));
		registry.insert(StatEntry( "acfcut" , ACFCUT     ));
		registry.insert(StatEntry( "acfplot", ACF        ));
		
		called = true;
	}

	// assert: registry is fully defined
	
	return registry;
}

}		// end lcmc::stats

namespace parse {

using namespace stats;

/** Returns a list of all statistic names recognized on the command line. 
 * 
 * @return A list of string identifiers, one for each allowed StatType 
 *	value. The order of the strings may be whatever is most convenient for 
 *	enumerating the types of statistics.
 */
const vector<string> statTypes() {
	const StatRegistry& registry = getStatRegistry();
	std::vector<string> theList;
	
	for(StatRegistry::const_iterator it = registry.begin(); 
			it != registry.end(); it++) {
		theList.push_back(it->first);
	}
	
	return theList;
}

/** Converts a string to its associated StatType.
 *
 * @param[in] statName A short string describing the type of statistic to calculate.
 *
 * @pre statName is an element of statTypes()
 * 
 * @return The statistic identifier, if one exists.
 * 
 * @exception domain_error Thrown if the statistic name does not have a 
 *	corresponding type.
 */
StatType parseStat(const string& statName) {
	const StatRegistry& registry = getStatRegistry();

	StatRegistry::const_iterator it = registry.find(statName);
	if (it != registry.end()) {
		return it->second;
	} else {
		throw std::domain_error("No such light curve: " + statName);
	}
}

}}		// end lcmc::parse
