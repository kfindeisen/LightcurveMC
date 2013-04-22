/** Header for defining overall program properties
 * @file header.h
 * @author Krzysztof Findeisen
 * @date Created April 19, 2013
 * @date Last modified April 21, 2013
 *
 * @todo Add proper exception classes. Put exceptions in exception namespace 
 *	within lcmc::models and lcmc::inject.
 * @todo Add exception guarantees to every function and method.
 */

#ifndef LCMCHEADERH
#define LCMCHEADERH

/** Brief description of the program, to make blurb quoted by program 
 *	consistent with documentation.
 */
#define PROG_SUMMARY "LightcurveMC carries out Monte Carlo simulations of a statistical light curve analysis given a particular observing cadence. It takes as its primary argument a list of time stamps, and returns a table of light curve statistics and their variance from run to run. Additional files may contain the statistics themselves."

/** Current version of the program, to make version quoted by program 
 *	consistent with documentation.
 */
#define VERSION_STRING "2.1.0-devel"

/** @mainpage
 *
 * @section intro Introduction
 *
 * Lightcurve MC carries out Monte Carlo simulations of statistical light curve 
 * analysis, given a fixed observing pattern. The program is designed to be 
 * highly flexible with respect to the models adopted, as these can be 
 * selected at run time. It is also somewhat modular with respect to the 
 * statistical tests run on each light curve, although at present these need 
 * to be selected in the code itself.
 *
 * Current features include:
 * - Support for periodic light curves having a variety of shapes
 * - Support for stochastic light curves having a variety of correlation 
 *	properties
 * - Support for a user-selected amount of Gaussian white noise
 * - Support for signal injection tests using a randomized sample of base 
 * 	light curves
 * - Support for calculation of C1, periods, and &Delta;m&Delta;t timescales
 * 
 * @section changelog Version History
 *
 * @subsection v2_1_0 2.1.0-devel
 *
 * - Changed: renamed lcmcinject, lcmcmodels, lcmcparse, lcmcstats, 
 *	and lcmcutils to lcmc::inject, lcmc::models, lcmc::parse, 
 *	lcmc::stats, and lcmc::utils, respectively.
 *
 * @subsection v2_0_0 2.0.0
 *
 * - Changed: replaced period-amplitude command-line interface with one that 
 *	allows the user to specify an arbitrary set of parameters, but at the 
 *	cost of evaluating only a single bin per run
 * - Changed: redid the entire way information is passed to 
 *	lcmcmodels::ILightCurve subclasses, in particular rewriting the 
 *	interface of the lcmcmodels::ParamList class from scratch and moving 
 *	it outside the lcmcmodels::ILightCurve class.
 * - Changed: redid lcmcmodels::ILightCurve interface to add support for 
 *	stochastic light curves.
 * - Added: implemented lcmcmodels::WhiteNoise, lcmcmodels::DampedRandomWalk, 
 *	and lcmcmodels::SimpleGp
 * - Changed: cleaned up documentation.
 * - Fixed: C1 will now be calculated correctly in magnitude space for light 
 *	curves where the measured (noisy) flux is negative.
 * - Added: support for &Delta;m&Delta;t timescales
 * - Deleted: ILightCurve::modelParams()
 * - Changed: from now on, version numbers will conform to the 
 *	Semantic Versioning specification
 *
 * @subsection v1_4_1 1.4.1
 *
 * - Added: signal injection capability (NonSpitzerNonVar only)
 *
 * @subsection v1_4 1.4
 *
 * - Changed: Added lcmcmodels::LightCurve heirarchy
 *
 * @subsection v1_3 1.3
 *
 * - Started formal version documentation
 */

#endif		// LCMCHEADERH
