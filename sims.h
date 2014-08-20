/** Drivers for major simulation steps
 * @file lightcurveMC/sims.h
 * @author Krzysztof Findeisen
 * @date Created May 26, 2013
 * @date Last modified May 26, 2013
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

#ifndef LCMCSIMH
#define LCMCSIMH

#include <string>
#include <vector>
#include "lightcurvetypes.h"
#include "paramlist.h"

namespace lcmc {

using std::string;
using std::vector;

/** Randomly generates parameter values within the specified limits
 */
models::ParamList drawParams(const models::RangeList& limits);

/** Returns the time stamps corresponding to a particular input file.
 */
void makeTimes(const string& dateList, vector<double>& times);

/** Generates white noise corresponding to a given cadence.
 */
void makeWhiteNoise(const vector<double>& times, double sigma, vector<double>& noise);

/** Generates an observed light curve for an injection analysis.
 */
void makeInjectNoise(const string& catalog, 
		vector<double>& times, vector<double>& baseFlux);

/** Generates a random light curve, incorporating all the simulation settings.
 */
void simLightCurve(const models::LightCurveType& curve, const models::ParamList& params, 
		const vector<double>& times, const vector<double>& noise, 
		vector<double>& lcFluxes);

}	// end lcmc

#endif	// LCMCSIMH
