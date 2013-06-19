/** Exceptions related to Not-a-Number
 * @file lightcurveMC/except/nan.h
 * @author Krzysztof Findeisen
 * @date Created May 2, 2013
 * @date Last modified June 18, 2013
 */

#ifndef LCMCNANEXCEPTH
#define LCMCNANEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"

namespace lcmc { namespace utils { namespace except {

using std::string;

/** This exception is thrown if any calculation finds a NaN value where none 
 *	is allowed.
 */
class UnexpectedNan : public std::invalid_argument {
public:
	/** Constructs an UnexpectedNan object.
	 */
	explicit UnexpectedNan(const string& what_arg);
};

}}}		// end lcmc::utils::except

#endif		// end ifndef LCMCNANEXCEPTH
