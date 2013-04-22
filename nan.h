/** Functions for handling data containing NaN values
 * @file nan.h
 * @author Krzysztof Findeisen
 * @date Created April 11, 2013
 * @date Last modified April 21, 2013
 */

#ifndef LCMCNANH
#define LCMCNANH

#include <vector>

namespace lcmc { 

/** This namespace contains miscellaneous functions for handling generic 
 *	problems.
 */
namespace utils {

using std::vector;

/** isNan tests whether a floating-point number is undefined
 */
bool isNan(double x);

/** isNanOrInf tests whether a floating-point number is non-finite
 */
bool isNanOrInf(double x);

/** lessFinite allows floating-point numbers to be compared safely in the 
 *	presence of NaNs.
 */
bool lessFinite(double x, double y);

/** Removes nans from a pair of vectors.
 */
void removeNans(const vector<double>& badVals, vector<double>& goodVals, 
		const vector<double>& sideVals, vector<double>& matchVals);

/** Calculates the mean, ignoring NaNs
 */
double meanNoNan(const vector<double>& vals);

/** Calculates the variance, ignoring NaNs
 */
double varianceNoNan(const vector<double>& vals);

}}	// end lcmc::utils

#endif	// ifndef LCMCNANH
