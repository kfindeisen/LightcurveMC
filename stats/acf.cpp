/** Temporary implementations of a lag-n autocorrelation function
 * @file acf.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 7, 2013
 */

#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_statistics_double.h>
#include "acf.h"

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
 * @post @f$ acfs[k] = \sum_{i = k}^{n-1} x_i x_{i-k} @f$, where @f$ x_i @f$ denotes data[i]
 *
 * @todo Incorporate input validation
 *
 * @perform O(n^2) worst-case time
 * @perfmore O(n log n) best-case time
 */
void autoCorrelation_sp(const double data[], double acfs[], size_t n) {
	using boost::shared_ptr;
	
	// Possible optimization: cache the workspaces and only recalculate 
	//	them when n changes
	
	shared_ptr<gsl_fft_real_workspace> work(
		 gsl_fft_real_workspace_alloc(2*n), 
		&gsl_fft_real_workspace_free);
	
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
	
		gsl_fft_real_transform(transforms.get(), 1, 2*n, 
			forwardTable.get(), work.get());
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
	
		gsl_fft_halfcomplex_inverse(transforms.get(), 1, 2*n, 
			inverseTable.get(), work.get());
	}
	
	// Include only the correct part
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
 * @post @f[ acfs[k] = \frac{\sum_{i = k}^{n-1} (x_{i} - \hat\mu) (x_{i-k} - \hat\mu)}
 * 	{\sum_{i = k}^{n-1} (x_{i} - \hat\mu) (x_{i} - \hat\mu)} @f]
 *	where @f$ x_i @f$ denotes data[i] and @f$ \hat\mu @f$ is the sample mean of data.
 *
 * @todo Incorporate input validation
 *
 * @perform O(n^2) worst-case time
 * @perfmore O(n log n) best-case time
 */
void autoCorrelation_stat(const double data[], double acfs[], size_t n) {
	// Normalizations
	double mean =         gsl_stats_mean(data, 1, n);
	double nVar = (n-1) * gsl_stats_variance_m(data, 1, n, mean);
	
	// It's most efficient to subtract off the mean beforehand
	boost::scoped_array<double> zeroMean(new double[n]);
	for(size_t i = 0; i < n; i++) {
		zeroMean[i] = data[i] - mean;
	}
	
	autoCorrelation_sp(zeroMean.get(), acfs, n);
	for(size_t i = 0; i < n; i++) {
		acfs[i] /= nVar;
	}
}

}}		// end lcmc::stats
