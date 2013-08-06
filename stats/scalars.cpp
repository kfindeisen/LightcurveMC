/** Classes for tracking test statistics
 * @file lightcurveMC/stats/scalars.cpp
 * @author Krzysztof Findeisen
 * @date Reconstructed June 7, 2013
 * @date Last modified August 6, 2013
 */

#include <limits>
#include <string>
#include <vector>
#include <cstdio>
#include "../mcio.h"
#include "output.h"
#include "statcollect.h"
#include "../../common/cerror.h"

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
CollectedScalars::CollectedScalars(const std::string& statName, const std::string& distribFile) 
		: NamedCollection(statName, distribFile), stats() {
}

/** Records the value of a scalar statistic.
 *
 * @param[in] value The statistic to store.
 *
 * @pre @p value may be NaN
 *
 * @post The object contains all the statistics previously stored, 
 *	plus a new entry equal to @p value.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to store the new statistic.
 *
 * @exceptsafe The object is unchanged in the event of an exception.
 */
void CollectedScalars::addStat(double value) {
	stats.push_back(value);
}

/** Records an invalid scalar statistic.
 *
 * @post The object contains all the statistics previously stored, 
 *	plus a new entry equal to NaN.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to store the extra value.
 *
 * @exceptsafe The object is unchanged in the event of an exception.
 */
void CollectedScalars::addNull() {
	addStat(std::numeric_limits<double>::quiet_NaN());
}

void CollectedScalars::printStats(FILE* const hOutput) const {
	printStat(hOutput, stats, getStatName(), getFileName());
}

void CollectedScalars::clear() {
	stats.clear();
}

/** Returns a vector containing the data in the same order as 
 * printed by printStats()
 *
 * @param[in] outVector A vector in which the data will be stored
 * 
 * @post @p outVector contains a copy of the data output by printStats(), 
 * 	in the same order
 * @post Any data previously in @p outVector is deleted
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to copy the data.
 * @exceptsafe Neither this object nor @p outVector are changed in the event 
 *	of an exception.
 */
void CollectedScalars::toVector(vector<double>& outVector) const {
	// copy-and-swap
	vector<double> temp = stats;

	// IMPORTANT: no exceptions beyond this point
	
	using std::swap;
	swap(outVector, temp);
}

/** Prints a header row representing the statistics printed by 
 *	printStats() to the specified file
 * 
 * The headers are the name of the statistic, the error, the fraction 
 * of runs in which the statistic is well defined, and the name of a 
 * file containing the distribution. The header is in tab-delimited 
 * format, except that the statistic and the error are separated by 
 * the ± sign.
 * 
 * @param[in] hOutput An open file handle representing the text file to write to.
 * @param[in] fieldName The name of the statistic to use in the header.
 *
 * @exception std::runtime_error Thrown if the header could not be constructed.
 *
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void CollectedScalars::printHeader(FILE* const hOutput, const string& fieldName) {
	int status = fprintf(hOutput, "\t%s±err\tFinite\t%s Distribution", 
			fieldName.c_str(), fieldName.c_str());
	if (status < 0) {
		kpfutils::fileError(hOutput, "String formatting error in printHeader(): ");
	}
}

/** Non-throwing swap
 *
 * @param[in,out] other The collection with which to exchange contents.
 *
 * @post The data previously contained in @p other is now stored in @p *this, 
 *	and vice versa. All names are switched as well.
 * 
 * @perform Constant time
 *
 * @exceptsafe Does not throw exceptions.
 */
void CollectedScalars::swap(CollectedScalars& other) {
	using std::swap;
	
	NamedCollection::swap(other);
	
	swap(this->stats, other.stats);
}

/** Non-throwing swap
 *
 * @param[in,out] a,b The collections to exchange contents.
 *
 * @post The data previously contained in @p a is now stored in @p b, 
 *	and vice versa. All names are switched as well.
 * 
 * @perform Constant time
 *
 * @exceptsafe Does not throw exceptions.
 */
void swap(CollectedScalars& a, CollectedScalars& b) {
	a.swap(b);
}

}}		// end lcmc::stats

namespace std {

template <>
void swap<lcmc::stats::CollectedScalars>(lcmc::stats::CollectedScalars& a, 
		lcmc::stats::CollectedScalars& b) {
	a.swap(b);
}

}
