/** Defines the FlareDip light curve class.
 * @file lcFlareDip.cpp
 * @author Krzysztof Findeisen
 * @date Created August 21, 2012
 * @date Last modified May 11, 2013
 */

#include <cmath>
#include <boost/lexical_cast.hpp>
#include "../except/data.h"
#include "lightcurves_fades.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodically fading function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] fade The start-to-minimum linear fade time of the flare-like dip, 
 *	in units of the period.
 * @param[in] width The exponential decay time of the flare-like dip, 
 *	in units of the period.
 *
 * @pre amp > 0
 * @pre amp &le; 1
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 * @pre 0 < fade < 1
 * @pre 0 < width < 1
 *
 * @post A light curve is a deterministic function of amp, period, phase,  
 *	fade, and width: knowing these values is sufficient to determine 
 *	flux(t) for any value of t.
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
FlareDip::FlareDip(const std::vector<double> &times, 
			double amp, double period, double phase, double fade, double width) 
			: PeriodicLc(times, amp, period, phase), tExp(width), tLin(fade) {
	if (amp > 1.0) {
		throw except::BadParam("All SlowDip light curves need amplitudes < 1 (gave " 
			+ lexical_cast<string, double>(amp) + ").");
	}
	if (fade <= 0.0) {
		throw except::BadParam("All FlareDip light curves need positive fall times (gave " 
			+ lexical_cast<string, double>(fade) + ").");
	}
	if (width <= 0.0) {
		throw except::BadParam("All FlareDip light curves need positive recovery times (gave " 
			+ lexical_cast<string, double>(width) + ").");
	}
}

/** Samples the waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
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
 * @return The flux emitted by the object at the specified phase.
 * 
 * @exception logic_error Thrown if a bug was found in the flux calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double FlareDip::fluxPhase(double phase, double amp) const {
	double tail = exp(-phase/tExp);
	if (phase < 1.0 - tLin) {
		return 1.0-amp*tail;
	} else {
//		return 1.0-tail - amp*(phase - 1.0 + tLin)/tLin;
		return 1.0 - amp + amp*(tail - 1) / tLin * (phase - 1.0);
	}
}

}}		// end lcmc::models
