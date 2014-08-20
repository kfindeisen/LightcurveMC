/** Defines the LightCurve base class.
 * @file lightcurveMC/lightcurve.cpp
 * @author Krzysztof Findeisen
 * @date Created April 23, 2012
 * @date Last modified May 11, 2013
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

#include "lightcurvetypes.h"

namespace lcmc { namespace models {

/** Initializes the light curve to represent a particular data set. Since 
 * ILightCurve is an interface class, its constructor does nothing.
 *
 * @exceptsafe Does not throw exceptions.
 */
ILightCurve::ILightCurve() {
}

ILightCurve::~ILightCurve() {
}

}}		// end lcmc::models
