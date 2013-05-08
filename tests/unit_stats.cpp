/** Test unit for statistics utilities
 * @file unit_stats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 4, 2013
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
#include <stdexcept>
#include <string>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "../stats/acfinterp.h"
#include "../approx.h"
#include "../lcsio.h"
#include "../mcio.h"
#include "test.h"

#include "../waves/generators.h"

using std::vector;
using boost::shared_ptr;

namespace lcmc { namespace utils {

void getHalfMatrix(const gsl_matrix* const a, gsl_matrix* & b);

}}	// end lcmc::utils

// Ignore this declaration, since for some reason it confuses Doxygen
/// @cond
namespace lcmc { namespace stats {

void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		const std::string& statName);

void autoCorrelation_stat(const double data[], double acfs[], size_t n);

}}	// end lcmc::stats
/// @endcond

namespace lcmc { namespace test {

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
	
	shared_ptr<gsl_matrix> initial(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
	initGauss(initial.get(), times, tau);
	
	// shared_ptr does not allow its internal pointer to be changed
	// So assign to a dummy pointer, then give it to shared_ptr once the 
	//	pointer target is known
	gsl_matrix * temp = NULL;
	lcmc::utils::getHalfMatrix(initial.get(), temp);
	shared_ptr<gsl_matrix> result(temp, &gsl_matrix_free);
	
	shared_ptr<gsl_matrix> product(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, result.get(), result.get(), 
			0.0, product.get());

	shared_ptr<gsl_matrix> residuals(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);
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

/** Test cases for testing functions related to generating 
 *	multinormal distributions
 * @class BoostTest::test_stats
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
	
	BOOST_CHECK(!testNan(stddev));
	BOOST_CHECK(stddev < 1e-12);
}

/* @fn lcmc::stats::autoCorrelation_stat()
 *
 * @test Consistent results with a_correlate() procedure from IDL
 */
/** Tests whether lcmc::stats::autoCorrelation_stat() matches existing 
 *	autocorrelation implementations from other languages
 */
BOOST_AUTO_TEST_CASE(acf) {
	FILE* hTarget = fopen("acftarget.txt", "r");
	if (hTarget == NULL) {
		throw std::runtime_error("Could not open reference file: acftarget.txt");
	}
	try {
		while(!feof(hTarget)) {
			double data[10];
			for(size_t i = 0; i < 10 && !feof(hTarget); i++) {
				double curVal;
				fscanf(hTarget, "%lf", &curVal);
				data[i] = curVal;
			}
			if (feof(hTarget)) {
				break;
			}

			double corrs[10];
			for(size_t i = 0; i < 10 && !feof(hTarget); i++) {
				double curVal;
				fscanf(hTarget, "%lf", &curVal);
				corrs[i] = curVal;
			}
			if (feof(hTarget)) {
				break;
			}
			
			double acfs[10];
			lcmc::stats::autoCorrelation_stat(data, acfs, 10);
			
			for(size_t i = 0; i < 10; i++) {
				myTestClose(acfs[i], corrs[i], 1e-5);
			}
		}
	} catch (std::exception &e) {
		fclose(hTarget);
		BOOST_ERROR("Could not finish reading from acftarget.txt.");
		throw;
	}
	
	fclose(hTarget);
}

/* @fn lcmc::stats::interp::autoCorrelation()
 *
 * @test Consistent results with original IDL code
 */
/** Tests whether lcmc::stats::interp::autoCorrelation() matches 
 *	Ann Marie's original program
 */
BOOST_AUTO_TEST_CASE(acf_interp) {
	for(size_t i = 0; i <= 13; i++) {
		vector<double> times, mags;
		double offsetIn;
		{
			char fileName[80];
			sprintf(fileName, "acfi_target_in_%i.txt", i);
			FILE* hInput = fopen(fileName, "r");
			if (hInput == NULL) {
				throw std::runtime_error("Could not open reference file: " 
					+ std::string(fileName));
			}
			readMcLightCurve(hInput, offsetIn, times, mags);
			fclose(hInput);
		}

		vector<double> lags, acfs;
		double offsetOut;
		{
			char fileName[80];
			sprintf(fileName, "acfi_target_out_%i.txt", i);
			FILE* hOutput = fopen(fileName, "r");
			if (hOutput == NULL) {
				throw std::runtime_error("Could not open reference file: "
					+ std::string(fileName));
			}
			readMcLightCurve(hOutput, offsetOut, lags, acfs);
			fclose(hOutput);
		}
		
		size_t   nAcf = lags.size();
		// Add a small offset to prevent roundoff errors from making 
		//	a longer grid than AMC used
		double deltaT = (lags.back() - lags.front())/(nAcf-1) + 1e-14;
		
		vector<double> myAcfs;
		BOOST_CHECK_NO_THROW(lcmc::stats::interp::autoCorr(times, mags, 
			deltaT, nAcf, myAcfs));
		
		unsigned int nBad = 0;
		for(size_t j = 0; j < nAcf; j++) {
			/*myTestClose(acfs[j], myAcfs[j], 1e-5);*/
			if (!isClose(acfs[j], myAcfs[j], 1e-5)) {
				nBad++;
			}
		}
		BOOST_WARN(nBad > 0 && nBad < nAcf/1000);
		BOOST_CHECK(nBad < nAcf/1000);
	}	// end loop over examples
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
