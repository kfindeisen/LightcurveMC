/** Definitions for functions that handle light curve types
 * @file lightcurveparser.h
 * @author Krzysztof Findeisen
 * @date Created April 3, 2012
 * @date Last modified April 12, 2013
 */

#ifndef LCMCPARSEH
#define LCMCPARSEH

#include <list>
#include <string>
#include "lightcurvetypes.h"

/** This namespace identifies data types and functions that handle the 
 *	simulation of astronomical data.
 */
namespace lcmcparse {

/** Returns a list of all lightCurve names recognized on the command line. 
 */
const std::list<std::string> lightCurveTypes();

/** Converts a string to its associated LightCurveType.
 */
const lcmcmodels::LightCurveType parseLightCurve(const std::string& lcName);

}		// end lcmcparse

#endif		// end ifndef LCMCPARSEH
