/** Test unit for Delta-m Delta-t analysis
 * @file unit_dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 19, 2013
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

#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "test.h"
#include "../stats/dmdt.h"
#include "../raiigsl.tmp.h"

using std::vector;

namespace lcmc { namespace stats {

using std::string;

void getSummaryStats(const vector<double>& values, double& mean, double& stddev, 
		const string& statName);

}}		// end lcmc::stats

namespace lcmc { namespace test {

/** Data common to the test cases.
 *
 * At present, the only data is a Gaussian process &Delta;m&Delta;t plot.
 */
class SimData {
public: 
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
	
	/** Randomly places points according to the distribution for a 
	 *	&Delta;m&Delta;t plot representing a Gaussian process 
	 *	with standard deviation 1 and timescale 1
	 */
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
	
	/** Number of points with which to sample a &Delta;m&Delta;t plot.
	 *
	 * The current value implies 10,000 points per bin
	 *
	 * @see BINWIDTH
	 */
	const static size_t TEST_LEN   = 1000000;
	/** Number of times to generate and analyze a &Delta;m&Delta;t plot.
	 */
	const static size_t TEST_COUNT =   100;
	/** Width of each &Delta;t bin in logarithmic space
	 *
	 * The current value isa factor of 1.004 difference in &Delta;t
	 *
	 * @see TEST_LEN
	 */
	const static double BINWIDTH   = 0.01;		// Factor of 1.004

	/** log10(&Delta;t) at the short-timescale end of the plot
	 */
	const static double MINTIME = -0.5;
	/** log10(&Delta;t) at the long-timescale end of the plot
	 */
	const static double MAXTIME =  0.5;
	
	/** 5%-95% amplitude of the hypothetical light curve
	 */
	double amplitude;

	/** Stores the &Delta;m&Delta;t plot.
	 */
	vector<double> deltaM;
	/** Stores the &Delta;m&Delta;t plot.
	 */
	vector<double> deltaT;

	/** Tracks the edged of the &Delta;t bins.
	 */
	vector<double> binEdges;
	
private: 
	// For generating random data
	RaiiGsl<gsl_rng> randomizer;
};

/** Test cases for testing &Delta;m&Delta;t-based timescales
 * @class Boost::Test::test_dmdt
 */
BOOST_FIXTURE_TEST_SUITE(test_dmdt, SimData)

/* @fn lcmc::stats::deltaMBinQuantile()
 *
 * @test For an N-fold sampling of a standard Gaussian process, the 
 *	probability that cutFunction(50th percentile, 1/3 amplitude) is NaN 
 *	converges to 1 as N grows large
 * @test For an N-fold sampling of a standard Gaussian process, the 
 *	probability that cutFunction(50th percentile, 1/2 amplitude) is NaN 
 *	converges to 1 as N grows large
 * @test For an N-fold sampling of a standard Gaussian process, 
 *	cutFunction(90th percentile, 1/3 amplitude) converges to 0.709&tau; 
 *	as N grows large
 * @test For an N-fold sampling of a standard Gaussian process, 
 *	cutFunction(90th percentile, 1/2 amplitude) converges to 1.178&tau; 
 *	as N grows large
 */
/** Tests whether cutting the quantiles at a particular amplitude in order to 
 * identify a timescale converges to analytical expressions in the limit of 
 * overwhelming data.
 */
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
	lcmc::stats::getSummaryStats(cut50Amp3s, mean50Amp3s, error50Amp3s, "Median @ 1/3 Amp");
	BOOST_CHECK(testNan(mean50Amp3s));
	
	double mean50Amp2s, error50Amp2s;
	lcmc::stats::getSummaryStats(cut50Amp2s, mean50Amp2s, error50Amp2s, "Median @ 1/3 Amp");
	BOOST_CHECK(testNan(mean50Amp2s));
	
	double mean90Amp3s, error90Amp3s;
	lcmc::stats::getSummaryStats(cut90Amp3s, mean90Amp3s, error90Amp3s, "90% @ 1/3 Amp");
	myTestClose(mean90Amp3s, 0.709, 
				testNan(error90Amp3s) || error90Amp3s < BINWIDTH ? 
				BINWIDTH : 2.0*error90Amp3s);

	double mean90Amp2s, error90Amp2s;
	lcmc::stats::getSummaryStats(cut90Amp2s, mean90Amp2s, error90Amp2s, "90% @ 1/3 Amp");
	myTestClose(mean90Amp2s, 1.178, 
			testNan(error90Amp2s) || error90Amp2s < BINWIDTH ? 
			BINWIDTH : 2.0*error90Amp2s);
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
