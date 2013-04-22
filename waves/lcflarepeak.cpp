/** Defines the FlarePeak light curve class.
 * @file lcflarepeak.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified March 20, 2013
 */

#include <stdexcept>
#include <cmath>
#include <cstdio>
#include "lightcurves_outbursts.h"

namespace lcmc { namespace models {

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
 * @pre amp > 0
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 * @pre fade > 0
 * @pre width > 0
 *
 * @post A light curve is a deterministic function of amp, period, phase, and 
 *	width: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
FlarePeak::FlarePeak(const std::vector<double> &times, 
			double amp, double period, double phase, double rise, double fade) 
			: PeriodicLc(times, amp, period, phase), tExp(fade), tLin(rise) {
	char errBuf[80];
	if (rise <= 0.0) {
		sprintf(errBuf, "All FlarePeak light curves need positive rise times (gave %g).", rise);
		throw std::invalid_argument(errBuf);
	}
	if (fade <= 0.0) {
		sprintf(errBuf, "All FlarePeak light curves need positive fade times (gave %g).", fade);
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
double FlarePeak::fluxPhase(double phase) const {
	double tail = exp(-phase/tExp);
	if (phase < 1.0 - tLin) {
		return tail;
	} else {
		return tail + (phase - 1.0 + tLin)/tLin;
	}
}

}}		// end lcmc::models
