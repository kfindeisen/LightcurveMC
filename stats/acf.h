/** Temporary replacements for a lag-n autocorrelation function
 * @file acf.h
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 7, 2013
 */

#ifndef LCMCACFH
#define LCMCACFH

namespace lcmc { namespace stats { 

/** This function computes the lag-n autocorrelation of the dataset data, 
 *	using the signal-processing convention.
 */
void autoCorrelation_sp(const double data[], double acfs[], size_t n);

/** This function computes the lag-n autocorrelation of the dataset data, 
 * 	using the statistical analysis convention.
 */
void autoCorrelation_stat(const double data[], double acfs[], size_t n);

}}		// end lcmc::stats

#endif		// LCMCACFH