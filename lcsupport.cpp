/** Definitions for lightcurveMC supporting infrastructure
 * @file lcsupport.cpp
 * @author Krzysztof Findeisen
 * @date Created April 20, 2012
 * @date Last modified May 29, 2013
 * 
 * The functions defined here act as an interface between the LightCurve class 
 * heirarchy and the main program.
 */

#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <cstdio>
#include "lightcurveparser.h"
#include "lightcurvetypes.h"

using std::string;
using std::vector;

namespace lcmc {

namespace models {

/** Helper type for streamlining the implementation of functions that 
 * construct LightCurves
 */
typedef  std::map<string, const LightCurveType> LightCurveRegistry;

/** Implements a global registry of light curves.
 */
const LightCurveRegistry& getLightCurveRegistry();

}		// end lcmc::models

namespace parse {

using models::LightCurveRegistry;
using models::getLightCurveRegistry;

/** Returns a list of all lightCurve names recognized on the command line. 
 * 
 * @return A list of string identifiers, one for each allowed LightCurveType 
 *	value. The order of the strings may be whatever is most convenient for 
 *	enumerating the types of light curves.
 */
const std::list<string> lightCurveTypes() {
	const LightCurveRegistry& registry = getLightCurveRegistry();
	std::list<string> lcList;
	
	for(LightCurveRegistry::const_iterator it = registry.begin(); 
			it != registry.end(); it++) {
		lcList.push_back(it->first);
	}
	
	return lcList;
}

/** Converts a string to its associated LightCurveType.
 *
 * @param[in] lcName A short string describing the type of light curve to generate.
 *
 * @pre lcName is an element of lightCurveTypes()
 * 
 * @return The light curve identifier, if one exists.
 * 
 * @exception domain_error Thrown if the light curve name is does not have a 
 *	corresponding type.
 */
const models::LightCurveType parseLightCurve(const string& lcName) {
	const LightCurveRegistry& registry = getLightCurveRegistry();
	
	LightCurveRegistry::const_iterator it = registry.find(lcName);
	if (it != registry.end()) {
		return it->second;
	} else {
		throw std::domain_error("No such light curve: " + lcName);
	}
}

}		// end lcmc::parse

}		// end lcmc
