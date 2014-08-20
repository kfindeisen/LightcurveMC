/** Exception definitions for ParamList and related types
 * @file lightcurveMC/except/paramlist.h
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

#ifndef LCMCPARAMEXCEPTH
#define LCMCPARAMEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"
#include "../paramlist.h"

namespace lcmc { namespace models { namespace except {

using std::string;

/** This exception is thrown if a query to a RangeList cannot find a 
 * particular parameter.
 *
 * The missing parameter is queryable in a machine-readable form.
 */
class MissingParam : public kpfutils::except::CheckedException {
public:
	/** Constructs a MissingParam object.
	 */
	explicit MissingParam(const string& what_arg, const ParamType& param);
	virtual ~MissingParam() throw();
	
	/** Identifies the parameter that triggered the exception.
	 */
	const ParamType getParam() const;
private: 
	ParamType param;
};

/** This exception is thrown if one attempts to add a duplicate entry 
 * to a ParamList or RangeList.
 *
 * The duplicate parameter is queryable in a machine-readable form.
 */
class ExtraParam : public std::invalid_argument {
public:
	/** Constructs an ExtraParam object.
	 */
	explicit ExtraParam(const string& what_arg, const ParamType& param);
	virtual ~ExtraParam() throw();
	
	/** Identifies the parameter that triggered the exception.
	 */
	const ParamType getParam() const;
private: 
	ParamType param;
};

/** This exception is thrown if a function attempts to add a range whose 
 * maximum is less than its minimum to a RangeList.
 *
 * The minimum and maximum values are queryable in a machine-readable form.
 */
class NegativeRange : public kpfutils::except::CheckedException {
public: 
	/** Constructs a NegativeRange object.
	 */
	explicit NegativeRange(const string& what_arg, double min, double max);
	
	/** Constructs a NegativeRange object.
	 */
	explicit NegativeRange(const string& what_arg, std::pair<double, double> range);
	
	/** Identifies the lower limit of the range that triggered the exception.
	 */
	double getMin() const;

	/** Identifies the upper limit of the range that triggered the exception.
	 */
	double getMax() const;
private: 
	double min;
	double max;
};

}}}		// end lcmc::models::except

#endif		// end ifndef LCMCPARAMEXCEPTH
