/** Test statistics based on examining the magnitude distribution
 * @file lightcurveMC/stats/magdist.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2011
 * @date Last modified May 22, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <vector>
#include "magdist.h"
#include "../nan.h"
#include "../except/undefined.h"

#if USELFP
#include <lfp/lfp.h>
#endif

namespace lcmc { namespace stats {

typedef std::vector<double> DoubleVec;

/** Calculates the modified C1 statistic. 
 * 
 * The statistic is identical to that presented in @cite RotorCtts, 
 * except that the minimum and maximum magnitude have been replaced with the 
 * 5th and 95th percentile, respectively. This change makes the C1 statistic 
 * much less sensitive to outliers.
 * 
 * @param[in] mags A vector of magnitudes from which to calculate C1.
 *
 * @return The C1 statistic.
 *
 * @pre @p mags contains at least three finite values
 * @pre @p mags contains at least two distinct finite values
 *
 * @pre @p mags may contain NaNs
 *
 * @perform O(N log N) time, where N = @p mags.size()
 *
 * @exception std::bad_alloc Thrown if not enough memory to calculate C1
 * @exception lcmc::stats::except::Undefined Thrown if C1 is undefined 
 *	because @p mags has no variability
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p mags does not 
 *	have enough values to calculate C1
 *
 * @exceptsafe Program state is unchanged in the event of an exception.
 */
double getC1(const DoubleVec& mags) {
	// Get rid of all NaN values
	// Need to make a copy anyway since mags is constant
	DoubleVec sMags(mags.size());
	DoubleVec::iterator newEnd = std::remove_copy_if(mags.begin(), mags.end(), 
			sMags.begin(), &utils::isNan);
	sMags.erase(newEnd, sMags.end());
	
	// Sort the survivors
	std::sort(sMags.begin(), sMags.end());
	size_t n = sMags.size();
	
	if (n < 3) {
		throw except::NotEnoughData("Need at least 3 values to compute C1.");
	}

	double medMin = 0.0, amplitude = 0.0;
	// Find the percentiles
	size_t lowRank = static_cast<size_t>(0.05 * n);
	size_t midRank = static_cast<size_t>(0.50 * n);
	size_t  hiRank = static_cast<size_t>(0.95 * n);
	// assert: 0 <= lowRank < midRank < hiRank < n

	if (sMags[hiRank] > sMags[lowRank]) {
		medMin    = sMags[midRank] - sMags[lowRank];
		amplitude = sMags[hiRank]  - sMags[lowRank];
	}
	// else no variability, so C1 is singular
	
	if (amplitude == 0.0) {
		throw except::Undefined("No variability, so C1 is singular");
	}
	
	return medMin / amplitude;
}

/** Calculates the light curve amplitude. 
 * 
 * The amplitude is defined as the range between the 5th and 95th 
 *	percentiles. This definition is relatively insensitive to outliers.
 * 
 * @param[in] mags A vector of magnitudes from which to calculate 
 *	the amplitude.
 *
 * @return The amplitude.
 *
 * @pre @p mags contains at least two finite values
 * @pre @p mags may contain NaNs
 *
 * @perform O(N log N) time, where N = @p mags.size()
 *
 * @exception std::bad_alloc Thrown if not enough memory to calculate amplitude
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p mags does not 
 *	have enough values to calculate an amplitude
 *
 * @exceptsafe Program state is unchanged in the event of an exception.
 */
double getAmplitude(const DoubleVec& mags) {
	// Get rid of all NaN values
	// Need to make a copy anyway since mags is constant
	DoubleVec sMags(mags.size());
	DoubleVec::iterator newEnd = std::remove_copy_if(mags.begin(), mags.end(), 
			sMags.begin(), &utils::isNan);
	sMags.erase(newEnd, sMags.end());
	
	// Sort the survivors
	std::sort(sMags.begin(), sMags.end());
	size_t n = sMags.size();

	if (n < 2) {
		throw except::NotEnoughData("Need at least 2 values to compute amplitude.");
	}

	// Find the percentiles
	size_t lowRank = static_cast<size_t>(0.05 * n);
	size_t  hiRank = static_cast<size_t>(0.95 * n);
	// assert: 0 <= lowRank < midRank < hiRank < n

	double amplitude = sMags[hiRank]  - sMags[lowRank];
	
	return amplitude;
}

}}	// end lcmc::stats
