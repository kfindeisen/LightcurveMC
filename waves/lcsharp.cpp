/** Defines the SharpPeakWave light curve class.
 * @file lcsharp.cpp
 * @author Krzysztof Findeisen
 * @date Created April 24, 2012
 * @date Last modified March 19, 2013
 */

#include <cmath>
#include "lightcurves_periodic.h"

/** Define Pi for convenience
 */
#ifndef M_PI
#define M_PI 3.1415927
#endif

namespace lcmcmodels {

/** Initializes the light curve to represent a periodic function 
 * flux(time).
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
 * @post The object represents a peaked periodic signal with the 
 *	given amplitude, period, and initial phase.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Reimplement as a more generic function?
 */
SharpPeakWave::SharpPeakWave(const std::vector<double> &times, 
			double amp, double period, double phase) 
			: PeriodicLc(times, amp, period, phase) {
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
double SharpPeakWave::fluxPhase(double phase) const {
	return -0.05 + 0.105/(1.1 + sin(2*M_PI*phase));
}

}		// end lcmcmodels
