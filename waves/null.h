/** Null hypotheses for light curve analysis
 * @file lightcurves_periodic.h
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified March 19, 2013
 */

#ifndef LCMCCURVENULLH
#define LCMCCURVENULLH

#include "lcsubtypes.h"

namespace lcmc { namespace models {

/** FlatWave represents light curves that show no variability aside from 
 * measurement error. It is provided as a convenient control case for 
 * comparisons to light curves with nonzero amplitudes.
 *
 * @invariant The flux is always zero.
 */
class FlatWave : public Deterministic {
public: 
	/** Initializes the light curve to represent a flat waveform.
	 */
	explicit FlatWave(const std::vector<double> &times);

private: 
	/** Returns a flat signal
	 */
	double flux(double time) const;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVEPERIH
