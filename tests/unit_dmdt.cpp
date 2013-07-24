/** Test unit for Delta-m Delta-t analysis
 * @file lightcurveMC/tests/unit_dmdt.cpp
 * @author Krzysztof Findeisen
 * @date Created April 19, 2013
 * @date Last modified July 24, 2013
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

#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "test.h"
#include "../binstats.h"
#include "../stats/cut.tmp.h"
#include "../stats/dmdt.h"
#include "../gsl_compat.h"
#include "../../common/alloc.tmp.h"

using std::vector;
using kpfutils::checkAlloc;

// Ignore this declaration, since for some reason it confuses Doxygen
/// @cond
namespace lcmc { namespace stats {

using std::string;

void getSummaryStats(const DoubleVec& values, double& mean, double& stddev, 
		const string& statName);

}}		// end lcmc::stats
/// @endcond

namespace lcmc { namespace test {

/** Data common to the test cases.
 *
 * At present, the only data is a Gaussian process &Delta;m&Delta;t plot.
 */
class SimData {
public: 
	/** Sets up the data for a test case.
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	construct the object.
	 *
	 * @exceptsafe Object construction is atomic.
	 */
	SimData() : amplitude(3.290), deltaM(), deltaT(), binEdges(), 
			randomizer(checkAlloc(gsl_rng_alloc(gsl_rng_mt19937)), &gsl_rng_free) {
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
	
	virtual ~SimData() {
	}
	
	/** Randomly places points according to the distribution for a 
	 *	&Delta;m&Delta;t plot representing a Gaussian process 
	 *	with standard deviation 1 and timescale 1
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory for 
	 *	the new &Delta;m&Delta;t plot
	 *
	 * @exceptsafe The object is unchanged in the event of an exception
	 */
	void makeDmdt() {
		using std::swap;
		
		vector<double> tempMags, tempTimes;

		for(size_t i = 0; i < TEST_LEN; i++) {
			double time = pow(10.0, 
					gsl_ran_flat(randomizer.get(), MINTIME, MAXTIME));
			tempTimes.push_back(time);
		}
		
		// Analysis code assumes dmdt plots sorted by time
		std::sort(tempTimes.begin(), tempTimes.end());
		for(vector<double>::const_iterator it = tempTimes.begin(); it != tempTimes.end(); 
				it++) {
			double sigma = sqrt(2.0*(1.0 - exp(-0.5*(*it)*(*it))));
			// not sure why stdlib::abs(long) takes priority over 
			// cmath::abs(double) when passed a double argument...
			double mag   = fabs(gsl_ran_gaussian(randomizer.get(), sigma));
			tempMags.push_back(mag);
		}
		
		swap(this->deltaT, tempTimes);
		swap(this->deltaM, tempMags );
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
	 * The current value is a factor of 1.004 difference in &Delta;t
	 *
	 * @see TEST_LEN
	 */
	const static double BINWIDTH;

	/** log10(&Delta;t) at the short-timescale end of the plot
	 */
	const static double MINTIME;
	/** log10(&Delta;t) at the long-timescale end of the plot
	 */
	const static double MAXTIME;
	
	/** 5%-95% amplitude of the hypothetical light curve
	 */
	double amplitude;

	/** Stores the &Delta;m&Delta;t plot.
	 */
	vector<double> deltaM;
	/** Stores the &Delta;m&Delta;t plot.
	 */
	vector<double> deltaT;

	/** Tracks the edges of the &Delta;t bins.
	 */
	vector<double> binEdges;
	
private: 
	// For generating random data
	boost::shared_ptr<gsl_rng> randomizer;
};
const double SimData::BINWIDTH = 0.01;		// Factor of 1.004
const double SimData::MINTIME  = -0.5;
const double SimData::MAXTIME  =  0.5;

/** Test cases for testing &Delta;m&Delta;t-based timescales
 * @class BoostTest::test_dmdt
 */
BOOST_FIXTURE_TEST_SUITE(test_dmdt, SimData)

/** Tests whether cutting the quantiles at a particular amplitude in order to 
 * identify a timescale converges to analytical expressions in the limit of 
 * overwhelming data.
 * 
 * @see @ref lcmc::stats::deltaMBinQuantile() "deltaMBinQuantile()"
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
 *
 * @exceptsafe Does not throw exceptions.
 */
BOOST_AUTO_TEST_CASE(quantileCuts)
{
	using lcmc::stats::cutFunction;
	using lcmc::stats::MoreThan;
	using lcmc::stats::deltaMBinQuantile;
	
	vector<double> cut50Amp3s, cut50Amp2s, cut90Amp3s, cut90Amp2s;

	for(size_t i = 0; i < TEST_COUNT; i++) {
		vector<double> change50, change90;
		BOOST_REQUIRE_NO_THROW(makeDmdt());
		
		BOOST_REQUIRE_NO_THROW(deltaMBinQuantile(deltaT, deltaM, binEdges, 
				change50, 0.50));
		BOOST_REQUIRE_NO_THROW(deltaMBinQuantile(deltaT, deltaM, binEdges, 
				change90, 0.90));
		
		// Only exceptions that will get thrown here are out-of-memory
		BOOST_REQUIRE_NO_THROW(cut50Amp3s.push_back(
				cutFunction(binEdges, change50, MoreThan(amplitude / 3.0) )) );
		BOOST_REQUIRE_NO_THROW(cut50Amp2s.push_back(
				cutFunction(binEdges, change50, MoreThan(amplitude / 2.0) )) );
	
		BOOST_REQUIRE_NO_THROW(cut90Amp3s.push_back(
				cutFunction(binEdges, change90, MoreThan(amplitude / 3.0) )) );
		BOOST_REQUIRE_NO_THROW(cut90Amp2s.push_back(
				cutFunction(binEdges, change90, MoreThan(amplitude / 2.0) )) );
	}
	
	double mean50Amp3s, error50Amp3s;
	BOOST_REQUIRE_NO_THROW(lcmc::stats::getSummaryStats(cut50Amp3s, mean50Amp3s, 
			error50Amp3s, "Median @ 1/3 Amp"));
	BOOST_CHECK(testNan(mean50Amp3s));
	
	double mean50Amp2s, error50Amp2s;
	BOOST_REQUIRE_NO_THROW(lcmc::stats::getSummaryStats(cut50Amp2s, mean50Amp2s, 
			error50Amp2s, "Median @ 1/3 Amp"));
	BOOST_CHECK(testNan(mean50Amp2s));
	
	double mean90Amp3s, error90Amp3s;
	BOOST_REQUIRE_NO_THROW(lcmc::stats::getSummaryStats(cut90Amp3s, mean90Amp3s, 
			error90Amp3s, "90% @ 1/3 Amp"));
	myTestClose(mean90Amp3s, 0.709, 
				testNan(error90Amp3s) || error90Amp3s < BINWIDTH ? 
				BINWIDTH : 2.0*error90Amp3s);

	double mean90Amp2s, error90Amp2s;
	BOOST_REQUIRE_NO_THROW(lcmc::stats::getSummaryStats(cut90Amp2s, mean90Amp2s, 
			error90Amp2s, "90% @ 1/3 Amp"));
	myTestClose(mean90Amp2s, 1.178, 
			testNan(error90Amp2s) || error90Amp2s < BINWIDTH ? 
			BINWIDTH : 2.0*error90Amp2s);
}

BOOST_AUTO_TEST_SUITE_END()

}}		// end lcmc::test
