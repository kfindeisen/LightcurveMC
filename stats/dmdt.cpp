/** Functions for analyzing Delta-m Delta-t plots
 * @file lightcurveMC/stats/dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified August 5, 2013
 *
 * @todo Break up this file.
 */

#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include <timescales/timescales.h>
#include "cut.tmp.h"
#include "dmdt.h"
#include "../except/data.h"
#include "magdist.h"
#include "statfamilies.h"
#include "../except/undefined.h"
#include "../../common/stat_except.h"
#include "../../common/stats.tmp.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using kpfutils::quantile;
using std::vector;

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
 * @exception kpfutils::except::NotSorted Thrown if either deltaT or binEdges 
 *	is unsorted.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
void hiAmpBinFrac(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &fracs, double threshold) {
	using std::swap;

	if (!kpfutils::isSorted(deltaT.begin(), deltaT.end())) {
		throw kpfutils::except::NotSorted("deltaT is not sorted in hiAmpBinFrac()");
	}
	if (!kpfutils::isSorted(binEdges.begin(), binEdges.end())) {
		throw kpfutils::except::NotSorted("binEdges is not sorted in hiAmpBinFrac()");
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
 * @exception kpfutils::except::NotSorted Thrown if either @p deltaT or @p binEdges 
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
		throw kpfutils::except::NotSorted("deltaT is not sorted in deltaMBinQuantile()");
	}
	if (!kpfutils::isSorted(binEdges.begin(), binEdges.end())) {
		throw kpfutils::except::NotSorted("binEdges is not sorted in deltaMBinQuantile()");
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

/** Does all &Delta;m&Delta;t-related computations for a given light curve.
 *
 * @param[in] times The times at which the light curve was sampled.
 * @param[in] mags The values of the light curve
 * @param[in] getCut Flag indicating that cuts through the the 
 *	&Delta;m&Delta;t quantiles should be extracted
 * @param[in] getPlot Flag indicating that the &Delta;m&Delta;t quantiles 
 *	should be stored
 * @param[out] cut50Amp3 The NamedCollection in which to record the 
 *	&Delta;t value at which the 50th percentile of &Delta;m crosses 1/3 
 *	the amplitude.
 * @param[out] cut50Amp2 The NamedCollection in which to record the 
 *	&Delta;t value at which the 50th percentile of &Delta;m crosses 1/2 
 *	the amplitude.
 * @param[out] cut90Amp3 The NamedCollection in which to record the 
 *	&Delta;t value at which the 90th percentile of &Delta;m crosses 1/3 
 *	the amplitude.
 * @param[out] cut90Amp2 The NamedCollection in which to record the 
 *	&Delta;t value at which the 90th percentile of &Delta;m crosses 1/2 
 *	the amplitude.
 * @param[out] dmdtMed The NamedCollection in which to record the 
 *	median value of &Delta;m as a function of &Delta;t.
 *
 * @pre @p times.size() = @p mags.size()
 * @pre Neither @p times nor @p mags may contain NaNs
 * 
 * @post if @p getCut, then new elements are appended to @p cut50Amp3, 
 *	@p cut50Amp2, @p cut90Amp3, and @p cut90Amp2. If any of the cuts are 
 *	undefined, NaN is appended to the corresponding collection.
 * @post if @p getPlot, then a new element is appended to @p dmdtMed.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store more statistics.
 * @exception std::invalid_argument Thrown if @p times and @p mags do not 
 *	have matching lengths or if @p times or @p data contains NaNs.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times 
 *	and @p mags are too short to calculate the desired statistics.
 *
 * @exceptsafe The program is in a consistent state in the event of an exception.
 */
void doDmdt(const vector<double>& times, const vector<double>& mags, 
		bool getCut, bool getPlot, 
		CollectedScalars& cut50Amp3, CollectedScalars& cut50Amp2, 
		CollectedScalars& cut90Amp3, CollectedScalars& cut90Amp2, 
		CollectedPairs& dmdtMed) {
	if (times.size() != mags.size()) {
		throw std::invalid_argument("Times and mags must have the same length in doDmdt() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>(mags.size()) + " for mags).");
	}

	if (getCut || getPlot) {
		// copy-and-swap
		CollectedScalars temp50Amp3 = cut50Amp3;
		CollectedScalars temp50Amp2 = cut50Amp2;
		CollectedScalars temp90Amp3 = cut90Amp3;
		CollectedScalars temp90Amp2 = cut90Amp2;
		CollectedPairs   tempMed    = dmdtMed;
		
		try {
			double amplitude = getAmplitude(mags);
			
			if (amplitude > 0) {
				double minBin = -1.97;
				double maxBin = log10(kpftimes::deltaT(times));
				
				DoubleVec binEdges;
				for (double bin = minBin; bin < maxBin; bin += 0.15) {
					binEdges.push_back(pow(10.0,bin));
				}
				// Get the bin containing maxBin as well
				binEdges.push_back(pow(10.0,maxBin));
				
				DoubleVec deltaT, deltaM;
				kpftimes::dmdt(times, mags, deltaT, deltaM);
				
				// Need median for for both getCut and getPlot
				DoubleVec change50;
				deltaMBinQuantile(deltaT, deltaM, binEdges, change50, 0.50);
				
				if (getCut) {
					DoubleVec change90;
					deltaMBinQuantile(deltaT, deltaM, binEdges, change90, 0.90);
					
					// Key cuts
					temp50Amp3.addStat(cutFunction(binEdges, 
						change50, MoreThan(amplitude / 3.0) ));
					temp50Amp2.addStat(cutFunction(binEdges, 
						change50, MoreThan(amplitude / 2.0) ));
					
					temp90Amp3.addStat(cutFunction(binEdges, 
						change90, MoreThan(amplitude / 3.0) ));
					temp90Amp2.addStat(cutFunction(binEdges, 
						change90, MoreThan(amplitude / 2.0) ));
				}

				if (getPlot) {
					tempMed.addStat(binEdges, change50);
				}
			}
		} catch (const except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (const except::Undefined &e) {
			// Don't know how many of the collections were updated... revert to input
			temp50Amp3 = cut50Amp3;
			temp50Amp2 = cut50Amp2;
			temp90Amp3 = cut90Amp3;
			temp90Amp2 = cut90Amp2;
			tempMed    = dmdtMed;
			
			// May still throw bad_alloc
			if (getCut) {
				temp50Amp3.addNull();
				temp50Amp2.addNull();
				temp90Amp3.addNull();
				temp90Amp2.addNull();
			}
//			if (getPlot) {
//				tempMed.addNull();
//			}
		}
		
		// IMPORTANT: no exceptions beyond this point
		
		using std::swap;		
		swap(cut50Amp3, temp50Amp3);
		swap(cut50Amp2, temp50Amp2);
		swap(cut90Amp3, temp90Amp3);
		swap(cut90Amp2, temp90Amp2);
		swap(dmdtMed  , tempMed   );
	}
}

}}	// end lcmc::stats
