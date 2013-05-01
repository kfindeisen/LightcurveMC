/** Defines the Stochastic base class.
 * @file lcstochastic.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified April 27, 2013
 */

#include <algorithm>
#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "lcsubtypes.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a particular function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @post getTimes() and getFluxes() return suitable data. getTimes() contains 
 * the same elements as times, possibly reordered.
 */
Stochastic::Stochastic(const std::vector<double> &times) 
		: ILightCurve(), times(times), fluxes(), fluxesSolved(false) {
	// Allow subclasses to assume times are in increasing order
	std::sort(this->times.begin(), this->times.end());
}

gsl_rng * const Stochastic::rng = Stochastic::init_rng();
/** Defines the random number state of the first Stochastic object.
 */
gsl_rng * Stochastic::init_rng() {
	gsl_rng * foo = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(foo, 42);
	return foo;
}

Stochastic::~Stochastic() {
}

/** Returns the times at which the simulated data were taken
 *
 * @param[out] timeArray A vector containing the desired times.
 *
 * @post timeArray.size() == getFluxes().size()
 */
void Stochastic::getTimes(std::vector<double>& timeArray) const {
	timeArray = times;
}

/** Returns the simulated fluxes at the corresponding times
 *
 * @param[out] fluxArray A vector containing the desired fluxes.
 *
 * @post fluxArray.size() == getTimes().size()
 * @post if getTimes()[i] == getTimes()[j] for i &ne; j, then 
 *	getFluxes()[i] == getFluxes()[j]
 * 
 * @post No element of fluxArray is NaN
 * @post All elements in fluxArray are non-negative
 * @post Either the mean, median, or mode of the flux is one, when 
 *	averaged over many elements and many light curve instances. 
 *	Subclasses of ILightCurve may chose the option 
 *	(mean, median, or mode) most appropriate for their light 
 *	curve shape.
 */
void Stochastic::getFluxes(std::vector<double>& fluxArray) const {
	if (!fluxesSolved) {
		solveFluxes(fluxes);
		fluxesSolved = true;
	}
	
	fluxArray = fluxes;
}

/** Returns the number of times and fluxes
 */
size_t Stochastic::size() const {
	return times.size();
}
	
/** Draws a standard uniform random variate.
 *
 * @return A number distributed uniformly over [0, 1), drawn independently 
 *	of any other calls to rUnif() or rNorm().
 */
double Stochastic::rUnif() const {
	return gsl_rng_uniform(rng);
}

/** Draws a standard normal random variate.
 *
 * @return A number distributed normally with mean 0 and variance 1, drawn 
 *	independently of any other calls to rUnif() or rNorm().
 *
 * @todo Benchmark which of several Gaussian generators works best on cowling
 */
double Stochastic::rNorm() const {
	return gsl_ran_ugaussian(rng);
}

}}		// end lcmc::models
