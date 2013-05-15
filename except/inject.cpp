/** Exception definitions for injection tests
 * @file lightcurveMC/except/inject.cpp
 * @author Krzysztof Findeisen
 * @date Created May 8, 2013
 * @date Last modified May 8, 2013
 */

#include <stdexcept>
#include <string>
#include "exception.h"
#include "fileio.h"
#include "inject.h"

namespace lcmc { namespace inject { namespace except {

using std::string;

/** Constructs a BadFile object.
 *
 * @param[in] what_arg A string with the same content as the value 
 *	returned by what().
 * @param[in] fileName The name of the file that triggered the exception.
 *
 * @post this->what() == what_arg.c_str()
 * @post this->getFile() == fileName
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
 * @post this->what() == what_arg.c_str()
 * @post this->getLine() == line
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
 * @post this->what() == what_arg.c_str()
 * @post this->getFile() == fileName
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
