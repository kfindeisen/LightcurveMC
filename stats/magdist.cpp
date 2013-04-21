/** Test statistics based on examining the magnitude distribution
 * @file magdist.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2011
 * @date Last modified April 12, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <vector>
#include "magdist.h"
#include "../nan.h"

#if USELFP
#include <lfp/lfp.h>
#endif

namespace lcmcstats {

typedef std::vector<double> DoubleVec;

/** Calculates the modified C1 statistic. 
 * 
 * The statistic is identical to that presented in Grankin (2007), except that 
 * the minimum and maximum magnitude have been replaced with the 5th and 95th 
 * percentile, respectively. This change makes the C1 statistic much less 
 * sensitive to outliers.
 * 
 * @param[in] mags A vector of magnitudes from which to calculate C1.
 *
 * @return The C1 statistic.
 *
 * @pre mags contains at least three finite values
 * @pre mags contains at least two distinct finite values
 *
 * @pre mags may contain NaNs
 *
 * @todo Decide on correct treatment of infinite magnitudes.
 *
 * @exception std::runtime_error Thrown if mags does not have enough distinct 
 * values to calculate C1
 */
double getC1(const DoubleVec& mags) {
	// Get rid of all NaN values
	DoubleVec sMags(mags.size());
	DoubleVec::iterator newEnd = std::remove_copy_if(mags.begin(), mags.end(), 
			sMags.begin(), &lcmcutils::isNan);
	sMags.erase(newEnd, sMags.end());
	
	// Sort the survivors
	std::sort(sMags.begin(), sMags.end());
	size_t n = sMags.size();

	double medMin = 0.0, amplitude = 0.0;
	if (n >= 3) {
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
	}
	
	if (amplitude == 0.0) {
		throw std::runtime_error("C1 is singular");
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
 * @pre mags contains at least two finite values
 *
 * @pre mags may contain NaNs
 *
 * @todo Decide on correct treatment of infinite magnitudes.
 *
 * @exception std::runtime_error Thrown if mags does not have enough distinct 
 * values to calculate an amplitude
 */
double getAmplitude(const DoubleVec& mags) {
	// Get rid of all NaN values
	DoubleVec sMags(mags.size());
	DoubleVec::iterator newEnd = std::remove_copy_if(mags.begin(), mags.end(), 
			sMags.begin(), &lcmcutils::isNan);
	sMags.erase(newEnd, sMags.end());
	
	// Sort the survivors
	std::sort(sMags.begin(), sMags.end());
	size_t n = sMags.size();

	double amplitude = 0.0;
	if (n >= 2) {
		// Find the percentiles
		size_t lowRank = static_cast<size_t>(0.05 * n);
		size_t  hiRank = static_cast<size_t>(0.95 * n);
		// assert: 0 <= lowRank < midRank < hiRank < n

		amplitude = sMags[hiRank]  - sMags[lowRank];
	}
	
	if (amplitude <= 0.0) {
		throw std::runtime_error("Zero amplitude");
	}
	
	return amplitude;
}

};	// end lcmcstats
