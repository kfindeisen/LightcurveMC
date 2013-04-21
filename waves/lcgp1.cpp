/** Defines the SimpleGp light curve class.
 * @file lcgp1.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified April 18, 2013
 */

#include <stdexcept>
#include <vector>
#include <cmath>
#include <gsl/gsl_matrix.h>
#include "../fluxmag.h"
#include "generators.h"
#include "lightcurves_gp.h"
#include "../raiigsl.tmp.h"

namespace lcmcmodels {

/** Initializes the light curve to represent a standard Gaussian process.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] sigma The root-mean-square amplitude of the Gaussian process
 * @param[in] tau The correlation time of the Gaussian process.
 *
 * @pre sigma > 0
 * @pre tau > 0
 *
 * @post The object represents a correlated Gaussian signal with the 
 *	given amplitude and correlation time.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Implement input validation
 */
SimpleGp::SimpleGp(const std::vector<double>& times, double sigma, double tau) 
		: Stochastic(times), sigma(sigma), tau(tau) {
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
void SimpleGp::solveFluxes(std::vector<double>& fluxes) const {
	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	size_t nTimes = times.size();
	
	fluxes.clear();
	fluxes.reserve(nTimes);
	
	for(size_t i = 0; i < nTimes; i++) {
		fluxes.push_back(rNorm());
	}

	RaiiGsl<gsl_matrix> corrs(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	// in gsl_matrix, consecutive second indices refer to adjacent memory spots
	// therefore, having the inner loop be over the second index is more localized
	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaTTau = (times[i] - times[j])/tau;
			gsl_matrix_set(corrs.get(), i, j, 
					sigma*sigma*exp(-0.5*deltaTTau*deltaTTau));
		}
	}
	lcmcutils::multiNormal(fluxes, *corrs, fluxes);
	
	lcmcutils::magToFlux(fluxes, fluxes);
}

}		// end lcmcmodels
