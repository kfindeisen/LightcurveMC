/** Defines the RandomWalk light curve class.
 * @file lcrw.cpp
 * @author Krzysztof Findeisen
 * @date Created April 29, 2013
 * @date Last modified May 12, 2013
 */

#include <memory>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include "../except/data.h"
#include "../fluxmag.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

using std::auto_ptr;
using boost::lexical_cast;

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
 * @exception bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception except::BadParam Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
RandomWalk::RandomWalk(const std::vector<double>& times, double diffus) 
		: Stochastic(times), d(diffus) {
	if (diffus <= 0.0) {
		throw except::BadParam("All RandomWalk light curves need positive diffusion coefficients (gave " 
			+ lexical_cast<string, double>(diffus) + ").");
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
 * @post fluxToMag(fluxes) has a mean of zero and a standard deviation, at 
 *	each time t, of sqrt(diffus*(t-getTimes()[0]))
 * @post cov(fluxToMag(fluxes[i]), fluxToMag(fluxes[j])) == 
 *	diffus * (min{getTimes()[i], getTimes()[j]} - getTimes()[0])
 * 
 * @exception bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception logic_error Thrown if a bug was found in the flux calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */	
void RandomWalk::solveFluxes(std::vector<double>& fluxes) const {
	using std::swap;

	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	// copy-and-swap
	auto_ptr<StochasticRng> rng = checkout();
	std::vector<double> temp;

	if (times.size() > 0) {
		temp.reserve(times.size());

		// A damped random walk is self-similar regardless of its starting value
		temp.push_back(0.0);
		
		// Evaluate f(t > t0)
		// Derived analogously to eq. 2.47 in Gillespie (1996)
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
				double deltaT = (*it - *(it-1));
				
				temp.push_back(oldFlux + sqrt(d * deltaT) * rng->rNorm());
			}
		}
		
		utils::magToFlux(temp, temp);
	}
		
	// IMPORTANT: no exceptions past this point
		
	swap(fluxes, temp);
	commit(rng);
}

}}		// end lcmc::models
