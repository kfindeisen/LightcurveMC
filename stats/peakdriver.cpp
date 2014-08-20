/** Functions for calculating related statistics without redundancy
 * @file lightcurveMC/stats/peakdriver.cpp
 * @author Krzysztof Findeisen
 * @date Created June 8, 2013
 * @date Last modified October 29, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <timescales/timescales.h>
#include "cut.tmp.h"
#include "magdist.h"
#include "../../common/nan.h"
#include "statcollect.h"
#include "statfamilies.h"
#include "../except/undefined.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using std::vector;
using kpftimes::peakFindTimescales;

/** Does all peak-finding related computations for a given light curve.
 *
 * @param[in] times The times at which the light curve was sampled.
 * @param[in] mags The values of the light curve
 * @param[in] getCut Flag indicating that cuts through the the 
 *	peak-finding curve should be extracted
 * @param[in] getPlot Flag indicating that the peak-finding curve should 
 *	be stored
 * @param[out] cut3 The NamedCollection in which to record the 
 *	time offset at which the peak-finding curve crosses 1/3 
 *	of the light curve amplitude.
 * @param[out] cut2 The NamedCollection in which to record the 
 *	time offset at which the peak-finding curve crosses 1/2 
 *	of the light curve amplitude.
 * @param[out] cut80 The NamedCollection in which to record the 
 *	time offset at which the peak-finding curve crosses 80% of its 
 *	maximum value.
 * @param[out] peakPlot The NamedCollection in which to record the 
 *	peak-finding curve.
 *
 * @pre @p times.size() = @p mags.size()
 * @pre Neither @p times nor @p mags may contain NaNs
 * 
 * @post if @p getCut, then new elements are appended to @p cut3, @p cut2, 
 *	and @p cut80. If the cut is undefined, NaN is appended.
 * @post if @p getPlot, then a new element is appended to @p peakPlot.
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
void doPeak(const vector<double>& times, const vector<double>& mags, 
		bool getCut, bool getPlot, 
		CollectedScalars& cut3, CollectedScalars& cut2, 
		CollectedScalars& cut80, CollectedPairs& peakPlot) {

	if (times.size() != mags.size()) {
		throw std::invalid_argument("Times and mags must have the same length in analyzeLightCurve() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>( mags.size()) + " for mags).");
	}

	if (getCut || getPlot) {
		// copy-and-swap
		CollectedScalars temp3    = cut3;
		CollectedScalars temp2    = cut2;
		CollectedScalars temp80   = cut80;
		CollectedPairs   tempPlot = peakPlot;
		
		try {
			double amplitude = getAmplitude(mags);
				
			// Treat cut2 and cut3 separately for improved efficiency
			if (getCut) {
				if (amplitude > 0) {
					DoubleVec magCuts;
					magCuts.push_back(amplitude / 3.0);
					magCuts.push_back(amplitude / 2.0);
				
					DoubleVec cutTimes;
					peakFindTimescales(times, mags, 
						magCuts, cutTimes);
					
					// Key cuts
					temp3.addStat(cutTimes[0]);
					temp2.addStat(cutTimes[1]);
				}
			}
			
			if (amplitude > 0) {
				const static double minMag = 0.01;
				
				DoubleVec magCuts;
				for (double mag = minMag; mag < amplitude; mag += minMag) {
					magCuts.push_back(mag);
				}
				
				DoubleVec cutTimes;
				peakFindTimescales(times, mags, magCuts, cutTimes);
				
				if (getPlot) {
					tempPlot.addStat(cutTimes, magCuts);
				}
				
				if (getCut) {
					// 80% of the highest mag with a defined timescale
					double mag08 = 0.8 * 
						cutFunctionReverse(magCuts, cutTimes, 
							kpfutils::NotNan());
					
					// can't use cutFunction() because cutTimes may 
					//	have NaNs
					DoubleVec singleTime;
					peakFindTimescales(times, mags, 
						vector<double>(1, mag08), singleTime);
					// singleTime.size() == 1 guaranteed
					temp80.addStat(singleTime.front());
				}
			}
		} catch (const except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (const except::Undefined &e) {
			// Don't know how many of the collections were updated... revert to input
			temp3    = cut3;
			temp2    = cut2;
			temp80   = cut80;
			tempPlot = peakPlot;
			
			// May still throw bad_alloc
			if (getCut) {
				temp3 .addNull();
				temp2 .addNull();
				temp80.addNull();
			}
//			if (getPlot) {
//				tempPlot.addNull();
//			}
		}
		
		// IMPORTANT: no exceptions beyond this point
		
		using std::swap;		
		swap(cut3    , temp3 );
		swap(cut2    , temp2 );
		swap(cut80   , temp80);
		swap(peakPlot, tempPlot);
	}
}

}}		// end lcmc::stats
