/** Functions for converting flux and magnitude representations of vectors
 * @file fluxmag.cpp
 * @author Krzysztof Findeisen
 * @date Created April 4, 2013
 * @date Last modified April 21, 2013
 */

#include <algorithm>
#include <vector>
#include <cmath>
#include "fluxmag.h"

namespace lcmc { namespace utils {

using std::vector;

/** Function for converting fluxes to magnitudes
 * 
 * @param[in] flux The flux to convert.
 *
 * @return The corresponding magnitude, assuming a flux of 1 unit corresponds 
 *	to a magnitude of 0. Returns NaN if the flux is negative.
 *
 * @todo Desired behavior for negative fluxes?
 */
double fluxToMag(double flux) {
	return -2.5*log10(flux);
}

/** Function for converting magnitudes to fluxes
 *
 * @param[in] mag The magnitude to convert.
 *
 * @return The corresponding flux, assuming a flux of 1 unit at 0th magnitude.
 */
double magToFlux(double mag) {
	return pow(10.0, -0.4*mag);
}

/** Converts fluxes to magnitudes.
 * 
 * @param[in] fluxes A vector of fluxes to convert.
 * @param[out] mags The corresponding magnitudes, assuming a flux of 1 unit 
 *	corresponds to a magnitude of 0. Returns NaN wherever flux is negative.
 *
 * @post mags.size() == fluxes.size()
 */
void fluxToMag(const vector<double>& fluxes, vector<double>& mags) {
	static double (* fp)(double) = fluxToMag;

	vector<double> temp(fluxes.size());
	std::transform(fluxes.begin(), fluxes.end(), temp.begin(), fp);
	mags = temp;
}

/** Converts magnitudes to fluxes.
 *
 * @param[in] mags A vector of magnitudes to convert.
 * @param[out] fluxes The corresponding fluxes, assuming a flux of 1 unit at 
 *	0th magnitude.
 * 
 * @post fluxes.size() == mags.size()
 */
void magToFlux(const vector<double>& mags, vector<double>& fluxes) {
	static double (* fp)(double) = magToFlux;

	vector<double> temp(mags.size());
	std::transform(mags.begin(), mags.end(), temp.begin(), fp);
	fluxes = temp;
}

}}	// end lcmc::utils
