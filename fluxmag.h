/** Functions for converting flux and magnitude representations of vectors
 * @file lightcurveMC/fluxmag.h
 * @author Krzysztof Findeisen
 * @date Created April 4, 2013
 * @date Last modified April 11, 2013
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

#ifndef LCMCFLUXMAGH
#define LCMCFLUXMAGH

#include <vector>

namespace lcmc { namespace utils {

/** Function for converting fluxes to magnitudes
 */
double fluxToMag(double flux);

/** Function for converting magnitudes to fluxes
 */
double magToFlux(double mag);

/** Converts fluxes to magnitudes.
 */
void fluxToMag(const std::vector<double>& fluxes, std::vector<double>& mags);

/** Converts magnitudes to fluxes.
 */
void magToFlux(const std::vector<double>& mags, std::vector<double>& fluxes);

}}		// end lcmc::utils

#endif 		// end LCMCFLUXMAGH
