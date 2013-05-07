/** Compatibility definitions to allow the program to interface 
 *	consistently with GSL
 * @file gsl_compat.h
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 7, 2013
 */

#ifndef LCMCGSLH
#define LCMCGSLH

#include <gsl/gsl_version.h>

#if GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= 15
#define _GSL_HAS_MATRIX_EQUAL
#endif

// No released version of GSL has proper autocorrelation support... yet
//#if GSL_MAJOR_VERSION >= 1 && GSL_MINOR_VERSION >= ???
//#define _GSL_HAS_ACF
//#endif

namespace lcmc { 



}		// end lcmc

#endif		// end LCMCGSLH
