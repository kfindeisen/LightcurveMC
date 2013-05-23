/** Test unit for Gaussian process generators
 * @file lightcurveMC/tests/unit_gp.cpp
 * @author Krzysztof Findeisen
 * @date Created April 17, 2013
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

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>
#include "test.h"
#include "../fluxmag.h"
#include "../lightcurvetypes.h"
#include "../mcio.h"
#include "../waves/lightcurves_gp.h"

namespace lcmc { namespace test {

/** Data common to the test cases.
 *
 * At present, the only data is the simulation parameters.
 */
class ObsData {
public: 
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

	/** Number of Gaussian processes to generate
	 */
	const static size_t TEST_COUNT     =  10000;

	/** Stores the the times at which the Gaussian process is sampled
	 */
	std::vector<double> times;
};

/** Factory class for generating lots of identical white noise processes
 */
class TestWhiteNoiseFactory : public TestFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] sigma the RMS amplitude of the white noise
	 */
	explicit TestWhiteNoiseFactory(const std::vector<double> times, double sigma) 
			: TestFactory(), times(times), sigma(sigma) {
	}
	
	/** Generates a new realization of a white noise process having the 
	 * properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::WhiteNoise(times, sigma));
	}

private: 
	std::vector<double> times;
	double sigma;
};

/** Factory class for generating lots of identical damped random walks
 */
class TestDrwFactory : public TestFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] diffus the diffusion constant for the random walk
	 * @param[in] tau the damping time for the random walk
	 */
	explicit TestDrwFactory(const std::vector<double> times, double diffus, double tau) 
			: TestFactory(), times(times), diffus(diffus), tau(tau) {
	}
	
	/** Generates a new realization of a damped random walk having the 
	 * properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::DampedRandomWalk(times, diffus, tau));
	}

private: 
	std::vector<double> times;
	double diffus;
	double tau;
};

/** Factory class for generating lots of identical random walks
 */
class TestRwFactory : public TestFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] diffus the diffusion constant for the random walk
	 */
	explicit TestRwFactory(const std::vector<double> times, double diffus) 
			: TestFactory(), times(times), diffus(diffus) {
	}
	
	/** Generates a new realization of a damped random walk having the 
	 * properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::RandomWalk(times, diffus));
	}

private: 
	std::vector<double> times;
	double diffus;
};

/** Factory class for generating lots of identical standard Gaussian processes
 */
class TestGpFactory : public TestFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] sigma the RMS amplitude of the Gaussian process
	 * @param[in] tau the coherence time for the Gaussian process
	 */
	explicit TestGpFactory(const std::vector<double> times, double sigma, double tau) 
			: TestFactory(), times(times), sigma(sigma), tau(tau) {
	}
	
	/** Generates a new realization of a standard Gaussian process having the 
	 * properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::SimpleGp(times, sigma, tau));
	}

private: 
	std::vector<double> times;
	double sigma;
	double tau;
};

/** Factory class for generating lots of identical two-timescale 
 *	Gaussian processes
 */
class TestTwoGpFactory : public TestFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] sigma1, sigma2 the RMS amplitudes of the Gaussian components
	 * @param[in] tau1, tau2 the coherence times for the Gaussian components
	 */
	explicit TestTwoGpFactory(const std::vector<double> times, 
			double sigma1, double tau1, double sigma2, double tau2) 
			: TestFactory(), times(times), 
			sigma1(sigma1), sigma2(sigma2), tau1(tau1), tau2(tau2) {
	}
	
	/** Generates a new realization of a two-timescale Gaussian process 
	 * having the properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::TwoScaleGp(times, 
					sigma1, tau1, sigma2, tau2));
	}

private: 
	std::vector<double> times;
	double sigma1, sigma2;
	double tau1, tau2;
};

/** Workhorse function for generating a Gaussian process and testing whether 
 * it follows the intended distribution.
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] fileName The file name to which to print the light curves for inspection
 * @param[in] factory A factory for generating light curves of a specific 
 *	type with specific parameters
 *
 * @warning At present, testGp() cannot verify whether or not the generated 
 * light curves follow the correct distribution. The light curves must be 
 * tested using an external R script.
 *
 * @todo use an interface library to call the R script directly
 */
void testGp(size_t nTest, const std::string& fileName, const TestFactory& factory) {
	FILE* hDump = fopen(fileName.c_str(), "w");
	if (hDump == NULL) {
		throw std::runtime_error("Could not open output file.");
	}

	try {
		for (size_t i = 0; i < nTest; i++) {
			std::vector<double> mags;
			std::auto_ptr<lcmc::models::ILightCurve> model = factory.make();
			model->getFluxes(mags);
			// getFluxes() returns fluxes, but we've only 
			//	specified the statistical properties of 
			//	magnitudes
			lcmc::utils::fluxToMag(mags, mags);
			
			for(std::vector<double>::const_iterator it = mags.begin(); 
					it != mags.end(); it++) {
				if (it == mags.begin()) {
					fprintf(hDump, "%0.5f", *it);
				} else {
					fprintf(hDump, ", %0.5f", *it);
				}
			}
			fprintf(hDump, "\n");
		}
	} catch (const std::exception& e) {
		fclose(hDump);
		BOOST_ERROR("Could not finish writing to " << fileName);
		BOOST_ERROR(e.what());
		throw;
	}
	
	fclose(hDump);
	BOOST_TEST_MESSAGE("Test output successfully written to " << fileName);
	BOOST_TEST_MESSAGE("Please verify this output using external tools");
	BOOST_CHECK(true);
}

/** Generates many white noise signals and tests whether they have the 
 * correct distribution.
 *
 * @see testGp()
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] times The times at which each light curve should be sampled.
 */
void testWhite(size_t nTest, const std::vector<double>& times) {
	char fileName[80];
	sprintf(fileName, "run_white_%i.dat", nTest);
	
	testGp(nTest, fileName, TestWhiteNoiseFactory(times, 1.0));
}

/** Generates many damped random walks and tests whether they have the 
 * correct distribution.
 *
 * @see testGp()
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] times The times at which each light curve should be sampled.
 * @param[in] tau A damping time of the random walk.
 */
void testDrw(size_t nTest, const std::vector<double>& times, double tau) {
	char fileName[80];
	sprintf(fileName, "run_drw_%i_%.1f.dat", nTest, tau);
	
	testGp(nTest, fileName, TestDrwFactory(times, 2.0/tau, tau));
}

/** Generates many random walks and tests whether they have the 
 * correct distribution.
 *
 * @see testGp()
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] times The times at which each light curve should be sampled.
 * @param[in] diffus The diffusion constant of the random walk.
 */
void testRw(size_t nTest, const std::vector<double>& times, double diffus) {
	char fileName[80];
	sprintf(fileName, "run_rw_%i_%.3f.dat", nTest, diffus);
	
	testGp(nTest, fileName, TestRwFactory(times, diffus));
}

/** Generates many standard Gaussian processes and tests whether they have 
 * the correct distribution.
 *
 * @see testGp()
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] times The times at which each light curve should be sampled.
 * @param[in] tau A coherence time of the Gaussian process.
 */
void testStandardGp(size_t nTest, const std::vector<double> times, double tau) {
	char fileName[80];
	sprintf(fileName, "run_gp1_%i_%.1f.dat", nTest, tau);
	
	testGp(nTest, fileName, TestGpFactory(times, 1.0, tau));
}

/** Generates many two-timescale Gaussian processes and tests whether 
 * they have the correct distribution.
 *
 * @see testGp()
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] times The times at which each light curve should be sampled.
 * @param[in] tau1, tau2 the coherence times of the components
 */
void testTwoGp(size_t nTest, const std::vector<double> times, double tau1, double tau2) {
	char fileName[80];
	sprintf(fileName, "run_gp2_%i_%.1f_%.1f.dat", nTest, tau1, tau2);
	
	testGp(nTest, fileName, TestTwoGpFactory(times, 1.0, tau1, 1./3., tau2));
}

/** Test cases for testing whether the simulated Gaussian processes have the 
 * correct distribution
 * @class BoostTest::test_gp
 */
BOOST_FIXTURE_TEST_SUITE(test_gp, ObsData)

/** Tests whether the simulated white noise processes have the correct distribution
 *
 * @see @ref lcmc::models::WhiteNoise "WhiteNoise"
 * @see testWhite()
 *
 * @test A @ref lcmc::models::WhiteNoise "WhiteNoise" sampled at the PTF epochs 
 *	has a distribution consistent with a multivariate normal with the 
 *	expected covariance matrix
 */
BOOST_AUTO_TEST_CASE(white)
{
	testWhite(TEST_COUNT, times);
}

/* @class lcmc::models::DampedRandomWalk
 *
 * @test A DampedRandomWalk sampled at the PTF epochs and 
 *	&tau; &isin; {2 days, 20 days, 200 days} has a distribution 
 *	consistent with a multivariate normal with the expected 
 *	covariance matrix
 */
/** Tests whether the simulated damped random walks have the correct distribution
 *
 * @see testDrw()
 */
BOOST_AUTO_TEST_CASE(drw)
{
	testDrw(TEST_COUNT, times,   2.0);
	testDrw(TEST_COUNT, times,  20.0);
	testDrw(TEST_COUNT, times, 200.0);
}

/* @class lcmc::models::RandomWalk
 *
 * @test A RandomWalk sampled at the PTF epochs and 
 *	D &isin; {0.001 day^-1, 0.01 day^-1, and 0.1 day^-1} has a distribution 
 *	consistent with a multivariate normal with the expected 
 *	covariance matrix
 */
/** Tests whether the simulated random walks have the correct distribution
 *
 * @see testRw()
 */
BOOST_AUTO_TEST_CASE(rw)
{
	testRw(TEST_COUNT, times, 0.001);
	testRw(TEST_COUNT, times, 0.01 );
	testRw(TEST_COUNT, times, 0.1  );
}

/* @class lcmc::models::SimpleGp
 *
 * @test A SimpleGp sampled at the PTF epochs and 
 *	&tau; &isin; {2 days, 20 days, 200 days} has a distribution 
 *	consistent with a multivariate normal with the expected 
 *	covariance matrix
 */
/** Tests whether the simulated Gaussian processes have the correct distribution
 *
 * @see testStandardGp()
 */
BOOST_AUTO_TEST_CASE(gp1)
{
	testStandardGp(TEST_COUNT, times,   2.0);
	testStandardGp(TEST_COUNT, times,  20.0);
	testStandardGp(TEST_COUNT, times, 200.0);
}

/* @class lcmc::models::TwoScaleGp
 *
 * @test A TwoScaleGp sampled at the PTF epochs and 
 *	&tau;_1 &isin; {2 days, 20 days, 200 days} and 
 *	&tau;_2 &isin; {1/10, 1/3, 1, 3, 10} &tau;_1 has a distribution 
 *	consistent with a multivariate normal with the expected 
 *	covariance matrix
 */
/** Tests whether the simulated Gaussian processes have the correct distribution
 *
 * @see testTwoGp()
 */
BOOST_AUTO_TEST_CASE(gp2)
{
	for(float tau1 = 2; tau1 < 1000.0; tau1 *= 10.0) {
		testTwoGp(TEST_COUNT, times, tau1, tau1 / 10.0);
		testTwoGp(TEST_COUNT, times, tau1, tau1 /  3.0);
		testTwoGp(TEST_COUNT, times, tau1, tau1);
		testTwoGp(TEST_COUNT, times, tau1, tau1 *  3.0);
		testTwoGp(TEST_COUNT, times, tau1, tau1 * 10.0);
	}
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
