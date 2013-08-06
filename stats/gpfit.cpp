/** Functions for fitting Gaussian Process models to data
 * @file lightcurveMC/stats/gpfit.cpp
 * @author Krzysztof Findeisen
 * @date Created June 24, 2013
 * @date Last modified July 31, 2013
 */

#include <limits>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include "../approx.h"
#include "../nan.h"
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
 * @param[out] timeError The estimated uncertainty on the model timescale
 * 
 * @pre @p times contains at least two unique values
 * @pre @p times.size() = @p data.size()
 * @pre for all i, @p data[i] is the measurement taken at @p time[i]
 * @pre Neither @p times nor @p data may contain NaNs
 * 
 * @post @p timescale and @p timeError contain the best-fit estimate 
 *	of the correlation timescale for a Gaussian process model
 * @post @p timescale > 0
 * @post @p timeError > 0
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
			+ lexical_cast<string>( data.size()) + " for data)");
	}
	
	Rcpp::NumericVector rTimes(times.begin(), times.end(), static_cast<int>(times.size()));
	Rcpp::NumericVector rData (data .begin(), data .end(), static_cast<int>(data .size()));
	
	(*r)["times"] = rTimes;
	(*r)["data" ] = rData;
	
	// Unfortunately, there seems to be no way to source() files relative 
	// to the directory containing the lightcurveMC program... it must 
	// be relative to a hardcoded path or to the lightcurveMC *working* 
	// directory
	r->parseEvalQ("suppressMessages(library(gptk))");
	r->parseEvalQ("suppressMessages(library(numDeriv))");
	r->parseEvalQ("gpSettings <- gpOptions('ftc')");	// Exact solution; approximations are no faster
	r->parseEvalQ("gpSettings$optimiser <- 'CG'");		// SCG gives low quality solutions
	r->parseEvalQ("gpSettings$kern$type <- 'cmpnd'");	// GP + noise
	r->parseEvalQ("gpSettings$kern$comp <- list('rbf', 'white')");
	r->parseEvalQ("gpSettings$scaleVal <- sd(data)");	// Equivalent to $scale2var1, but doesn't produce errors
	
	// Solve for the best fit
	r->parseEvalQ("model <- gpCreate(1, 1, as.matrix(times), as.matrix(data), gpSettings)");
	// Conjugate gradient optimization is supposed to converge within N iterations, 
	//	where N is the size of the kernel matrix
	// 10% margin for normal truncation errors
	r->parseEvalQ("model <- gpOptimise(model, 0, 1.1*length(times))");
	r->parseEvalQ("tau <- 1.0/sqrt(model$kern$comp[[1]]$inverseWidth)");

	// Estimate the errors
	r->parseEvalQ("hess <- jacobian(function(p) {gpGradient(p, model)}, gpExtractParam(model))");
	// If likelihood maximized poorly, Hessian matrix will be asymmetric
	r->parseEvalQ("ele13 <- hess[1,3]");
	r->parseEvalQ("ele31 <- hess[3,1]");
	if ( !utils::ApproxEqual(1e-4)(Rcpp::as<double>((*r)["ele13"]), 
			Rcpp::as<double>((*r)["ele31"])) ) {
		throw std::runtime_error("In fitGaussGp(), Hessian matrix is asymmetric. This probably means the fit is not a local likelihood maximum.");
	}

	r->parseEvalQ("covar <- solve(hess)");
	// Covariance matrix is for ln(1/tau^2), ln(amp^2), ln(noise^2), in that order
	r->parseEvalQ("err <- 0.5 * sqrt(covar[1,1]) * tau");

	try {
		// copy-and-swap
		double tempTime = Rcpp::as<double>((*r)["tau"]);
		double tempErr  = Rcpp::as<double>((*r)["err"]);
		// Rcpp will convert NAs to NaNs without error, but we don't 
		// want to allow those solutions
		if (utils::isNan(tempTime)) {
			throw std::runtime_error("In fitGaussGp(), code ran successfully, but time scale was NA");
		}
		if (utils::isNan(tempErr)) {
			throw std::runtime_error("In fitGaussGp(), found a time scale, but time scale error was NA");
		}

		// IMPORTANT: no exceptions beyond this point

		timescale = tempTime;
		timeError = tempErr;
	} catch (const std::exception& e) {
		// Only std::exception can catch all possible Rcpp errors
		throw std::runtime_error(e.what());
	}

}

}}		// end lcmc::stats
