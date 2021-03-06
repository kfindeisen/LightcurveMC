/** Functions for converting flux and magnitude representations of vectors
 * @file lightcurveMC/fluxmag.cpp
 * @author Krzysztof Findeisen
 * @date Created April 4, 2013
 * @date Last modified May 13, 2013
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

#include <algorithm>
#include <vector>
#include <cmath>
#include "fluxmag.h"

namespace lcmc { namespace utils {

using std::vector;

/** Function for converting fluxes to magnitudes
 * 
 * @param[in] flux The flux to convert.
 *
 * @return The corresponding magnitude, assuming a flux of 1 unit corresponds 
 *	to a magnitude of 0. Returns NaN if the flux is negative.
 *
 * @exceptsafe Does not throw exceptions.
 */
double fluxToMag(double flux) {
	return -2.5*log10(flux);
}

/** Function for converting magnitudes to fluxes
 *
 * @param[in] mag The magnitude to convert.
 *
 * @return The corresponding flux, assuming a flux of 1 unit at 0th magnitude.
 *
 * @exceptsafe Does not throw exceptions.
 */
double magToFlux(double mag) {
	return pow(10.0, -0.4*mag);
}

/** Converts fluxes to magnitudes.
 * 
 * @param[in] fluxes A vector of fluxes to convert.
 * @param[out] mags The corresponding magnitudes, assuming a flux of 1 unit 
 *	corresponds to a magnitude of 0. Returns NaN wherever @p flux is negative.
 *
 * @pre @p mags may refer to the same vector as @p fluxes
 *
 * @post @p mags.size() = @p fluxes.size()
 * @post for all i in @p mags, @p mags[i] = @p fluxToMag(fluxes[i])
 *
 * @exception std::bad_alloc Thrown if there is not enough room 
 *	to store the magnitudes
 *
 * @exceptsafe The arguments are unchanged in the event of an exception
 */
void fluxToMag(const vector<double>& fluxes, vector<double>& mags) {
	using std::swap;
	
	// Explicit declaration to disambiguate which fluxToMag is being called
	static double (* fp)(double) = fluxToMag;

	vector<double> temp(fluxes.size());
	std::transform(fluxes.begin(), fluxes.end(), temp.begin(), fp);
	
	swap(mags, temp);
}

/** Converts magnitudes to fluxes.
 *
 * @param[in] mags A vector of magnitudes to convert.
 * @param[out] fluxes The corresponding fluxes, assuming a flux of 1 unit at 
 *	0th magnitude.
 *
 * @pre @p fluxes may refer to the same vector as @p mags
 * 
 * @post @p fluxes.size() = @p mags.size()
 * @post for all i in @p mags, @p fluxes[i] = @p magToFlux(mags[i])
 *
 * @exception std::bad_alloc Thrown if there is not enough room 
 *	to store the fluxes
 *
 * @exceptsafe The arguments are unchanged in the event of an exception
 */
void magToFlux(const vector<double>& mags, vector<double>& fluxes) {
	using std::swap;

	// Explicit declaration to disambiguate which magToFlux is being called
	static double (* fp)(double) = magToFlux;

	vector<double> temp(mags.size());
	std::transform(mags.begin(), mags.end(), temp.begin(), fp);
	
	swap(fluxes, temp);
}

}}	// end lcmc::utils
