/** Declares the direct subclasses of LightCurve
 * @file lcsubtypes.h
 * @author Krzysztof Findeisen
 * @date Created March 18, 2013
 * @date Last modified March 21, 2013
 */

#ifndef LCMCCURVEMAJORH
#define LCMCCURVEMAJORH

#include <gsl/gsl_randist.h>
#include "../lightcurvetypes.h"


namespace lcmcmodels {

/** Deterministic is the base class for all light curve models that can, in 
 * principle, be expressed as a well-defined function of time. A Deterministic 
 * object represents a function whose value is determined entirely by the 
 * parameters passed to its constructor. 
 */
class Deterministic : public ILightCurve {
public: 
	virtual ~Deterministic();

	/** Returns the times at which the simulated data were taken
	 */
	virtual void getTimes(std::vector<double>& timeArray) const;

	/** Returns the simulated fluxes at the corresponding times
	 */
	virtual void getFluxes(std::vector<double>& fluxArray) const;

protected:
	/** Initializes the light curve to represent a particular function 
	 * flux(time).
	 */
	explicit Deterministic(const std::vector<double> &times);

private:
	/** Samples the light curve at the specified time.
	 * 
	 * @param[in] time The time at which an observation is taken. 
	 *	Observations are assumed to be instantaneous, with no averaging over 
	 *	rapid variability.
	 *
	 * @post flux(t) is a deterministic function of t.
	 * 
	 * @return The flux emitted by the object at the specified time.
	 */	
	virtual double flux(double time) const = 0;

	std::vector<double> times;
};

/** Stochastic is the base class for all light curve models that have a 
 * randomly fluctuating aspect. A Stochastic object represents a particular 
 * realization of a light curve observed at a particular cadence. While the 
 * value of getFluxes() is fixed for any individual object, two identically 
 * constructed Stochastic objects are *not* guaranteed to have the same value 
 * of getFluxes().
 */
class Stochastic : public ILightCurve {
public: 
	virtual ~Stochastic();

	/** Returns the times at which the simulated data were taken
	 */
	virtual void getTimes(std::vector<double>& timeArray) const;

	/** Returns the simulated fluxes at the corresponding times
	 */
	virtual void getFluxes(std::vector<double>& fluxArray) const;

protected:
	/** Initializes the light curve to represent an instance of a 
	 * stochastic time series.
	 */
	explicit Stochastic(const std::vector<double> &times);
	/** Draws a standard uniform random variate.
	 */
	double rUnif() const;
	
	/** Draws a standard normal random variate.
	 */
	double rNorm() const;

private:
	/** Computes a realization of the light curve. 
	 *
	 * The light curve is computed from times and the internal random 
	 * number generator, and its values are stored in fluxes.
	 *
	 * @param[out] fluxes A write-only reference to Stochastic::fluxes.
	 *
	 * @invariant solveFluxes() will be called at most once for any 
	 *	instance of Stochastic
	 * 
	 * @post getFluxes() will now return the correct light curve.
	 */	
	virtual void solveFluxes(std::vector<double>& fluxes) const = 0;
	
	/** Defines the random number state of the first object in the class.
	 */
	static gsl_rng * init_rng();

	std::vector<double> times;
	// Mutable allows use of solveFluxes() as a cache
	mutable std::vector<double> fluxes;
	mutable bool fluxesSolved;
	
	static gsl_rng * const rng;
};

}

#endif
