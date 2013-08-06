/** Global R instance to use in calculations
 * @file lightcurveMC/rinstance.cpp
 * @author Krzysztof Findeisen
 * @date Created June 24, 2013
 * @date Last modified July 22, 2013
 */

#include <exception>
#include <vector>
#include <boost/smart_ptr.hpp>
#include "r_compat.h"

namespace lcmc { 

using boost::shared_ptr;

/** Returns a well-defined reference to an R instance. 
 * 
 * @return A pointer to a running R session.
 * 
 * @invariant All pointers returned by getRInstance() refer to the same R instance.
 *
 * @note This behavior is forced by the R API: it is impossible for a 
 *	single process to open two consecutive R sessions.
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to start the process.
 * @exception lcmc::except::BadRSession Thrown if R could not be started due 
 *	to an internal error in R.
 * 
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
shared_ptr<RInside> getRInstance() {
	// invariant: globalInstance is null or points to a valid session
	static shared_ptr<RInside> globalInstance;
	if (globalInstance.get() == NULL) {
		try {
			globalInstance.reset(new RInside);
		} catch (const std::exception& e) {
			throw except::BadRSession(e.what());
		}
		// IMPORTANT: no exceptions beyond this point

		// Ensure warnings are visible to the caller
		globalInstance->parseEvalQ("options(warn = 2)");
	}
	
	// assert: globalInstance points to a valid session
	
	return globalInstance;
}

}		// end lcmc
