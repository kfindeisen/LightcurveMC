/** Exception definitions for invalid data
 * @file lightcurveMC/except/data.cpp
 * @author Krzysztof Findeisen
 * @date Created May 8, 2013
 * @date Last modified May 11, 2013
 */

#include <stdexcept>
#include <string>
#include "exception.h"
#include "data.h"

namespace lcmc { 

using std::string;

namespace stats { namespace except {

/** Constructs a NotSorted object.
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
NotSorted::NotSorted(const string& what_arg) : invalid_argument(what_arg) {
}

}}		// end lcmc::stats::except

namespace models { namespace except {

/** Constructs a BadParam object.
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
BadParam::BadParam(const string& what_arg) 
	: CheckedException(what_arg) {
}

}}		// end lcmc::models::except

}		// end lcmc
