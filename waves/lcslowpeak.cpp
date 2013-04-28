/** Defines the SlowPeak light curve class.
 * @file lcslowpeak.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified April 27, 2013
 */

#include <stdexcept>
#include <cmath>
#include <cstdio>
#include "lightcurves_outbursts.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a periodically outbursting 
 * function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] width The 1-&sigma; width of the Gaussian peak, in units of the 
 *	period.
 *
 * @pre amp > 0
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 * @pre width > 0
 * @pre width < 0.234107
 *
 * @note maximum width is set by a phase transition in the function behavior
 *
 * @post A light curve is a deterministic function of amp, period, phase, and 
 *	width: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
SlowPeak::SlowPeak(const std::vector<double> &times, 
		double amp, double period, double phase, double width) 
		: PeriodicLc(times, amp, period, phase), width(width) {
	char errBuf[80];
	if (width <= 0.0) {
		sprintf(errBuf, "All SlowPeak light curves need positive widths (gave %g).", width);
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
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the return value is greater than or equal to 1
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @return The flux emitted by the object at the specified phase.
 */
double SlowPeak::fluxPhase(double phase, double amp) const {
	return 1.0 + amp * exp(-(   phase *   phase) /(2.0*width*width)) 
		   + amp * exp(-((1-phase)*(1-phase))/(2.0*width*width));
}

}}		// end lcmc::models
