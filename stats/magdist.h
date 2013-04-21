/** Test statistics based on examining the magnitude distribution
 * @file magdist.h
 * @author Krzysztof Findeisen
 * @date Created April 12, 2011
 * @date Last modified April 12, 2013
 */

#include <vector>

#if USELFP
#include <lfp/lfp.h>
#endif

namespace lcmcstats {

/** Calculates the modified C1 statistic. 
 */
double getC1(const std::vector<double>& mags);

/** Calculates the light curve amplitude. 
 */
double getAmplitude(const std::vector<double>& mags);

};	// end lcmcstats
