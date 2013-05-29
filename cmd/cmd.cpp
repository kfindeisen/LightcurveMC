/** Code for parsing the command line interface
 * @file lightcurveMC/cmd/cmd.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified May 27, 2013
 */

#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include "../binstats.h"
#include "cmd.h"
#include "cmd_constraints.tmp.h"
#include "cmd_ranges.tmp.h"
#include "../lightcurveparser.h"
#include "../lightcurvetypes.h"
#include "../paramlist.h"
#include "../except/nan.h"
#include "../except/paramlist.h"
#include "../except/parse.h"
#include "../projectinfo.h"
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

/** Extracts formatted input into a pair of double precision floating point 
 * values.
 */
// Must be declared in the global namespace to be identified by TCLAP
std::istream& operator>> (std::istream& str, std::pair<double, double>& val);

namespace lcmc { namespace parse {

using std::string;
using std::vector;
using  stats::StatType;
using models::LightCurveType;
using models::RangeList;

/** Converts the input arguments to a set of variables.
 *
 * @param[in] argc,argv an array of C-strings denoting the arguments, using the C 
 *	convention
 * @param[out] sigma the root-mean-square amount of noise to add to all flux 
 *	measurements
 * @param[out] nTrials the number of simulations to execute per bin
 * @param[out] toPrint if > 0, the program will print the first few 
 *	light curves generated. This setting is intended primarily for 
 *	debugging.
 * @param[out] paramRanges the minimum and maximum values of light curve 
 *	parameters allowed by the user
 * @param[out] jdList the name of a file containing the Julian dates of the 
 *	simulated observations
 * @param[out] lcNameList the list of light curve types to simulate, one 
 *	after the other
 * @param[out] lcList the list of light curve types to simulate
 * @param[out] statList the list of statistics to calculate for each simulated 
 *	light curve
 * @param[out] dataSet the name of a file containing observed light curves into 
 *	which to inject simulated light curves
 * @param[out] injectMode if true, the program will carry out an injection 
 *	analysis rather than merely generating theoretical light curves.
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	represent the command line arguments.
 * @exception std::logic_error Thrown if a bug is detected in the code.
 * @exception lcmc::parse::except::ParseError Thrown if the output cannot be parsed.
 *
 * @exceptsafe All arguments are left in valid states in the event 
 *	of an exception.
 *
 * @todo Break up this function.
 * @todo Current command-line format vulnerable to mismatches in which 
 *	argument is being referred to. Rewrite!
 */
void parseArguments(int argc, char* argv[], 
		double& sigma, long& nTrials, long& toPrint, 
		RangeList& paramRanges, 
		string& jdList, vector<string>& lcNameList, 
		vector<LightCurveType>&  lcList, 
		vector<      StatType>& statList, 
		string& dataSet, bool& injectMode) {
	using namespace TCLAP;
	typedef std::pair<double, double> Range;
	
  	// Start by defining the command line
	CmdLine cmd(PROG_SUMMARY, ' ', VERSION_STRING);
	cmd.setExceptionHandling(false);
	
	try {
		//--------------------------------------------------
		// Common constraints
		// TCLAP::Constraint interface not declared const :(
		static    PositiveNumber<double>    posReal;
		static    PositiveNumber<long>      posInt;
		static    PositiveNumber<Range>     posRange;
		static NonNegativeNumber<double> nonNegReal;
		static NonNegativeNumber<long>   nonNegInt;
		static UnitSubrange unitRange;
	
		//--------------------------------------------------
		// Mandatory simulation settings
		ValueArg<double> argNoise("", "noise", "Gaussian error added to each photometric measurement, in units of the typical source flux. REQUIRES that <date file> is provided.", 
			false, 
			0.0, &nonNegReal);
	
		ValueArg<string> argInject("", "add", 
			"Name of a text file containing the names of light curves to sample.", 
			false, 
			"", "file list");
	
		UnlabeledValueArg<string> argDateFile("jdlist", 
			"Text file containing a list of Julian dates, one per line.", 
			true, 
			"", "date list");
		cmd.xorAdd(argInject, argDateFile);
		// argNoise is only allowed if argDateFile is used, but since 
		//	argDateFile is already in a xor relationship we'll have to 
		//	enforce this constraint manually after parsing
		cmd.add(argNoise);
	
		//--------------------------------------------------
		// Optional simulation settings
		ValueArg<long> argRepeat("", "ntrials", "Number of light curves generated per bin. 1000 if omitted.", 
			false, 
			1000, &posInt, cmd);
		ValueArg<long> argPrint("", "print", "Number of light curves to print. 0 if omitted.", 
			false, 
			0, &nonNegInt, cmd);
	
		//--------------------------------------------------
		// Light curve list
		std::vector<string> lcNames = lightCurveTypes();
		KeywordConstraint lcAllowed(lcNames);
		UnlabeledMultiArg<string> argLcList("lclist", 
			"List of light curves to model, in order. Allowed values are: " + lcAllowed.description(), 
			true, 
			&lcAllowed, cmd);
		
		//--------------------------------------------------
		// Statistics list
		std::vector<string> statNames = statTypes();
		KeywordConstraint statAllowed(statNames);
		MultiArg<string> argStatList("s", "stat", 
			"List of statistics to calculate, in order. Allowed values are: " + statAllowed.description(), 
			false, 
			&statAllowed, cmd);
		
		//--------------------------------------------------
		// Model parameters
		ValueArg<Range> argPer ("p", "period", 
			"the smallest and largest periods, in days, to be tested. The period will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
		ValueArg<Range> argAmp ("a", "amp", 
			"the smallest and largest amplitudes to be tested. The amplitude will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
		ValueArg<Range> argPhi ("", "ph", 
			"the smallest and largest initial phases to be tested. The phase will be drawn from a uniform distribution. MUST be a subinterval of [0.0, 1.0]. Set to \"0.0 1.0\" if unspecified.", 
			false, 
			Range(0.0, 1.0), &unitRange, cmd);
		ValueArg<Range> argDiffus ("d", "diffus", 
			"the smallest and largest diffusion constants to be tested. The constant will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
		ValueArg<Range> argWidth ("w", "width", 
			"the smallest and largest event widths to be tested. The width will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
		ValueArg<Range> argWidth2 ("", "width2", 
			"the smallest and largest secondary widths to be tested. The width will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
		ValueArg<Range> argPer2 ("", "period2", 
			"the smallest and largest secondary periods, in days, to be tested. The secondary period will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
		ValueArg<Range> argAmp2 ("", "amp2", 
			"the smallest and largest secondary amplitudes to be tested. The secondary amplitude will be drawn from a log-uniform distribution.", 
			false, 
			Range(), &posRange, cmd);
	
		//--------------------------------------------------
		// Read from the command line
	
		cmd.parse( argc, argv );
		// constraint: --noise only valid if jdList defined
		if (argNoise.isSet() && !argDateFile.isSet()) {
			throw CmdLineParseException("Mutually exclusive argument already set!", 
				"(--noise)");
		}
		
		//--------------------------------------------------
		// Export values
	
		// Mandatory simulation settings
		sigma    = argNoise   .getValue();
		dataSet  = argInject  .getValue();
		jdList   = argDateFile.getValue();
		// Test if --add argument was used
		injectMode = (dataSet.size() > 0);
	
		// Optional simulation settings
		nTrials  = argRepeat  .getValue();
		toPrint  = argPrint   .getValue();
	
		// Light curve list
		try {
			parseLcList(argLcList, lcNameList, lcList);
		} catch (const except::NoLightCurves& e) {
			throw except::NoLightCurves(string(e.what()) 
				+ " Type " + cmd.getProgramName() 
				+ " -h for a list of choices.");
		}
	
		// Statistics list
		try {
			parseStatList(argStatList, statList);
		} catch (const except::NoStats& e) {
			throw except::NoStats(string(e.what()) 
				+ " Type " + cmd.getProgramName() 
				+ " -h for a list of choices.");
		}
	
		// Model parameters
		try {
			paramRanges.clear();
			addParam(paramRanges, "a", argAmp, RangeList::LOGUNIFORM);
			addParam(paramRanges, "p", argPer, RangeList::LOGUNIFORM);
			// Include default phase if no user input
			paramRanges.add("ph", argPhi.getValue(), RangeList::UNIFORM);
			addParam(paramRanges, "width", argWidth, RangeList::LOGUNIFORM);
			addParam(paramRanges, "width2", argWidth2, RangeList::LOGUNIFORM);
			addParam(paramRanges, "d", argDiffus, RangeList::LOGUNIFORM);
			addParam(paramRanges, "a2", argAmp2, RangeList::LOGUNIFORM);
			addParam(paramRanges, "p2", argPer2, RangeList::LOGUNIFORM);
		} catch (const utils::except::UnexpectedNan& e) {
			throw std::logic_error("Parameter validation doesn't work!\nOriginal error: " + string(e.what()));
		} catch (const models::except::ExtraParam& e) {
			throw std::logic_error("Parameter parsing is incorrect!\nOriginal error: " + string(e.what()));
		} catch (const models::except::NegativeRange& e) {
			throw std::logic_error("Parameter validation doesn't work!\nOriginal error: " + string(e.what()));
		} catch (const std::invalid_argument& e) {
			throw std::logic_error("Parameter parsing is incorrect!\nOriginal error: " + string(e.what()));
		}
	} catch (const TCLAP::ArgException& e) {
		// Translate to a generic exception to encapsulate our choice of parser
		throw except::ParseError(e.argId() 
			// Emulate TCLAP error reporting format
			+ "\n             " + e.error() + "\n\n" 
			+ "For complete USAGE and HELP type: \n   " 
			+ cmd.getProgramName() + " --help\n");
	}
}

}}	// end lcmc::parse
