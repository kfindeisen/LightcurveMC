/** Type definitions for periodically fading light curves
 * @file lightcurveMC/waves/lightcurves_fades.h
 * @author Krzysztof Findeisen
 * @date Created August 21, 2012
 * @date Last modified March 20, 2013
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

#ifndef LCMCCURVEPOOFH
#define LCMCCURVEPOOFH

#include <stdexcept>
#include "lightcurves_periodic.h"

namespace lcmc { namespace models {

/** SlowDip describes periodic variables that have a Gaussian fade once per 
 * cycle. The light curve can be described by its amplitude, period, phase 
 * offset, and dip width.
 *
 * @invariant Zero is the maximum flux returned by SlowDip in the limit of 
 *	short fades. Long fades may return lower maxima.
 */
class SlowDip : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodically 
	 * fading function flux(time).
	 */
	explicit SlowDip(const std::vector<double> &times, 
			double amp, double period, double phase, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double width;
};

/** FlareDip describes periodic variables that have a stellar flare shaped 
 * dip (a fast linear fade, followed by an exponential return) once per 
 * cycle. The light curve can be described by its amplitude, period, phase 
 * offset, fade time, and rise time.
 *
 * @invariant Zero is the maximum flux returned by FlareDip in the limit of 
 *	short fades. Long fades may return lower maxima.
 */
class FlareDip : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodically 
	 * fading function flux(time).
	 */
	explicit FlareDip(const std::vector<double> &times, 
			double amp, double period, double phase, double fade, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double tExp, tLin;
};

/** SquareDip describes periodic variables that have a flat-bottomed dip once 
 * per cycle. The light curve can be described by its amplitude, period, phase 
 * offset, and dip width.
 *
 * @invariant Zero is the maximum flux returned by SquareDip.
 */
class SquareDip : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodically 
	 * fading function flux(time).
	 */
	explicit SquareDip(const std::vector<double> &times, 
			double amp, double period, double phase, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double width;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVEPERIH
