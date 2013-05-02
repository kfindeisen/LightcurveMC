/** Input and output routines for lightcurveMC
 * @file mcio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified February 4, 2011
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
void printLightCurve(size_t modelNum, std::string lcName, double logP, double logA, 
	double period, DoubleVec timeGrid, DoubleVec fluxGrid);

/** Dumps the contents of a periodogram to a file
 */	
void printPeriodogram(size_t modelNum, size_t lcType, double logP, double logA, 
	double fap, double threshold, double freq, DoubleVec freqGrid, DoubleVec powerGrid);

#endif		// end ifndef LMCIOH
