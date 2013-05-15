/** Exception definitions related to Not-a-Number
 * @file lightcurveMC/except/nan.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2013
 * @date Last modified May 2, 2013
 */

#include <stdexcept>
#include <string>
#include "nan.h"

namespace lcmc { namespace utils { namespace except {

using std::logic_error;
using std::string;

/** Constructs an UnexpectedNan object.
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
UnexpectedNan::UnexpectedNan(const string& what_arg) : invalid_argument(what_arg) {
}

}}}		// end lcmc::utils::except
