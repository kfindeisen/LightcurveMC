/** Exception declarations for injection tests
 * @file lightcurveMC/except/inject.h
 * @author Krzysztof Findeisen
 * @date Created May 8, 2013
 * @date Last modified May 8, 2013
 */

#ifndef LCMCINJECTEXCEPTH
#define LCMCINJECTEXCEPTH

#include <stdexcept>
#include <string>
#include "fileio.h"

namespace lcmc { namespace inject { namespace except {

using std::string;

/** Exception thrown when a light curve file could not be opened
 */
class BadFile: public lcmc::except::FileIo {
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
class BadFormat: public lcmc::except::FileIo {
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
class NoCatalog: public lcmc::except::CheckedException {
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
