/** Defines the TriangleWave light curve class.
 * @file lightcurveMC/waves/lctriangle.cpp
 * @author Krzysztof Findeisen
 * @date Created April 24, 2012
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
#include <gsl/gsl_math.h>
#include "../except/data.h"
#include "lightcurves_periodic.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The half-amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre @p amp > 0
 * @pre @p amp &le; 1
 * @pre @p period > 0
 * @pre @p phase &isin; [0, 1)
 *
 * @post The object represents a pseudo-sinusoidal signal with the 
 *	given amplitude, period, and initial phase.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
TriangleWave::TriangleWave(const std::vector<double> &times, 
		double amp, double period, double phase) 
		: PeriodicLc(times, amp, period, phase) {
	if (amp > 1.0) {
		throw except::BadParam("TriangleWaves must have amplitudes less than or equal to 1 (gave " + lexical_cast<string>(amp) + ").");
	}
}

/** Samples the waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
 * @param[in] amp The light curve amplitude, in the same units 
 *	as passed to the constructor.
 * 
 * @return The flux emitted by the object at the specified phase.
 * 
 * @pre @p phase &isin; [0, 1)
 *
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the mean and median of the flux are one, when 
 *	averaged over many times.
 * @post 0 &le; return value &le; 2
 * 
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double TriangleWave::fluxPhase(double phase, double amp) const {
	return 1 + amp*1.11803*sin(2*M_PI*phase)/(1.5 + cos(2*M_PI*phase));
}

}}		// end lcmc::models
