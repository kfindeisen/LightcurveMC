/** Main program for monte carlo lightcurve simulator
 * @file driver.cpp
 * @author Krzysztof Findeisen
 * @date Created January 22, 2010
 * @date Last modified April 21, 2013
 */

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <kpffileio.h>
#include <gsl/gsl_randist.h>
#include "binstats.h"
#include "lightcurvetypes.h"
#include "mcio.h"
#include "paramlist.h"
#include "samples/observations.h"

#if USELFP
#include <lfp/lfp.h>
#endif

using namespace lcmc;
using std::string;
using std::vector;
using models::RangeList;
using models::ParamList;
using inject::Observations;
using inject::dataSampler;
using stats::LcBinStats;

////////////////////////////////////////
// Forward declarations needed only by the main program

/** This namespace identifies all code specific to lightcurveMC.
 */
namespace lcmc { 

/** Randomly generates parameter values within the specified limits
 */
ParamList drawParams(RangeList limits);

namespace parse {
/** Converts the input arguments to a set of variables.
 */
void parseArguments(int argc, char* argv[], 
	double& sigma, long& nTrials, long& toPrint, 
	RangeList& paramRanges, 
	string& jdList, vector<string>& lcNameList, 
	vector<models::LightCurveType>& lcList, 
	string& dataSet, bool& injectMode);

}	// end lcmc::parse

namespace models {
// Full spec given with the declaration because lcregistry.cpp is deliberately 
//	hidden in the documentation
// 
/** lcFactory is a factory method that allocates and initializes a ILightCurve 
 *	object given its specification. Use of an auto_ptr prevents 
 *	client-side memory management issues.
 *
 * @param[in] whichLc The type of light curve to be created.
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] lcParams The bulk properties of the light curve. The parameters 
 * 	needed depend on each individual light curve and are given in the 
 * 	light curve documentation.
 *
 * @todo Write up the parameter list
 *
 * @return A smart pointer to an object of the subclass of ILightCurve 
 *	corresponding to the value of whichLc. The object pointed to by the 
 *	smart pointer has been initialized with the data in lcParams.
 *
 * @pre No element of times is NaN
 *
 * @exception invalid_argument Thrown if whichLc is invalid or if 
 *	lcParams does not have all the parameters needed by whichLc.
 */
std::auto_ptr<ILightCurve> lcFactory(LightCurveType whichLc, const vector<double> &times, const ParamList &lcParams);

}}	// end lcmc::models

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
	using std::auto_ptr;
	using namespace lcmc::models;
	
	#if USELFP
	PInit();
	#endif
	
	////////////////////
	// Major variables
	gsl_rng * mcDriver = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(mcDriver, 42);

	// Storage for run-time parameters
	long nTrials, numToPrint;
	double sigma;
	RangeList limits;
	vector<string> lcNameList;
	vector<LightCurveType> lcList;
	string dateList, injectType;
	bool injectMode;

	// Storage for the data
	vector<double> tSeries, lc;
	double minTStep, maxTStep;
	size_t nObs = 0;

	////////////////////
	// Parse the input
	try {
		parse::parseArguments(argc, argv, sigma, nTrials, numToPrint, 
			limits, dateList, lcNameList, lcList, injectType, injectMode);
	} catch(std::runtime_error &e) {
		fprintf(stderr, "ERROR: %s\n", e.what());
		return 1;
	}
	
	////////////////////
	// Load and preprocess the data
	/** @todo The injectMode flag here is dangerous... rewrite later!
	 */
	if (!injectMode) {
		try {
			FILE* hJulDates = kpffileio::bestFileOpen(dateList.c_str(), "r", 
				(string("Could not read photometry dates from ") + dateList).c_str());
			readTimeStamps(hJulDates, tSeries, minTStep, maxTStep);
			nObs = tSeries.size();
			fclose(hJulDates);
			hJulDates = NULL;
		} catch(std::runtime_error &e) {
			fprintf(stderr, "ERROR: %s\n", e.what());
			return 1;
		}
	}

	////////////////////
	// And start simulating
//	printf("%li trials per bin\n", nTrials);
	LcBinStats::printBinHeader(stdout, limits);

	try {
	for(vector<LightCurveType>::const_iterator curve = lcList.begin(); 
			curve != lcList.end(); curve++) {
		const string curName = *(lcNameList.begin() + (curve - lcList.begin()));
		char noiseStr[20];
		sprintf(noiseStr, "%0.2g", sigma);
		LcBinStats curBin(curName, limits, (injectMode ? injectType : noiseStr));
		
		#if USELFP
		PClear(1);
		PClear(2); PClear(3);
		#endif
		for(long i = 0; i < nTrials; i++) {
			#if USELFP
			PStart(1);
			#endif
		
			auto_ptr<Observations> curData;
			if (injectMode) {
				// Load the data into which the signal will be injected
				curData = dataSampler(injectType);
				curData->getTimes(tSeries);
				curData->getFluxes(lc);
				nObs = tSeries.size();
				if (nObs <= 0) {
					throw std::runtime_error("Empty light curve read.");
				}
			} else {
				// Generate a new dataset
				lc.clear();
				lc.resize(nObs);
			}

			// Choose the parameters for this star, constraining them to the bin
			ParamList params = drawParams(limits);

			// Generate the light curve...
			auto_ptr<ILightCurve> lcInstance = lcFactory(*curve, tSeries, params);
			if (injectMode) {
				for(size_t j = 0; j < nObs; j++) {
					// lc[j] already contains the "noise", 
					// so just add the "real" simulated  
					// signal
					/** @bug Either new ILightCurve implementation or its driver code much too slow when --add keyword is used
					 */
					vector<double> temp;
					lcInstance->getFluxes(temp);
					lc[j] += temp[j];
				}
			} else {
				#if USELFP
				PStart(3);
				#endif
				for(size_t j = 0; j < nObs; j++) {
					vector<double> temp;
					lcInstance->getFluxes(temp);
					// Include noise in the observations
					lc[j] += temp[j] + 
						gsl_ran_gaussian(mcDriver, sigma);
				}
				#if USELFP
				PEnd(3);
				#endif
			}

			#if USELFP
			PStart(2);
			#endif
			// Analyze the light curve
			curBin.analyzeLightCurve(tSeries, lc, params);
			#if USELFP
			PEnd(2);
			#endif

			// Print a few
			if(i < numToPrint) {
				printLightCurve(i, curName, limits.getMin("p"), 
						limits.getMin("a"),  params.get("p"), 
						tSeries, lc);
			}

			#if USELFP
			PEnd(1);
			#endif
		}	// end loop over simulations
		// Report the results for the current bin
		curBin.printBinStats(stdout);

		#if USELFP
		fprintf(stderr, "LFP: %i simulations of a %s.\n", nTrials, curName.c_str());
		fprintf(stderr, "LFP: Average time in loop: %f s\n", GetDoublePTime(1)/1000);

		fprintf(stderr, "LFP: \tFraction spent calculating the data: %f\n", 
			GetDoublePTime(3)/GetDoublePTime(1));
		fprintf(stderr, "LFP: Average time spent calculating the data: %f s\n", 
			GetDoublePTime(3)/1000);

		fprintf(stderr, "LFP: \tFraction spent analyzing the data: %f\n", 
			GetDoublePTime(2)/GetDoublePTime(1));
		fprintf(stderr, "LFP: Average time spent analyzing the data: %f s\n", 
			GetDoublePTime(2)/1000);

		fprintf(stderr, "\n");
		#endif

	}	// end loop over light curve types
	} catch(std::runtime_error &e) {
		fprintf(stderr, "ERROR: %s\n", e.what());
		return 1;
	// End of program; use Pokemon exception handling to 
	//	handle error messages gracefully
	} catch (std::exception &e) {
		fprintf(stderr, "ERROR: %s\n", e.what());
		return 1;
	} catch (...) {
		fprintf(stderr, "ERROR: Unknown exception. Sorry!\n");
		return 1;
	}
	
	gsl_rng_free(mcDriver);

	return 0;
}

namespace lcmc {

/** Randomly generates parameter values within the specified limits
 *
 * @param[in] limits A RangeList giving the parameters to generate, the 
 *	minimum and maximum values for each, and the distribution from which 
 *	to sample
 *
 * @return A ParamList giving a value for each parameter
 *
 * @post X is a parameter in the return value if and only if X is a parameter 
 *	in limits
 * @post for any X in the return value, 
 *	limits.getMin(X) <= return.get(X) <= limits.getMax(X) 
 *
 * @post No element of the return value is NaN.
 */
ParamList drawParams(RangeList limits) {
	// Need a random number generator
	static gsl_rng * randomizer = gsl_rng_alloc(gsl_rng_mt19937);
	static bool seeded = false;
	if (!seeded) {
		gsl_rng_set(randomizer, 42);
		seeded = true;
	}
	
	ParamList returnValue;
	// Convert all parameters
	for(RangeList::ConstIterator it = limits.begin(); it != limits.end(); it++) {
		double min                   = limits.getMin(*it);
		double max                   = limits.getMax(*it);
		RangeList::RangeType distrib = limits.getType(*it);
		
		double value;
		switch(distrib) {
			case RangeList::UNIFORM:
				value = (min == max ? min 
					: min + (max-min)*gsl_rng_uniform(randomizer));
				break;
			case RangeList::LOGUNIFORM:
				value = (min == max ? log10(min) 
					: log10(min) + (log10(max)-log10(min))*gsl_rng_uniform(randomizer));
				value = pow(10.0, value);
				break;
			default:
				throw std::logic_error("Unknown distribution!");
		};
		
		returnValue.add(*it, value);
	}
	
	return returnValue;
}

}	// end lcmc
