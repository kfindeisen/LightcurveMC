/** Class that calculates test statistics on artificial light curves
 * @file binstats.cpp
 * @author Krzysztof Findeisen
 * @date Created June 6, 2011
 * @date Last modified May 8, 2013
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
#include "stats/acfinterp.h"
#include "stats/dmdt.h"
#include "stats/experimental.h"
#include "stats/magdist.h"

#include "warnflags.h"

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

using interp::autoCorr;

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
LcBinStats::LcBinStats(string modelName, const RangeList& binSpecs, string noise, 
		std::vector<StatType> toCalc) 
		: binName(makeBinName(modelName, binSpecs, noise)), 
		fileName(makeFileName(modelName, binSpecs, noise)), 
		stats(toCalc), 
		C1vals()/*, periods()*/, cut50Amp3s(), cut50Amp2s(), 
		cut90Amp3s(), cut90Amp2s(), 
		dmdtMedianTimes(), dmdtMedians(),
		cutAcf9s(), cutAcf4s(), cutAcf2s() {
}

/** Unary predicate for whether a value is less than some threshold
 */
class LessThan {
public: 
	/** Sets the value to which other values will be compared
	 *
	 * @param[in] threshold The value to use for comparisons.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	LessThan(double threshold) : limit(threshold) {
	}
	
	/** Unary comparison of x
	 *
	 * @param[in] x The value to compare to threshold
	 *
	 * @return True if x < threshold, false otherwise.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	bool operator() (double x) {
		return (x < limit);
	}

private: 
	double limit;
};

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
 * @internal @note The implementation *should not* assume that any particular parameter 
 * is defined in trueParams.
 */
void LcBinStats::analyzeLightCurve(const DoubleVec& times, const DoubleVec& fluxes, 
		const ParamList& trueParams) {
	DoubleVec mags(fluxes.size());
	utils::fluxToMag(fluxes, mags);

	////////////////////////////////////////
	// Calculate c1
	
	if (hasStat(stats, C1)) {
		try {
			double C1 = getC1(mags);
			C1vals.push_back(C1);
		} catch (std::runtime_error &e) {
			// Don't add a C1; move on
		}
	}
	if (hasStat(stats, PERIOD) || hasStat(stats, PERIODOGRAM)) {
	}
	
	////////////////////////////////////////
	// Delta-m Delta-t plots
	if (hasStat(stats, DMDTCUT) || hasStat(stats, DMDT)) {
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
			kpftimes::dmdt(cleanTimes, cleanMags, deltaT, deltaM);
			
			DoubleVec change50;
			deltaMBinQuantile(deltaT, deltaM, binEdges, change50, 0.50);
			if (hasStat(stats, DMDT)) {
				dmdtMedianTimes.push_back(binEdges);
				dmdtMedians    .push_back(change50);
			}
			
			if (hasStat(stats, DMDTCUT)) {
				DoubleVec change90;
				deltaMBinQuantile(deltaT, deltaM, binEdges, change90, 0.90);
				
				// Key cuts
				cut50Amp3s.push_back(cutFunction(binEdges, change50, 
					amplitude / 3.0));
				cut50Amp2s.push_back(cutFunction(binEdges, change50, 
					amplitude / 2.0));
				
				cut90Amp3s.push_back(cutFunction(binEdges, change90, 
					amplitude / 3.0));
				cut90Amp2s.push_back(cutFunction(binEdges, change90, 
					amplitude / 2.0));
			}
		} catch (std::runtime_error &e) {
			// Don't add any dmdt stats; move on
		}
	}

	////////////////////////////////////////
	// ACF plots
	if (hasStat(stats, ACFCUT) || hasStat(stats, ACF)) {
		try {
			const static double offStep = 0.1;
			double maxOffset = kpftimes::deltaT(times);
			DoubleVec offsets;
			for (double t = 0.0; t < maxOffset; t += offStep) {
				offsets.push_back(t);
			}
			
			DoubleVec cleanTimes, cleanMags, acf;
			utils::removeNans(mags, cleanMags, times, cleanTimes);
			autoCorr(cleanTimes, cleanMags, offStep, offsets.size(), acf);
			
			if (hasStat(stats, ACFCUT)) {
				// Key cuts
				cutAcf9s.push_back(cutFunction(offsets, acf, 
					LessThan(1.0/9.0)));
				cutAcf4s.push_back(cutFunction(offsets, acf, 
					LessThan(0.25)   ));
				cutAcf2s.push_back(cutFunction(offsets, acf, 
					LessThan(0.5)    ));
			}
		} catch (std::runtime_error &e) {
			// Don't add any acf stats; move on
		}
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

//	periods.clear();
	cut50Amp3s.clear();
	cut50Amp2s.clear();
	cut90Amp3s.clear();
	cut90Amp2s.clear();
	dmdtMedianTimes.clear();
	dmdtMedians.clear();

	cutAcf9s.clear();
	cutAcf4s.clear();
	cutAcf2s.clear();
}

/** Prints a row representing the accumulated statistics to the specified file.
 *
 * Each row has the light curve type, followed by the parameters, followed by 
 * the statistics. The row is in tab-delimited format, with statistics 
 * separated from their errors by the ± sign. When feeding the log file into 
 * a csv reader, you should give both characters as delimiters.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 */
void LcBinStats::printBinStats(FILE* const file) const {
	// Start with the bin ID
	fprintf(file, "%s", binName.c_str());

	if (hasStat(stats, C1)) {
		// For now, define summary variables for each statistic
		// Eventually statistics will be objects, with mean, filename, etc. 
		//	accessible as members
		double meanC1, stddevC1;
		getSummaryStats(C1vals, meanC1, stddevC1, "C1");
		string    c1File =      "run_c1_" + fileName + ".dat";
		
		fprintf(file, "\t%6.3g±%5.2g\t%s", meanC1, stddevC1, c1File.c_str());
	
		// Store the C1 distribution
		{
			FILE* auxFile = fopen(c1File.c_str(), "w");
			for(size_t i = 0; i < C1vals.size(); i++) {
				fprintf(auxFile, "%0.3f\n", C1vals[i]);
			}
			fclose(auxFile);
		}
	}
	if (hasStat(stats, PERIOD)) {
	}
	if (hasStat(stats, PERIODOGRAM)) {
	}
	if (hasStat(stats, DMDTCUT)) {
		// For now, define summary variables for each statistic
		// Eventually statistics will be objects, with mean, filename, etc. 
		//	accessible as members
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
		
		fprintf(file, "\t%6.3g±%5.2g\t%6.3g\t%s\t%6.3g±%5.2g\t%6.3g\t%s\t%6.3g±%5.2g\t%6.3g\t%s\t%6.3g±%5.2g\t%6.3g\t%s",
				mean50Rise3, stddev50Rise3, frac50Rise3, cut50File3.c_str(), 
				mean50Rise2, stddev50Rise2, frac50Rise2, cut50File2.c_str(), 
				mean90Rise3, stddev90Rise3, frac90Rise3, cut90File3.c_str(), 
				mean90Rise2, stddev90Rise2, frac90Rise2, cut90File2.c_str());

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
	}
	if (hasStat(stats, DMDT)) {
		string  dmdtFile = "run_dmdtmed_" + fileName + ".dat";
		
		fprintf(file, "\t%s", dmdtFile.c_str());

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
	if (hasStat(stats, ACFCUT)) {
		// For now, define summary variables for each statistic
		// Eventually statistics will be objects, with mean, filename, etc. 
		//	accessible as members
		double meanAcf9, stddevAcf9, fracAcf9;
		getSummaryStats(cutAcf9s, meanAcf9, stddevAcf9, fracAcf9, 
				"ACF crossing 1/9");
		string cutAcf9 =   "run_acf9_" + fileName + ".dat";
		
		double meanAcf4, stddevAcf4, fracAcf4;
		getSummaryStats(cutAcf4s, meanAcf4, stddevAcf4, fracAcf4, 
				"ACF crossing 1/4");
		string cutAcf4 =   "run_acf4_" + fileName + ".dat";
		
		double meanAcf2, stddevAcf2, fracAcf2;
		getSummaryStats(cutAcf2s, meanAcf2, stddevAcf2, fracAcf2, 
				"ACF crossing 1/2");
		string cutAcf2 =   "run_acf2_" + fileName + ".dat";
		
		
		fprintf(file, "\t%6.3g±%5.2g\t%6.3g\t%s\t%6.3g±%5.2g\t%6.3g\t%s\t%6.3g±%5.2g\t%6.3g\t%s",
				meanAcf9, stddevAcf9, fracAcf9, cutAcf9.c_str(), 
				meanAcf4, stddevAcf4, fracAcf4, cutAcf4.c_str(), 
				meanAcf2, stddevAcf2, fracAcf2, cutAcf2.c_str());

		// Store the ACF cut at 1/9 distribution
		{
			FILE* auxFile = fopen(cutAcf9.c_str(), "w");
			for(size_t i = 0; i < cutAcf9.size(); i++) {
				fprintf(auxFile, "%0.3f\n", cutAcf9s[i]);
			}
			fclose(auxFile);
		}
		
		// Store the ACF cut at 1/4 distribution
		{
			FILE* auxFile = fopen(cutAcf4.c_str(), "w");
			for(size_t i = 0; i < cutAcf4.size(); i++) {
				fprintf(auxFile, "%0.3f\n", cutAcf4s[i]);
			}
			fclose(auxFile);
		}
		
		// Store the ACF cut at 1/2 distribution
		{
			FILE* auxFile = fopen(cutAcf2.c_str(), "w");
			for(size_t i = 0; i < cutAcf2.size(); i++) {
				fprintf(auxFile, "%0.3f\n", cutAcf2s[i]);
			}
			fclose(auxFile);
		}
		
	}
	if (hasStat(stats, ACF)) {
	}

	fprintf(file, "\n");
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
void LcBinStats::printBinHeader(FILE* const file, const RangeList& binSpecs, 
		const std::vector<StatType>& outputStats) {
	char binLabel[MAX_STRING];

	sprintf(binLabel, "LCType\t");
	for(RangeList::ConstIterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		sprintf(binLabel, "%s\t%-7s", binLabel, (*it).c_str());
	}
	
	sprintf(binLabel, "%s\tNoise", binLabel);

	if (hasStat(outputStats, C1)) {
		sprintf(binLabel, "%s\tGrankin C1±err\tC1 Distribution", binLabel);
	}
	if (hasStat(outputStats, PERIOD)) {
	}
	if (hasStat(outputStats, PERIODOGRAM)) {
	}
	if (hasStat(outputStats, DMDTCUT)) {
		sprintf(binLabel, "%s\t50%% cut at 1/3±err\tFinite\t50%%@1/3 Distribution\t50%% cut at 1/2±err\tFinite\t50%%@1/2 Distribution\t90%% cut at 1/3±err\tFinite\t90%%@1/3 Distribution\t90%% cut at 1/2±err\tFinite\t90%%@1/2 Distribution", binLabel);
	}
	if (hasStat(outputStats, DMDT)) {
		sprintf(binLabel, "%s\tDMDT Medians", binLabel);
	}
	if (hasStat(outputStats, ACFCUT)) {
		sprintf(binLabel, "%s\tACF cut at 1/9±err\tFinite\tACF@1/3 Distribution\tACF cut at 1/4±err\tFinite\tACF@1/4 Distribution\tACF cut at 1/2±err\tFinite\tACF@1/2 Distribution", binLabel);
	}
	if (hasStat(outputStats, ACF)) {
	}

	fprintf(file, "%s\n", binLabel);
}

/** Tests whether this object needs to calculate a particular statistic
 *
 * @param[in] orders A list of statistics to calculate.
 * @param[in] x The statistic to look up
 *
 * @return True if this object needs to calculate x, false otherwise.
 *
 * @exceptsafe Does not throw exceptions.
 */
bool LcBinStats::hasStat(const std::vector<StatType>& orders, StatType x) {
	return (std::find(orders.begin(), orders.end(), x) != orders.end());
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

/** Returns a list of all statistic names recognized on the command line. 
 * 
 * @return A list of string identifiers, one for each allowed StatType 
 *	value. The order of the strings may be whatever is most convenient for 
 *	enumerating the types of statistics.
 */
const std::vector<string> statTypes() {
	std::vector<string> theList;
	
	theList.push_back("C1");
	theList.push_back("period");
	theList.push_back("pgram");
	theList.push_back("dmdtcut");
	theList.push_back("dmdtplot");
	theList.push_back("acfcut");
	theList.push_back("acfplot");
	
	return theList;
}

/** Converts a string to its associated StatType.
 *
 * @param[in] statName A short string describing the type of statistic to calculate.
 *
 * @pre statName is an element of statTypes()
 * 
 * @return The statistic identifier, if one exists.
 * 
 * @exception domain_error Thrown if the statistic name does not have a 
 *	corresponding type.
 */
StatType parseStat(const string& statName) {
	if(statName == "C1") {
		return C1;
	} else if (statName == "period") {
		return PERIOD;
	} else if (statName == "pgram") {
		return PERIODOGRAM;
	} else if (statName == "dmdtcut") {
		return DMDTCUT;
	} else if (statName == "dmdtplot") {
		return DMDT;
	} else if (statName == "acfcut") {
		return ACFCUT;
	} else if (statName == "acfplot") {
		return ACF;
	} else {
		throw std::domain_error("Unknown statistic: " + statName);
	}
}

}}	// end lcmc::stats
