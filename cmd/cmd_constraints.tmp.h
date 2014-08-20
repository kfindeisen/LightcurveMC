/** Classes defining valid input
 * @file lightcurveMC/cmd/cmd_constraints.tmp.h
 * @author Krzysztof Findeisen
 * @date Created April 27, 2013
 * @date Last modified May 25, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LCMCCMDCONSTH
#define LCMCCMDCONSTH

#include <string>
#include <typeinfo>
#include <utility>
#include <vector>
#include <boost/concept_check.hpp>

#include "../../common/warnflags.h"

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

namespace lcmc { namespace parse {

using namespace std;

/** Constraint representing numbers that must be greater than zero
 *
 * @tparam Numeric A type comparable to zero.
 */
template<class Numeric>
class PositiveNumber : public TCLAP::Constraint<Numeric> {
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<Numeric>));	// Supports inequalities
	BOOST_CONCEPT_ASSERT((boost::Convertible<Numeric, double>));	// Numeric type
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return string("positive ") + typeid(Numeric).name();
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return string("positive ") + typeid(Numeric).name();
	}

	/** Returns true iff a value is positive.
	 *
	 * @param[in] value The number that must be greater than zero.
	 *
	 * @return True if @p value > 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const Numeric& value) const {
		return (value > 0);
	}

        virtual ~PositiveNumber() {}
};

/** Constraint representing integers that must be greater than zero
 *
 * @see PositiveNumber
 */
template<>
class PositiveNumber <long> : public TCLAP::Constraint<long> {
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return "positive integer";
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return "positive integer";
	}

	/** Returns true iff a value is positive.
	 *
	 * @param[in] value The number that must be greater than zero.
	 *
	 * @return True if @p value > 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const long& value) const {
		return (value > 0);
	}

        virtual ~PositiveNumber() {}
};

/** Constraint representing floating point numbers that must be greater than zero
 *
 * @see PositiveNumber
 */
template<>
class PositiveNumber <double> : public TCLAP::Constraint<double>{
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return "positive real number";
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return "positive real number";
	}

	/** Returns true iff a value is positive.
	 *
	 * @param[in] value The number that must be greater than zero.
	 *
	 * @return True if @p value > 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const double& value) const {
		return (value > 0.0);
	}

        virtual ~PositiveNumber() {}
};

/** Constraint representing floating point ranges where all values in the 
 * range must be greater than zero
 *
 * @see PositiveNumber
 */
template<>
class PositiveNumber <pair<double, double> > : public TCLAP::Constraint<pair<double, double> >{
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return "both numbers in the range must be positive, and the second must be no smaller than the first";
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return "positive interval";
	}

	/** Returns true iff the entire range is positive
	 *
	 * @param[in] value The interval that must be greater than zero.
	 *
	 * @return True if the interval is valid and both ends are > 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const pair<double, double>& value) const {
		return (value.first > 0.0 && value.second >= value.first);
	}

        virtual ~PositiveNumber() {}
};

/** Constraint representing numbers that must not be less than zero
 *
 * @tparam Numeric A type comparable to zero.
 */
template<class Numeric>
class NonNegativeNumber : public TCLAP::Constraint<Numeric> {
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<Numeric>));	// Supports inequalities
	BOOST_CONCEPT_ASSERT((boost::Convertible<Numeric, double>));	// Numeric type
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return string("non-negative ") + typeid(Numeric).name();
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return string("non-negative ") + typeid(Numeric).name();
	}

	/** Returns true iff a value is nonnegative.
	 *
	 * @param[in] value The number that must be greater than or equal 
	 *	to zero.
	 *
	 * @return True if @p value &ge; 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const Numeric& value) const {
		return (value >= 0);
	}

        virtual ~NonNegativeNumber() {}
};

/** Constraint representing integers that must not be less than zero
 *
 * @see NonNegativeNumber
 */
template<>
class NonNegativeNumber <long> : public TCLAP::Constraint<long> {
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return "non-negative integer";
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return "non-negative integer";
	}

	/** Returns true iff a value is nonnegative.
	 *
	 * @param[in] value The number that must be greater than or equal 
	 *	to zero.
	 *
	 * @return True if @p value &ge; 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const long& value) const {
		return (value >= 0);
	}

        virtual ~NonNegativeNumber() {}
};

/** Constraint representing floating point numbers that must not be less than zero
 *
 * @see NonNegativeNumber
 */
template<>
class NonNegativeNumber <double> : public TCLAP::Constraint<double> {
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return "non-negative real number";
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return "non-negative real number";
	}

	/** Returns true iff a value is nonnegative.
	 *
	 * @param[in] value The number that must be greater than or equal 
	 *	to zero.
	 *
	 * @return True if @p value &ge; 0, false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const double& value) const {
		return (value >= 0.0);
	}

        virtual ~NonNegativeNumber() {}
};

/** Constraint representing floating point ranges where all values in the 
 * range must not be less than zero
 *
 * @see NonNegativeNumber
 */
template<>
class NonNegativeNumber <pair<double, double> > 
		: public TCLAP::Constraint<pair<double, double> >{
public:
	/** Returns a long description used when a value fails the constraint
	 *
	 * @return string specifying the exact requirements for the value
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string description() const {
		return "both numbers in the range must be non-negative, and the second must be no smaller than the first";
	}

	/** Returns a short description used in the interface documentation
	 *
	 * @return string summarizing the data type
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual string shortID() const {
		return "non-negative interval";
	}

	/** Returns true iff the entire range is non-negative
	 *
	 * @param[in] value The interval that must be greater than or equal 
	 *	to zero.
	 *
	 * @return True if the interval is valid and both ends are &ge; 0, 
	 *	false otherwise
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual bool check(const pair<double, double>& value) const {
		return (value.first >= 0.0 && value.second >= value.first);
	}

        virtual ~NonNegativeNumber() {}
};

/** Constraint representing floating point ranges constrained to the interval [0, 1)
 */
class UnitSubrange : public NonNegativeNumber <pair<double, double> > {
public:
	/** Returns a long description used when a value fails the constraint
	 */
	virtual string description() const;

	/** Returns a short description used in the interface documentation
	 */
	virtual string shortID() const;

	/** Returns true iff the entire range is in [0, 1]
	 */
	virtual bool check(const pair<double, double>& value) const;

        virtual ~UnitSubrange();
};

/** Synonym for @c TCLAP::ValuesConstraint that takes up less space in the 
 * documentation. Especially well-suited for very long lists of allowed 
 * values.
 */
class KeywordConstraint : public TCLAP::ValuesConstraint<string> {
public:
	/** Defines a constraint that only accepts specific values.
	 */
	KeywordConstraint(vector<string>& allowed);	

	/** Returns a short description used in the interface documentation
	 */
	virtual string shortID() const;

        virtual ~KeywordConstraint();
};

}}		// end lcmc::parse

#endif		// LCMCCMDCONSTH
