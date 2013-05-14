/** Exception definitions for ParamList and related types
 * @file except/paramlist.h
 * @author Krzysztof Findeisen
 * @date Created April 22, 2013
 * @date Last modified May 7, 2013
 */

#ifndef LCMCPARAMEXCEPTH
#define LCMCPARAMEXCEPTH

#include <stdexcept>
#include <string>
#include "exception.h"
#include "../paramlist.h"

namespace lcmc { namespace models { namespace except {

using std::string;

/** This exception is thrown if a query to a RangeList cannot find a 
 * particular parameter.
 *
 * The missing parameter is queryable in a machine-readable form.
 */
class MissingParam : public lcmc::except::CheckedException {
public:
	/** Constructs a MissingParam object.
	 */
	explicit MissingParam(const string& what_arg, const ParamType& param);
	virtual ~MissingParam() throw();
	
	/** Identifies the parameter that triggered the exception.
	 */
	const ParamType getParam() const;
private: 
	ParamType param;
};

/** This exception is thrown if one attempts to add a duplicate entry 
 * to a ParamList or RangeList.
 *
 * The duplicate parameter is queryable in a machine-readable form.
 */
class ExtraParam : public std::invalid_argument {
public:
	/** Constructs an ExtraParam object.
	 */
	explicit ExtraParam(const string& what_arg, const ParamType& param);
	virtual ~ExtraParam() throw();
	
	/** Identifies the parameter that triggered the exception.
	 */
	const ParamType getParam() const;
private: 
	ParamType param;
};

/** This exception is thrown if a function attempts to add a range whose 
 * maximum is less than its minimum to a RangeList.
 *
 * The minimum and maximum values are queryable in a machine-readable form.
 */
class NegativeRange : public lcmc::except::CheckedException {
public: 
	/** Constructs a NegativeRange object.
	 */
	explicit NegativeRange(const string& what_arg, double min, double max);
	
	/** Constructs a NegativeRange object.
	 */
	explicit NegativeRange(const string& what_arg, std::pair<double, double> range);
	
	/** Identifies the lower limit of the range that triggered the exception.
	 */
	double getMin() const;

	/** Identifies the upper limit of the range that triggered the exception.
	 */
	double getMax() const;
private: 
	double min;
	double max;
};

}}}		// end lcmc::models::except

#endif		// end ifndef LCMCPARAMEXCEPTH
