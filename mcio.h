/** Input and output routines for Lightcurve MC
 * @file lightcurveMC/mcio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified May 27, 2013
 */

#ifndef LMCIOH
#define LMCIOH

#include <string>
#include <vector>
#include <cstdio>
#include <boost/shared_ptr.hpp>

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

/** Wrapper that throws @ref lcmc::except::FileIo "FileIo" if it cannot open a file
 */
boost::shared_ptr<FILE> fileCheckOpen(const std::string& fileName, const char* mode);

/** Wrapper that throws @ref lcmc::except::FileIo "FileIo" in response to 
 *	a C I/O library error
 */
void fileError(FILE* const stream, std::string msg);

#endif		// end ifndef LMCIOH
