/** Functions for handling data containing NaN values
 * @file lightcurveMC/nan.h
 * @author Krzysztof Findeisen
 * @date Created April 11, 2013
 * @date Last modified April 21, 2013
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

#ifndef LCMCNANH
#define LCMCNANH

#include <vector>

namespace lcmc { 

/** This namespace contains miscellaneous functions for handling generic 
 *	problems.
 */
namespace utils {

using std::vector;

/** lessFinite() allows floating-point numbers to be ordered consistently in the 
 *	presence of NaNs.
 */
bool lessFinite(double x, double y);

/** Removes nans from a pair of vectors.
 */
void removeNans(const vector<double>& badVals, vector<double>& goodVals, 
		const vector<double>& sideVals, vector<double>& matchVals);

/** Calculates the mean, ignoring NaNs
 */
double meanNoNan(const vector<double>& vals);

/** Calculates the variance, ignoring NaNs
 */
double varianceNoNan(const vector<double>& vals);

}}	// end lcmc::utils

#endif	// ifndef LCMCNANH
