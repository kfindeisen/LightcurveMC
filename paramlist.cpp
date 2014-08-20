/** Defines the ParamList and RangeList classes
 * @file lightcurveMC/paramlist.cpp
 * @author Krzysztof Findeisen
 * @date Created March 19, 2012
 * @date Last modified May 24, 2013
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

#include <map>
#include <stdexcept>
#include <utility>
#include <cstdio>
#include <boost/concept/assert.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/lexical_cast.hpp>
#include "../common/nan.h"
#include "paramlist.h"
#include "except/iterator.h"
#include "except/nan.h"
#include "except/paramlist.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes an empty parameter list
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the list.
 * 
 * @exceptsafe Object construction is atomic.
 */
ParamList::ParamList() : lookup(new MapType()) {
}

/** Initializes a parameter list with the same parameters and values as @p other
 * 
 * @param[in] other The parameter list to copy
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the list.
 * 
 * @exceptsafe Object construction is atomic.
 */
ParamList::ParamList(const ParamList& other) : lookup(new MapType(*(other.lookup))) {
}

/** Cleans up the parameter list
 *
 * @exceptsafe Does not throw exceptions.
 */
ParamList::~ParamList() {
	delete lookup;
}

/** Sets a parameter list to a specific value
 * 
 * @param[in] other The parameter list to copy
 *
 * @return An assignable lvalue
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the list.
 * 
 * @exceptsafe Neither list is changed in the event of an exception.
 */
ParamList& ParamList::operator=(const ParamList& other) {
	// swap() lookup only works properly with unqualified names. See 
	//	Item 25, Effective C++ 3E
	using std::swap;
	
	// map::operator= can throw exceptions, but only offers the basic guarantee
	// Use copy-and-swap, so that exceptions (Map() can throw 
	//	them too) don't cascade to this->lookup
	MapType temp = *(other.lookup);
	
	// IMPORTANT: No exceptions thrown past this point

	swap(*(this->lookup), temp);
	
	return *this;
}

/** Adds a new parameter.
 * 
 * @param[in] name The parameter to add.
 * @param[in] value The value of the parameter.
 *
 * @pre @p name is not already represented in the ParamList
 * @pre @p value is not NaN
 *
 * @post @ref get() "get"(@p name) returns value
 *
 * @exception lcmc::utils::except::UnexpectedNan Thrown if @p value is NaN
 * @exception lcmc::models::except::ExtraParam Thrown if a value for the 
 *	parameter is already in the list
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	add an element to the list.
 * 
 * @exceptsafe Neither the ParamList nor the arguments to add() are changed 
 *	in the event of an exception.
 */
void ParamList::add(ParamType name, double value) {
	if (kpfutils::isNan(value)) {
		try {
			throw utils::except::UnexpectedNan("NaN value for parameter: " 
				+ lexical_cast<std::string>(name));
		} catch (const boost::bad_lexical_cast &e) {
			throw utils::except::UnexpectedNan("NaN value for parameter");
		}
	}
	// Check for duplicate elements directly, since duplicates cause 
	//	map::insert() to fail quietly
	// map::count() has an atomic guarantee... but doesn't specify what 
	//	exceptions it throws???
	if (lookup->count(name) > 0) {
		try {
			throw except::ExtraParam("Duplicate parameter to ParamList: " 
				+ lexical_cast<std::string>(name), 
				name);
		} catch (const boost::bad_lexical_cast &e) {
			throw except::ExtraParam("Duplicate parameter to ParamList.", name);
		}
	}
	
	// assert: lookup has not changed since the beginning of the 
	//	function call

	// map::insert() has an atomic guarantee
	lookup->insert(MapType::value_type(name, value));
}

/** Returns the value of a specific parameter in the list
 *
 * @param[in] param The parameter whose value is needed
 *
 * @return The value of the desired parameter.
 *
 * @post return value is not NaN
 *
 * @exception lcmc::models::except::MissingParam Thrown if the desired 
 *	parameter is not in the ParamList.
 * 
 * @exceptsafe The parameter list is unchanged in the event of an exception.
 */
double ParamList::get(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		try {
			throw except::MissingParam("Required parameter not found: " 
				+ lexical_cast<std::string>(param), 
				param);
		} catch (const boost::bad_lexical_cast &e) {
			throw except::MissingParam("Required parameter not found.", param);
		}
	}
	
	return it->second;
}

//----------------------------------------------------------

/** Initializes an empty list
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the list.
 *
 * @exceptsafe Object construction is atomic.
 */
RangeList::RangeList() : lookup(new MapType()) {
}

/** Initializes a range list with the same parameters and ranges as @p other
 * 
 * @param[in] other The range list to copy
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the list.
 * 
 * @exceptsafe Object construction is atomic.
 */
RangeList::RangeList(const RangeList& other) : lookup(new MapType(*(other.lookup))) {
}

/** Cleans up the parameter list
 * 
 * @exceptsafe Does not throw exceptions.
 */
RangeList::~RangeList() {
	delete lookup;
}

/** Sets a range list to a specific value
 * 
 * @param[in] other The range list to copy
 *
 * @return An assignable lvalue
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the list.
 * 
 * @exceptsafe Neither list is changed in the event of an exception.
 */
RangeList& RangeList::operator=(const RangeList& other) {
	// swap() lookup only works properly with unqualified names. See 
	//	Item 25, Effective C++ 3E
	using std::swap;

	// map::operator= can throw exceptions, but only offers the basic guarantee
	// Use copy-and-swap, so that exceptions (Map() can throw 
	//	them too) don't cascade to this->lookup
	MapType temp = *(other.lookup);
	
	// IMPORTANT: No exceptions thrown past this point

	swap(*(this->lookup), temp);
	
	return *this;
}

/** Adds a new allowed range for a parameter
 * 
 * @param[in] name The parameter to add.
 * @param[in] min The minimum value of the parameter.
 * @param[in] max The maximum value of the parameter.
 * @param[in] distrib The distribution from which the parameter will be drawn.
 *
 * @pre @p name is not already represented in the RangeList
 * @pre @p min &le; @p max
 * @pre neither @p min nor @p max is NaN
 *
 * @post @ref getMin() "getMin"(@p name) returns @p min
 * @post @ref getMax() "getMax"(@p name) returns @p max
 *  
 * @exception lcmc::utils::except::UnexpectedNan Thrown if either @p min or @p max 
 *	is NaN
 * @exception lcmc::models::except::ExtraParam Thrown if a value for the 
 *	parameter is already in the list
 * @exception lcmc::models::except::NegativeRange Thrown if @p max > @p min
 * @exception std::invalid_argument Thrown if @p distrib is not a valid value.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	add an element to the list.
 * 
 * @exceptsafe Neither the RangeList nor the arguments to add() are changed 
 *	in the event of an exception.
 */
void RangeList::add(ParamType name, double min, double max, RangeType distrib) {
	// Check for duplicate elements directly, since duplicates cause 
	//	map::insert() to fail quietly
	// map::count() has an atomic guarantee... but doesn't specify what 
	//	exceptions it throws???
	if (lookup->count(name) > 0) {
		try {
			throw except::ExtraParam("Duplicate parameter to RangeList: " 
				+ lexical_cast<std::string>(name), 
				name);
		} catch (const boost::bad_lexical_cast &e) {
			throw except::ExtraParam("Duplicate parameter to RnageList.", name);
		}
	}
	
	// assert: lookup has not changed since the beginning of the 
	//	function call

	// RangeInfo() has an atomic guarantee
	// value_type() does not throw
	// map::insert() has an atomic guarantee
	lookup->insert(MapType::value_type(name, RangeInfo(min, max, distrib)));
}

/** Adds a new allowed range for a parameter
 * 
 * @param[in] name The parameter to add.
 * @param[in] range A pair containing the minimum and maximum values of the 
 *	parameter in the first and second fields, respectively.
 * @param[in] distrib The distribution from which the parameter will be drawn.
 *
 * @pre @p name is not already represented in the RangeList
 * @pre @p range.first &le; @p range.second
 * @pre neither element of @p range is NaN
 *
 * @post @ref getMin() "getMin"(@p name) returns @p range.first
 * @post @ref getMax() "getMax"(@p name) returns @p range.second
 *
 * @exception lcmc::utils::except::UnexpectedNan Thrown if either @p min or @p max 
 *	is NaN
 * @exception lcmc::models::except::ExtraParam Thrown if a value for the 
 *	parameter is already in the list
 * @exception lcmc::models::except::NegativeRange Thrown if @p max > @p min
 * @exception std::invalid_argument Thrown if @p distrib is not a valid value.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	add an element to the list.
 * 
 * @exceptsafe Neither the RangeList nor the arguments to add() are changed 
 *	in the event of an exception.
 */
void RangeList::add(ParamType name, std::pair<double,double> range, RangeType distrib) {
	add(name, range.first, range.second, distrib);
}

/** Removes all parameters from the list
 *
 * @exceptsafe Does not throw exceptions.
 */
void RangeList::clear() {
	lookup->clear();
}
	
/** Returns the minimum value of a specific parameter in the list
 *
 * @param[in] param The parameter whose minimum value is needed
 *
 * @return The minimum value of the desired parameter.
 *
 * @post return value is not NaN
 *
 * @exception lcmc::models::except::MissingParam Thrown if the desired 
 *	parameter is not in the RangeList.
 * 
 * @exceptsafe The range list is unchanged in the event of an exception.
 */
double RangeList::getMin(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		try {
			throw except::MissingParam("Required parameter not found: " 
				+ lexical_cast<std::string>(param), 
				param);
		} catch (const boost::bad_lexical_cast &e) {
			throw except::MissingParam("Required parameter not found.", param);
		}
	}
	
	// getMin() does not throw
	return it->second.getMin();
}

/** Returns the maximum value of a specific parameter in the list
 *
 * @param[in] param The parameter whose maximum value is needed
 *
 * @return The maximum value of the desired parameter.
 *
 * @post return value is not NaN
 *
 * @exception lcmc::models::except::MissingParam Thrown if the desired 
 *	parameter is not in the RangeList.
 * 
 * @exceptsafe The range list is unchanged in the event of an exception.
 */
double RangeList::getMax(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		try {
			throw except::MissingParam("Required parameter not found: " 
				+ lexical_cast<std::string>(param), 
				param);
		} catch (const boost::bad_lexical_cast &e) {
			throw except::MissingParam("Required parameter not found.", param);
		}
	}
	
	// getMax() does not throw
	return it->second.getMax();
}

/** Returns the distribution of a specific parameter in the list
 *
 * @param[in] param The parameter whose distribution is needed
 *
 * @return The distribution type of the desired parameter.
 *
 * @exception lcmc::models::except::MissingParam Thrown if the desired 
 *	parameter is not in the RangeList.
 * 
 * @exceptsafe The range list is unchanged in the event of an exception.
 */
RangeList::RangeType RangeList::getType(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		try {
			throw except::MissingParam("Required parameter not found: " 
				+ lexical_cast<std::string>(param), 
				param);
		} catch (const boost::bad_lexical_cast &e) {
			throw except::MissingParam("Required parameter not found.", param);
		}
	}
	
	// getType() does not throw
	return it->second.getType();
}

/** Returns an iterator to the first element in the RangeList
 *
 * @return a constant bidirectional iterator pointing to the start of 
 *	the list
 *
 * @exceptsafe Does not throw exceptions.
 */
RangeList::const_iterator RangeList::begin() const {
	return const_iterator(lookup->begin(), lookup);
}

/** Returns an iterator to after the last element in the RangeList
 *
 * @return a constant bidirectional iterator pointing to just past the 
 *	end of the list
 * 
 * @exceptsafe Does not throw exceptions.
 */
RangeList::const_iterator RangeList::end() const {
	return const_iterator(lookup->end(), lookup);
}

/** Tests whether x has an allowed value for a RangeType
 *
 * @param[in] x The value to test
 *
 * @return True iff @p x is a valid enum, false otherwise.
 *
 * @exceptsafe Does not throw exceptions.
 */
bool RangeList::checkRangeType(RangeList::RangeType x) {
	switch(x) {
	case UNIFORM:
	case LOGUNIFORM: 
		return true;
	default:
		return false;
	}
}


//----------------------------------------------------------

/** Initializes a range for a particular parameter.
 *
 * @param[in] min The smallest value the parameter can take.
 * @param[in] max The largest value the parameter can take.
 * @param[in] distrib The distribution over [@p min, @p max] from which the 
 *	parameter can be drawn.
 *
 * @pre @p min &le; @p max
 *
 * @pre neither @p min nor @p max is NaN
 *
 * @exception lcmc::utils::except::UnexpectedNan Thrown if either @p min or @p max 
 *	is NaN
 * @exception lcmc::models::except::NegativeRange Thrown if @p max > @p min
 * @exception std::invalid_argument Thrown if @p distrib is not a valid value.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	add an element to the list.
 * 
 * @exceptsafe Object creation is atomic.
 */
RangeList::RangeInfo::RangeInfo(double min, double max, RangeType distrib) 
	: min(min), max(max), distrib(distrib) {
	if (kpfutils::isNan(min) || kpfutils::isNan(max)) {
		throw utils::except::UnexpectedNan("NaN value for range limits.");
	}
	if (min > max) {
		throw except::NegativeRange("Minimum parameter value cannot exceed maximum value.", min, max);
	}
	if (!checkRangeType(distrib)) {
		throw std::invalid_argument("Nonexistent RangeType.");
	}
}

/** Returns the smallest value the parameter can take.
 *
 * @return The value.
 *
 * @post return value is not NaN
 *
 * @exceptsafe Does not throw exceptions.
 */
double RangeList::RangeInfo::getMin() const {
	return min;
}

/** Returns the largest value the parameter can take.
 *
 * @return The value.
 *
 * @post return value is not NaN
 *
 * @exceptsafe Does not throw exceptions.
 */
double RangeList::RangeInfo::getMax() const {
	return max;
}

/** Returns the distribution from which the parameter can be drawn.
 *
 * @return The value.
 *
 * @exceptsafe Does not throw exceptions.
 */
RangeList::RangeType RangeList::RangeInfo::getType() const {
	return distrib;
}


//----------------------------------------------------------

/** Standard constructor for const_iterator.
 *
 * @param[in] where An iterator over the RangeList's underlying 
 *	implementation, indicating the parameter the const_iterator 
 *	must point to.
 * @param[in] parent A reference to the RangeList the 
 *	const_iterator points to.
 *
 * @pre where points either to an element of (@p *parent)'s underlying 
 *	implementation, or to just after the end of that implementation
 *
 * @post const_iterator is equivalent to @p where.
 *
 * @exceptsafe Does not throw exceptions.
 *
 * @note const_iterator() can only be called from within RangeList or 
 *	RangeList::const_iterator. These classes are responsible for 
 *	ensuring the constructor's preconditions are satisfied.
 */
RangeList::const_iterator::const_iterator(MapType::const_iterator where, 
		const RangeList::MapType* parent) 
		: it(where), parent(parent) {
}

/** Copy-constructor for const_iterator.
 *
 * @param[in] other The iterator to copy.
 *
 * @post @p *this = @p other
 *
 * @exceptsafe Does not throw exceptions.
 */
RangeList::const_iterator::const_iterator(const RangeList::const_iterator& other) 
		: it(other.it), parent(other.parent) {
}

/** Assignment for const_iterator.
 *
 * @param[in] other The iterator to copy.
 *
 * @return An assignable lvalue
 *
 * @post @p *this = @p other
 *
 * @exceptsafe Does not throw exceptions.
 */
RangeList::const_iterator& RangeList::const_iterator::operator=(
		const RangeList::const_iterator& other) {
	it     = other.it;
	parent = other.parent;
	
	return *this;
}

/** Implements <tt>std::BidirectionalIterator::==</tt>
 *
 * @param[in] other The const_iterator to compare to this one.
 *
 * @return true if and only if the two iterators point to the same parameter
 *
 * @exceptsafe Does not throw exceptions.
 */
bool RangeList::const_iterator::operator==(const const_iterator& other) const {
	return (parent == (other.parent) && it == other.it);
}

/** Implements <tt>std::BidirectionalIterator::!=</tt>
 *
 * @param[in] other The const_iterator to compare to this one.
 *
 * @return false if and only if the two iterators point to the same parameter
 *
 * @exceptsafe Does not throw exceptions.
 */
bool RangeList::const_iterator::operator!=(const const_iterator& other) const {
	return (parent != (other.parent) || it!=other.it);
}

/** Implements <tt>std::BidirectionalIterator::*</tt>
 *
 * This is the one method that does not simply reflect the underlying 
 * iterator. const_iterator::operator* returns a 
 * pointer only to the parameter stored in the RangeList, not to the attached 
 * RangeInfo object. This change improves encapsulation of the RangeList 
 * implementation.
 *
 * @return The ID of the parameter to which the iterator is pointing. The 
 * range information may be recovered using the RangeList::getMin(), 
 * RangeList::getMax(), and RangeList::getType() methods.
 *
 * @exception lcmc::utils::except::BadIterator Thrown if the iterator does 
 *	not point to a valid element of a RangeList.
 *
 * @exceptsafe Neither the iterator nor any RangeList it points to are 
 *	changed in the event of an exception.
 */
const ParamType& RangeList::const_iterator::operator*() const {
	// Class invariant guarantees that this is the only way to 
	//	not point to a valid element
	if (it == this->end()) {
		throw utils::except::BadIterator("Cannot dereference RangeList::end().");
	}
	
	return it->first;
}

/** Implements <tt>std::BidirectionalIterator::-></tt>
 *
 * This is the one method that does not simply reflect the underlying 
 * iterator. const_iterator::operator-> returns a 
 * pointer only to the parameter stored in the RangeList, not to the attached 
 * RangeInfo object. This change improves encapsulation of the RangeList 
 * implementation.
 *
 * @return The ID of the parameter to which the iterator is pointing. The 
 * range information may be recovered using the RangeList::getMin(), 
 * RangeList::getMax(), and RangeList::getType() methods.
 *
 * @exception lcmc::utils::except::BadIterator Thrown if the iterator does 
 *	not point to a valid element of a RangeList.
 *
 * @exceptsafe Neither the iterator nor any RangeList it points to are 
 *	changed in the event of an exception.
 */
const ParamType* RangeList::const_iterator::operator->() const {
	// Class invariant guarantees that this is the only way to 
	//	not point to a valid element
	if (it == this->end()) {
		throw utils::except::BadIterator("Cannot dereference RangeList::end().");
	}
	
	return &(it->first);
}

/** Implements <tt>std::BidirectionalIterator::++</tt> (prefix)
 *
 * @return The new position of the iterator.
 *
 * @post The iterator is advanced by one space.
 *
 * @exception lcmc::utils::except::BadIterator Thrown if the iterator 
 *	points past the last element of a RangeList.
 *
 * @exceptsafe Neither the iterator nor the RangeList it points to are 
 *	changed in the event of an exception.
 */
RangeList::const_iterator& RangeList::const_iterator::operator++() {
	if (it == this->end()) {
		throw utils::except::BadIterator("Cannot increment past RangeList::end().");
	}
	
	++it;
	return *this;
}

/** Implements <tt>std::BidirectionalIterator::++</tt> (postfix)
 *
 * @return The old position of the iterator.
 *
 * @post The iterator is advanced by one space.
 *
 * @exception lcmc::utils::except::BadIterator Thrown if the iterator 
 *	points past the last element of a RangeList.
 *
 * @exceptsafe Neither the iterator nor the RangeList it points to are 
 *	changed in the event of an exception.
 */
RangeList::const_iterator RangeList::const_iterator::operator++(int) {
	if (it == this->end()) {
		throw utils::except::BadIterator("Cannot increment past RangeList::end().");
	}
	
	const_iterator temp(*this);
	++it;
	return temp;
}

/** Implements <tt>std::BidirectionalIterator::-\-</tt> (prefix)
 *
 * @return The new position of the iterator.
 *
 * @post The iterator is regressed by one space.
 *
 * @exception lcmc::utils::except::BadIterator Thrown if the iterator 
 *	points to the first element of a RangeList.
 *
 * @exceptsafe Neither the iterator nor the RangeList it points to are 
 *	changed in the event of an exception.
 */
RangeList::const_iterator& RangeList::const_iterator::operator--() {
	if (it == this->begin()) {
		throw utils::except::BadIterator("Cannot decrement past RangeList::begin().");
	}
	
	--it;
	return *this;
}

/** Implements <tt>std::BidirectionalIterator::-\-</tt> (postfix)
 *
 * @return The old position of the iterator.
 *
 * @post The iterator is regressed by one space.
 *
 * @exception lcmc::utils::except::BadIterator Thrown if the iterator 
 *	points to the first element of a RangeList.
 *
 * @exceptsafe Neither the iterator nor the RangeList it points to are 
 *	changed in the event of an exception.
 */
RangeList::const_iterator RangeList::const_iterator::operator--(int) {
	if (it == this->begin()) {
		throw utils::except::BadIterator("Cannot decrement past RangeList::begin().");
	}
	
	const_iterator temp(*this);
	--it;
	return temp;
}

/** Returns the earliest allowed value of the iterator.
 *
 * @return The first element of the RangeList.
 *
 * @exceptsafe Does not throw exceptions.
 */
RangeList::MapType::const_iterator RangeList::const_iterator::begin() const {
	return parent->begin();
}

/** Returns the last allowed value of the iterator.
 *
 * @return The position just after the last element of the RangeList.
 *
 * @exceptsafe Does not throw exceptions.
 */
RangeList::MapType::const_iterator RangeList::const_iterator::end() const {
	return parent->end();
}

using boost_concepts::      ReadableIteratorConcept;
using boost_concepts::    SinglePassIteratorConcept;
using boost_concepts::BidirectionalTraversalConcept;

BOOST_CONCEPT_ASSERT((      ReadableIteratorConcept<RangeList::const_iterator>));
// Not default-constructible
BOOST_CONCEPT_ASSERT((    SinglePassIteratorConcept<RangeList::const_iterator>));
//BOOST_CONCEPT_ASSERT((BidirectionalTraversalConcept<RangeList::const_iterator>));

}}	// end lcmc::models
