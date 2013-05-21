/** Utilities for test suite
 * @file lightcurveMC/tests/test_common.cpp
 * @author Krzysztof Findeisen
 * @date Created April 28, 2013
 * @date Last modified May 20, 2013
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
#include "test.h"
#include "../lightcurvetypes.h"

namespace lcmc { 

/** This namespace contains all the code dedicated to carrying out unit 
 *	tests, including components of the Boost.Test library.
 */
namespace test {

using boost::lexical_cast;
using std::vector;

/** Wrapper for a trusted Nan-testing function
 *
 * @param[in] x The number to test
 *
 * @return true iff x is one of the NaN values
 *
 * @exceptsafe Does not throw exceptions.
 */
bool testNan(const double x) {
	return static_cast<bool>(gsl_isnan(x));
}

/** Constructs a covariance matrix for a standard Gaussian process
 *
 * @param[in] covars A pointer to the matrix to initialize
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 *
 * @pre covars->size1 == covars->size2 == times.size()
 *
 * @exception Throws std::invalid_argument if lengths do not match
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
void initGauss(gsl_matrix* const covars, const vector<double>& times, double tau) {
	size_t nTimes = times.size();
	
	if(covars->size1 != covars->size2) {
		try {
			std::string len1 = lexical_cast<std::string>(covars->size1);
			std::string len2 = lexical_cast<std::string>(covars->size2);
			throw std::invalid_argument(len1 + "×" + len2 
				+ " covariance matrix passed to initGauss().");
		} catch (const boost::bad_lexical_cast &e) {
			throw std::invalid_argument(
				"Non-square covariance matrix passed to initGauss().");
		}
	}
	if(nTimes != covars->size1) {
		try {
			std::string lenV = lexical_cast<std::string>(nTimes);
			std::string lenM = lexical_cast<std::string>(covars->size1);
			throw std::invalid_argument("Vector of length " + lenV 
				+ " cannot be multiplied by " 
				+ lenM + "×" + lenM + " covariance matrix in initGauss().");
		} catch (const boost::bad_lexical_cast &e) {
			throw std::invalid_argument(
				"Length of input vector to initGauss() does not match dimensions of covariance matrix.");
		}
	}

	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / tau;
			gsl_matrix_set(covars, i, j, 
					2.0*exp(-0.5*deltaT*deltaT));
		}
	}
}

/** This function is a wrapper for a trusted approximate comparison method.
 * 
 * @param[in] val1, val2 The values to compare
 * @param[in] frac The fractional difference allowed between them
 * 
 * @return true iff |val1 - val2|/|val1| and |val1 - val2|/|val2| < frac
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
 * @return true iff |val1 - val2|/|val1| and |val1 - val2|/|val2| < frac
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
		: TestFactory(), rng(gsl_rng_alloc(gsl_rng_mt19937)) {
	if (rng == NULL) {
		throw std::bad_alloc();
	}
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
