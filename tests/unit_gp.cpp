/** Test unit for Gaussian process generators
 * @file unit_gp.cpp
 * @author Krzysztof Findeisen
 * @date Created April 17, 2013
 * @date Last modified April 17, 2013
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
#include "../fluxmag.h"
#include "../lightcurvetypes.h"
#include "../mcio.h"
#include "../waves/lightcurves_gp.h"

// Data common to the test cases
class ObsData {
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

void testDrw(size_t nTest, const std::vector<double> times, double tau) {
	char fileName[80];
	sprintf(fileName, "run_drw_%i_%.1f.dat", nTest, tau);
	
	FILE* hDump = fopen(fileName, "w");
	if (hDump == NULL) {
		throw std::runtime_error("Could not open output file.");
	}

	try {
		for (size_t i = 0; i < nTest; i++) {
			lcmc::models::DampedRandomWalk drwTest(times, 2.0/tau, tau);
			
			std::vector<double> mags;
			drwTest.getFluxes(mags);
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
	} catch (std::exception& e) {
		fclose(hDump);
		BOOST_ERROR("Could not finish writing to " << fileName);
		BOOST_ERROR(e.what());
		throw e;
	}
	
	fclose(hDump);
	BOOST_TEST_MESSAGE("Test output successfully written to " << fileName);
	BOOST_TEST_MESSAGE("Please verify this output using external tools");
}

void testStandardGp(size_t nTest, const std::vector<double> times, double tau) {
	char fileName[80];
	sprintf(fileName, "run_gp1_%i_%.1f.dat", nTest, tau);
	
	FILE* hDump = fopen(fileName, "w");
	if (hDump == NULL) {
		throw std::runtime_error("Could not open output file.");
	}

	try {
		for (size_t i = 0; i < nTest; i++) {
			lcmc::models::SimpleGp gpTest(times, 1.0, tau);
			
			std::vector<double> mags;
			gpTest.getFluxes(mags);
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
	} catch (std::exception& e) {
		fclose(hDump);
		BOOST_ERROR("Could not finish writing to " << fileName);
		BOOST_ERROR(e.what());
		throw e;
	}
	
	fclose(hDump);
	BOOST_TEST_MESSAGE("Test output successfully written to " << fileName);
	BOOST_TEST_MESSAGE("Please verify this output using external tools");
}

BOOST_AUTO_TEST_CASE(drw)
{
	testDrw(TEST_COUNT, times,   2.0);
	testDrw(TEST_COUNT, times,  20.0);
	testDrw(TEST_COUNT, times, 200.0);
}

BOOST_AUTO_TEST_CASE(gp1)
{
	testStandardGp(TEST_COUNT, times,   2.0);
	testStandardGp(TEST_COUNT, times,  20.0);
	testStandardGp(TEST_COUNT, times, 200.0);
}

BOOST_AUTO_TEST_SUITE_END()
