/** Utilities for test suite
 * @file lightcurveMC/tests/test_common.cpp
 * @author Krzysztof Findeisen
 * @date Created April 28, 2013
 * @date Last modified May 27, 2013
 */

#include "../warnflags.h"

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include "../gsl_compat.tmp.h"
#include "../lightcurvetypes.h"
#include "test.h"

namespace lcmc { 

/** This namespace contains all the code dedicated to carrying out unit 
 *	tests, including components of the Boost.Test library.
 */
namespace test {

using boost::lexical_cast;
using std::vector;
using utils::checkAlloc;

/** Wrapper for a trusted Nan-testing function
 *
 * @param[in] x The number to test
 *
 * @return true iff @p x is one of the NaN values
 *
 * @exceptsafe Does not throw exceptions.
 */
bool testNan(const double x) {
	return static_cast<bool>(gsl_isnan(x));
}

/** Constructs a covariance matrix for a white Gaussian process
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 *
 * @return A pointer to a newly allocated covariance matrix.
 *
 * @pre @p times.size() > 0
 *
 * @post return->size1 = return->size2 = @p times.size()
 * @post return[i, j] = \f$\delta(t_i, t_j)\f$, where \f$t_i\f$ = @p times[i]
 *
 * @exception std::invalid_argument Thrown if @p times is empty.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
shared_ptr<gsl_matrix> initGaussWn(const vector<double>& times) {
	size_t nTimes = times.size();
	
	if (nTimes == 0) {
		throw std::invalid_argument("Empty times vector passed to initGaussWn().");
	}

	shared_ptr<gsl_matrix> covars(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);

	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			// ALlow for times[i] == times[j] when i != j
			gsl_matrix_set(covars.get(), i, j, 
				2.0*(times[i] == times[j] ? 1 : 0));
		}
	}
	
	return covars;
}

/** Constructs a covariance matrix for a squared exponential Gaussian process
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 *
 * @return A pointer to a newly allocated covariance matrix.
 *
 * @pre @p times.size() > 0
 *
 * @post return->size1 = return->size2 = @p times.size()
 * @post return[i, j] = \f$e^{-(t_i - t_j)^2/2\tau^2}\f$, 
 *	where \f$t_i\f$ = @p times[i]
 *
 * @exception std::invalid_argument Thrown if @p times is empty.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
shared_ptr<gsl_matrix> initGaussSe(const vector<double>& times, double tau) {
	size_t nTimes = times.size();
	
	if (nTimes == 0) {
		throw std::invalid_argument("Empty times vector passed to initGaussSe().");
	}

	shared_ptr<gsl_matrix> covars(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);

	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / tau;
			gsl_matrix_set(covars.get(), i, j, 
					2.0*exp(-0.5*deltaT*deltaT));
		}
	}
	
	return covars;
}

/** Constructs a covariance matrix for an exponential Gaussian process
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 *
 * @return A pointer to a newly allocated covariance matrix.
 *
 * @pre @p times.size() > 0
 *
 * @post return->size1 = return->size2 = @p times.size()
 * @post return[i, j] = \f$e^{-|t_i - t_j|/\tau}\f$, where \f$t_i\f$ = @p times[i]
 *
 * @exception std::invalid_argument Thrown if @p times is empty.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
shared_ptr<gsl_matrix> initGaussDrw(const vector<double>& times, double tau) {
	size_t nTimes = times.size();
	
	if (nTimes == 0) {
		throw std::invalid_argument("Empty times vector passed to initGaussDrw().");
	}

	shared_ptr<gsl_matrix> covars(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);

	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / tau;
			gsl_matrix_set(covars.get(), i, j, 
					2.0*exp(-fabs(deltaT)) );
		}
	}
	
	return covars;
}

/** Constructs a covariance matrix for a Matérn Gaussian process
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 * @param[in] nu The order of the Bessel function
 *
 * @return A pointer to a newly allocated covariance matrix.
 *
 * @pre @p times.size() > 0
 * @pre @p nu > 0
 *
 * @post return->size1 = return->size2 = @p times.size()
 * @post return[i, j] = \f$\frac{2^{1-\nu}}{\Gamma(\nu)} 
 \left(\frac{\sqrt{2\nu}|t_i - t_j|}{\tau}\right)^\nu 
 K_\nu\left(\frac{\sqrt{2\nu}|t_i - t_j|}{\tau}\right)\f$, 
 * 	where \f$t_i\f$ = @p times[i]
 *
 * @exception std::invalid_argument Thrown if @p times is empty.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
shared_ptr<gsl_matrix> initGaussMat(const vector<double>& times, double tau, double nu);

/** Constructs a covariance matrix for a periodic Gaussian process
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 * @param[in] period The period of the Gaussian process
 *
 * @return A pointer to a newly allocated covariance matrix.
 *
 * @pre @p times.size() > 0
 *
 * @post return->size1 = return->size2 = @p times.size()
 * @post return[i, j] = \f$e^{-2\sin^2(2\pi \frac{t_i - t_j}{P})/\tau^2}\f$, 
 *	where \f$t_i\f$ = @p times[i]
 *
 * @exception std::invalid_argument Thrown if @p times is empty.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
shared_ptr<gsl_matrix> initGaussP(const vector<double>& times, double tau, double period) {
	size_t nTimes = times.size();
	
	if (nTimes == 0) {
		throw std::invalid_argument("Empty times vector passed to initGaussP().");
	}

	shared_ptr<gsl_matrix> covars(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);

	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / period;
			double sine   = sin(2.0*M_PI * deltaT) / tau;
			gsl_matrix_set(covars.get(), i, j, 
					2.0*exp(-2.0 * sine*sine) );
		}
	}
	
	return covars;
}

/** Constructs a covariance matrix for a rational Gaussian process
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 * @param[in] alpha The exponent of the process.
 *
 * @return A pointer to a newly allocated covariance matrix.
 *
 * @pre @p times.size() > 0
 *
 * @post return->size1 = return->size2 = @p times.size()
 * @post return[i, j] = \f$1/(1 + (t_i - t_j)^2/\tau^2)^\alpha\f$, 
 *	where \f$t_i\f$ = @p times[i]
 *
 * @exception std::invalid_argument Thrown if @p times is empty.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
shared_ptr<gsl_matrix> initGaussR(const vector<double>& times, double tau, double alpha) {
	size_t nTimes = times.size();
	
	if (nTimes == 0) {
		throw std::invalid_argument("Empty times vector passed to initGaussR().");
	}

	shared_ptr<gsl_matrix> covars(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);

	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / tau;
			gsl_matrix_set(covars.get(), i, j, 
					2.0 / pow(1 + deltaT*deltaT, alpha) );
		}
	}
	
	return covars;
}


/** This function is a wrapper for a trusted approximate comparison method.
 * 
 * @param[in] val1, val2 The values to compare
 * @param[in] frac The fractional difference allowed between them
 * 
 * @return true iff |val1 - val2|/|val1| and |val1 - val2|/|val2| < @p frac
 *
 * @exceptsafe Does not throw exceptions.
 */
bool isClose(double val1, double val2, double frac) {
	using namespace ::boost::test_tools;
	
	return static_cast<bool>(check_is_close(val1, val2, 
			fraction_tolerance_t<double>(frac)));
}

/** This function emulates the BOOST_CHECK_CLOSE_FRACTION macro.
 *
 * The macro crashes whenever the test fails, making it too unstable to use. 
 * This function behaves identically to the macro, except that it cannot 
 * report the code that triggered the test.
 *
 * @param[in] val1, val2 The values to compare
 * @param[in] frac The fractional difference allowed between them
 * 
 * @return true iff |@p val1 - @p val2|/|@p val1| and |@p val1 - @p val2|/|@p val2| < @p frac
 *
 * @exceptsafe Does not throw exceptions.
 */
void myTestClose(double val1, double val2, double frac) {
	using namespace ::boost::test_tools;
	
	predicate_result result = check_is_close(val1, val2, 
			fraction_tolerance_t<double>(frac));
	char buf[256];
	sprintf(buf, "floating point comparison failed: [%.10g != %.10g] (%.10g >= %.10g)", val1, val2, fabs(val1-val2), frac);
	
	BOOST_CHECK_MESSAGE(static_cast<bool>(result), buf);
}

TestFactory::~TestFactory() {
}

/** Initializes the random number generator
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to allocate 
 *	the random number generator.
 * 
 * @exceptsafe Object creation is atomic
 */
TestRandomFactory::TestRandomFactory() 
		: TestFactory(), rng(checkAlloc(gsl_rng_alloc(gsl_rng_mt19937))) {
	gsl_rng_set(rng, 5489);
}

TestRandomFactory::~TestRandomFactory() {
	gsl_rng_free(rng);
}

/** Draws a standard uniform variate.
 *
 * @return a number distributed uniformly over the interval [0, 1)
 *
 * @exceptsafe Does not throw exceptions.
 */
double TestRandomFactory::sample() const {
	return gsl_rng_uniform(rng);
}

}}		// end lcmc::test
