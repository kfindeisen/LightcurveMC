/** Defines the EclipseWave light curve class.
 * @file lceclipse.cpp
 * @author Krzysztof Findeisen
 * @date Created April 24, 2012
 * @date Last modified April 27, 2013
 */

#include <cmath>
#include <cstdio>
#include <stdexcept>
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
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post The mode of the flux is zero, when averaged over many times.
 * @post 0 &le; return value &le; 1
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @return The flux emitted by the object at the specified phase.
 */
double EclipseWave::fluxPhase(double phase, double amp) const {
	// Primary eclipse
	if (phase >= 0.0 && phase <= 0.05)
		return 1.0 - amp;
	// Secondary eclipse
	else if (phase >= 0.5 && phase <= 0.55)
		return 1.0 - 0.7*amp;
	else
		return 1.0;
}

}}		// end lcmc::models
