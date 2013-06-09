/** Workhorse code for statistics output
 * @file lightcurveMC/stats/output.h
 * @author Krzysztof Findeisen
 * @date Created June 7, 2013
 * @date Last modified June 7, 2013
 */

#include <string>
#include <vector>
#include <cstdio>

namespace lcmc { namespace stats {

using std::string;
using std::vector;

/** Calculates the mean and standard deviation of a collection of statistics
 */
void getSummaryStats(const vector<double>& values, double& mean, double& stddev, 
		const string& statName);

/** Calculates the mean, standard deviation, and definition rate of a collection of statistics
 */
void getSummaryStats(const vector<double>& values, double& mean, double& stddev, 
		double& goodFrac, const string& statName);

/** Prints a single family of statistics to the specified file
 */
void printStat(FILE* const file, const vector<double>& archive, 
	const string& statName, const string& distribFile);

/** Prints a single family of statistics to the specified file
 */
void printStatAlwaysDefined(FILE* const file, const vector<double>& archive, 
	const string& statName, const string& distribFile);

/** Prints a single family of statistics to the specified file
 */
void printStat(FILE* const file, const vector<vector<double> >& archive, 
	const string& distribFile);

/** Prints a set of functions to the specified file
 */
void printStat(FILE* const file, const vector<vector<double> >& timeArchive, 
	const vector<vector<double> >& statArchive, const string& distribFile);

}}	// end lcmc::stats
