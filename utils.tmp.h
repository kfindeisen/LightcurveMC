/** Support code for Featurizer
 * @file utils.tmp.h
 * @author Krzysztof Findeisen
 * @date Created July 21, 2011
 * @date Last modified April 21, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cstdio>
 
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
 * @tparam InputIterator The iterator type for the container over which the 
 *	mean is to be calculated
 * @param[in] first Input iterator marking the first element in the container.
 * @param[in] last Input iterator marking the position after the last 
 *	element in the container.
 *
 * @return The arithmetic mean of the elements between first, inclusive, and 
 *	last, exclusive. The return type is that of the elements pointed to by the first and 
 *	last iterators.
 *
 * @pre first is "before" last in the sense that incrementing first repeatedly 
 *	would reach last.
 * @pre There is at least one element in the interval [first, last)
 * @exception invalid_argument Thrown if there are not enough elements.
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
 */
template <typename InputIterator> 				// Iterator to use
		typename std::iterator_traits<InputIterator>::value_type 	// Type pointed to by iterator
		mean(InputIterator first, InputIterator last) {
	typedef typename std::iterator_traits<InputIterator>::value_type Value;

	Value  sum = 0;
	long count = 0;

	for(; first != last; first++) {
		sum += (*first);
		count++;
	}
	if (count <= 0) {
		throw std::invalid_argument("Not enough data to compute mean");
	}
	return static_cast<Value>(sum / count);
}

/** Finds the variance of the values in a generic container object. The 
 *	container class is accessed using first and last iterators, as in the 
 *	C++ STL convention, and the mean is computed over the interval 
 *	[first, last).
 * 
 * @tparam InputIterator The iterator type for the container over which the 
 *	variance is to be calculated
 * @param[in] first Input iterator marking the first element in the container.
 * @param[in] last Input iterator marking the position after the last element in the container.
 *
 * @return The (unbiased) sample variance of the elements between first, 
 *	inclusive, and last, exclusive. The return type is that of the 
 *	elements pointed to by the first and last iterators.
 *
 * @pre first is "before" last in the sense that incrementing first repeatedly 
 *	would reach last.
 * @pre There is at least two elements in the interval [first, last)
 * @exception invalid_argument Thrown if there are not enough elements.
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
 */
template <typename InputIterator> 				// Iterator to use
		typename std::iterator_traits<InputIterator>::value_type 	// Type pointed to by iterator 
		variance(InputIterator first, InputIterator last) {
	typedef typename std::iterator_traits<InputIterator>::value_type Value;

	Value  sum = 0, sumsq = 0;
	long count = 0;

	for(; first != last; first++) {
		sum   += (*first);
		sumsq += (*first)*(*first);
		count++;
	}
	if (count <= 1) {
		throw std::invalid_argument("Not enough data to compute variance");
	}

	// Minimize number of divisions and maximize dividend in case value_type is integral
	return static_cast<Value>((sumsq - sum*sum/count)/(count-1));
}

/** Finds the (uninterpolated) quantile of the values in a generic container 
 *	object. The container class is accessed using first and last 
 *	iterators, as in the C++ STL convention, and the quantile is 
 *	computed over the interval [first, last). Data is assumed unsorted.
 * 
 * @tparam InputIterator The iterator type for the container over which the 
 *	variance is to be calculated
 * @param[in] first Input iterator marking the first element in the 
 *	container.
 * @param[in] last Input iterator marking the position after the last 
 *	element in the container.
 * @param[in] quantile The percentile to recover.
 *
 * @return The largest value whose quantile is less than or equal to the 
 *	parameter "quantile".
 *
 * @pre first is "before" last in the sense that incrementing first repeatedly 
 *	would reach last.
 * @pre There is at least two elements in the interval [first, last)
 * @pre 0 <= quantile <= 1
 * @exception invalid_argument Thrown if there are not enough elements.
 * @exception domain_error Thrown if quantile is an invalid number.
 *
 * @todo Finish designing test cases. Can't use gsl_stats_quantile_from_sorted_data 
 *	as an oracle because it interpolates
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
 */
template <typename InputIterator> 				// Iterator to use
		typename std::iterator_traits<InputIterator>::value_type 	// Type pointed to by iterator 
		quantile(InputIterator first, InputIterator last, double quantile) {
	typedef typename std::iterator_traits<InputIterator>::value_type Value;
	
	// We don't want to alter the data, so we copy while sorting
	// Copy to a vector because sorting requires random access
	size_t vecSize = distance(first, last);
	if (vecSize < 1) {
		throw std::invalid_argument("Supplied empty data set to quantile()");
	}
	std::vector<Value> sortedVec(vecSize);
	std::partial_sort_copy(first, last, sortedVec.begin(), sortedVec.end());

	size_t index = 0;
	if (quantile >= 0.0 && quantile < 1.0) {
		index = static_cast<size_t>(quantile*vecSize);
	} else if (quantile == 1.0) {
		index = vecSize-1;
	} else {
		char buf[60];
		sprintf(buf, "Invalid quantile of %0.2f passed to quantile()", quantile);
		throw std::domain_error(buf);
	}
	
	return sortedVec[index];
}
