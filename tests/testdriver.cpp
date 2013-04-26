/** Unit test code for lightcurveMC
 * @file testdriver.cpp
 * @author Krzysztof Findeisen
 * @date Created April 17, 2013
 * @date Last modified April 26, 2013
 */

#include "../warnflags.h"

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// On KPF4-Hewlett, I can use dynamic linking
// I can't do so on hazard because I can't register shared libraries
#ifdef __CYGWIN__
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE LcmcTesting
#include <boost/test/unit_test.hpp>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

// All other code in the individual test libraries
