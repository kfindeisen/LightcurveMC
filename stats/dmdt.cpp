/** Functions for analyzing Delta-m Delta-t plots
 * @file lightcurveMC/stats/dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified May 22, 2013
 */

#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include "dmdt.h"
#include "../except/data.h"
#include "../utils.tmp.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using kpfutils::quantile;

/** Computes the fraction of pairs of magnitudes above some threshold found 
 *	in each &Delta;t bin of a &Delta;m&Delta;t plot.
 *
 * @param[in] deltaT The &Delta;t values of the &Delta;m&Delta;t plot.
 * @param[in] deltaM The corresponding &Delta;m values of the &Delta;m&Delta;t plot.
 * @param[in] binEdges A vector containing the (N+1) boundaries of the N &Delta;t bins 
 *	in which to count high-&Delta;m pairs.
 * @param[out] fracs A vector containing the fraction of &Delta;m values in 
 *	each bin that exceed threshold.
 * @param[in] threshold The characteristic magnitude difference above which 
 *	&Delta;m values are to be counted.
 *
 * @pre @p deltaT is sorted in ascending order
 * @pre @p binEdges is sorted in ascending order
 *
 * @pre @p deltaT does not contain any NaNs
 * @pre @p deltaM does not contain any NaNs
 * @pre @p binEdges does not contain any NaNs
 * 
 * @post @p fracs.size() = @p binEdges.size() - 1
 * @post For all i &isin; [0, @p binEdges.size()-1], @p fracs[i] contains the 
 *	fraction of @p deltaM > @p threshold, given deltaT &isin; 
 *	[@p binEdges[i], @p binEdges[i+1]).
 *
 * @perform O(M + N) time, where N = @p deltaM.size() and M = @p binEdges.size()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the bin fractions.
 * @exception lcmc::stats::except::NotSorted Thrown if either deltaT or binEdges 
 *	is unsorted.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
void hiAmpBinFrac(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &fracs, double threshold) {
	using std::swap;

	if (!kpfutils::isSorted(deltaT.begin(), deltaT.end())) {
		throw except::NotSorted("deltaT is not sorted in hiAmpBinFrac()");
	}
	if (!kpfutils::isSorted(binEdges.begin(), binEdges.end())) {
		throw except::NotSorted("binEdges is not sorted in hiAmpBinFrac()");
	}
	
	DoubleVec tempFracs;
	tempFracs.reserve(binEdges.size()-1);

	DoubleVec::const_iterator curDelta = std::lower_bound(deltaT.begin(), deltaT.end(), 
			binEdges.front());
	// assert: if any deltaT values fall within the bins specified by binEdges, 
	//	then curDelta is the lowest such value
	for(DoubleVec::const_iterator curBin = binEdges.begin(); curBin+1 != binEdges.end(); 
			curBin++) {
		// loop invariant: the pair pointed to by curDelta has not 
		//	been counted for this or any previous bin

		// Restart the count for each bin
		long numPairs = 0, numHighPairs = 0;
		while (curDelta+1 != deltaT.end() && 
				*curDelta >= *curBin && *curDelta < *(curBin+1)) {
			numPairs++;
			
			// what is the magnitude difference for the pair pointed to by curDelta?
			DoubleVec::const_iterator curDeltaM = deltaM.begin() 
					+ (curDelta - deltaT.begin());
			if (*curDeltaM > threshold) {
				numHighPairs++;
			}
			
			curDelta++;
		} // else no new pairs to add
		// assert: *curDelta > *(curBin+1) xor all pairs in bin already added
		
		tempFracs.push_back(numPairs > 0 ? 
				static_cast<double>(numHighPairs)/
				static_cast<double>(numPairs) : 
				std::numeric_limits<double>::signaling_NaN());
	}
	
	// IMPORTANT: no exceptions beyond this point
	
	swap(fracs, tempFracs);
}

/** Computes the quantile of pairs of magnitudes found in each &Delta;t bin 
 *	of a &Delta;m&Delta;t plot.
 *
 * @param[in] deltaT The &Delta;t values of the &Delta;m&Delta;t plot.
 * @param[in] deltaM The corresponding &Delta;m values of the &Delta;m&Delta;t plot.
 * @param[in] binEdges A vector containing the (N+1) boundaries of the N &Delta;t bins 
 *	in which to calculate quantiles.
 * @param[out] quants A vector containing the quantiles within each bin.
 * @param[in] q The quantile to calculate.
 *
 * @pre @p deltaT is sorted in ascending order
 * @pre @p binEdges is sorted in ascending order
 * @pre 0 < @p q < 1
 *
 * @pre @p deltaT does not contain any NaNs
 * @pre @p deltaM does not contain any NaNs
 * @pre @p binEdges does not contain any NaNs
 * 
 * @post @p quants.size() = @p binEdges.size() - 1
 * @post For all i &isin; [0, @p binEdges.size()-1], @p quants[i] contains the 
 *	<tt>q</tt>th quantile of @p deltaM, given @p deltaT &isin; 
 *	[@p binEdges[i], @p binEdges[i+1])
 *
 * @perform O(N log N) time, where N = @p deltaT.size()
 * @perfmore o(N log N - N log M) time, where N = @p deltaT.size() and M = @p binEdges.size()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the bin fractions.
 * @exception std::invalid_argument Thrown if @p q is not in (0, 1)
 * @exception lcmc::stats::except::NotSorted Thrown if either @p deltaT or @p binEdges 
 *	is unsorted.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
void deltaMBinQuantile(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &quants, double q) {
	using std::swap;
	
	if (q <= 0 || q >= 1) {
		throw std::invalid_argument("Quantile must be in (0, 1) (gave " 
			+ lexical_cast<std::string>(q) + ")");
	}
	if (!kpfutils::isSorted(deltaT.begin(), deltaT.end())) {
		throw except::NotSorted("deltaT is not sorted in deltaMBinQuantile()");
	}
	if (!kpfutils::isSorted(binEdges.begin(), binEdges.end())) {
		throw except::NotSorted("binEdges is not sorted in deltaMBinQuantile()");
	}
	
	// copy-and-swap
	DoubleVec tempQuants;
	tempQuants.reserve(binEdges.size()-1);
	
	for(DoubleVec::const_iterator curBin = binEdges.begin(); 
			curBin+1 != binEdges.end(); curBin++) {
		// Want deltam values for all pairs in the current bin
		// Start by finding the deltaT range in the interval [curBin, curBin+1)
		// Note: *_bound works only if deltaT is sorted
		DoubleVec::const_iterator deltaTStart = std::lower_bound(deltaT.begin(), 
				deltaT.end(), *curBin);
		DoubleVec::const_iterator deltaTEnd = std::lower_bound(deltaT.begin(), 
				deltaT.end(), *(curBin+1));
		DoubleVec::const_iterator deltaMStart = deltaM.begin() 
				+ (deltaTStart - deltaT.begin());
		DoubleVec::const_iterator deltaMEnd = deltaM.begin() 
				+ (deltaTEnd - deltaT.begin());
		
		if (deltaMStart != deltaMEnd) {
			double result = quantile(static_cast<DoubleVec::const_iterator>(deltaMStart), 
					deltaMEnd, q);
			tempQuants.push_back(result);
		} else {
			// The bin is empty
			tempQuants.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}

	// IMPORTANT: no exceptions beyond this point
	
	swap(quants, tempQuants);
}

}}	// end lcmc::stats
