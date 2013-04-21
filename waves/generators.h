/** Helper functions for generating light curves
 * @file generators.h
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified April 18, 2013
 */

#ifndef LCMCGENERATORSH
#define LCMCGENERATORSH

#include <vector>
#include <gsl/gsl_matrix.h>

namespace lcmcutils {

/** Transforms an uncorrelated sequence of Gaussian random numbers into a 
 *	correlated sequence
 */
void multiNormal(const std::vector<double>& indVec, const gsl_matrix& covar, 
		std::vector<double>& corrVec);

}		// end lcmcutils

#endif		// LCMCGENERATORSH
