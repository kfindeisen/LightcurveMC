/** Functions for fitting Gaussian Process models to data
 * @file lightcurveMC/stats/gpfit.cpp
 * @author Krzysztof Findeisen
 * @date Created June 24, 2013
 * @date Last modified June 26, 2013
 */

#include <limits>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include "../r_compat.h"
#include "../except/undefined.h"

namespace lcmc { namespace stats {

using std::string;
using std::vector;
using boost::lexical_cast;
using boost::shared_ptr;

/** Finds the best fit solution to a squared exponential Gaussian process model
 * 
 * @param[in] times The times at which the light curve was sampled.
 * @param[in] data The values of the light curve (typically fluxes or magnitudes)
 * @param[out] timescale The best-fit value of the model timescale
 * @param[out] timeError The uncertainty on the model timescale
 * 
 * @pre @p times contains at least two unique values
 * @pre @p times.size() = @p data.size()
 * @pre @p data[i] is the measurement taken at @p time[i], for all i
 * @pre Neither @p times nor @p data may contain NaNs
 * 
 * @post @p timescale and @p timeError contain the best-fit estimate 
 *	of the correlation timescale for a Gaussian process model
 * @post @p timescale > 0
 * @post @p timeError &ge; 0 or @p timeError is NaN
 * 
 * @perform O(N<sup>3</sup>) time, where N = @p times.size()
 * @perfmore O(N<sup>2</sup>) memory
 * 
 * @exception lcmc::utils::except::UnexpectedNan Thrown if there are any 
 *	NaN values present in @p times or @p data.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times and @p data do 
 *	not have at least two values. 
 * @exception std::invalid_argument Thrown if @p times and @p data 
 *	do not have the same length.
 * @exception std::runtime_error Thrown if the internal calculations produce 
 *	an error.
 * @exception std::bad_alloc Thrown if there is not enough memory to fit 
 *	the model.
 * 
 * @exceptsafe The program is in a consistent state in the event 
 *	of an exception.
 */
void fitGaussGp(const vector<double>& times, const vector<double>& data, 
		double& timescale, double& timeError) {
	shared_ptr<RInside> r = getRInstance();
	
	if (times.size() < 2) {
		throw except::NotEnoughData("Cannot fit Gaussian process model with fewer than 2 data points (gave " 
			+ lexical_cast<string>(times.size()) + ").");
	}
	if (times.size() != data.size()) {
		throw std::invalid_argument("Data and time arrays passed to fitGaussGp() must have the same length (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>(data.size()) + " for data)");
	}

	Rcpp::NumericVector rTimes(times.begin(), times.end(), static_cast<int>(times.size()));
	Rcpp::NumericVector rData (data .begin(), data .end(), static_cast<int>(data .size()));
	
	(*r)["times"] = rTimes;
	(*r)["data" ] = rData;
	
	r->parseEvalQ("suppressMessages(library(gptk))");
	r->parseEvalQ("gpSettings <- gpOptions('ftc')");	// Exact solution
//	r->parseEvalQ("gpSettings <- gpOptions('dtc')");	// Approximation
//	r->parseEvalQ("gpSettings$numActive <- 50");		// Number of samples in approx.
	r->parseEvalQ("gpSettings$kern$type <- 'cmpnd'");	// GP + noise
	r->parseEvalQ("gpSettings$kern$comp <- list('rbf', 'white')");
	
	r->parseEvalQ("model <- gpCreate(1, 1, as.matrix(times), as.matrix(data), gpSettings)");
	r->parseEvalQ("model <- gpOptimise(model, 0, 1000)");

	r->parseEvalQ("tau <- 1.0/sqrt(model$kern$comp[[1]]$inverseWidth)");
	timescale = Rcpp::as<double>((*r.get())["tau"]);
	// IMPORTANT: no exceptions beyond this point
	timeError = std::numeric_limits<double>::quiet_NaN();
}

}}		// end lcmc::stats
