/** Class for organizing test statistics on artificial light curves
 * @file binstats.h
 * @author Krzysztof Findeisen
 * @date Reconstructed June 23, 2011
 * @date Last modified May 9, 2013
 */

#ifndef BINSTATSH
#define BINSTATSH

#include <string>
#include <vector>
#include <cstdio>
#include "paramlist.h"

namespace lcmc { 

/** This namespace identifies data types and functions that handle 
 * data analysis on simulated light curves.
 */
namespace stats {

using models::RangeList;
using models::ParamList;

/** Shorthand for a vector of doubles.
 */
typedef std::vector<double> DoubleVec;

/** Type used to tell the program which statistics to calculate
 */
enum StatType {
	/** Represents the C1 statistic
	 */
	C1, 
	/** Represents the period
	 */
	PERIOD, 
	/** Represents dumps of periodograms
	 */
	PERIODOGRAM, 
	/** Represents cuts through &Delta;m&Delta;t plots
	 */
	DMDTCUT, 
	/** Represents dumps of &Delta;m&Delta;t plots
	 */
	DMDT, 
	/** Represents cuts through ACF plots
	 */
	ACFCUT, 
	/** Represents dumps of ACF plots
	 */
	ACF
};

/** Organizes test statistics on artificial light curves. LcBinStats can 
 * collate the results from multiple runs and print summary statistics to log 
 * files.
 *
 * The following statistics are supported:
 * - C1
 * - Period [disabled]
 * - &Delta;m-&Delta;t-based timescales
 * - ACF-based timescales
 *
 * @todo May be good idea to redesign in terms of strategy pattern, to allow 
 * for more flexibility in which tests get run.
 */
class LcBinStats {
public: 
	/** Creates a new stat counter
	 */
	explicit LcBinStats(std::string modelName, const RangeList& binSpecs, 
			std::string noise, std::vector<StatType> toCalc);

	/** Calculates statistics from the light curve and records them in lcBinStats.
	 */
	void analyzeLightCurve(const DoubleVec& times, const DoubleVec& fluxes, 
		const ParamList& trueParams);

	/** Deletes all the simulation results from the object.
	 */
	void clear();

	/** Prints a row representing the accumulated statistics to the specified file
	 */
	void printBinStats(FILE* const file) const;

	/** Creates a unique name for the simulation run, formatted to go in a table.
	 */
	static std::string makeBinName(std::string lcName, 
		const RangeList& binSpecs, std::string noise);

	/** Creates a unique name for the simulation run, formatted for use in a file name.
	 */
	static std::string makeFileName(std::string lcName, 
		const RangeList& binSpecs, std::string noise);

	/** Prints a header row representing the statistics printed by 
	 *	printBinStats to the specified file
	 */
	static void printBinHeader(FILE* const file, const RangeList& binSpecs, 
		const std::vector<StatType>& outputStats);

private: 
	/** Tests whether the object needs to calculate a particular statistic
	 */
	static bool hasStat(const std::vector<StatType>& orders, StatType x);
	
	std::string binName;
	std::string fileName;
	
	std::vector<StatType> stats;
	
	DoubleVec C1vals;
	
//	DoubleVec periods;
	DoubleVec cut50Amp3s;
	DoubleVec cut50Amp2s;
	DoubleVec cut90Amp3s;
	DoubleVec cut90Amp2s;
	std::vector<DoubleVec> dmdtMedianTimes;
	std::vector<DoubleVec> dmdtMedians;
	
	DoubleVec cutAcf9s;
	DoubleVec cutAcf4s;
	DoubleVec cutAcf2s;
	std::vector<DoubleVec> acfTimes;
	std::vector<DoubleVec> acfs;
};

}}		// end lcmc::stats

#endif		// End ifndef BINSTATSH
