/** Defines the ParamList and RangeList classes
 * @file paramlist.cpp
 * @author Krzysztof Findeisen
 * @date Created March 19, 2012
 * @date Last modified April 21, 2013
 */

#include <map>
#include <stdexcept>
#include <utility>
#include "paramlist.h"

namespace lcmc { namespace models {

/** Initializes an empty parameter list
 */
ParamList::ParamList() : lookup(new MapType()) {
}

/** Initializes a parameter list with the same parameters and values as other
 * 
 * @param[in] other The parameter list to copy
 */
ParamList::ParamList(const ParamList& other) : lookup(new MapType(*(other.lookup))) {
}

/** Cleans up the parameter list
 */
ParamList::~ParamList() {
	delete lookup;
}

/** Sets a parameter list to a specific value
 * 
 * @param[in] other The parameter list to copy
 *
 * @return An assignable lvalue
 */
ParamList& ParamList::operator=(const ParamList& other) {
	*lookup = *(other.lookup);
	return *this;
}

/** Adds a new parameter.
 * 
 * @param[in] name The parameter to add.
 * @param[in] value The value of the parameter.
 *
 * @pre name is not already represented in the ParamList
 * @pre value is not NaN
 *
 * @post get(name) returns value
 *
 * @exception std::invalid_argument Thrown if a value for the parameter is already in the list
 */
void ParamList::add(ParamType name, double value) {
	if (lookup->find(name) != lookup->end()) {
		/** @todo Re-implement exception string using a string stream
		 */
		throw std::invalid_argument("Duplicate parameter");
	}
	
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
 * @exception std::invalid_argument Thrown if the desired parameter is not in the 
 *	ParamList.
 */
double ParamList::get(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		/** @todo Re-implement exception string using a string stream
		 */
		throw std::invalid_argument("Required parameter not found");
	}
	
	return it->second;
}

//----------------------------------------------------------

/** Initializes an empty list
 */
RangeList::RangeList() : lookup(new MapType()) {
}

/** Initializes a range list with the same parameters and ranges as other
 * 
 * @param[in] other The range list to copy
 */
RangeList::RangeList(const RangeList& other) : lookup(new MapType(*(other.lookup))) {
}

/** Cleans up the parameter list
 */
RangeList::~RangeList() {
	delete lookup;
}

/** Sets a range list to a specific value
 * 
 * @param[in] other The range list to copy
 *
 * @return An assignable lvalue
 */
RangeList& RangeList::operator=(const RangeList& other) {
	*lookup = *(other.lookup);
	return *this;
}

/** Adds a new allowed range for a parameter
 * 
 * @param[in] name The parameter to add.
 * @param[in] min The minimum value of the parameter.
 * @param[in] max The minimum value of the parameter.
 * @param[in] distrib The distribution from which the parameter will be drawn.
 *
 * @pre name is not already represented in the RangeList
 * @pre min <= max
 * @pre neither min nor max is NaN
 *
 * @post getMin(name) returns min
 * @post getMax(name) returns max
 *
 * @exception std::invalid_argument Thrown if a range for the parameter is already in the list
 * @exception std::invalid_argument Thrown if max > min
 */
void RangeList::add(ParamType name, double min, double max, RangeType distrib) {
	if (lookup->find(name) != lookup->end()) {
		/** @todo Re-implement exception string using a string stream
		 */
		throw std::invalid_argument("Duplicate parameter.");
	}
	if (min > max) {
		throw std::invalid_argument("Minimum parameter value cannot exceed maximum value.");
	}
	
	lookup->insert(MapType::value_type(name, RangeInfo(min, max, distrib)));
}

/** Adds a new allowed range for a parameter
 * 
 * @param[in] name The parameter to add.
 * @param[in] range A pair containing the minimum and maximum values of the 
 *	parameter in the first and second fields, respectively.
 * @param[in] distrib The distribution from which the parameter will be drawn.
 *
 * @pre name is not already represented in the RangeList
 * @pre range.first <= range.second
 * @pre neither element of range is NaN
 *
 * @post getMin(name) returns range.first
 * @post getMax(name) returns range.second
 *
 * @exception std::invalid_argument Thrown if a range for the parameter is already in the list
 * @exception std::invalid_argument Thrown if range.second > range.first
 */
void RangeList::add(ParamType name, std::pair<double,double> range, RangeType distrib) {
	add(name, range.first, range.second, distrib);
}

/** Removes all parameters from the list
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
 * @exception std::invalid_argument Thrown if the desired parameter is not in the 
 *	RangeList.
 */
double RangeList::getMin(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		/** @todo Re-implement exception string using a string stream
		 */
		throw std::invalid_argument("Required parameter not found");
	}
	
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
 * @exception std::invalid_argument Thrown if the desired parameter is not in the 
 *	RangeList.
 */
double RangeList::getMax(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		/** @todo Re-implement exception string using a string stream
		 */
		throw std::invalid_argument("Required parameter not found");
	}
	
	return it->second.getMax();
}

/** Returns the distribution of a specific parameter in the list
 *
 * @param[in] param The parameter whose distribution is needed
 *
 * @return The distribution type of the desired parameter.
 *
 * @exception std::invalid_argument Thrown if the desired parameter is not in the 
 *	RangeList.
 */
RangeList::RangeType RangeList::getType(ParamType param) const {
	MapType::const_iterator it = lookup->find(param);
	if (it == lookup->end()) {
		/** @todo Re-implement exception string using a string stream
		 */
		throw std::invalid_argument("Required parameter not found");
	}
	
	return it->second.getType();
}

/** Returns an iterator to the first element in the RangeList
 */
RangeList::ConstIterator RangeList::begin() const {
	return ConstIterator(lookup->begin());
}

/** Returns an iterator to after the last element in the RangeList
 */
RangeList::ConstIterator RangeList::end() const {
	return ConstIterator(lookup->end());
}


//----------------------------------------------------------

/** Initializes a range for a particular parameter.
 *
 * @param[in] min The smallest value the parameter can take.
 * @param[in] max The largest value the parameter can take.
 * @param[in] distrib The distribution over [min, max] from which the 
 *	parameter can be drawn.
 *
 * @pre min <= max
 *
 * @pre neither min nor max is NaN
 */
RangeList::RangeInfo::RangeInfo(double min, double max, RangeType distrib) 
	: min(min), max(max), distrib(distrib) {
}

/** Returns the smallest value the parameter can take.
 *
 * @return The value.
 *
 * @post return value is not NaN
 */
double RangeList::RangeInfo::getMin() const {
	return min;
}

/** Returns the largest value the parameter can take.
 *
 * @return The value.
 *
 * @post return value is not NaN
 */
double RangeList::RangeInfo::getMax() const {
	return max;
}

/** Returns the distribution from which the parameter can be drawn.
 *
 * @return The value.
 */
RangeList::RangeType RangeList::RangeInfo::getType() const {
	return distrib;
}


//----------------------------------------------------------

/** Default constructor.
 *
 * @post The object is initialized to an invalid reference.
 */
RangeList::ConstIterator::ConstIterator() : it() {
}

/** Standard constructor for ConstIterator.
 *
 * @param[in] where An iterator over the RangeList's underlying 
 * implementation, indicating the parameter the ConstIterator must point to.
 *
 * @post ConstIterator is equivalent to where.
 */
RangeList::ConstIterator::ConstIterator(const RangeList::MapType::const_iterator& where) 
		: it(where) {
}

/** Implements std::BidirectionalIterator::==
 *
 * @param[in] other The ConstIterator to compare to this one.
 *
 * @return true if and only if the two iterators point to the same parameter
 */
bool RangeList::ConstIterator::operator==(const ConstIterator& other) const {
	return it==other.it;
}

/** Implements std::BidirectionalIterator::!=
 *
 * @param[in] other The ConstIterator to compare to this one.
 *
 * @return false if and only if the two iterators point to the same parameter
 */
bool RangeList::ConstIterator::operator!=(const ConstIterator& other) const {
	return it!=other.it;
}

/** Implements std::BidirectionalIterator::*
 *
 * This is the one method that does not simply reflect the underlying 
 * iterator. ConstIterator::-> returns a pointer only to the parameter stored 
 * in the RangeList, not to the attached RangeInfo object. This change 
 * improves encapsulation of the RangeList implementation.
 *
 * @return The ID of the parameter to which the iterator is pointing. The 
 * range information may be recovered using the RangeList::getMin(), 
 * RangeList::getMax(), and RangeList::getType() methods.
 */
const ParamType& RangeList::ConstIterator::operator*() const {
	return it->first;
}

/** Implements std::BidirectionalIterator::++ (prefix)
 *
 * @post The iterator is advanced by one space.
 *
 * @todo What is the behavior if this == RangeList::end()?
 *
 * @return The new position of the iterator.
 */
RangeList::ConstIterator& RangeList::ConstIterator::operator++() {
	++it;
	return *this;
}

/** Implements std::BidirectionalIterator::++ (postfix)
 *
 * @post The iterator is advanced by one space.
 *
 * @todo What is the behavior if this == RangeList::end()?
 *
 * @return The old position of the iterator.
 */
RangeList::ConstIterator RangeList::ConstIterator::operator++(int) {
	ConstIterator temp(*this);
	++it;
	return temp;
}

/** Implements std::BidirectionalIterator::-\- (prefix)
 *
 * @post The iterator is regressed by one space.
 *
 * @todo What is the behavior if this == RangeList::begin()?
 *
 * @return The new position of the iterator.
 */
RangeList::ConstIterator& RangeList::ConstIterator::operator--() {
	--it;
	return *this;
}

/** Implements std::BidirectionalIterator::-\- (postfix)
 *
 * @post The iterator is regressed by one space.
 *
 * @todo What is the behavior if this == RangeList::begin()?
 *
 * @return The old position of the iterator.
 */
RangeList::ConstIterator RangeList::ConstIterator::operator--(int) {
	ConstIterator temp(*this);
	--it;
	return temp;
}

}}	// end lcmc::models
