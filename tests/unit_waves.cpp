/** Test unit for wavelike periodic functions
 * @file unit_waves.cpp
 * @author Krzysztof Findeisen
 * @date Created April 28, 2013
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

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>
#include <gsl/gsl_math.h>
#include "test.h"
#include "../fluxmag.h"
#include "../lightcurvetypes.h"
#include "../mcio.h"
#include "../waves/lightcurves_periodic.h"

namespace lcmc { namespace test {

/** Data common to the test cases.
 *
 * At present, the only data is the simulation parameters.
 */
class WaveData {
public: 
	WaveData() : times() {
		double minTStep, maxTStep;
		FILE* hJulDates = fopen("ptfjds.txt", "r");
		if (hJulDates == NULL) {
			throw std::runtime_error("Could not open ptfjds.txt.");
		}
		readTimeStamps(hJulDates, times, minTStep, maxTStep);
		fclose(hJulDates);
		hJulDates = NULL;
	}
	
	~WaveData() {
	}

	/** Number of light curves to generate
	 */
	const static size_t TEST_COUNT = 100;

	/** Stores the the times at which the light curve is sampled
	 */
	std::vector<double> times;
};

/** Factory class for generating lots of identical sine waves
 */
class TestSineFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 */
	explicit TestSineFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::SineWave(times, amp, period, sample()));
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
};

/** Factory class for generating lots of identical triangle waves
 */
class TestTriangleFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 */
	explicit TestTriangleFactory(const std::vector<double> times, 
			double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::TriangleWave(times, amp, period, sample()));
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
};

/** Factory class for generating lots of identical ellipse waves
 */
class TestEllipseFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 */
	explicit TestEllipseFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::EllipseWave(times, amp, period, sample()));
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
};

/** Factory class for generating lots of identical eclipsing binary curves
 */
class TestEclipseFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 */
	explicit TestEclipseFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::EclipseWave(times, amp, period, sample()));
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
};

/** Factory class for generating lots of identical broad peak waves
 */
class TestBroadFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 */
	explicit TestBroadFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::BroadPeakWave(times, amp, period, sample()));
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
};

/** Factory class for generating lots of identical sharp peak waves
 */
class TestSharpFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 */
	explicit TestSharpFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		return std::auto_ptr<lcmc::models::ILightCurve>(
			new lcmc::models::SharpPeakWave(times, amp, period, sample()));
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
};

/** Workhorse function for generating a periodic light curve and testing whether 
 * it has the required properties.
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] factory A factory for generating light curves of a specific 
 *	type with specific parameters
 */
void testPeriodic(size_t nTest, const TestFactory& factory) {

	for(size_t i = 0; i < nTest; i++) {
		std::auto_ptr<lcmc::models::ILightCurve> model = factory.make();
		
		std::vector<double> times, fluxes;
		model->getTimes(times);
		model->getFluxes(fluxes);
		
		// @post fluxArray.size() == getTimes().size()
		// If this condition is violated, all other tests are buggy
		BOOST_REQUIRE(times.size() == fluxes.size());	

		for(size_t j = 0; j < times.size(); j++) {
/*			for(size_t k = j+1; k < times.size(); k++) {
				//@post if getTimes()[i] == getTimes()[j] for i &ne; j, then 
				//	getFluxes()[i] == getFluxes()[j]
				BOOST_CHECK(!isClose( times[j],  times[k], 1e-12) 
					  || isClose(fluxes[j], fluxes[k], 1e-12));
			}*/

			//@post No element of fluxArray is NaN
			BOOST_REQUIRE(!testNan(fluxes[j]));
			//@post All elements in fluxArray are non-negative
			BOOST_CHECK(fluxes[j] >= 0.0);
		}

		//@post Either the mean, median, or mode of the flux is one, when 
		//	averaged over many elements and many light curve instances. 
		//	Subclasses of ILightCurve may chose the option 
		//	(mean, median, or mode) most appropriate for their light 
		//	curve shape.
	}
}

/** Test cases for testing whether the simulated Gaussian processes have the 
 * correct distribution
 * @class Boost::Test::test_gp
 */
BOOST_FIXTURE_TEST_SUITE(test_wave, WaveData)

/* @class lcmc::models::SineWave
 *
 * @test A SineWave sampled at the PTF epochs and amplitudes 
 *	&isin; {0.05, 0.5} and periods &isin; {0.2 days, 2 days, 20 days}
 *	is a valid implementation of ILightCurve
 * @test A SineWave sampled at the PTF epochs with amplitude = 1.0 and 
 *	period = 0.2 days is a valid implementation of ILightCurve
 * @test A SineWave will throw a std::invalid_argument if constructed with 
 *	amplitude = -1.0 and period 0.2 days
 * @test A SineWave will throw a std::invalid_argument if constructed with 
 *	amplitude = 0.0 and period 0.2 days
 * @test A SineWave will throw a std::invalid_argument if constructed with 
 *	amplitude = 1.1 and period 0.2 days
 * @test A SineWave will throw a std::invalid_argument if constructed with 
 *	amplitude = 0.05 and period 0.0 days
 * @test A SineWave will throw a std::invalid_argument if constructed with 
 *	amplitude = 0.05 and period -1.0 days
 */
/** Tests whether the simulated damped random walks have the correct distribution
 *
 * @see testPeriodic()
 */
BOOST_AUTO_TEST_CASE(sine)
{
	for(float amp = 0.05; amp < 1.0; amp *= 10.0) {
		for (float period = 0.2; period < 100.0; period *= 10.0) {
			testPeriodic(TEST_COUNT, TestSineFactory(times, amp, period));
		}
	}
	BOOST_CHECK_THROW(testPeriodic(TEST_COUNT, TestSineFactory(times,-2.0, 0.2)), 
			std::invalid_argument);
	BOOST_CHECK_THROW(testPeriodic(TEST_COUNT, TestSineFactory(times, 0.0, 0.2)), 
			std::invalid_argument);
	testPeriodic(TEST_COUNT, TestSineFactory(times, 1.0, 0.2));
	BOOST_CHECK_THROW(testPeriodic(TEST_COUNT, TestSineFactory(times, 1.1, 0.2)), 
			std::invalid_argument);
	BOOST_CHECK_THROW(testPeriodic(TEST_COUNT, TestSineFactory(times, 0.05, 0.0)), 
			std::invalid_argument);
	BOOST_CHECK_THROW(testPeriodic(TEST_COUNT, TestSineFactory(times, 0.05,-1.0)), 
			std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
