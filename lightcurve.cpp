/** Defines the LightCurve base class.
 * @file lightcurve.cpp
 * @author Krzysztof Findeisen
 * @date Created April 23, 2012
 * @date Last modified April 20, 2013
 */

#include "lightcurvetypes.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a particular data set. Since 
 * ILightCurve is an interface class, its constructor does nothing.
 */
ILightCurve::ILightCurve() {
}

ILightCurve::~ILightCurve() {
}

}}		// end lcmc::models
