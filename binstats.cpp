/** Class that calculates test statistics on artificial light curves
 * @file binstats.cpp
 * @author Krzysztof Findeisen
 * @date Created June 6, 2011
 * @date Last modified May 9, 2013
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
#include "stats/cut.tmp.h"
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
 */
void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		const string& statName);

/** Calculates the mean, standard deviation, and definition rate of a collection of statistics
 */
void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		double& goodFrac, const string& statName);

/** Prints a single family of statistics to the specified file
 */
void printStat(FILE* const file, const DoubleVec& archive, 
	const std::string& statName, const std::string& distribFile);

/** Prints a single family of statistics to the specified file
 */
void printStatAlwaysDefined(FILE* const file, const DoubleVec& archive, 
	const std::string& statName, const std::string& distribFile);

/** Prints a set of functions to the specified file
 */
void printStat(FILE* const file, const std::vector<DoubleVec>& timeArchive, 
	const std::vector<DoubleVec>& statArchive, const std::string& distribFile);

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
 * @param[in] toCalc A list of statistics whose values will be calculated.
 *
 * @pre toCalc is not empty
 */
LcBinStats::LcBinStats(string modelName, const RangeList& binSpecs, string noise, 
		std::vector<StatType> toCalc) 
		: binName(makeBinName(modelName, binSpecs, noise)), 
		fileName(makeFileName(modelName, binSpecs, noise)), 
		stats(toCalc), 
		C1vals()/*, periods()*/, cut50Amp3s(), cut50Amp2s(), 
		cut90Amp3s(), cut90Amp2s(), 
		dmdtMedianTimes(), dmdtMedians(),
		cutAcf9s(), cutAcf4s(), cutAcf2s(), 
		acfTimes(), acfs() {
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
					MoreThan(amplitude / 3.0) ));
				cut50Amp2s.push_back(cutFunction(binEdges, change50, 
					MoreThan(amplitude / 2.0) ));
				
				cut90Amp3s.push_back(cutFunction(binEdges, change90, 
					MoreThan(amplitude / 3.0) ));
				cut90Amp2s.push_back(cutFunction(binEdges, change90, 
					MoreThan(amplitude / 2.0) ));
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
			if (hasStat(stats, ACF)) {
				acfTimes.push_back(offsets);
				acfs    .push_back(acf);
			}
			
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
	acfTimes.clear();
	acfs.clear();
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
		printStatAlwaysDefined(file, C1vals, "C1", "run_c1_" + fileName + ".dat");
	}
	if (hasStat(stats, PERIOD)) {
	}
	if (hasStat(stats, PERIODOGRAM)) {
	}
	if (hasStat(stats, DMDTCUT)) {
		printStat(file, cut50Amp3s, "50th percentile crossing 1/3 amp", 
			"run_cut50_3_" + fileName + ".dat");
		printStat(file, cut50Amp2s, "50th percentile crossing 1/2 amp", 
			"run_cut50_2_" + fileName + ".dat");
		printStat(file, cut90Amp3s, "90th percentile crossing 1/3 amp", 
			"run_cut90_3_" + fileName + ".dat");
		printStat(file, cut90Amp2s, "90th percentile crossing 1/2 amp", 
			"run_cut90_2_" + fileName + ".dat");
	}
	if (hasStat(stats, DMDT)) {
		printStat(file, dmdtMedianTimes, dmdtMedians, 
			"run_dmdtmed_" + fileName + ".dat");
	}
	if (hasStat(stats, ACFCUT)) {
		printStat(file, cutAcf9s, "ACF crossing 1/9", "run_acf9_" + fileName + ".dat");
		printStat(file, cutAcf4s, "ACF crossing 1/4", "run_acf4_" + fileName + ".dat");
		printStat(file, cutAcf2s, "ACF crossing 1/2", "run_acf2_" + fileName + ".dat");
	}
	if (hasStat(stats, ACF)) {
		printStat(file, acfTimes, acfs, "run_acf_" + fileName + ".dat");
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
 * @param[in] outputStats A list of statistics whose headers need to be included.
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
		sprintf(binLabel, "%s\tACFs", binLabel);
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

/** Prints a single family of statistics to the specified file
 *
 * The function will print, in order: the mean of the statistic, the 
 * standard deviation of the statistic, the fraction of times each 
 * statistic was defined, and the name of a file containing the 
 * distribution of the statistics. The row is in tab-delimited 
 * format, except that the mean and standard deviation are separated by 
 * a ± sign for improved readability.
 *
 * This function differs from printStatAlwaysDefined() only in that the 
 * latter does not print the number of times each statistic had a value.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 * @param[in] archive The statistics to summarize.
 * @param[in] statName The name of the statistic to use for error messages.
 * @param[in] distribFile The prefix identifying the distribution file as 
 *	being for this particular statistic.
 */
void printStat(FILE* const file, const DoubleVec& archive, 
		const std::string& statName, const std::string& distribFile) {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanStats, stddevStats, fracStats;
	getSummaryStats(archive, meanStats, stddevStats, fracStats, 
			statName);
	
	fprintf(file, "\t%6.3g±%5.2g\t%6.3g\t%s",
			meanStats, stddevStats, fracStats, distribFile.c_str() );

	FILE* auxFile = fopen(distribFile.c_str(), "w");
	for(size_t i = 0; i < archive.size(); i++) {
		fprintf(auxFile, "%0.3f\n", archive[i]);
	}
	fclose(auxFile);
}

/** Prints a single family of statistics to the specified file
 *
 * The function will print, in order: the mean of the statistic, the 
 * standard deviation of the statistic, and the name of a file 
 * containing the distribution of the statistics. The row is in tab-delimited 
 * format, except that the mean and standard deviation are separated by 
 * a ± sign for improved readability.
 * 
 * This function differs from printStat() only in that the 
 * latter also prints the number of times each statistic had a value.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 * @param[in] archive The statistics to summarize.
 * @param[in] statName The name of the statistic to use for error messages.
 * @param[in] distribFile The prefix identifying the distribution file as 
 *	being for this particular statistic.
 */
void printStatAlwaysDefined(FILE* const file, const DoubleVec& archive, 
		const std::string& statName, const std::string& distribFile) {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanStats, stddevStats;
	getSummaryStats(archive, meanStats, stddevStats, statName);
	
	fprintf(file, "\t%6.3g±%5.2g\t%s",
			meanStats, stddevStats, distribFile.c_str() );

	FILE* auxFile = fopen(distribFile.c_str(), "w");
	for(size_t i = 0; i < archive.size(); i++) {
		fprintf(auxFile, "%0.3f\n", archive[i]);
	}
	fclose(auxFile);
}

/** Prints a set of functions to the specified file
 *
 * The function will print only the name of a file containing the 
 * distribution of the functions. The row is in tab-delimited 
 * format.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 * @param[in] timeArchive The times of the functions to print.
 * @param[in] statArchive The values of the functions to print.
 * @param[in] distribFile The prefix identifying the distribution file as 
 *	being for this particular statistic.
 */
void printStat(FILE* const file, const std::vector<DoubleVec>& timeArchive, 
		const std::vector<DoubleVec>& statArchive, 
		const std::string& distribFile) {
	
	fprintf(file, "\t%s", distribFile.c_str());

	FILE* auxFile = fopen(distribFile.c_str(), "w");
	for(size_t i = 0; i < statArchive.size(); i++) {
		for(size_t j = 0; j < timeArchive[i].size(); j++) {
			fprintf(auxFile, "%0.3f ", timeArchive[i][j]);
		}
		fprintf(auxFile, "\n");
		for(size_t j = 0; j < statArchive[i].size(); j++) {
			fprintf(auxFile, "%0.3f ", statArchive[i][j]);
		}
		fprintf(auxFile, "\n");
	}
	fclose(auxFile);
}

}}	// end lcmc::stats
