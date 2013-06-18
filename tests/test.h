/** Utilities for test suite
 * @file lightcurveMC/tests/test.h
 * @author Krzysztof Findeisen
 * @date Created April 28, 2013
 * @date Last modified May 28, 2013
 */

#ifndef LCMCTESTH
#define LCMCTESTH

#include <memory>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "../except/fileio.h"
#include "../lightcurvetypes.h"
#include "../mcio.h"

namespace lcmc { namespace test {

using boost::shared_ptr;
using std::vector;

/** Wrapper for a trusted Nan-testing function
 */
bool testNan(const double x);

/** Constructs a covariance matrix for a white Gaussian process
 */
shared_ptr<gsl_matrix> initGaussWn(const vector<double>& times);
/** Constructs a covariance matrix for a squared exponential Gaussian process
 */
shared_ptr<gsl_matrix> initGaussSe(const vector<double>& times, double tau);
/** Constructs a covariance matrix for an exponential Gaussian process
 */
shared_ptr<gsl_matrix> initGaussDrw(const vector<double>& times, double tau);
/** Constructs a covariance matrix for a Matérn Gaussian process
 */
shared_ptr<gsl_matrix> initGaussMat(const vector<double>& times, double tau, double nu);
/** Constructs a covariance matrix for a periodic Gaussian process
 */
shared_ptr<gsl_matrix> initGaussP(const vector<double>& times, double tau, double period);
/** Constructs a covariance matrix for a rational Gaussian process
 */
shared_ptr<gsl_matrix> initGaussR(const vector<double>& times, double tau, double alpha);

/** This function is a wrapper for a trusted approximate comparison method.
 */
bool isClose(double val1, double val2, double frac);

/** This function emulates the BOOST_CHECK_CLOSE_FRACTION macro.
 */
void myTestClose(double val1, double val2, double frac);

/** Parent factory class for generating lots of identical light curves
 */
class TestFactory {
public: 
	/** Generates a new realization of a damped random walk having the 
	 * properties given to the constructor
	 */
	virtual std::auto_ptr<lcmc::models::ILightCurve> make() const = 0;

	virtual ~TestFactory();
};

/** Factory class for generating lots of identical sine waves
 */
class TestRandomFactory : public TestFactory {
public: 
	/** Initializes the random number generator
	 */
	explicit TestRandomFactory();

	virtual ~TestRandomFactory();

protected: 
	/** Draws a standard uniform variate.
	 */
	double sample() const;

private: 
	gsl_rng* rng;
};

/** Data common to the test cases.
 *
 * At present, the only data is the simulation parameters.
 */
class ObsData {
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
	ObsData() : times() {
		shared_ptr<FILE> hJulDates = fileCheckOpen("ptfjds.txt", "r");
		readTimeStamps(hJulDates.get(), times);
	}
	
	~ObsData() {
	}

	/** Number of Gaussian processes to generate
	 */
	const static size_t STOC_TEST_COUNT = 10000;

	/** Number of deterministic light curves to generate
	 */
	const static size_t  DET_TEST_COUNT =   100;

	/** Stores the the times at which the Gaussian process is sampled
	 */
	std::vector<double> times;
};

}}		// end lcmc::test

#endif		// end LCMCTESTH
