/** Port of Ann Marie's count.pro to a C++ function
 * @file lightcurveMC/stats/peakfind.h
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 15, 2013
 * @date Last modified May 15, 2013
 */

#include <vector>

namespace lcmc { namespace stats { 

typedef std::vector<double> DoubleVec;

/** Calculates the number of monotonic intervals in a light curve having a 
 * magnitude change greater than a threshold
 */
void peakFind(const DoubleVec& times, const DoubleVec& data, 
		double minAmp, DoubleVec& peakTimes, DoubleVec& peakHeights);

/** Calculates the waiting time for variability of a given amplitude as 
 *	a function of amplitude
 */
void peakFindTimescales(const DoubleVec& times, const DoubleVec& data, 
		const DoubleVec& magCuts, DoubleVec& timescales);

}}		// end lcmc::stats
