/** Defines the Deterministic base class.
 * @file lightcurveMC/waves/lcdeterministic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 18, 2013
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

#include <vector>
#include "lcdeterministic.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a particular function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post getTimes() and getFluxes() return suitable data. getTimes() contains 
 * the same elements as @p times, possibly reordered.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 *
 * @exceptsafe Object construction is atomic.
 */
Deterministic::Deterministic(const std::vector<double> &times) : ILightCurve(), times(times) {
}

Deterministic::~Deterministic() {
}

/** Returns the times at which the simulated data were taken
 *
 * @param[out] timeArray A vector containing the desired times.
 *
 * @post Any data previously in @p timeArray is erased
 * @post @p timeArray.size() = size()
 * @post @p timeArray contains the times with which the light curve was initialized
 *
 * @post No element of @p timeArray is NaN
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	return a copy of the times.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
void Deterministic::getTimes(std::vector<double>& timeArray) const {
	using std::swap;

	// copy-and-swap to allow atomic guarantee
	// vector::= only offers the basic guarantee
	std::vector<double> temp = times;
	
	swap(timeArray, temp);
}

/** Returns the simulated fluxes at the corresponding times
 *
 * @param[out] fluxArray A vector containing the desired fluxes.
 *
 * @post Any data previously in @p fluxArray is erased
 * @post @p fluxArray.size() = size()
 * @post if getTimes()[i] = getTimes()[j] for i &ne; j, then 
 *	getFluxes()[i] = getFluxes()[j]
 * @post @p fluxArray[i] is determined entirely by getTimes()[i] and the 
 *	parameters passed to the constructor
 * 
 * @post No element of @p fluxArray is NaN
 * @post All elements in @p fluxArray are non-negative
 * @post Either the mean, median, or mode of the flux is one, when 
 *	averaged over many elements. Subclasses of Deterministic may 
 *	chose the option (mean, median, or mode) most appropriate 
 *	for their light curve shape.
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception std::logic_error Thrown if a bug was found in the flux calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
void Deterministic::getFluxes(std::vector<double>& fluxArray) const {
	using std::swap;

	// Must use virtual getTimes() to ensure consistency with getFluxes()
	std::vector<double> times;
	getTimes(times);
	
	size_t n = times.size();

	// copy-and-swap
	std::vector<double> temp;
	temp.reserve(n);
	
	for(std::vector<double>::const_iterator it = times.begin();
			it != times.end(); it++) {
		temp.push_back(flux(*it));
	}
	
	swap(fluxArray, temp);
}

/** Returns the number of times and fluxes
 *
 * @return The number of data points represented by the light curve.
 *
 * @post return value = getTimes().%size() = getFluxes().%size()
 *
 * @exceptsafe Does not throw exceptions.
 */
size_t Deterministic::size() const {
	return times.size();
}
	
}}		// end lcmc::models
