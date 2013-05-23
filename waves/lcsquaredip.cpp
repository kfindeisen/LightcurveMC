/** Defines the SquareDip light curve class.
 * @file lightcurveMC/waves/lcSquareDip.cpp
 * @author Krzysztof Findeisen
 * @date Created August 21, 2012
 * @date Last modified May 22, 2013
 */

#include <cmath>
#include <boost/lexical_cast.hpp>
#include "../except/data.h"
#include "lightcurves_fades.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodically fading function 
 * flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] width The width of the dip, in units of the period.
 *
 * @pre amp > 0
 * @pre amp &le; 1
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 * @pre width > 0
 * @pre width < 1
 *
 * @post A light curve is a deterministic function of amp, period, phase, and 
 *	width: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
SquareDip::SquareDip(const std::vector<double> &times, 
			double amp, double period, double phase, double width) 
			: PeriodicLc(times, amp, period, phase), width(width) {
	if (amp > 1.0) {
		throw except::BadParam("All SlowDip light curves need amplitudes < 1 (gave " 
			+ lexical_cast<string>(amp) + ").");
	}
	if (width <= 0.0) {
		throw except::BadParam("All SquareDip light curves need positive widths (gave " 
			+ lexical_cast<string>(width) + ").");
	}
}

/** Samples the waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
 * 
 * @return The flux emitted by the object at the specified phase.
 * 
 * @pre phase &isin; [0, 1)
 *
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the return value is non-negative
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @exception std::logic_error Thrown if a bug was found in the flux calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double SquareDip::fluxPhase(double phase, double amp) const {
	if (phase < width) {
		return 1.0 - amp;
	} else {
		return 1.0;
	}
}

}}		// end lcmc::models
