/** Defines the MagSineWave light curve class.
 * @file lcmagsine.cpp
 * @author Krzysztof Findeisen
 * @date Created April 11, 2013
 * @date Last modified April 11, 2013
 */

#include <cmath>
#include <cstdio>
#include "../fluxmag.h"
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
 * @param[in] amp The amplitude of the light curve in magnitudes
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre amp > 0
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 *
 * @post The object represents a sinusoidal signal with the given 
 *	amplitude, period, and initial phase.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 */
MagSineWave::MagSineWave(const std::vector<double> &times, double amp, double period, 
		double phase) : PeriodicLc(times, amp, period, phase) {
}

/** Samples the sinusoidal waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
 * 
 * @pre phase &isin; [0, 1)
 *
 * @post fluxToMag(fluxPhase(phi)) is a sinusoidal function of phi with period 1.
 * 
 * @return The flux emitted by the object at the specified phase.
 */
double MagSineWave::fluxPhase(double phase) const {
	return lcmcutils::magToFlux(0.5*sin(2*M_PI*phase));
}

}		// end lcmcmodels
