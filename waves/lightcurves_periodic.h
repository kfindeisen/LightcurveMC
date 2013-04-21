/** Type definitions for all periodic light curves
 * @file lightcurves_periodic.h
 * @author Krzysztof Findeisen
 * @date Created February 23, 2012
 * @date Last modified April 3, 2013
 */

#ifndef LCMCCURVEPERIH
#define LCMCCURVEPERIH

#include <string>
#include "lcsubtypes.h"

namespace lcmcmodels {

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
	 * 
	 * @pre phase &isin; [0, 1)
	 *
	 * @post fluxPhase(phi) is a deterministic function of phi.
	 * 
	 * @return The amplitude-normalized flux emitted by the object at the 
	 *	specified phase.
	 */
	virtual double fluxPhase(double phase) const = 0;
	
	double amp, period, phase0;
};

/** SineWave describes sinusoidal variables in flux space. The light curve can 
 * be described entirely by its amplitude, period, and phase offset.
 *
 * @invariant Zero is the mean flux returned by SineWave.
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
	double fluxPhase(double phase) const;
};

/** TriangleWave describes pseudo-sinusoidal variables with sharper minima and 
 * maxima. The light curve can be described entirely by its amplitude, period, 
 * and phase offset.
 *
 * @invariant Zero is the mean flux returned by TriangleWave.
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
	double fluxPhase(double phase) const;
};

/** EllipseWave describes pseudo-sinusoidal variables with asymmetric minima 
 * and maxima. The light curve can be described entirely by its amplitude, 
 * period, and phase offset.
 *
 * @invariant Zero is the mean flux returned by EllipseWave.
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
	double fluxPhase(double phase) const;
};

/** EclipseWave describes variables with pairs of periodic dimmings. The 
 * dimming events are rectangular, take 5% of the cycle each, and one is 70% 
 * the depth of the other. The light curve can be described entirely by its 
 * amplitude, period, and phase offset.
 *
 * @invariant Zero is the maximum flux returned by EclipseWave. Note this 
 *	behavior differs from that of many other subclasses of PeriodicLc.
 * @todo Find a spec that doesn't violate least surprise?
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
	double fluxPhase(double phase) const;
};

/** BroadPeakWave describes periodic variables that smoothly rise to a maximum. 
 * The light curve can be described entirely by its amplitude, period, and 
 * phase offset.
 *
 * @invariant Zero is the minimum flux returned by BroadPeakWave.
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
	double fluxPhase(double phase) const;
};

/** SharpPeakWave describes periodic variables that smoothly rise to a maximum. 
 * The light curve can be described entirely by its amplitude, period, and 
 * phase offset.
 *
 * @invariant Zero is the minimum flux returned by SharpPeakWave.
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
	double fluxPhase(double phase) const;
};

/** MagSineWave describes sinusoidal variables in magnitude space. The light 
 * curve can be described entirely by its amplitude, period, and phase offset.
 *
 * @invariant Zero is the minimum flux returned by SineWave.
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
	double fluxPhase(double phase) const;
};

}		// end lcmcmodels

#endif		// end ifndef LCMCCURVEPERIH
