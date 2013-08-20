/** Functions for handling command line objects
 * @file lightcurveMC/cmd/cmd.tmp.h
 * @author Krzysztof Findeisen
 * @date Created May 11, 2013
 * @date Last modified August 19, 2013
 */

#include <stdexcept>
#include <string>
#include <typeinfo>
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

/** Specifies the command line parameters that define the simulation
 */
void logSimType(CmdLineInterface& cmd);

/** Parses the command line parameters that define the simulation
 */
void parseSimType(CmdLineInterface& cmd, string& cadenceFile, string& catalogFile, 
		bool& isInject, double& noise);

/** Specifies the command line parameters that change optional settings
 */
void logSimOptions(CmdLineInterface& cmd);

/** Parses the command line parameters that change optional settings
 */
void parseSimOptions(CmdLineInterface& cmd, long& nTrials, long& nPrint);

/** Specifies the command line parameters that list light curves and statistics
 */
void logSimLists(CmdLineInterface& cmd);

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

/** Specifies the command line parameters that describe model parameter ranges
 */
void logModelParams(CmdLineInterface& cmd);

/** Parses the command line parameters that describe model parameter ranges
 */
void parseModelParams(CmdLineInterface& cmd, RangeList& range);


/** Adds a parameter to a light curve specification if the appropriate 
 * command line argument was used
 */
void addParam(RangeList& range, const ParamType& paramName, 
		ValueArg< std::pair<double, double> >& paramValue, 
		RangeList::RangeType distrib);

/** Extracts a specific parameter from a command line parser
 *
 * @param[in] cmd the command-line parser
 * @param[in] name the name of the parameter to look up
 *
 * @return A reference to the desired parameter.
 *
 * @pre @p cmd has exactly one parameter named @p name
 *
 * @exception std::logic_error Thrown if @p cmd does not have a parameter 
 *	named @p name or if the parameter is not a @c TCLAP::ValueArg
 * 
 * @exceptsafe The arguments are unchanged in the event of an exception.
 */
template <typename ArgType>
ArgType& getParam(CmdLineInterface& cmd, const std::string& name) {
	std::list<Arg*> allArgs = cmd.getArgList();
	
	for(std::list<Arg*>::const_iterator it = allArgs.begin(); it != allArgs.end(); it++) {
		if ((*it)->getName() == name) {
			try {
				ArgType* goodArg = dynamic_cast<ArgType*>(*it);
				return *goodArg;
			} catch (const std::bad_cast& e) {
				throw std::logic_error("Command-line parameter '" 
					+ name + "' is not a " + typeid(ArgType).name());
			}
		}
	}
	
	throw std::logic_error("CMD does not have a command-line parameter named " + name);
}

}}	// end lcmc::parse
