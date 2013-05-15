/** Autocorrelation functions using interpolation
 * @file lightcurveMC/stats/acfinterp.h
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 6, 2013
 * @date Last modified May 7, 2013
 */

#ifndef LCMCACFINTERPH
#define LCMCACFINTERPH

#include <vector>

namespace lcmc { namespace stats {

typedef std::vector<double> DoubleVec;

/** The interp namespace is intended to allow other ACF algorithms to be 
 *	swapped with a simple using declaration.
 */
namespace interp {

/** Calculates the autocorrelation function for a time series. 
 */
void autoCorr(const DoubleVec &times, const DoubleVec &data, 
		double deltaT, size_t nAcf, DoubleVec &acf);

}		// end lcmc::stats::interp

}}		// end lcmc::stats

#endif		// end LCMCACFINTERPH
