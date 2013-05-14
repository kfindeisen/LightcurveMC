/** Defines the factory function for Observations
 * @file datafactory.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2012
 * @date Last modified May 10, 2013
 */

#include <memory>
#include <string>
#include "observations.h"

// Declare constructors for every observations subtype supported by dataSampler()
//#include "obssamples.h"

using std::auto_ptr;
using std::string;

namespace lcmc { namespace inject {

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type. Use of an auto_ptr prevents 
 *	client-side memory management issues.
 *
 * @param[in] whichSample The name of the light curve catalog to read.
 *
 * @deprecated If whichSample takes the special keywords NonSpitzerNonVar, 
 *	NonSpitzerVar, SpitzerNonVar, or SpitzerVar, a file named 
 *	nonspitzernonvar.cat, nonspitzervar.cat, spitzernonvar.cat, or 
 *	spitzervar.cat will be used, respectively. These special cases are 
 *	offered for backward-compatibility with shell scripts that used 
 *	previous versions of lightcurveMC.
 *
 * @return A smart pointer to an Observations object generated from the 
 *	given catalog.
 * 
 * @exception lcmc::inject::except::NoCatalog Thrown if the catalog file 
 *	does not exist.
 * @exception lcmc::except::FileIo Thrown if the catalog or the light 
 *	curve could not be read.
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	create the object.
 *
 * @exceptsafe Object construction is atomic.
 */
std::auto_ptr<Observations> dataSampler(const string& whichSample) {
	if (       whichSample == "NonSpitzerNonVar") {
		return auto_ptr<Observations>(new Observations("nonspitzernonvar.cat"));
	} else if (whichSample == "NonSpitzerVar") {
		return auto_ptr<Observations>(new Observations(   "nonspitzervar.cat"));
	} else if (whichSample ==    "SpitzerNonVar") {
		return auto_ptr<Observations>(new Observations(   "spitzernonvar.cat"));
	} else if (whichSample ==    "SpitzerVar") {
		return auto_ptr<Observations>(new Observations(      "spitzervar.cat"));
	} else {
		return auto_ptr<Observations>(new Observations(whichSample));
	}
}

}}		// end lcmc::inject
