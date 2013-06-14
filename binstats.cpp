/** Class that calculates test statistics on artificial light curves
 * @file lightcurveMC/binstats.cpp
 * @author Krzysztof Findeisen
 * @date Created June 6, 2011
 * @date Last modified June 8, 2013
 */

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <timescales/timescales.h>
#include "stats/acfinterp.h"
#include "binstats.h"
#include "cerror.h"
#include "stats/cut.tmp.h"
#include "stats/dmdt.h"
#include "stats/experimental.h"
#include "except/fileio.h"
#include "fluxmag.h"
#include "stats/magdist.h"
#include "mcio.h"
#include "stats/output.h"
#include "nan.h"
#include "stats/peakfind.h"
#include "stats/statcollect.h"
#include "stats/statfamilies.h"
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
 * @pre @p toCalc is not empty
 *
 * @exception std::bad_alloc Thrown if not enough memory to construct LcBinStats
 * @exception std::runtime_error Thrown if bin name could not be constructed.
 * @exception std::invalid_argument Thrown if @p toCalc is empty
 *
 * @exceptsafe Object creation is atomic.
 */
LcBinStats::LcBinStats(const string& modelName, const RangeList& binSpecs, const string& noise, 
		const std::vector<StatType>& toCalc) 
		: binName(makeBinName(modelName, binSpecs, noise)), 
		fileName(makeFileName(modelName, binSpecs, noise)), 
		stats(toCalc), 
		C1vals("C1", "run_c1_" + fileName + ".dat"), 
		periods("Period", "run_peri_" + fileName + ".dat"), 
		periodograms("Periodograms", "run_pgram_" + fileName + ".dat"), 
		cutDmdt50Amp3s("50th percentile crossing 1/3 amp", "run_cut50_3_" + fileName + ".dat"), 
		cutDmdt50Amp2s("50th percentile crossing 1/2 amp", "run_cut50_2_" + fileName + ".dat"), 
		cutDmdt90Amp3s("90th percentile crossing 1/3 amp", "run_cut90_3_" + fileName + ".dat"), 
		cutDmdt90Amp2s("90th percentile crossing 1/2 amp", "run_cut90_2_" + fileName + ".dat"), 
		dmdtMedians("DMDT Medians", "run_dmdtmed_" + fileName + ".dat"), 
		cutIAcf9s("ACF crossing 1/9", "run_acf9_" + fileName + ".dat"), 
		cutIAcf4s("ACF crossing 1/4", "run_acf4_" + fileName + ".dat"), 
		cutIAcf2s("ACF crossing 1/2", "run_acf2_" + fileName + ".dat"), 
		iAcfs("ACFs", "run_acf_" + fileName + ".dat"), 
		cutSAcf9s("ACF crossing 1/9", "run_sacf9_" + fileName + ".dat"), 
		cutSAcf4s("ACF crossing 1/4", "run_sacf4_" + fileName + ".dat"), 
		cutSAcf2s("ACF crossing 1/2", "run_sacf2_" + fileName + ".dat"), 
		sAcfs("ACFs", "run_sacf_" + fileName + ".dat"), 
		cutPeakAmp3s("Timescales for peaks > 1/3 amp", "run_cutpeak3_" + fileName + ".dat"), 
		cutPeakAmp2s("Timescales for peaks > 1/3 amp", "run_cutpeak2_" + fileName + ".dat"), 
		cutPeakMax08s("Timescales for peaks > 80% max", "run_cutpeak45_" + fileName + ".dat"), 
		peaks("Peaks", "run_peaks_" + fileName + ".dat") {
	if (toCalc.size() == 0) {
		throw std::invalid_argument("LcBinStats won't calculate any statistics");
	}
}

/** Wrapper for calculating the Scargle ACF.
 * 
 * @param[in] times	Times at which data were taken
 * @param[in] data	The data (typically fluxes or magnitudes) measured 
 *			at each time
 * @param[in] offStep, nOffsets The spacing and number of grid points over which 
 *			the ACF should be calculated. The grid will run from 
 *			lags of 0 to (<tt>nOffsets</tt>-1)*<tt>offStep</tt>.
 * @param[out] acfs	The value of the autocorrelation function at each 
 *			offset.
 *
 * @exceptsafe The program is in a consistent state in the event of 
 *	an exception.
 */
void scargleAdapter(const DoubleVec& times, const DoubleVec& data, 
		double offStep, size_t nOffsets, DoubleVec& acfs) {
	DoubleVec offsets;
	for (size_t i = 0; i < nOffsets; i++) {
		offsets.push_back(i*offStep);
	}
	
	kpftimes::autoCorr(times, data, offsets, acfs);
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
 * @pre @p times.size() = @p fluxes.size()
 * @pre No element of @p times is NaN
 * @pre @p fluxes may contain NaNs
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store more statistics.
 * @exception std::invalid_argument Thrown if @p times and @p fluxes do not 
 *	have matching lengths.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times 
 *	and @p fluxes are too short to calculate the desired statistics.
 *
 * @exceptsafe The object is in a valid state in the event of an exception.
 *
 * @internal @note The implementation *should not* assume that any particular parameter 
 * is defined in trueParams.
 */
void LcBinStats::analyzeLightCurve(const DoubleVec& times, const DoubleVec& fluxes, 
		const ParamList& trueParams) {
	
	if (times.size() != fluxes.size()) {
		throw std::invalid_argument("Times and fluxes must have the same length in analyzeLightCurve() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>(fluxes.size()) + " for fluxes).");
	}

	DoubleVec mags;
	utils::fluxToMag(fluxes, mags);

	DoubleVec cleanTimes, cleanMags;
	utils::removeNans(mags, cleanMags, times, cleanTimes);

	////////////////////////////////////////
	// The statistics
	
	if (hasStat(stats, C1)) {
		try {
			double C1 = getC1(cleanMags);
			C1vals.addStat(C1);
		} catch (const except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (const except::Undefined &e) {
			// Undefined must have been thrown by getC1(), 
			//	so addStat() has not yet been called
			C1vals.addNull();
		}
	}

	doPeriodogram(cleanTimes, cleanMags, hasStat(stats, PERIOD), 
		hasStat(stats, PERIODOGRAM), this->periods, this->periodograms);
	
	doDmdt(cleanTimes, cleanMags, hasStat(stats, DMDTCUT), hasStat(stats, DMDT), 
		this->cutDmdt50Amp3s, this->cutDmdt50Amp2s, 
		this->cutDmdt90Amp3s, this->cutDmdt90Amp2s, 
		this->dmdtMedians);

	doAcf(cleanTimes, cleanMags, interp::autoCorr, 
		hasStat(stats, IACFCUT), hasStat(stats, IACF), 
		this->cutIAcf9s, this->cutIAcf4s, this->cutIAcf2s, this->iAcfs);

	doAcf(cleanTimes, cleanMags, scargleAdapter, 
		hasStat(stats, SACFCUT), hasStat(stats, SACF), 
		this->cutSAcf9s, this->cutSAcf4s, this->cutSAcf2s, this->sAcfs);

	doPeak(cleanTimes, cleanMags, hasStat(stats, PEAKCUT), hasStat(stats, PEAKFIND), 
		this->cutPeakAmp3s, this->cutPeakAmp2s, this->cutPeakMax08s, this->peaks);
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

	periods.clear();
	periodograms.clear();

	cutDmdt50Amp3s.clear();
	cutDmdt50Amp2s.clear();
	cutDmdt90Amp3s.clear();
	cutDmdt90Amp2s.clear();
	dmdtMedians.clear();

	cutIAcf9s.clear();
	cutIAcf4s.clear();
	cutIAcf2s.clear();
	iAcfs.clear();

	cutSAcf9s.clear();
	cutSAcf4s.clear();
	cutSAcf2s.clear();
	sAcfs.clear();
	
	// Peak-finding statistics
	cutPeakAmp3s.clear();
	cutPeakAmp2s.clear();
	cutPeakMax08s.clear();
	peaks.clear();
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
 *	to @p file
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
		cError("Could not print output in printBinStats(): ");
	}

	if (hasStat(stats, C1)) {
		C1vals.printStats(file);
	}
	if (hasStat(stats, PERIOD)) {
		periods.printStats(file);
	}
	if (hasStat(stats, PERIODOGRAM)) {
		periodograms.printStats(file);
	}
	if (hasStat(stats, DMDTCUT)) {
		cutDmdt50Amp3s.printStats(file);
		cutDmdt50Amp2s.printStats(file);
		cutDmdt90Amp3s.printStats(file);
		cutDmdt90Amp2s.printStats(file);
	}
	if (hasStat(stats, DMDT)) {
		dmdtMedians.printStats(file);
	}

	if (hasStat(stats, IACFCUT)) {
		cutIAcf9s.printStats(file);
		cutIAcf4s.printStats(file);
		cutIAcf2s.printStats(file);
	}
	if (hasStat(stats, IACF)) {
		iAcfs.printStats(file);
	}

	if (hasStat(stats, SACFCUT)) {
		cutSAcf9s.printStats(file);
		cutSAcf4s.printStats(file);
		cutSAcf2s.printStats(file);
	}
	if (hasStat(stats, SACF)) {
		sAcfs.printStats(file);
	}

	if (hasStat(stats, PEAKCUT)) {
		cutPeakAmp3s .printStats(file);
		cutPeakAmp2s .printStats(file);
		cutPeakMax08s.printStats(file);
	}
	if (hasStat(stats, PEAKFIND)) {
		peaks.printStats(file);
	}

	status = fprintf(file, "\n");
	if (status < 0) {
		cError("Could not print output in printBinStats(): ");
	}
}

/** Prints a header row representing the statistics printed by 
 *	printBinStats() to the specified file
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

	int status = fprintf(file, "LCType\t");
	if (status < 0) {
		fileError(file, "Header output failed in printBinHeader(): ");
	}
	for(RangeList::const_iterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		status = fprintf(file, "\t%-7s", lexical_cast<string>(*it).c_str());
		if (status < 0) {
			fileError(file, "Header output failed in printBinHeader(): ");
		}
	}
	
	status = fprintf(file, "\tNoise");
	if (status < 0) {
		fileError(file, "Header output failed in printBinHeader(): ");
	}

	if (hasStat(outputStats, C1)) {
		CollectedScalars::printHeader(file, "C1");
	}
	if (hasStat(outputStats, PERIOD)) {
		CollectedScalars::printHeader(file, "Period");
	}
	if (hasStat(outputStats, PERIODOGRAM)) {
		CollectedPairs::printHeader(file, "Periodograms");
	}
	if (hasStat(outputStats, DMDTCUT)) {
		CollectedScalars::printHeader(file, "50%@1/3");
		CollectedScalars::printHeader(file, "50%@1/2");
		CollectedScalars::printHeader(file, "90%@1/3");
		CollectedScalars::printHeader(file, "90%@1/2");
	}
	if (hasStat(outputStats, DMDT)) {
		CollectedPairs::printHeader(file, "DMDT Medians");
	}
	if (hasStat(outputStats, IACFCUT)) {
		CollectedScalars::printHeader(file, "ACF@1/9");
		CollectedScalars::printHeader(file, "ACF@1/4");
		CollectedScalars::printHeader(file, "ACF@1/2");
	}
	if (hasStat(outputStats, IACF)) {
		CollectedPairs::printHeader(file, "ACFs");
	}
	if (hasStat(outputStats, SACFCUT)) {
		CollectedScalars::printHeader(file, "ACF@1/9");
		CollectedScalars::printHeader(file, "ACF@1/4");
		CollectedScalars::printHeader(file, "ACF@1/2");
	}
	if (hasStat(outputStats, SACF)) {
		CollectedPairs::printHeader(file, "ACFs");
	}
	if (hasStat(outputStats, PEAKCUT)) {
		CollectedScalars::printHeader(file, "PeakFind@1/3");
		CollectedScalars::printHeader(file, "PeakFind@1/2");
		CollectedScalars::printHeader(file, "PeakFind@80%");
	}
	if (hasStat(outputStats, PEAKFIND)) {
		CollectedPairs::printHeader(file, "Peaks");
	}

	status = fprintf(file, "\n");
	if (status < 0) {
		fileError(file, "Header output failed in printBinHeader(): ");
	}
}

/** Tests whether this object needs to calculate a particular statistic
 *
 * @param[in] orders A list of statistics to calculate.
 * @param[in] x The statistic to look up
 *
 * @return True if this object needs to calculate @p x, false otherwise.
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
 *
 * @exception std::runtime_error Thrown if the name could not be constructed.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
string LcBinStats::makeBinName(const string& lcName, const RangeList& binSpecs, 
		const string& noise) {
	char binId[MAX_STRING];
	
	int status = sprintf(binId, "%-14s", lcName.c_str());
	if (status < 0) {
		cError("String formatting error in makeBinName(): ");
	}
	for(RangeList::const_iterator it = binSpecs.begin(); 
			it != binSpecs.end(); it++) {
		double paramMin = binSpecs.getMin(*it);
		status = sprintf(binId, "%s\t%0.3g", binId, paramMin);
		if (status < 0) {
			cError("String formatting error in makeBinName(): ");
		}
	}
	status = sprintf(binId, "%s\t%s", binId, noise.c_str());
	if (status < 0) {
		cError("String formatting error in makeBinName(): ");
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
 * @return The string to use as the base for naming files associated with this 
 *	simulation run.
 *
 * @exception std::runtime_error Thrown if the name could not be constructed.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
string LcBinStats::makeFileName(const string& lcName, const RangeList& binSpecs, 
		const string& noise) {
	char binId[MAX_STRING];

	int status = sprintf(binId, "%s", lcName.c_str());
	if (status < 0) {
		cError("String formatting error in makeFileName(): ");
	}
	for(RangeList::const_iterator it = binSpecs.begin(); it != binSpecs.end(); it++) {
		char shortName = lexical_cast<string>(*it)[0];
		double paramMin = binSpecs.getMin(*it);
		
		status = sprintf(binId, "%s_%c%0.2f", binId, shortName, paramMin);
		if (status < 0) {
			cError("String formatting error in makeFileName(): ");
		}
	}
	status = sprintf(binId, "%s_n%s", binId, noise.c_str());
	if (status < 0) {
		cError("String formatting error in makeFileName(): ");
	}
	
	return binId;
}

}}	// end lcmc::stats
