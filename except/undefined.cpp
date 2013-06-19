/** Exception definitions for missing data errors
 * @file lightcurveMC/except/undefined.cpp
 * @author Krzysztof Findeisen
 * @date Created May 5, 2013
 * @date Last modified June 18, 2013
 */

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"
#include "undefined.h"

namespace lcmc { namespace stats { namespace except {

using std::string;

/** Constructs an Undefined object.
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
Undefined::Undefined(const string& what_arg) : CheckedException(what_arg) {
}

/** Constructs a LowerBound object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] threshold A lower limit on the value of the statistic.
 *
 * @post this->what() = @p what_arg.c_str()
 * @post this->getBound() = @p threshold
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
LowerBound::LowerBound(const string& what_arg, double threshold) 
		: Undefined(what_arg), limit(threshold) {
}

/** Returns the value of the lower limit.
 *
 * @return The lower limit on the value that triggered the exception.
 *
 * @exceptsafe Does not throw exceptions.
 */
double LowerBound::getBound() const {
	return limit;
}

/** Constructs an UpperBound object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] threshold An upper limit on the value of the statistic.
 *
 * @post this->what() = @p what_arg.c_str()
 * @post this->getBound() = @p threshold
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
UpperBound::UpperBound(const string& what_arg, double threshold) 
		: Undefined(what_arg), limit(threshold) {
}

/** Returns the value of the upper limit.
 *
 * @return The upper limit on the value that triggered the exception.
 */
double UpperBound::getBound() const {
	return limit;
}

/** Constructs a NotEnoughData object.
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
NotEnoughData::NotEnoughData(const string& what_arg) : Undefined(what_arg) {
}

}}}		// end lcmc::stats::except
