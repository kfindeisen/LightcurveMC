/** Test unit for statistics utilities
 * @file lightcurveMC/tests/unit_stats.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 22, 2013
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
#include <cerrno>
#include <cmath>
#include <cstring>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "../stats/acfinterp.h"
#include "../approx.h"
#include "../except/fileio.h"
#include "../waves/generators.h"
#include "../lcsio.h"
#include "../stats/magdist.h"
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
	/** Defines the data for each test case.
	 *
	 * @pre A text file called @c ptfjds.txt exists in the 
	 *	working directory and contains a list of Julian dates.
	 *
	 * @exception lcmc::except::FileIo Thrown if @c ptfjds.txt could not 
	 *	be opened or has the wrong format.
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store the times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	StatData() : ptfTimes(), unifTimes() {
		double minTStep, maxTStep;
		shared_ptr<FILE> hJulDates(fopen("ptfjds.txt", "r"), &fclose);
		if (hJulDates.get() == NULL) {
			throw except::FileIo("Could not open ptfjds.txt.");
		}
		readTimeStamps(hJulDates.get(), ptfTimes, minTStep, maxTStep);

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
};

/** Data common to the test cases.
 *
 * At present, contains only sampling times
 */
class StatDummy {
public: 
	/** Defines the data for each test case.
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store the times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
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
 *
 * @param[in] seed A random seed for generating multiple data sets.
 * @param[in] nTests The number of values whose mean and variance will be calculated.
 *
 * @exceptsafe Does not throw exceptions.
 */
void testNanProof(unsigned long int seed, size_t nTests) {
	vector<double> dirty, clean;
	
	try {
		dirty.reserve(nTests);
		clean.reserve(nTests);
			
		shared_ptr<gsl_rng> rng(gsl_rng_alloc(gsl_rng_mt19937), &gsl_rng_free);
		if (rng.get() == NULL) {
			throw std::bad_alloc();
		}
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
	} catch (const std::bad_alloc& e) {
		BOOST_FAIL("Out of memory!");
	}
	
	/** @todo Use an external mean / variance as the reference
	 */
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
 *
 * @exceptsafe Does not throw exceptions.
 */
void testProduct(double tau, const vector<double>& times)
{
	const size_t nTimes = times.size();

	// Can't guarantee that the gsl_ functions won't cause errors... 
	//	especially since the error conditions are undocumented
	try {	
		shared_ptr<gsl_matrix> initial(gsl_matrix_alloc(nTimes, nTimes), &gsl_matrix_free);
		if (initial.get() == NULL) {
			throw std::bad_alloc();
		}
		initGauss(initial.get(), times, tau);
		
		shared_ptr<gsl_matrix> result = lcmc::utils::getHalfMatrix(initial);
		
		shared_ptr<gsl_matrix> product(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);
		if (product.get() == NULL) {
			throw std::bad_alloc();
		}
		int status = gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, result.get(), result.get(), 
				0.0, product.get());
		if (status) {
			throw std::runtime_error(gsl_strerror(status));
		}
	
		shared_ptr<gsl_matrix> residuals(gsl_matrix_calloc(nTimes, nTimes), &gsl_matrix_free);
		if (residuals.get() == NULL) {
			throw std::bad_alloc();
		}
		status = gsl_matrix_memcpy(residuals.get(), product.get());
		if (status) {
			throw std::runtime_error(gsl_strerror(status));
		}
		status = gsl_matrix_sub   (residuals.get(), initial.get());
		if (status) {
			throw std::runtime_error(gsl_strerror(status));
		}

		
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
	} catch (const std::exception& e) {
		BOOST_FAIL(e.what());
	}
	
}

/** Test cases for testing functions related to handling Nan values
 * @class BoostTest::test_nan
 */
BOOST_FIXTURE_TEST_SUITE(test_nan, StatDummy)

/** Tests whether NaN-proof statistics are consistent with normal ones
 *
 * @see testNanProof()
 *
 * @exceptsafe Does not throw exceptions.
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
 *
 * @exceptsafe Does not throw exceptions.
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

/** Tests whether getHalfMatrix() returns the correct decomposition of a matrix
 *
 * @see @ref lcmc::utils::getHalfMatrix() "getHalfMatrix()"
 * @see testProduct()
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
 *
 * @exceptsafe Does not throw exceptions.
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

/** Tests whether @ref lcmc::stats::getSummaryStats() "getSummaryStats()" 
 *	correctly handles vectors with no variance
 *
 * @see @ref lcmc::stats::getSummaryStats() "getSummaryStats()"
 *
 * @test given a constant vector equal to ln(2), returns a variance of 0
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(zero_variance) {
	try {
		// Vector of constant flux
		// Bug doesn't come up if I use 1.0 as the value
		vector<double> constantSignal(ptfTimes.size(), log(2.0));
		
		double mean, stddev;
		BOOST_REQUIRE_NO_THROW(lcmc::stats::getSummaryStats(constantSignal, mean, stddev, "Constant Signal Test"));
		
		BOOST_CHECK(!testNan(stddev));
		BOOST_CHECK(stddev < 1e-12);

	} catch (const std::bad_alloc& e) {
		BOOST_FAIL("Out of memory!");
	}
}

/** Tests whether @ref lcmc::stats::autoCorrelation_stat() "autoCorrelation_stat()" 
 *	matches existing autocorrelation implementations from other languages
 * @see @ref lcmc::stats::autoCorrelation_stat() "autoCorrelation_stat()"
 *
 * @test Consistent results with a_correlate() procedure from IDL
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(acf) {
	shared_ptr<FILE> hTarget(fopen("acftarget.txt", "r"), &fclose);
	if (hTarget.get() == NULL) {
		BOOST_FAIL("Could not open reference file: acftarget.txt");
	}
	
	try {
		while(!feof(hTarget.get())) {
			double data[10];
			for(size_t i = 0; i < 10 && !feof(hTarget.get()); i++) {
				double curVal;
				fscanf(hTarget.get(), "%lf", &curVal);
				data[i] = curVal;
			}
			if (feof(hTarget.get())) {
				break;
			}

			double corrs[10];
			for(size_t i = 0; i < 10 && !feof(hTarget.get()); i++) {
				double curVal;
				fscanf(hTarget.get(), "%lf", &curVal);
				corrs[i] = curVal;
			}
			if (feof(hTarget.get())) {
				break;
			}
			
			double acfs[10];
			BOOST_REQUIRE_NO_THROW(
				lcmc::stats::autoCorrelation_stat(data, acfs, 10));
			
			for(size_t i = 0; i < 10; i++) {
				myTestClose(acfs[i], corrs[i], 1e-5);
			}
		}
	} catch (const std::exception &e) {
		BOOST_MESSAGE("Could not finish reading from acftarget.txt.");
		BOOST_FAIL(e.what());
	}
}

/** Tests whether @ref lcmc::stats::interp::autoCorr() "interp::autoCorr()" 
 *	matches Ann Marie's original program
 *
 * @see @ref lcmc::stats::interp::autoCorr() "interp::autoCorr()"
 *
 * @test Consistent results with original IDL code
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(acf_interp) {
	for(size_t i = 0; i <= 13; i++) {
		vector<double> times, mags;
		vector<double> lags, acfs;

		try {
			double offsetIn;
			{
				char fileName[80];
				int status = sprintf(fileName, "idl_target_in_%i.txt", i);
				if (status < 0) {
					throw std::runtime_error(strerror(status));
				}
				
				shared_ptr<FILE> hInput(fopen(fileName, "r"), &fclose);
				if (hInput.get() == NULL) {
					throw except::FileIo("Could not open reference file: " 
						+ std::string(fileName));
				}
				readMcLightCurve(hInput.get(), offsetIn, times, mags);
			}
	
			double offsetOut;
			{
				char fileName[80];
				int status = sprintf(fileName, "idl_target_acf_%i.txt", i);
				if (status < 0) {
					throw std::runtime_error(strerror(status));
				}
	
				shared_ptr<FILE> hOutput(fopen(fileName, "r"), &fclose);
				if (hOutput.get() == NULL) {
					throw except::FileIo("Could not open reference file: "
						+ std::string(fileName));
				}
				readMcLightCurve(hOutput.get(), offsetOut, lags, acfs);
			}
		} catch (const std::exception& e) {
			BOOST_FAIL(e.what());
		}
		
		size_t   nAcf = lags.size();
		// Add a small offset to prevent roundoff errors from making 
		//	a longer grid than AMC used
		double deltaT = (lags.back() - lags.front())/(nAcf-1) + 1e-14;
		
		vector<double> myAcfs;
		BOOST_REQUIRE_NO_THROW(lcmc::stats::interp::autoCorr(times, mags, 
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

/** Tests whether @ref lcmc::stats::peakFind() "peakFind()" matches 
 *	Ann Marie's original program
 *
 * @see @ref lcmc::stats::peakFind() "peakFind()"
 *
 * @test Consistent results with original IDL code
 *
 * @exceptsafe Does not throw exceptions
 */
BOOST_AUTO_TEST_CASE(peakfind) {
	for(size_t i = 0; i <= 13; i++) {
		vector<double> times, mags;
		vector<double> peakTimes, peaks;
		
		try {
			double offsetIn;
			{
				char fileName[80];
				int status = sprintf(fileName, "idl_target_in_%i.txt", i);
				if (status < 0) {
					throw std::runtime_error(strerror(status));
				}
				
				shared_ptr<FILE> hInput(fopen(fileName, "r"), &fclose);
				if (hInput.get() == NULL) {
					throw except::FileIo("Could not open reference file: " 
						+ std::string(fileName));
				}
				readMcLightCurve(hInput.get(), offsetIn, times, mags);
			}
	
			double offsetOut;
			{
				char fileName[80];
				int status = sprintf(fileName, "idl_target_peak_%i.txt", i);
				if (status < 0) {
					throw std::runtime_error(strerror(status));
				}
				
				shared_ptr<FILE> hOutput(fopen(fileName, "r"), &fclose);
				if (hOutput.get() == NULL) {
					throw except::FileIo("Could not open reference file: "
						+ std::string(fileName));
				}
				readMcLightCurve(hOutput.get(), offsetOut, peakTimes, peaks);
			}
		} catch (const std::exception& e) {
			BOOST_FAIL(e.what());
		}
		
		vector<double> myTimes, myPeaks;
		BOOST_REQUIRE_NO_THROW(lcmc::stats::peakFind(times, mags, 0.05, 
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

/* Tests for a bug where @ref lcmc::stats::peakFind() "peakFind()" produces 
 * non-monototonic output
 *
 * @see @ref lcmc::stats::peakFind() "peakFind()"
 *
 * @test Monotonic output
 *
 * @exceptsafe Does not throw exceptions.
 */
/*BOOST_AUTO_TEST_CASE(peakfind_mono) {

	vector<double> times, mags;
	
	try {
		double offsetIn;
		char fileName[80];
		int status = sprintf(fileName, "peakfind_mono_target.txt");
		if (status < 0) {
			throw std::runtime_error(strerror(status));
		}
		
		shared_ptr<FILE> hInput(fopen(fileName, "r"), &fclose);
		if (hInput.get() == NULL) {
			throw except::FileIo("Could not open reference file: " 
				+ std::string(fileName));
		}
		readMcLightCurve(hInput.get(), offsetIn, times, mags);
	} catch (const std::exception& e) {
		BOOST_FAIL(e.what());
	}

	vector<double> magCuts, timescales;
	try {
		const static double minMag = 0.01;
		double amp = stats::getAmplitude(mags);
		for (double mag = minMag; mag < amp; mag += minMag) {
			magCuts.push_back(mag);
		}
	} catch (const std::exception& e) {
		BOOST_FAIL(e.what());
	}

	BOOST_CHECK_NO_THROW(lcmc::stats::peakFindTimescales(times, mags, 
		magCuts, timescales));
	
	BOOST_REQUIRE_EQUAL(magCuts.size(), timescales.size());
		
	// Monotonic?
	for(size_t i = 1; i < timescales.size(); i++) {
		BOOST_CHECK_LE(timescales[i-1], timescales[i]);
	}
}*/

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
