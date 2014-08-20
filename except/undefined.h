/** Exception declarations for missing data errors
 * @file lightcurveMC/except/undefined.h
 * @author Krzysztof Findeisen
 * @date Created May 5, 2013
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

#ifndef LCMCUNDEFEXCEPTH
#define LCMCUNDEFEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"

namespace lcmc { namespace stats { namespace except {

using std::string;

/** This exception is thrown if a statistic cannot be calculated from the 
 *	available data.
 */
class Undefined : public kpfutils::except::CheckedException {
public:
	/** Constructs an Undefined object.
	 */
	explicit Undefined(const string& what_arg);
};

/** This exception is thrown if only a lower limit is available on the value 
 *	of a statistic
 */
class LowerBound : public Undefined {
public:
	/** Constructs a LowerBound object.
	 */
	explicit LowerBound(const string& what_arg, double threshold);

	/** Returns the value of the lower limit.
	 */
	double getBound() const;
private:
	double limit;
};

/** This exception is thrown if only a upper limit is available on the value 
 *	of a statistic
 */
class UpperBound : public Undefined {
public:
	/** Constructs an UpperBound object.
	 */
	explicit UpperBound(const string& what_arg, double threshold);

	/** Returns the value of the upper limit.
	 */
	double getBound() const;
private:
	double limit;
};

/** This exception is thrown if a range does not contain enough data to 
 *	calculate the desired statistic
 */
class NotEnoughData : public Undefined {
public:
	/** Constructs a NotEnoughData object.
	 */
	explicit NotEnoughData(const string& what_arg);
};

}}}		// end lcmc::stats::except

#endif		// end ifndef LCMCUNDEFEXCEPTH
