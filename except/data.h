/** Exception declarations for invalid data
 * @file lightcurveMC/except/data.h
 * @author Krzysztof Findeisen
 * @date Created May 8, 2013
 * @date Last modified May 11, 2013
 */

#ifndef LCMCDATAEXCEPTH
#define LCMCDATAEXCEPTH

#include <stdexcept>
#include <string>
#include "exception.h"

namespace lcmc { 

using std::string;

namespace stats { namespace except {

/** This exception is thrown if a statistic cannot be calculated from the 
 *	available data.
 */
class NotSorted : public std::invalid_argument {
public:
	/** Constructs a NotSorted object.
	 */
	explicit NotSorted(const string& what_arg);
};

}}		// end lcmc::stats::except

namespace models { namespace except {

/** This exception is thrown if a model parameter is given a value 
 *	outside its allowed range.
 */
class BadParam : public lcmc::except::CheckedException {
public:
	/** Constructs a BadParam object.
	 */
	explicit BadParam(const string& what_arg);
};

}}		// end lcmc::models::except

}		// end lcmc

#endif		// end ifndef LCMCDATAEXCEPTH
