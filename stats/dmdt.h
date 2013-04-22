/** Functions for extracting timescales out of Delta-m Delta-t plots
 * @file dmdt.h
 * by Krzysztof Findeisen
 * Created April 12, 2013
 * Last modified April 12, 2013
 */

#ifndef LCMCDMDTH
#define LCMCDMDTH

#include <vector>

namespace lcmc { namespace stats {

typedef std::vector<double> DoubleVec;

/** Computes the fraction of pairs of magnitudes above some threshold found 
 *	in each &Delta;t bin of a &Delta;m&Delta;t plot.
 */
void hiAmpBinFrac(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &fracs, double threshold);

/** Computes the quantile of pairs of magnitudes found in each &Delta;t bin 
 *	of a &Delta;m&Delta;t plot.
 */
void deltaMBinQuantile(const DoubleVec &deltaT, const DoubleVec &deltaM, 
		const DoubleVec &binEdges, DoubleVec &quants, double q);

/** Finds the first element of a range that exceeds a given value.
 */
DoubleVec::const_iterator crossThreshold(const DoubleVec& grid, double threshold);

/** Finds the location on a grid where a function exceeds a threshold.
 */
double cutFunction(const DoubleVec& pos, const DoubleVec& func, double cut);

}}		// end lcmc::stats

#endif		// end LCMCDMDTH
