/** Header for defining overall program properties
 * @file lightcurveMC/projectinfo.h
 * @author Krzysztof Findeisen
 * @date Created April 19, 2013
 * @date Last modified August 31, 2013
 *
 * @todo Consider adding an algorithms overview for interested researchers.
 * @todo Rely less on integration tests
 * @todo Look into a truly free alternative to GSL
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LCMCPROJINFOH
#define LCMCPROJINFOH

/** Brief description of the program, to make blurb quoted by program 
 *	consistent with documentation.
 */
#define PROG_SUMMARY "Lightcurve MC carries out Monte Carlo simulations of a statistical light curve analysis given a particular observing cadence. It takes as its primary argument a list of time stamps, and returns a table of light curve statistics and their variance from run to run. Additional files may contain the statistics themselves."

/** Current version of the program, to make version quoted by program 
 *	consistent with documentation.
 *
 * "+build" tag can be used to distinguish which development version 
 *	was used to create which output
 */
#define VERSION_STRING "2.3.0"

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
 * - Support for calculation of C1, periods, &Delta;m&Delta;t timescales, 
 *	ACF timescales, peak-finding, and Gaussian process modeling.
 *
 * @section legal License
 * 
 * Copyright (C) 2014 California Institute of Technology
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * in <a href="../../LICENSE">lightcurveMC/LICENSE</a> 
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section credits Credits
 *
 * Lightcurve MC was primarily written by Krzysztof Findeisen. Please contact 
 * him at krzys, astro caltech edu for questions, feedback, or bug reports. 
 * The @c ParamList class was suggested by Timothy Morton.
 * 
 * @page install Installation
 *
 * @section install_reqs Requirements
 *
 * Lightcurve MC is written primarily in standard C++, with a few components 
 * written in R. The program should build on any C++98/C++03-compliant 
 * compiler. For portability between build environments, no TR1 or C++11 
 * features are included. However, the makefile itself is specific to GCC, so 
 * users wishing to install Lightcurve MC on a non-Unix platform may need to 
 * revise the contents of the following files: 
 * - @c makefile
 * - @c makefile.inc
 * - @c makefile.common
 * - @c makefile.subdirs
 * 
 * The program may switch to CMake in the future for improved portability.
 *
 * The sections written in R require version 2.15.1 or later of the 
 * R interpreter. The program has not been tested with R version 3.
 *
 * Lightcurve MC depends on the following libraries, available from the 
 * same source as Lightcurve MC: 
 * - Timescales 1.0 or later
 * - kpfutils 1.0 or later
 * 
 * and on the following external libraries:
 * - <a href="http://www.boost.org/">Boost</a> 1.33 or later
 * - <a href="http://www.gnu.org/software/gsl/">Gnu Scientific Library (GSL)</a> 1.8 
 *	or later (version 1.15 or later recommended for best performance when 
 *	generating Gaussian process light curves)
 * - <a href="http://tclap.sourceforge.net/">Templatized Command Line Parser (TCLAP)</a> 1.2 
 *	or later
 * - <a href="http://dirk.eddelbuettel.com/code/rcpp.html">Rcpp</a> 0.8.5 or later
 * - <a href="http://dirk.eddelbuettel.com/code/rinside.html">RInside</a> 0.2.6 or later
 * - <a href="http://cran.r-project.org/web/packages/gptk/">Gaussian Process Tool Kit (gptk)</a> 1.03 or later
 * - <a href="http://cran.r-project.org/web/packages/numDeriv/">numDeriv</a> 2006.4-1 or later
 * 
 * These libraries are not provided with the installation package, as most 
 * systems will have Boost, GSL, and R installed already. Please contact your 
 * system administrator if these libraries are not installed.
 * 
 * In addition, (re-)generating this documentation requires 
 * <a href="http://www.doxygen.org/">Doxygen</a> 1.8.1 or later.
 *
 * @section install_make Build Commands
 *
 * Running <tt>make</tt>, with no arguments, will build @c lightcurveMC in the 
 * Lightcurve MC root directory. Running <tt>make unittest</tt> will build the 
 * unit test suite at @c tests/test. Running <tt>make autotest</tt> will 
 * build both Lightcurve MC and the test suite, if neccessary, before 
 * executing @c tests/autotest.sh. Finally, running <tt>make doc</tt> will 
 * generate this documentation at @c doc/html/index.html and 
 * (if you have LaTeX installed) @c doc/latex/refman.pdf, and running 
 * <tt>make all</tt> will generate the program, the test suite, and the 
 * documentation.
 *
 * @page use User's Guide
 * 
 * @section cmd Command-Line Options
 * 
 * Lightcurve MC accepts the following arguments: 
 *
 * @subsection args_basic Simulation Type
 * 
 * Lightcurve MC must take exactly one of the following options: 
 * <dl>
 * <dt><tt>-\-add &lt;catalog&gt;</tt></dt><dd>Gives the name of a file 
 * containing a list of file names (paths may be absolute or relative to the 
 * working directory), one per line. The files must be three-column, 
 * space-delimited text files containing a light curve in the format 
 * (JD mag magerr). The program will carry out a signal injection analysis, 
 * adding each simulated light curve to the data from a randomly selected 
 * file in the catalog.<br>
 * The keywords NonSpitzerNonVar, NonSpitzerVar, SpitzerNonVar, and SpitzerVar 
 * may be used as aliases for @c nonspitzernonvar.cat, @c nonspitzervar.cat, 
 * @c spitzernonvar.cat, or @c spitzervar.cat, respectively, but this 
 * functionality is deprecated and will be removed in the next major version.</dd>
 * <dt><tt>&lt;jdlist&gt;</tt></dt><dd>Gives the name of a file containing a 
 * list of Julian dates, one per line. The program will carry out a pure 
 * simulation analysis, generating light curves sampled at the times listed 
 * in the file.</dd>
 * </dl>
 * 
 * @subsection args_opts Simulation Options
 * 
 * <dl>
 * <dt><tt>-\-noise &lt;amp&gt;</tt></dt><dd>Adds white noise of the specified 
 *	RMS amplitude (in units of the median light curve flux) to the 
 *	measurements before carrying out any statistical analysis. This 
 *	argument may not be combined with the @c -\-add argument.</dd>
 * <dt><tt>-\-ntrials &lt;number&gt;</tt></dt><dd>The number of light curves to 
 *	generate (per light curve type). If no @c -\-ntrials argument is 
 *	specified, defaults to 1000.</dd>
 * <dt><tt>-\-print &lt;number&gt;</tt></dt><dd>This argument is intended primarily for 
 *	debugging program behavior. It prints the specified number of light 
 *	curves (per light curve type) to temporary files as tables of date and 
 *	flux. If no @c -\-print argument is specified, defaults to 0.</dd>
 * <dt><tt>-s, -\-stat &lt;keyword&gt;</tt></dt><dd>This argument, which may be 
 * repeated, specifies the test(s) to run on the simulated light curves. 
 * Allowed keywords are: 
 *	<dl>
 *	<dt><tt>c1</tt></dt><dd>Calculates the mean and scatter, across multiple light 
 *	curves, of the modified C1 statistic @cite RotorCtts</dd>
 *	<dt><tt>period</tt></dt><dd>Calculates the mean and scatter, across multiple 
 *	light curves, of the period of the highest peak in a Lomb-Scargle 
 *	periodogram @cite LSPeriodogram. Only periods with FAP < 1% are 
 *	counted. For computational efficiency, the FAP is determined with 
 *	respect to a null hypothesis of Gaussian white noise.</dd>
 *	<dt><tt>periplot</tt></dt><dd>Creates a file containing all the Lomb-Scargle 
 *	periodograms generated in a particular run</dd>
 *	<dt><tt>dmdtcut</tt></dt><dd>Calculates the mean and scatter, across multiple 
 *	light curves, of the &Delta;t value where the median and the 90th 
 *	percentile of &Delta;m cross one third and one half of the measured 
 *	light curve amplitude</dd>
 *	<dt><tt>dmdtplot</tt></dt><dd>Creates a file containing all the median 
 *	&Delta;m curves generated in a particular run</dd>
 *	<dt><tt>iacfcut</tt></dt><dd>Calculates the mean and scatter, across multiple 
 *	light curves, of the time lag where the interpolated 
 *	autocorrelation function crosses one ninth, one fourth, 
 *	and one half</dd>
 *	<dt><tt>iacfplot</tt></dt><dd>Creates a file containing all the interpolated 
 *	autocorrelation functions generated in a particular run.</dd>
 *	<dt><tt>sacfcut</tt></dt><dd>Calculates the mean and scatter, across multiple 
 *	light curves, of the time lag where the Scargle autocorrelation
 *	function @cite ScargleAcf crosses one ninth, one fourth, 
 *	and one half</dd>
 *	<dt><tt>sacfplot</tt></dt><dd>Creates a file containing all the Scargle 
 *	autocorrelation functions generated in a particular run.</dd>
 *	<dt><tt>peakcut</tt></dt><dd>Calculates the mean and scatter, across multiple 
 *	light curves, of the time separation between consecutive local 
 *	minima and maxima separated by at least one third and one half 
 *	the measured light curve amplitude</dd>
 *	<dt><tt>peakplot</tt></dt><dd>Creates a file containing all the plots of time 
 *	separation vs. threshold magnitude for identifying a peak.</dd>
 *	<dt><tt>gptau</tt></dt><dd>Fits a Gaussian process model to each light 
 *	curve and returns the best-fit time scale parameters. Be aware that 
 *	this statistic is several orders of magnitude slower than the other 
 *	options.</dd>
 *	</dl>
 * If no @c -\-stat arguments are given, the default behavior is to run all tests.
 * </dd>
 * </dl>
 * 
 * @subsection args_lc Light Curve Properties
 *
 * <dl>
 * <dt><tt>-\-amp, -\-period, etc.</tt></dt><dd>Parameters specifying the light curve 
 * parameters. For a list of which parameters apply to which light curves, 
 * see below. Parameters must be given as pairs of numbers in quotes, 
 * representing the range from which the parameter value will be drawn. 
 * For example, <tt>-\-amp "0.5 0.6"</tt> means to randomly draw an amplitude 
 * between 0.5 and 0.6. A fixed parameter value can be specified by making 
 * the first and second numbers the same.</dd>
 * <dt><tt>&lt;light curve list&gt;</tt></dt><dd>A space-separated list of keywords 
 * describing the light curve(s) to simulate. At least one value is required, 
 * and the list must be given after @c &lt;jdlist&gt; if the latter is used. 
 * Allowed keywords are:
 *	<dl>
 *	<dt><tt>flat</tt></dt><dd>Generates a light curve with no variability. 
 *	Particularly useful as a comparison case for injection tests, since the 
 *	light curves from the sample catalog will be read unchanged into the 
 *	statistics calculations.</dd>
 *	<dt><tt>sine</tt></dt><dd>Generates a signal that varies sinusoidally in flux.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the half-amplitude of the 
 *			light curve, in units of the median flux. 
 *			The amplitude must be at most 1 to keep the flux 
 *			non-negative at all times.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>magsine</tt></dt><dd>Generates a signal that varies sinusoidally in magnitude.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the half-amplitude of the 
 *			light curve, in magnitudes.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>aatau</tt></dt><dd>Generates a signal that has periodic dips in magnitude.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the half-amplitude of the 
 *			light curve, in magnitudes.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The edge-to-edge width of 
 *			the dip, in units of the period. Must be less 
 *			than 1.0.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>ellipse</tt></dt><dd>Generates a signal that varies like a distorted 
 *		sinusoid in flux, with a slow rise and a steep fall.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the half-amplitude of the 
 *			light curve, in units of the median flux. 
 *			The amplitude must be at most 1 to keep the flux 
 *			non-negative at all times.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>triangle</tt></dt><dd>Generates a signal that varies like a rounded 
 *		sawtooth wave in flux, with a slow rise and a steep fall.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the half-amplitude of the 
 *			light curve, in units of the median flux. 
 *			The amplitude must be at most 1 to keep the flux 
 *			non-negative at all times.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>broad_peak</tt></dt><dd>Generates an asymmetric signal in flux space 
 *		that has a relatively flat minimum and a relatively extended 
 *		maximum.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the peak rises 
 *			above the flat minimum, in units of the 
 *			median flux.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>sharp_peak</tt></dt><dd>Generates an asymmetric signal in flux space 
 *		that has a long, flat minimum and a short maximum.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the peak rises 
 *			above the flat minimum, in units of the 
 *			median flux.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>eclipse</tt></dt><dd>Generates a square wave approximation of an 
 *		eclipsing binary, with the secondary eclipse having 70% the 
 *		depth of the primary eclipse in flux space. The width of both 
 *		eclipses is fixed at 5% of the period.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>The depth of the primary eclipse, in 
 *			units of the baseline flux. The amplitude must be 
 *			at most 1 to keep the flux non-negative at all 
 *			times.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The period of the light curve, 
 *			in days.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>slow_peak</tt></dt><dd>Generates a signal with periodic Gaussian 
 *		outbursts in flux space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the peak rises 
 *			above the flat minimum, in units of the 
 *			median flux.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The interval between consecutive 
 *			outbursts, in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The 1-&sigma; width of the outburst, 
 *			in units of the period. Must be less than 0.234107 to 
 *			preserve qualitative light curve behavior.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>flare_peak</tt></dt><dd>Generates a signal with periodic outbursts in 
 *		flux space consisting of a linear rise and an exponential 
 *		fade.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the peak rises 
 *			above the flat minimum, in units of the 
 *			median flux.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The interval between consecutive 
 *			outbursts, in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The exponential decay time of the 
 *			outburst, in units of the period.</dd>
 *		<dt><tt>-\-width2</tt></dt><dd>The linear rise time of the outburst, 
 *			in units of the period. Must be less than 1.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>flat_peak</tt></dt><dd>Generates a signal with periodic flat-topped 
 *		outbursts in flux space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the peak rises 
 *			above the flat minimum, in units of the 
 *			median flux.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The interval between consecutive 
 *			outbursts, in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The width of the outburst, 
 *			in units of the period. Must be less than 1.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>slow_dip</tt></dt><dd>Generates a signal with periodic Gaussian fades 
 *		in flux space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the fade falls 
 *			below the flat maximum, in units of the 
 *			median flux. The amplitude must be at most 1 to keep 
 *			the flux non-negative at all times.</dd></dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The interval between consecutive 
 *			fades, in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The 1-&sigma; width of the fade, 
 *			in units of the period. Must be less than 0.234107 to 
 *			preserve qualitative light curve behavior.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>flare_dip</tt></dt><dd>Generates a signal with periodic fades in flux 
 *		space consisting of a linear fall and an exponential rise.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the fade falls 
 *			below the flat maximum, in units of the 
 *			median flux. The amplitude must be at most 1 to keep 
 *			the flux non-negative at all times.</dd></dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The interval between consecutive 
 *			fades, in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The exponential recovery time of the 
 *			outburst, in units of the period.</dd>
 *		<dt><tt>-\-width2</tt></dt><dd>The linear fall time of the outburst, 
 *			in units of the period. Must be less than 1.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>flat_dip</tt></dt><dd>Generates a signal with periodic flat-bottomed 
 *		fades in flux space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>Specifies the amount the fade falls 
 *			below the flat maximum, in units of the 
 *			median flux. The amplitude must be at most 1 to keep 
 *			the flux non-negative at all times.</dd></dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The interval between consecutive 
 *			fades, in days.</dd>
 *		<dt><tt>-w, -\-width</tt></dt><dd>The width of the fade, 
 *			in units of the period. Must be less than 1.</dd>
 *		</dl>
 *	Optional keywords: <dl>
 *		<dt><tt>-\-ph</tt></dt><dd>The phase of the light curve at time zero. 
 *			If the keyword is omitted, the phase is 
 *			drawn randomly from the interval [0, 1).</dd>
 *		</dl></dd>
 *	<dt><tt>white_noise</tt></dt><dd>Generates a white noise signal in magnitude space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>The RMS amplitude of the white noise, 
 *			in magnitudes.</dd>
 *		</dl></dd>
 *	<dt><tt>simple_gp</tt></dt><dd>Generates a Gaussian process signal in 
 *		magnitude space with Gaussian covariance.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>The RMS amplitude of the Gaussian 
 *			process, in magnitudes.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The coherence time of the Gaussian 
 *			process, in days.</dd>
 *		</dl></dd>
 *	<dt><tt>two_gp</tt></dt><dd>Generates a Gaussian process signal in magnitude 
 *		space with a covariance having two Gaussian components.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-a, -\-amp</tt></dt><dd>The RMS amplitude of the first 
 *			component, in magnitudes.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The coherence time of the first 
 *			component, in days.</dd>
 *		<dt><tt>-\-amp2</tt></dt><dd>The RMS amplitude of the second 
 *			component, in magnitudes.</dd>
 *		<dt><tt>-\-period2</tt></dt><dd>The coherence time of the second 
 *			component, in days.</dd>
 *		</dl></dd>
 *	<dt><tt>walk</tt></dt><dd>Generates a random walk signal in magnitude space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-d, -\-diffus</tt></dt><dd>The diffusion constant of the 
 *			random walk, in mag<sup>2</sup>/day.</dd>
 *		</dl></dd>
 *	<dt><tt>drw</tt></dt><dd>Generates a damped random walk signal in magnitude space.<br>
 *	Required keywords: <dl>
 *		<dt><tt>-d, -\-diffus</tt></dt><dd>The diffusion constant of the 
 *			random walk, in mag<sup>2</sup>/day.</dd>
 *		<dt><tt>-p, -\-period</tt></dt><dd>The damping time of the random 
 *			walk, in days.</dd>
 *		</dl></dd>
 *	</dl>
 * </dd>
 * </dl>
 * 
 * @subsection args_extra Information
 * 
 * <dl>
 * <dt><tt>-h, -\-help</tt></dt><dd>Prints a brief summary of the arguments, then exits.</dd>
 * <dt><tt>-\-version</tt></dt><dd>Prints the program version, then exits.</dd>
 * </dl>
 *
 * @section output Output Format
 *
 * Lightcurve MC generates a tab- and &plusmn;-delimited row for each valid 
 * keyword in &lt;light curve list&gt;. If the program output is redirected to 
 * a file, the results of multiple runs may be concatenated to create a table 
 * of results. This table may be read by any CSV reader by telling it to treat 
 * both tabs and &plusmn; as delimiters.
 *
 * The first entry in the row is the light curve keyword. The next entries 
 * are the light curve parameters. While the order is guaranteed to be 
 * consistent regardless of which light curve parameters were constrained, 
 * specifying different numbers of parameters from run to run will lead to 
 * outputs with different numbers of columns, which can no longer be easily 
 * read as a single table. The next entry is the file name provided with the 
 * @c -\-add keyword, if applicable, or the @c -\-noise keyword otherwise.
 * 
 * The remaining elements of the row are the outputs of the statistical 
 * analysis. Any statistics not calculated by the program are omitted. 
 * Each statistic requested on the command line adds the following data to 
 * the program output: 
 * <dl>
 * <dt><tt>c1</tt></dt><dd>The program prints the average value of C1 (@cite RotorCtts) 
 * calculated from the simulation runs, followed by &plusmn;, followed by the 
 * standard deviation of the C1 values. The program then prints the fraction 
 * of simulations in which C1 was defined, followed by the name of a 
 * file containing the individual C1 values from each simulation, one per 
 * line, for more in-depth analysis of the results.</dd>
 * <dt><tt>period</tt></dt><dd>The program prints the average period calculated from 
 * the simulation runs, followed by &plusmn;, followed by the standard deviation 
 * of the periods. The program then prints the fraction of simulations in 
 * which a significant (FAP < 1% relative to Gaussian white noise) period was 
 * found, followed by the name of a file containing the individual periods 
 * from each simulation, one per line, for more in-depth analysis of the 
 * results.</dd>
 * <dt><tt>periplot</tt></dt><dd>The program prints the name of a file containing 
 * pairs of rows representing the individual periodograms from each run. 
 * The first row in each pair contains the frequencies in space-
 * delimited format, while the second row contains the power at each frequency 
 * in space-delimited format.</dd>
 * <dt><tt>dmdtcut</tt></dt><dd>The program prints the average &Delta;t value at which 
 * the median &Delta;m curve crosses one third the measured light curve 
 * amplitude, followed by &plusmn;, followed by the standard deviation of the 
 * crossing times. The program then prints the fraction of simulations in 
 * which the median &Delta;m curve did cross one third the amplitude, 
 * followed by the name of a file containing the individual values from each 
 * simulation (or NaN where no crossing occurred), one per line, for more 
 * in-depth analysis of the results.<br> 
 * These four values are then repeated for the point at which the median 
 * &Delta;m curve crosses half the amplitude, the point at which the 90th 
 * percentile of &Delta;m crosses one third the amplitude, and the point 
 * where the 90th percentile crosses half the amplitude.</dd>
 * <dt><tt>dmdtplot</tt></dt><dd>The program prints the name of a file containing 
 * pairs of rows representing the individual &Delta;m&Delta;t plots from each 
 * run. The first row in each pair contains the &Delta;t values in space-
 * delimited format, while the second row contains the &Delta;m values in 
 * space-delimited format.</dd>
 * <dt><tt>iacfcut</tt></dt><dd>The program prints the average time lag at which 
 * the interpolated ACF crosses one ninth the measured light curve 
 * amplitude, followed by &plusmn;, followed by the standard deviation of the 
 * crossing times. The program then prints the fraction of simulations in 
 * which the ACF did cross one ninth the amplitude, followed by the name 
 * of a file containing the individual values from each simulation 
 * (or NaN where no crossing occurred), one per line, for more in-depth 
 * analysis of the results.<br> 
 * These four values are then repeated for the point at which the ACF 
 * crosses one quarter of the amplitude and the point at which the ACF 
 * crosses half the amplitude.</dd>
 * <dt><tt>iacfplot</tt></dt><dd>The program prints the name of a file containing 
 * pairs of rows representing the individual interpolated autocorrelation 
 * functions from each run. The first row in each pair contains the time 
 * lags in space-delimited format, while the second row contains the 
 * correlations in space-delimited format. For brevity, only lags differing by 
 * at least 5\% are printed, but all lags are used to calculate the statistics 
 * in @c iacfcut.</dd>
 * <dt><tt>sacfcut</tt></dt><dd>The program prints the average time lag at which 
 * the Scargle ACF @cite ScargleAcf crosses one ninth the measured 
 * light curve amplitude, followed by &plusmn;, followed by the standard 
 * deviation of the crossing times. The program then prints the fraction of 
 * simulations in which the ACF did cross one ninth the amplitude, followed 
 * by the name of a file containing the individual values from each 
 * simulation (or NaN where no crossing occurred), one per line, for more 
 * in-depth analysis of the results.<br> 
 * These four values are then repeated for the point at which the ACF 
 * crosses one quarter of the amplitude and the point at which the ACF 
 * crosses half the amplitude.</dd>
 * <dt><tt>sacfplot</tt></dt><dd>The program prints the name of a file containing 
 * pairs of rows representing the individual Scargle autocorrelation 
 * functions from each run. The first row in each pair contains the time 
 * lags in space-delimited format, while the second row contains the 
 * correlations in space-delimited format. For brevity, only lags differing by 
 * at least 5\% are printed, but all lags are used to calculate the statistics 
 * in @c sacfcut.</dd>
 * <dt><tt>peakcut</tt></dt><dd>The program prints the average characteristic time 
 * between local minima and maxima separated by at least one third the 
 * measured light curve amplitude, followed by &plusmn;, followed by the 
 * standard deviation of the characteristic times. The program then prints 
 * the fraction of simulations in which multiple minima or maxima were 
 * identified, followed by the name of a file containing the individual 
 * values from each simulation (or NaN where it could not be defined), one 
 * per line, for more in-depth analysis of the results.<br> 
 * These four values are then repeated for the time between local extrema 
 * separated by one half the amplitude and the time between extrema separated 
 * by by 80% of the amplitude.</dd>
 * <dt><tt>peakplot</tt></dt><dd>The program prints the name of a file containing 
 * pairs of rows representing the individual peak-finding functions from each 
 * run. The first row in each pair contains the timescales in space-delimited 
 * format, while the second row contains the magnitude differences at which 
 * each timescale was calculated in space-delimited format.</dd>
 * <dt><tt>gptau</tt></dt><dd>The program prints the average time scale 
 * calculated from the simulation runs, followed by &plusmn;, followed by the 
 * standard deviation of the time scales. The program then prints the fraction 
 * of simulations in which a time scale could be solved for, followed by 
 * the name of a file containing the individual time scales from each 
 * simulation, one per line, for more in-depth analysis of the results. 
 * These four values are then repeated for the formal uncertainty on the 
 * time scale. Finally, the program prints the &chi;<sup>2</sup> statistic 
 * describing whether the inferred time scales are consistent with the true 
 * light curve time scale, given their formal errors.</dd>
 * </dl>
 * 
 * @section examples Examples
 *
 * @subsection ex_basic Minimal Usage
 *
 * What is the scatter in a periodogram produced at a particular cadence?
 * 
 * @par Type: 
@verbatim
lightcurveMC myjdlist.txt sine \
	--amp "1.0 1.0" --period "4.0 4.0" \
	--stat periplot
@endverbatim
 * @par Output: 
@verbatim
LCType		a      	p      	ph     	Noise	Periodograms
sine          	1	4	0	0	run_pgram_sine_a1.00_p4.00_p0.00_n0.dat
@endverbatim
 * 
 * @subsection ex_stats Summary Statistics, Variable Parameters
 *
 * How accurate is the period in the presence of noise?
 * 
 * @par Type: 
@verbatim
lightcurveMC myjdlist.txt sine --noise 0.05 \
	--amp "0.01 1.0" --period "4.0 4.0" \
	--stat period
@endverbatim
 * @par Output: 
@verbatim
LCType          a       p       ph      Noise   Period�err      Finite  Period Distribution
sine            0.01    4       0       0.05      4.01� 0.19         1  run_peri_sine_a0.01_p4.00_p0.00_n0.05.dat
@endverbatim
 * 
 * @subsection ex_toomany Unnecessary Parameters
 *
 * Unused parameters change the output format but not the statistics: 
 * 
 * @par Type: 
@verbatim
lightcurveMC myjdlist.txt sine --noise 0.05 \
	--amp "0.01 1.0" --period "4.0 4.0" --width "0.1 0.1" \
	--stat period
@endverbatim
 * @par Output: 
@verbatim
LCType          a       p       ph      width   Noise   Period�err      Finite  Period Distribution
sine            0.01    4       0       0.1     0.05      4.01� 0.19         1  run_peri_sine_a0.01_p4.00_p0.00_w0.10_n0.05.dat
@endverbatim
 * 
 * @subsection ex_params Multiple Light Curves, More Complex Parameters
 *
 * What is the C1 distribution for an asymmetric light curve?
 * 
 * @par Type: 
@verbatim
lightcurveMC myjdlist.txt sine magsine flare_dip flare_peak \
	--amp "1.0 1.0" --period "4.0 4.0" \
	--width "0.3 0.3" --width2 "0.01 0.01" \
	--stat C1
@endverbatim
 * @par Output: 
@verbatim
LCType          a       p       ph      width   width2  Noise   C1�err  Finite	C1 Distribution
sine            1       4       0       0.3     0.01    0        0.158�0.093    1	run_c1_sine_a1.00_p4.00_p0.00_w0.30_w0.01_n0.dat
magsine         1       4       0       0.3     0.01    0        0.496� 0.11    1	run_c1_magsine_a1.00_p4.00_p0.00_w0.30_w0.01_n0.dat
flare_dip       1       4       0       0.3     0.01    0        0.111�0.053    1	run_c1_flare_dip_a1.00_p4.00_p0.00_w0.30_w0.01_n0.dat
flare_peak      1       4       0       0.3     0.01    0        0.747�0.069    1	run_c1_flare_peak_a1.00_p4.00_p0.00_w0.30_w0.01_n0.dat
@endverbatim
 * 
 * @subsection ex_inject Injection Tests, Multiple Statistics, Multiple Runs
 *
 * At what amplitude can clean statistics still be extracted given realistic noise?
 * 
 * @par Type: 
@verbatim
lightcurveMC --add myobslist.txt flat broad_peak sharp_peak \
	--amp "0.01 0.01" --period "4.0 4.0" \
	--stat C1 --stat period
lightcurveMC --add myobslist.txt flat broad_peak sharp_peak \
	--amp "0.1 0.1" --period "4.0 4.0" \
	--stat C1 --stat period
lightcurveMC --add myobslist.txt flat broad_peak sharp_peak \
	--amp "1.0 1.0" --period "4.0 4.0" \
	--stat C1 --stat period
@endverbatim
 * @par Output: 
@verbatim
LCType          a       p       ph      Noise   C1�err  Finite	C1 Distribution Period�err      Finite  Period Distribution
flat            0.01    4       0       myobslist.txt     0.401�7.5e-10  1	run_c1_flat_a0.01_p4.00_p0.00_nmyobslist.txt.dat    nan�  nan         0  run_peri_flat_a0.01_p4.00_p0.00_nmyobslist.txt.dat
broad_peak      0.01    4       0       myobslist.txt     0.402�0.0026   1	run_c1_broad_peak_a0.01_p4.00_p0.00_nmyobslist.txt.dat      nan�  nan         0  run_peri_broad_peak_a0.01_p4.00_p0.00_nmyobslist.txt.dat
sharp_peak      0.01    4       0       myobslist.txt     0.402�0.0031   1	run_c1_sharp_peak_a0.01_p4.00_p0.00_nmyobslist.txt.dat      nan�  nan         0  run_peri_sharp_peak_a0.01_p4.00_p0.00_nmyobslist.txt.dat
LCType          a       p       ph      Noise   C1�err  C1 Distribution Period�err      Finite  Period Distribution
flat            0.1     4       0       myobslist.txt     0.401�7.5e-10  1	run_c1_flat_a0.10_p4.00_p0.00_nmyobslist.txt.dat    nan�  nan         0  run_peri_flat_a0.10_p4.00_p0.00_nmyobslist.txt.dat
broad_peak      0.1     4       0       myobslist.txt     0.426�0.024    1	run_c1_broad_peak_a0.10_p4.00_p0.00_nmyobslist.txt.dat      nan�  nan         0  run_peri_broad_peak_a0.10_p4.00_p0.00_nmyobslist.txt.dat
sharp_peak      0.1     4       0       myobslist.txt     0.414�0.022    1	run_c1_sharp_peak_a0.10_p4.00_p0.00_nmyobslist.txt.dat      nan�  nan         0  run_peri_sharp_peak_a0.10_p4.00_p0.00_nmyobslist.txt.dat
LCType          a       p       ph      Noise   C1�err  C1 Distribution Period�err      Finite  Period Distribution
flat            1       4       0       myobslist.txt     0.401�7.5e-10  1	run_c1_flat_a1.00_p4.00_p0.00_nmyobslist.txt.dat    nan�  nan         0  run_peri_flat_a1.00_p4.00_p0.00_nmyobslist.txt.dat
broad_peak      1       4       0       myobslist.txt     0.635�0.059    1	run_c1_broad_peak_a1.00_p4.00_p0.00_nmyobslist.txt.dat      nan�  nan         0  run_peri_broad_peak_a1.00_p4.00_p0.00_nmyobslist.txt.dat
sharp_peak      1       4       0       myobslist.txt      0.67�0.074    1	run_c1_sharp_peak_a1.00_p4.00_p0.00_nmyobslist.txt.dat      nan�  nan         0  run_peri_sharp_peak_a1.00_p4.00_p0.00_nmyobslist.txt.dat
@endverbatim
 * 
 * @page changelog Version History
 * 
 * @brief <b></b>
 * 
 * Lightcurve MC conforms to 
 * <a href="http://semver.org/spec/v2.0.0.html">version 2.0.0 of the Semantic Versioning specification</a>. 
 * All version numbers are to be interpreted as described therein. The 
 * @ref use "\"User's Guide\"" @htmlonly page, @endhtmlonly 
 * @latexonly chapter, @endlatexonly not including the "Examples" section, 
 * fills the role of the public API for the program.
 *
 * @section v2_3_0 Version 2.3.0
 *
 * @subsection v2_3_0_diff Changes 
 * 
 * - Generic light curve code has now been moved to the @c kpfutils common library
 * - Factored Gaussian process code into a new class, 
 *	@ref lcmc::models::GaussianProcess "GaussianProcess"
 * - Factored command line parsing code into separate functions.
 * - Versioning convention updated from version 2.0.0-rc.1 to 
 *	<a href="http://semver.org/spec/v2.0.0.html">version 2.0.0 of the Semantic Versioning specification</a>.
 * 
 * @subsection v2_3_0_new New Features 
 * 
 * - Support for AA Tau light curves
 * - Support for Gaussian process timescales
 * 
 * @subsection v2_3_0_fix Bug Fixes 
 * 
 * - @c makefile is now more robust to missing directories
 * - Periodic light curves now check for invalid @p width and @p width2 parameters.
 * - @ref lcmc::inject::Observations "Observations" now handles exceptions 
 *	correctly.
 * - @ref lcmc::inject::Observations "Observations" objects 
 *	are now correctly randomized.
 * - Undefined values of C1 and the period will no longer be omitted from 
 *	the program output. Program output now includes the fraction of runs 
 *	in which C1 is defined.
 * - @c simple_gp and @c two_gp light curves now work correctly when one of 
 *	the input time stamps equals exactly zero
 * - Corrected some compatibility issues with GCC 4.7's warnings system
 * - Error messages related to parameters now use parameter names expected 
 *	by the user
 *
 * @section v2_2_0 Version 2.2.0
 *
 * @subsection v2_2_0_diff Changes 
 * 
 * - @c unit_dmdt.cpp now conforms to ISO C++
 * - Added test cases to @ref lcmc::test::BoostTest::test_stats "test_stats".
 * - Shortened all integration test scripts, cutting the 
 *	testing time by a factor of five without sacrificing accuracy
 * - Improved error recovery and reporting
 * - Make generates fewer spurious compilations
 * - Added installation instructions and user guide. The 
 *	@ref use "\"User's Guide\"" @htmlonly page, @endhtmlonly 
 *	@latexonly chapter, @endlatexonly not including the "Examples" 
 *	section, will be henceforth considered the public API for the 
 *	purposes of <a href="http://semver.org/">semantic versioning</a>.
 * - Corrected and standardized documentation
 * - Added PDF documentation in @c doc/latex/refman.pdf.
 * 
 * @subsection v2_2_0_new New Features 
 * 
 * - @c -\-add keyword can now take arbitrary file names as arguments. 
 *	The use of the keywords @c NonSpitzerNonVar, @c NonSpitzerVar, 
 *	@c SpitzerNonVar, and @c SpitzerVar is now deprecated.
 * - @c -\-stat (@c -s) keyword now allows run time selection of statistics 
 *	to compute for each light curve
 * - Support for ACF timescales and peak-finding timescales
 * 
 * @subsection v2_2_0_fix Bug Fixes 
 * 
 * - Output files now give more significant digits for the 
 *	simulation parameters, reducing naming conflicts between bins. Light 
 *	curve dump files created with the @c -\-print keyword now use the same 
 *	naming convention as the statistic output files.
 * - Noise will no longer be correlated with 
 *	@ref lcmc::models::WhiteNoise "WhiteNoise", 
 *	@ref lcmc::models::RandomWalk "RandomWalk", and 
 *	@ref lcmc::models::DampedRandomWalk "DampedRandomWalk" light curves
 * - @ref lcmc::models::FlarePeak "FlarePeak" and 
 *	@ref lcmc::models::FlareDip "FlareDip" light curves now have correct 
 *	parameter checking
 * - @ref lcmc::utils::ApproxEqual "ApproxEqual" now tests the correct condition
 *
 * @section v2_1_0 Version 2.1.0
 *
 * @subsection v2_1_0_diff Changes 
 * 
 * - Added documentation for test code. Declared a new namespace, 
 *	lcmc::test, containing all test suites.
 * - Added test cases to @ref lcmc::test::BoostTest::test_stats "test_stats".
 * - Added @ref lcmc::test::BoostTest::test_wave "test_wave", a unit test 
 *	suite for the older periodic light curves
 * - Added @c cmdtest.sh, an integration test suite for command line errors
 * - @c makefile now handles include dependencies in subdirectories correctly
 * - Removed information about the local GCC configuration from 
 *	@c makefile.inc.
 * - Incorporated @c mcio.cpp, @c lcsio.cpp, and @c kpffileio.cpp 
 *	into main program rather than as an external dependency.
 * - Sped up evaluation of @ref lcmc::models::SimpleGp "SimpleGp" 
 *	and @ref lcmc::models::TwoScaleGp "TwoScaleGp" by a factor of five 
 *	in the case that the model parameters are held fixed 
 *	(i.e., @c -\-amp, @c -\-period, etc. are given with a range of zero length)
 * 
 * @subsection v2_1_0_new New Features 
 * 
 * - Input validation for all command line arguments
 * - @ref lcmc::models::RandomWalk "RandomWalk" and 
 *	@ref lcmc::models::TwoScaleGp "TwoScaleGp". Added corresponding test 
 *	cases to @ref lcmc::test::BoostTest::test_gp "test_gp".
 * - Support for command-line arguments @c -\-amp2 and @c -\-period2.
 * 
 * @subsection v2_1_0_fix Bug Fixes 
 * 
 * - Bug that @ref lcmc::models::SimpleGp "SimpleGp" produces 
 *	inconsistent output downgraded to a warning, and moved to 
 *	@ref lcmc::utils::getHalfMatrix() "getHalfMatrix()". See 
 *	@ref lcmc::utils::getHalfMatrix() "getHalfMatrix()" documentation for 
 *	more details.
 * - Bug where deltaMBinQuantile() appeared to show the &Delta;m median crossing 
 *      half the amplitude before reaching the third-amplitude in Gaussian processes 
 *	has been explained. The behavior is the result of half-amplitude crossings 
 *	happening either in the poorly populated bins where 
 *	&Delta;t &asymp; 0.5&nbsp;day, or not at all, whereas statistical 
 *	fluctuations can cause spurious third-amplitude crossings (with no 
 *	corresponding half-amplitude crossing) in any &Delta;t bin. 
 *	This behavior, while odd, is a problem with long ground-based survey 
 *	cadences rather than with the analysis software, and is no longer 
 *	considered a bug.
 *	The behavior may be reproduced with the files 
 *	<tt>tests/paradox*.*</tt>.
 * - Tests in @ref lcmc::test::BoostTest::test_gp "test_gp" now report 
 *	success on completion
 * - @ref lcmc::stats::LcBinStats::analyzeLightCurve() 
 *	"LcBinStats::analyzeLightCurve()" now propagates exception 
 *	types correctly
 * - Test output files now distinguish runs with identical light curve 
 *	parameters but different noise levels
 * - The @c -\-add command line argument can no longer be combined with 
 *	either the list of julian dates or with the @c -\-noise argument
 * - @ref lcmc::stats::LcBinStats::printBinStats() "LcBinStats::printBinStats()" 
 *	will now report zero standard deviation, not NaN, for statistics 
 *	that were always calculated to the same value
 * - The @c -\-amp parameter for sines now represents the half-amplitude, as 
 *	intended, not the amplitude
 * - Light curves no longer generate negative fluxes
 * - Integration test scripts run more consistently across systems. In 
 *	particular, they are now designed for execution, not sourcing.
 * - makefile now finds libraries on C-shell
 *
 * @section v2_0_0 Version 2.0.0
 *
 * @subsection v2_0_0_diff Changes 
 * 
 * - Redid the entire way information is passed to 
 *	@ref lcmc::models::ILightCurve "ILightCurve" subclasses, 
 *	in particular rewriting the interface of the 
 *	@ref lcmc::models::ParamList "ParamList" class 
 *	from scratch and moving it outside the 
 *	@ref lcmc::models::ILightCurve "ILightCurve" class.
 * - Redid @ref lcmc::models::ILightCurve "ILightCurve" 
 *	interface to add support for stochastic light curves.
 * - Deleted ILightCurve::modelParams()
 * - Cleaned up documentation.
 * - From now on, version numbers will conform to 
 *	<a href="http://semver.org/spec/v2.0.0-rc.1.html">version 2.0.0-rc.1 of the Semantic Versioning specification</a>.
 * 
 * @subsection v2_0_0_new New Features 
 * 
 * - Replaced period-amplitude command-line interface with one that 
 *	allows the user to specify an arbitrary set of parameters, but at the 
 *	cost of evaluating only a single bin per run
 * - Implemented @ref lcmc::models::WhiteNoise "WhiteNoise", 
 *	@ref lcmc::models::DampedRandomWalk "DampedRandomWalk", and
 *	@ref lcmc::models::SimpleGp "SimpleGp".
 * - Support for &Delta;m&Delta;t timescales
 * 
 * @subsection v2_0_0_fix Bug Fixes 
 * 
 * - Fixed: C1 will now be calculated correctly in magnitude space for light 
 *	curves where the measured (noisy) flux is negative.
 *
 * @section v1_4_1 Version 1.4.1
 *
 * @subsection v1_4_1_new New Features 
 * 
 * - Signal injection capability
 *
 * @section v1_4 Version 1.4
 *
 * @subsection v1_4_diff Changes 
 * 
 * - Added @ref lcmc::models::ILightCurve "LightCurve" heirarchy
 *
 * @section v1_3 Version 1.3
 *
 * @subsection v1_3_diff Changes 
 * 
 * - Started formal version documentation
 * 
 */

#endif		// LCMCPROJINFOH
