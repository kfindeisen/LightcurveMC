/** Null hypotheses for light curve analysis
 * @file lightcurveMC/waves/lightcurves_periodic.h
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified March 19, 2013
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

#ifndef LCMCCURVENULLH
#define LCMCCURVENULLH

#include "lcdeterministic.h"

namespace lcmc { namespace models {

/** FlatWave represents light curves that show no variability aside from 
 * measurement error. It is provided as a convenient control case for 
 * comparisons to light curves with nonzero amplitudes.
 *
 * @invariant The flux is always one.
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
