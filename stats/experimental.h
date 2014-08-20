/** Functions for trying out new features
 * @file lightcurveMC/stats/experimental.h
 * @author Krzysztof Findeisen
 * @date Created August 3, 2011
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

#ifndef LCMCEXPERIMENTH
#define LCMCEXPERIMENTH

#include <vector>

namespace lcmc { namespace stats {

typedef std::vector<double> DoubleVec;

/** Calculates the RMS binned over ever-larger subintervals of the data
 */
void rmsVsTRooted(const DoubleVec &times, const DoubleVec &fluxes, 
	DoubleVec &timeSteps, DoubleVec & rmsValues);
void rmsVsTAllPairs(const DoubleVec &times, const DoubleVec &fluxes, 
	DoubleVec &timeSteps, DoubleVec & rmsValues);

}}		// end lcmc::stats

#endif		// end LCMCEXPERIMENTH
