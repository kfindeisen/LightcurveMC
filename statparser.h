/** Definitions for functions that handle statistic types
 * @file lightcurveMC/statparser.h
 * @author Krzysztof Findeisen
 * @date Created May 9, 2013
 * @date Last modified May 9, 2013
 */

#ifndef LCMCSTATPARSEH
#define LCMCSTATPARSEH

#include <vector>
#include <string>
#include "binstats.h"

namespace lcmc { namespace parse {

/** Returns a list of all statistic names recognized on the command line. 
 */
const std::vector<std::string> statTypes();

/** Converts a string to its associated StatType.
 */
lcmc::stats::StatType parseStat(const std::string& statName);

}}		// end lcmc::parse

#endif		// end ifndef LCMCSTATPARSEH
