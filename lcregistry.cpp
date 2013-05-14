/** Definitions for lightcurveMC type manipulation functions
 * @file lcregistry.cpp
 * @author Krzysztof Findeisen
 * @date Created April 25, 2012
 * @date Last modified May 10, 2013
 * 
 * The functions defined here handle the details of the ILightCurve subclasses.
 */

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "lightcurvetypes.h"
#include "paramlist.h"

using std::map;
using std::pair;
using std::string;

// Declare constructors for every ILightCurve subtype supported by lcFactory()
#include "waves/lightcurves_outbursts.h"
#include "waves/lightcurves_fades.h"
#include "waves/lightcurves_periodic.h"
#include "waves/lightcurves_gp.h"
#include "waves/null.h"

namespace lcmc { namespace models {

/* LightCurveTypes specify which LightCurve should be created for a 
 * 	particular simulation.
 *
 * @bug Replace LightCurveTypes with stringly typed code -- actually safer than this mess!
 */
const LightCurveType      FLATWAVE() {return LightCurveType( 0);}
const LightCurveType      SINEWAVE() {return LightCurveType( 1);}
const LightCurveType  TRIANGLEWAVE() {return LightCurveType( 2);}
const LightCurveType   ELLIPSEWAVE() {return LightCurveType( 3);}
const LightCurveType BROADPEAKWAVE() {return LightCurveType( 4);}
const LightCurveType SHARPPEAKWAVE() {return LightCurveType( 5);}
const LightCurveType   ECLIPSEWAVE() {return LightCurveType( 6);}
const LightCurveType   MAGSINEWAVE() {return LightCurveType( 7);}
	
const LightCurveType      SLOWPEAK() {return LightCurveType(10);}
const LightCurveType     FLAREPEAK() {return LightCurveType(11);}
const LightCurveType    SQUAREPEAK() {return LightCurveType(12);}
const LightCurveType       SLOWDIP() {return LightCurveType(13);}
const LightCurveType      FLAREDIP() {return LightCurveType(14);}
const LightCurveType     SQUAREDIP() {return LightCurveType(15);}

const LightCurveType    WHITENOISE() {return LightCurveType(20);}
const LightCurveType    RANDOMWALK() {return LightCurveType(21);}
const LightCurveType  DAMPRANDWALK() {return LightCurveType(22);}
const LightCurveType         ONEGP() {return LightCurveType(23);}
const LightCurveType         TWOGP() {return LightCurveType(24);}

// Helper types for streamlining the implementations
typedef  map<string, const LightCurveType> LightCurveRegistry;
typedef pair<string, const LightCurveType> LightCurveEntry;

/** Implements a global registry of light curves. This registry is used to 
 * implement the functions lightCurveTypes() and parseLightCurve(). While 
 * constructing a map is somewhat less efficient than hardcoding the light 
 * curve information into the two functions, it is much easier to update 
 * because all the information for each light curve is declared in one place.
 *
 * @return The registry.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to create 
 *	the registry.
 *
 * @exceptsafe The program state is unchanged in the event of an exception.
 */
const LightCurveRegistry & getLightCurveRegistry() {
	// invariant: registry contains the names of all valid light curves, 
	//	or called == false
	static LightCurveRegistry registry;
	static bool called = false;
	if (called == false) {
		registry.clear();

		// Original waveforms
		registry.insert(LightCurveEntry(       "flat",      FLATWAVE() ));
		registry.insert(LightCurveEntry(       "sine",      SINEWAVE() ));
		registry.insert(LightCurveEntry(   "triangle",  TRIANGLEWAVE() ));
		registry.insert(LightCurveEntry(    "ellipse",   ELLIPSEWAVE() ));
		registry.insert(LightCurveEntry( "broad_peak", BROADPEAKWAVE() ));
		registry.insert(LightCurveEntry( "sharp_peak", SHARPPEAKWAVE() ));
		registry.insert(LightCurveEntry(    "eclipse",   ECLIPSEWAVE() ));
		registry.insert(LightCurveEntry(    "magsine",   MAGSINEWAVE() ));
		
		// Outburst waveforms
		registry.insert(LightCurveEntry(  "slow_peak",      SLOWPEAK() ));
		registry.insert(LightCurveEntry( "flare_peak",     FLAREPEAK() ));
		registry.insert(LightCurveEntry(  "flat_peak",    SQUAREPEAK() ));
		registry.insert(LightCurveEntry(   "slow_dip",       SLOWDIP() ));
		registry.insert(LightCurveEntry(  "flare_dip",      FLAREDIP() ));
		registry.insert(LightCurveEntry(   "flat_dip",     SQUAREDIP() ));

		// Gaussian process waveforms
		registry.insert(LightCurveEntry("white_noise",    WHITENOISE() ));
		registry.insert(LightCurveEntry(       "walk",    RANDOMWALK() ));
		registry.insert(LightCurveEntry(        "drw",  DAMPRANDWALK() ));
		registry.insert(LightCurveEntry(  "simple_gp",         ONEGP() ));
		registry.insert(LightCurveEntry(     "two_gp",         TWOGP() ));

		// No exceptions past this point
		// To preserve the invariant in the face of exceptions, 
		//	only set called flag when we know the registry 
		//	is ready
		called = true;
	}

	// assert: registry is fully defined
	
	return registry;
}

/** lcFactory is a factory method that allocates and initializes a ILightCurve 
 *	object given its specification.
 */
std::auto_ptr<ILightCurve> lcFactory(LightCurveType whichLc, const std::vector<double> &times, const ParamList &lcParams) {
	using std::auto_ptr;

	/** @todo Look into using argument forwarding to make calls like 
	 *	lcFactory(Sine, amp, per, phase)
	 */

	// once the LightCurve is constructed, constructing auto_ptr 
	//	does not throw an exception.
	// Therefore, no memory is leaked if an exception is thrown

	// Basic periodic light curves	
	if (whichLc == FLATWAVE()) {
		return auto_ptr<ILightCurve>(new FlatWave     (times));
	} else if (whichLc == SINEWAVE()) {
		return auto_ptr<ILightCurve>(new SineWave     (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));
	} else if (whichLc == TRIANGLEWAVE()) {
		return auto_ptr<ILightCurve>(new TriangleWave (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));
	} else if (whichLc == ELLIPSEWAVE()) {
		return auto_ptr<ILightCurve>(new EllipseWave  (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));
	} else if (whichLc == BROADPEAKWAVE()) {
		return auto_ptr<ILightCurve>(new BroadPeakWave(times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));
	} else if (whichLc == SHARPPEAKWAVE()) {
		return auto_ptr<ILightCurve>(new SharpPeakWave(times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));
	} else if (whichLc == ECLIPSEWAVE()) {
		return auto_ptr<ILightCurve>(new EclipseWave  (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));
	} else if (whichLc == MAGSINEWAVE()) {
		return auto_ptr<ILightCurve>(new MagSineWave  (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph")));

	// Bursty light curves	
	} else if (whichLc ==   SLOWPEAK()) {
		return auto_ptr<ILightCurve>(new SlowPeak     (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph"), lcParams.get("width")));
	} else if (whichLc ==  FLAREPEAK()) {
		return auto_ptr<ILightCurve>(new FlarePeak    (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph"), lcParams.get("width2"), lcParams.get("width")));
	} else if (whichLc == SQUAREPEAK()) {
		return auto_ptr<ILightCurve>(new SquarePeak   (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph"), lcParams.get("width")));

	// Fadey light curves	
	} else if (whichLc ==   SLOWDIP()) {
		return auto_ptr<ILightCurve>(new SlowDip     (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph"), lcParams.get("width")));
	} else if (whichLc ==  FLAREDIP()) {
		return auto_ptr<ILightCurve>(new FlareDip    (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph"), lcParams.get("width2"), lcParams.get("width")));
	} else if (whichLc == SQUAREDIP()) {
		return auto_ptr<ILightCurve>(new SquareDip   (times, lcParams.get("a"), lcParams.get("p"), lcParams.get("ph"), lcParams.get("width")));

	// Gaussian process light curves	
	} else if (whichLc ==   WHITENOISE()) {
		return auto_ptr<ILightCurve>(new WhiteNoise  (times, lcParams.get("a")));
	} else if (whichLc == RANDOMWALK()) {
		return auto_ptr<ILightCurve>(new RandomWalk(times, lcParams.get("d")));
	} else if (whichLc == DAMPRANDWALK()) {
		return auto_ptr<ILightCurve>(new DampedRandomWalk(times, lcParams.get("d"), lcParams.get("p")));
	} else if (whichLc == ONEGP()) {
		return auto_ptr<ILightCurve>(new SimpleGp(times, lcParams.get("a"), lcParams.get("p")));
	} else if (whichLc == TWOGP()) {
		return auto_ptr<ILightCurve>(new TwoScaleGp(times, lcParams.get("a"), lcParams.get("p"), lcParams.get("a2"), lcParams.get("p2") ));

	} else {
		throw std::invalid_argument("Unsupported light curve.");
	}
}

}}		// end lcmc::models
