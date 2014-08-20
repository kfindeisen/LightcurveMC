/** Defines the factory function for Observations
 * @file lightcurveMC/samples/datafactory.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2012
 * @date Last modified May 22, 2013
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

#include <memory>
#include <string>
#include "observations.h"

// Declare constructors for every observations subtype supported by dataSampler()
//#include "obssamples.h"

using std::auto_ptr;
using std::string;

namespace lcmc { namespace inject {

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type.
 *
 * @param[in] whichSample The name of the light curve catalog to read.
 *
 * @deprecated If @p whichSample takes the special keywords @c NonSpitzerNonVar, 
 *	@c NonSpitzerVar, @c SpitzerNonVar, or @c SpitzerVar, a file named 
 *	@c nonspitzernonvar.cat, @c nonspitzervar.cat, @c spitzernonvar.cat, or 
 *	@c spitzervar.cat will be used, respectively. These special cases are 
 *	offered for backward-compatibility with shell scripts that used 
 *	previous versions of Lightcurve MC.
 *
 * @return A smart pointer to an Observations object generated from the 
 *	given catalog.
 * 
 * @exception lcmc::inject::except::NoCatalog Thrown if the catalog file 
 *	does not exist.
 * @exception kpfutils::except::FileIo Thrown if the catalog or the light 
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
