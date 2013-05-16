/** Header for defining overall program properties
 * @file lightcurveMC/projectinfo.h
 * @author Krzysztof Findeisen
 * @date Created April 19, 2013
 * @date Last modified May 16, 2013
 */

#ifndef LCMCPROJINFOH
#define LCMCPROJINFOH

/** Brief description of the program, to make blurb quoted by program 
 *	consistent with documentation.
 */
#define PROG_SUMMARY "LightcurveMC carries out Monte Carlo simulations of a statistical light curve analysis given a particular observing cadence. It takes as its primary argument a list of time stamps, and returns a table of light curve statistics and their variance from run to run. Additional files may contain the statistics themselves."

/** Current version of the program, to make version quoted by program 
 *	consistent with documentation.
 */
#define VERSION_STRING "2.2.0-devel"

/** @mainpage
 *
 * @section intro Overview
 *
 * Lightcurve MC carries out Monte Carlo simulations of statistical light curve 
 * analysis, given a fixed observing pattern. The program is designed to be 
 * highly flexible with respect to the models adopted and to the statistical 
 * tests run on each light curve. The user may select either at run time, 
 * allowing for simulation details to be fine-tuned to the problem at hand.
 *
 * Current features include:
 * - Support for periodic light curves having a variety of shapes
 * - Support for stochastic light curves having a variety of correlation 
 *	properties
 * - Support for a user-selected amount of Gaussian measurement error
 * - Support for signal injection tests using a randomized sample of base 
 * 	light curves
 * - Support for calculation of C1, periods, &Delta;m&Delta;t timescales, and 
 *	ACF timescales
 * 
 * @section install Installation
 * 
 * @section use Usage
 * 
 * @section examples Examples
 * 
 * @section changelog Version History
 *
 * @subsection v2_2_0 2.2.0-devel
 *
 * - Changed: shortened all integration test scripts, cutting the 
 *	testing time by a factor of five without sacrificing accuracy
 * - Added: -\-stat (-s) keyword now allows run time selection of statistics 
 *	to compute for each light curve
 * - Added: support for ACF timescales
 * - Fixed: output files now give more significant digits for the 
 *	simulation parameters, reducing naming conflicts between bins. Light 
 *	curve dump files created with the -\-print keyword now use the same 
 *	naming convention as the statistic output files.
 * - Changed: -\-add keyword can now take arbitrary file names as arguments. 
 *	The use of the keywords NonSpitzerNonVar, NonSpitzerVar, 
 *	SpitzerNonVar, and SpitzerVar is now deprecated.
 * - Changed: improved error recovery and reporting
 * - Fixed: noise will no longer be correlated with white noise, 
 *	random walk, and damped random walk light curves
 *
 * @subsection v2_1_0 2.1.0
 *
 * - Fixed: tests in @ref lcmc::test::BoostTest::test_gp "test_gp" now report 
 *	success on completion
 * - Fixed: @ref lcmc::stats::LcBinStats::analyzeLightCurve() 
 *	"LcBinStats::analyzeLightCurve()" now propagates exception 
 *	types correctly
 * - Added: test cases to @ref lcmc::test::BoostTest::test_stats "test_stats".
 * - Changed: bug that @ref lcmc::models::SimpleGp "SimpleGp" produces 
 *	inconsistent output downgraded to a warning, and moved to 
 *	getHalfMatrix(). See getHalfMatrix() documentation for more details.
 * - Fixed: test output files now distinguish runs with identical light curve 
 *	parameters but different noise levels
 * - Fixed: the -\-add command line argument can no longer be combined with 
 *	either the list of julian dates or with the -\-noise argument
 * - Added: input validation for all command line arguments
 * - Added: cmdtest.sh, an integration test suite for command line errors
 * - Fixed: @ref lcmc::stats::LcBinStats::printBinStats() "LcBinStats::printBinStats()" 
 *	will now report zero standard deviation, not NaN, for statistics 
 *	that were always calculated to the same value
 * - Added: documentation for test code. Declared a new namespace, 
 *	lcmc::test, containing all test suites.
 * - Fixed: -\-amp parameter for sines now represents the half-amplitude, as 
 *	intended, not the amplitude
 * - Fixed: light curves no longer generate negative fluxes
 * - Added: @ref lcmc::test::BoostTest::test_wave "test_wave", a unit test 
 *	suite for the older periodic light curves
 * - Fixed: makefile now handles include dependencies in 
 *	subdirectories correctly
 * - Added: implemented @ref lcmc::models::RandomWalk "RandomWalk" and 
 *	@ref lcmc::models::TwoScaleGp "TwoScaleGp". Added corresponding test 
 *	cases to @ref lcmc::test::BoostTest::test_gp "test_gp".
 * - Added: support for command-line arguments "amp2" and "period2".
 * - Changed: speeded up evaluation of @ref lcmc::models::SimpleGp "SimpleGp" 
 *	and @ref lcmc::models::TwoScaleGp "TwoScaleGp" by a factor of five 
 *	in the case that the model parameters are held fixed 
 *	(i.e., -\-amp, -\-period, etc. are given with a range of zero length)
 * - Changed: bug where deltaMBinQuantile() appeared to show the 
 *	&Delta;m median crossing half the amplitude before reaching the 
 *	third-amplitude in Gaussian processes has been explained. 
 *	The behavior is the result of half-amplitude crossings happening 
 *	either in the poorly populated bins where 
 *	&Delta;t &asymp; 0.5&nbsp;day, or not at all, whereas statistical 
 *	fluctuations can cause spurious third-amplitude crossings (with no 
 *	corresponding half-amplitude crossing) in any &Delta;t bin. 
 *	This behavior, while odd, is a problem with long ground-based survey 
 *	cadences rather than with the analysis software, and is no longer 
 *	considered a bug.
 *	The behavior may be reproduced with the files tests/paradox*.*.
 * - Fixed: integration test scripts run more consistently across systems. In 
 *	particular, they are now designed for execution, not sourcing.
 * - Fixed: makefile now finds libraries on tcsh
 * - Changed: removed information about the local GCC configuration from 
 *	makefile.inc.
 * - Changed: incorporated mcio.cpp, lcsio.cpp, and kpffileio.cpp into main 
 *	program rather than as an external dependency.
 *
 * @subsection v2_0_0 2.0.0
 *
 * - Changed: replaced period-amplitude command-line interface with one that 
 *	allows the user to specify an arbitrary set of parameters, but at the 
 *	cost of evaluating only a single bin per run
 * - Changed: redid the entire way information is passed to 
 *	@ref lcmc::models::ILightCurve "ILightCurve" subclasses, 
 *	in particular rewriting the interface of the 
 *	@ref lcmc::models::ParamList "ParamList" class 
 *	from scratch and moving it outside the 
 *	@ref lcmc::models::ILightCurve "ILightCurve" class.
 * - Changed: redid @ref lcmc::models::ILightCurve "ILightCurve" 
 *	interface to add support for stochastic light curves.
 * - Added: implemented @ref lcmc::models::WhiteNoise "WhiteNoise", 
 *	@ref lcmc::models::DampedRandomWalk "DampedRandomWalk", and
 *	@ref lcmc::models::SimpleGp "SimpleGp".
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
 * - Changed: Added @ref lcmc::models::ILightCurve "LightCurve" heirarchy
 *
 * @subsection v1_3 1.3
 *
 * - Started formal version documentation
 */

#endif		// LCMCPROJINFOH
