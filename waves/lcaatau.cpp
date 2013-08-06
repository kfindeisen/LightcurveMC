/** Defines the AaTauWave light curve class.
 * @file lightcurveMC/waves/lcaatau.cpp
 * @author Krzysztof Findeisen
 * @date Created July 31, 2013
 * @date Last modified August 1, 2013
 */

#include <cmath>
#include <boost/lexical_cast.hpp>
#include <gsl/gsl_math.h>
#include "../except/data.h"
#include "../fluxmag.h"
#include "lightcurves_periodic.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodic function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve in magnitudes
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] width The fraction of the light curve that takes place 
 *	within a dip
 *
 * @pre @p amp > 0
 * @pre @p period > 0
 * @pre @p phase &isin; [0, 1)
 * @pre @p width &isin; (0, 1]
 *
 * @post The object represents a periodic signal with the given 
 *	amplitude, period, and initial phase, and dips lasting 
 *	@p width of a full cycle.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
AaTauWave::AaTauWave(const std::vector<double> &times, double amp, double period, 
		double phase, double width) : 
		PeriodicLc(times, amp, period, phase), width(width) {
	if (width <= 0.0) {
		throw except::BadParam("All AaTauWave light curves need positive widths (gave " 
			+ lexical_cast<string>(width) + ").");
	}
	if (width > 1.0) {
		throw except::BadParam("All AaTauWave light curves need widths less than or equal to 1 (gave " 
			+ lexical_cast<string>(width) + ").");
	}
}

/** Samples the waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
 * @param[in] amp The light curve amplitude, in the same units 
 *	as passed to the constructor.
 * 
 * @return The flux emitted by the object at the specified phase.
 * 
 * @pre @p phase &isin; [0, 1)
 *
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the return value is non-negative
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double AaTauWave::fluxPhase(double phase, double amp) const {
	double mag;
	if (phase < 0.5*width) {
		mag = -amp*cos(M_PI * phase / width);
	} else if (phase > 1.0 - 0.5*width) {
		mag = -amp*cos(M_PI * (phase-1.0) / width);
	} else {
		mag = 0.0;
	}
	
	return utils::magToFlux(mag);
}

}}		// end lcmc::models
