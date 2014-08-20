/** Definitions for parsing ranges of values
 * @file lightcurveMC/cmd/cmd_ranges.tmp.h
 * @author Krzysztof Findeisen
 * @date Created April 27, 2013
 * @date Last modified May 11, 2013
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

#ifndef LCMCCMDRANGEH
#define LCMCCMDRANGEH

#include <string>
#include <utility>

#include "../../common/warnflags.h"

// TCLAP uses C-style casts
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

// TCLAP uses C-style casts
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <tclap/CmdLine.h>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

namespace TCLAP {
/** Defines the behavior of a pair of numbers so that TCLAP can parse range 
 * input
 */
template<class T, class U> 
struct ArgTraits<std::pair<T, U> > {
	/** Specifies the way that values gets assigned to this type during 
	 *	parsing. The two supported types are @c StringLike and 
	 *	@c ValueLike. 
	 */
	typedef ValueLike ValueCategory;
};

}		// end TCLAP

#endif 		// LCMCCMDRANGEH
