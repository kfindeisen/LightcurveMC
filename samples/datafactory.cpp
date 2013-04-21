/** Defines the factory function for Observations
 * @file datafactory.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2012
 * @date Last modified May 7, 2012
 */

#include <memory>
#include <stdexcept>
#include <string>
#include <cstdio>
#include "observations.h"

// Declare constructors for every observations subtype supported by dataSampler()
#include "obssamples.h"

using namespace std;

namespace lcmcinject {

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type.
 */
std::auto_ptr<Observations> dataSampler(const std::string& whichSample) {
	if (       whichSample == "NonSpitzerNonVar") {
		return std::auto_ptr<Observations>(new NonSpitzerNonVar());
	} else if (whichSample == "NonSpitzerVar") {
		return std::auto_ptr<Observations>(new NonSpitzerVar());
	} else if (whichSample ==    "SpitzerNonVar") {
		return std::auto_ptr<Observations>(new    SpitzerNonVar());
	} else if (whichSample ==    "SpitzerVar") {
		return std::auto_ptr<Observations>(new    SpitzerVar());
	} else {
		char buf[80];
		sprintf(buf, "Unsupported sample name: %s.", whichSample.c_str());
		throw invalid_argument(buf);
	}
}

}		// end lcmcinject
