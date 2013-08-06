/** Defines the SimpleGp light curve class.
 * @file lightcurveMC/waves/lcgp1.cpp
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified August 1, 2013
 */

#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include "../except/data.h"
#include "lightcurves_gp.h"

namespace lcmc { namespace models {

using boost::lexical_cast;
using boost::shared_ptr;
//using std::auto_ptr;

/** Initializes the light curve to represent a standard Gaussian process.
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] sigma The root-mean-square amplitude of the Gaussian process
 * @param[in] tau The correlation time of the Gaussian process.
 *
 * @pre @p sigma > 0
 * @pre @p tau > 0
 *
 * @post The object represents a correlated Gaussian signal with the 
 *	given amplitude and correlation time.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the parameters 
 *	are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
SimpleGp::SimpleGp(const std::vector<double>& times, double sigma, double tau) 
		: GaussianProcess(times), sigma(sigma), tau(tau) {
	if (sigma <= 0.0) {
		throw except::BadParam("All SimpleGp light curves need positive standard deviations (gave " 
			+ lexical_cast<std::string>(sigma) + ").");
	}
	if (tau <= 0.0) {
		throw except::BadParam("All SimpleGp light curves need positive coherence times (gave " 
			+ lexical_cast<std::string>(tau) + ").");
	}
}

/** Approximate comparison function that handles zeros cleanly.
 */
bool cacheCheck(double x, double y);

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
shared_ptr<gsl_matrix> SimpleGp::getCovar() const {
	using std::swap;
	
	// Define a cache to prevent identical simulation runs from having to 
	//	recalculate the covariance
	// invariant: oldCov is empty <=> oldTimes is empty
	static shared_ptr<gsl_matrix> oldCov;
	static std::vector<double> oldTimes;
	static double oldSigma = 0.0;
	static double oldTau = 0.0;

	std::vector<double> times;
	this->getTimes(times);
	size_t nTimes = times.size();

	if(oldCov.get() == NULL 
			|| !cacheCheck(oldSigma, sigma)
			|| !cacheCheck(oldTau, tau)
			|| oldTimes.size() != nTimes
			|| !std::equal(oldTimes.begin(), oldTimes.end(), 
					times.begin(), &cacheCheck) ) {
		// Cache is out of date
		
		// copy-and-swap
		shared_ptr <gsl_matrix> temp(gsl_matrix_alloc(nTimes, nTimes), 
			&gsl_matrix_free);
		for(size_t i = 0; i < nTimes; i++) {
			for(size_t j = 0; j < nTimes; j++) {
				// i and j are valid indices for temp and times
				// therefore, no out-of-range errors
				double deltaTTau = (times[i] - times[j])/tau;
				gsl_matrix_set(temp.get(), i, j, 
						sigma*sigma*exp(-0.5*deltaTTau*deltaTTau));
			}
		}
		
		// No exceptions beyond this point
		
		swap(oldCov, temp);
		swap(oldTimes, times);
		oldSigma = sigma;
		oldTau = tau;
	}
	
	// assert: the Cache is up-to-date
	
	// Return a copy of the cache as output
	shared_ptr<gsl_matrix> cov(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	gsl_matrix_memcpy(cov.get(), oldCov.get());

	return cov;
}

}}		// end lcmc::models
