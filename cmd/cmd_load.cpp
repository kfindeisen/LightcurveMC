/** Code for converting command line objects into program variables
 * @file lightcurveMC/cmd/cmd_load.cpp
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
 * @date Last modified August 19, 2013
 */

#include <stdexcept>
#include <string>
#include <vector>
#include "../binstats.h"
#include "cmd.tmp.h"
#include "cmd_constraints.tmp.h"
#include "../lightcurveparser.h"
#include "../lightcurvetypes.h"
#include "../paramlist.h"
#include "../except/parse.h"
#include "../statparser.h"

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

using std::string;
using models::LightCurveType;
using models::ParamType;
using models::RangeList;
using stats::StatType;
using namespace TCLAP;

/** Returns a TCLAP::Constraint object that forces the argument to be a known 
 * light curve.
 *
 * @return A global constraint object.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to create 
 *	the object.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
KeywordConstraint* getLcList() {
	static KeywordConstraint* theConstraint = NULL;
	if (theConstraint == NULL) {
		// Because KeywordConstraint takes a mutable reference, need a 
		//	local variable to store the function value
		std::vector<string> lcNames = lightCurveTypes();
		theConstraint = new KeywordConstraint(lcNames);
	}
	
	return theConstraint;
}

/** Returns a TCLAP::Constraint object that forces the argument to be a known 
 * statistic.
 *
 * @return A global constraint object.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to create 
 *	the object.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
KeywordConstraint* getStatList() {
	static KeywordConstraint* theConstraint = NULL;
	if (theConstraint == NULL) {
		// Because KeywordConstraint takes a mutable reference, need a 
		//	local variable to store the function value
		std::vector<string> statNames = statTypes();
		theConstraint = new KeywordConstraint(statNames);
	}
	
	return theConstraint;
}

/** Adds a parameter to a light curve specification if the appropriate 
 * command line argument was used
 *
 * @param[in] range the @ref models::RangeList "RangeList" to update
 * @param[in] paramName the name of the parameter
 * @param[in] paramValue the command-line argument to translate into a range
 * @param[in] distrib the distribution values follow within the range
 *
 * @post If @p paramValue was specified on the command line, the corresponding 
 *	parameter range is added to @p range. Otherwise, @p range is unchanged.
 *
 * @exception lcmc::utils::except::UnexpectedNan Thrown if either element of 
 *	@p paramValue is NaN
 * @exception lcmc::stats::except::ExtraParam Thrown if a value for the parameter is 
 *	already in the list
 * @exception lcmc::stats::except::NegativeRange Thrown if 
 *	@p paramValue.second > @p paramValue.first
 * @exception std::invalid_argument Thrown if @p distrib is not a valid value.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	add an element to the list.
 * 
 * @exceptsafe The arguments are unchanged in the event of an exception.
 */
void addParam(RangeList& range, const ParamType& paramName, 
		ValueArg< std::pair<double, double> >& paramValue, 
		RangeList::RangeType distrib) {
	if (paramValue.isSet()) {
		range.add(paramName, paramValue.getValue(), distrib);
	}
}

/** Specifies the command line parameters that list light curves and statistics
 *
 * @param[in,out] cmd The command-line parser used by the program
 * 
 * @post @p cmd now contains command-line parameters for list input to 
 *	Lightcurve MC
 * 
 * @exceptsafe Does not throw exceptions.
 */
void logSimLists(CmdLineInterface& cmd) {
	// Light curve list
	KeywordConstraint* lcAllowed = getLcList();
	UnlabeledMultiArg<string>* argLcList = new UnlabeledMultiArg<string>("lclist", 
		"List of light curves to model, in order. Allowed values are: " + 
		lcAllowed->description(), 
		true, lcAllowed);
	cmd.add(argLcList);
	
	// Statistics list
	KeywordConstraint* statAllowed = getStatList();
	MultiArg<string>* argStatList = new MultiArg<string>("s", "stat", 
		"List of statistics to calculate, in order. Allowed values are: " + statAllowed->description(), 
		false, statAllowed);
	cmd.add(argStatList);
}

/** Transforms a list of light curve names on the command line to a list of 
 *	light curves parseable by the rest of the program.
 *
 * @param[in] argList a multi-value argument containing the light curve names
 * @param[out] lcNameList a vector of the valid light curve names
 * @param[out] lcList a vector of light curve types
 *
 * @post @p lcNameList.size() &le; <tt>argList.getValue().size()</tt>
 * @post @p lcList = @p std::for_each(@p lcNameList, &@ref lcmc::parse::parseLightCurve() "parseLightCurve")
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	represent the light curve list.
 * @exception lcmc::parse::except::NoLightCurves thrown if no valid light 
 *	curves given
 *
 * @exceptsafe The arguments are unchanged in the event of an exception.
 */
void parseLcList(UnlabeledMultiArg<string>& argList, 
		std::vector<string>& lcNameList, 
		std::vector<LightCurveType>& lcList) {
	using std::swap;

	std::vector<string> fullLcList(argList.getValue());

	// Iterate over the argument list, preserving the user-requested execution order
	std::vector<string        > tempNameList;
	std::vector<LightCurveType> tempLcList;

	for(std::vector<string>::const_iterator it=fullLcList.begin(); 
			it != fullLcList.end(); it++) {
		try {
			const LightCurveType curLc = parseLightCurve(*it);
			// Don't count light curves multiple times
			if (find(tempLcList.begin(), tempLcList.end(), curLc) 
					== tempLcList.end()) {
				tempNameList.push_back(*it);
				tempLcList.push_back(curLc);
			}
		} catch(std::domain_error e) {
			// If there's an invalid light curve, print a warning but keep going
			fprintf(stderr, "WARNING: %s\n", e.what());
		}
	}
	if (tempLcList.size() <= 0) {
		throw except::NoLightCurves("No valid light curves given.");
			// Type " + progName + " -h for a list of choices.");
	}
	
	// IMPORTANT: no exceptions past this point
	// swap<vector> is guaranteed not to throw for equal allocators
	//	Since all vectors here use the default allocator, 
	//	this condition is satisfied
	swap(tempNameList, lcNameList);
	swap(  tempLcList,     lcList);
}

/** Transforms a list of statistic names on the command line to a list of 
 *	statistics parseable by the rest of the program.
 *
 * @param[in] argList a multi-value argument containing the statistic names
 * @param[out] statList a vector of statistic types
 *
 * @post @p statList.size() &le; <tt>argList.getValue().size()</tt>
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	represent the statistic list.
 * @exception lcmc::parse::except::NoStats thrown if no valid 
 *	statistics given
 *
 * @exceptsafe The arguments are unchanged in the event of an exception.
 */
void parseStatList(MultiArg<string>& argList, 
		std::vector<StatType>& statList) {
	using std::swap;

	std::vector<string> fullStatList(argList.getValue());
	if (fullStatList.empty()) {
		fullStatList = statTypes();
	}
	// Iterate over the argument list, preserving the user-requested execution order
	std::vector<StatType> tempStatList;

	for(std::vector<string>::const_iterator it=fullStatList.begin(); 
			it != fullStatList.end(); it++) {
		try {
			const StatType curStat = parseStat(*it);
			// Don't count light curves multiple times
			if (find(tempStatList.begin(), tempStatList.end(), curStat) 
					== tempStatList.end()) {
				tempStatList.push_back(curStat);
			}
		} catch(std::domain_error e) {
			// If there's an invalid light curve, print a warning but keep going
			fprintf(stderr, "WARNING: %s\n", e.what());
		}
	}
	if (tempStatList.size() <= 0) {
		throw except::NoStats("No valid statistics given.");
			// Type " + progName + " -h for a list of choices.");
	}
	
	// IMPORTANT: no exceptions past this point
	// swap<vector> is guaranteed not to throw for equal allocators
	//	Since all vectors here use the default allocator, 
	//	this condition is satisfied
	swap(tempStatList, statList);
}

}}	// end lcmc::parse
