/** Port of Ann Marie's IDL code fragment to a C++ function
 * @file lightcurveMC/stats/acfinterp.cpp
 * @author Ann Marie Cody
 * @author Krzysztof Findeisen
 * @date Created May 6, 2013
 * @date Last modified May 26, 2013
 */

#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_spline.h>
#include "acfinterp.h"
#include "../gsl_compat.h"
#include "../nan.h"
#include "../except/nan.h"
#include "../except/undefined.h"
#include "../../common/vecarray.tmp.h"
#include "../../common/alloc.tmp.h"

#ifndef _GSL_HAS_ACF
#include "acf.h"
#endif

namespace lcmc { namespace stats { namespace interp {

using std::string;
using std::vector;
using boost::lexical_cast;
using boost::scoped_array;
using boost::shared_array;
using boost::shared_ptr;

/** Generates a C array of evenly spaced values
 *
 * @param[in] min The minimum value in the grid
 * @param[in] max An upper limit on the grid
 * @param[in] step The grid spacing
 *
 * @param[out] grid,n A newly allocated array containing the grid.
 *
 * @pre @p min < @p max
 * @pre @p step > 0
 *
 * @post @p min = min(@p grid)
 * @post @p max - @p step &le; max(@p grid) < @p max
 * @post for all i &isin; [0, n-2], @p grid[i+1] - @p grid[i] = @p step
 *
 * @exception std::invalid_argument Thrown if @p max &le; @p min 
 *	or @p step &le; 0
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct @p grid.
 *
 * @exceptsafe Function arguments are unchanged in the event 
 *	of an exception.
 */
void evenGrid(double min, double max, double step, shared_array<double>& grid, size_t& n) {
	if (step <= 0) {
		throw std::runtime_error("Need positive step in evenGrid() (gave " 
			+ lexical_cast<string>(step) + ")");
	}
	if (max <= min) {
		throw std::runtime_error("Need min < max in evenGrid() (gave " 
			+ lexical_cast<string>(min) + " and " 
			+ lexical_cast<string>(max) + ")");
	}
	
	// Don't update n until the array is allocated
	size_t nAlloc = static_cast<size_t>(ceil( (max-min)/step ));

	grid.reset(new double[nAlloc]);
	
	// IMPORTANT: no exceptions beyond this point
	
	n = nAlloc;
	for(size_t i = 0; i < nAlloc; i++) {
		grid[i] = min + i*step;
	}
}


/** Calculates the autocorrelation function for a time series. 
 * 
 * @param[in] times	Times at which data were taken
 * @param[in] data	The data (typically fluxes or magnitudes) measured 
 *			at each time
 * @param[in] deltaT, nAcf The spacing and number of grid points over which 
 *			the ACF should be calculated. The grid will run from 
 *			lags of 0 to (<tt>nAcf</tt>-1)*<tt>deltaT</tt>.
 * @param[out] acf	The value of the autocorrelation function at each 
 *			offset.
 *
 * @pre @p times contains at least two unique values
 * @pre @p times is sorted in ascending order
 * @pre @p data.size() = @p times.size()
 * @pre @p data[i] is the measurement taken at @p times[i], for all i
 * @pre neither @p times nor @p data contains NaNs
 * @pre @p deltaT > 0.0
 * @pre @p nAcf > 0
 *
 * @post the data previously in @p acf are erased
 * @post @p acf.size() = @p nAcf
 * @post @p acf[i] is an estimator of the autocorrelation function evaluated 
 *	at &Delta;t = i*<tt>deltaT</tt>, for all i
 * @post @p acf[i] = 0 if i*<tt>deltaT</tt> > max(@p times) - min(@p times)
 *
 * @perform O(F<sup>2</sup>) worst-case time, where 
 *	F = ceil((max(@p times)-min(@p times))/<tt>deltaT</tt>)
 * @perfmore O(F log F) best-case time
 * 
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in @p times or @p data.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times and @p data do 
 *	not have at least two values. 
 * @exception std::invalid_argument Thrown if @p times and @p data 
 *	do not have the same length or if @p deltaT or @p nAcf are not positive.
 * @exception std::runtime_error Thrown if the internal calculations produce an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
 *	the autocorrelation function
 *
 * @exceptsafe The function parameters are unchanged in the event of 
 *	an exception.
 */
void autoCorr(const DoubleVec &times, const DoubleVec &data, 
		double deltaT, size_t nAcf, DoubleVec &acf) {
	using std::swap;
	using kpfutils::checkAlloc;

	const size_t NOLD = times.size();
	if (NOLD < 2) {
		throw except::NotEnoughData("Cannot calculate autocorrelation function with fewer than 2 data points (gave " 
			+ lexical_cast<string>(NOLD) + ").");
	}
	if (NOLD != data.size()) {
		throw std::invalid_argument("Data and time arrays passed to autoCorr() must have the same length (gave " 
			+ lexical_cast<string>(NOLD) + " for times and " 
			+ lexical_cast<string>(data.size()) + " for data)");
	}
	if (deltaT <= 0) {
		throw std::invalid_argument("Need a positive time lag to construct an autocorrelation grid (gave " 
			+ lexical_cast<string>(deltaT) + ")");
	}
	if (nAcf <= 0) {
		throw std::invalid_argument("Must calculate autocorrelation function at a positive number of points " 
			+ lexical_cast<string>(nAcf) + ")");
	}

	// In C++11, can directly return an array view of times and data
	// For now, make a copy
	shared_array<double> tempTimes = kpfutils::vecToArr(times);
	shared_array<double> tempData  = kpfutils::vecToArr(data );
	
	// Generate a grid with spacing deltaT
	shared_array<double> evenTimes;
	size_t nNew;
	evenGrid(times.front(), times.back(), deltaT, evenTimes, nNew);

	// Interpolate to the new grid
	// Despite its name, gsl_spline is not necessarily a spline
	shared_ptr<gsl_spline> interp(checkAlloc(gsl_spline_alloc(gsl_interp_linear, NOLD)), 
		          &gsl_spline_free);
	shared_ptr<gsl_interp_accel> interpTable(checkAlloc(gsl_interp_accel_alloc()), 
		&gsl_interp_accel_free);
	gslCheck( gsl_spline_init(interp.get(), tempTimes.get(), tempData.get(), NOLD), 
			"While computing ACF: ");
	
	scoped_array<double> evenData(new double[nNew]);
	for(size_t i = 0; i < nNew; i++) {
		gslCheck( gsl_spline_eval_e(interp.get(), evenTimes[i], 
			interpTable.get(), evenData.get()+i), "While computing ACF: ");
	}

	// ACF
	scoped_array<double> tempAcfs(new double[nNew]);
	#ifdef _GSL_HAS_ACF
	Unknown specification. Please update this when GSL ACF available.
	#else
	autoCorrelation_stat(evenData.get(), tempAcfs.get(), nNew);
	#endif
		
	// Convert to vector and trim off lags longer than (nAcf-1)*deltaT
	vector<double> temp = kpfutils::arrToVec(tempAcfs.get(), std::min(nAcf, nNew));

	if (nNew < nAcf) {
		temp.insert(temp.end(), nAcf-nNew, 0.0);
	}

	// IMPORTANT: no exceptions past this point
	
	swap(acf, temp);
}

}}}		// end lcmc::stats::interp
