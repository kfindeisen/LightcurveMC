/** Defines the PeriodicLc base class.
 * @file lcperiodic.cpp
 * @author Krzysztof Findeisen
 * @date Created April 23, 2012
 * @date Last modified April 27, 2013
 */

#include <stdexcept>
#include <cmath>
#include <cstdio>
#include "lightcurves_periodic.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre amp > 0
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 *
 * @post A light curve is a deterministic function of the amplitude, period, 
 *	and phase: knowing them is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
PeriodicLc::PeriodicLc(const std::vector<double> &times, 
			double amp, double period, double phase) 
			: Deterministic(times), amp(amp), period(period), phase0(phase) {
	char errBuf[80];
	if (amp <= 0.0) {
		sprintf(errBuf, "All periodic light curves need positive amplitudes (gave %g).", amp);
		throw std::invalid_argument(errBuf);
	}
	if (period <= 0.0) {
		sprintf(errBuf, "All periodic light curves need positive periods (gave %g).", period);
		throw std::invalid_argument(errBuf);
	}
	if (phase < 0.0 || phase >= 1.0) {
		sprintf(errBuf, "All periodic light curves need initial phases in the interval [0, 1) (gave %g).", phase);
		throw std::invalid_argument(errBuf);
	}
}

PeriodicLc::~PeriodicLc() {
}

/** Samples the light curve at the specified time.
 * 
 * @param[in] time The time at which an observation is taken. 
 *	Observations are assumed to be instantaneous, with no averaging over 
 *	rapid variability.
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
 * @return The flux emitted by the object at the specified time.
 *
 * @note This method is final. Subclasses should override fluxPhase() 
 * 	instead.
 */
double PeriodicLc::flux(double time) const {
	double phase = phase0 + time / period;
	phase = phase - floor(phase);
	return fluxPhase(phase, amp);
}

}}		// end lcmc::models
