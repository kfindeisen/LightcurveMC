/** Defines the FlatWave light curve class.
 * @file lcflat.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified May 11, 2013
 */

#include "null.h"

// The version of G++ being used determines how much finesse I can use to 
//	disable warnings
#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#define GNUC_FINEWARN
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#define GNUC_COARSEWARN
#endif
// Will need a separate set of flags if we're using a different compiler


// By definition, FlatWave::flux() ignores the time given to it
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace lcmc { namespace models {

/** Initializes the light curve to represent a flat waveform.
 *
 * @param[in] times The times at which the light curve will be sampled.
 *
 * @exceptsafe Does not throw exceptions.
 */
FlatWave::FlatWave(const std::vector<double> &times) : Deterministic(times) {
}

// By definition, FlatWave::flux() ignores the time given to it
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

/** Returns a flat signal
 * 
 * @param[in] time The time at which an observation is taken. 
 *	Observations are assumed to be instantaneous, with no averaging over 
 *	rapid variability.
 *
 * @post flux(t) == 1.0 for all t.
 * 
 * @return The flux of the object.
 *
 * @exceptsafe Does not throw exceptions.
 */
double FlatWave::flux(double time) const {
	return 1.0;
}

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

}}		// end lcmc::models
