/** Classes for tracking test statistics
 * @file lightcurveMC/stats/named.cpp
 * @author Krzysztof Findeisen
 * @date Reconstructed June 7, 2013
 * @date Last modified August 5, 2013
 */

#include <algorithm>
#include <string>
#include "statcollect.h"

namespace lcmc { namespace stats {

using std::string;

/** Names a collection of statistics.
 *
 * @param[in] statName The name of the statistic to use in program output.
 * @param[in] distribFile The prefix identifying the distribution file as 
 *	being for this particular statistic.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to construct the object.
 *
 * @exceptsafe Object construction is atomic.
 */
NamedCollection::NamedCollection(const string& statName, const string& distribFile) 
		: IStats(), statName(statName), auxFile(distribFile) {
}

/** Returns the name of the statistic.
 *
 * @return A name to use for subclass output.
 *
 * @exceptsafe Does not throw exceptions.
 */
const string& NamedCollection::getStatName() const {
	return statName;
}

/** Returns the file name of the statistic.
 *
 * @return A base to use for files associated with the statistic.
 *
 * @exceptsafe Does not throw exceptions.
 */
const string& NamedCollection::getFileName() const {
	return auxFile;
}

/** Non-throwing swap
 *
 * @param[in,out] other The collection with which to exchange contents.
 *
 * @post @p other.getStatName() and @p other.getFileName() return the 
 *	values previously returned by @p this->getStatName() and 
 *	@p this->getFileName(), and vice versa
 *
 * @perform Constant time
 *
 * @exceptsafe Does not throw exceptions.
 */
void NamedCollection::swap(NamedCollection& other) {
	using std::swap;
	
	swap(this->statName, other.statName);
	swap(this->auxFile , other.auxFile );
}

}}		// end lcmc::stats
