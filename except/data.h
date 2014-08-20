/** Exception declarations for invalid data
 * @file lightcurveMC/except/data.h
 * @author Krzysztof Findeisen
 * @date Created May 8, 2013
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

#ifndef LCMCDATAEXCEPTH
#define LCMCDATAEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"

namespace lcmc { 

using std::string;

namespace stats { namespace except {

///** This exception is thrown if a function argument is not sorted.
// */
//class NotSorted : public std::invalid_argument {
//public:
//	/** Constructs a NotSorted object.
//	 */
//	explicit NotSorted(const string& what_arg);
//};

}}		// end lcmc::stats::except

namespace models { namespace except {

/** This exception is thrown if a model parameter is given a value 
 *	outside its allowed range.
 */
class BadParam : public kpfutils::except::CheckedException {
public:
	/** Constructs a BadParam object.
	 */
	explicit BadParam(const string& what_arg);
};

}}		// end lcmc::models::except

}		// end lcmc

#endif		// end ifndef LCMCDATAEXCEPTH
