/** Conversion functions between vectors and arrays
 * @file vecarray.tmp.h
 * @author Krzysztof Findeisen
 * @date Created May 26, 2013
 * @date Last modified May 26, 2013
 */

#include <stdexcept>
#include <vector>
#include <boost/smart_ptr.hpp>

/** Returns an array copy of a vector.
 *
 * @param[in] vec The vector to copy.
 *
 * @return An array containing the same elements in the same order 
 *	as @p vec.
 *
 * @pre @p vec.size() > 0
 *
 * @exception std::bad_alloc Thrown if could not allocate the array.
 * @exception std::length_error Thrown if attempted to create 
 *	a length-0 array.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
template <typename T>
boost::shared_array<T> vecToArr(const std::vector<T>& vec) {
	size_t n = vec.size();
	if (n <= 0) {
		throw std::length_error("Empty vector passed to vecToArray().");
	}
	
	boost::shared_array<T> arr(new T[n]);
	for(size_t i = 0; i < n; i++) {
		arr[i] = vec[i];
	}
	
	return arr;
}

/** Returns a vector copy of an array.
 *
 * @param[in] arr,n The array to copy.
 *
 * @return A vector containing the same elements in the same order 
 *	as @p arr.
 *
 * @exception std::bad_alloc Thrown if could not allocate the array.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
template <typename T>
std::vector<T> arrToVec(const T arr[], size_t n) {
	std::vector<T> vec;
	vec.reserve(n);
	
	for(size_t i = 0; i < n; i++) {
		vec.push_back(arr[i]);
	}
	
	return vec;
}
