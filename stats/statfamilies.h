/** Functions for calculating related statistics without redundancy
 * @file lightcurveMC/stats/statfamilies.h
 * @author Krzysztof Findeisen
 * @date Created June 8, 2013
 * @date Last modified June 8, 2013
 */

#ifndef LCMCSTATFAMH
#define LCMCSTATFAMH

#include <vector>
#include "statcollect.h"

namespace lcmc { namespace stats {

using std::vector;

/** Does all periodogram-related computations for a given light curve.
 */
void doPeriodogram(const vector<double>& times, const vector<double>& data, 
		bool getPeriod, bool getPlot, 
		CollectedScalars& periods, CollectedPairs& periodograms);

/** Does all &Delta;m&Delta;t-related computations for a given light curve.
 */
void doDmdt(const vector<double>& times, const vector<double>& mags, 
		bool getCut, bool getPlot, 
		CollectedScalars& cut50Amp3, CollectedScalars& cut50Amp2, 
		CollectedScalars& cut90Amp3, CollectedScalars& cut90Amp2, 
		CollectedPairs& dmdtMed);

/** Does all ACF-related computations for a given light curve.
 */
void doAcf(const vector<double>& times, const vector<double>& data, 
		void (*acfFunc) (const vector<double>&, const vector<double>&, 
				double, size_t, vector<double>&), 
		bool getCut, bool getPlot, 
		CollectedScalars& cut9, CollectedScalars& cut4, CollectedScalars& cut2, 
		CollectedPairs& acfPlot);

/** Does all peak-finding related computations for a given light curve.
 */
void doPeakMax(const vector<double>& times, const vector<double>& mags, 
		bool getCut, bool getPlot, 
		CollectedScalars& cut3, CollectedScalars& cut2, 
		CollectedScalars& cut80, CollectedPairs& peakPlot);

}}		// end lcmc::stats

#endif		// End ifndef LCMCSTATFAMH
