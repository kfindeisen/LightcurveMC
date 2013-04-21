/** Defines the SquarePeak light curve class.
 * @file lcsquarepeak.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified March 19, 2013
 */

#include <stdexcept>
#include <cstdio>
#include "lightcurves_outbursts.h"

namespace lcmcmodels {

/** Initializes the light curve to represent a periodically outbursting function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] width The width of the peak, in units of the period.
 *
 * @pre amp > 0
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 * @pre width > 0
 * @pre width < 1
 *
 * @post A light curve is a deterministic function of amp, period, phase, and 
 *	width: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
SquarePeak::SquarePeak(const std::vector<double> &times, 
			double amp, double period, double phase, double width) : PeriodicLc(times, amp, period, phase), width(width) {
	char errBuf[80];
	if (width <= 0.0) {
		sprintf(errBuf, "All SquarePeak light curves need positive widths (gave %g).", width);
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
double SquarePeak::fluxPhase(double phase) const {
	if (phase < width) {
		return 1.0;
	} else {
		return 0.0;
	}
}

}		// end lcmcmodels
