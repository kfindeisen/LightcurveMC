/** Defines the SlowDip light curve class.
 * @file lcSlowDip.cpp
 * @author Krzysztof Findeisen
 * @date Created August 21, 2012
 * @date Last modified March 19, 2013
 */

#include <stdexcept>
#include <cmath>
#include <cstdio>
#include "lightcurves_fades.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a periodically fading 
 * function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] width The 1-&sigma; width of the Gaussian dip, in units of the 
 *	period.
 *
 * @pre amp > 0
 * @pre amp &le; 1
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 * @pre width > 0
 *
 * @post A light curve is a deterministic function of amp, period, phase, and 
 *	width: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
SlowDip::SlowDip(const std::vector<double> &times, 
		double amp, double period, double phase, double width) 
		: PeriodicLc(times, amp, period, phase), width(width) {
	char errBuf[80];
	if (amp > 1.0) {
		sprintf(errBuf, "All SlowDip light curves need amplitudes < 1 (gave %g).", amp);
		throw std::invalid_argument(errBuf);
	}
	if (width <= 0.0) {
		sprintf(errBuf, "All SlowDip light curves need positive widths (gave %g).", width);
		throw std::invalid_argument(errBuf);
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
 * @post fluxPhase(phi) is a periodic function of phi with period 1.
 * 
 * @return The flux emitted by the object at the specified phase.
 */
double SlowDip::fluxPhase(double phase) const {
	return   -exp(-(   phase *   phase) /(2.0*width*width)) 
		- exp(-((1-phase)*(1-phase))/(2.0*width*width));
}

}}		// end lcmc::models
