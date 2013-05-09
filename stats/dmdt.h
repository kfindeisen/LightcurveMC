/** Functions for extracting timescales out of Delta-m Delta-t plots
 * @file dmdt.h
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified April 12, 2013
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

/** Finds the first element of a range that exceeds a given value.
 */
DoubleVec::const_iterator crossThreshold(const DoubleVec& grid, double threshold);

/** Finds the location on a grid where a function exceeds a threshold.
 */
double cutFunction(const DoubleVec& pos, const DoubleVec& func, double cut);

/** Finds the location on a grid where a predicate is true.
 *
 * @param[in] pos The position grid on which to search.
 * @param[in] func The function to test against cut.
 * @param[in] pred The condition that must be satisfied by the return value.
 *
 * @return The value of pos at which pred(func) is first true, or NaN if no 
 *	such value exists.
 *
 * @pre pos.size() == func.size()
 * 
 * @pre pos does not contain NaNs
 * @pre func may contain NaNs
 */
template <class MyUnaryPredicate> 
BOOST_CONCEPT_REQUIRES(
	((UnaryFunction<MyUnaryPredicate, double>)),	// Predicate semantics
	(double)) // Return type
cutFunction(const DoubleVec& pos, const DoubleVec& func, MyUnaryPredicate pred) {
	DoubleVec::const_iterator whereCut = std::find_if(func.begin(), func.end(), pred);

	if (whereCut != func.end()) {
		return *(pos.begin() + (whereCut-func.begin()));
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

}}		// end lcmc::stats

#endif		// end LCMCDMDTH
