/** Exception for command-line parser
 * @file lightcurveMC/except/parse.cpp
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
 * @date Last modified May 11, 2013
 */

#include <stdexcept>
#include <string>
#include "exception.h"
#include "parse.h"

namespace lcmc { namespace parse { namespace except {

using std::string;

/** Constructs a ParseError object.
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
ParseError::ParseError(const string& what_arg) : CheckedException(what_arg) {
}

/** Constructs a NoLightCurves object.
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
NoLightCurves::NoLightCurves(const string& what_arg) : ParseError(what_arg) {
}

/** Constructs a NoStats object.
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
NoStats::NoStats(const string& what_arg) : ParseError(what_arg) {
}

}}}		// end lcmc::models::except
