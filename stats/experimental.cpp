/** Functions for trying out new features
 * @file experimental.cpp
 * @author Krzysztof Findeisen
 * @date Created August 3, 2011
 * @date Last modified May 12, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include "experimental.h"
#include "../except/data.h"
#include "../utils.tmp.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;

/** Calculates the RMS binned over ever-larger subintervals of the data. rmsVsTRooted() 
 *	considers only subintervals from the first data point to some later point
 * 
 * @param[in] times		Times at which data were taken
 * @param[in] fluxes		Flux measurements of a source
 * @param[out] timeSteps	A list of the lengths of all subintervals
 * @param[out] rmsValues	A list of the RMS over each subinterval
 *
 * @pre times.size() >= 2
 * @pre fluxes.size() == times.size()
 * @pre times is sorted in ascending order
 * @pre fluxes[i] is the flux of the source at times[i], for all i
 *
 * @pre times does not contain NaNs
 * @pre fluxes does not contain NaNs
 * @pre timeSteps does not contain NaNs
 *
 * @post timeSteps is sorted in ascending order
 * @post if N == times.size(), timeSteps.size() == rmsValues.size() == N-1
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to do the 
 *	calculations.
 * @exception std::invalid_argument Thrown if times is insufficiently long 
 *	or if fluxes has a different length from times
 * @exception lcmc::stats::except::NotSorted Thrown if times is unsorted.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 *
 * @perform O(times.size()^2) time
 *
 */
void rmsVsTRooted(const DoubleVec &times, const DoubleVec &fluxes, 
		DoubleVec &timeSteps, DoubleVec & rmsValues) {
	using std::swap;
	
	size_t nData = times.size();
	
	if(nData < 2) {
		throw std::invalid_argument("Can't take RMS over a light curve of less than 2 points (gave " 
		+ lexical_cast<std::string, size_t>(nData) + ")");
	}
	if(nData != fluxes.size()) {
		throw std::invalid_argument("Times and fluxes have different lengths in rmsVsTRooted() (gave " 
		+ lexical_cast<std::string, size_t>(nData) + " for times and " 
		+ lexical_cast<std::string, size_t>(fluxes.size()) + " for fluxes)");
	}
	if (!kpfutils::isSorted(times.begin(), times.end())) {
		throw except::NotSorted("times is not sorted in rmsVsTRooted()");
	}
	
	// copy-and-swap
	DoubleVec tempSteps, tempRms;
	tempSteps.reserve(nData-1);
	tempRms  .reserve(nData-1);
	
	// Do the RMS calculations
	for(size_t length = 2; length <= nData; length++) {
		double var = kpfutils::variance(fluxes.begin(), fluxes.begin()+length);
		tempSteps.push_back(times[length-1] - times[0]);
		tempRms.push_back(sqrt(var));
	}
	
	// IMPORTANT: no exceptions beyond this point

	swap(timeSteps, tempSteps);
	swap(rmsValues, tempRms);
}

/** Calculates the RMS binned over ever-larger subintervals of the data. rmsVsTAllPairs() 
 *	considers all subintervals of the data set
 * 
 * @param[in] times		Times at which data were taken
 * @param[in] fluxes		Flux measurements of a source
 * @param[out] timeSteps	A list of the lengths of all subintervals
 * @param[out] rmsValues	A list of the RMS over each subinterval
 *
 * @pre times contains at least two unique values
 * @pre times is sorted in ascending order
 * @pre fluxes is of the same length as times
 * @pre fluxes[i] is the flux of the source at times[i], for all i
 *
 * @pre times does not contain NaNs
 * @pre fluxes does not contain NaNs
 * @pre intervals does not contain NaNs
 *
 * @post intervals is sorted in ascending order
 * @post if N == length(times), length(intervals) == length(rmsValues) == @f$ \binom{N}{2} @f$
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to do the 
 *	calculations.
 * @exception std::invalid_argument Thrown if times is insufficiently long 
 *	or if fluxes has a different length from times
 * @exception lcmc::stats::except::NotSorted Thrown if times is unsorted.
 * 
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 *
 * @perform O(times.size()^3) time
 * 
 * @todo Reduce to O(times.size()^2) time through dynamic programming
 */
void rmsVsTAllPairs(const DoubleVec &times, const DoubleVec &fluxes, 
		DoubleVec &timeSteps, DoubleVec & rmsValues) {
	using std::swap;

	size_t nData = times.size();
	
	if(nData < 2) {
		throw std::invalid_argument("Can't take RMS over a light curve of less than 2 points (gave " 
		+ lexical_cast<std::string, size_t>(nData) + ")");
	}
	if(nData != fluxes.size()) {
		throw std::invalid_argument("Times and fluxes have different lengths in rmsVsTRooted() (gave " 
		+ lexical_cast<std::string, size_t>(nData) + " for times and " 
		+ lexical_cast<std::string, size_t>(fluxes.size()) + " for fluxes)");
	}
	if (!kpfutils::isSorted(times.begin(), times.end())) {
		throw except::NotSorted("times is not sorted in rmsVsTAllPairs()");
	}
	
	// Temporary storage that allows the data to be sorted together
	std::vector<std::pair<double, double> > sortableVec;
	
	// Do the RMS calculations
	for(size_t first = 0; first < nData; first++) {
		for(size_t last = first+1; last < nData; last++) {
			double var = kpfutils::variance(fluxes.begin()+first, 
				fluxes.begin()+last+1);
//			sortableVec.push_back(std::pair<double, double>(
//				times[last]-times[first], sqrt(var) ));
			sortableVec.push_back(std::make_pair(times[last]-times[first], 
					sqrt(var) ));
		}
	}
	
	// Sort timeSteps and rmsValues together
	// Now we just need to sort it
	// I don't have a parallel sort function yet
	// For now, do it the clumsy way, using pair<>
	std::sort(sortableVec.begin(), sortableVec.end());
	
	// copy-and-swap
	DoubleVec tempSteps, tempRms;
	tempSteps.reserve(sortableVec.size());
	tempRms  .reserve(sortableVec.size());
	
	for(std::vector<std::pair<double, double> >::const_iterator it = sortableVec.begin(); 
			it != sortableVec.end(); it++) {
		tempSteps.push_back(it->first);
		tempRms  .push_back(it->second);
	}

	// IMPORTANT: no exceptions beyond this point

	swap(timeSteps, tempSteps);
	swap(rmsValues, tempRms);
}

}}	// end lcmc::stats
