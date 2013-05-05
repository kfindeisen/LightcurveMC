/** Defines the TwoScaleGp light curve class.
 * @file lcgp2.cpp
 * @author Krzysztof Findeisen
 * @date Created April 29, 2013
 * @date Last modified April 30, 2013
 */

#include <stdexcept>
#include <vector>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include "../approx.h"
#include "../fluxmag.h"
#include "generators.h"
#include "lightcurves_gp.h"

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
	
		// shared_ptr does not allow its internal pointer to be changed
		// So assign to a dummy pointer, then give it to shared_ptr 
		//	once the pointer target is known
		gsl_matrix* corrPtr = getCovar();
		boost::shared_ptr<gsl_matrix> corrs(corrPtr, &gsl_matrix_free);
		
		utils::multiNormal(fluxes, *corrs, fluxes);
		
		utils::magToFlux(fluxes, fluxes);
	}
}

/** Allocates and initializes the covariance matrix for the 
 *	Gaussian process. 
 *
 * @todo Respec using a copyable smart pointer when I get the chance
 *
 * @return A pointer containing the new matrix
 */	
gsl_matrix* TwoScaleGp::getCovar() const {
	// Define a cache to prevent identical simulation runs from having to 
	const static utils::ApproxEqual cacheCheck(1e-12);
	static gsl_matrix* oldCov = NULL;
	static std::vector<double> oldTimes;
	static double oldSigma1 = 0.0, oldSigma2 = 0.0;
	static double oldTau1 = 0.0, oldTau2 = 0.0;

	std::vector<double> times;
	this->getTimes(times);
	size_t nTimes = times.size();

	if(oldCov == NULL 
			|| !cacheCheck(oldSigma1, sigma1)
			|| !cacheCheck(oldSigma2, sigma2)
			|| !cacheCheck(oldTau1, tau1)
			|| !cacheCheck(oldTau2, tau2)
			|| !cacheCheck(oldTimes.size(), nTimes)
			|| !std::equal(oldTimes.begin(), oldTimes.end(), 
					times.begin(), cacheCheck) ) {
		// Cache is out of date
		if (oldCov != NULL) {
			gsl_matrix_free(oldCov);
		}
		
		oldCov = gsl_matrix_alloc(nTimes, nTimes);
		for(size_t i = 0; i < nTimes; i++) {
			for(size_t j = 0; j < nTimes; j++) {
				double deltaTTau1 = (times[i] - times[j])/tau1;
				double deltaTTau2 = (times[i] - times[j])/tau2;
				gsl_matrix_set(oldCov, i, j, 
					  sigma1*sigma1*exp(-0.5*deltaTTau1*deltaTTau1)
					+ sigma2*sigma2*exp(-0.5*deltaTTau2*deltaTTau2));
			}
		}
		
		oldTimes = times;
		oldSigma1 = sigma1;
		oldSigma2 = sigma2;
		oldTau1   = tau1;
		oldTau2   = tau2;
	}
	
	// assert: the Cache is up-to-date
	
	// Return a copy of the cache as output
	gsl_matrix* cov = gsl_matrix_alloc(nTimes, nTimes);
	gsl_matrix_memcpy(cov, oldCov);

	return cov;
}

}}		// end lcmc::models
