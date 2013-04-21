/** Defines the EclipseWave light curve class.
 * @file lceclipse.cpp
 * @author Krzysztof Findeisen
 * @date Created April 24, 2012
 * @date Last modified March 19, 2013
 */

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include "lightcurves_periodic.h"

namespace lcmcmodels {

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre amp > 0
 * @pre amp &le; 1
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 *
 * @post The object represents a periodic signal with the given 
 *	amplitude, period, and initial phase. The light curve shows two 
 *	minima of different depths per cycle.
 *
 * @exception std::invalid_argument Thrown if any of the parameters are 
 *	outside their allowed ranges.
 *
 * @todo Reimplement as a more generic function?
 */
EclipseWave::EclipseWave(const std::vector<double> &times, 
			double amp, double period, double phase) 
			: PeriodicLc(times, amp, period, phase) {
	if (amp > 1.0) {
		char errBuf[80];
		sprintf(errBuf, "EclipseWaves must have amplitudes less than or equal to 1 (gave %g).", amp);
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
double EclipseWave::fluxPhase(double phase) const {
	// Primary eclipse
	if (phase >= 0.0 && phase <= 0.05)
		return -1.0;
	// Secondary eclipse
	else if (phase >= 0.5 && phase <= 0.55)
		return -0.7;
	else
		return 0.0;
}

}		// end lcmcmodels
