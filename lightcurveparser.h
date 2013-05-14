/** Definitions for functions that handle light curve types
 * @file lightcurveparser.h
 * @author Krzysztof Findeisen
 * @date Created April 3, 2012
 * @date Last modified April 12, 2013
 */

#ifndef LCMCPARSEH
#define LCMCPARSEH

#include <string>
#include <vector>
#include "lightcurvetypes.h"

namespace lcmc { 

/** This namespace identifies data types and functions that handle 
 * model selection for lightcurveMC.
 */
namespace parse {

/** Returns a list of all lightCurve names recognized on the command line. 
 */
const std::vector<std::string> lightCurveTypes();

/** Converts a string to its associated LightCurveType.
 */
const models::LightCurveType parseLightCurve(const std::string& lcName);

}}		// end lcmc::parse

#endif		// end ifndef LCMCPARSEH
