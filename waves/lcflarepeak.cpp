/** Defines the FlarePeak light curve class.
 * @file lcflarepeak.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified April 28, 2013
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
 * @pre 0 < width < 1
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
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the return value is greater than or equal to 1
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @return The flux emitted by the object at the specified phase.
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
