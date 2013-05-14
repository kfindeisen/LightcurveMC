/** Functions and classes for reducing vector data to scalars
 * @file cut.tmp.h
 * @author Krzysztof Findeisen
 * @date Created May 9, 2013
 * @date Last modified May 12, 2013
 */

#ifndef LCMCCUTH
#define LCMCCUTH

#include <vector>
#include <boost/concept_check.hpp>
#include <boost/concept/requires.hpp>

namespace lcmc { namespace stats {

using std::vector;

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
 *
 * @exceptsafe If pred throws exceptions, then cutFunction() has the same 
 *	exception guarantee as pred. Otherwise, does not throw exceptions.
 */
template <class UnaryPredicate> 
BOOST_CONCEPT_REQUIRES(
	((boost::UnaryPredicate<UnaryPredicate, double>)),	// Predicate semantics
	(double)) 						// cutFunction() return type
cutFunction(const vector<double>& pos, const vector<double>& func, UnaryPredicate pred) {
	vector<double>::const_iterator whereCut = std::find_if(func.begin(), func.end(), pred);

	if (whereCut != func.end()) {
		return *(pos.begin() + (whereCut-func.begin()));
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

/** Unary predicate for whether a value is more than some threshold
 */
class MoreThan {
public: 
	/** Sets the value to which other values will be compared
	 *
	 * @param[in] threshold The value to use for comparisons.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	MoreThan(double threshold) : limit(threshold) {
	}
	
	/** Unary comparison of x
	 *
	 * @param[in] x The value to compare to threshold
	 *
	 * @return True if x > threshold, false otherwise.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	bool operator() (double x) {
		return (x > limit);
	}

private: 
	double limit;
};
BOOST_CONCEPT_ASSERT((boost::UnaryPredicate<MoreThan, double>));

/** Unary predicate for whether a value is less than some threshold
 */
class LessThan {
public: 
	/** Sets the value to which other values will be compared
	 *
	 * @param[in] threshold The value to use for comparisons.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	LessThan(double threshold) : limit(threshold) {
	}
	
	/** Unary comparison of x
	 *
	 * @param[in] x The value to compare to threshold
	 *
	 * @return True if x < threshold, false otherwise.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	bool operator() (double x) {
		return (x < limit);
	}

private: 
	double limit;
};
BOOST_CONCEPT_ASSERT((boost::UnaryPredicate<LessThan, double>));

}}		// end lcmc::stats

#endif		// end LCMCCUTH
