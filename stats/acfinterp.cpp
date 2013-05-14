/** Port of Ann Marie's IDL code fragment to a C++ function
 * @file acfinterp.cpp
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 6, 2013
 * @date Last modified May 12, 2013
 */

#include <string>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_spline.h>
#include "acfinterp.h"
#include "../gsl_compat.h"
#include "../nan.h"
#include "../except/nan.h"
#include "../except/undefined.h"

#ifndef _GSL_HAS_ACF
#include "acf.h"
#endif

namespace lcmc { namespace stats { namespace interp {

using std::vector;
using boost::lexical_cast;
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
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in times or data.
 * @exception except::NotEnoughData Thrown if times and data do not 
 *	have at least two values. 
 * @exception std::invalid_argument Thrown if times and data 
 *	do not have the same length or if deltaT or nAcf are not positive.
 * @exception std::runtime_error Thrown if the internal calculations produce an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the autocorrelation function
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 *
 * @perform O(F^2) worst-case time, where F == ceil((max(times)-min(times))/deltaT)
 * @perfmore O(F log F) best-case time
 * 
 * @todo Break up this function
 */
void autoCorr(const DoubleVec &times, const DoubleVec &data, 
		double deltaT, size_t nAcf, DoubleVec &acf) {

	const size_t NOLD = times.size();
	if (NOLD < 2) {
		throw except::NotEnoughData("Cannot calculate autocorrelation function with fewer than 2 data points (gave " 
			+ lexical_cast<std::string,size_t>(NOLD) + ").");
	}
	if (NOLD != data.size()) {
		throw std::invalid_argument("Data and time arrays passed to autoCorr() must have the same length (gave " 
			+ lexical_cast<std::string,size_t>(NOLD) + " and " 
			+ lexical_cast<std::string,size_t>(data.size()) + ")");
	}
	if (deltaT <= 0) {
		throw std::invalid_argument("Need a positive time lag to construct an autocorrelation grid (gave " 
			+ lexical_cast<std::string,double>(deltaT) + ")");
	}
	if (nAcf <= 0) {
		throw std::invalid_argument("Must calculate autocorrelation function at a positive number of points " 
			+ lexical_cast<std::string,size_t>(nAcf) + ")");
	}

	// In C++11, can directly return an array view of times and data
	// For now, make a copy
	scoped_array<double> tempTimes(new double[NOLD]);
	for(size_t i = 0; i < NOLD; i++) {
		if (utils::isNan(times[i])) {
			throw utils::except::UnexpectedNan("NaN found in times given to autoCorr()");
		}
		tempTimes[i] = times[i];
	}
	scoped_array<double> tempData(new double[NOLD]);
	for(size_t i = 0; i < NOLD; i++) {
		if (utils::isNan(data[i])) {
			throw utils::except::UnexpectedNan("NaN found in data given to autoCorr()");
		}
		tempData[i] = data[i];
	}
	
	// Generate a grid with spacing deltaT
	const size_t NNEW = static_cast<size_t>(ceil((times.back()-times.front())/deltaT));
	scoped_array<double> evenTimes(new double[NNEW]);
	for(size_t i = 0; i < NNEW; i++) {
		evenTimes[i] = times.front() + i*deltaT;
	}
	// assert: times.back() - deltaT <= evenTimes[NNEW-1] < times.back()

	// Interpolate to the new grid
	// Despite its name, gsl_spline is not necessarily a spline
	shared_ptr<gsl_spline> dataInterp(gsl_spline_alloc(gsl_interp_linear, NOLD), 
		&gsl_spline_free);
	if (dataInterp.get() == NULL) {
		throw std::bad_alloc();
	}
	int status = gsl_spline_init(dataInterp.get(), tempTimes.get(), tempData.get(), NOLD);
	if (status) {
		throw std::runtime_error(std::string("While computing ACF: ") 
			+ gsl_strerror(status));
	}
	shared_ptr<gsl_interp_accel> interpTable(gsl_interp_accel_alloc(), 
		&gsl_interp_accel_free);
	if (interpTable.get() == NULL) {
		throw std::bad_alloc();
	}
	
	scoped_array<double> evenData(new double[NNEW]);
	for(size_t i = 0; i < NNEW; i++) {
		status = gsl_spline_eval_e(dataInterp.get(), evenTimes[i], 
			interpTable.get(), evenData.get()+i);
		if (status) {
			throw std::runtime_error(std::string("While computing ACF: ") 
				+ gsl_strerror(status));
		}
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
	
	// IMPORTANT: no exceptions past this point
	
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
