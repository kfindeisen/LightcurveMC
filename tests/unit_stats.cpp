/** Test unit for statistics utilities
 * @file unit_stats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified April 28, 2013
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

using std::vector;

namespace lcmc { namespace utils {

void getHalfMatrix(const gsl_matrix* const a, gsl_matrix* & b);

}}	// end lcmc::utils

namespace lcmc { namespace stats {

void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		const std::string& statName);

}}	// end lcmc::stats

/** Data common to the test cases.
 *
 * At present, contains only sampling times
 */
class StatData {
public: 
	StatData() : ptfTimes(), unifTimes() {
		initTimes();
		for(size_t i = 0; i < TEST_LEN; i++) {
			unifTimes.push_back(static_cast<double>(i));
		}
	}
	
	~StatData() {
	}

	/** Number of times to use for a uniformly sampled data set
	 */
	const static size_t TEST_LEN = 100;

	/** Represents sampling at the irregular PTF cadence.
	 *
	 * This cadence has several features, particularly a long gap 
	 * between the 2009 and 2010 seasons, that tends to introduce 
	 * artifacts in statistical analyses.
	 */
	vector<double> ptfTimes;

	/** Represents sampling at a uniform daily cadence for 100 days.
	 *
	 * Statistical tests using this cadence should be 
	 *	well-behaved.
	 */
	vector<double> unifTimes;

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

/** Constructs a covariance matrix for a standard Gaussian process
 *
 * @param[in] covars A pointer to the matrix to initialize
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 *
 * @pre covars->size1 == covars->size2 == times.size()
 */
void initGauss(gsl_matrix* const covars, const vector<double>& times, double tau) {
	size_t nTimes = times.size();
	
	for(size_t i = 0; i < nTimes; i++) {
		for(size_t j = 0; j < nTimes; j++) {
			double deltaT = (times[i] - times[j]) / tau;
			gsl_matrix_set(covars, i, j, 
					2.0*exp(-0.5*deltaT*deltaT));
		}
	}
}

/** Function object for testing whether two values are approximately equal
 */
class ApproxEqual {
public: 
	/** Defines an object testing for approximate equality with 
	 *	a particular tolerance
	 *
	 * @param[in] epsilon The maximum fractional difference between two 
	 *	values that are considered equal
	 * 
	 * @post ApproxEqual will accept a pair of values as equal 
	 *	iff |val1 - val2|/|val1| and |val1 - val2|/|val2| < epsilon
	 */
	explicit ApproxEqual(double epsilon) : epsilon(epsilon) {
	}

	/** Tests whether two values are approximately equal
	 *
	 * @param[in] x, y The values to compare
	 * 
	 * @return true iff |x - y|/|x| and |x - y|/|y| < epsilon
	 */
	bool operator() (double x, double y) {
		using namespace ::boost::test_tools;
		
		predicate_result result = check_is_close(x, y, 
			fraction_tolerance_t<double>(epsilon));

		if (static_cast<bool>(result)) {
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

/** Tests whether lcmc::utils::getHalfMatrix() correctly decomposes a matrix into two factors
 *
 * The specification is tested directly; i.e. the output of getHalfMatrix() 
 * is multiplied by its transpose and the result compared to the original 
 * matrix. To guarantee that the input matrix is positive-nondefinite, 
 * currently the only matrix tested is the covariance matrix of a standard 
 * Gaussian process.
 *
 * @param[in] times The observation times at which the Gaussian process is sampled
 * @param[in] tau The coherence time of the Gaussian process
 */
void testProduct(double tau, const vector<double>& times)
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

/** Tests whether calls to multiNormal() consistent random vectors on 
 * different machines.
 *
 * The test case is a Gaussian process with &tau; = 2 days and with 
 *	the given sampling.
 *
 * @param[in] seed The seed used for the random number generator
 * @param[in] times The times at which the Gaussian process is sampled. The 
 *	sampling may be highly irregular.
 */
void testVectors(unsigned long int seed, const vector<double>& times)
{
	const static double tau = 2.0;
	const size_t nTimes = times.size();
	
	// Initialize the covariance matrix
	RaiiGsl<gsl_matrix> covar(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	initGauss(covar.get(), times, tau);

	// Initialize the noise vector
	RaiiGsl<gsl_rng> rng(gsl_rng_alloc(gsl_rng_mt19937), &gsl_rng_free);
	gsl_rng_set(rng.get(), seed);
	
	vector<double> fluxes;
	fluxes.reserve(nTimes);
	
	for(size_t i = 0; i < nTimes; i++) {
		fluxes.push_back(gsl_ran_ugaussian(rng.get()));
	}
	
	// Moment of truth...
	lcmc::utils::multiNormal(fluxes, *covar, fluxes);
	
	// Input
	char fileName[80];
	sprintf(fileName, "target_vectors_%lu.txt", seed);
	FILE* hOriginal = fopen(fileName, "r");
	if (hOriginal == NULL) {
		throw std::runtime_error("Could not open reference file.");
	}
	
	vector<double> oldFluxes;
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
		throw;
	}
	
	fclose(hOriginal);
	
	BOOST_CHECK(fluxes.size() == oldFluxes.size() && 
		std::equal(fluxes.begin(), fluxes.end(), oldFluxes.begin(), 
		ApproxEqual(1e-10)) );
}

/** Test cases for testing functions related to generating 
 *	multinormal distributions
 * @class Boost::Test::test_stats
 */
BOOST_FIXTURE_TEST_SUITE(test_stats, StatData)

/* @fn lcmc::utils::getHalfMatrix()
 *
 * @test given a Gaussian process covariance matrix corresponding to 
 *	uniform sampling and &tau; = 100, 50, 25, 10, 5, 2.5, or 1.0 days, 
 *	the result of getHalfMatrix() multiplied by its own transpose 
 *	equals the covariance matrix
 * @test given a Gaussian process covariance matrix corresponding to 
 *	PTF-like sampling and &tau; = 100, 50, 25, 10, 5, 2.5, or 1.0 days, 
 *	the result of getHalfMatrix() multiplied by its own transpose 
 *	equals the covariance matrix
 *
 * @todo Add broader test cases
 */
/** Tests whether getHalfMatrix() returns the correct decomposition of a matrix
 *
 * @see testProduct()
 */
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

/* @fn lcmc::utils::multiNormal()
 *
 * @test produces the same output on all platforms if the input 
 * 	random numbers were sampled using a gsl_rng_mt19937 generator 
 *	with seeds of 42, 43, 1196, 1764, 3125
 *
 * @todo Add more rigorous test cases
 */
/** Tests whether multiNormal() behaves consistently across platforms
 *
 * @see testVectors()
 */
BOOST_AUTO_TEST_CASE(vec_transformation) {
	testVectors(42,   unifTimes);
	testVectors(43,   unifTimes);
	testVectors(1196, unifTimes);
	testVectors(1764, unifTimes);
	testVectors(3125, unifTimes);
}

/* @fn lcmc::stats::getSummaryStats()
 *
 * @test given a constant vector equal to ln(2), returns a variance of 0
 */
/** Tests whether lcmc::stats::getSummaryStats() correctly handles 
 *	vectors with no variance
 */
BOOST_AUTO_TEST_CASE(zero_variance) {
	// Vector of constant flux
	// Bug doesn't come up if I use 1.0 as the value
	vector<double> constantSignal(ptfTimes.size(), log(2.0));
	
	double mean, stddev;
	lcmc::stats::getSummaryStats(constantSignal, mean, stddev, "Constant Signal Test");
	
	BOOST_CHECK(!gsl_isnan(stddev));
	BOOST_CHECK(stddev < 1e-12);
}

BOOST_AUTO_TEST_SUITE_END()
