/** Functions converting C-style error codes to exceptions
 * @file cerror_except.cpp
 * @author Krzysztof Findeisen
 * @date Created May 26, 2013
 * @date Last modified May 27, 2013
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
#include <cerrno>
#include <cstring>
#include <gsl/gsl_errno.h>
//#include "cerror.h"

/** Wrapper that throws @c std::runtime_error in response to a GSL error
 *
 * @param[in] status A GSL error code.
 * @param[in] msg A string prepended to the exception's error message.
 *
 * @exception std::runtime_error Thrown if @p status &ne; 0
 * 
 * @exceptsafe The function arguments are unchanged in the event of an exception.
 */
void gslCheck(int status, std::string msg) {
	if (status != 0) {
		throw std::runtime_error(msg + gsl_strerror(status));
	}
}
