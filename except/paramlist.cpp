/** Exceptions for ParamList and related types
 * @file lightcurveMC/except/paramlist.cpp
 * @author Krzysztof Findeisen
 * @date Created April 22, 2013
 * @date Last modified May 7, 2013
 */

#include <stdexcept>
#include <string>
#include "exception.h"
#include "paramlist.h"
#include "../paramlist.h"


namespace lcmc { namespace models { namespace except {

using std::invalid_argument;
using std::string;

/** Constructs a MissingParam object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] param The parameter that could not be found in RangeList.
 *
 * @post this->what() == what_arg.c_str()
 * @post this->getParam() == param
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
MissingParam::MissingParam(const string& what_arg, const ParamType& param) 
		: CheckedException(what_arg), param(param) {
}

MissingParam::~MissingParam() throw () {
}

/** Identifies the parameter that triggered the exception.
 *
 * @return A parameter equal to that passed as the second argument of 
 *	the constructor.
 *
 * @exceptsafe Does not throw exceptions.
 */
const ParamType MissingParam::getParam() const {
	return param;
}

/** Constructs an ExtraParam object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] param The parameter that was duplicated.
 *
 * @post this->what() == what_arg.c_str()
 * @post this->getParam() == param
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
ExtraParam::ExtraParam(const string& what_arg, const ParamType& param) 
		: invalid_argument(what_arg), param(param) {
}

ExtraParam::~ExtraParam() throw () {
}

/** Identifies the parameter that triggered the exception.
 *
 * @return A parameter equal to that passed as the second argument of 
 *	the constructor.
 *
 * @exceptsafe Does not throw exceptions.
 */
const ParamType ExtraParam::getParam() const {
	return param;
}

/** Constructs a NegativeRange object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] min The minimum value of the range that triggered the exception.
 * @param[in] max The maximum value of the range that triggered the exception.
 *
 * @post this->what() == what_arg.c_str()
 * @post this->getMin() == min
 * @post this->getMax() == max
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
NegativeRange::NegativeRange(const string& what_arg, double min, double max) 
		: CheckedException(what_arg), min(min), max(max) {
}

/** Constructs a NegativeRange object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] range A pair containing, in the first and second fields, 
 *	the minimum and maximum values of the parameter that triggered 
 *	the exception
 *
 * @post this->what() == what_arg.c_str()
 * @post this->getMin() == range.first
 * @post this->getMax() == range.second
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
NegativeRange::NegativeRange(const string& what_arg, std::pair<double, double> range) 
		: CheckedException(what_arg), min(range.first), max(range.second) {
}

/** Identifies the lower limit of the range that triggered the exception.
 *
 * @return A copy of the lower limit.
 *
 * @exceptsafe Does not throw exceptions.
 */
double NegativeRange::getMin() const {
	return min;
}

/** Identifies the upper limit of the range that triggered the exception.
 *
 * @return A copy of the upper limit.
 *
 * @exceptsafe Does not throw exceptions.
 */
double NegativeRange::getMax() const {
	return max;
}

}}}		// end lcmc::models::except
