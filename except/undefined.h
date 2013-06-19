/** Exception declarations for missing data errors
 * @file lightcurveMC/except/undefined.h
 * @author Krzysztof Findeisen
 * @date Created May 5, 2013
 * @date Last modified June 18, 2013
 */

#ifndef LCMCUNDEFEXCEPTH
#define LCMCUNDEFEXCEPTH

#include <stdexcept>
#include <string>
#include "../../common/checkedexception.h"

namespace lcmc { namespace stats { namespace except {

using std::string;

/** This exception is thrown if a statistic cannot be calculated from the 
 *	available data.
 */
class Undefined : public kpfutils::except::CheckedException {
public:
	/** Constructs an Undefined object.
	 */
	explicit Undefined(const string& what_arg);
};

/** This exception is thrown if only a lower limit is available on the value 
 *	of a statistic
 */
class LowerBound : public Undefined {
public:
	/** Constructs a LowerBound object.
	 */
	explicit LowerBound(const string& what_arg, double threshold);

	/** Returns the value of the lower limit.
	 */
	double getBound() const;
private:
	double limit;
};

/** This exception is thrown if only a upper limit is available on the value 
 *	of a statistic
 */
class UpperBound : public Undefined {
public:
	/** Constructs an UpperBound object.
	 */
	explicit UpperBound(const string& what_arg, double threshold);

	/** Returns the value of the upper limit.
	 */
	double getBound() const;
private:
	double limit;
};

/** This exception is thrown if a range does not contain enough data to 
 *	calculate the desired statistic
 */
class NotEnoughData : public Undefined {
public:
	/** Constructs a NotEnoughData object.
	 */
	explicit NotEnoughData(const string& what_arg);
};

}}}		// end lcmc::stats::except

#endif		// end ifndef LCMCUNDEFEXCEPTH
