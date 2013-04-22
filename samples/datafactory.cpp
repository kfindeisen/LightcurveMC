/** Defines the factory function for Observations
 * @file datafactory.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2012
 * @date Last modified April 21, 2013
 */

#include <memory>
#include <stdexcept>
#include <string>
#include <cstdio>
#include "observations.h"

// Declare constructors for every observations subtype supported by dataSampler()
#include "obssamples.h"

using std::auto_ptr;
using std::string;

namespace lcmc { namespace inject {

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type.
 */
std::auto_ptr<Observations> dataSampler(const string& whichSample) {
	if (       whichSample == "NonSpitzerNonVar") {
		return auto_ptr<Observations>(new NonSpitzerNonVar());
	} else if (whichSample == "NonSpitzerVar") {
		return auto_ptr<Observations>(new NonSpitzerVar());
	} else if (whichSample ==    "SpitzerNonVar") {
		return auto_ptr<Observations>(new    SpitzerNonVar());
	} else if (whichSample ==    "SpitzerVar") {
		return auto_ptr<Observations>(new    SpitzerVar());
	} else {
		char buf[80];
		sprintf(buf, "Unsupported sample name: %s.", whichSample.c_str());
		throw std::invalid_argument(buf);
	}
}

}}		// end lcmc::inject
