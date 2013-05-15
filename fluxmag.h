/** Functions for converting flux and magnitude representations of vectors
 * @file lightcurveMC/fluxmag.h
 * @author Krzysztof Findeisen
 * @date Created April 4, 2013
 * @date Last modified April 11, 2013
 */

#ifndef LCMCFLUXMAGH
#define LCMCFLUXMAGH

#include <vector>

namespace lcmc { namespace utils {

/** Function for converting fluxes to magnitudes
 */
double fluxToMag(double flux);

/** Function for converting magnitudes to fluxes
 */
double magToFlux(double mag);

/** Converts fluxes to magnitudes.
 */
void fluxToMag(const std::vector<double>& fluxes, std::vector<double>& mags);

/** Converts magnitudes to fluxes.
 */
void magToFlux(const std::vector<double>& mags, std::vector<double>& fluxes);

}}		// end lcmc::utils

#endif 		// end LCMCFLUXMAGH
