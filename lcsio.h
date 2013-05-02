/** File I/O routines for lightcurve analysis tool
 * @file lcsio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified May 7, 2012
 */

#ifndef LCSIOH
#define LCSIOH

#include <string>
#include <vector>
#include <cstdio>

/** Shorthand for a vector of doubles
 */
typedef std::vector<double> DoubleVec;

/** Reads a file containing a list of file names
 */
void readFileNames(FILE* hInput, std::vector<std::string> &fileList);

/** Reads a file containing a list of julian days, magnitudes, and errors
 */	
void readWgLightCurve(FILE* hInput, double errMax, double &offset, DoubleVec &timeVec, 
	DoubleVec &fluxVec, DoubleVec &errVec);

/** Reads a file containing a list of obsids, julian days, magnitudes, errors, and limits
 */	
void readWg2LightCurve(FILE* hInput, double errMax, double &offset, DoubleVec &timeVec, 
	DoubleVec &fluxVec, DoubleVec &errVec);

/** Reads a file containing a list of julian days and fluxes
 */	
void readMcLightCurve(FILE* hInput, double &offset, DoubleVec &timeVec, 
	DoubleVec &fluxVec);

/** Reads a file containing a list of julian days and fluxes
 */	
void readCsvLightCurve(FILE* hInput, double &offset, DoubleVec &timeVec, 
	DoubleVec &fluxVec);

/** Prints a file containing a periodogram
 */	
void printPeriodogram(FILE* hOutput, const DoubleVec &freq, const DoubleVec &power, 
	double threshold, double fap);

/** Prints a file containing an autocorrelation function
 */	
void printAcf(FILE* hOutput, const DoubleVec &times, const DoubleVec &acf);

/** Prints a file containing a dmdt scatter plot
 */	
void printDmDt(FILE* hOutput, const DoubleVec &times, const DoubleVec &deltam);

/** Prints a file containing a histogram
 */	
void printHist(FILE* hOutput, const DoubleVec &binEdges, const DoubleVec &values);

/** Prints a file containing a rms vs. t scatter plot
 */	
void printRmsT(FILE* hOutput, const DoubleVec &times, const DoubleVec &rmsVals);

/** Filters a set of vectors to include only dates between date1 and date2
 */	
void filterLightCurve(double date1, double date2, DoubleVec &times, 
	DoubleVec &arr1, DoubleVec &arr2);

#endif
