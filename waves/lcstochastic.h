/** Declares the stochastic light curve class
 * @file lightcurveMC/waves/lcstochastic.h
 * @author Krzysztof Findeisen
 * @date Created May 12, 2013
 * @date Last modified May 22, 2013
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

#ifndef LCMCCURVESTOCHH
#define LCMCCURVESTOCHH

#include <memory>
#include <gsl/gsl_rng.h>
#include "../lightcurvetypes.h"


namespace lcmc { namespace models {

using std::auto_ptr;

/** Wrapper class for a random number generator.
 *
 * This class allows stochastic light curves to use basic RNG functionality 
 *	without being sensitive to the implementation of the random number 
 *	generator.
 *
 * @todo Rewrite in terms of pImpl.
 */
class StochasticRng {
public: 
	/** Initializes a random number generator
	 */
	explicit StochasticRng(unsigned long seed);

	~StochasticRng();
	
	/** Creates a random number generator with an identical 
	 *	state to another
	 */
	StochasticRng(const StochasticRng& other);

	/** Sets the random number generator state equal to another generator
	 */
	const StochasticRng& operator=(const StochasticRng& other);
	
	/** Draws a standard uniform random variate.
	 */
	double rUnif() const;
	
	/** Draws a standard normal random variate.
	 */
	double rNorm() const;

private:
	gsl_rng * const rng;
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

	/** Returns the number of times and fluxes
	 */
	virtual size_t size() const;
	
protected:
	/** Initializes the light curve to represent an instance of a 
	 * stochastic time series.
	 */
	explicit Stochastic(const std::vector<double> &times);
	
	/** Creates a temporary copy of a random number generator
	 */
	auto_ptr<StochasticRng> checkout() const;

	/** Updates the state of the random number generator
	 */
	void commit(auto_ptr<StochasticRng> newState) const;

private:
	/** Computes a realization of the light curve. 
	 *
	 * The light curve is computed from times and the internal random 
	 * number generator, and its values are stored in fluxes.
	 *
	 * @param[out] fluxes A write-only reference to Stochastic::fluxes.
	 * 
	 * @post getFluxes() will now return the correct light curve.
	 * 
	 * @post @p fluxes.size() = getTimes().size()
	 * @post if getTimes()[i] = getTimes()[j] for i &ne; j, then 
	 *	@p fluxes[i] = @p fluxes[j]
	 * 
	 * @post No element of @p fluxes is NaN
	 * @post All elements in @p fluxes are non-negative
	 * @post Either the mean, median, or mode of the flux is one, when 
	 *	averaged over many elements and many light curve instances. 
	 *	Subclasses of Stochastic may chose the option 
	 *	(mean, median, or mode) most appropriate for their light 
	 *	curve shape.
	 *
	 * @invariant solveFluxes() will be called at most once for any 
	 *	instance of Stochastic
	 * 
	 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
	 *	the light curve.
	 * @exception std::logic_error Thrown if a bug was found in the flux calculations.
	 *
	 * @exceptsafe Neither the object nor the argument are changed in the 
	 *	event of an exception.
	 */	
	virtual void solveFluxes(std::vector<double>& fluxes) const = 0;
	
	/** Defines the random number generator for stochastic light curves
	 */
	static StochasticRng& rng();

	std::vector<double> times;
	// Mutable allows use of solveFluxes() as a cache
	// assert: only solveFluxes(), and no other function, can change 
	//	these values
	mutable std::vector<double> fluxes;
	mutable bool fluxesSolved;
};

}}	// end lcmc::models

#endif
