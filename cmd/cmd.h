/** Functions for handling command line objects
 * @file lightcurveMC/cmd/cmd.h
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
 * @date Last modified May 11, 2013
 */

#include <string>
#include <vector>
#include "../binstats.h"
#include "../lightcurvetypes.h"
#include "../paramlist.h"

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

/** Adds a parameter to a light curve specification if the appropriate 
 * command line argument was used
 */
void addParam(RangeList& range, const ParamType& paramName, 
		ValueArg< std::pair<double, double> >& paramValue, 
		RangeList::RangeType distrib);

/** Transforms a list of light curve names on the command line to a list of 
 *	light curves parseable by the rest of the program.
 */
void parseLcList(UnlabeledMultiArg<string>& argList, 
		std::vector<string>& lcNameList, 
		std::vector<LightCurveType>& lcList);

/** Transforms a list of statistic names on the command line to a list of 
 *	statistics parseable by the rest of the program.
 */
void parseStatList(MultiArg<string>& argList, 
		std::vector<StatType>& statList);

}}	// end lcmc::parse
