/** Input and output routines for Lightcurve MC
 * @file lightcurveMC/mcio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified June 18, 2013
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

/** Reads a file containing timestamps into a vector of dates
 */	
void readTimeStamps(FILE* hInput, DoubleVec &dates);

/** Dumps the contents of a lightcurve to a file
 */
void printLightCurve(const std::string& fileName, 
		const DoubleVec& timeGrid, const DoubleVec& fluxGrid);

#endif		// end ifndef LMCIOH
