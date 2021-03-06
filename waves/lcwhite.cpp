/** Defines the WhiteNoise light curve class.
 * @file lightcurveMC/waves/lcwhite.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified August 1, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#include <memory>
#include <stdexcept>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include "../except/data.h"
#include "../fluxmag.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

using std::auto_ptr;
using boost::lexical_cast;
using boost::shared_ptr;

/** Initializes the light curve to represent a white noise process.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] sigma The root-mean-square amplitude of the white noise
 *
 * @pre @p sigma > 0
 *
 * @post The object represents an uncorrelated Gaussian signal with the 
 *	given amplitude.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the parameters 
 *	are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
WhiteNoise::WhiteNoise(const std::vector<double>& times, double sigma) 
		: GaussianProcess(times), sigma(sigma) {
	if (sigma <= 0.0) {
		throw except::BadParam("All WhiteNoise light curves need positive standard deviations (gave " 
			+ lexical_cast<string>(sigma) + ").");
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
 * @post fluxToMag(@p fluxes) has a mean of zero and a standard deviation of sigma
 * @post cov(fluxToMag(fluxes[i]), fluxToMag(fluxes[j])) = 0 
 *	if getTimes()[i] &ne; getTimes()[j]
 * 
 * @perform O(N) time, where N = @p times.size()
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */	
void WhiteNoise::solveFluxes(std::vector<double>& fluxes) const {
	using std::swap;

	// invariant: this->times() is sorted in ascending order
	std::vector<double> times;
	this->getTimes(times);
	
	// copy-and-swap
	auto_ptr<StochasticRng> rng = checkout();
	std::vector<double> temp;

	if (times.size() > 0) {
		temp.reserve(times.size());

		// f(t0) ~ N(0, sigma) to avoid giving first point any special treatment
		temp.push_back(sigma * rng->rNorm());
		
		// Evaluate f(t > t0)
		for(std::vector<double>::const_iterator it = times.begin()+1; 
				it != times.end(); it++) {
			// Observations taken at the same time should have the same flux
			// Sorting invariant guarantees that all duplicate times will 
			//	be next to each other
			if (*it == *(it-1)) {
				temp.push_back(temp.back());
			// Observations taken at different times are uncorrelated
			} else {
				temp.push_back(sigma * rng->rNorm());
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
shared_ptr<gsl_matrix> WhiteNoise::getCovar() const {
	throw std::logic_error("Unexpected call to WhiteNoise::getCovar().");
}

}}		// end lcmc::models
