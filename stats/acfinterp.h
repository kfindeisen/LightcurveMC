/** Autocorrelation functions using interpolation
 * @file lightcurveMC/stats/acfinterp.h
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 6, 2013
 * @date Last modified May 7, 2013
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

#ifndef LCMCACFINTERPH
#define LCMCACFINTERPH

#include <vector>

namespace lcmc { namespace stats {

typedef std::vector<double> DoubleVec;

/** The @ref lcmc::stats::interp "interp" namespace is intended to allow 
 *	other ACF algorithms to be swapped with a simple @c using declaration.
 */
namespace interp {

/** Calculates the autocorrelation function for a time series. 
 */
void autoCorr(const DoubleVec &times, const DoubleVec &data, 
		double deltaT, size_t nAcf, DoubleVec &acf);

}		// end lcmc::stats::interp

}}		// end lcmc::stats

#endif		// end LCMCACFINTERPH
