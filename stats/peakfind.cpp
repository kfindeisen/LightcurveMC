/** Port of Ann Marie's count.pro to a C++ function
 * @file lightcurveMC/stats/peakfind.cpp
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 14, 2013
 * @date Last modified May 15, 2013
 */

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/concept_check.hpp>
#include <boost/lexical_cast.hpp>
//#include "../nan.h"
//#include "../except/nan.h"
#include "peakfind.h"
#include "../except/undefined.h"

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
 * @param[out] peakHeights		The heights of the endpoints of significant intervals.	
 *
 * @pre times contains at least two unique values
 * @pre times is sorted in ascending order
 * @pre data.size() == times.size()
 * @pre data[i] is the measurement taken at times[i], for all i
 * @pre neither times nor data contains NaNs
 * @pre minAmp > 0
 *
 * @post the data previously in peakTimes and peakData are erased
 * @post peakTimes.size() == peakData.size() <= times.size()
 * @post (peakTimes[i], peakData[i]) are the coordinates of the ith extremum 
 *	of the light curve
 * 
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in times or data.
 * @exception except::NotEnoughData Thrown if times and data do not 
 *	have at least two values. 
 * @exception std::invalid_argument Thrown if times and data 
 *	do not have the same length or if minAmp is not positive.
 * @exception std::bad_alloc Thrown if there is not enough memory to find 
 *	the peaks
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 *
 * @perform O(N) time, where N = times.size()
 * 
 * @todo Prove loop behavior
 * @todo This function is highly redundant. Simplify!
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
	DoubleVec tempTimes  (1, times.front());
	DoubleVec tempHeights(1,  data.front());

	DoubleVec::const_iterator first = std::find_if(data.begin(), data.end(), 
		FartherThan(data.front(), minAmp) );
	
	if (first != data.end()) {
		// offset always positive
		DoubleVec::const_iterator::difference_type offset = (first - data.begin());
		
		tempTimes  .push_back(times[offset]);
		tempHeights.push_back( data[offset]);
		
		double y0 = tempHeights[0];
		double y1 = tempHeights[1];

		int sign = (y1-y0 > 0 ? 1 : (y1-y0 < 0 ? -1 : 0));
		
		for (size_t i = offset+1; i < data.size(); i++) {
			if (sign > 0) {
				// How far up does the trend go?
				if ((data[i] - y0) > (y1 - y0)) {
					tempHeights.back() =  data[i];
					tempTimes  .back() = times[i];
					y1 = data[i];
				}

				// Significant downturn?
				if ((y1 - data[i]) > minAmp) {
					tempHeights.push_back( data[i]);
					tempTimes  .push_back(times[i]);
					y0 = y1;
					y1 = data[i];
					sign = -1;
				}

			} else if (sign < 0) {
				// How far down does the trend go?
				if ((y0-data[i]) > (y0-y1)) {
					tempHeights.back() =  data[i];
					tempTimes  .back() = times[i];
					y1 = data[i];
				}

				// Significant upturn?
				if ((data[i]-y1) > minAmp) {
					tempHeights.push_back( data[i]);
					tempTimes  .push_back(times[i]);
					y0 = y1;
					y1 = data[i];
					sign = 1;
				}

			} else {
				/** @todo What is the correct behavior if sign == 0?
				 */
			}
		}	// end loop over data
	}	// end if minAmp < amplitude

	// IMPORTANT: no exceptions beyond this point
	
	swap(peakTimes  , tempTimes  );
	swap(peakHeights, tempHeights);
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
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in times or data.
 * @exception except::NotEnoughData Thrown if times and data do not 
 *	have at least two values. 
 * @exception std::invalid_argument Thrown if times and data 
 *	do not have the same length or if minAmp contains negative values.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the timescales
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 *
 * @perform O(CN) time, where C = magCuts.size() and N = times.size()
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
			tempTimes.push_back( (peakTimes.back() - peakTimes.front()) 
					/ (peakTimes.size()-1) );
		} else {
			tempTimes.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}
	
	// IMPORTANT: no exceptions beyond this point
	
	swap(timescales, tempTimes);
}

}}		// end lcmc::stats
