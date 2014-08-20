/** Exception definitions for command-line parser
 * @file lightcurveMC/except/parse.h
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
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

#ifndef LCMCPARSEEXCEPTH
#define LCMCPARSEEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"

namespace lcmc { namespace parse { namespace except {

using std::string;

/** This exception is thrown for general parsing errors.
 */
class ParseError : public kpfutils::except::CheckedException {
public:
	/** Constructs a ParseError object.
	 */
	explicit ParseError(const string& what_arg);
};

/** This exception is thrown if Lightcurve MC is called with no valid light 
 *	curves to simulate.
 */
class NoLightCurves : public ParseError {
public:
	/** Constructs a NoLightCurves object.
	 */
	explicit NoLightCurves(const string& what_arg);
};

/** This exception is thrown if Lightcurve MC is called with no valid 
 *	statistics to calculate.
 */
class NoStats : public ParseError {
public:
	/** Constructs a NoStats object.
	 */
	explicit NoStats(const string& what_arg);
};

}}}		// end lcmc::models::except

#endif		// end ifndef LCMCPARSEEXCEPTH
