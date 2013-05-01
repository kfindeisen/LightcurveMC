/** Functions for testing approximately equal values
 * @file approx.h
 * @author Krzysztof Findeisen
 * @date Created April 30, 2013
 * @date Last modified April 30, 2013
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
