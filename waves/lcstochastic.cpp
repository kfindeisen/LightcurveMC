/** Defines the Stochastic base class.
 * @file lightcurveMC/waves/lcstochastic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified May 12, 2013
 */

#include <algorithm>
#include <memory>
#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "lcstochastic.h"

#include "../warnflags.h"

namespace lcmc { namespace models {

using std::auto_ptr;

/** Initializes the light curve to represent a particular function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post getTimes() and getFluxes() return suitable data. getTimes() contains 
 * the same elements as times, possibly reordered.
 *
 * @post Calls to Stochastic::rng() will not throw exceptions.
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
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
 * @post timeArray.size() == getFluxes().size()
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
 *	return a copy of the times.
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
 * @post fluxArray.size() == getTimes().size()
 * @post if getTimes()[i] == getTimes()[j] for i &ne; j, then 
 *	getFluxes()[i] == getFluxes()[j]
 * 
 * @post No element of fluxArray is NaN
 * @post All elements in fluxArray are non-negative
 * @post Either the mean, median, or mode of the flux is one, when 
 *	averaged over many elements and many light curve instances. 
 *	Subclasses of ILightCurve may chose the option 
 *	(mean, median, or mode) most appropriate for their light 
 *	curve shape.
 * 
 * @exception bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception logic_error Thrown if a bug was found in the flux calculations.
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
 * @post return value == getTimes().size() == getFluxes.size()
 *
 * @exceptsafe Does not throw exceptions.
 */
size_t Stochastic::size() const {
	return times.size();
}
	
/** Defines the random number state of the first Stochastic object.
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
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
 * @post The internal generator is in the same state as newState
 *
 * @exceptsafe Does not throw exceptions.
 */
void Stochastic::commit(auto_ptr<StochasticRng> newState) const {
	// Since Stochastic has been constructed, rng() no longer 
	//	throws exceptions
	rng() = *(newState.get());
}

}}		// end lcmc::models
