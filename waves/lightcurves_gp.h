/** Type definitions for light curves describable as Gaussian processes
 * @file lightcurveMC/waves/lightcurves_gp.h
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified May 12, 2013
 */

#ifndef LCMCCURVEGPH
#define LCMCCURVEGPH

#include <boost/shared_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include "lcstochastic.h"

namespace lcmc { namespace models {

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
	/** Initializes the light curve to represent a standard Gaussian process.
	 */
	explicit SimpleGp(const std::vector<double>& times, double sigma, double tau);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	boost::shared_ptr<gsl_matrix> getCovar() const;
	
	double sigma, tau;
};

/** TwoScaleGp represents variables that vary as a two-timescale analog to a 
 * standard Gaussian process in magnitude space.
 */
class TwoScaleGp : public Stochastic {
public: 
	/** Initializes the light curve to represent a two-component 
	 *	Gaussian process.
	 */
	explicit TwoScaleGp(const std::vector<double>& times, 
			double sigma1, double tau1, double sigma2, double tau2);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	boost::shared_ptr<gsl_matrix> getCovar() const;

	double sigma1, sigma2, tau1, tau2;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVEGPH
