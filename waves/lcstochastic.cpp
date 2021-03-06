/** Defines the Stochastic base class.
 * @file lightcurveMC/waves/lcstochastic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
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

#include <algorithm>
#include <memory>
#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "lcstochastic.h"

#include "../../common/warnflags.h"

namespace lcmc { namespace models {

using std::auto_ptr;

/** Initializes the light curve to represent an instance of a stochastic time series.
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post getTimes() and getFluxes() return suitable data. getTimes() contains 
 * the same elements as @p times, possibly reordered.
 *
 * @internal @post Calls to Stochastic::rng() will not throw exceptions. @endinternal
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 *
 * @exceptsafe Object construction is atomic.
 */
Stochastic::Stochastic(const std::vector<double> &times) 
		: ILightCurve(), times(times), fluxes(), fluxesSolved(false) {
	// Allow subclasses to assume times are in increasing order
	std::sort(this->times.begin(), this->times.end());
	
	// Since only the first call to rng() throws exceptions, deal with it 
	//	in the constructor
	rng();
}

Stochastic::~Stochastic() {
}

/** Returns the times at which the simulated data were taken
 *
 * @param[out] timeArray A vector containing the desired times.
 *
 * @post Any data previously in @p timeArray is erased
 * @post @p timeArray.size() = size()
 * @post @p timeArray contains the times with which the light curve was initialized
 * @post @p timeArray is sorted in ascending order
 *
 * @post No element of @p timeArray is NaN
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	return a copy of @p times.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
void Stochastic::getTimes(std::vector<double>& timeArray) const {
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
 * 
 * @post No element of @p fluxArray is NaN
 * @post All elements in @p fluxArray are non-negative
 * @post Either the mean, median, or mode of the flux is one, when 
 *	averaged over many elements and many light curve instances. 
 *	Subclasses of Stochastic may chose the option 
 *	(mean, median, or mode) most appropriate for their light 
 *	curve shape.
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception std::logic_error Thrown if a bug was found in the flux calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
void Stochastic::getFluxes(std::vector<double>& fluxArray) const {
	using std::swap;

	if (!fluxesSolved) {
		// solveFluxes() already has the atomic guarantee
		solveFluxes(fluxes);
		fluxesSolved = true;
	}
	
	// copy-and-swap to allow atomic guarantee
	// vector::= only offers the basic guarantee
	std::vector<double> temp = fluxes;
	
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
size_t Stochastic::size() const {
	return times.size();
}
	
/** Defines the random number state of the first Stochastic object.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	allocate the random number generator.
 *
 * @exceptsafe The program state is unchanged in the event of an exception. 
 *	Does not throw exceptions after the first call.
 */
StochasticRng& Stochastic::rng() {
	static StochasticRng* foo = NULL;
	static bool ready = false;
	
	if(!ready) {
		foo = new StochasticRng(42);
	
		// Only set the flag once no exceptions will be thrown
		ready = true;
	}
	
	return *foo;
}

/** Creates a temporary copy of a random number generator
 *
 * @return A newly allocated StochasticRng equal to the internal random 
 *	number generator
 *
 * @exception std::bad_alloc Thrown if there was not enough memory 
 *	to construct the generator.
 *
 * @exceptsafe Object construction is atomic.
 */
auto_ptr<StochasticRng> Stochastic::checkout() const {
	// Since Stochastic has been constructed, rng() no longer 
	//	throws exceptions
	return auto_ptr<StochasticRng>(new StochasticRng(rng()));
}

/** Updates the state of the random number generator
 *
 * @param[in] newState A temporary copy of the internal random 
 *	number generator
 *
 * @post The internal generator is in the same state as @p newState
 *
 * @exceptsafe Does not throw exceptions.
 */
void Stochastic::commit(auto_ptr<StochasticRng> newState) const {
	// Since Stochastic has been constructed, rng() no longer 
	//	throws exceptions
	rng() = *(newState.get());
}

}}		// end lcmc::models
