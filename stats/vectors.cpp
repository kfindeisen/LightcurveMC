/** Classes for tracking test statistics
 * @file lightcurveMC/stats/vectors.cpp
 * @author Krzysztof Findeisen
 * @date Reconstructed June 7, 2013
 * @date Last modified June 7, 2013
 */

#include <string>
#include <vector>
#include <cstdio>
#include "../mcio.h"
#include "output.h"
#include "statcollect.h"

namespace lcmc { namespace stats {

using std::string;
using std::vector;

/** Constructs a collection of statistics.
 *
 * @param[in] statName The name of the statistic to use in program output.
 * @param[in] distribFile The prefix identifying the distribution file as 
 *	being for this particular statistic.
 *
 * @post The object represents an empty set of statistics.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to construct the object.
 *
 * @exceptsafe Object construction is atomic.
 */
CollectedVectors::CollectedVectors(const std::string& statName, const std::string& distribFile) 
		: NamedCollection(statName, distribFile), stats() {
}

/** Records the value of a vector statistic.
 *
 * @param[in] value The statistic to store.
 *
 * @pre @p value may contain NaNs
 *
 * @post The object contains all the statistics previously stored, 
 *	plus a new entry equal to @p value.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to store the new statistic.
 *
 * @exceptsafe The object is unchanged in the event of an exception.
 */
void CollectedVectors::addStat(const DoubleVec& value) {
	// DoubleVec is copyable, therefore stats.push_back() is atomic
	stats.push_back(value);
}

void CollectedVectors::printStats(FILE* const hOutput) const {
	printStat(hOutput, stats, getFileName());
}

void CollectedVectors::clear() {
	stats.clear();
}

/** Prints a header row representing the statistics printed by 
 *	printStats() to the specified file
 * 
 * The only header is the name of the statistic.
 * 
 * @param[in] hOutput An open file handle representing the text file to write to.
 * @param[in] fieldName The name of the statistic to use in the header.
 *
 * @exception std::runtime_error Thrown if the header could not be constructed.
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void CollectedVectors::printHeader(FILE* const hOutput, const string& fieldName) {
	int status = fprintf(hOutput, "\t%s", fieldName.c_str());
	if (status < 0) {
		fileError(hOutput, "String formatting error in printHeader(): ");
	}
}

}}		// end lcmc::stats
