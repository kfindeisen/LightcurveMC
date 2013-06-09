/** Input and output routines for Lightcurve MC
 * @file lightcurveMC/mcio.cpp
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified May 28, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include "except/fileio.h"
#include "mcio.h"

using std::sort;
using boost::shared_ptr;

/** Reads a file containing timestamps into a vector of dates
 *
 * @param[in] hInput an open file handle to be read. The file represented by @p hInput 
 *	is assumed to be formatted as a list of floating-point values, 
 *	one per line. 
 * @param[out] dates a vector of doubles that stores the timestamps in @p hInput. The dates 
 *	will be sorted in ascending order.
 * @param[out] minDelT a floating-point number that stores the smallest difference 
 *	between two elements of dates
 * @param[out] maxDelT a floating-point number that stores the largest difference 
 *	between two elements of dates
 *
 * @post @p dates does not contain any NaNs
 * @post neither @p minDelT nor @p maxDelT are NaNs
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store the 
 *	file contents.
 * @exception lcmc::except::FileIo Thrown if file has the wrong format.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception. 
 */
void readTimeStamps(FILE* hInput, DoubleVec& dates, double& minDelT, double& maxDelT) {
	dates.clear();

	// Read into dates
	double buffer;
	int nRead;
	while(!feof(hInput)) {
		nRead = fscanf(  hInput, "%lf\n", &buffer);
		if(nRead < 1/* || ferror(hInput) || nRead == EOF*/) {
			fileError(hInput, "Misformatted time stamp file: ");
		}
		dates.push_back(buffer);
	}
	sort(dates.begin(), dates.end());

	// Clean up the dates to make them easier to work with
	// While we're at it, find the minimum and maximum intervals
	double last = 0.0;
	int count = 0;
	// Default values in case dates.size() == 1
	minDelT = maxDelT = 0.0;
	for(DoubleVec::iterator it = dates.begin(); it != dates.end(); it++) {
		if (count > 1) {
			// Update minDelT
			if (*it - last < minDelT) {
				minDelT = *it - last;
			}
		}
		else if (count == 1) {
			// Initialize minDelT
			minDelT = *it - last;
		}
		last = *it;
		count++;
	}
	maxDelT = dates.back() - dates.front();
}

/** Reads a file containing timestamps into a vector of dates
 *
 * @param[in] hInput an open file handle to be read. The file represented by @p hInput 
 *	is assumed to be formatted as a list of floating-point values, 
 *	one per line. 
 * @param[out] dates a vector of doubles that stores the timestamps in @p hInput. The dates 
 *	will be sorted in ascending order.
 *
 * @post @p dates does not contain any NaNs
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store the 
 *	file contents.
 * @exception lcmc::except::FileIo Thrown if file has the wrong format.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception. 
 */
void readTimeStamps(FILE* hInput, DoubleVec& dates) {
	// To be discarded
	double minTemp, maxTemp;
	
	readTimeStamps(hInput, dates, minTemp, maxTemp);
}

/** Dumps the contents of a lightcurve to a file
 *
 * @param[in] fileName The name of the file to which to log the light curve
 * @param[in] timeGrid a vector containing timestamps to copy to disk
 * @param[in] fluxGrid a vector containing fluxes to copy to disk. Assumed to be the 
 *	same length as @p timeGrid.
 *
 * @pre @p timeGrid does not contain any NaNs
 * @pre @p fluxGrid does not contain any NaNs
 *
 * @exception lcmc::except::FileIo Thrown if file has the wrong format.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception. 
 *
 * @bug Write more (flux) digits!
 */
void printLightCurve(const std::string& fileName, 
		const DoubleVec& timeGrid, const DoubleVec& fluxGrid) {
	shared_ptr<FILE> hOutput = fileCheckOpen(fileName, "w");

	// Print the table
	int status = fprintf(hOutput.get(), "#Time\tFlux\n");
	if (status < 0) {
		fileError(hOutput.get(), "Could not write to file '" 
			+ fileName + "' in printLightCurve(): ");
	}
	for(size_t i = 0; i < timeGrid.size(); i++) {
		status = fprintf(hOutput.get(), "%0.5f\t%7.4f\n", timeGrid[i], fluxGrid[i]);
		if (status < 0) {
			fileError(hOutput.get(), "Could not write to file '" 
				+ fileName + "' in printLightCurve(): ");
		}
	}
}

/** Wrapper that throws @ref lcmc::except::FileIo "FileIo" if it cannot open a file
 *
 * @param[in] filename The file to open.
 * @param[in] mode The mode to open the file, following the same 
 *	conventions as for @c fopen()
 *
 * @return A handle to the newly opened file.
 *
 * @exception lcmc::except::FileIo Thrown if the file could not be opened.
 * 
 * @exceptsafe The function arguments are unchanged in the event of an exception.
 */
shared_ptr<FILE> fileCheckOpen(const std::string& fileName, const char* mode) {
	FILE* handle = fopen(fileName.c_str(), mode);
	
	if (handle == NULL) {
		// Not all implementations of fopen() set errno on an error
		if (errno) {
			int err = errno;
			errno = 0;
			throw lcmc::except::FileIo("Could not open " 
				+ fileName + ": " + strerror(err));
		} else {
			throw lcmc::except::FileIo("Could not open " + fileName);
		}
	}
	
	return shared_ptr<FILE>(handle, &fclose);
}

/** Wrapper that throws @ref lcmc::except::FileIo "FileIo" in response to 
 *	a C I/O library error
 *
 * @param[in] stream The stream to test for errors
 * @param[in] msg A string prepended to the error message.
 *
 * @exception lcmc::except::FileIo Thrown if @c ferror(@p stream)
 * 
 * @exceptsafe The function arguments are unchanged in the event of an exception.
 */
void fileError(FILE* const stream, std::string msg) {
	if (ferror(stream) || feof(stream)) {
		int err = errno;
		clearerr(stream);
		errno = 0;
		throw lcmc::except::FileIo(msg + strerror(err));
	}
}
