/** Definitions for non-template types needed by the parser
 * @file cmd_classes.cpp
 * @author Krzysztof Findeisen
 * @date Created April 27, 2013
 * @date Last modified April 27, 2013
 */

#include <istream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>
#include "cmd_constraints.tmp.h"

#include "warnflags.h"

// TCLAP uses C-style casts
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

// TCLAP uses C-style casts
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <tclap/CmdLine.h>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

using namespace std;

/** Extracts formatted input into a pair of double precision floating point 
 * values.
 * 
 * The string is assumed to be formatted as two real literals, separated 
 * by whitespace.
 *
 * @param[in] str The stream to read from.
 * @param[out] val The pair in which to store the values.
 *
 * @return A reference to str.
 *
 * @exception std::ios_base::failure Thrown if the input cannot be read and 
 *	str.exceptions() is set to throw exceptions. If thrown, str remains in 
 *	a valid state.
 *
 * @todo Add support for reading a single number
 */
// Must be declared in the global namespace to be identified by TCLAP
istream& operator>> (istream& str, pair<double, double>& val) {
	str >> val.first >> val.second;
	return str;
}

namespace lcmc { namespace parse {

using namespace std;

/** Defines a constraint that only accepts specific values.
 *
 * @param[in] allowed The allowed values
 *
 * @post This constraint will accept a value if and only if it 
 *	compares equal to one of the elements of allowed.
 */
KeywordConstraint::KeywordConstraint(vector<string>& allowed) 
		: TCLAP::ValuesConstraint<string>(allowed) {
}

/** Returns a short description used in the interface documentation
 *
 * @return string summarizing the data type
 */
string KeywordConstraint::shortID() const {
	return "keyword";
}

KeywordConstraint::~KeywordConstraint() {
}

/** Returns a long description used when a value fails the constraint
 *
 * @return string specifying the exact requirements for the value
 */
string UnitSubrange::description() const {
	return "both numbers in the range must be in [0, 1], and the second must be no smaller than the first";
}

/** Returns a short description used in the interface documentation
 *
 * @return string summarizing the data type
 */
string UnitSubrange::shortID() const {
	return "unit subinterval";
}

/** Returns true iff the entire range is in [0, 1]
 *
 * @param[in] value The interval that must be a subset of the unit 
 *	interval.
 *
 * @return True if the interval is valid and both ends are between 
 *	0 and 1, inclusive; false otherwise
 */
bool UnitSubrange::check(const pair<double, double>& value) const {
	return (NonNegativeNumber<pair<double, double> >::check(value) && 
			value.first >= 0.0 && value.second <= 1.0 );
}

UnitSubrange::~UnitSubrange() {
}

}}	// end lcmc::parse
