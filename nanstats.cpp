/** Functions for handling data containing NaN values
 * @file lightcurveMC/nanstats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 11, 2013
 * @date Last modified May 25, 2013
 */

#include <limits>
#include <vector>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/lexical_cast.hpp>
#include "nan.h"
#include "except/undefined.h"
#include "../common/stats.tmp.h"

namespace lcmc { namespace utils {

using std::vector;
using std::numeric_limits;
using boost::lexical_cast;

/** isNan() tests whether a floating-point number is undefined
 *
 * @param[in] x The number to test
 * 
 * @return true if and only if @p x equals signaling or quiet not-a-number
 *
 * @exceptsafe Does not throw exceptions.
 */
bool isNan(const double x) {
	return (x != x);
}

/** isNanOrInf() tests whether a floating-point number is non-finite
 *
 * @param[in] x The number to test
 * 
 * @return true if and only if @p x equals signaling or quiet not-a-number, 
 *	or @p x is infinite
 *
 * @exceptsafe Does not throw exceptions.
 */
bool isNanOrInf(const double x) {
	return ( isNan(x) || (x ==  numeric_limits<double>::infinity()) 
			  || (x == -numeric_limits<double>::infinity()) );
}

/** lessFinite() allows floating-point numbers to be ordered consistently in the 
 *	presence of NaNs.
 *
 * @param[in] x First value to compare
 * @param[in] y Second value to compare
 *
 * @return If neither @p x nor @p y is NaN, returns (<tt>x < y</tt>). 
 *	Otherwise, assumes NaN is larger than any finite values, but less 
 *	than positive infinity.
 *
 * @exceptsafe Does not throw exceptions.
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

/** Removes NaNs from a pair of vectors.
 *
 * @param[in] badVals A vector of values that may contain NaNs.
 * @param[out] goodVals A vector containing all elements in @p badVals that are 
 *	not NaNs.
 * @param[in] sideVals A vector of other measurements that correspond to the 
 *	values in @p badVals.
 * @param[out] matchVals A vector containing those elements in @p sideVals whose 
 *	counterparts in @p badVals are not NaNs.
 *
 * @pre @p badVals.size() = @p sideVals.size()
 * @pre @p goodVals, @p badVals, @p sideVals, and @p matchVals are all distinct objects
 *
 * @post @p goodVals.size() = @p matchVals.size()
 *
 * @post @p goodVals.size() &le; @p badVals.size()
 * @post @p sideVals.size() &le; @p matchVals.size()
 *
 * @post The elements in @p goodVals are in the same order as in @p badVals
 * @post The elements in @p matchVals are in the same order as in @p sideVals
 *
 * @exception std::invalid_argument Thrown if @p badVals.size() &ne; @p sideVals.size()
 * @exception std::bad_alloc Thrown if could not allocate @p sideVals or @p matchVals
 *
 * @exceptsafe None of the arguments are changed in the event of an exception.
 */
void removeNans(const vector<double>& badVals, vector<double>& goodVals, 
		const vector<double>& sideVals, vector<double>& matchVals) {
	// swap() lookup only works properly with unqualified names. See 
	//	Item 25, Effective C++ 3E
	using std::swap;

	if (badVals.size() != sideVals.size()) {
		try {
			throw std::invalid_argument("Passed arrays of different lengths into removeNans(): " 
				+ lexical_cast<std::string>( badVals.size()) 
				+ " for array with NaNs, and " 
				+ lexical_cast<std::string>(sideVals.size()) 
				+ " for matching array");
		} catch (const boost::bad_lexical_cast &e) {
			throw std::invalid_argument("Passed arrays of different lengths into removeNans().");
		}
	}
	size_t n = badVals.size();

	// copy-and-swap to ensure that throwing std::bad_alloc doesn't 
	//	corrupt goodVals and matchVals
	vector<double> goodOut, matchOut;
	 goodOut.reserve(n);
	matchOut.reserve(n);
	
	for(vector<double>::const_iterator badIt = badVals.begin(), sideIt = sideVals.begin();
			badIt != badVals.end(); badIt++, sideIt++) {
		if (!isNan(*badIt)) {
			goodOut. push_back( *badIt);
			matchOut.push_back(*sideIt);
		}
	}
	
	// IMPORTANT: No exceptions thrown past this point
	// swap<vector> is guaranteed not to throw for equal allocators
	//	Since all vector<double> use the default 
	//	allocator, this condition is satisfied
	swap( goodOut,  goodVals);
	swap(matchOut, matchVals);
}

/** Calculates the mean, ignoring NaNs
 *
 * @param[in] vals The values whose mean to take. May include NaNs.
 *
 * @return The mean of the non-NaN elements of @p vals.
 *
 * @exception lcmc::stats::NotEnoughData Thrown if all the elements of 
 *	@p vals are NaNs.
 *
 * @exceptsafe @p vals is unchanged in the event of an exception.
 */
double meanNoNan(const vector<double>& vals) {
	using namespace boost;
	
	// Ignore all NaN values
	// Predicate argument needs to be default-constructible
	// 	Can't use either std::unary_negate or boost:unary_negate
	typedef filter_iterator<NotNan, vector<double>::const_iterator> NotNanFilter;
	NotNanFilter firstClean(vals.begin(), vals.end());
	NotNanFilter  lastClean(vals.end()  , vals.end());
	
	try {
		return kpfutils::mean(firstClean, lastClean);
	} catch (const kpfutils::except::NotEnoughData &e) {
		throw stats::except::NotEnoughData(e.what());
//		return numeric_limits<double>::quiet_NaN();
	}
}

/** Calculates the variance, ignoring NaNs
 *
 * @param[in] vals The values whose variance to take. May include NaNs.
 *
 * @return The variance of the non-NaN elements of @p vals.
 *
 * @exception lcmc::stats::NotEnoughData Thrown if all or all but one of 
 *	the elements of @p vals are NaNs.
 *
 * @exceptsafe @p vals is unchanged in the event of an exception.
 */
double varianceNoNan(const vector<double>& vals) {
	using namespace boost;
	
	// Ignore all NaN values
	// Predicate argument needs to be default-constructible
	// 	Can't use either std::unary_negate or boost:unary_negate
	typedef filter_iterator<NotNan, vector<double>::const_iterator> NotNanFilter;
	NotNanFilter firstClean(vals.begin(), vals.end());
	NotNanFilter  lastClean(vals.end()  , vals.end());

	try {	
		double rawVariance = kpfutils::variance(firstClean, lastClean);
		// Floor at 0 to prevent rounding errors from causing a negative variance
		if (rawVariance < 0.0 && rawVariance > -1e-12) {
			return 0.0;
		} else {
			return rawVariance;
		}
	} catch (const kpfutils::except::NotEnoughData &e) {
		// Translate to lcmc exception to avoid exposing implementation
		throw stats::except::NotEnoughData(e.what());
//		return numeric_limits<double>::quiet_NaN();
	}
}

}}	// end lcmc::utils
