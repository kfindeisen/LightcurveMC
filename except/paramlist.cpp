/** Exceptions for ParamList and related types
 * @file lightcurveMC/except/paramlist.cpp
 * @author Krzysztof Findeisen
 * @date Created April 22, 2013
 * @date Last modified June 18, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"
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
 * @post this->what() = @p what_arg.c_str()
 * @post this->getParam() = @p param
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
 * @post this->what() = @p what_arg.c_str()
 * @post this->getParam() = @p param
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
 * @post this->what() = @p what_arg.c_str()
 * @post this->getMin() = @p min
 * @post this->getMax() = @p max
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
 * @param[in] range A pair containing, in the @p first and @p second fields, 
 *	the minimum and maximum values of the parameter that triggered 
 *	the exception
 *
 * @post this->what() = @p what_arg.c_str()
 * @post this->getMin() = @p range.first
 * @post this->getMax() = @p range.second
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
