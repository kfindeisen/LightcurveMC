/** Type definitions for Lightcurve MC light curves
 * @file lightcurveMC/lightcurvetypes.h
 * @author Krzysztof Findeisen
 * @date Created February 9, 2012
 * @date Last modified May 22, 2013
 * 
 * These types represent the interface Lightcurve MC uses to handle light 
 * curves in an abstract fashion. Do not add anything to this header unless 
 * (nearly) the entire program needs it.
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

#ifndef LCMCCURVESH
#define LCMCCURVESH

#include <string>
#include <vector>

namespace lcmc { 

/** This namespace identifies data types and functions that handle the 
 *	simulation of astronomical data.
 */
namespace models {

/** LightCurveType specifies which @ref ILightCurve "LightCurve" should be created for a 
 * 	particular simulation. Each allowed value corresponds to exactly one 
 *	light curve model.
 *
 * @note Since forward declaring enums requires a compiler that supports 
 * C++11, and I don't want to assume that yet, define LightCurveType as a 
 * class supporting no operation other than equality. 
 * Actual LightCurveType constants are only visible to those functions that 
 *	need them.
 */
class LightCurveType {
public: 
	/** Initializes the object to a particular enumerated constant. Client 
	 *	is responsible for ensuring that constant values are not 
	 *	duplicated.
	 *
	 * @param[in] id The unique constant identifying a particular value of 
	 *	the LightCurveType enumeration.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	explicit LightCurveType(unsigned int id) : id(id) {
	}

	/** Tests whether two @ref LightCurveType "LightCurveTypes" represent the same light curve.
	 *
	 * @param[in] other The LightCurveType to compare this to.
	 *
	 * @return True if and only if the two @ref LightCurveType "LightCurveTypes" correspond to 
	 *	the same light curve.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	bool operator== (const LightCurveType& other) const {
		return (this->id == other.id);
	}

	/** Tests whether two @ref LightCurveType "LightCurveTypes" represent different LightCurves.
	 *
	 * @param[in] other The LightCurveType to compare this to.
	 *
	 * @return True if and only if the two @ref LightCurveType "LightCurveTypes" correspond to 
	 *	different light curves.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	bool operator!= (const LightCurveType& other) const {
		return (this->id != other.id);
	}
private: 
	unsigned int id;
};

/** ILightCurve is the interface class for all light curve models. Each class 
 * implementing ILightCurve represents a particular kind of light curve, and 
 * each ILightCurve object represents a single realization of that type of 
 * light curve. No application using the light curves should need more than 
 * the interface provided by ILightCurve.
 * 
 * An ILightCurve object represents a simulated set of observations, with fluxes 
 * determined by the model parameters and times determined by the desired 
 * cadence of the mock data set.
 * 
 * @invariant ILightCurve is immutable.
 */
class ILightCurve {
public: 
	virtual ~ILightCurve() = 0;

	/** Returns the times at which the simulated data were taken
	 *
	 * @param[out] timeArray A vector containing the desired times.
	 *
	 * @post Any data previously in @p timeArray is erased
	 * @post @p timeArray.size() = size()
	 * @post @p timeArray contains the times with which the light curve was initialized
	 * 
	 * @post No element of @p timeArray is NaN
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to 
	 *	return a copy of the times.
	 *
	 * @exceptsafe Neither the object nor the argument are changed in the 
	 *	event of an exception.
	 */
	virtual void getTimes(std::vector<double>& timeArray) const = 0;

	/** Returns the simulated fluxes at the corresponding times
	 *
	 * @param[out] fluxArray A vector containing the desired fluxes.
	 *
	 * @post Any data previously in @p fluxArray is erased
	 * @post @p fluxArray.size() = size()
	 * @post if getTimes()[i] = getTimes()[j] for i &ne; j, then 
	 *	getFluxes()[i] = getFluxes()[j]
	 * 
	 * @post No element of @p fluxArray is NaN
	 * @post All elements in @p fluxArray are non-negative
	 * @post Either the mean, median, or mode of the flux is one, when 
	 *	averaged over many elements and many light curve instances. 
	 *	Subclasses of ILightCurve may chose the option 
	 *	(mean, median, or mode) most appropriate for their light 
	 *	curve shape.
	 *
	 * @exception std::bad_alloc Thrown if there is not enough memory to compute 
	 *	the light curve.
	 * @exception std::logic_error Thrown if a bug was found in the flux calculations.
	 *
	 * @exceptsafe Neither the object nor the argument are changed in the 
	 *	event of an exception.
	 */
	virtual void getFluxes(std::vector<double>& fluxArray) const = 0;


	/** Returns the number of times and fluxes
	 *
	 * @return The number of data points represented by the ILightCurve.
	 *
	 * @post return value = getTimes().%size() = getFluxes().%size()
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual size_t size() const = 0;
	
protected: 
	/** Initializes the light curve to represent a particular data set.
	 */
	explicit ILightCurve();
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVESH
