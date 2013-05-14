/** Exception definitions for iterator-related errors
 * @file except/iterator.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2013
 * @date Last modified May 2, 2013
 */

#include <stdexcept>
#include <string>
#include "iterator.h"

namespace lcmc { namespace utils { namespace except {

using std::string;

/** Constructs a BadIterator object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 *
 * @post this->what() == what_arg.c_str()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
BadIterator::BadIterator(const string& what_arg) : out_of_range(what_arg) {
}

BadIterator::~BadIterator() throw() {
}

}}}		// end lcmc::utils::except
