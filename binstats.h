/** Class for organizing test statistics on artificial light curves
 * @file lightcurveMC/binstats.h
 * @author Krzysztof Findeisen
 * @date Reconstructed June 23, 2011
 * @date Last modified August 5, 2013
 *
 * @todo Break up this file
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

#ifndef BINSTATSH
#define BINSTATSH

#include <string>
#include <vector>
#include <cstdio>
#include "paramlist.h"
#include "stats/statcollect.h"

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
	/** Represents cuts through interpolated ACF plots
	 */
	IACFCUT, 
	/** Represents dumps of interpolated ACF plots
	 */
	IACF, 
	/** Represents cuts through Scargle ACF plots
	 */
	SACFCUT, 
	/** Represents dumps of Scargle ACF plots
	 */
	SACF, 
	/** Represents cuts through peak-find plots
	 */
	PEAKCUT, 
	/** Represents dumps of peak-find plots
	 */
	PEAKFIND, 
	/** Represents the best-fit Gaussian process model
	 */
	GPTAU
};

/** Organizes test statistics on artificial light curves. LcBinStats can 
 * collate the results from multiple runs and print summary statistics to log 
 * files.
 *
 * The following statistics are supported:
 * - C1
 * - Period
 * - &Delta;m-&Delta;t-based timescales
 * - ACF-based timescales
 * - peak-finding timescales
 *
 * @todo May be good idea to redesign in terms of strategy pattern, to allow 
 * for more flexibility in which tests get run.
 */
class LcBinStats {
public: 
	/** Creates a new stat counter
	 */
	explicit LcBinStats(const std::string& modelName, const RangeList& binSpecs, 
			const std::string& noise, const std::vector<StatType>& toCalc);

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
	static std::string makeBinName(const std::string& lcName, 
		const RangeList& binSpecs, const std::string& noise);

	/** Creates a unique name for the simulation run, formatted for use in a file name.
	 */
	static std::string makeFileName(const std::string& lcName, 
		const RangeList& binSpecs, const std::string& noise);

	/** Prints a header row representing the statistics printed by 
	 *	printBinStats() to the specified file
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
	
	//----------------------------------------

	CollectedScalars c1vals;
	
	CollectedScalars periods;
	CollectedPairs periodograms;

	// Delta-m Delta-t statistics
	CollectedScalars cutDmdt50Amp3s;
	CollectedScalars cutDmdt50Amp2s;
	CollectedScalars cutDmdt90Amp3s;
	CollectedScalars cutDmdt90Amp2s;
	CollectedPairs dmdtMedians;
	
	// ACF statistics (in multiple flavors)
	CollectedScalars cutIAcf9s;
	CollectedScalars cutIAcf4s;
	CollectedScalars cutIAcf2s;
	CollectedPairs iAcfs;

	CollectedScalars cutSAcf9s;
	CollectedScalars cutSAcf4s;
	CollectedScalars cutSAcf2s;
	CollectedPairs sAcfs;
	
	// Peak-finding statistics
	CollectedScalars cutPeakAmp3s;
	CollectedScalars cutPeakAmp2s;
	CollectedScalars cutPeakMax08s;
	CollectedPairs peaks;

	CollectedScalars gpTaus;
	CollectedScalars gpErrors;
	CollectedScalars gpChi;
};

}}		// end lcmc::stats

#endif		// End ifndef BINSTATSH
