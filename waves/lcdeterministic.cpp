/** Defines the Deterministic base class.
 * @file lightcurveMC/waves/lcdeterministic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 18, 2013
 * @date Last modified May 11, 2013
 */

#include <vector>
#include "lcdeterministic.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a particular function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post getTimes() and getFluxes() return suitable data. getTimes() contains 
 * the same elements as times, possibly reordered.
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
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
 * @post timeArray.size() == getFluxes().size()
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
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
 * @post fluxArray.size() == getTimes().size()
 * @post if getTimes()[i] == getTimes()[j] for i &ne; j, then 
 *	getFluxes()[i] == getFluxes()[j]
 * @post fluxArray[i] is determined entirely by getTimes[i] and the 
 *	parameters passed to the constructor
 * 
 * @post No element of fluxArray is NaN
 * @post All elements in fluxArray are non-negative
 * @post Either the mean, median, or mode of the flux is one, when 
 *	averaged over many elements. Subclasses of Deterministic may 
 *	chose the option (mean, median, or mode) most appropriate 
 *	for their light curve shape.
 * 
 * @exception bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception logic_error Thrown if a bug was found in the flux calculations.
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
 * @post return value == getTimes().size() == getFluxes.size()
 *
 * @exceptsafe Does not throw exceptions.
 */
size_t Deterministic::size() const {
	return times.size();
}
	
}}		// end lcmc::models
