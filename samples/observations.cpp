/** Implementation of Observations
 * @file observations.cpp
 * @author Krzysztof Findeisen
 * @date Created May 4, 2012
 * @date Last modified April 4, 2013
 */

#include <string>
#include <vector>
#include <cstdio>
#include "../fluxmag.h"
#include "observations.h"
#include "../../lightCurveSpecs/lcsio.h"
#include "../../featurizer original/utils.tmp.h"

namespace lcmcinject {

/** Default constructor prevents Observations from being created 
 * directly.
 */
Observations::Observations() : times(), fluxes() {
}

/** Initializes an object to the value of a particular light curve.
 */
void Observations::init(const std::string& fileName) {
	FILE* hLightCurve = fopen(fileName.c_str(), "r");
	if (hLightCurve == NULL) {
		char buf[80];
		sprintf(buf, "Could not open file %s.", fileName.c_str());
		throw FileIo(buf);
	}
	
	// Use the existing interface
	double offset;
	std::vector<double> dummyErrors;
	try {
		readWgLightCurve(hLightCurve, 0.1, offset, this->times, 
			this->fluxes, dummyErrors);
	} catch (std::runtime_error e) {
		// Convert to proper exception type
		// readWgLightCurve doesn't know the filename, so add that in
		char buf[80];
		sprintf(buf, "While reading %s: %s", fileName.c_str(), e.what());
		throw FileIo(buf);
	}
	
	fclose(hLightCurve);

	// Add the offset back in -- it was a crappy idea anyway
	for(std::vector<double>::iterator it = times.begin(); it != times.end(); it++) {
		*it += offset;
	}
	
	// Convert from magnitudes to fluxes
	lcmcutils::magToFlux(fluxes, fluxes);

	// Normalize to a median flux of 1
	// assert: all fluxes strictly positive because transformed from magnitudes
	double median = quantile(this->fluxes.begin(), this->fluxes.end(), 0.5);
	for(std::vector<double>::iterator it = this->fluxes.begin(); 
			it != this->fluxes.end(); it++) {
		(*it) /= median;
	}
}

/** Returns the timestamps associated with this simulation.
 *
 * @param[in] timeArray the array that will contain the timestamps
 * 
 * @post timeArray does not contain NaNs
 */
void Observations::getTimes(std::vector<double> &timeArray) const {
	timeArray = this->times;
}

/** Returns the flux measurements associated with this simulation.
 *
 * @param[in] fluxArray the array that will contain the measurements
 * 
 * @post fluxArray does not contain NaNs
 */
void Observations::getFluxes(std::vector<double> &fluxArray) const {
	fluxArray = this->fluxes;
}

/** Basic constructor.
 */
FileIo::FileIo(const std::string& what_arg) : std::runtime_error(what_arg) {
}

}		// end lcmcinject
