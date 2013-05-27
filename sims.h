/** Drivers for major simulation steps
 * @file lightcurveMC/sims.h
 * @author Krzysztof Findeisen
 * @date Created May 26, 2013
 * @date Last modified May 26, 2013
 */

#ifndef LCMCSIMH
#define LCMCSIMH

#include <string>
#include <vector>
#include "lightcurvetypes.h"
#include "paramlist.h"

namespace lcmc {

using std::string;
using std::vector;

/** Randomly generates parameter values within the specified limits
 */
models::ParamList drawParams(const models::RangeList& limits);

/** Returns the time stamps corresponding to a particular input file.
 */
void makeTimes(const string& dateList, vector<double>& times);

/** Generates white noise corresponding to a given cadence.
 */
void makeWhiteNoise(const vector<double>& times, double sigma, vector<double>& noise);

/** Generates an observed light curve for an injection analysis.
 */
void makeInjectNoise(const string& catalog, 
		vector<double>& times, vector<double>& baseFlux);

/** Generates a random light curve, incorporating all the simulation settings.
 */
void simLightCurve(const models::LightCurveType& curve, const models::ParamList& params, 
		const vector<double>& times, const vector<double>& noise, 
		vector<double>& lcFluxes);

}	// end lcmc

#endif	// LCMCSIMH
