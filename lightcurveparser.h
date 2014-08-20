/** Definitions for functions that handle light curve types
 * @file lightcurveMC/lightcurveparser.h
 * @author Krzysztof Findeisen
 * @date Created April 3, 2012
 * @date Last modified April 12, 2013
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

#ifndef LCMCPARSEH
#define LCMCPARSEH

#include <string>
#include <vector>
#include "lightcurvetypes.h"

namespace lcmc { 

/** This namespace identifies data types and functions that handle 
 * model selection for Lightcurve MC.
 */
namespace parse {

/** Returns a list of all light curve names recognized on the command line. 
 */
const std::vector<std::string> lightCurveTypes();

/** Converts a string to its associated @ref models::LightCurveType "LightCurveType".
 */
const models::LightCurveType parseLightCurve(const std::string& lcName);

}}		// end lcmc::parse

#endif		// end ifndef LCMCPARSEH
