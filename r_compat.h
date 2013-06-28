/** Compatibility definitions to allow the program to interface 
 *	well with R
 * @file lightcurveMC/r_compat.h
 * @author Krzysztof Findeisen
 * @date Created June 24, 2013
 * @date Last modified June 24, 2013
 */

#ifndef LCMCRH
#define LCMCRH

#include <stdexcept>
#include <string>
#include <boost/smart_ptr.hpp>

#include "../common/warnflags.h"

// RInside uses C-style casts and non-virtual destructors
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// RInside uses C-style casts and non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <RInside.h>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif


namespace lcmc {

using boost::shared_ptr;

/** Returns a well-defined reference to an R instance. 
 */
shared_ptr<RInside> getRInstance();

namespace except {

/** This exception is thrown if an embedded R session could not be started
 */
class BadRSession: public std::runtime_error {
public:
	/** Constructs a BadRSession object.
	 */
	explicit BadRSession(const std::string& what_arg);
};

/** This exception represents errors within R code
 */
class RError: public std::runtime_error {
public:
	/** Constructs an RError object.
	 */
	explicit RError(const std::string& what_arg);
};

}		// end lcmc::except

}		// end lcmc

#endif		// end LCMCRH
