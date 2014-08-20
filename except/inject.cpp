/** Exception definitions for injection tests
 * @file lightcurveMC/except/inject.cpp
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

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"
#include "../../common/fileio.h"
#include "inject.h"

namespace lcmc { namespace inject { namespace except {

using std::string;

/** Constructs a BadFile object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] fileName The name of the file that triggered the exception.
 *
 * @post this->what() = @p what_arg.c_str()
 * @post this->getFile() = @p fileName
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
BadFile::BadFile(const string& what_arg, const string& fileName) 
		: FileIo(what_arg), fileName(fileName) {
}

BadFile::~BadFile() throw() {
}

/** Returns the name of the unreadable file.
 *
 * @return A string equal to that passed to the constructor.
 *
 * @exceptsafe Does not throw exceptions.
 */
const string BadFile::getFile() const {
	return fileName;
}

/** Constructs a BadFormat object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] line The input line that could not be parsed.
 *
 * @post this->what() = @p what_arg.c_str()
 * @post this->getLine() = @p line
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
BadFormat::BadFormat(const string& what_arg, const string& line) 
		: FileIo(what_arg), unparseable(line) {
}

BadFormat::~BadFormat() throw() {
}

/** Returns the name of the unreadable file.
 *
 * @return A string equal to that passed to the constructor.
 *
 * @exceptsafe Does not throw exceptions.
 */
const string BadFormat::getLine() const {
	return unparseable;
}

/** Constructs a NoCatalog object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] fileName The name of the missing catalog file.
 *
 * @post this->what() = @p what_arg.c_str()
 * @post this->getFile() = @p fileName
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the exception.
 * 
 * @exceptsafe Object construction is atomic.
 */
NoCatalog::NoCatalog(const string& what_arg, const string& fileName) 
		: CheckedException(what_arg), fileName(fileName) {
}

NoCatalog::~NoCatalog() throw() {
}

/** Returns the name of the missing catalog.
 *
 * @return A string equal to that passed to the constructor.
 *
 * @exceptsafe Does not throw exceptions.
 */
const string NoCatalog::getFile() const {
	return fileName;
}

}}}		// end lcmc::inject::except
