/** Definitions for lightcurveMC supporting infrastructure
 * @file lightcurveMC/statsupport.cpp
 * @author Krzysztof Findeisen
 * @date Created May 9, 2013
 * @date Last modified May 15, 2013
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
 *
 * @return The registry.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to create 
 *	the registry.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
const StatRegistry& getStatRegistry() {
	// invariant: registry contains the names of all valid statistics, 
	//	or called == false
	static StatRegistry registry;
	static bool called = false;
	if (called == false) {
		registry.clear();
		
		// Original waveforms
		registry.insert(StatEntry(      "C1", C1         ));
		registry.insert(StatEntry("period"  , PERIOD     ));
		registry.insert(StatEntry("periplot", PERIODOGRAM));
		registry.insert(StatEntry("dmdtcut" , DMDTCUT    ));
		registry.insert(StatEntry("dmdtplot", DMDT       ));
		registry.insert(StatEntry("iacfcut" , IACFCUT    ));
		registry.insert(StatEntry("iacfplot", IACF       ));
		registry.insert(StatEntry("sacfcut" , SACFCUT    ));
		registry.insert(StatEntry("sacfplot", SACF       ));
		registry.insert(StatEntry("peakcut" , PEAKCUT    ));
		registry.insert(StatEntry("peakplot", PEAKFIND   ));
		
		// No exceptions past this point
		// To preserve the invariant in the face of exceptions, 
		//	only set called flag when we know the registry 
		//	is ready
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
 *
 * @exception std::bad_alloc Thrown if not enough memory to construct the 
 *	list, or if there is not enough memory to enumerate the 
 *	allowed statistics.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
const vector<string> statTypes() {
	// Let the exception propagate up: if there's not enough memory to 
	// call getStatRegistry(), there's not enough memory to do anything 
	// with the statistic names anyway
	const StatRegistry& registry = getStatRegistry();
	
	std::vector<string> theList;
	theList.reserve(registry.size());
	
	// no exceptions past this point
	
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
 * @exception std::bad_alloc Thrown if not enough memory to match names 
 *	to statistics.
 *
 * @exceptsafe The arguments are unchanged in the event of an exception.
 *
 * @todo Is there a way to handle bad_alloc internally? Letting it get 
 *	thrown exposes the internal implementation.
 */
StatType parseStat(const string& statName) {
	const StatRegistry& registry = getStatRegistry();

	StatRegistry::const_iterator it = registry.find(statName);
	if (it != registry.end()) {
		return it->second;
	} else {
		throw std::domain_error("No such statistic: " + statName);
	}
}

}}		// end lcmc::parse
