/** Test unit for statistics utilities
 * @file unit_stats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified April 21, 2013
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

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

#include <string>
#include <cmath>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include "../raiigsl.tmp.h"

namespace lcmc { namespace utils {

void getHalfMatrix(const gsl_matrix* const a, gsl_matrix* & b);

}}	// end lcmc::utils

// Data common to the test cases
/*class ObsData {
public: 
//	const static size_t TEST_LEN       = 100;
	const static size_t TEST_COUNT     =  10000;
//	const static double TEST_TOLERANCE = 1e-10;

	ObsData() : times() {
		double minTStep, maxTStep;
		FILE* hJulDates = fopen("ptfjds.txt", "r");
		if (hJulDates == NULL) {
			throw std::runtime_error("Could not open ptfjds.txt.");
		}
		readTimeStamps(hJulDates, times, minTStep, maxTStep);
		fclose(hJulDates);
		hJulDates = NULL;
	}
	
	~ObsData() {
	}

	// Times at which all light curves are sampled
	std::vector<double> times;
};

BOOST_FIXTURE_TEST_SUITE(test_gp, ObsData)
*/

void testProduct(double sigma)
{
	const static size_t TEST_LEN = 100;
	
	RaiiGsl<gsl_matrix> initial(gsl_matrix_alloc(TEST_LEN, TEST_LEN), &gsl_matrix_free);
	for(size_t i = 0; i < TEST_LEN; i++) {
		for(size_t j = 0; j < TEST_LEN; j++) {
			double deltaT = (static_cast<double>(i) - static_cast<double>(j)) 
					/ sigma;
			gsl_matrix_set(initial.get(), i, j, 
					2.0*exp(-0.5*deltaT*deltaT));
		}
	}
	
	// RaiiGsl does not allow its internal pointer to be changed
	// So assign to a dummy pointer, then add RaiiGsl once the value is fixed
	gsl_matrix * temp = NULL;
	lcmc::utils::getHalfMatrix(initial.get(), temp);
	RaiiGsl<gsl_matrix> result(temp, &gsl_matrix_free);
	
	RaiiGsl<gsl_matrix> product(gsl_matrix_calloc(TEST_LEN, TEST_LEN), &gsl_matrix_free);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, result.get(), result.get(), 
			0.0, product.get());

	RaiiGsl<gsl_matrix> residuals(gsl_matrix_calloc(TEST_LEN, TEST_LEN), &gsl_matrix_free);
	gsl_matrix_memcpy(residuals.get(), product.get());
	gsl_matrix_sub (residuals.get(), initial.get());
	
	bool lowCheck  = (gsl_matrix_min(residuals.get()) > -1e-12);
	bool highCheck = (gsl_matrix_max(residuals.get()) <  1e-12);
	if(!highCheck || !lowCheck) {
		BOOST_ERROR("Residuals too high!");
		BOOST_WARN_MESSAGE(lowCheck, "Min residuals: " 
				<< gsl_matrix_min(residuals.get()));
		BOOST_WARN_MESSAGE(highCheck, "Max residuals: " 
				<< gsl_matrix_max(residuals.get()));

		BOOST_MESSAGE("Min initial: " << gsl_matrix_min(initial.get()));
		BOOST_MESSAGE("Max initial: " << gsl_matrix_max(initial.get()));
		size_t i, j;
		gsl_matrix_min_index(residuals.get(), &i, &j);
		BOOST_WARN_MESSAGE(lowCheck, "Initial @min: " 
				<< gsl_matrix_get(initial.get(), i, j));
		gsl_matrix_max_index(residuals.get(), &i, &j);
		BOOST_WARN_MESSAGE(highCheck, "Initial @max: " 
				<< gsl_matrix_get(initial.get(), i, j));

		BOOST_MESSAGE("Min product: " << gsl_matrix_min(product.get()));
		BOOST_MESSAGE("Max product: " << gsl_matrix_max(product.get()));
		gsl_matrix_min_index(residuals.get(), &i, &j);
		BOOST_WARN_MESSAGE(lowCheck, "Product @min: " 
				<< gsl_matrix_get(product.get(), i, j));
		gsl_matrix_max_index(residuals.get(), &i, &j);
		BOOST_WARN_MESSAGE(highCheck, "Product @max: " 
				<< gsl_matrix_get(product.get(), i, j));
	} else {
		BOOST_CHECK(true);
	}
}

BOOST_AUTO_TEST_SUITE(test_stats)

BOOST_AUTO_TEST_CASE(decomposition) {
	testProduct(100.0);
	testProduct( 50.0);
	testProduct( 25.0);
	testProduct( 10.0);
	testProduct(  5.0);
	testProduct(  2.5);
	testProduct(  1.0);
}

BOOST_AUTO_TEST_SUITE_END()
