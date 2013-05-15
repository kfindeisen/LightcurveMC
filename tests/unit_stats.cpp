/** Test unit for statistics utilities
 * @file lightcurveMC/tests/unit_stats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 5, 2013
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
#include <limits>
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
#include "../waves/generators.h"
#include "../lcsio.h"
#include "../mcio.h"
#include "../nan.h"
#include "../stats/peakfind.h"
#include "test.h"
#include "../except/undefined.h"


using std::vector;
using boost::shared_ptr;

namespace lcmc { namespace utils {

shared_ptr<gsl_matrix> getHalfMatrix(const shared_ptr<gsl_matrix>& a);

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

/** Data common to the test cases.
 *
 * At present, contains only sampling times
 */
class StatDummy {
public: 
	StatDummy() : emptyVec(), awfulVec() {
		awfulVec.reserve(TEST_LEN);
		for(size_t i = 0; i < TEST_LEN; i++) {
			awfulVec.push_back(std::numeric_limits<double>::quiet_NaN());
		}
	}
	
	~StatDummy() {
	}

	/** Number of times to use for a uniformly sampled data set
	 */
	const static size_t TEST_LEN = 100;

	/** Represents an empty data set.
	 */
	vector<double> emptyVec;

	/** Represents a data set consisting only of NaNs.
	 */
	vector<double> awfulVec;
};

/** Tests whether meanNoNan() and varianceNoNan() correctly handle NaN values
 */
void testNanProof(unsigned long int seed, size_t nTests) {
	vector<double> dirty, clean;
	dirty.reserve(nTests);
	clean.reserve(nTests);
		
	shared_ptr<gsl_rng> rng(gsl_rng_alloc(gsl_rng_mt19937), &gsl_rng_free);
	gsl_rng_set(rng.get(), seed);
	
	// Clean is identical to dirty, except that is doesn't have any NaNs
	for(size_t i = 0; i < nTests; i++) {
		if (gsl_rng_uniform(rng.get()) < 0.1) {
  			dirty.push_back(std::numeric_limits<double>::quiet_NaN());
		} else {
			double x = gsl_ran_ugaussian(rng.get());
			dirty.push_back(x);
			clean.push_back(x);
		}
	}
	
	BOOST_CHECK_NO_THROW(myTestClose(utils::    meanNoNan(dirty), 
		utils::    meanNoNan(clean), 1e-10));
	BOOST_CHECK_NO_THROW(myTestClose(utils::varianceNoNan(dirty), 
		utils::varianceNoNan(clean), 1e-10));
}

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
	shared_ptr<gsl_matrix> result = lcmc::utils::getHalfMatrix(initial);
	
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

/** Test cases for testing functions related to handling Nan values
 * @class BoostTest::test_nan
 */
BOOST_FIXTURE_TEST_SUITE(test_nan, StatDummy)

/** Tests whether NaN-proof statistics are consistent with normal ones
 *
 * @see testNanProof()
 */
BOOST_AUTO_TEST_CASE(nan_proof) {
	testNanProof(  42, TEST_LEN);
	testNanProof(  43, TEST_LEN);
	testNanProof(1196, TEST_LEN);
	testNanProof(1764, TEST_LEN);
	testNanProof(3125, TEST_LEN);
	
	using lcmc::utils::    meanNoNan;
	using lcmc::utils::varianceNoNan;
	BOOST_CHECK_THROW(    meanNoNan(emptyVec), stats::except::NotEnoughData);
	BOOST_CHECK_THROW(varianceNoNan(emptyVec), stats::except::NotEnoughData);
	BOOST_CHECK_THROW(    meanNoNan(awfulVec), stats::except::NotEnoughData);
	BOOST_CHECK_THROW(varianceNoNan(awfulVec), stats::except::NotEnoughData);
}

/** Tests whether NaN diagnostics work as advertised
 */
BOOST_AUTO_TEST_CASE(nan_check) {
	// Easy access to special double values
	typedef std::numeric_limits<double> d;
	
	using lcmc::utils::        isNan;
	using lcmc::utils::   isNanOrInf;
	using lcmc::utils::    meanNoNan;
	using lcmc::utils::varianceNoNan;
	
	BOOST_WARN(d::     has_infinity);
	BOOST_WARN(d::    has_quiet_NaN);
	BOOST_WARN(d::has_signaling_NaN);

	// Skip tests if the system doesn't support special floating-point values
	BOOST_CHECK(!d::     has_infinity || !isNan( d::     infinity()));
	BOOST_CHECK(                         !isNan( 3.0               ));
	BOOST_CHECK(                         !isNan( 0.0               ));
	BOOST_CHECK(                         !isNan(-3.0               ));
	BOOST_CHECK(!d::     has_infinity || !isNan(-d::     infinity()));
	BOOST_CHECK(!d::    has_quiet_NaN ||  isNan(-d::    quiet_NaN()));
	BOOST_CHECK(!d::has_signaling_NaN ||  isNan(-d::signaling_NaN()));
	
	BOOST_CHECK(!d::     has_infinity ||  isNanOrInf( d::     infinity()));
	BOOST_CHECK(                         !isNanOrInf( 3.0               ));
	BOOST_CHECK(                         !isNanOrInf( 0.0               ));
	BOOST_CHECK(                         !isNanOrInf(-3.0               ));
	BOOST_CHECK(!d::     has_infinity ||  isNanOrInf(-d::     infinity()));
	BOOST_CHECK(!d::    has_quiet_NaN ||  isNanOrInf(-d::    quiet_NaN()));
	BOOST_CHECK(!d::has_signaling_NaN ||  isNanOrInf(-d::signaling_NaN()));
}

BOOST_AUTO_TEST_SUITE_END()


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
			sprintf(fileName, "idl_target_in_%i.txt", i);
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
			sprintf(fileName, "idl_target_acf_%i.txt", i);
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
		
		BOOST_REQUIRE_EQUAL(nAcf, myAcfs.size());
		
		// Allow for occasional deviations due to roundoff errors
		unsigned int nBad = 0;
		for(size_t j = 0; j < nAcf; j++) {
			if (!isClose(acfs[j], myAcfs[j], 1e-5)) {
				nBad++;
			}
		}
		BOOST_WARN(nBad == 0 || nBad >= nAcf/1000);
		BOOST_CHECK(nBad <= nAcf/1000);
	}	// end loop over examples
}

/* @fn lcmc::stats::peakFind()
 *
 * @test Consistent results with original IDL code
 */
/** Tests whether lcmc::stats::peakFind() matches 
 *	Ann Marie's original program
 */
BOOST_AUTO_TEST_CASE(peakfind) {
	for(size_t i = 0; i <= 13; i++) {
		vector<double> times, mags;
		double offsetIn;
		{
			char fileName[80];
			sprintf(fileName, "idl_target_in_%i.txt", i);
			FILE* hInput = fopen(fileName, "r");
			if (hInput == NULL) {
				throw std::runtime_error("Could not open reference file: " 
					+ std::string(fileName));
			}
			readMcLightCurve(hInput, offsetIn, times, mags);
			fclose(hInput);
		}

		vector<double> peakTimes, peaks;
		double offsetOut;
		{
			char fileName[80];
			sprintf(fileName, "idl_target_peak_%i.txt", i);
			FILE* hOutput = fopen(fileName, "r");
			if (hOutput == NULL) {
				throw std::runtime_error("Could not open reference file: "
					+ std::string(fileName));
			}
			readMcLightCurve(hOutput, offsetOut, peakTimes, peaks);
			fclose(hOutput);
		}
		
		vector<double> myTimes, myPeaks;
		BOOST_CHECK_NO_THROW(lcmc::stats::peakFind(times, mags, 0.05, 
			myTimes, myPeaks));
		
		BOOST_REQUIRE_EQUAL(myTimes.size(), peakTimes.size());
		BOOST_REQUIRE_EQUAL(myTimes.size(), myPeaks.size());
		
		// Allow for occasional deviations due to roundoff errors
		unsigned int nBad = 0;
		for(size_t j = 0; j < myTimes.size(); j++) {
			if (!isClose(myTimes[j], peakTimes[j], 1e-5) 
					|| !isClose(myPeaks[j], peaks[j], 1e-5)) {
				nBad++;
			}
		}
		BOOST_WARN(nBad == 0 || nBad >= peakTimes.size()/1000);
		BOOST_CHECK(nBad <= peakTimes.size()/1000);
	}	// end loop over examples
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
