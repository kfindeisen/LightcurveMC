// mcio.h
// by Krzysztof Findeisen
// Created February 4, 2011
// Last modified February 4, 2011
// Input and output routines for lightcurveMC

#ifndef LMCIOH
#define LMCIOH

#include <string>
#include <vector>
#include <cstdio>

typedef std::vector<double> DoubleVec;


/* Reads a file containing timestamps into a vector of dates
 * Input:
 *	hInput: an open file handle to be read. The file represented by hInput 
 *		is assumed to be formatted as a list of floating-point values, 
 *		one per line. If the file does not match this format the 
 *		function throws an exception.
 * Output:
 *	dates: a vector of doubles that stores the timestamps in hInput. The dates 
 *		will be sorted in ascending order.
 *	minDelT: a floating-point number that stores the smallest difference 
 *		between two elements of dates
 *	maxDelT: a floating-point number that stores the largest difference 
 *		between two elements of dates
 */	
void readTimeStamps(FILE* hInput, DoubleVec &dates, double &minDelT, double &maxDelT);

/* Dumps the contents of a lightcurve to a file
 * Input:
 *	lcName, logP, logA: a complete identifier for the model that generated 
 *		the lightcurve
 *	modelNum: a unique identifier distinguishing different instances of 
 *		identical models
 *	period: the true period of the lightcurve (for reference)
 *	timeGrid: a vector containing timestamps to copy to disk
 *	fluxGrid: a vector containing fluxes to copy to disk. Assumed to be the 
 *		same length as timeGrid.
 */	
void printLightCurve(size_t modelNum, std::string lcName, double logP, double logA, 
	double period, DoubleVec timeGrid, DoubleVec fluxGrid);

/* Dumps the contents of a periodogram to a file
 * Input:
 *	lcType, logP, logA: a complete identifier for the model that generated 
 *		the periodogram
 *	modelNum: a unique identifier distinguishing different instances of 
 *		identical models
 *	fap: the false alarm probability used to search for significant peaks (for reference)
 *	threshold: the height of the smallest peak that would be considered significant (for reference)
 *	freq: the true frequency of the lightcurve (for reference)
 *	freqGrid: a vector containing frequencies to copy to disk
 *	powerGrid: a vector containing powers to copy to disk. Assumed to be the 
 *		same length as freqGrid.
 */	
void printPeriodogram(size_t modelNum, size_t lcType, double logP, double logA, 
	double fap, double threshold, double freq, DoubleVec freqGrid, DoubleVec powerGrid);

#endif		// end ifndef LMCIOH
