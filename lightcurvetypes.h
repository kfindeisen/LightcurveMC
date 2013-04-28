/** Type definitions for lightcurveMC light curves
 * @file lightcurvetypes.h
 * @author Krzysztof Findeisen
 * @date Created February 9, 2012
 * @date Last modified April 27, 2013
 * 
 * These types represent the interface lightcurveMC uses to handle light 
 * curves in an abstract fashion. Do not add anything to this header unless 
 * (nearly) the entire program needs it.
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

/** LightCurveType specifies which LightCurve should be created for a 
 * 	particular simulation. Each allowed value corresponds to exactly one 
 *	light curve model.
 *
 * @note Since forward declaring enums requires a compiler that supports 
 * C++11, and I don't want to assume that yet, define LightCurveType as a 
 * class supporting no operation other than equality. 
 * Actual LightCurveType constants are only visible to those functions that 
 *	need them.
 */
//enum LightCurveType;
class LightCurveType {
public: 
	/** Initializes the object to a particular enumerated constant. Client 
	 *	is responsible for ensuring that constant values are not 
	 *	duplicated.
	 *
	 * @param[in] id The unique constant identifying a particular value of 
	 *	the LightCurveType enumeration.
	 */
	explicit LightCurveType(unsigned int id) : id(id) {
	}

	/** Tests whether two LightCurveTypes represent the same %LightCurve.
	 *
	 * @param[in] other The LightCurveType to compare this to.
	 *
	 * @return True if and only if the two LightCurveTypes correspond to 
	 *	the same constant.
	 */
	bool operator== (const LightCurveType& other) const {
		return (this->id == other.id);
	}

	/** Tests whether two LightCurveTypes represent different LightCurves.
	 *
	 * @param[in] other The LightCurveType to compare this to.
	 *
	 * @return True if and only if the two LightCurveTypes correspond to 
	 *	different constants.
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
 * light curve. No application using the LightCurves should need more than 
 * the interface provided by ILightCurve.
 * 
 * A LightCurve object represents a simulated set of observations, with fluxes 
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
	 * @post timeArray.size() == getFluxes().size()
	 * 
	 * @post No element of timeArray is NaN
	 */
	virtual void getTimes(std::vector<double>& timeArray) const = 0;

	/** Returns the simulated fluxes at the corresponding times
	 *
	 * @param[out] fluxArray A vector containing the desired fluxes.
	 *
	 * @post fluxArray.size() == getTimes().size()
	 * @post if getTimes()[i] == getTimes()[j] for i &ne; j, then 
	 *	getFluxes()[i] == getFluxes()[j]
	 * 
	 * @post No element of fluxArray is NaN
	 * @post All elements in fluxArray are non-negative
	 * @post Either the mean, median, or mode of the flux is one, when 
	 *	averaged over many elements and many light curve instances. 
	 *	Subclasses of ILightCurve may chose the option 
	 *	(mean, median, or mode) most appropriate for their light 
	 *	curve shape.
	 */
	virtual void getFluxes(std::vector<double>& fluxArray) const = 0;

protected: 
	/** Initializes the light curve to represent a particular data set.
	 */
	explicit ILightCurve();
};

}}		// end lcmc::models

#endif		// end ifndef LCMCCURVESH
