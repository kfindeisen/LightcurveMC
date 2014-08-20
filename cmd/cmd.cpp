/** Code for parsing the command line interface
 * @file lightcurveMC/cmd/cmd.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified August 19, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <string>
#include <vector>
#include "cmd.tmp.h"
#include "cmdlinedyn.h"
#include "../lightcurvetypes.h"
#include "../paramlist.h"
#include "../except/nan.h"
#include "../except/paramlist.h"
#include "../except/parse.h"
#include "../projectinfo.h"

#include "../../common/warnflags.h"

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
 * @todo Reimplenent using <tt>Boost.Program Options</tt>
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
	
  	// Start by defining the command line
	CmdLineDyn cmd(PROG_SUMMARY, ' ', VERSION_STRING);
	
	try {
		// Register program parameters
		logSimType(cmd);
		logSimOptions(cmd);
		logSimLists(cmd);
		logModelParams(cmd);
		
		// Read from the command line
	
		cmd.parse( argc, argv );
		// constraint: --noise only valid if jdList defined
		if (getParam<ValueArg<double> >(cmd, "noise").isSet() 
				&& !getParam<UnlabeledValueArg<string> >(cmd, "jdlist")
				.isSet()) {
			throw CmdLineParseException("Mutually exclusive argument already set!", 
				"(--noise)");
		}
		
		//--------------------------------------------------
		// Export values
	
		// Mandatory simulation settings
		parseSimType(cmd, jdList, dataSet, injectMode, sigma);
	
		// Optional simulation settings
		parseSimOptions(cmd, nTrials, toPrint);
	
		// Light curve list
		try {
			parseLcList(getParam<UnlabeledMultiArg<string> >(cmd, "lclist"), 
				lcNameList, lcList);
		} catch (const except::NoLightCurves& e) {
			throw except::NoLightCurves(string(e.what()) 
				+ " Type " + cmd.getProgramName() 
				+ " -h for a list of choices.");
		}
	
		// Statistics list
		try {
			parseStatList(getParam<MultiArg<string> >(cmd, "stat"), 
				statList);
		} catch (const except::NoStats& e) {
			throw except::NoStats(string(e.what()) 
				+ " Type " + cmd.getProgramName() 
				+ " -h for a list of choices.");
		}
	
		// Model parameters
		try {
			parseModelParams(cmd, paramRanges);
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
