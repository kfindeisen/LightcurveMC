/** Functions for handling data containing NaN values
 * @file nan.h
 * @author Krzysztof Findeisen
 * @date Created April 11, 2013
 * @date Last modified April 12, 2013
 */

#ifndef LCMCNANH
#define LCMCNANH

#include <vector>

namespace lcmcutils {

using namespace std;

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
double meanNoNan(const std::vector<double>& vals);

/** Calculates the variance, ignoring NaNs
 */
double varianceNoNan(const std::vector<double>& vals);

};	// end lcmcutils

#endif	// ifndef LCMCNANH
