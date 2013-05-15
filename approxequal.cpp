/** Predicate for floating-point comparisons
 * @file lightcurveMC/approxequal.cpp
 * @author Krzysztof Findeisen
 * @date Created April 30, 2013
 * @date Last modified May 9, 2013
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
 * @pre[in] epsilon > 0
 * 
 * @post ApproxEqual will accept a pair of values as equal 
 *	iff |val1 - val2|/|val1| and |val1 - val2|/|val2| < epsilon
 * 
 * @exception std::invalid_argument Thrown if epsilon <= 0
 *
 * @exceptsafe Object creation is atomic.
 *
 * @todo Verify that the postcondition is met
 */
ApproxEqual::ApproxEqual(double epsilon) : epsilon(epsilon) {
	if (epsilon <= 0.0) {
		throw std::invalid_argument("Cannot do approximate comparison with a nonpositive precision: " + boost::lexical_cast<std::string>(epsilon));
	}
}

/** Tests whether two values are approximately equal
 *
 * @param[in] x, y The values to compare
 *
 * @pre[in] x &ne; 0
 * @pre[in] y &ne; 0
 *
 * @exception std::invalid_argument Thrown if x == 0 or y == 0
 *
 * @exceptsafe The parameters and object state are unchanged in the event 
 *	of an exception.
 *
 * @return true iff |x - y|/|x| and |x - y|/|y| < epsilon
 */
bool ApproxEqual::operator() (double x, double y) const {
	if (x == 0.0 || y == 0.0) {
		throw std::invalid_argument("Cannot do approximate comparison to zero.");
	}
	// assert: gsl_fcmp() will not invoke the error handler for 
	//	the allowed arguments
	return (gsl_fcmp(x, y, epsilon) == 0);
}

/** Tests whether two matrices have approximately equal elements
 *
 * @param[in] a The first matrix to compare
 * @param[in] b The second matrix to compare
 * @param[in] tolerance The fractional tolerance to which a[i,j] and b[i,j] must match
 *
 * @pre tolerance > 0
 *
 * @return True iff a and b have the same dimensions, and each 
 *	corresponding element is equal to within tolerance
 *
 * @note if either a or b is a null pointer, returns false
 * 
 * @exception std::invalid_argument Thrown if tolerance <= 0
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
