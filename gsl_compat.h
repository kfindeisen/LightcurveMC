/** Compatibility definitions to allow the program to interface 
 *	well with GSL
 * @file lightcurveMC/gsl_compat.h
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 26, 2013
 */

#ifndef LCMCGSLH
#define LCMCGSLH

#include <string>
#include <gsl/gsl_version.h>

//----------------------------------------------------------
// Feature flags 

// is_matrix_equal()
#if GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= 15
#define _GSL_HAS_MATRIX_EQUAL
#endif

// No released version of GSL has proper autocorrelation support... yet
//#if GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= ???
//#define _GSL_HAS_ACF
//#endif

/** Wrapper that throws @c std::runtime_error in response to a GSL error
 */
void gslCheck(int status, std::string msg);

#endif		// end LCMCGSLH
