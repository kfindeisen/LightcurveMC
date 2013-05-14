/** Input and output routines for lightcurveMC
 * @file mcio.cpp
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified May 14, 2013
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
 * @param[in] hInput an open file handle to be read. The file represented by hInput 
 *	is assumed to be formatted as a list of floating-point values, 
 *	one per line. If the file does not match this format the 
 *	function throws an exception.
 * @param[out] dates a vector of doubles that stores the timestamps in hInput. The dates 
 *	will be sorted in ascending order.
 * @param[out] minDelT a floating-point number that stores the smallest difference 
 *	between two elements of dates
 * @param[out] maxDelT a floating-point number that stores the largest difference 
 *	between two elements of dates
 *
 * @post dates does not contain any NaNs
 * @post neither minDelT nor maxDelT are NaNs
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
		if(nRead < 1 || ferror(hInput) || nRead == EOF) {
			throw lcmc::except::FileIo("Misformatted time stamp file");
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

/** Dumps the contents of a lightcurve to a file
 *
 * @param[in] fileName The name of the file to which to log the light curve
 * @param[in] timeGrid a vector containing timestamps to copy to disk
 * @param[in] fluxGrid a vector containing fluxes to copy to disk. Assumed to be the 
 *	same length as timeGrid.
 *
 * @pre timeGrid does not contain any NaNs
 * @pre fluxGrid does not contain any NaNs
 *
 * @exception lcmc::except::FileIo Thrown if file has the wrong format.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception. 
 */
void printLightCurve(const std::string& fileName, 
		const DoubleVec& timeGrid, const DoubleVec& fluxGrid) {
	shared_ptr<FILE> hOutput(fopen(fileName.c_str(), "w"), &fclose);
	if (NULL == hOutput.get()) {
		throw lcmc::except::FileIo("Could not open light curve output file '" 
			+ fileName + "' in printLightCurve(): " + strerror(errno));
	}

	// Print the table
	int status = fprintf(hOutput.get(), "#Time\tFlux\n");
	if (status < 0) {
		throw lcmc::except::FileIo("Could not write to file '" 
			+ fileName + "' in printLightCurve(): " + strerror(errno));
	}
	for(size_t i = 0; i < timeGrid.size(); i++) {
		status = fprintf(hOutput.get(), "%0.5f\t%7.4f\n", timeGrid[i], fluxGrid[i]);
		if (status < 0) {
			throw lcmc::except::FileIo("Could not write to file '" 
				+ fileName + "' in printLightCurve(): " + strerror(errno));
		}
	}
}
