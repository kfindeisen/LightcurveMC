/** Exception definitions for iterator-related errors
 * @file lightcurveMC/except/iterator.h
 * @author Krzysztof Findeisen
 * @date Created May 2, 2013
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

#ifndef LCMCITEREXCEPTH
#define LCMCITEREXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"

namespace lcmc { namespace utils { namespace except {

using std::string;

/** This exception is thrown if a null pointer is dereferenced.
 */
class NullPointer : public std::logic_error {
public:
	/** Constructs a NullPointer object.
	 */
	explicit NullPointer(const string& what_arg);
};

/** This exception is thrown if an iterator is shifted or dereferenced out 
 *	of bounds.
 */
class BadIterator : public std::out_of_range {
public:
	/** Constructs a BadIterator object.
	 */
	explicit BadIterator(const string& what_arg);
	virtual ~BadIterator() throw();
};

}}}		// end lcmc::utils::except

#endif		// end ifndef LCMCITEREXCEPTH
