/** Defines the Deterministic base class.
 * @file lcdeterministic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 18, 2013
 * @date Last modified April 27, 2013
 */

#include <vector>
#include "lcsubtypes.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a particular function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post getTimes() and getFluxes() return suitable data. getTimes() contains 
 * the same elements as times, possibly reordered.
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
 */
void Deterministic::getTimes(std::vector<double>& timeArray) const {
	timeArray = times;
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
 */
void Deterministic::getFluxes(std::vector<double>& fluxArray) const {
	// Copy of time vector is for convenience only
	// Must use virtual getTimes() to ensure consistency with getFluxes()
	std::vector<double> times;
	getTimes(times);
	
	size_t n = times.size();
	fluxArray.clear();
	fluxArray.reserve(n);
	
	for(std::vector<double>::const_iterator it = times.begin();
			it != times.end(); it++) {
		fluxArray.push_back(flux(*it));
	}
}

}}		// end lcmc::models
