/** Functions for analyzing Delta-m Delta-t plots
 * @file lightcurveMC/stats/dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified October 29, 2013
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

#include <stdexcept>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <timescales/timescales.h>
#include "cut.tmp.h"
#include "magdist.h"
#include "statfamilies.h"
#include "../except/undefined.h"
#include "../../common/stats.tmp.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using kpfutils::quantile;
using std::vector;

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
				kpftimes::deltaMBinQuantile(deltaT, deltaM, binEdges, change50, 0.50);
				
				if (getCut) {
					DoubleVec change90;
					kpftimes::deltaMBinQuantile(deltaT, deltaM, binEdges, change90, 0.90);
					
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
