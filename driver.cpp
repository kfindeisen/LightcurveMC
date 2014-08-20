/** Main program for monte carlo lightcurve simulator
 * @file lightcurveMC/driver.cpp
 * @author Krzysztof Findeisen
 * @date Created January 22, 2010
 * @date Last modified May 26, 2013
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
#include <cstdio>
#include <boost/lexical_cast.hpp>	// dump only
#include <tclap/ArgException.h>
#include "binstats.h"
#include "lightcurvetypes.h"
#include "mcio.h"			// dump only
#include "paramlist.h"
#include "except/parse.h"
#include "sims.h"

using namespace lcmc;
using std::string;
using std::vector;
using stats::LcBinStats;

////////////////////////////////////////
// Forward declarations needed only by the main program

/** This namespace identifies all code specific to Lightcurve MC.
 */
namespace lcmc { 

namespace parse {
/** Converts the input arguments to a set of variables.
 */
void parseArguments(int argc, char* argv[], 
	double& sigma, long& nTrials, long& toPrint, 
	models::RangeList& paramRanges, 
	string& jdList, vector<string>& lcNameList, 
	vector<models::LightCurveType>& lcList, 
	vector< stats::      StatType>& statList, 
	string& dataSet, bool& injectMode);

}}	// end lcmc::parse

/** Returns a string description of the simulation's noise properties
 * 
 * @param[in] injectMode If true, program is being run in injection mode. 
 *	If false, program is being run in noisy simulation mode.
 * @param[in] The name of the catalog file, if any.
 * @param[in] The amplitude of the noise, if any.
 *
 * @return A string representation of (injectMode ? catName : noiseAmp)
 *
 * @exception std::runtime_error Thrown if a string representation could not 
 *	be constructed.
 * @exception std::bad_alloc Thrown if there is not enough memory for a 
 *	string representation.
 *
 * @exceptsafe The function arguments are unchanged in the event 
 *	of an exception.
 */
string noiseDesc(bool injectMode, string catName, double noiseAmp) {
	string noiseStr;
	
	if (injectMode) {
		noiseStr = catName;
	} else {
		char noiseBuf[20];
		if (sprintf(noiseBuf, "%0.2g", noiseAmp) < 0) {
			throw std::runtime_error("Could not format bin name.");
		}
		noiseStr = noiseBuf;
	}
	
	return noiseStr;
}

////////////////////////////////////////
// Main Program

/** The driver for Lightcurve MC.
 *
 * @param[in] argc, argv: an array of C-strings denoting the arguments, using the C 
 *	convention
 *
 * @return 0 if successful, 1 if an error occurred
 */
int main(int argc, char* argv[]) {
	using namespace lcmc::models;
	
	try {
		////////////////////
		// Parse the input
		long nTrials, numToPrint;
		double sigma;
		RangeList limits;
		vector<string> lcNameList;
		vector<models::LightCurveType>   lcList;
		vector< stats::      StatType> statList;
		string dateList, injectCat;
		bool injectMode;
	
		parse::parseArguments(argc, argv, sigma, nTrials, numToPrint, 
			limits, dateList, lcNameList, lcList, statList, injectCat, injectMode);
	
		// For file name formatting
		string noiseStr = noiseDesc(injectMode, injectCat, sigma);
		
		////////////////////
		// And start simulating
		LcBinStats::printBinHeader(stdout, limits, statList);
		
		for(vector<LightCurveType>::const_iterator curve = lcList.begin(); 
				curve != lcList.end(); curve++) {
			const string curName = *(lcNameList.begin() + (curve - lcList.begin()));
			LcBinStats curBin(curName, limits, noiseStr, statList);
			
			for(long i = 0; i < nTrials; i++) {
				// Set up noise or injection tests
				vector<double> times, noise;
				if (injectMode) {
					makeInjectNoise(injectCat, times, noise);
				} else {
					makeTimes(dateList, times);
					makeWhiteNoise(times, sigma, noise);
				}
				
				models::ParamList params = drawParams(limits);
	
				// Generate the light curve
				vector<double> lc;
				simLightCurve(*curve, params, times, noise, lc);
	
				// Collect the statistics
				curBin.analyzeLightCurve(times, lc, params);
	
				// Print a few
				if(i < numToPrint) {
					string dumpFile = "lightcurve_" 
						+ LcBinStats::makeFileName(curName, limits, 
							noiseStr) 
						+ "_" + boost::lexical_cast<string>(i) + ".dat";
					printLightCurve(dumpFile, times, lc);
				}
			}	// end loop over simulations
	
			curBin.printBinStats(stdout);
	
		}	// end loop over light curve types
	
	// End of program; use Pokemon exception handling to 
	//	handle error messages gracefully
	} catch(TCLAP::ExitException &e) {
		// Not necessarily an error; could be --version or --help argument
		return e.getExitStatus();
	} catch(parse::except::ParseError &e) {
		// For consistency with default TCLAP output
		fprintf(stderr, "PARSE ERROR: %s\n", e.what());
		return 1;
	} catch(std::logic_error &e) {
		fprintf(stderr, "BUG: %s\nPlease report this to the developer at krzys@astro.caltech.edu.\n", e.what());
		return 1;
	} catch (const std::exception &e) {
		fprintf(stderr, "ERROR: %s\n", e.what());
		return 1;
	} catch (...) {
		fprintf(stderr, "BUG: Unknown exception.\nPlease report this to the developer at krzys@astro.caltech.edu.\n");
		return 1;
	}
	
	return 0;
}
