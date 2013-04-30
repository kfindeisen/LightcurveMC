/** Defines the TwoScaleGp light curve class.
 * @file lcgp2.cpp
 * @author Krzysztof Findeisen
 * @date Created April 29, 2013
 * @date Last modified April 29, 2013
 */

#include <stdexcept>
#include <vector>
#include <cmath>
#include <gsl/gsl_matrix.h>
#include "../fluxmag.h"
#include "generators.h"
#include "lightcurves_gp.h"
#include "../raiigsl.tmp.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a two-component Gaussian process.
 * 
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] sigma1, sigma2 The root-mean-square amplitudes of the 
 *	two components of the Gaussian process
 * @param[in] tau1, tau2 The correlation times of the 
 *	two components of the Gaussian process.
 *
 * @pre sigma1 > 0
 * @pre sigma2 > 0
 * @pre tau1 > 0
 * @pre tau2 > 0
 *
 * @post The object represents a correlated Gaussian signal with the 
 *	given amplitudes and correlation times.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Implement input validation
 *
 * @todo This interface is error-prone. Redefine in terms of ADTs?
 */
TwoScaleGp::TwoScaleGp(const std::vector<double>& times, 
			double sigma1, double tau1, double sigma2, double tau2) 
		: Stochastic(times), sigma1(sigma1), sigma2(sigma2), 
		tau1(tau1), tau2(tau2) {
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
 * @post fluxToMag(fluxes) has a mean of zero and a standard deviation of 
 *	sqrt(sigma_1^2 + sigma_2^2)
 * @post cov(fluxToMag(fluxes[i]), fluxToMag(fluxes[j])) == 
 *	  sigma_1^2 &times; exp(-0.5*((getTimes()[i]-getTimes()[j])/tau_1)^2) 
 *	+ sigma_2^2 &times; exp(-0.5*((getTimes()[i]-getTimes()[j])/tau_2)^2) 
 */
void TwoScaleGp::solveFluxes(std::vector<double>& fluxes) const {
	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	size_t nTimes = times.size();
	
	fluxes.clear();
	if (nTimes > 0) {
		fluxes.reserve(nTimes);
		
		for(size_t i = 0; i < nTimes; i++) {
			fluxes.push_back(rNorm());
		}
	
		RaiiGsl<gsl_matrix> corrs(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
		// in gsl_matrix, consecutive second indices refer to adjacent memory spots
		// therefore, having the inner loop be over the second index is more localized
		for(size_t i = 0; i < nTimes; i++) {
			for(size_t j = 0; j < nTimes; j++) {
				double deltaTTau1 = (times[i] - times[j])/tau1;
				double deltaTTau2 = (times[i] - times[j])/tau2;
				gsl_matrix_set(corrs.get(), i, j, 
					  sigma1*sigma1*exp(-0.5*deltaTTau1*deltaTTau1)
					+ sigma2*sigma2*exp(-0.5*deltaTTau2*deltaTTau2));
			}
		}
		utils::multiNormal(fluxes, *corrs, fluxes);
		
		utils::magToFlux(fluxes, fluxes);
	}
}

}}		// end lcmc::models
