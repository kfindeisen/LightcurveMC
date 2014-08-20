/** Functions and classes for reducing vector data to scalars
 * @file lightcurveMC/stats/cut.tmp.h
 * @author Krzysztof Findeisen
 * @date Created May 9, 2013
 * @date Last modified July 24, 2013
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

#ifndef LCMCCUTH
#define LCMCCUTH

#include <algorithm>
#include <vector>
#include <boost/concept_check.hpp>
#include <boost/concept/requires.hpp>

namespace lcmc { namespace stats {

using std::vector;

/** Finds the first location on a grid where a predicate is true.
 *
 * @tparam UnaryPredicate The type of condition to test. Must be a <a href="http://www.sgi.com/tech/stl/Predicate.html">unary predicate</a>.
 *
 * @param[in] pos The position grid on which to search.
 * @param[in] func The function to test against cut.
 * @param[in] pred The condition that must be satisfied by the return value.
 *
 * @return The value @p pos[i] at which @p pred(func[i]) is first true, 
 *	or NaN if @p pred(x) is false for all x &isin; @p func.
 *
 * @pre @p pos.size() = @p func.size()
 * 
 * @pre @p pos does not contain NaNs
 * @pre @p func may contain NaNs
 *
 * @perform O(N) time, where N = @p func.size()
 *
 * @exceptsafe If @p pred throws exceptions, then cutFunction() has the same 
 *	exception guarantee as @p pred. Otherwise, does not throw exceptions.
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

/** Finds the last location on a grid where a predicate is true.
 *
 * @tparam UnaryPredicate The type of condition to test. Must be a <a href="http://www.sgi.com/tech/stl/Predicate.html">unary predicate</a>.
 *
 * @param[in] pos The position grid on which to search.
 * @param[in] func The function to test against cut.
 * @param[in] pred The condition that must be satisfied by the return value.
 *
 * @return The value @p pos[i] at which @p pred(func[i]) is last true, 
 *	or NaN if @p pred(x) is false for all x &isin; @p func.
 *
 * @pre @p pos.size() = @p func.size()
 * 
 * @pre @p pos does not contain NaNs
 * @pre @p func may contain NaNs
 *
 * @perform O(N) time, where N = @p func.size()
 *
 * @exceptsafe If @p pred throws exceptions, then cutFunctionReverse() has the 
 *	same exception guarantee as @p pred. Otherwise, does not throw exceptions.
 */
template <class UnaryPredicate> 
BOOST_CONCEPT_REQUIRES(
	((boost::UnaryPredicate<UnaryPredicate, double>)),	// Predicate semantics
	(double)) 						// return type
cutFunctionReverse(const vector<double>& pos, const vector<double>& func, UnaryPredicate pred) {
	vector<double>::const_reverse_iterator whereCut = 
			std::find_if(func.rbegin(), func.rend(), pred);

	if (whereCut != func.rend()) {
		return *(pos.rbegin() + (whereCut-func.rbegin()));
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
	 * @param[in] x The value to compare to @p threshold
	 *
	 * @return True if @p x > @p threshold, false otherwise.
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
	 * @param[in] x The value to compare to @p threshold
	 *
	 * @return True if @p x < @p threshold, false otherwise.
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
