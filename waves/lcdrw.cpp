/** Defines the DampedRandomWalk light curve class.
 * @file lightcurveMC/waves/lcdrw.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified August 1, 2013
 */

#include <memory>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include "../except/data.h"
#include "../fluxmag.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

using std::auto_ptr;
using boost::lexical_cast;
using boost::shared_ptr;

/** Initializes the light curve to represent a damped random walk.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] diffus The diffusion constant of the damped random walk, in 
 *	units of mag<sup>2</sup>/time
 * @param[in] tau The damping time of the damped random walk.
 *
 * @pre @p diffus > 0
 * @pre @p tau > 0
 *
 * @post The object represents a damped random walk with the given diffusion 
 *	constant and correlation time.
 *
 * @perform O(N) time, where N = @p times.size()
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the parameters 
 *	are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
DampedRandomWalk::DampedRandomWalk(const std::vector<double>& times, double diffus, double tau) 
		: GaussianProcess(times), sigma(sqrt(0.5*diffus*tau)), tau(tau) {
	if (diffus <= 0.0) {
		throw except::BadParam("All DampedRandomWalk light curves need positive diffusion coefficients (gave " 
			+ lexical_cast<string>(diffus) + ").");
	}
	if (tau <= 0.0) {
		throw except::BadParam("All DampedRandomWalk light curves need positive coherence times (gave " 
			+ lexical_cast<string>(tau) + ").");
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
 * @post @p fluxes.size() = getTimes().size()
 * @post if getTimes()[i] = getTimes()[j] for i &ne; j, then 
 *	@p fluxes[i] = @p fluxes[j]
 * 
 * @post No element of @p fluxes is NaN
 * @post All elements in @p fluxes are non-negative
 * @post The median of the flux is one, when averaged over many elements and 
 *	many light curve instances.
 *
 * @post fluxToMag(@p fluxes) has a mean of zero and a standard deviation of 
 *	sqrt(0.5*@p diffus/@p tau)
 * @post cov(fluxToMag(@p fluxes[i]), fluxToMag(@p fluxes[j])) = 
 *	0.5*<tt>diffus</tt>/<tt>tau</tt> &times; exp(|getTimes()[i]-getTimes()[j]|/tau) 
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */	
void DampedRandomWalk::solveFluxes(std::vector<double>& fluxes) const {
	using std::swap;

	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	// copy-and-swap
	auto_ptr<StochasticRng> rng = checkout();
	std::vector<double> temp;

	if (times.size() > 0) {
		temp.reserve(times.size());

		// f(t0) ~ N(0, sigma) to ensure self-similarity
		temp.push_back(sigma * rng->rNorm());
		
		// Evaluate f(t > t0)
		// This simplified algorithm works only for an exponential covariance, 
		//	a white noise process, or a constant process, since it 
		//	requires rho(t1,t3) = rho(t1,t2)*rho(t2,t3). However, it's 
		//	much faster than the general-purpose, matrix-based algorithm.
		for(std::vector<double>::const_iterator it = times.begin()+1; 
				it != times.end(); it++) {
			double oldFlux = temp.back();
			
			// Observations taken at the same time should have the same flux
			// Sorting invariant guarantees that all duplicate times will 
			//	be next to each other
			if (*it == *(it-1)) {
				temp.push_back(oldFlux);
			// Observations taken at different times are partially 
			//	correlated
			} else {
				// Implement Equation 2.47 of Gillespie (1996)
				// His algorithm is not worth the trouble, since it 
				//	relies heavily on deltaT being constant
				double deltaTTau  = (*it - *(it-1))/tau;
				
				temp.push_back(oldFlux*exp(-deltaTTau) 
					+ sigma*sqrt(1.0 - exp(-2.0*deltaTTau)) 
						* rng->rNorm());
			}
		}
		
		utils::magToFlux(temp, temp);
	}
		
	// IMPORTANT: no exceptions past this point
		
	swap(fluxes, temp);
	commit(rng);
}

/** Allocates and initializes the covariance matrix for the 
 *	Gaussian process. 
 */
shared_ptr<gsl_matrix> DampedRandomWalk::getCovar() const {
	throw std::logic_error("Unexpected call to DampedRandomWalk::getCovar().");
}

}}		// end lcmc::models
