/** Exception declarations for file operations
 * @file lightcurveMC/except/fileio.h
 * @author Krzysztof Findeisen
 * @date Created May 8, 2013
 * @date Last modified May 8, 2013
 */

#ifndef LCMCFILEEXCEPTH
#define LCMCFILEEXCEPTH

#include <stdexcept>
#include <string>
#include "exception.h"

namespace lcmc { namespace except {

using std::string;

/** Exception thrown when a file could not be read or written to.
 */
class FileIo: public std::runtime_error {
public:
	/** Constructs a FileIo object.
	 */
	FileIo(const std::string& what_arg);
};

}}		// end lcmc::except

#endif		// end ifndef LCMCFILEEXCEPTH
