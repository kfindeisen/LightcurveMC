/** Test unit for wavelike periodic functions
 * @file lightcurveMC/tests/unit_waves.cpp
 * @author Krzysztof Findeisen
 * @date Created April 28, 2013
 * @date Last modified May 24, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../common/warnflags.h"

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
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_math.h>
#include "test.h"
#include "../except/data.h"
#include "../fluxmag.h"
#include "../lightcurvetypes.h"
#include "../waves/lightcurves_periodic.h"

namespace lcmc { namespace test {

using boost::shared_ptr;

/** Factory class for generating lots of identical sine waves
 */
class TestSineFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store @p times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	explicit TestSineFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period), 
			nextPhase(sample()) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 *
	 * @return A pointer to a newly constructed 
	 *	@ref lcmc::models::SineWave "SineWave".
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct a new object.
	 * @exception lcmc::models::except::BadParam Thrown if an illegal 
	 *	value of @p amp or @p period was passed to the factory constructor.
	 *
	 * @exceptsafe Object construction is atomic. The factory is unchanged 
	 *	in the event of an exception.
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		std::auto_ptr<lcmc::models::ILightCurve> lc(
				new lcmc::models::SineWave(times, amp, period, nextPhase));
		// Only draw a new random number if the object was successfully constructed
		nextPhase = sample();
		return lc;
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
	mutable double nextPhase;
};

/** Factory class for generating lots of identical triangle waves
 */
class TestTriangleFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store @p times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	explicit TestTriangleFactory(const std::vector<double> times, 
			double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period), 
			nextPhase(sample()) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 *
	 * @return A pointer to a newly constructed 
	 *	@ref lcmc::models::TriangleWave "TriangleWave".
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct a new object.
	 * @exception lcmc::models::except::BadParam Thrown if an illegal 
	 *	value of @p amp or @p period was passed to the factory constructor.
	 *
	 * @exceptsafe Object construction is atomic. The factory is unchanged 
	 *	in the event of an exception.
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		std::auto_ptr<lcmc::models::ILightCurve> lc(
			new lcmc::models::TriangleWave(times, amp, period, nextPhase));
		// Only draw a new random number if the object was successfully constructed
		nextPhase = sample();
		return lc;
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
	mutable double nextPhase;
};

/** Factory class for generating lots of identical ellipse waves
 */
class TestEllipseFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store @p times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	explicit TestEllipseFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period), 
			nextPhase(sample()) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 *
	 * @return A pointer to a newly constructed 
	 *	@ref lcmc::models::EllipseWave "EllipseWave".
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct a new object.
	 * @exception lcmc::models::except::BadParam Thrown if an illegal 
	 *	value of @p amp or @p period was passed to the factory constructor.
	 *
	 * @exceptsafe Object construction is atomic. The factory is unchanged 
	 *	in the event of an exception.
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		std::auto_ptr<lcmc::models::ILightCurve> lc(
			new lcmc::models::EllipseWave(times, amp, period, nextPhase));
		// Only draw a new random number if the object was successfully constructed
		nextPhase = sample();
		return lc;
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
	mutable double nextPhase;
};

/** Factory class for generating lots of identical eclipsing binary curves
 */
class TestEclipseFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store @p times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	explicit TestEclipseFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period), 
			nextPhase(sample()) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 *
	 * @return A pointer to a newly constructed 
	 *	@ref lcmc::models::EclipseWave "EclipseWave".
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct a new object.
	 * @exception lcmc::models::except::BadParam Thrown if an illegal 
	 *	value of @p amp or @p period was passed to the factory constructor.
	 *
	 * @exceptsafe Object construction is atomic. The factory is unchanged 
	 *	in the event of an exception.
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		std::auto_ptr<lcmc::models::ILightCurve> lc(
			new lcmc::models::EclipseWave(times, amp, period, nextPhase));
		// Only draw a new random number if the object was successfully constructed
		nextPhase = sample();
		return lc;
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
	mutable double nextPhase;
};

/** Factory class for generating lots of identical broad peak waves
 */
class TestBroadFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store @p times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	explicit TestBroadFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period), 
			nextPhase(sample()) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 *
	 * @return A pointer to a newly constructed 
	 *	@ref lcmc::models::BroadPeakWave "BroadPeakWave".
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct a new object.
	 * @exception lcmc::models::except::BadParam Thrown if an illegal 
	 *	value of @p amp or @p period was passed to the factory constructor.
	 *
	 * @exceptsafe Object construction is atomic. The factory is unchanged 
	 *	in the event of an exception.
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		std::auto_ptr<lcmc::models::ILightCurve> lc(
			new lcmc::models::BroadPeakWave(times, amp, period, nextPhase));
		// Only draw a new random number if the object was successfully constructed
		nextPhase = sample();
		return lc;
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
	mutable double nextPhase;
};

/** Factory class for generating lots of identical sharp peak waves
 */
class TestSharpFactory : public TestRandomFactory {
public: 
	/** Sets the properties of the light curve to generate
	 *
	 * @param[in] times the times at which the light curve is sampled
	 * @param[in] amp,period the light curve parameters to use
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	store @p times.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	explicit TestSharpFactory(const std::vector<double> times, double amp, double period) 
			: TestRandomFactory(), times(times), amp(amp), period(period), 
			nextPhase(sample()) {
	}
	
	/** Generates a new realization of a sine wave having the 
	 * 	properties given to the constructor
	 *
	 * @return A pointer to a newly constructed 
	 *	@ref lcmc::models::SharpPeakWave "SharpPeakWave".
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct a new object.
	 * @exception lcmc::models::except::BadParam Thrown if an illegal 
	 *	value of @p amp or @p period was passed to the factory constructor.
	 *
	 * @exceptsafe Object construction is atomic. The factory is unchanged 
	 *	in the event of an exception.
	 */
	std::auto_ptr<lcmc::models::ILightCurve> make() const {
		std::auto_ptr<lcmc::models::ILightCurve> lc(
			new lcmc::models::SharpPeakWave(times, amp, period, nextPhase));
		// Only draw a new random number if the object was successfully constructed
		nextPhase = sample();
		return lc;
	}

private: 
	std::vector<double> times;
	double amp;
	double period;
	mutable double nextPhase;
};

/** Workhorse function for generating a periodic light curve and testing whether 
 * it has the required properties.
 *
 * @param[in] nTest The number of light curve instances to generate
 * @param[in] factory A factory for generating light curves of a specific 
 *	type with specific parameters
 *
 * @exception lcmc::models::except::BadParam Thrown if the factory generates 
 *	invalid light curves.
 *
 * @exceptsafe Function arguments are in a valid state in the event of an exception.
 */
void testPeriodic(size_t nTest, const TestFactory& factory) {

	try {
		for(size_t i = 0; i < nTest; i++) {
			std::auto_ptr<lcmc::models::ILightCurve> model = factory.make();
			
			std::vector<double> times, fluxes;
			model->getTimes(times);
			model->getFluxes(fluxes);
			
			// @post fluxArray.size() == getTimes().size()
			// If this condition is violated, all other tests are buggy
			BOOST_REQUIRE(times.size() == fluxes.size());	
			
			for(size_t j = 0; j < times.size(); j++) {
				/*for(size_t k = j+1; k < times.size(); k++) {
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
	} catch (const models::except::BadParam& e) {
		throw;
	} catch (const std::exception& e) {
		BOOST_FAIL(e.what());
	}
}

/** Test cases for testing whether the simulated light curves have the 
 * correct properties
 * @class BoostTest::test_wave
 */
BOOST_FIXTURE_TEST_SUITE(test_wave, ObsData)

/** Tests whether the simulated damped random walks have the correct distribution
 *
 * @see @ref lcmc::models::SineWave "SineWave"
 * @see testPeriodic()
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
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(sine)
{
	for(float amp = 0.05; amp < 1.0; amp *= 10.0) {
		for (float period = 0.2; period < 100.0; period *= 10.0) {
			testPeriodic(DET_TEST_COUNT, TestSineFactory(times, amp, period));
		}
	}
	BOOST_CHECK_THROW(testPeriodic(DET_TEST_COUNT, TestSineFactory(times,-2.0, 0.2)), 
			models::except::BadParam);
	BOOST_CHECK_THROW(testPeriodic(DET_TEST_COUNT, TestSineFactory(times, 0.0, 0.2)), 
			models::except::BadParam);
	testPeriodic(DET_TEST_COUNT, TestSineFactory(times, 1.0, 0.2));
	BOOST_CHECK_THROW(testPeriodic(DET_TEST_COUNT, TestSineFactory(times, 1.1, 0.2)), 
			models::except::BadParam);
	BOOST_CHECK_THROW(testPeriodic(DET_TEST_COUNT, TestSineFactory(times, 0.05, 0.0)), 
			models::except::BadParam);
	BOOST_CHECK_THROW(testPeriodic(DET_TEST_COUNT, TestSineFactory(times, 0.05,-1.0)), 
			models::except::BadParam);
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
