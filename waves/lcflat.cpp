/** Defines the FlatWave light curve class.
 * @file lightcurveMC/waves/lcflat.cpp
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified May 22, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
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
 * @return The flux of the object.
 *
 * @post flux(t) = 1.0 for all t.
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
