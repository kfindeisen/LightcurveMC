/** Defines the FlarePeak light curve class.
 * @file lightcurveMC/waves/lcflarepeak.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
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
#include "lightcurves_outbursts.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodically 
 * outbursting function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] rise The start-to-peak linear rise time of the flare-like peak, 
 *	in units of the period.
 * @param[in] fade The exponential decay time of the flare-like peak, 
 *	in units of the period.
 *
 * @pre @p amp > 0
 * @pre @p period > 0
 * @pre @p phase &isin; [0, 1)
 * @pre 0 < @p fade
 * @pre 0 < @p rise < 1
 *
 * @post A light curve is a deterministic function of @p amp, @p period, @p phase, @p rise, and 
 *	@p fade: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
FlarePeak::FlarePeak(const std::vector<double> &times, 
			double amp, double period, double phase, double rise, double fade) 
			: PeriodicLc(times, amp, period, phase), tExp(fade), tLin(rise) {
	if (rise <= 0.0) {
		throw except::BadParam("All FlarePeak light curves need positive linear rise times (gave " 
			+ lexical_cast<string>(rise) + ").");
	}
	if (fade <= 0.0) {
		throw except::BadParam("All FlarePeak light curves need positive exponential fade times (gave " 
			+ lexical_cast<string>(fade) + ").");
	}
	if (rise > 1.0) {
		throw except::BadParam("FlarePeaks must have linear rise times less than one period (gave " 
			+ lexical_cast<string>(rise) + " periods).");
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
 * @post the return value is greater than or equal to 1
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double FlarePeak::fluxPhase(double phase, double amp) const {
	double tail = amp*exp(-phase/tExp);
	if (phase < 1.0 - tLin) {
		return 1.0+tail;
	} else {
//		return 1.0+tail + amp*(phase - 1.0 + tLin)/tLin;
		return 1.0 + amp - amp*(tail - 1) / tLin * (phase - 1.0);
	}
}

}}		// end lcmc::models
