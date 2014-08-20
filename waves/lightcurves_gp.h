/** Type definitions for light curves describable as Gaussian processes
 * @file lightcurveMC/waves/lightcurves_gp.h
 * @author Krzysztof Findeisen
 * @date Created March 21, 2013
 * @date Last modified August 1, 2013
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

#ifndef LCMCCURVEGPH
#define LCMCCURVEGPH

#include <boost/smart_ptr.hpp>
#include <gsl/gsl_matrix.h>
#include "lcstochastic.h"

namespace lcmc { namespace models {

/** GaussianProcess represents variables that vary as any kind of Gaussian 
 * process in magnitude space.
 *
 * GaussianProcess is an abstract base class. Subclasses of GaussianProcess 
 * represent Gaussian processes with specific kernels by implementing 
 * the private function getCovar() and optionally overriding the private 
 * function solveFluxes().
 */
class GaussianProcess : public Stochastic {
public: 
	/** Initializes the light curve to represent a Gaussian process.
	 */
	explicit GaussianProcess(const std::vector<double>& times);

private:
	/** Computes a realization of the light curve. 
	 */
	virtual void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	virtual boost::shared_ptr<gsl_matrix> getCovar() const = 0;
};

/** WhiteNoise represents variables that vary stochastically on infinitesimal 
 *	time scales.
 */
class WhiteNoise : public GaussianProcess {
public: 
	/** Initializes the light curve to represent a white noise process.
	 */
	explicit WhiteNoise(const std::vector<double>& times, double sigma);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	boost::shared_ptr<gsl_matrix> getCovar() const;

	double sigma;
};

/** RandomWalk represents variables that vary stochastically without bound in 
 * magnitude space.
 */
class RandomWalk : public GaussianProcess {
public: 
	/** Initializes the light curve to represent a random walk.
	 */
	explicit RandomWalk(const std::vector<double>& times, double diffus);

private:
	/** Computes a realization of the light curve. 
	 */	
	void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	boost::shared_ptr<gsl_matrix> getCovar() const;

	double d;
};

/** DampedRandomWalk represents variables that vary stochastically within 
 * bounds in magnitude space.
 */
class DampedRandomWalk : public GaussianProcess {
public: 
	/** Initializes the light curve to represent a damped random walk.
	 */
	explicit DampedRandomWalk(const std::vector<double>& times, double diffus, double tau);

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

/** SimpleGp represents variables that vary as a standard Gaussian process in 
 * magnitude space.
 */
class SimpleGp : public GaussianProcess {
public: 
	/** Initializes the light curve to represent a standard Gaussian process.
	 */
	explicit SimpleGp(const std::vector<double>& times, double sigma, double tau);

private:
	/** Computes a realization of the light curve. 
	 */	
//	void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	boost::shared_ptr<gsl_matrix> getCovar() const;
	
	double sigma, tau;
};

/** TwoScaleGp represents variables that vary as a two-timescale analog to a 
 * standard Gaussian process in magnitude space.
 */
class TwoScaleGp : public GaussianProcess {
public: 
	/** Initializes the light curve to represent a two-component 
	 *	Gaussian process.
	 */
	explicit TwoScaleGp(const std::vector<double>& times, 
			double sigma1, double tau1, double sigma2, double tau2);

private:
	/** Computes a realization of the light curve. 
	 */	
//	void solveFluxes(std::vector<double>& fluxes) const;
	
	/** Allocates and initializes the covariance matrix for the 
	 *	Gaussian process. 
	 */
	boost::shared_ptr<gsl_matrix> getCovar() const;

	double sigma1, sigma2, tau1, tau2;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVEGPH
