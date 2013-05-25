/** Generic statistics for data sets in containers
 * @file utils.tmp.h
 * @author Krzysztof Findeisen
 * @date Created July 21, 2011
 * @date Last modified May 22, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <boost/concept/requires.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include "utils_except.h"

namespace kpfutils {

using namespace boost;
using boost_concepts::ReadableIteratorConcept;
using boost_concepts::ForwardTraversalConcept;
using boost_concepts::RandomAccessTraversalConcept;
 
/** A convenient shorthand for vectors of doubles.
 */
typedef std::vector<double> DoubleVec;

/*----------------------------------------------------------
 * Speed-optimized statistics routines
 * I already had these written, so it was simpler than writing a wrapper to 
 *	convert vectors to C arrays just so GSL can understand them
 */

/** Finds the mean of the values in a generic container object. The container 
 *	class is accessed using first and last iterators, as in the C++ STL 
 *	convention, and the variance is computed over the interval [first, last).
 * 
 * @tparam ConstInputIterator The iterator type for the container over which the 
 *	mean is to be calculated. Must be <a href="http://www.boost.org/doc/libs/release/libs/iterator/doc/ReadableIterator.html">readable</a> and support <a href="http://www.boost.org/doc/libs/release/libs/iterator/doc/ForwardTraversal.html">forward traversal</a>.
 * @param[in] first Input iterator marking the first element in the container.
 * @param[in] last Input iterator marking the position after the last 
 *	element in the container.
 *
 * @return The arithmetic mean of the elements between @p first, inclusive, and 
 *	@p last, exclusive. The return type is that of the elements pointed to by 
 *	the @p first and @p last iterators.
 *
 * @pre [@p first, @p last) is a valid range
 * @pre There is at least one element in the range [@p first, @p last)
 *
 * @perform O(D), where D = std::distance(@p first, @p last).
 *
 * @exception kpfutils::except::NotEnoughData Thrown if there are not enough 
 *	elements to define a mean.
 * 
 * @exceptsafe The range [@p first, @p last) is unchanged in the event of an exception.
 *
 * @test List of ints, length 0. Expected behavior: throw invalid_argument.
 * @test List of ints, length 1. Expected behavior: return list[0]
 * @test List of ints, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_mean in 10 out of 10 trials.
 * @test List of doubles, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_mean to within 1e-10 in 10 out of 10 trials.
 * @test Vector of doubles, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_mean to within 1e-10 in 10 out of 10 trials.
 * @test Array of doubles, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_mean to within 1e-10 in 10 out of 10 trials.
 *
 * @todo Apply concept checking to the return type
 */
template <typename ConstInputIterator> 				// Iterator to use
BOOST_CONCEPT_REQUIRES(
	((ReadableIteratorConcept<ConstInputIterator>)) 
	((ForwardTraversalConcept<ConstInputIterator>)),	// Iterator semantics
	(typename std::iterator_traits<ConstInputIterator>::value_type)) // Return type
mean(ConstInputIterator first, ConstInputIterator last) {
	typedef typename std::iterator_traits<ConstInputIterator>::value_type Value;

	Value  sum = 0;
	long count = 0;

	// Since iterators are passed by value, incrementing first does not 
	//	violate exception guarantee
	for(; first != last; first++) {
		sum += (*first);
		count++;
	}
	if (count <= 0) {
		throw except::NotEnoughData("Not enough data to compute mean");
	}
	return static_cast<Value>(sum / count);
}

/** Finds the variance of the values in a generic container object. The 
 *	container class is accessed using first and last iterators, as in the 
 *	C++ STL convention, and the mean is computed over the interval 
 *	[first, last).
 * 
 * @tparam ConstInputIterator The iterator type for the container over which the 
 *	variance is to be calculated. Must be <a href="http://www.boost.org/doc/libs/release/libs/iterator/doc/ReadableIterator.html">readable</a> and support <a href="http://www.boost.org/doc/libs/release/libs/iterator/doc/ForwardTraversal.html">forward traversal</a>.
 * @param[in] @p first Input iterator marking the first element in the container.
 * @param[in] @p last Input iterator marking the position after the last element in the container.
 *
 * @return The (unbiased) sample variance of the elements between @p first, 
 *	inclusive, and @p last, exclusive. The return type is that of the 
 *	elements pointed to by the @p first and @p last iterators.
 *
 * @pre [@p first, @p last) is a valid range
 * @pre There are at least two elements in the range [@p first, @p last)
 *
 * @perform O(D), where D = std::distance(@p first, @p last).
 *
 * @exception kpfutils::except::NotEnoughData Thrown if there are not enough 
 *	elements to define a variance.
 * 
 * @exceptsafe The range [@p first, @p last) is unchanged in the event of an exception.
 *
 * @test List of ints, length 0. Expected behavior: throw invalid_argument.
 * @test List of ints, length 1. Expected behavior: throw invalid_argument.
 * @test List of ints, length 2. Expected behavior: return (%list::back()-%list::front())^2/2
 * @test List of ints, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_variance in 10 out of 10 trials.
 * @test List of doubles, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_variance to within 1e-10 in 10 out of 10 trials.
 * @test Vector of doubles, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_variance to within 1e-10 in 10 out of 10 trials.
 * @test Array of doubles, length 100, randomly generated. Expected behavior: 
 *	agrees with gsl_stats_variance to within 1e-10 in 10 out of 10 trials.
 *
 * @todo Apply concept checking to the return type
 */
template <typename ConstInputIterator> 				// Iterator to use
BOOST_CONCEPT_REQUIRES(
	((ReadableIteratorConcept<ConstInputIterator>)) 
	((ForwardTraversalConcept<ConstInputIterator>)),	// Iterator semantics
	(typename std::iterator_traits<ConstInputIterator>::value_type)) // Return type
variance(ConstInputIterator first, ConstInputIterator last) {
	typedef typename std::iterator_traits<ConstInputIterator>::value_type Value;

	Value  sum = 0, sumsq = 0;
	long count = 0;

	// Since iterators are passed by value, incrementing first does not 
	//	violate exception guarantee
	for(; first != last; first++) {
		sum   += (*first);
		sumsq += (*first)*(*first);
		count++;
	}
	if (count <= 1) {
		throw except::NotEnoughData("Not enough data to compute variance");
	}

	// Minimize number of divisions and maximize dividend in case value_type is integral
	return static_cast<Value>((sumsq - sum*sum/count)/(count-1));
}

/** Finds the (uninterpolated) quantile of the values in a generic container 
 *	object. The container class is accessed using first and last 
 *	iterators, as in the C++ STL convention, and the quantile is 
 *	computed over the interval [first, last). Data is assumed unsorted.
 * 
 * @tparam ConstRandomAccessIterator The iterator type for the container over which the 
 *	variance is to be calculated. Must be <a href="http://www.boost.org/doc/libs/release/libs/iterator/doc/ReadableIterator.html">readable</a> and support <a href="http://www.boost.org/doc/libs/release/libs/iterator/doc/RandomAccessTraversal.html">random access</a>
 * @param[in] first Input iterator marking the first element in the 
 *	container.
 * @param[in] last Input iterator marking the position after the last 
 *	element in the container.
 * @param[in] quantile The percentile to recover.
 *
 * @return The largest value whose quantile is less than or equal to 
 *	@p quantile.
 *
 * @pre [@p first, @p last) is a valid range
 * @pre There are at least two elements in the range [@p first, @p last)
 * @pre 0 <= @p quantile <= 1
 * @pre No value in [@p first, @p last) is NaN
 *
 * @post The return value is not NaN
 *
 * @perform O(D log D), where D = std::distance(@p first, @p last).
 * 
 * @exception kpfutils::except::InvalidQuantile Thrown if @p quantile is not 
 *	in the interval [0, 1].
 *
 * @exception kpfutils::except::NotEnoughData Thrown if there are not enough 
 *	elements to define a quantile.
 * 
 * @exceptsafe The range [@p first, @p last) is unchanged in the event of an exception.
 *
 * @test List of ints, length 0. Expected behavior: throw invalid_argument.
 * @test List of ints, length 1, quantile=0.00. Expected behavior: return %list::front()
 * @test List of ints, length 1, quantile=0.42. Expected behavior: return %list::front()
 * @test List of ints, length 1, quantile=1.00. Expected behavior: return %list::front()
 * @test List of ints, length 100, randomly generated, quantile=-0.01 or 1.01. 
 *	Expected behavior: throw domain_error
 * @test List of ints, length 100, randomly generated, quantile=0.00, 0.42, or 1.00. 
 *	Expected behavior: TBD
 * @test List of doubles, length 100, randomly generated, quantile=0.00, 0.42, or 1.00. 
 *	Expected behavior: TBD
 * @test Vector of doubles, length 100, randomly generated, quantile=0.00, 0.42, or 1.00. 
 *	Expected behavior: TBD
 * @test Array of doubles, length 100, randomly generated, quantile=0.00, 0.42, or 1.00. 
 *	Expected behavior: TBD
 *
 * @bug Behavior inconsistent with Wikipedia's definition of quantile
 *
 * @todo Finish designing test cases. Can't use gsl_stats_quantile_from_sorted_data 
 *	as an oracle because it interpolates
 * @todo Apply concept checking to the return type
 */
template <typename ConstRandomAccessIterator> 				// Iterator to use
BOOST_CONCEPT_REQUIRES(
	((ReadableIteratorConcept<ConstRandomAccessIterator>)) 
	((RandomAccessTraversalConcept<ConstRandomAccessIterator>)),	// Iterator semantics
	(typename std::iterator_traits<ConstRandomAccessIterator>::value_type)) // Return type
quantile(ConstRandomAccessIterator first, ConstRandomAccessIterator last, double quantile) {
	typedef typename std::iterator_traits<ConstRandomAccessIterator>::value_type Value;
	
	if (quantile < 0.0 || quantile > 1.0) {
		char buf[60];
		sprintf(buf, "Invalid quantile of %0.2f passed to quantile()", quantile);
		throw except::InvalidQuantile(buf);
	}

	// We don't want to alter the data, so we copy while sorting
	// Copy to a vector because sorting requires random access
	size_t vecSize = distance(first, last);
	if (vecSize < 1) {
		throw except::NotEnoughData("Supplied empty data set to quantile()");
	}
	std::vector<Value> sortedVec(vecSize);
	/** @todo Reimplement using partial sorting
	 */
	std::partial_sort_copy(first, last, sortedVec.begin(), sortedVec.end());

	size_t index = 0;
	if (quantile >= 0.0 && quantile < 1.0) {
		index = static_cast<size_t>(quantile*vecSize);
	} else if (quantile == 1.0) {
		index = vecSize-1;
	}
	
	return sortedVec[index];
}

/** Tests whether a range is sorted.
 *
 * This function emulates <tt>std::is_sorted()</tt> for platforms without access 
 * to either a C++11-compliant compiler or Boost 1.50 or later.
 *
 * @tparam ForwardIteratorIterator The iterator type for the container to be tested. 
 * Must be a <a href="http://www.boost.org/doc/libs/release/doc/html/ForwardIterator.html">forward iterator</a>.
 * @param[in] first,last Forward iterators to the range to test. The range 
 *	is all elements in the interval [@p first, @p last).
 *
 * @return True if [@p first, @p last) is sorted in ascending order. A range 
 *	containing less than two elements is always sorted.
 *
 * @perform O(D), where D = std::distance(@p first, @p last).
 *
 * @exceptsafe The range [@p first, @p last) is unchanged in the event of 
 *	an exception if ForwardIterator provides at least the basic 
 *	guarantee. Does not throw exceptions unless ForwardIterator throws.
 */
template <class ForwardIterator>
BOOST_CONCEPT_REQUIRES(
	((ReadableIteratorConcept<ForwardIterator>)) 
	((ForwardTraversalConcept<ForwardIterator>)),	// Iterator semantics
	(bool)) 					// Return type
isSorted (ForwardIterator first, ForwardIterator last) {
	// Code shamelessly copied from http://www.cplusplus.com/reference/algorithm/is_sorted/
	if (first==last) {
		return true;
	}
	
	// Since iterators are passed by value, incrementing first does not 
	//	violate exception guarantee
	ForwardIterator next = first;
	for(ForwardIterator next = first; ++next != last; ++first) {
		if (*next<*first) {
			return false;
		}
	}
	return true;
}

}	// end kpfutils
