/** Definitions for non-template types needed by the parser
 * @file lightcurveMC/cmd/cmd_classes.cpp
 * @author Krzysztof Findeisen
 * @date Created April 27, 2013
 * @date Last modified May 11, 2013
 */

#include <istream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>
#include "cmd_constraints.tmp.h"

#include "../warnflags.h"

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
 *	str.exceptions() is set to throw exceptions.
 *
 * @todo Add support for reading a single number
 * 
 * @exceptsafe Function arguments remain in a valid state in the event of 
 *	an exception.
 */
// Must be declared in the global namespace to be identified by TCLAP
istream& operator>> (istream& str, pair<double, double>& val) {
	double min = 0, max = 0;
	str >> min >> max;
	
	// Update val only if the *entire* read from str was successful
	val.first  = min;
	val.second = max;
	
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
 *
 * @exception std::bad_alloc Thrown if there is not enough room to store 
 *	the allowed values.
 * @exception ios::failure Thrown if could not construct the documentation 
 *	for this constraint.
 *
 * @exceptsafe Object construction is atomic.
 */
KeywordConstraint::KeywordConstraint(vector<string>& allowed) 
		: TCLAP::ValuesConstraint<string>(allowed) {
}

/** Returns a short description used in the interface documentation
 *
 * @return string summarizing the data type
 *
 * @exceptsafe Does not throw exceptions.
 */
string KeywordConstraint::shortID() const {
	return "keyword";
}

KeywordConstraint::~KeywordConstraint() {
}

/** Returns a long description used when a value fails the constraint
 *
 * @return string specifying the exact requirements for the value
 *
 * @exceptsafe Does not throw exceptions.
 */
string UnitSubrange::description() const {
	return "both numbers in the range must be in [0, 1], and the second must be no smaller than the first";
}

/** Returns a short description used in the interface documentation
 *
 * @return string summarizing the data type
 *
 * @exceptsafe Does not throw exceptions.
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
 *
 * @exceptsafe Does not throw exceptions.
 */
bool UnitSubrange::check(const pair<double, double>& value) const {
	return (NonNegativeNumber<pair<double, double> >::check(value) && 
			value.first >= 0.0 && value.second <= 1.0 );
}

UnitSubrange::~UnitSubrange() {
}

}}	// end lcmc::parse
