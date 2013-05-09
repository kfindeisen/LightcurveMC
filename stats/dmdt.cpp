/** Functions for analyzing Delta-m Delta-t plots
 * @file dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified May 9, 2013
 */

#include <algorithm>
#include <limits>
#include <vector>
#include <cstdio>
#include <timescales/utils.h>
#include "dmdt.h"

namespace lcmc { namespace stats {

/** Computes the quantile of the data in an arbitrary range.
 *
 * @param[in] first An iterator pointing to the first element of the range.
 * @param[in] last An iterator pointing to just after the last element of the range.
 * @param[in] quantile The choice of quantile to calculate.
 *
 * @return The value of the quantile.
 * 
 * @pre 0 < quantile < 1
 * @pre No value in [first, last) is NaN
 *
 * @post The return value is not NaN
 *
 * @perform O(D log D), where D = std::distance(first, last).
 *
 * @todo Merge with other quantile implementations
 */
template <typename InputIterator> 				// Iterator to use
		typename std::iterator_traits<InputIterator>::value_type 	// Type pointed to by iterator 
		quantile(InputIterator first, InputIterator last, double quantile) {
	typedef typename std::iterator_traits<InputIterator>::value_type Value;
	
	// We don't want to alter the data, so we copy while sorting
	// Copy to a vector because sorting requires random access
	size_t vecSize = distance(first, last);
	if (vecSize < 1) {
		throw std::invalid_argument("Supplied empty data set to quantile()");
	}
	std::vector<Value> sortedVec(vecSize);
	std::partial_sort_copy(first, last, sortedVec.begin(), sortedVec.end());

	size_t index = 0;
	if (quantile >= 0.0 && quantile < 1.0) {
		index = static_cast<size_t>(quantile*vecSize);
	} else if (quantile == 1.0) {
		index = vecSize-1;
	} else {
		char buf[60];
		sprintf(buf, "Invalid quantile of %0.2f passed to quantile()", quantile);
		throw std::domain_error(buf);
	}

	return sortedVec[index];
}

/** Computes the fraction of pairs of magnitudes above some threshold found 
 *	in each &Delta;t bin of a &Delta;m&Delta;t plot.
 *
 * @param[in] deltaT The time differences of the &Delta;m&Delta;t plot.
 * @param[in] deltaM The magnitude differences of the &Delta;m&Delta;t plot.
 * @param[in] binEdges A vector containing the (N+1) boundaries of the N bins 
 *	in which to count high-&Delta;m pairs.
 * @param[out] fracs A vector containing the fraction of &Delta;m values in 
 *	each bin that exceed threshold.
 * @param[in] threshold The characteristic magnitude difference above which 
 *	&Delta;m values are to be counted.
 *
 * @pre deltaT is sorted in ascending order
 * @pre binEdges is sorted in ascending order
 *
 * @pre deltaT does not contain any NaNs
 * @pre deltaM does not contain any NaNs
 * @pre binEdges does not contain any NaNs
 * 
 * @post fracs.size() = binEdges.size() - 1
 * @post For all i &isin; [0, binEdges.size()-1], fracs[i] contains the 
 *	fraction of deltaM > threshold where deltaT &isin; 
 *	[binEdges[i], binEdges[i+1]).
 */
void hiAmpBinFrac(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &fracs, double threshold) {
	
	// deltaT should be sorted in increasing order... that makes 
	//	counting easier
	fracs.clear();
	fracs.reserve(binEdges.size()-1);
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
		
		fracs.push_back(numPairs > 0 ? 
				static_cast<double>(numHighPairs)/
				static_cast<double>(numPairs) : 
				std::numeric_limits<double>::signaling_NaN());
	}
}

/** Computes the quantile of pairs of magnitudes found in each &Delta;t bin 
 *	of a &Delta;m&Delta;t plot.
 *
 * @param[in] deltaT The time differences of the &Delta;m&Delta;t plot.
 * @param[in] deltaM The magnitude differences of the &Delta;m&Delta;t plot.
 * @param[in] binEdges A vector containing the (N+1) boundaries of the N bins 
 *	in which to calculate quantiles.
 * @param[out] quants A vector containing the quantiles within each bin.
 * @param[in] q The quantile to calculate.
 *
 * @pre deltaT is sorted in ascending order
 * @pre binEdges is sorted in ascending order
 * @pre 0 < quantile < 1
 *
 * @pre deltaT does not contain any NaNs
 * @pre deltaM does not contain any NaNs
 * @pre binEdges does not contain any NaNs
 * 
 * @post quants.size() = binEdges.size() - 1
 * @post For all i &isin; [0, binEdges.size()-1], quants[i] contains the 
 *	qth quantile of deltaM where deltaT &isin; 
 *	[binEdges[i], binEdges[i+1])
 *
 * @perform O(N log (N/M)), where N = deltaT.size() and M = binEdges.size()
 *
 * @todo Prove performance
 */
void deltaMBinQuantile(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &quants, double q) {
	// deltaT should be sorted in increasing order... that makes 
	//	counting easier
	quants.clear();
	quants.reserve(binEdges.size()-1);
	for(DoubleVec::const_iterator curBin = binEdges.begin(); curBin+1 != binEdges.end(); 
			curBin++) {
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
			quants.push_back(result);
		} else {
			// The bin is empty
			quants.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}
}

}}	// end lcmc::stats
