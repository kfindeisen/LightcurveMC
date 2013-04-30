/** Defines the RandomWalk light curve class.
 * @file lcrw.cpp
 * @author Krzysztof Findeisen
 * @date Created April 29, 2013
 * @date Last modified April 29, 2013
 */

#include <vector>
#include <cmath>
#include "../fluxmag.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a damped random walk.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] diffus The diffusion constant of the random walk, in 
 *	units of flux^2/time
 *
 * @pre diffus > 0
 *
 * @post The object represents a random walk with the given diffusion 
 *	constant.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Implement input validation
 */
RandomWalk::RandomWalk(const std::vector<double>& times, double diffus) 
		: Stochastic(times), d(diffus) {
}

/** Computes a realization of the light curve. 
 *
 * The light curve is computed from times and the internal random 
 * number generator, and its values are stored in fluxes.
 *
 * @param[out] fluxes The flux vector to update.
 * 
 * @post getFluxes() will now return the correct light curve.
 * 
 * @post fluxes.size() == getTimes().size()
 * @post if getTimes()[i] == getTimes()[j] for i &ne; j, then 
 *	fluxes[i] == fluxes[j]
 * 
 * @post No element of fluxes is NaN
 * @post All elements in fluxes are non-negative
 * @post The median of the flux is one, when averaged over many elements and 
 *	many light curve instances.
 *
 * @post fluxToMag(fluxes) has a mean of zero and a standard deviation, at 
 *	each time t, of sqrt(diffus*(t-getTimes()[0]))
 * @post cov(fluxToMag(fluxes[i]), fluxToMag(fluxes[j])) == 
 *	diffus * (min{getTimes()[i], getTimes()[j]} - getTimes()[0])
 */	
void RandomWalk::solveFluxes(std::vector<double>& fluxes) const {
	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	fluxes.clear();
	if (times.size() > 0) {
		fluxes.reserve(times.size());
		// A damped random walk is self-similar regardless of its starting value
		fluxes.push_back(0.0);
		
		// Evaluate f(t > t0)
		// Derived analogously to eq. 2.47 in Gillespie (1996)
		for(std::vector<double>::const_iterator it = times.begin()+1; it != times.end(); it++) {
			double oldFlux = fluxes.back();
			
			// Observations taken at the same time should have the same flux
			// Sorting invariant guarantees that all duplicate times will 
			//	be next to each other
			if (*it == *(it-1)) {
				fluxes.push_back(oldFlux);
			// Observations taken at different times are partially 
			//	correlated
			} else {
				double deltaT = (*it - *(it-1));
				
				fluxes.push_back(oldFlux + sqrt(d * deltaT) * rNorm());
			}
		}
		
		utils::magToFlux(fluxes, fluxes);
	}
}

}}		// end lcmc::models
