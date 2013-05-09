/** Functions for extracting timescales out of Delta-m Delta-t plots
 * @file dmdt.h
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified May 9, 2013
 */

#ifndef LCMCDMDTH
#define LCMCDMDTH

#include <vector>
#include <boost/concept_check.hpp>
#include <boost/concept/requires.hpp>

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

}}		// end lcmc::stats

#endif		// end LCMCDMDTH
