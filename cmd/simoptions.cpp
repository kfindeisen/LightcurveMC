/** Definitions for optional command line arguments
 * @file lightcurveMC/cmd/simoptions.cpp
 * @author Krzysztof Findeisen
 * @date Created August 19, 2013
 * @date Last modified August 19, 2013
 */

#include <string>
#include "cmd.tmp.h"
#include "cmd_constraints.tmp.h"

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
using namespace TCLAP;

/** Specifies the command line parameters that change optional settings
 *
 * @param[in,out] cmd The command-line parser used by the program
 * 
 * @post @p cmd now contains the optional command-line parameters 
 *	for Lightcurve MC
 * 
 * @exceptsafe Does not throw exceptions.
 */
void logSimOptions(CmdLineInterface& cmd) {
	static    PositiveNumber<long>    posInt;
	static NonNegativeNumber<long> nonNegInt;

	ValueArg<long>* argRepeat = new ValueArg<long>("", "ntrials", "Number of light curves generated per bin. 1000 if omitted.", 
		false, 1000, &posInt);
	cmd.add(argRepeat);
	ValueArg<long>* argPrint = new ValueArg<long>("", "print", "Number of light curves to print. 0 if omitted.", 
		false, 0, &nonNegInt);
	cmd.add(argPrint);
}

/** Parses the command line parameters that change optional settings
 *
 * @param[in] cmd The command-line parser used by the program
 * @param[out] nTrials The number of times to run the simulations.
 * @param[out] nPrint The number of simulations whose light curves should 
 *	be dumped to file.
 * 
 * @exception std::logic_error Thrown if the expected parameters are missing from @p cmd.
 *
 * @exceptsafe All arguments are left in valid states in the event 
 *	of an exception.
 */
void parseSimOptions(CmdLineInterface& cmd, long& nTrials, long& nPrint) {
	nTrials = getParam<ValueArg<long> >(cmd, "ntrials").getValue();
	nPrint  = getParam<ValueArg<long> >(cmd, "print").getValue();
}

}}	// end lcmc::parse
