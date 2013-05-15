/** Test statistics based on examining the magnitude distribution
 * @file lightcurveMC/stats/magdist.h
 * @author Krzysztof Findeisen
 * @date Created April 12, 2011
 * @date Last modified April 12, 2013
 */

#ifndef LCMCMAGDISTH
#define LCMCMAGDISTH

#include <vector>

namespace lcmc { namespace stats {

/** Calculates the modified C1 statistic. 
 */
double getC1(const std::vector<double>& mags);

/** Calculates the light curve amplitude. 
 */
double getAmplitude(const std::vector<double>& mags);

}}	// end lcmc::stats

#endif 	//LCMCMAGDISTH
