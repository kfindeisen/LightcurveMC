/** Definitions for essential command line arguments
 * @file lightcurveMC/cmd/simtype.cpp
 * @author Krzysztof Findeisen
 * @date Created May 29, 2013
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

/** Specifies the command line parameters that define the simulation
 *
 * @param[in,out] cmd The command-line parser used by the program
 * 
 * @post @p cmd now contains the essential command-line parameters 
 *	for Lightcurve MC
 * 
 * @exceptsafe Does not throw exceptions.
 */
void logSimType(CmdLineInterface& cmd) {
	static NonNegativeNumber<double> nonNegReal;

	ValueArg<double>* argNoise = new ValueArg<double>("", "noise", "Gaussian error added to each photometric measurement, in units of the typical source flux. REQUIRES that <date file> is provided.", 
		false, 0.0, &nonNegReal);

	ValueArg<string>* argInject = new ValueArg<string>("", "add", 
		"Name of a text file containing the names of light curves to sample.", 
		false, "", "file list");

	UnlabeledValueArg<string>* argDateFile = new UnlabeledValueArg<string>("jdlist", 
		"Text file containing a list of Julian dates, one per line.", 
		true, "", "date list");
	cmd.xorAdd(*argInject, *argDateFile);
	// argNoise is only allowed if argDateFile is used, but since 
	//	argDateFile is already in a xor relationship we'll have to 
	//	enforce this constraint manually after parsing
	cmd.add(argNoise);
}

/** Parses the command line parameters that define the simulation
 *
 * @param[in] cmd The command-line parser used by the program
 * @param[out] cadenceFile The name of a file containing the time stamps 
 *	to simulate with (if specified).
 * @param[out] catalogFile The name of a file containing a list 
 *	of light curves to add the simulated signal to (if specified).
 * @param[out] isInject True if the program will do an injection analysis, 
 *	and false if it will do a standalone simulation.
 * @param[out] noise The amount of white noise to add to the program.
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	represent the arguments.
 * @exception std::logic_error Thrown if the expected parameters are missing from @p cmd.
 *
 * @exceptsafe All arguments are left in valid states in the event 
 *	of an exception.
 */
void parseSimType(CmdLineInterface& cmd, string& cadenceFile, string& catalogFile, 
		bool& isInject, double& noise) {
	cadenceFile = getParam<UnlabeledValueArg<string> >(cmd, "jdlist").getValue();
	catalogFile = getParam<ValueArg<string> >(cmd, "add").getValue();
	isInject    = (catalogFile.size() > 0);
	noise       = getParam<ValueArg<double> >(cmd, "noise").getValue();
}

}}	// end lcmc::parse
