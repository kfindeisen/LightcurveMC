/** Class that calculates test statistics on artificial light curves
 * @file binstats.cpp
 * @author Krzysztof Findeisen
 * @date Created June 6, 2011
 * @date Last modified April 27, 2013
 */

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <cmath>
#include <cstdio>
#include <timescales/timescales.h>
#include "binstats.h"
#include "fluxmag.h"
#include "nan.h"
#include "stats/dmdt.h"
#include "stats/experimental.h"
#include "stats/magdist.h"

#include "warnflags.h"
#if USELFP
#include <lfp/lfp.h>
#endif

using std::string;
using lcmc::models::RangeList;
using lcmc::models::ParamList;

/** Size of character buffers to use when generating strings
 */
#define MAX_STRING 512

// Current implementation of analyzeLightCurve does not use 
// trueParams, but it should still be part of the interface
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace lcmc { namespace stats {

/** Calculates the mean and standard deviation of a collection of statistics
 *
 * @param[in] values The statistics to be summarized.
 * @param[out] mean The mean of the statistics.
 * @param[out] stddev The standard deviation of the statistics.
 * @param[in] statName The name of the statistic, for error messages.
 *
 * @post mean and stddev ignore any NaNs present in values.
 * @post If there are no non-NaN elements in values, mean equals NaN
 * @post If there are less than two non-NaN elements in values, stddev equals NaN
 */
void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		const string& statName) {
	mean   = std::numeric_limits<double>::quiet_NaN();
	stddev = std::numeric_limits<double>::quiet_NaN();
	
	try {
		// For the exception handling to work right, order matters!
		// Mean requires at least one measurement
		// Standard deviation requires at least two, i.e. it has 
		//	strictly tighter requirements than the mean and must 
		//	go later
		mean   =      utils::    meanNoNan(values);
		stddev = sqrt(utils::varianceNoNan(values));
	} catch (std::invalid_argument &e) {
		// These should just be warnings that a statistic is undefined
		fprintf(stderr, "WARNING: %s summary: %s\n", statName.c_str(), e.what());
	}
}

/** Calculates the mean, standard deviation, and definition rate of a collection of statistics
 *
 * @param[in] values The statistics to be summarized.
 * @param[out] mean The mean of the statistics.
 * @param[out] stddev The standard deviation of the statistics.
 * @param[out] goodFrac The fraction of measurements that are finite values.
 * @param[in] statName The name of the statistic, for error messages.
 *
 * @post mean and stddev ignore and NaNs present in values.
 * @post If there are no non-NaN elements in values, mean equals NaN
 * @post If there are less than two non-NaN elements in values, stddev equals NaN
 */
void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		double& goodFrac, 
		const string& statName) {
	size_t n = values.size();
	
	double badCount = static_cast<double>(
		std::count_if(values.begin(), values.end(), &utils::isNanOrInf));
	goodFrac = (n > 0 ? 1.0 - badCount/n : 0.0);
	getSummaryStats(values, mean, stddev, statName);
}

/** Creates a new stat counter.
 *
 * @param[in] modelName The name of the model being tested. Used to name the 
 * bin's entry in the log file, as well as auxiliary files.
 * @param[in] binSpecs The properties of the model being tested. Used to name 
 * the bin's entry in the log file, as well as auxiliary files.
 * @param[in] noise The noise level of the model being tested. If white noise 
 *	is being used, this string is a decimal representation of the root-
 *	mean-square noise level. If instead signal injection is being used, 
 *	this string is the name of the light curve library to which signals 
 *	are added.
 */
LcBinStats::LcBinStats(string modelName, const RangeList& binSpecs, string noise) 
		: binName(makeBinName(modelName, binSpecs, noise)), 
		fileName(makeFileName(modelName, binSpecs, noise)), 
		C1vals()/*, periods()*/, cut50Amp3s(), cut50Amp2s(), 
		cut90Amp3s(), cut90Amp2s(), 
		dmdtMedianTimes(), dmdtMedians() {
}

// Current implementation of analyzeLightCurve does not use  
// trueParams, but it should still be part of the interface
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

/** Calculates statistics from the light curve and records them in lcBinStats.
 * 
 * @param[in] times The time stamps of the observations.
 * @param[in] fluxes A Monte Carlo realization of a light curve sampled over times.
 * @param[in] trueParams The parameters used to calculate fluxes.
 *
 * @pre No element of times is NaN
 * @pre Fluxes may contain NaNs
 *
 * @todo Add test case for flux-sine light curve, to see if NaNs get ignored
 *
 * @internal @note The implementation *should not* assume that any particular parameter 
 * is defined in trueParams.
 */
void LcBinStats::analyzeLightCurve(const DoubleVec& times, const DoubleVec& fluxes, 
		const ParamList& trueParams) {
	DoubleVec mags(fluxes.size());
	utils::fluxToMag(fluxes, mags);

	////////////////////////////////////////
	// Calculate c1
	
	try {
		double C1 = getC1(mags);
		C1vals.push_back(C1);
	} catch (std::runtime_error &e) {
		// Don't add a C1; move on
	}
	
	////////////////////////////////////////
	// Period measures
	DoubleVec change50, change90;

	try {
		double amplitude = getAmplitude(mags);
		
		double  minBin = -1.97, maxBin = log10(kpftimes:: deltaT(times));
		DoubleVec binEdges;
		for (double bin = minBin; bin < maxBin; bin += 0.15) {
			binEdges.push_back(pow(10.0,bin));
		}
		// Get the bin containing maxBin as well
		binEdges.push_back(pow(10.0,maxBin));
		
		DoubleVec cleanTimes, cleanMags, deltaT, deltaM;
		utils::removeNans(mags, cleanMags, times, cleanTimes);
		#if USELFP
		PClear(7);
		PStart(7);
		try{
		#endif
		kpftimes::dmdt(cleanTimes, cleanMags, deltaT, deltaM);
		#if USELFP
		PEnd(7);
		} catch (std::runtime_error &e) {
			PEnd(7);
			throw;
		}
		#endif
		
		#if USELFP
		PClear(3);
		PStart(3);
		try{
		#endif
		deltaMBinQuantile(deltaT, deltaM, binEdges, change50, 0.50);
		#if USELFP
		PEnd(3);
		} catch (std::runtime_error &e) {
			PEnd(3);
			throw;
		}
		#endif
		dmdtMedianTimes.push_back(binEdges);
		dmdtMedians    .push_back(change50);

		#if USELFP
		PClear(4);
		PStart(4);
		try{
		#endif
		deltaMBinQuantile(deltaT, deltaM, binEdges, change90, 0.90);
		#if USELFP
		PEnd(4);
		} catch (std::runtime_error &e) {
			PEnd(4);
			throw;
		}
		#endif
	
//		periods.push_back(truePeriod);

		// Key cuts
		cut50Amp3s.push_back(cutFunction(binEdges, change50, amplitude / 3.0));
		cut50Amp2s.push_back(cutFunction(binEdges, change50, amplitude / 2.0));
	
		cut90Amp3s.push_back(cutFunction(binEdges, change90, amplitude / 3.0));
		cut90Amp2s.push_back(cutFunction(binEdges, change90, amplitude / 2.0));
	} catch (std::runtime_error &e) {
		// Don't add any dmdt stats; move on
	}
}

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

/** Deletes all the simulation results from the object. 
 * 
 * The object reverts to its initial state after being constructed.
 */
void LcBinStats::clear() {
	C1vals.clear();
}

/** Prints a row representing the accumulated statistics to the specified file.
 *
 * Each row has the light curve type, followed by the parameters, followed by 
 * the statistics. The row is in tab-delimited format, with statistics 
 * separated from their errors by the � sign. When feeding the log file into 
 * a csv reader, you should give both characters as delimiters.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 */
void LcBinStats::printBinStats(FILE* const file) const {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanC1, stddevC1;
	getSummaryStats(C1vals, meanC1, stddevC1, "C1");
	string    c1File =      "run_c1_" + fileName + ".dat";

	double mean50Rise3, stddev50Rise3, frac50Rise3;
	getSummaryStats(cut50Amp3s, mean50Rise3, stddev50Rise3, frac50Rise3, 
			"50th percentile crossing 1/3 amp");
	string cut50File3 =   "run_cut50_3_" + fileName + ".dat";
	
	double mean50Rise2, stddev50Rise2, frac50Rise2;
	getSummaryStats(cut50Amp2s, mean50Rise2, stddev50Rise2, frac50Rise2, 
			"50th percentile crossing 1/2 amp");
	string cut50File2 =   "run_cut50_2_" + fileName + ".dat";

	double mean90Rise3, stddev90Rise3, frac90Rise3;
	getSummaryStats(cut90Amp3s, mean90Rise3, stddev90Rise3, frac90Rise3, 
			"90th percentile crossing 1/3 amp");
	string cut90File3 =   "run_cut90_3_" + fileName + ".dat";
	
	double mean90Rise2, stddev90Rise2, frac90Rise2;
	getSummaryStats(cut90Amp2s, mean90Rise2, stddev90Rise2, frac90Rise2, 
			"90th percentile crossing 1/2 amp");
	string cut90File2 =   "run_cut90_2_" + fileName + ".dat";

	// Separate file for storing dmdt distributions
	string  dmdtFile = "run_dmdtmed_" + fileName + ".dat";

	fprintf(file, "%s\t%6.3g�%5.2g\t%s\t%6.3g�%5.2g\t%6.3g\t%s\t%6.3g�%5.2g\t%6.3g\t%s\t%6.3g�%5.2g\t%6.3g\t%s\t%6.3g�%5.2g\t%6.3g\t%s\t%s\n",
			binName.c_str(), 
			meanC1, stddevC1, 
			c1File.c_str(), 
			mean50Rise3, stddev50Rise3, frac50Rise3, cut50File3.c_str(), 
			mean50Rise2, stddev50Rise2, frac50Rise2, cut50File2.c_str(), 
			mean90Rise3, stddev90Rise3, frac90Rise3, cut90File3.c_str(), 
			mean90Rise2, stddev90Rise2, frac90Rise2, cut90File2.c_str(), 
			dmdtFile.c_str());

	// Store the C1 distribution
	{
	FILE* auxFile = fopen(c1File.c_str(), "w");
	for(size_t i = 0; i < C1vals.size(); i++) {
		fprintf(auxFile, "%0.3f\n", C1vals[i]);
	}
	fclose(auxFile);
	}

	// Store the 50% cut at 1/3 amp distribution
	{
	FILE* auxFile = fopen(cut50File3.c_str(), "w");
	for(size_t i = 0; i < cut50Amp3s.size(); i++) {
		fprintf(auxFile, "%0.3f\n", cut50Amp3s[i]);
	}
	fclose(auxFile);
	}

	// Store the 50% cut at 1/2 amp distribution
	{
	FILE* auxFile = fopen(cut50File2.c_str(), "w");
	for(size_t i = 0; i < cut50Amp2s.size(); i++) {
		fprintf(auxFile, "%0.3f\n", cut50Amp2s[i]);
	}
	fclose(auxFile);
	}

	// Store the 90% cut at 1/3 amp distribution
	{
	FILE* auxFile = fopen(cut90File3.c_str(), "w");
	for(size_t i = 0; i < cut90Amp3s.size(); i++) {
		fprintf(auxFile, "%0.3f\n", cut90Amp3s[i]);
	}
	fclose(auxFile);
	}

	// Store the 90% cut at 1/2 amp distribution
	{
	FILE* auxFile = fopen(cut90File2.c_str(), "w");
	for(size_t i = 0; i < cut90Amp2s.size(); i++) {
		fprintf(auxFile, "%0.3f\n", cut90Amp2s[i]);
	}
	fclose(auxFile);
	}

	// Store the dmdt medians
	{
	FILE* auxFile = fopen(dmdtFile.c_str(), "w");
	for(size_t i = 0; i < dmdtMedians.size(); i++) {
		for(size_t j = 0; j < dmdtMedianTimes[i].size(); j++) {
			fprintf(auxFile, "%0.3f ", dmdtMedianTimes[i][j]);
		}
		fprintf(auxFile, "\n");
		for(size_t j = 0; j < dmdtMedians[i].size(); j++) {
			fprintf(auxFile, "%0.3f ", dmdtMedians[i][j]);
		}
		fprintf(auxFile, "\n");
	}
	fclose(auxFile);
	}
}

/** Prints a header row representing the statistics printed by 
 *	printBinStats to the specified file
 * 
 * The headers are the light curve type, followed by the parameters, followed 
 * by the statistics. The header is in tab-delimited format.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 * @param[in] binSpecs The properties of the model being tested. Used to 
 *	determine the parameter column headings. The values of the ranges are 
 *	ignored.
 */
void LcBinStats::printBinHeader(FILE* const file, const RangeList& binSpecs) {
	char binLabel[MAX_STRING];

	sprintf(binLabel, "LCType\t");
	for(RangeList::ConstIterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		sprintf(binLabel, "%s\t%-7s", binLabel, (*it).c_str());
	}
	
	sprintf(binLabel, "%s\tNoise", binLabel);

	fprintf(file, "%s\tGrankin C1�err\tC1 Distribution\t50%% cut at 1/3�err\tFinite\t50%%@1/3 Distribution\t50%% cut at 1/2�err\tFinite\t50%%@1/2 Distribution\t90%% cut at 1/3�err\tFinite\t90%%@1/3 Distribution\t90%% cut at 1/2�err\tFinite\t90%%@1/2 Distribution\tDMDT Medians\n", binLabel);
}

/** Creates a unique name for the simulation run, formatted to go in a table.
 * 
 * The name is the light curve type, followed by the parameters, in tab-
 * delimited format.
 * 
 * @param[in] lcName The name of the light curve.
 * @param[in] binSpecs The properties of the model being tested.
 * @param[in] noise The noise level of the model being tested. If white noise 
 *	is being used, this string is a decimal representation of the root-
 *	mean-square noise level. If instead signal injection is being used, 
 *	this string is the name of the light curve library to which signals 
 *	are added.
 *
 * @return The string with which to label this test in a log file.
 */
string LcBinStats::makeBinName(string lcName, const RangeList& binSpecs, string noise) {
	char binId[MAX_STRING];
	
	sprintf(binId, "%-14s", lcName.c_str());
	for(RangeList::ConstIterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		double paramMin = binSpecs.getMin(*it);
		sprintf(binId, "%s\t%0.3g", binId, paramMin);
	}
	sprintf(binId, "%s\t%s", binId, noise.c_str());
	
	return binId;
}

/** Creates a unique name for the simulation run, formatted for use in a file name.
 * 
 * @param[in] lcName The name of the light curve.
 * @param[in] binSpecs The properties of the model being tested.
 * @param[in] noise The noise level of the model being tested. If white noise 
 *	is being used, this string is a decimal representation of the root-
 *	mean-square noise level. If instead signal injection is being used, 
 *	this string is the name of the light curve library to which signals 
 *	are added.
 * 
 * @return The string to use as the base for naming files associated with this 
 * simulation run.
 */
string LcBinStats::makeFileName(string lcName, const RangeList& binSpecs, string noise) {
	char binId[MAX_STRING];

	sprintf(binId, "%s", lcName.c_str());
	for(RangeList::ConstIterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		char shortName = (*it)[0];
		double paramMin = binSpecs.getMin(*it);
		
		sprintf(binId, "%s_%c%+0.1f", binId, shortName, paramMin);
	}
	sprintf(binId, "%s_n%s", binId, noise.c_str());
	
	return binId;
}

}}	// end ::stats
