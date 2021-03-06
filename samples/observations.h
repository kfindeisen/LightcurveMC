/** Type definitions for Lightcurve MC data representations
 * @file lightcurveMC/samples/observations.h
 * @author Krzysztof Findeisen
 * @date Created May 4, 2012
 * @date Last modified May 9, 2012
 * 
 * These types represent different sets of observations that can be used to 
 * inject simulated signals into real data. The actual population of the sets 
 * is currently done using external catalog files, but this implementation is 
 * subject to change.
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

#ifndef LCMCDATAH
#define LCMCDATAH

#include <memory>
#include <string>
#include <vector>

namespace lcmc { 

/** This namespace identifies data types and functions that handle the loading 
 * of data so that it can be injected with simulated data.
 */
namespace inject {

/** Class for generating data samples for signal injection testing. 
 * 
 * Objects of class Observations represent individual sources drawn from a 
 * selected sample. Observations objects are non-deterministic in the sense 
 * that the user only specifies which sample the light curve comes from, but 
 * not which light curve from within the sample gets used. However, any 
 * individual object is immutable, and can be can be used without worrying 
 * about determinism.
 *
 * @invariant Observations is immutable.
 * @invariant The light curve has a median flux of 1, so that synthetic light 
 *	curve amplitudes can be expressed in units of the source flux.
 */
class Observations {
public: 
	/** Initializes the object to represent a randomly selected light 
	 * curve from the selected catalog.
	 */
	Observations(const std::string& catalogName);
	
	/** Returns the timestamps associated with this source.
	 */
	void getTimes(std::vector<double>& timeArray) const;

	/** Returns the flux measurements associated with this source.
	 */
	void getFluxes(std::vector<double>& fluxArray) const;

	// Virtual destructor following Effective C++
	virtual ~Observations() {};

private:
	/** Initializes an object to the value of a particular light curve.
	 */
	void readFile(const std::string& fileName);

	/** Returns the list of files from which the class may select sources
	 */
	static const std::vector<std::string> getLcLibrary(const std::string& catalogName);

	/** Store the light curve itself
	 */
	std::vector<double> times;
	std::vector<double> fluxes;
};

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type.
 */
std::auto_ptr<Observations> dataSampler(const std::string& whichSample);

}}		// end lcmc::inject

#endif		// end ifndef LCMCDATAH
