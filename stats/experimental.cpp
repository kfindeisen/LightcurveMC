/** Functions for trying out new features
 * @file experimental.cpp
 * by Krzysztof Findeisen
 * Created August 3, 2011
 * Last modified April 12, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cmath>
#include <timescales/utils.h>
#include "experimental.h"

#if USELFP
#include <lfp/lfp.h>
#endif

namespace lcmc { namespace stats {

/** Calculates the RMS binned over ever-larger subintervals of the data. rmsVsTRooted() 
 *	considers only subintervals from the first data point to some later point
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
 * @pre timeSteps does not contain NaNs
 *
 * @post intervals is sorted in ascending order
 * @post if N == length(times), length(intervals) == length(rmsValues) == N-1
 * @exception invalid_argument Thrown if times is insufficiently long or if 
 *	fluxes has a different length from times
 *
 * @perform O(times.size()^2) time
 *
 */
void rmsVsTRooted(const DoubleVec &times, const DoubleVec &fluxes, 
		DoubleVec &timeSteps, DoubleVec & rmsValues) {
	size_t nData = times.size();
	
	if(nData != fluxes.size()) {
		throw std::invalid_argument("times and fluxes have different lengths");
	}
	if(nData <= 1) {
		throw std::invalid_argument("Can't take RMS over a light curve of less than 2 points");
	}
	
	// Prepare the output vectors	
	timeSteps.clear();
	timeSteps.reserve(nData-1);
	rmsValues.clear();
	rmsValues.reserve(nData-1);
	
	// Do the RMS calculations
	// Since we're dealing with parallel vectors, numerical indices are 
	//	easier to use than iterators
	for(size_t length = 2; length <= nData; length++) {
		double var = kpftimes::variance(fluxes.begin(), fluxes.begin()+length);
		timeSteps.push_back(times[length-1]-times[0]);
		rmsValues.push_back(sqrt(var));
	}
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
 * @post if N == length(times), length(intervals) == length(rmsValues) == (N C 2)
 * @exception invalid_argument Thrown if the preconditions on times or 
 *	length(fluxes) are violated.
 *
 * @perform O(times.size()^3) time
 * @todo Reduce to O(times.size()^2) time through dynamic programming
 *
 */
void rmsVsTAllPairs(const DoubleVec &times, const DoubleVec &fluxes, 
		DoubleVec &timeSteps, DoubleVec & rmsValues) {
	size_t nData = times.size();
	
	if(nData != fluxes.size()) {
		throw std::invalid_argument("times and fluxes have different lengths");
	}
	if(nData <= 1) {
		throw std::invalid_argument("Can't take RMS over a light curve of less than 2 points");
	}
	
	// Prepare the output vectors	
	timeSteps.clear();
	rmsValues.clear();
	std::vector<std::pair<double, double> > sortableVec;
	
	// Do the RMS calculations
	// Since we're dealing with parallel vectors, numerical indices are 
	//	easier to use than iterators
	for(size_t first = 0; first < nData; first++) {
		for(size_t last = first+1; last < nData; last++) {
			double var = kpftimes::variance(fluxes.begin()+first, 
				fluxes.begin()+last+1);
//			timeSteps.push_back(times[last]-times[first]);
//			rmsValues.push_back(sqrt(var));
			sortableVec.push_back(std::pair<double, double>(
				times[last]-times[first], sqrt(var) ));
		}
	}
	
	// Sort timeSteps and rmsValues together
	// Now we just need to sort it
	// I don't have a parallel sort function yet
	// For now, do it the clumsy way, using pair<>
	std::sort(sortableVec.begin(), sortableVec.end());
	
	for(std::vector<std::pair<double, double> >::const_iterator it = sortableVec.begin(); 
			it != sortableVec.end(); it++) {
		timeSteps.push_back(it->first);
		rmsValues.push_back(it->second);
	}
}

}}	// end lcmc::stats
