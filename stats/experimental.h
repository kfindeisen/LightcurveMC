/** Functions for trying out new features
 * @file experimental.h
 * @author Krzysztof Findeisen
 * @date Created August 3, 2011
 * @date Last modified April 12, 2013
 */

#ifndef LCMCEXPERIMENTH
#define LCMCEXPERIMENTH

#include <vector>

namespace lcmc { namespace stats {

typedef std::vector<double> DoubleVec;

/** Calculates the RMS binned over ever-larger subintervals of the data
 */
void rmsVsTRooted(const DoubleVec &times, const DoubleVec &fluxes, 
	DoubleVec &timeSteps, DoubleVec & rmsValues);
void rmsVsTAllPairs(const DoubleVec &times, const DoubleVec &fluxes, 
	DoubleVec &timeSteps, DoubleVec & rmsValues);

}}		// end lcmc::stats

#endif		// end LCMCEXPERIMENTH
