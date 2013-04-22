/** Defines the WhiteNoise light curve class.
 * @file lcwhite.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified April 4, 2013
 */

#include <stdexcept>
#include <vector>
#include "../fluxmag.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a white noise process.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] sigma The root-mean-square amplitude of the white noise
 *
 * @pre sigma > 0
 *
 * @post The object represents an uncorrelated Gaussian signal with the 
 *	given amplitude.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Implement input validation
 */
WhiteNoise::WhiteNoise(const std::vector<double>& times, double sigma) 
		: Stochastic(times), sigma(sigma) {
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
 * @bug Case where times() is empty not supported
 */	
void WhiteNoise::solveFluxes(std::vector<double>& fluxes) const {
	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	fluxes.clear();
	fluxes.reserve(times.size());
	// f(t0) ~ N(0, sigma) to avoid giving first point any special treatment
	fluxes.push_back(sigma * rNorm());
	
	// Evaluate f(t > t0)
	for(std::vector<double>::const_iterator it = times.begin()+1; it != times.end(); it++) {
		// Observations taken at the same time should have the same flux
		// Sorting invariant guarantees that all duplicate times will 
		//	be next to each other
		if (*it == *(it-1)) {
			fluxes.push_back(fluxes.back());
		// Observations taken at different times are uncorrelated
		} else {
			fluxes.push_back(sigma * rNorm());
		}
	}
	
	utils::magToFlux(fluxes, fluxes);
}

}}		// end lcmc::models
