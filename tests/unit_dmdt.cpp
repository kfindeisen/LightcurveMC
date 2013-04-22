/** Test unit for Delta-m Delta-t analysis
 * @file unit_dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 19, 2013
 * @date Last modified April 19, 2013
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

#include <string>
#include <vector>
#include <cmath>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "../stats/dmdt.h"
#include "../nan.h"
#include "../raiigsl.tmp.h"

using std::vector;

// Data common to the test cases
class SimData {
public: 
	const static size_t TEST_LEN   = 1000000/*0*/;	// 100,000 per bin
	const static size_t TEST_COUNT =   100;
	const static double BINWIDTH   = 0.01;		// Factor of 1.004

	SimData() : amplitude(3.290), deltaM(), deltaT(), binEdges(), 
			randomizer(gsl_rng_alloc(gsl_rng_mt19937), &gsl_rng_free) {
		gsl_rng_set(randomizer.get(), 42);
		
		// Preliminary data set
		makeDmdt();
		
		// Set up the analysis bins
		for (double bin = MINTIME; bin < MAXTIME; bin += 0.01) {
			binEdges.push_back(pow(10.0,bin));
		}
		// Get the bin containing MAXTIME as well
		binEdges.push_back(pow(10.0,MAXTIME));
	}
	
	~SimData() {
	}
	
	void makeDmdt() {
		deltaM.clear();
		deltaT.clear();

		for(size_t i = 0; i < TEST_LEN; i++) {
			double time = pow(10.0, 
					gsl_ran_flat(randomizer.get(), MINTIME, MAXTIME));
			deltaT.push_back(time);
		}
		
		// Analysis code assumes dmdt plots sorted by time
		std::sort(deltaT.begin(), deltaT.end());
		for(vector<double>::const_iterator it = deltaT.begin(); it != deltaT.end(); 
				it++) {
			double sigma = sqrt(2.0*(1.0 - exp(-0.5*(*it)*(*it))));
			// not sure why stdlib::abs(long) takes priority over cmath::abs(double)
			double mag   = fabs(gsl_ran_gaussian(randomizer.get(), sigma));
			deltaM.push_back(mag);
		}
	}
	
	// Time range to test
	const static double MINTIME = -0.5;
	const static double MAXTIME =  0.5;
	
	// 5%-95% amplitude of the hypothetical light curve
	double amplitude;

	// Delta-m delta-t plot
	vector<double> deltaM;
	vector<double> deltaT;

	// Bins in which to measure the quantiles
	vector<double> binEdges;
	
private: 
	// For generating random data
	RaiiGsl<gsl_rng> randomizer;
};

BOOST_FIXTURE_TEST_SUITE(test_dmdt, SimData)

void getSummaryStats(const vector<double>& values, double& mean, double& error) {
	mean   = std::numeric_limits<double>::quiet_NaN();
	error  = std::numeric_limits<double>::quiet_NaN();
	
	try {
		mean   =      lcmc::utils::    meanNoNan(values);
		error  = sqrt(lcmc::utils::varianceNoNan(values) / static_cast<double>(values.size()));
	} catch (std::invalid_argument &e) {
		// These should just be warnings that a statistic is undefined
		BOOST_TEST_MESSAGE( "WARNING: " << e.what());
	}
}

#include <algorithm>

// Emulate BOOST_CHECK_CLOSE_FRACTION without crashing and burning
void myTestClose(double val1, double val2, double frac) {
	using namespace ::boost::test_tools;
	
	predicate_result result = check_is_close(val1, val2, 
			fraction_tolerance_t<double>(frac));
	char buf[256];
	sprintf(buf, "floating point comparison failed: [%.10g != %.10g] (%.10g >= %.10g)", val1, val2, fabs(val1-val2), frac);
	
	BOOST_CHECK_MESSAGE(static_cast<bool>(result), buf);
//	BOOST_CHECK_CLOSE_FRACTION(val1, val2, frac);
}

BOOST_AUTO_TEST_CASE(quantileCuts)
{
	using lcmc::stats::cutFunction;
	using lcmc::stats::deltaMBinQuantile;
	
	vector<double> cut50Amp3s, cut50Amp2s, cut90Amp3s, cut90Amp2s;

	for(size_t i = 0; i < TEST_COUNT; i++) {
		vector<double> change50, change90;
		makeDmdt();
		
		deltaMBinQuantile(deltaT, deltaM, binEdges, change50, 0.50);
		deltaMBinQuantile(deltaT, deltaM, binEdges, change90, 0.90);
		
		cut50Amp3s.push_back(cutFunction(binEdges, change50, amplitude / 3.0));
		cut50Amp2s.push_back(cutFunction(binEdges, change50, amplitude / 2.0));
	
		cut90Amp3s.push_back(cutFunction(binEdges, change90, amplitude / 3.0));
		cut90Amp2s.push_back(cutFunction(binEdges, change90, amplitude / 2.0));
	}
	
	double mean50Amp3s, error50Amp3s;
	getSummaryStats(cut50Amp3s, mean50Amp3s, error50Amp3s);
	BOOST_CHECK(gsl_isnan(mean50Amp3s));
	
	double mean50Amp2s, error50Amp2s;
	getSummaryStats(cut50Amp2s, mean50Amp2s, error50Amp2s);
	BOOST_CHECK(gsl_isnan(mean50Amp2s));
	
	double mean90Amp3s, error90Amp3s;
	getSummaryStats(cut90Amp3s, mean90Amp3s, error90Amp3s);
	myTestClose(mean90Amp3s, 0.709, 
				gsl_isnan(error90Amp3s) || error90Amp3s < BINWIDTH ? 
				BINWIDTH : 2.0*error90Amp3s);
//	double sigma = sqrt(2.0*(1.0 - exp(-0.5*mean90Amp3s*mean90Amp3s)));
//	myTestClose(gsl_cdf_gaussian_Pinv(0.95, sigma), amplitude / 3.0, BINWIDTH);

	double mean90Amp2s, error90Amp2s;
	getSummaryStats(cut90Amp2s, mean90Amp2s, error90Amp2s);
	myTestClose(mean90Amp2s, 1.178, 
			gsl_isnan(error90Amp2s) || error90Amp2s < BINWIDTH ? 
			BINWIDTH : 2.0*error90Amp2s);
//	sigma = sqrt(2.0*(1.0 - exp(-0.5*mean90Amp2s*mean90Amp2s)));
//	myTestClose(gsl_cdf_gaussian_Pinv(0.95, sigma), amplitude / 2.0, BINWIDTH);
}

BOOST_AUTO_TEST_SUITE_END()
