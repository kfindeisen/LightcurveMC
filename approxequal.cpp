/** Predicate for floating-point comparisons
 * @file lightcurveMC/approxequal.cpp
 * @author Krzysztof Findeisen
 * @date Created April 30, 2013
 * @date Last modified May 24, 2013
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

#include <stdexcept>
#include <string>
#include <boost/lexical_cast.hpp>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include "approx.h"

namespace lcmc { namespace utils {

/** Defines an object testing for approximate equality with 
 *	a particular tolerance
 *
 * @param[in] epsilon The maximum fractional difference between two 
 *	values that are considered equal
 *
 * @pre[in] @p epsilon > 0
 * 
 * @post ApproxEqual will accept a pair of values as equal 
 *	iff |val1 - val2|/|val1| and |val1 - val2|/|val2| < @p epsilon
 * 
 * @exception std::invalid_argument Thrown if @p epsilon &le; 0
 *
 * @exceptsafe Object creation is atomic.
 */
ApproxEqual::ApproxEqual(double epsilon) : epsilon(epsilon) {
	if (epsilon <= 0.0) {
		throw std::invalid_argument("Cannot do approximate comparison with a nonpositive precision: gave " + boost::lexical_cast<std::string>(epsilon));
	}
}

/** Tests whether two values are approximately equal
 *
 * @param[in] x, y The values to compare
 *
 * @return true iff |@p x - @p y|/|@p x| and |@p x - @p y|/|@p y| < @p epsilon
 *
 * @pre @p x &ne; 0
 * @pre @p y &ne; 0
 *
 * @exception std::invalid_argument Thrown if @p x = 0 or @p y = 0
 *
 * @exceptsafe The parameters and object state are unchanged in the event 
 *	of an exception.
 */
bool ApproxEqual::operator() (double x, double y) const {
	if (x == 0.0 || y == 0.0) {
		throw std::invalid_argument("Cannot do approximate comparison to zero.");
	}
	
	// gsl_fcmp does not have the desired behavior (for starters, it does 
	//	an OR test, not an AND test), so just implement the 
	//	definition directly
	double diff = fabs(x - y);
	return (diff < epsilon * fabs(x) && diff < epsilon * fabs(y));
}

/** Tests whether two matrices have approximately equal elements
 *
 * @param[in] a The first matrix to compare
 * @param[in] b The second matrix to compare
 * @param[in] tolerance The fractional tolerance to which <tt>a[i,j]</tt> 
 *	and <tt>b[i,j]</tt> must match
 *
 * @return True iff @p a and @p b have the same dimensions, and each 
 *	corresponding element is equal to within tolerance
 *
 * @note if either @p a or @p b is a null pointer, returns false
 *
 * @pre @p tolerance > 0
 * 
 * @exception std::invalid_argument Thrown if @p tolerance &le; 0
 *
 * @exceptsafe The parameters are unchanged in the event of an exception.
 */
bool isMatrixClose(const gsl_matrix * const a, const gsl_matrix * const b, 
		double tolerance) {
	if (tolerance <= 0.0) {
		throw std::invalid_argument("Cannot do matrix comparison with a nonpositive tolerance: " + boost::lexical_cast<std::string>(tolerance));
	}

	// Invalid matrices
	if(a == NULL || b == NULL) {
		return false;

	// Mismatched matrices
	} else if (a->size1 != b->size1 || a->size2 != b->size2) {
		return false;
	
	// Element-by-element comparison
	} else {
		for(size_t i = 0; i < a->size1; i++) {
			for(size_t j = 0; j < a->size2; j++) {
				// assert: indices to gsl_matrix_get() are valid, so 
				//	error handler will not be invoked
				if (gsl_fcmp(gsl_matrix_get(a, i, j), gsl_matrix_get(b, i, j), 
						tolerance) != 0) {
					return false;
				}
			}
		}
		
		// All elements compared equal
		return true;
	}
}

}}		// end lcmc::utils
