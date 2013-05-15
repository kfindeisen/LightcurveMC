/** Class that calculates test statistics on artificial light curves
 * @file lightcurveMC/binstats.cpp
 * @author Krzysztof Findeisen
 * @date Created June 6, 2011
 * @date Last modified May 15, 2013
 */

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <timescales/timescales.h>
#include "stats/acfinterp.h"
#include "binstats.h"
#include "stats/cut.tmp.h"
#include "stats/dmdt.h"
#include "stats/experimental.h"
#include "except/fileio.h"
#include "fluxmag.h"
#include "stats/magdist.h"
#include "nan.h"
#include "stats/peakfind.h"
#include "except/undefined.h"

#include "warnflags.h"

/** Size of character buffers to use when generating strings
 */
#define MAX_STRING 1024

// Current implementation of analyzeLightCurve does not use 
// trueParams, but it should still be part of the interface
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace lcmc { namespace stats {

using boost::lexical_cast;
using boost::shared_ptr;
using std::string;
using lcmc::models::RangeList;
using lcmc::models::ParamList;

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
 *
 * @exception std::bad_alloc Thrown if not enough memory to construct LcBinStats
 * @exception std::runtime_error Thrown if bin name could not be constructed.
 * @exception std::invalid_argument Thrown if toCalc is empty
 *
 * @exceptsafe Object creation is atomic.
 */
LcBinStats::LcBinStats(const string& modelName, const RangeList& binSpecs, const string& noise, 
		std::vector<StatType> toCalc) 
		: binName(makeBinName(modelName, binSpecs, noise)), 
		fileName(makeFileName(modelName, binSpecs, noise)), 
		stats(toCalc), 
		C1vals()/*, periods()*/, 
		cutDmdt50Amp3s(), cutDmdt50Amp2s(), cutDmdt90Amp3s(), cutDmdt90Amp2s(), 
		dmdtMedianTimes(), dmdtMedians(),
		cutIAcf9s(), cutIAcf4s(), cutIAcf2s(), 
		iAcfTimes(), iAcfs(), 
		cutPeakAmp3s(), cutPeakAmp2s(), 
		peakTimes(), peakValues() {
	if (stats.size() == 0) {
		throw std::invalid_argument("LcBinStats won't calculate any statistics");
	}
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
 * @pre times.size() == fluxes.size()
 * @pre No element of times is NaN
 * @pre Fluxes may contain NaNs
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store more statistics.
 * @exception std::invalid_argument Thrown if times and fluxes do not 
 *	have matching lengths.
 * @exception lcmc::stats::except::NotEnoughData Thrown if times and fluxes 
 *	are too short to calculate the desired statistics.
 *
 * @exceptsafe The object is in a valid state in the event of an exception.
 *
 * @internal @note The implementation *should not* assume that any particular parameter 
 * is defined in trueParams.
 */
void LcBinStats::analyzeLightCurve(const DoubleVec& times, const DoubleVec& fluxes, 
		const ParamList& trueParams) {
	using interp::autoCorr;
	
	if (times.size() != fluxes.size()) {
		throw std::invalid_argument("Times and fluxes must have the same length in analyzeLightCurve() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>(fluxes.size()) + " for fluxes).");
	}

	DoubleVec mags;
	utils::fluxToMag(fluxes, mags);

	////////////////////////////////////////
	// Calculate c1
	
	if (hasStat(stats, C1)) {
		try {
			double C1 = getC1(mags);
			C1vals.push_back(C1);
		} catch (except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (except::Undefined &e) {
			// Don't add a C1; move on
		}
	}

	////////////////////////////////////////
	// Periodogram analysis
	if (hasStat(stats, PERIOD) || hasStat(stats, PERIODOGRAM)) {
	}
	
	////////////////////////////////////////
	// Delta-m Delta-t plots
	if (hasStat(stats, DMDTCUT) || hasStat(stats, DMDT)) {
		try {
			double amplitude = getAmplitude(mags);
			
			if (amplitude > 0) {
				double minBin = -1.97;
				double maxBin = log10(kpftimes:: deltaT(times));
				
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
					// Since vector::reserve() has the strong guarantee 
					// and doesn't change the data in any case, it's 
					// effectively a function that is guaranteed to not 
					// change the content of a vector. 
					// Use it to ensure that either vector is changed 
					// only if no exception is thrown
					dmdtMedianTimes.reserve(dmdtMedianTimes.size() + 1);
					dmdtMedians    .reserve(dmdtMedians    .size() + 1);
					dmdtMedianTimes.push_back(binEdges);
					dmdtMedians    .push_back(change50);
				}
				
				if (hasStat(stats, DMDTCUT)) {
					DoubleVec change90;
					deltaMBinQuantile(deltaT, deltaM, binEdges, change90, 0.90);
					
					// Key cuts
					cutDmdt50Amp3s.push_back(cutFunction(binEdges, 
						change50, MoreThan(amplitude / 3.0) ));
					cutDmdt50Amp2s.push_back(cutFunction(binEdges, 
						change50, MoreThan(amplitude / 2.0) ));
					
					cutDmdt90Amp3s.push_back(cutFunction(binEdges, 
						change90, MoreThan(amplitude / 3.0) ));
					cutDmdt90Amp2s.push_back(cutFunction(binEdges, 
						change90, MoreThan(amplitude / 2.0) ));
				}
			}
		} catch (except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (except::Undefined &e) {
			// Don't add any dmdt stats; move on
		}
	}

	////////////////////////////////////////
	// ACF plots
	if (hasStat(stats, IACFCUT) || hasStat(stats, IACF)) {
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
			if (hasStat(stats, IACF)) {
				// Since vector::reserve() has the strong guarantee 
				// and doesn't change the data in any case, it's 
				// effectively a function that is guaranteed to not 
				// change the content of a vector. 
				// Use it to ensure that either vector is changed 
				// only if no exception is thrown
				iAcfTimes.reserve(iAcfTimes.size() + 1);
				iAcfs    .reserve(iAcfs    .size() + 1);
				iAcfTimes.push_back(offsets);
				iAcfs    .push_back(acf);
			}
			
			if (hasStat(stats, IACFCUT)) {
				// Key cuts
				cutIAcf9s.push_back(cutFunction(offsets, acf, 
					LessThan(1.0/9.0)));
				cutIAcf4s.push_back(cutFunction(offsets, acf, 
					LessThan(0.25)   ));
				cutIAcf2s.push_back(cutFunction(offsets, acf, 
					LessThan(0.5)    ));
			}
		} catch (except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (except::Undefined &e) {
			// Don't add any acf stats; move on
		}
	}

	////////////////////////////////////////
	// Peak-finding plots
	if (hasStat(stats, PEAKCUT)) {
		try {
			double amplitude = getAmplitude(mags);
			
			if (amplitude > 0) {
				DoubleVec magCuts;
				magCuts.push_back(amplitude / 3.0);
				magCuts.push_back(amplitude / 2.0);
			
				DoubleVec cleanTimes, cleanMags, cutTimes;
				utils::removeNans(mags, cleanMags, times, cleanTimes);
				peakFindTimescales(cleanTimes, cleanMags, magCuts, cutTimes);
				
				// Key cuts
				cutPeakAmp3s.push_back(cutTimes[0]);
				cutPeakAmp2s.push_back(cutTimes[1]);
			}
		} catch (except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (except::Undefined &e) {
			// Don't add any acf stats; move on
		}
	}
	if (hasStat(stats, PEAKFIND)) {
		try {
			double amplitude = getAmplitude(mags);
			
			if (amplitude > 0) {
				double minMag = 0.01;
				
				DoubleVec magCuts;
				for (double mag = minMag; mag < amplitude; mag += minMag) {
					magCuts.push_back(mag);
				}
				
				DoubleVec cleanTimes, cleanMags, cutTimes;
				utils::removeNans(mags, cleanMags, times, cleanTimes);
				peakFindTimescales(cleanTimes, cleanMags, magCuts, cutTimes);
				
				// Since vector::reserve() has the strong guarantee 
				// and doesn't change the data in any case, it's 
				// effectively a function that is guaranteed to not 
				// change the content of a vector. 
				// Use it to ensure that either vector is changed 
				// only if no exception is thrown
				peakTimes .reserve(peakTimes .size() + 1);
				peakValues.reserve(peakValues.size() + 1);
				peakTimes .push_back(magCuts);
				peakValues.push_back(cutTimes);
			}
		} catch (except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (except::Undefined &e) {
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
 *
 * @exceptsafe Does not throw exceptions.
 */
void LcBinStats::clear() {
	C1vals.clear();

//	periods.clear();

	cutDmdt50Amp3s.clear();
	cutDmdt50Amp2s.clear();
	cutDmdt90Amp3s.clear();
	cutDmdt90Amp2s.clear();
	dmdtMedianTimes.clear();
	dmdtMedians.clear();

	cutIAcf9s.clear();
	cutIAcf4s.clear();
	cutIAcf2s.clear();
	iAcfTimes.clear();
	iAcfs.clear();
	
	// Peak-finding statistics
	cutPeakAmp3s.clear();
	cutPeakAmp2s.clear();
	peakTimes.clear();
	peakValues.clear();
}

/** Prints a row representing the accumulated statistics to the specified file.
 *
 * Each row has the light curve type, followed by the parameters, followed by 
 * the statistics. The row is in tab-delimited format, with statistics 
 * separated from their errors by the ± sign. When feeding the log file into 
 * a csv reader, you should give both characters as delimiters.
 * 
 * @param[in] file An open file handle representing the text file to write to.
 *
 * @exception std::runtime_error Thrown if there are difficulties writing 
 *	to file
 * @exception lcmc::except::FileIo Thrown if there are difficulties writing 
 *	to an auxiliary file
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void LcBinStats::printBinStats(FILE* const file) const {
	// Start with the bin ID
	int status = fprintf(file, "%s", binName.c_str());
	if (status < 0) {
		throw std::runtime_error("Could not print output in printBinStats(): " 
			+ string(strerror(errno)));
	}

	if (hasStat(stats, C1)) {
		printStatAlwaysDefined(file, C1vals, "C1", "run_c1_" + fileName + ".dat");
	}
	if (hasStat(stats, PERIOD)) {
	}
	if (hasStat(stats, PERIODOGRAM)) {
	}
	if (hasStat(stats, DMDTCUT)) {
		printStat(file, cutDmdt50Amp3s, "50th percentile crossing 1/3 amp", 
			"run_cut50_3_" + fileName + ".dat");
		printStat(file, cutDmdt50Amp2s, "50th percentile crossing 1/2 amp", 
			"run_cut50_2_" + fileName + ".dat");
		printStat(file, cutDmdt90Amp3s, "90th percentile crossing 1/3 amp", 
			"run_cut90_3_" + fileName + ".dat");
		printStat(file, cutDmdt90Amp2s, "90th percentile crossing 1/2 amp", 
			"run_cut90_2_" + fileName + ".dat");
	}
	if (hasStat(stats, DMDT)) {
		printStat(file, dmdtMedianTimes, dmdtMedians, 
			"run_dmdtmed_" + fileName + ".dat");
	}
	if (hasStat(stats, IACFCUT)) {
		printStat(file, cutIAcf9s, "ACF crossing 1/9", "run_acf9_" + fileName + ".dat");
		printStat(file, cutIAcf4s, "ACF crossing 1/4", "run_acf4_" + fileName + ".dat");
		printStat(file, cutIAcf2s, "ACF crossing 1/2", "run_acf2_" + fileName + ".dat");
	}
	if (hasStat(stats, IACF)) {
		printStat(file, iAcfTimes, iAcfs, "run_acf_" + fileName + ".dat");
	}

	if (hasStat(stats, PEAKCUT)) {
		printStat(file, cutPeakAmp3s, "Peak timescales crossing 1/3 amp", 
			"run_cutpeak3_" + fileName + ".dat");
		printStat(file, cutPeakAmp2s, "Peak timescales crossing 1/2 amp", 
			"run_cutpeak2_" + fileName + ".dat");
	}
	if (hasStat(stats, PEAKFIND)) {
		printStat(file, peakTimes, peakValues, "run_peaks_" + fileName + ".dat");
	}

	status = fprintf(file, "\n");
	if (status < 0) {
		throw std::runtime_error("Could not print output in printBinStats(): " 
			+ string(strerror(errno)));
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
 * @param[in] outputStats A list of statistics whose headers need to be included.
 *
 * @exception std::runtime_error Thrown if the header could not be constructed.
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void LcBinStats::printBinHeader(FILE* const file, const RangeList& binSpecs, 
		const std::vector<StatType>& outputStats) {
	char binLabel[MAX_STRING];

	int status = sprintf(binLabel, "LCType\t");
	if (status < 0) {
		throw std::runtime_error("String formatting error in printBinHeader(): " 
			+ string(strerror(errno)));
	}
	for(RangeList::const_iterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		status = sprintf(binLabel, "%s\t%-7s", binLabel, (*it).c_str());
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	
	status = sprintf(binLabel, "%s\tNoise", binLabel);
	if (status < 0) {
		throw std::runtime_error("String formatting error in printBinHeader(): " 
			+ string(strerror(errno)));
	}

	if (hasStat(outputStats, C1)) {
		status = sprintf(binLabel, "%s\tGrankin C1±err\tC1 Distribution", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	if (hasStat(outputStats, PERIOD)) {
	}
	if (hasStat(outputStats, PERIODOGRAM)) {
	}
	if (hasStat(outputStats, DMDTCUT)) {
		status = sprintf(binLabel, "%s\t50%% cut at 1/3±err\tFinite\t50%%@1/3 Distribution\t50%% cut at 1/2±err\tFinite\t50%%@1/2 Distribution\t90%% cut at 1/3±err\tFinite\t90%%@1/3 Distribution\t90%% cut at 1/2±err\tFinite\t90%%@1/2 Distribution", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	if (hasStat(outputStats, DMDT)) {
		status = sprintf(binLabel, "%s\tDMDT Medians", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	if (hasStat(outputStats, IACFCUT)) {
		status = sprintf(binLabel, "%s\tACF cut at 1/9±err\tFinite\tACF@1/3 Distribution\tACF cut at 1/4±err\tFinite\tACF@1/4 Distribution\tACF cut at 1/2±err\tFinite\tACF@1/2 Distribution", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	if (hasStat(outputStats, IACF)) {
		status = sprintf(binLabel, "%s\tACFs", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	if (hasStat(outputStats, PEAKCUT)) {
		status = sprintf(binLabel, "%s\tPeakFind cut at 1/3±err\tFinite\tPeakFind@1/3 Distribution\tPeakFind at 1/2±err\tFinite\tPeakFind@1/2 Distribution", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}
	if (hasStat(outputStats, PEAKFIND)) {
		status = sprintf(binLabel, "%s\tPeaks", binLabel);
		if (status < 0) {
			throw std::runtime_error("String formatting error in printBinHeader(): " 
				+ string(strerror(errno)));
		}
	}

	status = fprintf(file, "%s\n", binLabel);
	if (status < 0) {
		throw std::runtime_error("Could not print header in printBinHeader(): " 
			+ string(strerror(errno)));
	}
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
 * @exception std::runtime_error Thrown if the name could not be constructed.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 *
 * @return The string with which to label this test in a log file.
 */
string LcBinStats::makeBinName(const string& lcName, const RangeList& binSpecs, 
		const string& noise) {
	char binId[MAX_STRING];
	
	int status = sprintf(binId, "%-14s", lcName.c_str());
	if (status < 0) {
		throw std::runtime_error("String formatting error in makeBinName(): " 
			+ string(strerror(errno)));
	}
	for(RangeList::const_iterator it = binSpecs.begin(); 
			it != binSpecs.end(); it++) {
		double paramMin = binSpecs.getMin(*it);
		status = sprintf(binId, "%s\t%0.3g", binId, paramMin);
		if (status < 0) {
			throw std::runtime_error("String formatting error in makeBinName(): " 
				+ string(strerror(errno)));
		}
	}
	status = sprintf(binId, "%s\t%s", binId, noise.c_str());
	if (status < 0) {
		throw std::runtime_error("String formatting error in makeBinName(): " 
			+ string(strerror(errno)));
	}
	
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
 * @exception std::runtime_error Thrown if the name could not be constructed.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 * 
 * @return The string to use as the base for naming files associated with this 
 *	simulation run.
 */
string LcBinStats::makeFileName(const string& lcName, const RangeList& binSpecs, 
		const string& noise) {
	char binId[MAX_STRING];

	int status = sprintf(binId, "%s", lcName.c_str());
	if (status < 0) {
		throw std::runtime_error("String formatting error in makeBinName(): " 
			+ string(strerror(errno)));
	}
	for(RangeList::const_iterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		char shortName = (*it)[0];
		double paramMin = binSpecs.getMin(*it);
		
		status = sprintf(binId, "%s_%c%0.2f", binId, shortName, paramMin);
		if (status < 0) {
			throw std::runtime_error("String formatting error in makeBinName(): " 
				+ string(strerror(errno)));
		}
	}
	status = sprintf(binId, "%s_n%s", binId, noise.c_str());
	if (status < 0) {
		throw std::runtime_error("String formatting error in makeBinName(): " 
			+ string(strerror(errno)));
	}
	
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
 *
 * @exceptsafe Does not throw exceptions.
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
	} catch (except::NotEnoughData &e) {
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
 *
 * @exceptsafe Does not throw exceptions.
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
 *
 * @exception std::runtime_error Thrown if there are difficulties writing 
 *	to file
 * @exception lcmc::except::FileIo Thrown if there are difficulties writing 
 *	to distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStat(FILE* const file, const DoubleVec& archive, 
		const std::string& statName, const std::string& distribFile) {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanStats, stddevStats, fracStats;
	getSummaryStats(archive, meanStats, stddevStats, fracStats, 
			statName);
	
	int status = fprintf(file, "\t%6.3g±%5.2g\t%6.3g\t%s",
			meanStats, stddevStats, fracStats, distribFile.c_str() );
	if (status < 0) {
		throw std::runtime_error(string("Could not print statistics in printStat(): ") 
			+ strerror(errno));
	}

	shared_ptr<FILE> auxFile(fopen(distribFile.c_str(), "w"), &fclose);
	if (auxFile.get() == NULL) {
		throw lcmc::except::FileIo("Could not open log file '" + distribFile 
			+ "' in printStat(): " + strerror(errno));
	}
	
	for(size_t i = 0; i < archive.size(); i++) {
		status = fprintf(auxFile.get(), "%0.3f\n", archive[i]);
		if (status < 0) {
			throw lcmc::except::FileIo("Could not write to log file '" + distribFile 
				+ "' in printStat(): " + strerror(errno));
		}
	}
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
 *
 * @exception std::runtime_error Thrown if there are difficulties writing 
 *	to file
 * @exception lcmc::except::FileIo Thrown if there are difficulties writing 
 *	to distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStatAlwaysDefined(FILE* const file, const DoubleVec& archive, 
		const std::string& statName, const std::string& distribFile) {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanStats, stddevStats;
	getSummaryStats(archive, meanStats, stddevStats, statName);
	
	int status = fprintf(file, "\t%6.3g±%5.2g\t%s",
			meanStats, stddevStats, distribFile.c_str() );
	if (status < 0) {
		throw std::runtime_error(string("Could not print statistics in printStatAlwaysDefined(): ") 
			+ strerror(errno));
	}

	shared_ptr<FILE> auxFile(fopen(distribFile.c_str(), "w"), &fclose);
	if (auxFile.get() == NULL) {
		throw lcmc::except::FileIo("Could not open log file '" + distribFile 
			+ "' in printStatAlwaysDefined(): " + strerror(errno));
	}
	
	for(size_t i = 0; i < archive.size(); i++) {
		status = fprintf(auxFile.get(), "%0.3f\n", archive[i]);
		if (status < 0) {
			throw lcmc::except::FileIo("Could not write to log file '" + distribFile 
				+ "' in printStatAlwaysDefined(): " + strerror(errno));
		}
	}
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
 *
 * @exception std::runtime_error Thrown if there are difficulties writing 
 *	to file
 * @exception lcmc::except::FileIo Thrown if there are difficulties writing 
 *	to distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStat(FILE* const file, const std::vector<DoubleVec>& timeArchive, 
		const std::vector<DoubleVec>& statArchive, 
		const std::string& distribFile) {
	
	int status = fprintf(file, "\t%s", distribFile.c_str());
	if (status < 0) {
		throw std::runtime_error(string("Could not print log file name in printStat(): ") 
			+ strerror(errno));
	}

	shared_ptr<FILE> auxFile(fopen(distribFile.c_str(), "w"), &fclose);
	if (auxFile.get() == NULL) {
		throw lcmc::except::FileIo("Could not open log file '" + distribFile 
			+ "' in printStat(): " + strerror(errno));
	}
	
	for(size_t i = 0; i < statArchive.size(); i++) {
		for(size_t j = 0; j < timeArchive[i].size(); j++) {
			status = fprintf(auxFile.get(), "%0.3f ", timeArchive[i][j]);
			if (status < 0) {
				throw lcmc::except::FileIo("Could not write to log file '" 
					+ distribFile + "' in printStat(): " + strerror(errno));
			}
		}
		status = fprintf(auxFile.get(), "\n");
		if (status < 0) {
			throw lcmc::except::FileIo("Could not write to log file '" 
				+ distribFile + "' in printStat(): " + strerror(errno));
		}
		for(size_t j = 0; j < statArchive[i].size(); j++) {
			status = fprintf(auxFile.get(), "%0.3f ", statArchive[i][j]);
			if (status < 0) {
				throw lcmc::except::FileIo("Could not write to log file '" 
					+ distribFile + "' in printStat(): " + strerror(errno));
			}
		}
		status = fprintf(auxFile.get(), "\n");
		if (status < 0) {
			throw lcmc::except::FileIo("Could not write to log file '" 
				+ distribFile + "' in printStat(): " + strerror(errno));
		}
	}
}

}}	// end lcmc::stats
