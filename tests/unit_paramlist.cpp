/** Test unit for ParamList and RangeList
 * @file lightcurveMC/tests/unit_paramlist.cpp
 * @author Krzysztof Findeisen
 * @date Created May 3, 2013
 * @date Last modified May 4, 2013
 */

#include "../../common/warnflags.h"

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <boost/test/unit_test.hpp>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

#include <stdexcept>
#include <string>
#include "test.h"
#include "../except/iterator.h"
#include "../except/nan.h"
#include "../except/paramlist.h"
#include "../paramlist.h"

using lcmc::models::ParamList;
using lcmc::models::RangeList;

using namespace lcmc::models::except;
using namespace lcmc::utils::except;

namespace lcmc { namespace test {

/** Common data for @ref lcmc::models::ParamList "ParamList" tests
 */
class ParamData {
public: 
	/** Constructs parameter lists for each test case.
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory 
	 *	for the parameter lists.
	 * 
	 * @exceptsafe Object construction is atomic.
	 */
	ParamData() : emptyParamList(), dummyParamList(), 
			emptyRangeList(), dummyRangeList() {
		dummyParamList.add("d",   1.0);
		dummyParamList.add("b", -42.0);
		dummyParamList.add("c",   0.0);
		dummyParamList.add("a",  1e10);

		dummyRangeList.add("d",  0.0,  1.0, RangeList::UNIFORM);
		dummyRangeList.add("a",  3.5,  4.2, RangeList::UNIFORM);
		dummyRangeList.add("c", -8.0, -7.3, RangeList::LOGUNIFORM);
	}

	/** Empty @ref lcmc::models::ParamList "ParamList" object.
	 */	
	ParamList emptyParamList;
	/** Non-empty @ref lcmc::models::ParamList "ParamList" object.
	 */	
	ParamList dummyParamList;
	/** Empty @ref lcmc::models::RangeList "RangeList" object.
	 */	
	RangeList emptyRangeList;
	/** Non-empty @ref lcmc::models::RangeList "RangeList" object.
	 */	
	RangeList dummyRangeList;
};

/** Verifies that a list contains all the elements of @ref ParamData::dummyParamList "dummyParamList". Does not 
 *	rule out that the list contains more elements.
 *
 * @param[in] x The list to compare to @ref ParamData::dummyParamList "dummyParamList"
 *
 * @post Runs BOOST_CHECK repeatedly to test the list contents.
 *
 * @exceptsafe Does not throw exceptions.
 */
void matchDummyPos(const ParamList& x) {
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.get("a"),  1e10));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.get("b"), -42.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.get("c"),   0.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.get("d"),   1.0));
}

/** Verifies that a list is identical to the @ref ParamData::dummyParamList "dummyParamList"
 *
 * @param[in] x The list to compare to @ref ParamData::dummyParamList "dummyParamList"
 *
 * @post Runs BOOST_CHECK repeatedly to test the list contents.
 *
 * @exceptsafe Does not throw exceptions.
 */
void matchDummy(const ParamList& x) {
	matchDummyPos(x);
	BOOST_CHECK_THROW(x.get("e"), MissingParam);
	BOOST_CHECK_THROW(x.get("f"), MissingParam);
	BOOST_CHECK_THROW(x.get("g"), MissingParam);
}

/** Verifies that a list is identical to the @ref ParamData::dummyRangeList "dummyRangeList"
 *
 * @param[in] x The list to compare to @ref ParamData::dummyRangeList "dummyRangeList"
 *
 * @post Runs BOOST_CHECK repeatedly to test the list contents.
 *
 * @exceptsafe Does not throw exceptions.
 */
void matchDummy(const RangeList& x) {
	RangeList::const_iterator it = x.begin();
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(*it, "a"));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMin(*it),  3.5));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMax(*it),  4.2));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(x.getType(*it) == RangeList::UNIFORM));

	BOOST_CHECK_NO_THROW(it++);
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(*it, "c"));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMin(*it), -8.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMax(*it), -7.3));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(x.getType(*it) == RangeList::LOGUNIFORM));

	BOOST_CHECK_NO_THROW(it++);
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(*it, "d"));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMin(*it),  0.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMax(*it),  1.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(x.getType(*it) == RangeList::UNIFORM));
	
	BOOST_CHECK_NO_THROW(it++);
	BOOST_CHECK_NO_THROW(BOOST_CHECK(it == x.end()));
}

/** Verifies that a list is identical to the @ref ParamData::dummyRangeList "dummyRangeList".  Does not 
 *	rule out that the list contains more elements.
 *
 * @param[in] x The list to compare to @ref ParamData::dummyRangeList "dummyRangeList"
 *
 * @post Runs BOOST_CHECK repeatedly to test the list contents.
 *
 * @exceptsafe Does not throw exceptions.
 */
void matchDummyPos(const RangeList& x) {
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMin("a"),  3.5));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMax("a"),  4.2));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(x.getType("a") == RangeList::UNIFORM));

	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMin("c"), -8.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMax("c"), -7.3));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(x.getType("c") == RangeList::LOGUNIFORM));

	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMin("d"),  0.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(x.getMax("d"),  1.0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(x.getType("d") == RangeList::UNIFORM));
}

/** Test cases for the @ref lcmc::models::ParamList "ParamList" class
 * @class BoostTest::test_paramlist
 */
BOOST_FIXTURE_TEST_SUITE(test_paramlist, ParamData)

/** Tests @ref lcmc::models::ParamList "ParamList" construction and copying
 *
 * @see @ref lcmc::models::ParamList "ParamList"
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(make) {
	// Before we use it, verify that matchDummy() works!
	BOOST_REQUIRE_NO_THROW(matchDummy(dummyParamList));

	//is default-constructed list empty?
	// how to test?

	//is a copy (constructed or assigned) identical?
	try {
		ParamList copyList1(dummyParamList);
		BOOST_CHECK_NO_THROW(matchDummy(copyList1));
		
		ParamList copyList2;
		BOOST_CHECK_NO_THROW(copyList2.add("b", 27.340));
		BOOST_CHECK_NO_THROW(copyList2.add("e",  8.22 ));
		BOOST_CHECK_NO_THROW(copyList2 = dummyParamList);
		BOOST_CHECK_NO_THROW(matchDummy(copyList2));
		
		//is a copy (constructed or assigned) independent?
		BOOST_CHECK_NO_THROW(copyList1.add("e", 3.14159));
		BOOST_CHECK_NO_THROW(dummyParamList.add("f", 3.14159));
		BOOST_CHECK_NO_THROW(copyList2.add("g", 2.71828));
		BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(copyList1.get("e"), 3.14159));
		BOOST_CHECK_THROW(copyList1.get("f"), MissingParam);
		BOOST_CHECK_THROW(copyList1.get("g"), MissingParam);
		
		BOOST_CHECK_THROW(dummyParamList.get("e"), MissingParam);
		BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyParamList.get("f"), 3.14159));
		BOOST_CHECK_THROW(dummyParamList.get("g"), MissingParam);
		
		BOOST_CHECK_THROW(copyList2.get("e"), MissingParam);
		BOOST_CHECK_THROW(copyList2.get("f"), MissingParam);
		BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(copyList2.get("g"), 2.71828));
	} catch (const std::bad_alloc& e) {
		BOOST_FAIL("Out of memory.");
	}
}

/** Tests @ref lcmc::models::ParamList "ParamList" modification
 *
 * @see @ref lcmc::models::ParamList "ParamList"
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(add) {
	// Verify that dummyParamList has been reset
	BOOST_REQUIRE_NO_THROW(matchDummy(dummyParamList));
	
	// Adding a parameter works whether or not the list was 
	//	previously empty
	BOOST_CHECK_NO_THROW(emptyParamList.add("f", 1.1489));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(emptyParamList.get("f"), 1.1489));
	BOOST_CHECK_NO_THROW(dummyParamList.add("f", 1.1489));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyParamList.get("f"), 1.1489));

	//adding a Nan returns an error
	BOOST_CHECK_THROW(dummyParamList.add("g", std::numeric_limits<double>::quiet_NaN()), 
			UnexpectedNan);
	// atomic?
	BOOST_CHECK_NO_THROW(matchDummyPos(dummyParamList));
	BOOST_CHECK_EQUAL(dummyParamList.get("f"), 1.1489);
	BOOST_CHECK_THROW(dummyParamList.get("g"), MissingParam);
	
	//adding a duplicate parameter returns an error
	BOOST_CHECK_THROW(dummyParamList.add("b", 1e-3), ExtraParam);
	// atomic?
	BOOST_CHECK_NO_THROW(matchDummyPos(dummyParamList));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyParamList.get("f"), 1.1489));
	BOOST_CHECK_THROW(dummyParamList.get("g"), MissingParam);
}


BOOST_AUTO_TEST_SUITE_END()

/** Test cases for the @ref lcmc::models::RangeList "RangeList" class
 * @class BoostTest::test_rangelist
 */
BOOST_FIXTURE_TEST_SUITE(test_rangelist, ParamData)

/** Tests @ref lcmc::models::RangeList "RangeList" construction
 *
 * @see @ref lcmc::models::RangeList "RangeList"
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(make) {
	// Before we use it, verify that matchDummy() works!
	BOOST_REQUIRE_NO_THROW(matchDummy(dummyRangeList));

	//is default-constructed list empty?
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(std::distance(emptyRangeList.begin(), 
			emptyRangeList.end()), 0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(emptyRangeList.begin() == emptyRangeList.end()));

	//is a built-up list non-empty?
	BOOST_CHECK_NO_THROW(BOOST_CHECK(std::distance(dummyRangeList.begin(), 
			dummyRangeList.end()) > 0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(dummyRangeList.begin() != dummyRangeList.end()));
	
	try {
		//is a copy (constructed or assigned) identical?
		RangeList copyList1(dummyRangeList);
		BOOST_CHECK_NO_THROW(matchDummy(copyList1));
		
		RangeList copyList2;
		BOOST_CHECK_NO_THROW(copyList2.add("b", 27.340, 30.42, RangeList::UNIFORM));
		BOOST_CHECK_NO_THROW(copyList2.add("e",  8.22,  10.0 , RangeList::UNIFORM));
		BOOST_CHECK_NO_THROW(copyList2 = dummyRangeList);
		BOOST_CHECK_NO_THROW(matchDummy(copyList2));
		
		//is a copy (constructed or assigned) independent?
		BOOST_CHECK_NO_THROW(BOOST_CHECK(dummyRangeList.begin() != copyList1.begin()));
		BOOST_CHECK_NO_THROW(BOOST_CHECK(dummyRangeList.end() != copyList1.end()));
		
		BOOST_CHECK_NO_THROW(copyList1.add("e", 1.61803, 3.14159, RangeList::UNIFORM));
		BOOST_CHECK_NO_THROW(dummyRangeList.add("f", 1.61803, 3.14159, RangeList::UNIFORM));
		BOOST_CHECK_NO_THROW(copyList2.add("g", 1.0, 2.71828, RangeList::UNIFORM));
		
		BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(copyList1.getMin("e"), 1.61803));
		BOOST_CHECK_THROW(copyList1.getMin("f"), MissingParam);
		BOOST_CHECK_THROW(copyList1.getMin("g"), MissingParam);
		
		BOOST_CHECK_THROW(dummyRangeList.getMax("e"), MissingParam);
		BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyRangeList.getMax("f"), 3.14159));
		BOOST_CHECK_THROW(dummyRangeList.getMax("g"), MissingParam);
		
		BOOST_CHECK_THROW(copyList2.getType("e"), MissingParam);
		BOOST_CHECK_THROW(copyList2.getType("f"), MissingParam);
		BOOST_CHECK_NO_THROW(BOOST_CHECK(copyList2.getType("g") == RangeList::UNIFORM));
	} catch (const std::bad_alloc& e) {
		BOOST_FAIL("Out of memory.");
	}
}

/** Tests @ref lcmc::models::RangeList "RangeList" modification
 *
 * @see @ref lcmc::models::RangeList "RangeList"
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(add) {
	// Verify that dummyRangeList has been reset
	BOOST_REQUIRE_NO_THROW(matchDummy(dummyRangeList));
	
	// Adding a parameter works whether or not the list was 
	//	previously empty
	BOOST_CHECK_NO_THROW(emptyRangeList.add("f", std::pair<double, double>(1.1489, 2.0), RangeList::UNIFORM ));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(emptyRangeList.getMin("f"), 1.1489));
	BOOST_CHECK_NO_THROW(dummyRangeList.add("f", 1.1489, 2.0, RangeList::UNIFORM));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyRangeList.getMin("f"), 1.1489));

	//adding corrupted data returns errors
	const static double nan = std::numeric_limits<double>::quiet_NaN();
	BOOST_CHECK_THROW(dummyRangeList.add("g", nan, 3.4, RangeList::LOGUNIFORM), 
			UnexpectedNan);
	BOOST_CHECK_THROW(dummyRangeList.add("g", std::pair<double, double>(2.0, nan), 
			RangeList::LOGUNIFORM), 
			UnexpectedNan);
	BOOST_CHECK_THROW(dummyRangeList.add("g", 2.0, 3.4, 
			static_cast<RangeList::RangeType>(2)), 
			std::invalid_argument);
	BOOST_CHECK_THROW(dummyRangeList.add("g", 3.4, 2.0, RangeList::LOGUNIFORM), 
			NegativeRange);
	// atomic?
	BOOST_CHECK_NO_THROW(matchDummyPos(dummyRangeList));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyRangeList.getMin("f"), 1.1489));
	BOOST_CHECK_THROW(dummyRangeList.getMin("g"), MissingParam);
	
	//adding a duplicate parameter returns an error
	BOOST_CHECK_THROW(dummyRangeList.add("c", 1e-3, 0.01, RangeList::UNIFORM), ExtraParam);
	BOOST_CHECK_THROW(dummyRangeList.add("c", std::pair<double, double>(1e-3, 0.01), RangeList::UNIFORM), ExtraParam);
	// atomic?
	BOOST_CHECK_NO_THROW(matchDummyPos(dummyRangeList));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(dummyRangeList.getMin("f"), 1.1489));
	BOOST_CHECK_THROW(dummyRangeList.getMin("g"), MissingParam);
}

/** Tests @ref lcmc::models::RangeList "RangeList" modification
 *
 * @see @ref lcmc::models::RangeList "RangeList"
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(remove) {
	// Verify that dummyRangeList has been reset
	BOOST_REQUIRE_NO_THROW(matchDummy(dummyRangeList));
	
	// List is non-empty
	BOOST_CHECK_NO_THROW(BOOST_CHECK(std::distance(dummyRangeList.begin(), 
			dummyRangeList.end()) > 0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(dummyRangeList.begin() != dummyRangeList.end()));
	
	// Clearing should create an empty list
	BOOST_CHECK_NO_THROW(dummyRangeList.clear());
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(std::distance(dummyRangeList.begin(), 
			dummyRangeList.end()), 0));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(dummyRangeList.begin() == dummyRangeList.end()));
}

/** Tests @ref lcmc::models::RangeList "RangeList" iterator
 *
 * @see @ref lcmc::models::RangeList::const_iterator "RangeList::const_iterator"
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(iterator) {
	const static size_t CHAR_OFFSET = 0x41;
	const static size_t I_END = 15;
	BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(std::distance(emptyRangeList.begin(), 
			emptyRangeList.end()), 0));

	// Test whether begin() and end() have correct separation
	// Test postfix ++
	for(size_t i = 0; i < I_END; i++) {
		BOOST_CHECK_NO_THROW(emptyRangeList.add(
				std::string(1, static_cast<char>(CHAR_OFFSET+i)), 
				static_cast<double>(i), 3.0*static_cast<double>(i), RangeList::UNIFORM));
		BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(std::distance(emptyRangeList.begin(), emptyRangeList.end()), 
			static_cast<ptrdiff_t>(i+1)));
	}
	
	// Test whether begin() and end() have correct values
	// Test prefix ++
	// Test *
	{
		size_t i = 0;
		RangeList::const_iterator it = emptyRangeList.begin();
		for(; it != emptyRangeList.end(); ++it, ++i) {
			BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL(*it, 
					std::string(1, static_cast<char>(CHAR_OFFSET+i))));
		}
	}

	// Test prefix --
	// Test ->
	{
		size_t i = I_END;
		RangeList::const_iterator it = emptyRangeList.end();
		// Start with a decrement to avoid RangeList::end()
		for(it--, i--; it != emptyRangeList.begin(); --it, --i) {
			BOOST_CHECK_NO_THROW(BOOST_CHECK_EQUAL((it->c_str())[0], 
					static_cast<char>(CHAR_OFFSET+i)));
		}
	}
	
	// Equality and self-consistency
	// Also, test postfix --
	RangeList::const_iterator it1 = emptyRangeList.begin();
	BOOST_CHECK_NO_THROW(it1++);
	BOOST_CHECK_NO_THROW(it1++);
	BOOST_CHECK_NO_THROW(it1--);
	RangeList::const_iterator it2 = emptyRangeList.begin();
	BOOST_CHECK_NO_THROW(it2++);
	
	BOOST_CHECK_NO_THROW(BOOST_CHECK(it1 != emptyRangeList.begin()));
	BOOST_CHECK_NO_THROW(BOOST_CHECK(it1 == it2));
	
	// Out-of-bounds
	RangeList::const_iterator front = emptyRangeList.begin();
	BOOST_CHECK_NO_THROW(BOOST_CHECK_THROW(front--, BadIterator));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_THROW(--front, BadIterator));
	// atomic?
	BOOST_CHECK_NO_THROW(BOOST_CHECK(front == emptyRangeList.begin()));

	RangeList::const_iterator back = emptyRangeList.end();
	BOOST_CHECK_NO_THROW(BOOST_CHECK_THROW(back++, BadIterator));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_THROW(++back, BadIterator));
	// atomic?
	BOOST_CHECK_NO_THROW(BOOST_CHECK(back == emptyRangeList.end()));
	
	// Invalid dereference
	BOOST_CHECK_NO_THROW(BOOST_CHECK_THROW(*back, BadIterator));
	BOOST_CHECK_NO_THROW(BOOST_CHECK_THROW(back->c_str(), BadIterator));
	// atomic?
	BOOST_CHECK_NO_THROW(BOOST_CHECK(back == emptyRangeList.end()));
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
