/** Defines the EllipseWave light curve class.
 * @file lcellipse.cpp
 * @author Krzysztof Findeisen
 * @date Created April 24, 2012
 * @date Last modified May 11, 2013
 */

#include <cmath>
#include <boost/lexical_cast.hpp>
#include "../except/data.h"
#include "lightcurves_periodic.h"

/** Define Pi for convenience
 */
#ifndef M_PI
#define M_PI 3.1415927
#endif

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The half-amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 *
 * @pre amp > 0
 * @pre amp &le; 1
 * @pre period > 0
 * @pre phase &isin; [0, 1)
 *
 * @post The object represents a pseudo-sinusoidal signal with the 
 *	given amplitude, period, and initial phase.
 *
 * @exception bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 *
 * @todo Reimplement as a more generic function?
 */
EllipseWave::EllipseWave(const std::vector<double> &times, 
		double amp, double period, double phase) 
		: PeriodicLc(times, amp, period, phase) {
	if (amp > 1.0) {
		throw except::BadParam("EllipseWaves must have amplitudes less than or equal to 1 (gave " + lexical_cast<string, double>(amp) + ").");
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
 * @post the mean and median of the flux are one, when 
 *	averaged over many times.
 * @post 0 &le; return value &le; 2
 * 
 * @return The flux emitted by the object at the specified phase.
 * 
 * @exception logic_error Thrown if a bug was found in the flux calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double EllipseWave::fluxPhase(double phase, double amp) const {
	return 1 + amp*0.458258*sin(2*M_PI*phase)/(1.1 + cos(2*M_PI*phase));
}

}}		// end lcmc::models
