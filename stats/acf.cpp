/** Temporary implementations of a lag-n autocorrelation function
 * @file lightcurveMC/stats/acf.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 12, 2013
 */

#include <stdexcept>
#include <string>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_statistics_double.h>
#include "acf.h"
#include "../except/undefined.h"

namespace lcmc { namespace stats { 

/** This function computes the lag-n autocorrelation of the dataset data, 
 *	using the signal-processing convention.
 *
 * The interface is deliberately written in C style, for ease of 
 *	swapping out with an eventual GSL function.
 *
 * @param[in] data, n The dataset to autocorrelate.
 * @param[out] acfs An array of length n that will contain the lag-0 
 *	through lag-(n-1) offsets
 *
 * @pre n >= 2
 *
 * @post @f$ acfs[k] = \sum_{i = k}^{n-1} x_i x_{i-k} @f$, where @f$ x_i @f$ denotes data[i]
 *
 * @exception std::invalid_argument Thrown if n < 2
 * @exception std::runtime_error Thrown if the internal calculations produce an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the autocorrelation function
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 *
 * @perform O(n^2) worst-case time
 * @perfmore O(n log n) best-case time
 */
void autoCorrelation_sp(const double data[], double acfs[], size_t n) {
	using boost::shared_ptr;
	
	if (n < 2) {
		throw std::invalid_argument("Cannot calculate autocorrelation function with fewer than 2 data points.");
	}
	
	// Possible optimization: cache the workspaces and only recalculate 
	//	them when n changes
	
	shared_ptr<gsl_fft_real_workspace> work(
		 gsl_fft_real_workspace_alloc(2*n), 
		&gsl_fft_real_workspace_free);
	if (work.get() == NULL) {
		throw std::bad_alloc();
	}
	
	// Zero-pad the workspace to avoid aliasing
	boost::scoped_array<double> transforms(new double[2*n]);
	for(size_t i = 0; i < n; i++) {
		transforms[i] = data[i];
	}
	for(size_t i = n; i < 2*n; i++) {
		transforms[i] = 0.0;
	}
	
	// Forward FFT
	{
		shared_ptr<gsl_fft_real_wavetable> forwardTable(
			 gsl_fft_real_wavetable_alloc(2*n), 
			&gsl_fft_real_wavetable_free);
		if (forwardTable.get() == NULL) {
			throw std::bad_alloc();
		}

		int status = gsl_fft_real_transform(transforms.get(), 1, 2*n, 
			forwardTable.get(), work.get());
		if (status) {
			throw std::runtime_error(std::string("While computing ACF: ") 
				+ gsl_strerror(status));
		}
	}

	// Take the squared amplitude of the FFT while preserving the 
	//	half-complex format
	transforms[0] *= transforms[0];
	// loop invariant: rpart is odd
	// loop invariant: transforms[rpart, rpart+1] represents the real and 
	//	imaginary parts of the (rpart+1)/2th complex number in the FFT
	// loop invariant: rpart <= 2*n-3
	for(size_t rpart = 1; rpart < 2 * n - 1; rpart += 2) {
		size_t ipart = rpart+1;
		transforms[rpart] = (transforms[rpart] * transforms[rpart]) 
			+ (transforms[ipart] * transforms[ipart]);
		transforms[ipart] = 0.0;
	}
	// Represents the real part of the nth complex number (imaginary 
	//	part is 0)
	transforms[2*n-1] *= transforms[2*n-1];
	
	// Inverse FFT
	{
		shared_ptr<gsl_fft_halfcomplex_wavetable> inverseTable(
			 gsl_fft_halfcomplex_wavetable_alloc(2*n), 
			&gsl_fft_halfcomplex_wavetable_free);
		if (inverseTable.get() == NULL) {
			throw std::bad_alloc();
		}
	
		int status = gsl_fft_halfcomplex_inverse(transforms.get(), 1, 2*n, 
			inverseTable.get(), work.get());
		if (status) {
			throw std::runtime_error(std::string("While computing ACF: ") 
				+ gsl_strerror(status));
		}
	}
	
	// IMPORTANT: no exceptions past this point
	
	// Include only the correct part of the ACF
	for(size_t i = 0; i < n; i++) {
		acfs[i] = transforms[i];
	}
}

/** This function computes the lag-n autocorrelation of the dataset data, 
 * 	using the statistical analysis convention.
 *
 * The interface is deliberately written in C style, for ease of 
 *	swapping out with an eventual GSL function.
 *
 * @param[in] data, n The dataset to autocorrelate.
 * @param[out] acfs An array of length n that will contain the lag-0 
 *	through lag-(n-1) offsets
 *
 * @pre n >= 2
 *
 * @post @f[ acfs[k] = \frac{\sum_{i = k}^{n-1} (x_{i} - \hat\mu) (x_{i-k} - \hat\mu)}
 * 	{\sum_{i = k}^{n-1} (x_{i} - \hat\mu) (x_{i} - \hat\mu)} @f]
 *	where @f$ x_i @f$ denotes data[i] and @f$ \hat\mu @f$ is the sample mean of data.
 *
 * @exception std::invalid_argument Thrown if n < 2
 * @exception std::runtime_error Thrown if the internal calculations produce an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the autocorrelation function
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 *
 * @perform O(n^2) worst-case time
 * @perfmore O(n log n) best-case time
 */
void autoCorrelation_stat(const double data[], double acfs[], size_t n) {
	if (n < 2) {
		throw except::NotEnoughData("Cannot calculate autocorrelation function with fewer than 2 data points.");
	}
	
	// Normalizations
	double mean =         gsl_stats_mean(data, 1, n);
	//double nVar = (n-1) * gsl_stats_variance_m(data, 1, n, mean);
	double nVar = gsl_stats_tss_m(data, 1, n, mean);
	
	// It's most efficient to subtract off the mean beforehand
	boost::scoped_array<double> zeroMean(new double[n]);
	for(size_t i = 0; i < n; i++) {
		zeroMean[i] = data[i] - mean;
	}

	// First function that changes acfs
	// since autoCorrelation_sp is atomic, and the normalization loop 
	//	doesn't throw exceptions, this function is also atomic
	autoCorrelation_sp(zeroMean.get(), acfs, n);
	for(size_t i = 0; i < n; i++) {
		acfs[i] /= nVar;
	}
}

}}		// end lcmc::stats
