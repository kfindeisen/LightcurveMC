/** Test unit for statistics utilities
 * @file unit_stats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified April 26, 2013
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

#include <algorithm>
#include <string>
#include <cmath>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "../mcio.h"
#include "../raiigsl.tmp.h"

#include "../waves/generators.h"

namespace lcmc { namespace utils {

void getHalfMatrix(const gsl_matrix* const a, gsl_matrix* & b);

}}	// end lcmc::utils

// Data common to the test cases
class ObsData {
public: 
	const static size_t TEST_LEN = 100;

	ObsData() : ptfTimes(), unifTimes() {
		initTimes();
		for(size_t i = 0; i < TEST_LEN; i++) {
			unifTimes.push_back(static_cast<double>(i));
		}
	}
	
	~ObsData() {
	}

	// Times at which all light curves are sampled
	std::vector<double> ptfTimes, unifTimes;

private:
	void initTimes() {
		double minTStep, maxTStep;
		FILE* hJulDates = fopen("ptfjds.txt", "r");
		if (hJulDates == NULL) {
			throw std::runtime_error("Could not open ptfjds.txt.");
		}
		readTimeStamps(hJulDates, ptfTimes, minTStep, maxTStep);
		fclose(hJulDates);
		hJulDates = NULL;
	}
};

void initGauss(gsl_matrix* const covars, const std::vector<double>& times, double tau) {
	size_t nTimes = times.size();
	
	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / tau;
			gsl_matrix_set(covars, i, j, 
					2.0*exp(-0.5*deltaT*deltaT));
		}
	}
}

class ApproxEqual {
public: 
	explicit ApproxEqual(double epsilon) : epsilon(epsilon) {
	}
	bool operator() (double x, double y) {
		if (gsl_fcmp(x, y, epsilon) == 0) {
			return true;
		} else {
			char buf[256];
			sprintf(buf, "floating point comparison failed: [%.10g != %.10g] (%.10g >= %.10g)", 
					x, y, fabs(x-y), epsilon);
			BOOST_WARN_MESSAGE(false, buf);
			return false;
		}
	}
private:
	double epsilon;
};

void testProduct(double tau, const std::vector<double>& times)
{
	const size_t nTimes = times.size();
	
	RaiiGsl<gsl_matrix> initial(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	initGauss(initial.get(), times, tau);
	
	// RaiiGsl does not allow its internal pointer to be changed
	// So assign to a dummy pointer, then add RaiiGsl once the value is fixed
	gsl_matrix * temp = NULL;
	lcmc::utils::getHalfMatrix(initial.get(), temp);
	RaiiGsl<gsl_matrix> result(temp, &gsl_matrix_free);
	
	RaiiGsl<gsl_matrix> product(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, result.get(), result.get(), 
			0.0, product.get());

	RaiiGsl<gsl_matrix> residuals(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);
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

void testVectors(unsigned long int seed, const std::vector<double>& times)
{
	const static double tau = 2.0;
	const size_t nTimes = times.size();
	
	// Initialize the covariance matrix
	RaiiGsl<gsl_matrix> covar(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	initGauss(covar.get(), times, tau);

	// Initialize the noise vector
	RaiiGsl<gsl_rng> rng(gsl_rng_alloc(gsl_rng_mt19937), &gsl_rng_free);
	gsl_rng_set(rng.get(), seed);
	
	std::vector<double> fluxes;
	fluxes.reserve(nTimes);
	
	for(size_t i = 0; i < nTimes; i++) {
		fluxes.push_back(gsl_ran_ugaussian(rng.get()));
	}
	
	// Moment of truth...
	lcmc::utils::multiNormal(fluxes, *covar, fluxes);
	
	// Input
	char fileName[80];
	sprintf(fileName, "run_test_vectors_%lu.txt", seed);
	FILE* hOriginal = fopen(fileName, "r");
	if (hOriginal == NULL) {
		throw std::runtime_error("Could not open reference file.");
	}
	
	std::vector<double> oldFluxes;
	oldFluxes.reserve(nTimes);
	try {
		while (!feof(hOriginal)) {
			double curFlux;
			if (oldFluxes.empty()) {
				fscanf(hOriginal, "%lf", &curFlux);
			} else {
				fscanf(hOriginal, ", %lf", &curFlux);
			}
			oldFluxes.push_back(curFlux);
		}
	} catch (std::exception& e) {
		fclose(hOriginal);
		BOOST_ERROR("Could not finish reading from " << fileName);
		BOOST_ERROR(e.what());
		throw;
	}
	
	fclose(hOriginal);
	
	BOOST_CHECK(fluxes.size() == oldFluxes.size() && 
		std::equal(fluxes.begin(), fluxes.end(), oldFluxes.begin(), 
		ApproxEqual(1e-10)) );
}

BOOST_FIXTURE_TEST_SUITE(test_stats, ObsData)

BOOST_AUTO_TEST_CASE(decomposition) {
	testProduct(100.0, unifTimes);
	testProduct( 50.0, unifTimes);
	testProduct( 25.0, unifTimes);
	testProduct( 10.0, unifTimes);
	testProduct(  5.0, unifTimes);
	testProduct(  2.5, unifTimes);
	testProduct(  1.0, unifTimes);

	testProduct(100.0,  ptfTimes);
	testProduct( 50.0,  ptfTimes);
	testProduct( 25.0,  ptfTimes);
	testProduct( 10.0,  ptfTimes);
	testProduct(  5.0,  ptfTimes);
	testProduct(  2.5,  ptfTimes);
	testProduct(  1.0,  ptfTimes);
}

BOOST_AUTO_TEST_CASE(vec_transformation) {
	testVectors(42,   unifTimes);
	testVectors(43,   unifTimes);
	testVectors(1196, unifTimes);
	testVectors(1764, unifTimes);
	testVectors(3125, unifTimes);
}

BOOST_AUTO_TEST_SUITE_END()
