/** Functions for handling data containing NaN values
 * @file nanstats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 11, 2013
 * @date Last modified April 21, 2013
 */

#include <limits>
#include <vector>
#include "utils.tmp.h"
#include "nan.h"

namespace lcmc { namespace utils {

using std::vector;
using std::numeric_limits;

/** isNan tests whether a floating-point number is undefined
 *
 * @param[in] x The number to test
 * 
 * @return true if and only if x equals signaling or quiet not-a-number
 */
bool isNan(double x) {
	return (x != x);
}

/** isNanOrInf tests whether a floating-point number is non-finite
 *
 * @param[in] x The number to test
 * 
 * @return true if and only if x equals signaling or quiet not-a-number, 
 *	or x is infinite
 */
bool isNanOrInf(double x) {
	return ( (x != x) || (x ==  numeric_limits<double>::infinity()) 
			  || (x == -numeric_limits<double>::infinity()) );
}

/** lessFinite allows floating-point numbers to be compared safely in the 
 *	presence of NaNs.
 *
 * @param[in] x First value to compare
 * @param[in] y Second value to compare
 *
 * @return If neither x nor y is NaN, returns (x < y). Otherwise, assumes 
 *	NaN is larger than any finite values, but less than positive infinity.
 */
bool lessFinite(double x, double y) {
	if (!isNan(x) && !isNan(y)) {
		return (x < y);
	} else if (isNan(x)) {
		// if isNan(y), returns false, as it should
		return (y == numeric_limits<double>::infinity());
	} else {
		// isNan(y) && !isNan(x)
		return (x != numeric_limits<double>::infinity());
	}
}

/** Removes nans from a pair of vectors.
 *
 * @param[in] badVals A vector of values that may contain NaNs.
 * @param[out] goodVals A vector containing all elements in badVals that are 
 *	not NaNs.
 * @param[in] sideVals A vector of other measurements that correspond to the 
 *	values in badVals.
 * @param[out] matchVals A vector containing those elements in sideVals whose 
 *	counterparts in badVals are not NaNs.
 *
 * @pre badVals.size() == sideVals.size()
 * @pre goodVals, badVals, sideVals, and matchVals are all distinct objects
 *
 * @post goodVals.size() == matchVals.size()
 *
 * @post goodVals.size() <= badVals.size()
 * @post sideVals.size() <= matchVals.size()
 *
 * @post The elements in goodVals are in the same order as in badVals
 * @post The elements in matchVals are in the same order as in sideVals
 */
void removeNans(const DoubleVec& badVals, DoubleVec& goodVals, 
		const DoubleVec& sideVals, DoubleVec& matchVals) {
	if (badVals.size() != sideVals.size()) {
		throw std::logic_error("Passed arrays of different lengths into removeNans().");
	}
	
	goodVals.clear();
	matchVals.clear();
	for(DoubleVec::const_iterator badIt = badVals.begin(), sideIt = sideVals.begin();
			badIt != badVals.end(); badIt++, sideIt++) {
		if (!isNan(*badIt)) {
			goodVals. push_back( *badIt);
			matchVals.push_back(*sideIt);
		}
	}
}

/** Calculates the mean, ignoring NaNs
 *
 * @param[in] vals The values of which to take the mean. May include NaNs.
 *
 * @return The mean of the non-NaN elements of vals.
 */
double meanNoNan(const vector<double>& vals) {
	// Get rid of all NaN values
	vector<double> clean(vals.size());
	vector<double>::iterator newEnd = remove_copy_if(vals.begin(), vals.end(), 
			clean.begin(), &isNan);
	clean.erase(newEnd, clean.end());
	
	return mean(clean.begin(), clean.end());
}

/** Calculates the variance, ignoring NaNs
 *
 * @param[in] vals The values of which to take the variance. May include NaNs.
 *
 * @return The variance of the non-NaN elements of vals.
 */
double varianceNoNan(const vector<double>& vals) {
	// Get rid of all NaN values
	vector<double> clean(vals.size());
	vector<double>::iterator newEnd = remove_copy_if(vals.begin(), vals.end(), 
			clean.begin(), &isNan);
	clean.erase(newEnd, clean.end());
	
	double rawVariance = variance(clean.begin(), clean.end());
	// Floor at 0 to prevent rounding errors from causing a negative variance
	if (rawVariance < 0.0 && rawVariance > -1e-12) {
		return 0.0;
	} else {
		return rawVariance;
	}
}

}}	// end lcmc::utils
