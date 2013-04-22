/** Type definitions for ParamList and related types
 * @file paramlist.h
 * @author Krzysztof Findeisen
 * @date Created April 3, 2012
 * @date Last modified April 21, 2013
 * 
 * These types handle information needed to set up simulation runs.
 */

#ifndef LCMCPARAMLISTH
#define LCMCPARAMLISTH

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
	
	/** Initializes a parameter list with the same parameters and values as other
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
	class ConstIterator;
	
	/** Initializes an empty list
	 */
	RangeList();
	
	/** Initializes a range list with the same parameters and ranges as other
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
	void add(ParamType name, double min, double max, RangeType distrib=UNIFORM);
	void add(ParamType name, std::pair<double,double> range, RangeType distrib=UNIFORM);
	
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
	ConstIterator begin() const;

	/** Returns an iterator to after the last element in the RangeList
	 */
	ConstIterator end() const;
	
private: 
	/** Stores all the information about the range for a particular 
	 * parameter.
	 */
	class RangeInfo;

	/** Shorthand used by RangeInfo and ConstIterator to refer to the 
	 * internal data
	 */	
	typedef std::map<ParamType, RangeInfo> MapType;

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

/** Minimal iterator class for finding the parameters stored in a RangeList.
 *
 * The ConstIterator allows callers to access the entire RangeList 
 * while hiding the implementation of the class.
 */
class RangeList::ConstIterator {
public: 
	/** Default constructor.
	 */
	ConstIterator();
	/** Standard constructor for ConstIterator.
	 */
	ConstIterator(const MapType::const_iterator& where);

	/** Implements std::BidirectionalIterator::==
	 */
	bool operator==(const ConstIterator& other) const;
	/** Implements std::BidirectionalIterator::!=
	 */
	bool operator!=(const ConstIterator& other) const;

	/** Implements std::BidirectionalIterator::*
	 */
	const ParamType& operator*() const;

	/** Implements std::BidirectionalIterator::++ (prefix)
	 */
	ConstIterator& operator++();
	/** Implements std::BidirectionalIterator::++ (postfix)
	 */
	ConstIterator operator++(int);
	/** Implements std::BidirectionalIterator::-\- (prefix)
	 */
	ConstIterator& operator--();
	/** Implements std::BidirectionalIterator::-\- (postfix)
	 */
	ConstIterator operator--(int);
private:
	MapType::const_iterator it;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCPARAMLISTH
