/** Exception definitions for utils.tmp.h
 * @file utils_except.cpp
 * @author Krzysztof Findeisen
 * @date Created May 5, 2013
 * @date Last modified May 5, 2013
 */

#include <stdexcept>
#include <string>
#include "utils_except.h"

namespace kpfutils { namespace except {

using std::string;

/** Constructs a NotEnoughData object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 *
 * @post @p this->what() == @p what_arg.c_str()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
NotEnoughData::NotEnoughData(const string& what_arg) : invalid_argument(what_arg) {
}

/** Constructs an InvalidQuantile object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 *
 * @post @p this->what() == @p what_arg.c_str()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
InvalidQuantile::InvalidQuantile(const string& what_arg) : domain_error(what_arg) {
}

}}		// end kpfutils::except
