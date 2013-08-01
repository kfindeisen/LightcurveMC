/** Type definitions for all periodic light curves
 * @file lightcurveMC/waves/lightcurves_periodic.h
 * @author Krzysztof Findeisen
 * @date Created February 23, 2012
 * @date Last modified May 22, 2013
 */

#ifndef LCMCCURVEPERIH
#define LCMCCURVEPERIH

#include <string>
#include "lcdeterministic.h"

namespace lcmc { namespace models {

/** PeriodicLc is the base class for all periodic light curve models. 
 * Each PeriodicLc subclass represents a particular waveform.
 * 
 * PeriodicLc and its subclasses all have three common parameters: the 
 * amplitude of the light curve, the period, and the phase at time zero. 
 * Subclasses may define additional parameters.
 */
class PeriodicLc : public Deterministic {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit PeriodicLc(const std::vector<double> &times, 
			double amp, double period, double phase);
	virtual ~PeriodicLc();

private: 
	/** Samples the light curve at the specified time.
	 */
	double flux(double time) const;

	/** Samples the light curve at the specified phase. Subclasses of 
	 * PeriodicLc should override this method to represent 
	 * different waveforms.
	 * 
	 * @param[in] phase The light curve phase at which an observation is 
	 *	taken. Observations are assumed to be instantaneous, with no 
	 *	averaging over rapid variability.
	 * @param[in] amp The light curve amplitude.
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
	 * @post Either the mean, median, or mode of the flux is one, when 
	 *	averaged over many times. Subclasses of PeriodicLc may 
	 *	chose the option (mean, median, or mode) most appropriate 
	 *	for their light curve shape.
	 * 
	 * @exception std::logic_error Thrown if a bug was found in the flux 
	 *	calculations.
	 *
	 * @exceptsafe Neither the object nor the argument are changed in the 
	 *	event of an exception.
	 */
	virtual double fluxPhase(double phase, double amp) const = 0;
	
	double amp, period, phase0;
};

/** SineWave describes sinusoidal variables in flux space. The light curve can 
 * be described entirely by its amplitude, period, and phase offset.
 *
 * @invariant One is the mean flux returned by SineWave.
 */
class SineWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit SineWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the sinusoidal waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** TriangleWave describes pseudo-sinusoidal variables with sharper minima and 
 * maxima. The light curve can be described entirely by its amplitude, period, 
 * and phase offset.
 *
 * @invariant One is the mean flux returned by TriangleWave.
 */
class TriangleWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit TriangleWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** EllipseWave describes pseudo-sinusoidal variables with asymmetric minima 
 * and maxima. The light curve can be described entirely by its amplitude, 
 * period, and phase offset.
 *
 * @invariant One is the mean flux returned by EllipseWave.
 */
class EllipseWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit EllipseWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** EclipseWave describes variables with pairs of periodic dimmings. The 
 * dimming events are rectangular, and take 5% of the cycle each. The 
 * secondary dimming is 70% the depth of the primary dimming. The light curve 
 * can be described entirely by its (primary) amplitude, period, and 
 * phase offset.
 *
 * @invariant One is the maximum flux returned by EclipseWave.
 */
class EclipseWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit EclipseWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** BroadPeakWave describes periodic variables that smoothly rise to a maximum. 
 * The light curve can be described entirely by its amplitude, period, and 
 * phase offset.
 *
 * @invariant One is the minimum flux returned by BroadPeakWave.
 */
class BroadPeakWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit BroadPeakWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** SharpPeakWave describes periodic variables that smoothly rise to a maximum. 
 * The light curve can be described entirely by its amplitude, period, and 
 * phase offset.
 *
 * @invariant One is the minimum flux returned by SharpPeakWave.
 */
class SharpPeakWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit SharpPeakWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** MagSineWave describes sinusoidal variables in magnitude space. The light 
 * curve can be described entirely by its amplitude, period, and phase offset.
 *
 * @invariant One is the median flux returned by MagSineWave.
 */
class MagSineWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit MagSineWave(const std::vector<double> &times, 
			double amp, double period, double phase);

private:
	/** Samples the sinusoidal waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
};

/** AaTauWave describes variables with periodic dips in magnitude space. The light 
 * curve can be described entirely by its amplitude, period, phase offset, 
 * and dip width.
 *
 * @invariant One is the modal flux returned by AaTauWave.
 */
class AaTauWave : public PeriodicLc {
public: 
	/** Initializes the light curve to represent a periodic function 
	 * flux(time).
	 */
	explicit AaTauWave(const std::vector<double> &times, 
			double amp, double period, double phase, double width);

private:
	/** Samples the waveform at the specified phase.
	 */
	double fluxPhase(double phase, double amp) const;
	
	double width;
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVEPERIH
