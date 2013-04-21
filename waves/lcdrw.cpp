/** Defines the DampedRandomWalk light curve class.
 * @file lcdrw.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified April 4, 2013
 */

#include <vector>
#include <cmath>
#include "../fluxmag.h"
#include "lightcurves_gp.h"

namespace lcmcmodels {

/** Initializes the light curve to represent a damped random walk.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] diffus The diffusion constant of the damped random walk, in 
 *	units of flux^2/time
 * @param[in] tau The damping time of the damped random walk.
 *
 * @pre diffus > 0
 * @pre tau > 0
 *
 * @post The object represents a damped random walk with the given diffusion 
 *	constant and correlation time.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Implement input validation
 */
DampedRandomWalk::DampedRandomWalk(const std::vector<double>& times, double diffus, double tau) 
		: Stochastic(times), sigma(sqrt(0.5*diffus*tau)), tau(tau) {
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
void DampedRandomWalk::solveFluxes(std::vector<double>& fluxes) const {
	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	fluxes.clear();
	fluxes.reserve(times.size());
	// f(t0) ~ N(0, sigma) to ensure self-similarity
	fluxes.push_back(sigma * rNorm());
	
	// Evaluate f(t > t0)
	// This simplified algorithm works only for an exponential covariance, 
	//	a white noise process, or a constant process, since it 
	//	requires rho(t1,t3) = rho(t1,t2)*rho(t2,t3). However, it's 
	//	much faster than the general-purpose, matrix-based algorithm.
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
			// Implement Equation 2.47 of Gillespie (1996)
			// His algorithm is not worth the trouble, since it 
			//	relies heavily on deltaT being constant
			double deltaTTau  = (*it - *(it-1))/tau;
			
			fluxes.push_back(oldFlux*exp(-deltaTTau) 
				+ sigma*sqrt((1.0 - exp(-2.0*deltaTTau))) * rNorm());
		}
	}
	
	lcmcutils::magToFlux(fluxes, fluxes);
}

}		// end lcmcmodels
