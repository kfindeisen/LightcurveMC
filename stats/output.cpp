/** Workhorse code for statistics output
 * @file lightcurveMC/stats/output.cpp
 * @author Krzysztof Findeisen
 * @date Created June 7, 2013
 * @date Last modified June 18, 2013
 */

#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <boost/smart_ptr.hpp>
#include "../../common/cerror.h"
#include "../../common/alloc.tmp.h"
#include "../mcio.h"
#include "output.h"
#include "../../common/nan.h"
#include "../nan.h"
#include "../except/undefined.h"

namespace lcmc { namespace stats {

using boost::shared_ptr;
using std::string;
using std::vector;
using namespace kpfutils;

/** Calculates the mean and standard deviation of a collection of statistics
 *
 * @param[in] values The statistics to be summarized.
 * @param[out] mean The mean of the statistics.
 * @param[out] stddev The standard deviation of the statistics.
 * @param[in] statName The name of the statistic, for error messages.
 *
 * @post @p mean and @p stddev ignore any NaNs present in @p values.
 * @post If there are no non-NaN elements in @p values, @p mean equals NaN
 * @post If there are less than two non-NaN elements in @p values, 
 *	@p stddev equals NaN
 *
 * @exceptsafe Does not throw exceptions.
 */
void getSummaryStats(const vector<double>& values, double& mean, double& stddev, 
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
	} catch (const except::NotEnoughData &e) {
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
 * @post @p mean and @p stddev ignore any NaNs present in @p values.
 * @post If there are no non-NaN elements in @p values, @p mean equals NaN
 * @post If there are less than two non-NaN elements in @p values, 
 *	@p stddev equals NaN
 * @post If there are no elements in @p values, @p goodFrac equals 0
 *
 * @exceptsafe Does not throw exceptions.
 */
void getSummaryStats(const vector<double>& values, double& mean, double& stddev, 
		double& goodFrac, 
		const string& statName) {
	size_t n = values.size();
	
	double badCount = static_cast<double>(
		std::count_if(values.begin(), values.end(), &kpfutils::isNanOrInf));
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
 *	to @p file
 * @exception kpfutils::except::FileIo Thrown if there are difficulties writing 
 *	to @p distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStat(FILE* const file, const vector<double>& archive, 
		const string& statName, const string& distribFile) {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanStats, stddevStats, fracStats;
	getSummaryStats(archive, meanStats, stddevStats, fracStats, 
			statName);
	
	int status = fprintf(file, "\t%6.3g±%5.2g\t%6.3g\t%s",
			meanStats, stddevStats, fracStats, distribFile.c_str() );
	if (status < 0) {
		cError("Could not print statistics in printStat(): ");
	}

	shared_ptr<FILE> auxFile = fileCheckOpen(distribFile, "w");
	
	for(size_t i = 0; i < archive.size(); i++) {
		status = fprintf(auxFile.get(), "%0.3f\n", archive[i]);
		if (status < 0) {
			fileError(auxFile.get(), "Could not write to log file '" + distribFile 
				+ "' in printStat(): ");
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
 *	to @p file
 * @exception kpfutils::except::FileIo Thrown if there are difficulties writing 
 *	to @p distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStatAlwaysDefined(FILE* const file, const vector<double>& archive, 
		const string& statName, const string& distribFile) {
	// For now, define summary variables for each statistic
	// Eventually statistics will be objects, with mean, filename, etc. 
	//	accessible as members
	double meanStats, stddevStats;
	getSummaryStats(archive, meanStats, stddevStats, statName);
	
	int status = fprintf(file, "\t%6.3g±%5.2g\t%s",
			meanStats, stddevStats, distribFile.c_str() );
	if (status < 0) {
		cError("Could not print statistics in printStatAlwaysDefined(): ");
	}

	shared_ptr<FILE> auxFile = fileCheckOpen(distribFile, "w");
	
	for(size_t i = 0; i < archive.size(); i++) {
		status = fprintf(auxFile.get(), "%0.3f\n", archive[i]);
		if (status < 0) {
			fileError(auxFile.get(), "Could not write to log file '" + distribFile 
				+ "' in printStatAlwaysDefined(): ");
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
 * @param[in] archive The statistic to print.
 * @param[in] distribFile The prefix identifying the distribution file as 
 *	being for this particular statistic.
 *
 * @exception std::runtime_error Thrown if there are difficulties writing 
 *	to @p file
 * @exception kpfutils::except::FileIo Thrown if there are difficulties writing 
 *	to @p distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStat(FILE* const file, const std::vector<vector<double> >& archive, 
		const string& distribFile) {
	
	int status = fprintf(file, "\t%s", distribFile.c_str());
	if (status < 0) {
		cError("Could not print log file name in printStat(): ");
	}

	shared_ptr<FILE> auxFile = fileCheckOpen(distribFile, "w");
	
	for(size_t i = 0; i < archive.size(); i++) {
		for(size_t j = 0; j < archive[i].size(); j++) {
			status = fprintf(auxFile.get(), "%0.3f ", archive[i][j]);
			if (status < 0) {
				fileError(auxFile.get(), "Could not write to log file '" 
					+ distribFile + "' in printStat(): ");
			}
		}
		status = fprintf(auxFile.get(), "\n");
		if (status < 0) {
			fileError(auxFile.get(), "Could not write to log file '" 
				+ distribFile + "' in printStat(): ");
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
 *	to @p file
 * @exception kpfutils::except::FileIo Thrown if there are difficulties writing 
 *	to @p distribFile
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void printStat(FILE* const file, const std::vector<vector<double> >& timeArchive, 
		const std::vector<vector<double> >& statArchive, 
		const string& distribFile) {
	
	int status = fprintf(file, "\t%s", distribFile.c_str());
	if (status < 0) {
		cError("Could not print log file name in printStat(): ");
	}

	shared_ptr<FILE> auxFile = fileCheckOpen(distribFile, "w");
	
	for(size_t i = 0; i < statArchive.size(); i++) {
		for(size_t j = 0; j < timeArchive[i].size(); j++) {
			status = fprintf(auxFile.get(), "%0.3f ", timeArchive[i][j]);
			if (status < 0) {
				fileError(auxFile.get(), "Could not write to log file '" 
					+ distribFile + "' in printStat(): ");
			}
		}
		status = fprintf(auxFile.get(), "\n");
		if (status < 0) {
			fileError(auxFile.get(), "Could not write to log file '" 
				+ distribFile + "' in printStat(): ");
		}
		for(size_t j = 0; j < statArchive[i].size(); j++) {
			status = fprintf(auxFile.get(), "%0.3f ", statArchive[i][j]);
			if (status < 0) {
				fileError(auxFile.get(), "Could not write to log file '" 
					+ distribFile + "' in printStat(): ");
			}
		}
		status = fprintf(auxFile.get(), "\n");
		if (status < 0) {
			fileError(auxFile.get(), "Could not write to log file '" + distribFile 
				+ "' in printStat(): ");
		}
	}
}

}}	// end lcmc::stats
