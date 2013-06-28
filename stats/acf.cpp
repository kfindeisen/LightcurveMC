/** Temporary implementations of a lag-n autocorrelation function
 * @file lightcurveMC/stats/acf.cpp
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 26, 2013
 */

#include <stdexcept>
#include <string>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_statistics_double.h>
#include "acf.h"
#include "../gsl_compat.h"
#include "../except/undefined.h"
#include "../../common/alloc.tmp.h"

namespace lcmc { namespace stats { 

/** Takes the squared amplitude of a half-complex vector
 *
 * @param[in,out] hcArr A vector assumed to be in half-complex format.
 * @param[in] n The length of @p hcArr
 *
 * @pre @p n > 0
 *
 * @post @p hcArr is modified in place
 *
 * @exception std::invalid_argument Thrown if @p n &le; 0
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 */
void squareAmpHalfComplex(double hcArr[], size_t n) {
	if (n <= 0) {
		throw std::invalid_argument("Cannot square half-complex array with zero length.");
	}

	// Take the squared amplitude of the FFT while preserving the 
	//	half-complex format
	hcArr[0] *= hcArr[0];
	// loop invariant: rpart is odd
	// loop invariant: hcArr[rpart, rpart+1] represents the real and 
	//	imaginary parts of the (rpart+1)/2th complex number in the FFT
	// loop invariant: rpart <= n-3 if n is even, and rpart <= n-2 if n is odd
	for(size_t rpart = 1; rpart < n-1; rpart += 2) {
		size_t ipart = rpart+1;
		hcArr[rpart] = (hcArr[rpart] * hcArr[rpart]) 
			+ (hcArr[ipart] * hcArr[ipart]);
		hcArr[ipart] = 0.0;
	}
	if (GSL_IS_EVEN(n)) {
		// Represents the real part of the nth complex number (imaginary 
		//	part is 0)
		hcArr[n-1] *= hcArr[n-1];
	}
}

/** This function computes the lag-@p n autocorrelation of the dataset @p data, 
 *	using the signal-processing convention.
 *
 * The interface is deliberately written in C style, for ease of 
 *	swapping out with an eventual GSL function.
 *
 * @param[in] data, n The dataset to autocorrelate.
 * @param[out] acfs An array of length @p n that will contain the lag-0 
 *	through lag-(@p n-1) offsets
 *
 * @pre @p n &ge; 2
 *
 * @post @f$ a_k = \sum_{i = k}^{n-1} x_i x_{i-k} @f$, where @f$ a_k @f$ 
 *	denotes @p acf[k] and @f$ x_i @f$ denotes @p data[i]
 *
 * @perform O(<tt>n</tt><sup>2</sup>) worst-case time
 * @perfmore O(<tt>n</tt> log <tt>n</tt>) best-case time
 *
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p n < 2
 * @exception std::runtime_error Thrown if the internal calculations produce an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the autocorrelation function
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 */
void autoCorrelation_sp(const double data[], double acfs[], size_t n) {
	using boost::shared_ptr;
	using kpfutils::checkAlloc;
	
	if (n < 2) {
		throw except::NotEnoughData("Cannot calculate autocorrelation function with fewer than 2 data points.");
	}
	
	// Possible optimization: cache the workspaces and only recalculate 
	//	them when n changes
	
	shared_ptr<gsl_fft_real_workspace> work(
		checkAlloc(gsl_fft_real_workspace_alloc(2*n)), 
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
			checkAlloc(gsl_fft_real_wavetable_alloc(2*n)), 
			          &gsl_fft_real_wavetable_free);

		gslCheck( gsl_fft_real_transform(transforms.get(), 1, 2*n, 
			forwardTable.get(), work.get()), "While computing ACF: ");
	}

	// Take the squared amplitude of the FFT while preserving the 
	//	half-complex format
	squareAmpHalfComplex(transforms.get(), 2*n);
	
	// Inverse FFT
	{
		shared_ptr<gsl_fft_halfcomplex_wavetable> inverseTable(
			checkAlloc(gsl_fft_halfcomplex_wavetable_alloc(2*n)), 
			          &gsl_fft_halfcomplex_wavetable_free);
	
		gslCheck( gsl_fft_halfcomplex_inverse(transforms.get(), 1, 2*n, 
			inverseTable.get(), work.get()), "While computing ACF: ");
	}
	
	// IMPORTANT: no exceptions past this point
	
	// Include only the correct part of the ACF
	for(size_t i = 0; i < n; i++) {
		acfs[i] = transforms[i];
	}
}

/** This function computes the lag-@p n autocorrelation of the dataset @p data, 
 * 	using the statistical analysis convention.
 *
 * The interface is deliberately written in C style, for ease of 
 *	swapping out with an eventual GSL function.
 *
 * @param[in] data, n The dataset to autocorrelate.
 * @param[out] acfs An array of length @p n that will contain the lag-0 
 *	through lag-(@p n-1) offsets
 *
 * @pre @p n &ge; 2
 *
 * @post @f[ a_k = \frac{\sum_{i = k}^{n-1} (x_{i} - \hat\mu) (x_{i-k} - \hat\mu)}
 * 	{\sum_{i = k}^{n-1} (x_{i} - \hat\mu) (x_{i} - \hat\mu)} @f]
 *	where where @f$ a_k @f$ denotes @p acf[k], @f$ x_i @f$ denotes 
 *	@p data[i], and @f$ \hat\mu @f$ is the sample mean of @p data.
 *
 * @perform O(<tt>n</tt><sup>2</sup>) worst-case time
 * @perfmore O(<tt>n</tt> log <tt>n</tt>) best-case time
 *
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p n < 2
 * @exception std::runtime_error Thrown if the internal calculations produce an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the autocorrelation function
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
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
