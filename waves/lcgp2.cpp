/** Defines the TwoScaleGp light curve class.
 * @file lightcurveMC/waves/lcgp2.cpp
 * @author Krzysztof Findeisen
 * @date Created April 29, 2013
 * @date Last modified May 22, 2013
 */

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include "../approx.h"
#include "../except/data.h"
#include "../fluxmag.h"
#include "generators.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

using boost::lexical_cast;
using boost::shared_ptr;
using std::auto_ptr;

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
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the parameters 
 *	are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 *
 * @todo This interface is error-prone. Redefine in terms of ADTs?
 */
TwoScaleGp::TwoScaleGp(const std::vector<double>& times, 
			double sigma1, double tau1, double sigma2, double tau2) 
		: Stochastic(times), sigma1(sigma1), sigma2(sigma2), 
		tau1(tau1), tau2(tau2) {
	if (sigma1 <= 0.0) {
		throw except::BadParam("All DampedRandomWalk light curves need positive standard deviations (gave " 
			+ lexical_cast<string>(sigma1) + " for the first component).");
	}
	if (tau1 <= 0.0) {
		throw except::BadParam("All DampedRandomWalk light curves need positive coherence times (gave " 
			+ lexical_cast<string>(tau1) + " for the first component).");
	}
	if (sigma2 <= 0.0) {
		throw except::BadParam("All DampedRandomWalk light curves need positive standard deviations (gave " 
			+ lexical_cast<string>(sigma2) + " for the first component).");
	}
	if (tau2 <= 0.0) {
		throw except::BadParam("All DampedRandomWalk light curves need positive coherence times (gave " 
			+ lexical_cast<string>(tau2) + " for the first component).");
	}
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
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
void TwoScaleGp::solveFluxes(std::vector<double>& fluxes) const {
	using std::swap;

	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	// copy-and-swap
	auto_ptr<StochasticRng> rng = checkout();
	std::vector<double> temp;

	if (times.size() > 0) {
		temp.reserve(times.size());
		
		for(size_t i = 0; i < times.size(); i++) {
			temp.push_back(rng->rNorm());
		}
	
		shared_ptr<gsl_matrix> corrs = getCovar();
		
		try {
			utils::multiNormal(temp, corrs, temp);
		} catch (const std::invalid_argument& e) {
			throw std::logic_error("BUG: TwoScaleGp uses invalid correlation matrix.\nOriginal error: " + std::string(e.what()));
		}
		
		utils::magToFlux(temp, temp);
	}
	
	// IMPORTANT: no exceptions past this point
	
	swap(fluxes, temp);
	commit(rng);
}

/** Allocates and initializes the covariance matrix for the 
 *	Gaussian process. 
 *
 * @return A pointer containing the new matrix
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the covariance matrix
 *
 * @exceptsafe The object is unchanged in the event of an exception
 *
 * @internal @exceptsafe The internal cache in getCovar() provides only the 
 *	basic exception guarantee. However, aside from run time this is not 
 *	visible to the rest of the program.
 */
shared_ptr<gsl_matrix> TwoScaleGp::getCovar() const {
	using std::swap;
	
	// Define a cache to prevent identical simulation runs from having to 
	//	recalculate the covariance
	const static utils::ApproxEqual cacheCheck(1e-12);
	// invariant: oldCov is empty <=> oldTimes is empty
	static shared_ptr<gsl_matrix> oldCov;
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
			|| oldTimes.size() != nTimes
			|| !std::equal(oldTimes.begin(), oldTimes.end(), 
					times.begin(), cacheCheck) ) {
		// Cache is out of date
		// Cache is out of date
		
		// copy-and-swap
		shared_ptr <gsl_matrix> temp(gsl_matrix_alloc(nTimes, nTimes), 
			&gsl_matrix_free);
		for(size_t i = 0; i < nTimes; i++) {
			for(size_t j = 0; j < nTimes; j++) {
				// i and j are valid indices for temp and times
				// therefore, no out-of-range errors
				double deltaTTau1 = (times[i] - times[j])/tau1;
				double deltaTTau2 = (times[i] - times[j])/tau2;
				gsl_matrix_set(temp.get(), i, j, 
					  sigma1*sigma1*exp(-0.5*deltaTTau1*deltaTTau1)
					+ sigma2*sigma2*exp(-0.5*deltaTTau2*deltaTTau2));
			}
		}
		
		// No exceptions beyond this point
		
		swap(oldCov, temp);
		swap(oldTimes, times);
		oldSigma1 = sigma1;
		oldSigma2 = sigma2;
		oldTau1   = tau1;
		oldTau2   = tau2;
	}
	
	// assert: the Cache is up-to-date
	
	// Return a copy of the cache as output
	shared_ptr<gsl_matrix> cov(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	gsl_matrix_memcpy(cov.get(), oldCov.get());

	return cov;
}

}}		// end lcmc::models
