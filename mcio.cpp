/** Input and output routines for Lightcurve MC
 * @file lightcurveMC/mcio.cpp
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified June 18, 2013
 */

#include <algorithm>
#include <string>
#include <vector>
#include <cstdio>
#include <boost/smart_ptr.hpp>
#include "../common/cerror.h"
#include "../common/alloc.tmp.h"
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
 * @exception kpfutils::except::FileIo Thrown if file has the wrong format.
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
			kpfutils::fileError(hInput, "Misformatted time stamp file: ");
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
 * @exception kpfutils::except::FileIo Thrown if file has the wrong format.
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
 * @exception kpfutils::except::FileIo Thrown if file has the wrong format.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception. 
 *
 * @bug Write more (flux) digits!
 */
void printLightCurve(const std::string& fileName, 
		const DoubleVec& timeGrid, const DoubleVec& fluxGrid) {
	shared_ptr<FILE> hOutput = kpfutils::fileCheckOpen(fileName, "w");

	// Print the table
	int status = fprintf(hOutput.get(), "#Time\tFlux\n");
	if (status < 0) {
		kpfutils::fileError(hOutput.get(), "Could not write to file '" 
			+ fileName + "' in printLightCurve(): ");
	}
	for(size_t i = 0; i < timeGrid.size(); i++) {
		status = fprintf(hOutput.get(), "%0.5f\t%7.4f\n", timeGrid[i], fluxGrid[i]);
		if (status < 0) {
			kpfutils::fileError(hOutput.get(), "Could not write to file '" 
				+ fileName + "' in printLightCurve(): ");
		}
	}
}
