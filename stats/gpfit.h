/** Functions for fitting Gaussian Process models to data
 * @file lightcurveMC/stats/gpfit.h
 * @author Krzysztof Findeisen
 * @date Created June 24, 2013
 * @date Last modified June 24, 2013
 */

#ifndef LCMCGPFITH
#define LCMCGPFITH

#include <vector>

namespace lcmc { namespace stats {

using std::vector;

/** Finds the best fit solution to a squared exponential Gaussian process model
 */
void fitGaussGp(const vector<double>& times, const vector<double>& data, 
		double& timescale, double& timeError);

}}		// end lcmc::stats

#endif		// end LCMCGPFITH
