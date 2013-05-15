/** Exception definitions for iterator-related errors
 * @file lightcurveMC/except/iterator.h
 * @author Krzysztof Findeisen
 * @date Created May 2, 2013
 * @date Last modified May 9, 2013
 */

#ifndef LCMCITEREXCEPTH
#define LCMCITEREXCEPTH

#include <stdexcept>
#include <string>
#include "exception.h"

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
