/** Exception declarations for injection tests
 * @file lightcurveMC/except/inject.h
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

#ifndef LCMCINJECTEXCEPTH
#define LCMCINJECTEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"
#include "../../common/fileio.h"

namespace lcmc { namespace inject { namespace except {

using std::string;

/** Exception thrown when a light curve file could not be opened
 */
class BadFile: public kpfutils::except::FileIo {
public:
	/** Constructs a BadFile object.
	 */
	BadFile(const string& what_arg, const string& fileName);
	
	virtual ~BadFile() throw();
	
	/** Returns the name of the unreadable file.
	 */
	const string getFile() const;

private:
	string fileName;
};

/** Exception thrown when a light curve file is not formatted correctly
 */
class BadFormat: public kpfutils::except::FileIo {
public:
	/** Constructs a BadFormat object.
	 */
	BadFormat(const string& what_arg, const string& line);
	
	virtual ~BadFormat() throw();
	
	/** Returns the text that was mis-formatted.
	 */
	const string getLine() const;

private:
	string unparseable;
};

/** Exception thrown when a light curve catalog could not be found
 */
class NoCatalog: public kpfutils::except::CheckedException {
public:
	/** Constructs a NoCatalog object.
	 */
	NoCatalog(const string& what_arg, const string& fileName);
	
	virtual ~NoCatalog() throw();
	
	/** Returns the name of the missing catalog.
	 */
	const string getFile() const;

private:
	string fileName;
};

}}}		// end lcmc::inject::except

#endif		// end ifndef LCMCINJECTEXCEPTH
