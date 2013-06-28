/** Exception definitions related to R sessions
 * @file lightcurveMC/except/r.cpp
 * @author Krzysztof Findeisen
 * @date Created June 24, 2013
 * @date Last modified June 24, 2013
 */

#include <stdexcept>
#include <string>
#include "../r_compat.h"

namespace lcmc { namespace except {

using std::string;

/** Constructs a BadRSession object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 *
 * @post this->what() = @p what_arg.c_str()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
BadRSession::BadRSession(const string& what_arg) : std::runtime_error(what_arg) {
}

/** Constructs a RError object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 *
 * @post this->what() = @p what_arg.c_str()
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
RError::RError(const string& what_arg) : std::runtime_error(what_arg) {
}

}}		// end lcmc::except
