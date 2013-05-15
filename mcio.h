/** Input and output routines for lightcurveMC
 * @file lightcurveMC/mcio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified May 14, 2013
 */

#ifndef LMCIOH
#define LMCIOH

#include <string>
#include <vector>
#include <cstdio>

/** Shorthand for a vector of doubles
 */
typedef std::vector<double> DoubleVec;


/** Reads a file containing timestamps into a vector of dates
 */	
void readTimeStamps(FILE* hInput, DoubleVec &dates, double &minDelT, double &maxDelT);

/** Dumps the contents of a lightcurve to a file
 */
void printLightCurve(const std::string& fileName, 
		const DoubleVec& timeGrid, const DoubleVec& fluxGrid);

#endif		// end ifndef LMCIOH
