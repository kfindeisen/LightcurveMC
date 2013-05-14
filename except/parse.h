/** Exception definitions for command-line parser
 * @file except/parse.h
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
 * @date Last modified May 11, 2013
 */

#ifndef LCMCPARSEEXCEPTH
#define LCMCPARSEEXCEPTH

#include <stdexcept>
#include <string>
#include "exception.h"

namespace lcmc { namespace parse { namespace except {

using std::string;

/** This exception is thrown for general parsing errors.
 */
class ParseError : public lcmc::except::CheckedException {
public:
	/** Constructs a ParseError object.
	 */
	explicit ParseError(const string& what_arg);
};

/** This exception is thrown if lightcurveMC is called with no valid light 
 *	curves to simulate.
 */
class NoLightCurves : public ParseError {
public:
	/** Constructs a NoLightCurves object.
	 */
	explicit NoLightCurves(const string& what_arg);
};

/** This exception is thrown if lightcurveMC is called with no valid 
 *	statistics to calculate.
 */
class NoStats : public ParseError {
public:
	/** Constructs a NoStats object.
	 */
	explicit NoStats(const string& what_arg);
};

}}}		// end lcmc::models::except

#endif		// end ifndef LCMCPARSEEXCEPTH
