/** Port of Ann Marie's IDL code fragment to a C++ function
 * @file acfinterp.cpp
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 6, 2013
 * @date Last modified May 8, 2013
 */

#include <vector>
#include <cmath>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_spline.h>
#include "acfinterp.h"
#include "../gsl_compat.h"

#ifndef _GSL_HAS_ACF
#include "acf.h"
#endif

namespace lcmc { namespace stats { namespace interp {

using std::vector;
using boost::scoped_array;
using boost::shared_ptr;

/** Calculates the autocorrelation function for a time series. 
 * 
 * @param[in] times	Times at which data were taken
 * @param[in] data	The data (typically fluxes or magnitudes) measured 
 *			at each time
 * @param[in] deltaT, nAcf The spacing and number of grid points over which 
 *			the ACF should be calculated. The grid will run from 
 *			lags of 0 to (nAcf-1)*deltaT.
 * @param[out] acf	The value of the autocorrelation function at each 
 *			offset.
 *
 * @pre times contains at least two unique values
 * @pre times is sorted in ascending order
 * @pre data.size() == times.size()
 * @pre data[i] is the measurement taken at times[i], for all i
 * @pre neither times nor data contains NaNs
 * @pre deltaT > 0.0
 * @pre nAcf > 0
 *
 * @post the data previously in acf are erased
 * @post acf.size() == nAcf
 * @post acf[i] is an estimator of the autocorrelation function evaluated 
 *	at &Delta;t = i*deltaT, for all i
 * @post acf[i] == 0 if i*deltaT > max(times) - min(times)
 * 
 * @exception ??? Thrown if times has a single value.
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in times or data.
 * @exception lcmc::utils::except::LengthMismatch Thrown if times and data 
 *	do not have the same length.
 * @exception ??? Thrown if deltaT or nAcf are not positive.
 *
 * @todo Incorporate input validation
 *
 * @exceptsafe TBD
 *
 * @perform 
 */
void autoCorr(const DoubleVec &times, const DoubleVec &data, 
		double deltaT, size_t nAcf, DoubleVec &acf) {

	const size_t NOLD = times.size();

	// Generate a grid with spacing deltaT
	const size_t NNEW = static_cast<size_t>(ceil((times.back()-times.front())/deltaT));
	scoped_array<double> evenTimes(new double[NNEW]);
	for(size_t i = 0; i < NNEW; i++) {
		evenTimes[i] = times.front() + i*deltaT;
	}
	// assert: times.back() - deltaT <= evenTimes[NNEW-1] < times.back()

	// In C++11, can directly return an array view of times and data
	// For now, make a copy
	scoped_array<double> tempTimes(new double[NOLD]);
	for(size_t i = 0; i < NOLD; i++) {
		tempTimes[i] = times[i];
	}
	scoped_array<double> tempData(new double[NOLD]);
	for(size_t i = 0; i < NOLD; i++) {
		tempData[i] = data[i];
	}
	
	// Interpolate to the new grid
	// Despite its name, gsl_spline is not necessarily a spline
	shared_ptr<gsl_spline> dataInterp(gsl_spline_alloc(gsl_interp_linear, NOLD), 
		&gsl_spline_free);
	gsl_spline_init(dataInterp.get(), tempTimes.get(), tempData.get(), NOLD);
	shared_ptr<gsl_interp_accel> interpTable(gsl_interp_accel_alloc(), 
		&gsl_interp_accel_free);
	
	scoped_array<double> evenData(new double[NNEW]);
	for(size_t i = 0; i < NNEW; i++) {
		evenData[i] = gsl_spline_eval(dataInterp.get(), evenTimes[i], 
			interpTable.get());
	}

	// ACF
	scoped_array<double> tempAcfs(new double[NNEW]);
	#ifdef _GSL_HAS_ACF
	Unknown specification. Please update this when GSL ACF available.
	#else
	autoCorrelation_stat(evenData.get(), tempAcfs.get(), NNEW);
	#endif
		
	// Convert to vector and trim off lags longer than (nAcf-1)*deltaT
	acf.reserve(nAcf);
	acf.clear();
	for(size_t i = 0; i < nAcf && i < NNEW; i++) {
		acf.push_back(tempAcfs[i]);
	}
	// If (nAcf-1)*deltaT > times.back() - times.front(), set the 
	//	remaining values to 0. Note that if NNEW > nAcf this 
	//	loop has no effect
	for(size_t i = NNEW; i < nAcf; i++) {
		acf.push_back(0.0);
	}
}

}}}		// end lcmc::stats::interp
