/** Type definitions for light curves describable as Gaussian processes
 * @file lightcurves_gp.h
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified April 3, 2013
 */

#ifndef LCMCCURVEGPH
#define LCMCCURVEGPH

#include "lcsubtypes.h"

namespace lcmcmodels {

/** WhiteNoise represents variables that vary stochastically on infinitesimal 
 *	time scales.
 */
class WhiteNoise : public Stochastic {
public: 
	/** Initializes the light curve to represent a white noise process.
	 */
	explicit WhiteNoise(const std::vector<double>& times, double sigma);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	double sigma;
};

/** RandomWalk represents variables that vary stochastically without bound in 
 * magnitude space.
 */
class RandomWalk : public Stochastic {
public: 
	/** Initializes the light curve to represent a random walk.
	 */
	explicit RandomWalk(const std::vector<double>& times, double diffus);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	double d;
};

/** DampedRandomWalk represents variables that vary stochastically within 
 * bounds in magnitude space.
 */
class DampedRandomWalk : public Stochastic {
public: 
	/** Initializes the light curve to represent a damped random walk.
	 */
	explicit DampedRandomWalk(const std::vector<double>& times, double diffus, double tau);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	double sigma, tau;
};

/** SimpleGp represents variables that vary as a standard Gaussian process in 
 * magnitude space.
 */
class SimpleGp : public Stochastic {
public: 
	/** Initializes the light curve to represent a Gaussian process.
	 */
	explicit SimpleGp(const std::vector<double>& times, double sigma, double tau);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	double sigma, tau;
};

/** TwoScaleGp represents variables that vary as a two-timescale analog to a 
 * standard Gaussian process in magnitude space.
 */
class TwoScaleGp : public Stochastic {
public: 
	/** Initializes the light curve to represent a Gaussian process.
	 */
	explicit TwoScaleGp(const std::vector<double>& times, 
			double sigma1, double sigma2, double tau1, double tau2);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	double sigma1, sigma2, tau1, tau2;
};

}		// end lcmcmodels

#endif		// end ifndef LCMCCURVEGPH
