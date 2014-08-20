/** Functions for testing approximately equal values
 * @file lightcurveMC/approx.h
 * @author Krzysztof Findeisen
 * @date Created April 30, 2013
 * @date Last modified April 30, 2013
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

#ifndef LCMCAPPROXH
#define LCMCAPPROXH

#include <gsl/gsl_matrix.h>

namespace lcmc { namespace utils {

/** Function object for testing whether two values are approximately equal
 */
class ApproxEqual {
public: 
	/** Defines an object testing for approximate equality with 
	 *	a particular tolerance
	 */
	explicit ApproxEqual(double epsilon);

	/** Tests whether two values are approximately equal
	 */
	bool operator() (double x, double y) const;

private:
	double epsilon;
};

/** Tests whether two matrices have approximately equal elements
 */
bool isMatrixClose(const gsl_matrix * const a, const gsl_matrix * const b, 
		double tolerance);

}}		// end lcmc::utils

#endif		// end LCMCAPPROXH
