/** Declares the direct subclasses of LightCurve
 * @file lightcurveMC/waves/lcdeterministic.h
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

#ifndef LCMCCURVEDETERMH
#define LCMCCURVEDETERMH

#include <gsl/gsl_rng.h>
#include "../lightcurvetypes.h"


namespace lcmc { namespace models {

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

	/** Returns the number of times and fluxes
	 */
	virtual size_t size() const;
	
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
	 * @return The flux emitted by the object at the specified time.
	 *
 	 * @post the return value is determined entirely by the time and 
 	 *	the parameters passed to the constructor
 	 *
 	 * @post the return value is not NaN
 	 * @post the return value is non-negative
	 * @post Either the mean, median, or mode of the flux is one, when 
	 *	averaged over many times. Subclasses of Deterministic may 
	 *	chose the option (mean, median, or mode) most appropriate 
	 *	for their light curve shape.
	 * 
	 * @exception std::logic_error Thrown if a bug was found in the flux calculations.
	 *
	 * @exceptsafe Neither the object nor the argument are changed in the 
	 *	event of an exception.
	 */	
	virtual double flux(double time) const = 0;

	std::vector<double> times;
};

}}	// end lcmc::models

#endif
