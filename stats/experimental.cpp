/** Functions for trying out new features
 * @file lightcurveMC/stats/experimental.cpp
 * @author Krzysztof Findeisen
 * @date Created August 3, 2011
 * @date Last modified May 22, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
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
#include "../../common/stats.tmp.h"

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
 * @pre @p times.size() &ge; 2
 * @pre @p fluxes.size() = @p times.size()
 * @pre @p times is sorted in ascending order
 * @pre @p fluxes[i] is the flux of the source at @p times[i], for all i
 *
 * @pre @p times does not contain NaNs
 * @pre @p fluxes does not contain NaNs
 * @pre @p timeSteps does not contain NaNs
 *
 * @post @p timeSteps is sorted in ascending order
 * @post if N = @p times.size(), @p timeSteps.size() = @p rmsValues.size() = N-1
 *
 * @perform O(N<sup>2</sup>) time, where N = @p times.size()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to do the 
 *	calculations.
 * @exception std::invalid_argument Thrown if @p times is insufficiently long 
 *	or if @p fluxes has a different length from @p times
 * @exception kpfutils::except::NotSorted Thrown if @p times is unsorted.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
void rmsVsTRooted(const DoubleVec &times, const DoubleVec &fluxes, 
		DoubleVec &timeSteps, DoubleVec & rmsValues) {
	using std::swap;
	
	size_t nData = times.size();
	
	if(nData < 2) {
		throw std::invalid_argument("Can't take RMS over a light curve of less than 2 points (gave " 
		+ lexical_cast<std::string>(nData) + ")");
	}
	if(nData != fluxes.size()) {
		throw std::invalid_argument("Times and fluxes have different lengths in rmsVsTRooted() (gave " 
		+ lexical_cast<std::string>(nData) + " for times and " 
		+ lexical_cast<std::string>(fluxes.size()) + " for fluxes)");
	}
	if (!kpfutils::isSorted(times.begin(), times.end())) {
		throw kpfutils::except::NotSorted("times is not sorted in rmsVsTRooted()");
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
 * @pre @p times contains at least two unique values
 * @pre @p times is sorted in ascending order
 * @pre @p fluxes is of the same length as times
 * @pre @p fluxes[i] is the flux of the source at @p times[i], for all i
 *
 * @pre @p times does not contain NaNs
 * @pre @p fluxes does not contain NaNs
 * @pre @p timeSteps does not contain NaNs
 *
 * @post @p timeSteps is sorted in ascending order
 * @post if N = @p times.size(), @p timeSteps.size() = @p rmsValues.size() = @f$ \binom{N}{2} @f$
 *
 * @perform O(N<sup>3</sup>) time, where N = times.size()
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to do the 
 *	calculations.
 * @exception std::invalid_argument Thrown if @p times is insufficiently long 
 *	or if @p fluxes has a different length from @p times
 * @exception kpfutils::except::NotSorted Thrown if @p times is unsorted.
 * 
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 * 
 * @todo Reduce to O(N<sup>2</sup>) time through dynamic programming
 */
void rmsVsTAllPairs(const DoubleVec &times, const DoubleVec &fluxes, 
		DoubleVec &timeSteps, DoubleVec & rmsValues) {
	using std::swap;

	size_t nData = times.size();
	
	if(nData < 2) {
		throw std::invalid_argument("Can't take RMS over a light curve of less than 2 points (gave " 
		+ lexical_cast<std::string>(nData) + ")");
	}
	if(nData != fluxes.size()) {
		throw std::invalid_argument("Times and fluxes have different lengths in rmsVsTRooted() (gave " 
		+ lexical_cast<std::string>(nData) + " for times and " 
		+ lexical_cast<std::string>(fluxes.size()) + " for fluxes)");
	}
	if (!kpfutils::isSorted(times.begin(), times.end())) {
		throw kpfutils::except::NotSorted("times is not sorted in rmsVsTAllPairs()");
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
