/** Defines the SineWave light curve class.
 * @file lcsine.cpp
 * @author Krzysztof Findeisen
 * @date Created April 24, 2012
 * @date Last modified March 19, 2013
 */

#include <stdexcept>
#include <cmath>
#include <cstdio>
#include "lightcurves_periodic.h"

/** Define Pi for convenience
 */
#ifndef M_PI
#define M_PI 3.1415927
#endif

namespace lcmcmodels {

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre amp > 0
 * @pre amp &le; 2
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 *
 * @post The object represents a sinusoidal signal with the given 
 *	amplitude, period, and initial phase.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
SineWave::SineWave(const std::vector<double> &times, double amp, double period, double phase) 
		: PeriodicLc(times, amp, period, phase) {
	if (amp > 2.0) {
		char errBuf[80];
		sprintf(errBuf, "SineWaves must have amplitudes less than or equal to 2 (gave %g).", amp);
		throw std::invalid_argument(errBuf);
	}
}

/** Samples the sinusoidal waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
 * 
 * @pre phase &isin; [0, 1)
 *
 * @post fluxPhase(phi) is a sinusoidal function of phi with period 1.
 * 
 * @return The flux emitted by the object at the specified phase.
 */
double SineWave::fluxPhase(double phase) const {
	return 0.5*sin(2*M_PI*phase);
}

}		// end lcmcmodels
