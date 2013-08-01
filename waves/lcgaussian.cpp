/** Defines the GaussianProcess light curve class.
 * @file lightcurveMC/waves/lcgaussian.cpp
 * @author Krzysztof Findeisen
 * @date Created August 1, 2013
 * @date Last modified August 1, 2013
 */

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>
//#include <cmath>
//#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include "../approx.h"
//#include "../except/data.h"
#include "../fluxmag.h"
#include "generators.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

//using boost::lexical_cast;
using boost::shared_ptr;
using std::auto_ptr;

/** Initializes the light curve to represent a Gaussian process.
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post The object represents an instance of a Gaussian process light curve.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 *
 * @exceptsafe Object construction is atomic.
 */
GaussianProcess::GaussianProcess(const std::vector<double>& times) : Stochastic(times) {
}

/** Computes a realization of the light curve. 
 *
 * The light curve is computed from @p times and the internal random 
 * number generator, and its values are stored in @p fluxes.
 *
 * This implementation of solveFluxes() uses a generic, but slow, method of 
 * generating Gaussian process instances. If a more efficient algorithm is 
 * available for a particular type of Gaussian process, you should override 
 * this function with a more specialized implementation.
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
 * @perform O(N<sup>3</sup>) time, where N = @p times.size()
 * @perfmore O(N<sup>2</sup>) memory
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the light curve.
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
void GaussianProcess::solveFluxes(std::vector<double>& fluxes) const {
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
			throw std::logic_error("Gaussian process uses invalid correlation matrix.\nOriginal error: " + std::string(e.what()));
		}
		
		utils::magToFlux(temp, temp);
	}
	
	// IMPORTANT: no exceptions past this point
	
	swap(fluxes, temp);
	commit(rng);
}

/** Approximate comparison function that handles zeros cleanly.
 *
 * Intended for use only by implementations of GaussianProcess::getCovar()
 *
 * @param[in] x, y The values to compare
 *
 * @return true iff (x = y = 0) or (|@p x - @p y|/|@p x| and |@p x - @p y|/|@p y| < @p epsilon)
 *
 * @exception std::bad_alloc Thrown if could not compute approximate comparison test.
 *
 * @exceptsafe The parameters and object state are unchanged in the event 
 *	of an exception. Does not throw after the first call.
 *
 * @todo Is there any way to handle bad_alloc internally? Letting it get 
 *	thrown exposes the function implementation.
 */
bool cacheCheck(double x, double y) {
	const static utils::ApproxEqual approx(1e-12);
	
	if (x == 0.0 || y == 0.0) {
		return (x == y);
	} else {
		return approx(x, y);
	}
}

}}		// end lcmc::models
