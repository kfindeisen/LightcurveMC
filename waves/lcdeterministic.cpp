/** Defines the Deterministic base class.
 * @file lcdeterministic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 18, 2013
 * @date Last modified March 19, 2013
 */

#include <vector>
#include "lcsubtypes.h"

namespace lcmcmodels {

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
 *
 * @bug Some light curves still follow a spec that allows them to return 
 *	negative fluxes
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

}		// end lcmcmodels
