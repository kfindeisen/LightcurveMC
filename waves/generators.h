/** Helper functions for generating light curves
 * @file lightcurveMC/waves/generators.h
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 6, 2013
 */

#ifndef LCMCGENERATORSH
#define LCMCGENERATORSH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_matrix.h>

namespace lcmc { namespace utils {

/** Transforms an uncorrelated sequence of Gaussian random numbers into a 
 *	correlated sequence
 */
void multiNormal(const std::vector<double>& indVec, const boost::shared_ptr<gsl_matrix>& covar, 
		std::vector<double>& corrVec);

}}		// end lcmc::utils

#endif		// LCMCGENERATORSH
