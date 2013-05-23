/** Port of Ann Marie's count.pro to a C++ function
 * @file lightcurveMC/stats/peakfind.cpp
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 14, 2013
 * @date Last modified May 22, 2013
 */

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <boost/concept_check.hpp>
#include <boost/lexical_cast.hpp>
//#include "../nan.h"
//#include "../except/nan.h"
#include "peakfind.h"
#include "../except/undefined.h"
#include "../utils.tmp.h"

namespace lcmc { namespace stats { 

using std::string;
using std::vector;
using boost::lexical_cast;

/** Unary predicate for whether a value is more a certain distance from 
 *	some threshold
 */
class FartherThan {
public: 
	/** Defines the distance comparison to use
	 *
	 * @param[in] base The value from which a distance will be 
	 *	measured.
	 * @param[in] threshold The distance to exceed.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	FartherThan(double base, double threshold) : base(base), limit(threshold) {
	}
	
	/** Unary comparison of x
	 *
	 * @param[in] x The value to compare to base
	 *
	 * @return True if abs(x-base) >= threshold, false otherwise.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	bool operator() (double x) {
		return (fabs(x-base) >= limit);
	}

private: 
	double base, limit;
};
BOOST_CONCEPT_ASSERT((boost::UnaryPredicate<FartherThan, double>));


/** Calculates the number of monotonic intervals in a light curve having a 
 * magnitude change greater than a threshold
 * 
 * @param[in] times	Times at which data were taken
 * @param[in] data	The data (typically fluxes or magnitudes) measured 
 *			at each time
 * @param[in] minAmp	The smallest change in the value of data to count
 * @param[out] peakTimes	The times of the endpoints of significant intervals.
 * @param[out] peakHeights	The heights of the endpoints of significant intervals.	
 *
 * @pre times contains at least two unique values
 * @pre times is sorted in ascending order
 * @pre data.size() == times.size()
 * @pre data[i] is the measurement taken at times[i], for all i
 * @pre neither times nor data contains NaNs
 * @pre minAmp > 0
 *
 * @post the data previously in peakTimes and peakData are erased
 * @post 1 <= peakTimes.size() == peakData.size() <= times.size()
 * @post for all i in peakTimes except the first and last elements, 
 *	(peakTimes[i], peakData[i]) are the coordinates of a local minimum 
 *	or maximum in (times, data)
 * @post for all i in peakTimes except the first two and last single elements, 
 *	if peakTimes[i] is a local minimum in (times, data), then 
 *	peakTimes[k-1] is a local maximum, and if peakTimes[i] is a local 
 *	maximum, then peakTimes[i] is a local minimum
 * @post for all i in peakTimes except the first element, peakHeights[i] 
 *	differs from peakHeights[i-1] by at least minAmp
 * @post for all points in (times, data) that are not in (peakTimes, peakHeights), 
 *	inserting that point anywhere in (peakTimes, peakHeights) would 
 *	violate one of the above conditions
 *
 * @perform O(N) time, where N = times.size()
 * 
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in times or data.
 * @exception lcmc::stats::except::NotEnoughData Thrown if times and data do not 
 *	have at least two values. 
 * @exception std::invalid_argument Thrown if times and data 
 *	do not have the same length or if minAmp is not positive.
 * @exception std::bad_alloc Thrown if there is not enough memory to find 
 *	the peaks
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 */
void peakFind(const DoubleVec& times, const DoubleVec& data, 
		double minAmp, DoubleVec& peakTimes, DoubleVec& peakHeights) {
	using std::swap;

	const size_t N = times.size();
	if (N < 2) {
		throw except::NotEnoughData("Cannot find peaks with fewer than 2 data points in peakFind() (gave " 
			+ lexical_cast<string>(N) + ").");
	}
	if (N != data.size()) {
		throw std::invalid_argument("Data and time arrays passed to peakFind() must have the same length (gave " 
			+ lexical_cast<string>(N) + " for times and " 
			+ lexical_cast<string>(data.size()) + " for data)");
	}
	if (minAmp <= 0) {
		throw std::invalid_argument("Need a positive threshold for magnitude changes in peakFind() (gave " 
			+ lexical_cast<string>(minAmp) + ")");
	}

	// copy-and-swap
	DoubleVec tempTimes(1, times.front());
	DoubleVec tempPeaks(1,  data.front());

	DoubleVec::const_iterator first = std::find_if(data.begin(), data.end(), 
		FartherThan(data.front(), minAmp) );

	if (first != data.end()) {
		// offset always non-negative
		DoubleVec::const_iterator::difference_type offset = (first - data.begin());
		
		tempTimes  .push_back(times[offset]);
		tempPeaks.push_back( data[offset]);
		
		// assert: tempPeaks[0] != tempPeaks[1] by construction of 
		// first, given minAmp > 0
		int sign = (tempPeaks[1] - tempPeaks[0] > 0 ? 1 : -1);
		
		/** @todo Find a more compact way of expressing the loop invariant
		 */
		// invariant: sign != 0
		// invariant: tempTimes.size() >= 2
		// invariant: tempTimes.size() == tempPeaks.size()
		// invariant: for all 1 <= k < tempTimes.size()-1, tempTimes[k] is a local 
		//	minimum or local maximum in (times, data).
		// invariant: for all 2 <= k < tempTimes.size()-1, if tempTimes[k] is a local 
		//	minimum in (times, data), then tempTimes[k-1] is a local maximum, and 
		//	if tempTimes[k] is a local maximum, then tempTimes[k-1] is a local 
		//	minimum
		// invariant: for all 1 <= k < tempTimes.size(), tempPeaks[k] differs from 
		//	tempPeaks[k-1] by at least minAmp
		// invariant: for all points in (times, data)[0 .. i-1] that are not in 
		//	(tempTimes, tempPeaks), inserting that point anywhere in 
		//	(tempTimes, tempPeaks) would violate one of the above conditions
		// invariant: if tempTimes[back-1] is a local minimum in (times, data), then 
		//	sign > 0, and if tempTimes[back-1] is a local maximum, then sign < 0. 
		//	If tempTimes[back-1] is neither a minimum nor a maximum, then 
		//	if tempTimes[1] > tempTimes[0], then sign > 0, and 
		//	if tempTimes[1] < tempTimes[0], then sign < 0
		// invariant: if tempTimes[back-1] is a local minimum in (times, data), then 
		//	tempPeaks[back] = max data[tempTimes[back-1] .. times[i-1]], and if 
		//	tempTimes[back-1] is a local maximum, then tempPeaks[back] = 
		//	min data[tempTimes[back-1] .. times[i-1]]
		for (size_t i = offset+1; i < data.size(); i++) {
			if ( (sign > 0 && data[i] > tempPeaks.back()) || 
					(sign < 0 && data[i] < tempPeaks.back()) ) {
				tempPeaks.back() =  data[i];
				tempTimes.back() = times[i];
			} else if ( (sign > 0 && (tempPeaks.back()-data[i]) > minAmp) || 
					(sign < 0 && (data[i]-tempPeaks.back()) > minAmp) ) {
				tempPeaks.push_back( data[i]);
				tempTimes.push_back(times[i]);
				sign = -sign;
			}
		}	// end loop over data
	}	// end if minAmp < amplitude

	// IMPORTANT: no exceptions beyond this point
	
	swap(peakTimes  , tempTimes);
	swap(peakHeights, tempPeaks);
}

/** Calculates the waiting time for variability of a given amplitude as 
 *	a function of amplitude
 * 
 * @param[in] times	Times at which data were taken
 * @param[in] data	The data (typically fluxes or magnitudes) measured 
 *			at each time
 * @param[in] magCuts	The variability amplitudes at which to characterize the timescale
 * @param[out] timescales	The waiting times
 *
 * @pre times contains at least two unique values
 * @pre times is sorted in ascending order
 * @pre data.size() == times.size()
 * @pre data[i] is the measurement taken at times[i], for all i
 * @pre neither times nor data contains NaNs
 * @pre magCuts[i] > 0, for all i
 *
 * @post the data previously in timescales is erased
 * @post timescales.size() == magCuts.size()
 * @post timescales[i] is the waiting time for variability greater 
 *	than magCuts[i], for all i
 *
 * @perform O(CN log N) time, where C = magCuts.size() and N = times.size()
 * 
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in times or data.
 * @exception lcmc::stats::except::NotEnoughData Thrown if times and data 
 *	do not have at least two values. 
 * @exception std::invalid_argument Thrown if times and data 
 *	do not have the same length or if minAmp contains negative values.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the timescales
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 */
void peakFindTimescales(const DoubleVec& times, const DoubleVec& data, 
		const DoubleVec& magCuts, DoubleVec& timescales) {
	using std::swap;

	const size_t N = times.size();
	if (N < 2) {
		throw except::NotEnoughData("Cannot find timescales with fewer than 2 data points in peakFindTimescales() (gave " 
			+ lexical_cast<string>(N) + ").");
	}
	if (N != data.size()) {
		throw std::invalid_argument("Data and time arrays passed to peakFindTimescales() must have the same length (gave " 
			+ lexical_cast<string>(N) + " for times and " 
			+ lexical_cast<string>(data.size()) + " for data)");
	}

	// copy-and-swap
	DoubleVec tempTimes;

	for (DoubleVec::const_iterator mag = magCuts.begin(); mag != magCuts.end(); mag++) {
		if (*mag <= 0) {
			throw std::invalid_argument("Need a positive threshold for magnitude changes in peakFindTimescales() (gave magCuts[" 
				+ lexical_cast<string>(mag-magCuts.begin()) + "] = " 
				+ lexical_cast<string>(*mag) + ")");
		}
		
		DoubleVec peakTimes, peakHeights;
		peakFind(times, data, *mag, peakTimes, peakHeights);
		
		if (peakTimes.size() > 1) {
//			tempTimes.push_back( (times.back() - times.front()) 
//					/ (peakTimes.size()-1) );
			/** @todo Reimplement using an iterator adapter
			 */
			DoubleVec intervals;
			for(DoubleVec::const_iterator it = peakTimes.begin()+1; 
					it != peakTimes.end(); it++) {
				intervals.push_back(*it - *(it-1));
			}
			tempTimes.push_back(kpfutils::quantile(intervals.begin(), 
					intervals.end(), 0.5));
		} else {
			tempTimes.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}
	
	// IMPORTANT: no exceptions beyond this point
	
	swap(timescales, tempTimes);
}

}}		// end lcmc::stats
