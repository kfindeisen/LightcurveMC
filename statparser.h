/** Definitions for functions that handle statistic types
 * @file lightcurveMC/statparser.h
 * @author Krzysztof Findeisen
 * @date Created May 9, 2013
 * @date Last modified May 9, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
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

/** Converts a string to its associated @ref stats::StatType "StatType".
 */
lcmc::stats::StatType parseStat(const std::string& statName);

}}		// end lcmc::parse

#endif		// end ifndef LCMCSTATPARSEH
