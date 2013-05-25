/** Exception declarations for utils.tmp.h
 * @file utils_except.h
 * @author Krzysztof Findeisen
 * @date Created May 5, 2013
 * @date Last modified May 5, 2013
 */

#ifndef KPFEXCEPTH
#define KPFEXCEPTH

#include <stdexcept>
#include <string>

namespace kpfutils { namespace except {

using std::string;

/** This exception is thrown if a range does not contain enough data to 
 *	calculate the desired statistic
 */
class NotEnoughData : public std::invalid_argument {
public:
	/** Constructs a NotEnoughData object.
	 */
	explicit NotEnoughData(const string& what_arg);
};

/** This exception is thrown if a nonsense quantile is requested
 */
class InvalidQuantile : public std::domain_error {
public:
	/** Constructs an InvalidQuantile object.
	 */
	explicit InvalidQuantile(const string& what_arg);
};

}}		// end kpfutils::except

#endif		// end ifndef KPFEXCEPTH
