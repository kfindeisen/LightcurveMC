/** Defines the SquarePeak light curve class.
 * @file lightcurveMC/waves/lcsquarepeak.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified May 22, 2013
 */

#include <boost/lexical_cast.hpp>
#include "../except/data.h"
#include "lightcurves_outbursts.h"

namespace lcmc { namespace models {

using boost::lexical_cast;

/** Initializes the light curve to represent a periodically outbursting function flux(time).
 *
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] amp The amplitude of the light curve
 * @param[in] period The period of the light curve
 * @param[in] phase The phase of the light curve at time 0
 * @param[in] width The width of the peak, in units of the period.
 *
 * @pre @p amp > 0
 * @pre @p period > 0
 * @pre @p phase &isin; [0, 1)
 * @pre @p width > 0
 * @pre @p width < 1
 *
 * @post A light curve is a deterministic function of @p amp, @p period, @p phase, and 
 *	@p width: knowing these values is sufficient to determine flux(t) for any 
 *	value of t.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception lcmc::models::except::BadParam Thrown if any of the 
 *	parameters are outside their allowed ranges.
 *
 * @exceptsafe Object construction is atomic.
 */
SquarePeak::SquarePeak(const std::vector<double> &times, 
			double amp, double period, double phase, double width) : PeriodicLc(times, amp, period, phase), width(width) {
	if (width <= 0.0) {
		throw except::BadParam("All SquarePeak light curves need positive widths (gave " 
		+ lexical_cast<string>(width) + ").");
	}
}

/** Samples the waveform at the specified phase.
 * 
 * @param[in] phase The light curve phase at which an observation is 
 *	taken. Observations are assumed to be instantaneous, with no 
 *	averaging over rapid variability.
 * 
 * @return The flux emitted by the object at the specified phase.
 * 
 * @pre @p phase &isin; [0, 1)
 *
 * @post the return value is determined entirely by the phase and 
 *	the parameters passed to the constructor
 *
 * @post the return value is not NaN
 * @post the return value is greater than or equal to 1
 * @post the mode of the flux is one, when averaged over many times.
 * 
 * @exception std::logic_error Thrown if a bug was found in the flux 
 *	calculations.
 *
 * @exceptsafe Neither the object nor the argument are changed in the 
 *	event of an exception.
 */
double SquarePeak::fluxPhase(double phase, double amp) const {
	if (phase < width) {
		return 1.0 + amp;
	} else {
		return 1.0;
	}
}

}}		// end lcmc::models
