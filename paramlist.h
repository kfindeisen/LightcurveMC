/** Type definitions for ParamList and related types
 * @file lightcurveMC/paramlist.h
 * @author Krzysztof Findeisen
 * @date Created April 3, 2012
 * @date Last modified May 7, 2013
 * 
 * These types handle information needed to set up simulation runs.
 */

#ifndef LCMCPARAMLISTH
#define LCMCPARAMLISTH

#include "../common/warnflags.h"

// std::iterator<> uses non-virtual destructors
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// std::iterator<> uses non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <iterator>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

#include <map>
#include <string>
#include <utility>

namespace lcmc { namespace models {

/** Type representing which parameters a LightCurve needs, or the 
 * values of those parameters
 */
typedef std::string ParamType;

/** A ParamList contains the arguments needed by the light curve.
 * Providing the arguments in this form allows the client code to not 
 * care what kind of light curve it is generating parameters for.
 */
class ParamList {
public: 
	/** Initializes an empty parameter list
	 */
	ParamList();
	
	/** Initializes a parameter list with the same parameters and values as @p other
	 */
	ParamList(const ParamList& other);
	
	/** Cleans up the parameter list
	 */
	~ParamList();
	
	/** Sets a parameter list to a specific value
	 */
	ParamList& operator=(const ParamList& other);
	
	/** Adds a new parameter
	 */
	void add(ParamType name, double value);
	
	/** Returns the value of a specific parameter in the list
	 */
	double get(ParamType param) const;
private: 
	typedef std::map<ParamType, double> MapType;
	/** @todo replace lookup with a smart pointer
	 */
	MapType* const lookup;
};

/** A RangeList contains the minimum and maximum arguments to consider 
 * for the light curve. The minimum and maximum argument values are used to 
 * generate a suitable ParamList for use in simulation.
 */
class RangeList {
public:
	/** Represents the distribution from which a parameter is to be drawn.
	 */
	enum RangeType {
		/** Indicates a parameter is drawn uniformly from the interval 
		 * [minimum, maximum).
		 */
		UNIFORM, 
		/** Indicates a parameter is drawn log-uniformly from the 
		 * interval [minimum, maximum).
		 */
		LOGUNIFORM
	};

	/** Minimal iterator class for finding the parameters stored in a RangeList.
	 */
	class const_iterator;
	
	/** Initializes an empty list
	 */
	RangeList();
	
	/** Initializes a range list with the same parameters and ranges as @p other
	 */
	RangeList(const RangeList& other);
	
	/** Cleans up the parameter list
	 */
	~RangeList();
	
	/** Sets a range list to a specific value
	 */
	RangeList& operator=(const RangeList& other);
	
	/** Adds a new allowed range for a parameter
	 */
	void add(ParamType name, double min, double max, RangeType distrib);
	void add(ParamType name, std::pair<double,double> range, RangeType distrib);
	
	/** Removes all parameters from the list
	 */
	void clear();
	
	/** Returns the minimum value of a specific parameter in the list
	 */
	double getMin(ParamType param) const;

	/** Returns the maximum value of a specific parameter in the list
	 */
	double getMax(ParamType param) const;
	
	/** Returns the distribution of a specific parameter in the list
	 */
	RangeType getType(ParamType param) const;
	
	/** Returns an iterator to the first element in the RangeList
	 */
	const_iterator begin() const;

	/** Returns an iterator to after the last element in the RangeList
	 */
	const_iterator end() const;
	
private: 
	/** Tests whether x has an allowed value for a RangeType
	 */
	static bool checkRangeType(RangeType x);
	
	/** Stores all the information about the range for a particular 
	 * parameter.
	 */
	class RangeInfo;

	/** Shorthand used by RangeInfo and const_iterator to refer to the 
	 * internal data
	 */	
	typedef std::map<ParamType, RangeInfo> MapType;

	/** @todo replace lookup with a smart pointer
	 */
	MapType* const lookup;
};

/** Stores all the information about the range for a particular 
 * parameter.
 *
 * @invariant RangeInfo is immutable.
 */
class RangeList::RangeInfo {
public: 
	/** Initializes a range for a particular parameter.
	 */
	RangeInfo(double min, double max, RangeType distrib);
	/** Returns the smallest value the parameter can take.
	 */
	double getMin() const;
	/** Returns the largest value the parameter can take.
	 */
	double getMax() const;
	/** Returns the distribution from which the parameter can be drawn.
	 */
	RangeType getType() const;
private:
	double min;
	double max;
	RangeType distrib;
};

/** Iterator class for finding the parameters stored in a RangeList.
 *
 * The RangeList::const_iterator allows callers to access the entire RangeList 
 * while hiding the implementation of the class.
 *
 * Similar to a @c std::BidirectionalIterator, but does not model it 
 * because it is not default-constructible.
 *
 * @note The name const_iterator does not follow the conventions of the rest 
 * of this program. This is to accommodate libraries that assume the 
 * standard C++ conventions for an iterable container.
 */
class RangeList::const_iterator 
		: public std::iterator<std::bidirectional_iterator_tag, 
		RangeList::MapType::key_type, 
		RangeList::MapType::difference_type, 
		RangeList::MapType::const_pointer, 
		RangeList::MapType::const_reference> {
	// Private constructor ensures that only RangeLists can construct 
	// 	const_iterators from scratch
	friend class RangeList;
	
public: 
	/** Copy-constructor for const_iterator.
	 */
	const_iterator(const const_iterator& other);

	/** Assignment for const_iterator.
	 */
	const_iterator& operator=(const const_iterator& other);
	
	/** Implements <tt>std::BidirectionalIterator::==</tt>
	 */
	bool operator==(const const_iterator& other) const;
	/** Implements <tt>std::BidirectionalIterator::!=</tt>
	 */
	bool operator!=(const const_iterator& other) const;

	/** Implements <tt>std::BidirectionalIterator::*</tt>
	 */
	const ParamType& operator*() const;

	/** Implements <tt>std::BidirectionalIterator::-></tt>
	 */
	const ParamType* operator->() const;

	/** Implements <tt>std::BidirectionalIterator::++</tt> (prefix)
	 */
	const_iterator& operator++();
	/** Implements <tt>std::BidirectionalIterator::++</tt> (postfix)
	 */
	const_iterator operator++(int);
	/** Implements <tt>std::BidirectionalIterator::-\-</tt> (prefix)
	 */
	const_iterator& operator--();
	/** Implements <tt>std::BidirectionalIterator::-\-</tt> (postfix)
	 */
	const_iterator operator--(int);
private:
	/** Standard constructor for const_iterator.
	 */
	const_iterator(MapType::const_iterator where, const RangeList::MapType* parent);

	MapType::const_iterator begin() const;
	MapType::const_iterator end() const;
	
	MapType::const_iterator it;
	// Used to implement begin() and end() in the presence of changes to RangeList
	const RangeList::MapType* parent;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCPARAMLISTH
