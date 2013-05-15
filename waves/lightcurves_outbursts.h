/** Type definitions for periodically outbursting light curves
 * @file lightcurveMC/waves/lightcurves_outbursts.h
 * @author Krzysztof Findeisen
 * @date Created May 2, 2012
 * @date Last modified March 20, 2013
 */

#ifndef LCMCCURVEBOOMH
#define LCMCCURVEBOOMH

#include <stdexcept>
#include "lightcurves_periodic.h"

namespace lcmc { namespace models {

/** SlowPeak describes periodic variables that have a Gaussian peak once per 
 * cycle. The light curve can be described by its amplitude, period, phase 
 * offset, and peak width.
 *
 * @invariant Zero is the minimum flux returned by SlowPeak in the limit of 
 *	short flares.
 */
class SlowPeak : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodically 
	 * outbursting function flux(time).
	 */
	explicit SlowPeak(const std::vector<double> &times, 
			double amp, double period, double phase, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double width;
};

/** FlarePeak describes periodic variables that have a stellar flare shaped 
 * peak (a fast linear rise, followed by an exponential decay) once per 
 * cycle. The light curve can be described by its amplitude, period, phase 
 * offset, rise time, and fade time.
 *
 * @invariant Zero is the minimum flux returned by FlarePeak in the limit of 
 *	short flares.
 */
class FlarePeak : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodically 
	 * outbursting function flux(time).
	 */
	explicit FlarePeak(const std::vector<double> &times, 
			double amp, double period, double phase, double rise, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double tExp, tLin;
};

/** SquarePeak describes periodic variables that have a flat-topped peak once 
 * per cycle. The light curve can be described by its amplitude, period, phase 
 * offset, and peak width.
 *
 * @invariant Zero is the minimum flux returned by SquarePeak.
 */
class SquarePeak : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodically 
	 * outbursting function flux(time).
	 */
	explicit SquarePeak(const std::vector<double> &times, 
			double amp, double period, double phase, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double width;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVEPERIH
