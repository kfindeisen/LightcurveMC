/** Defines the PeriodicLc base class.
 * @file lightcurveMC/waves/lcperiodic.cpp
 * @author Krzysztof Findeisen
 * @date Created April 23, 2012
 * @date Last modified July 31, 2013
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

#include <cmath>
#include <boost/lexical_cast.hpp>
#include "../except/data.h"
#include "lightcurves_periodic.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre @p amp > 0
 * @pre @p period > 0
 * @pre @p phase &isin; [0, 1)
 *
 * @post A light curve is a deterministic function of the amplitude, period, 
 *	and phase: knowing them is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
PeriodicLc::PeriodicLc(const std::vector<double> &times, 
			double amp, double period, double phase) 
			: Deterministic(times), amp(amp), period(period), phase0(phase) {
	if (amp <= 0.0) {
		throw except::BadParam("All periodic light curves need positive amplitudes (gave " + lexical_cast<string>(amp) + ").");
	}
	if (period <= 0.0) {
		throw except::BadParam("All periodic light curves need positive periods (gave " + lexical_cast<string>(period) + ").");
	}
	if (phase < 0.0 || phase >= 1.0) {
		throw except::BadParam("All periodic light curves need initial phases in the interval [0, 1) (gave " + lexical_cast<string>(phase) + ").");
	}
}

PeriodicLc::~PeriodicLc() {
}

/** Samples the light curve at the specified time.
 *
 * @note This method is final. Subclasses should override fluxPhase() 
 * 	instead.
 * 
 * @param[in] time The time at which an observation is taken. 
 *	Observations are assumed to be instantaneous, with no averaging over 
 *	rapid variability.
 * @param[in] amp The light curve amplitude, in the same units 
 *	as passed to the constructor.
 * 
 * @return The flux emitted by the object at the specified time.
 *
 * @post the return value is determined entirely by the time and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the return value is non-negative
 * @post Either the mean, median, or mode of the flux is one, when 
 *	averaged over many times. Subclasses of PeriodicLc may 
 *	chose the option (mean, median, or mode) most appropriate 
 *	for their light curve shape.
 * 
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double PeriodicLc::flux(double time) const {
	double phase = phase0 + time / period;
	phase = phase - floor(phase);
	
	return fluxPhase(phase, amp);
}

}}		// end lcmc::models
