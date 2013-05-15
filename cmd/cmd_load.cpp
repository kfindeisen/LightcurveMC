/** Code for converting command line objects into program variables
 * @file lightcurveMC/cmd/cmd_load.cpp
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
 * @date Last modified May 11, 2013
 */

#include <stdexcept>
#include <string>
#include <vector>
#include "../binstats.h"
#include "cmd.h"
//#include "cmd_constraints.tmp.h"
//#include "cmd_ranges.tmp.h"
#include "../lightcurveparser.h"
#include "../lightcurvetypes.h"
#include "../paramlist.h"
#include "../except/parse.h"
//#include "../projectinfo.h"
#include "../statparser.h"

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

namespace lcmc { namespace parse {

using std::string;
using models::LightCurveType;
using models::ParamType;
using models::RangeList;
using stats::StatType;
using namespace TCLAP;

/** Adds a parameter to a light curve specification if the appropriate 
 * command line argument was used
 *
 * @param[in] range the RangeList to update
 * @param[in] paramName the name of the parameter
 * @param[in] paramValue the command-line argument to translate into a range
 * @param[in] distrib the distribution values assume within the range
 *
 * @post If paramValue was specified on the command line, the corresponding 
 *	parameter range is added to range. Otherwise, range is unchanged.
 *
 * @exception utils::except::UnexpectedNan Thrown if either min or max is NaN
 * @exception except::ExtraParam Thrown if a value for the parameter is 
 *	already in the list
 * @exception except::NegativeRange Thrown if max > min
 * @exception std::invalid_argument Thrown if distrib is not a valid value.
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

/** Transforms a list of light curve names on the command line to a list of 
 *	light curves parseable by the rest of the program.
 *
 * @param[in] argList a multi-value argument containing the light curve names
 * @param[out] lcNameList a vector of the valid light curve names
 * @param[out] lcList a vector of light curve types
 *
 * @post lcNameList.size() <= argList.getValue().size()
 * @post lcList == std::for_each(lcNameList, &lcmc::parse::parseLightCurve)
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
 * @post statList.size() <= argList.getValue().size()
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
