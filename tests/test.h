/** Utilities for test suite
 * @file test.h
 * @author Krzysztof Findeisen
 * @date Created April 28, 2013
 * @date Last modified April 28, 2013
 */

#ifndef LCMCTESTH
#define LCMCTESTH

#include <memory>
#include <vector>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "../lightcurvetypes.h"

namespace lcmc { namespace test {

/** Wrapper for a trusted Nan-testing function
 */
bool testNan(const double x);

/** Constructs a covariance matrix for a standard Gaussian process
 */
void initGauss(gsl_matrix* const covars, const std::vector<double>& times, double tau);

/** This function is a wrapper for a trusted approximate comparison method.
 */
bool isClose(double val1, double val2, double frac);

/** This function emulates the BOOST_CHECK_CLOSE_FRACTION macro.
 */
void myTestClose(double val1, double val2, double frac);

/** Function object for testing whether two values are approximately equal
 */
class ApproxEqual {
public: 
	/** Defines an object testing for approximate equality with 
	 *	a particular tolerance
	 */
	explicit ApproxEqual(double epsilon);

	/** Tests whether two values are approximately equal
	 */
	bool operator() (double x, double y);

private:
	double epsilon;
};

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

}}		// end lcmc::test

#endif		// end LCMCTESTH
