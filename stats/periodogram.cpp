/** Calculates periodograms and periods
 * @file lightcurveMC/stats/periodogram.cpp
 * @author Krzysztof Findeisen
 * @date Created June 8, 2013
 * @date Last modified June 8, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <timescales/timescales.h>
#include "statcollect.h"
#include "statfamilies.h"
#include "../except/undefined.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using std::vector;

/** Does all periodogram-related computations for a given light curve.
 *
 * @param[in] times The times at which the light curve was sampled.
 * @param[in] data The values of the light curve
 * @param[in] getPeriod Flag indicating that the best period (having <1% FAP 
 *	in the case of Gaussian white noise) should be extracted
 * @param[in] getPlot Flag indicating that the periodograms should be stored
 * @param[out] periods The NamedCollection in which to record the period, 
 *	if any.
 * @param[out] periodograms The NamedCollection in which to record 
 *	the periodograms.
 *
 * @pre @p times.size() = @p data.size()
 * @pre Neither @p times nor @p data may contain NaNs
 * 
 * @post if @p getPeriod, then a new element is appended to @p periods. 
 *	If no significant period is found, the appended value is NaN.
 * @post if @p getPlot, then a new element is appended to @p periodograms.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store more statistics.
 * @exception std::invalid_argument Thrown if @p times and @p data do not 
 *	have matching lengths or if @p times or @p data contains NaNs.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times 
 *	and @p data are too short to calculate the desired statistics.
 *
 * @exceptsafe The program is in a consistent state in the event of an exception.
 */
void doPeriodogram(const vector<double>& times, const vector<double>& data, 
		bool getPeriod, bool getPlot, 
		CollectedScalars& periods, CollectedPairs& periodograms) {
	if (times.size() != data.size()) {
		throw std::invalid_argument("Times and data must have the same length in doPeriodogram() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>(data.size()) + " for data).");
	}

	if (getPeriod || getPlot) {
		try {
			double freqMin = 1.0/kpftimes::deltaT(times);
			double freqMax = kpftimes::pseudoNyquistFreq(times);
			if (freqMin < 0.005) {
				freqMin = 0.005;
			}
			
			DoubleVec freq;
			kpftimes::freqGen(times, freq, freqMin, freqMax);
			
			// False alarm probability
			static double cacheFreqMin = -1.0, cacheFreqMax = -1.0;
			static double threshold;
			if (cacheFreqMax < 0.0 
					|| cacheFreqMin != freqMin 
					|| cacheFreqMax != freqMax) {
				threshold = kpftimes::lsThreshold(times, freq, 
						0.01, 1000);
				cacheFreqMin = freqMin;
				cacheFreqMax = freqMax;
			}
			
			// Periodogram
			DoubleVec power;
			kpftimes::lombScargle(times, data, freq, power);
			
			if (getPeriod) {	
				// Find the highest peak in the periodogram
				DoubleVec::const_iterator iMax = 
						std::max_element(power.begin(), power.end());
				if(iMax == power.end()) {
					throw std::logic_error("No power spectrum recorded!");
				}

				// no exceptions (other than bad_alloc) beyond this point

				if (*iMax < threshold) {
					// Need an index on freqGrid as well
					DoubleVec::const_iterator fMax = freq.begin() + (iMax - power.begin());
					
					periods.addStat(1.0 / *fMax);
				} else {
					periods.addNull();
				}
			}
			
			// no exceptions (other than bad_alloc) beyond this point

			if (getPlot) {
				periodograms.addStat(freq, power);
			}
			
		} catch (const std::invalid_argument& e) {
			throw except::NotEnoughData(e.what());
		}
	}
}

}}		// end lcmc::stats
