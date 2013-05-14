/** File I/O routines for lightcurve analysis tool
 * @file lcsio.cpp
 * @author Krzysztof Findeisen
 * @date Created February 6, 2011
 * @date Last modified May 4, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include "lcsio.h"

using namespace std;

/** Tests whether a character is a newline or not
 *
 * @param[in] x The character to test
 *
 * @return True if x represents a newline on Windows, Unix, or MacOS.
 *
 * @exceptsafe Does not throw exceptions.
 */
bool isNewLine(char x) {
	switch (x) {
	case '\r':
	case '\n':
		return true;
	default:
		return false;
	}
}

/** Reads a file containing a list of file names
 * 
 * @param[in] hInput an open file handle to be read. The file represented 
 *	by hInput is assumed to be formatted as a list of strings, one per 
 *	line. If the file does not match this format the function throws 
 *	std::runtime_error.
 *@param[out] fileList a list of strings that stores the filenames in 
 *	hInput. The list may be empty.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the file list.
 * @exception std::runtime_error Thrown if the file has the wrong format.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */	
void readFileNames(FILE* hInput, vector<string> &fileList) {
	using boost::algorithm::trim_right_copy_if;
	
	fileList.clear();

	// Read into dates
	char buffer[80];
	while(!feof(hInput)) {
		if(ferror(hInput)) {
			throw runtime_error("Misformatted file");
		}
		// Empty line
		if(NULL == fgets(buffer, 80, hInput)) {
			continue;
		}
		// Commented line
		if(buffer[0] == '#') {
			continue;
		}
		// use strtrim to remove any trailing newline
		// be careful not to remove spaces, lest there be a 
		//	pathological filename
		fileList.push_back(trim_right_copy_if(string(buffer), &isNewLine));
	}
}


/** Reads a file containing a list of julian days, magnitudes, and errors
 * 
 * @param[in] hInput an open file handle to be read. The file represented 
 *	by hInput is assumed to be formatted as a 3xN space-delimited table, 
 *	with the first column a floating point Julian date, the second 
 *	a floating point magnitude, and the third a floating point 
 *	magnitude error. The file may also contain comment lines 
 *	preceded by '#'. If the file does not match this format the 
 *	function throws std::runtime_error. The data are returned 
 *	sorted by increasing Julian date.
 * @param[in] errMax the maximum magnitude error to tolerate in a data 
 *	point. Any points with an error exceeding errMax are ignored.
 * @param[out] offset a Julian date to be added to all values in timeVec. This 
 *		lets subsequent functions work with smaller numbers.
 * @param[out] timeVec a [not yet sorted] vector containing the times of each 
 *		observation, minus offset
 * @param[out] fluxVec a vector containing the flux observed at each time
 * @param[out] errVec a vector containing the error on each flux
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the data.
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */	
void readWgLightCurve(FILE* hInput, double errMax, double &offset, DoubleVec &timeVec, 
		DoubleVec &fluxVec, DoubleVec &errVec) {
	timeVec.clear();
	fluxVec.clear();
	errVec .clear();
	
	// Read into dates
	double timeBuffer, magBuffer, errBuffer;
	char lineBuffer[256], commentTest;
	int nRead;
	fpos_t curLine;
	while(!feof(hInput)) {
		// Save start-of-line
		if (0 != fgetpos(hInput, &curLine)) {
			throw runtime_error("");
		}
		
		// Test whether it's a comment line
		// Grab the first NON-WHITESPACE character
		nRead = fscanf(  hInput, " %1c\n", &commentTest);
		if(nRead == EOF) {
			break;
		} else if(nRead < 1 || ferror(hInput)) {
			throw runtime_error("Unexpected end of file.");
		}
		
		if ('#' == commentTest) {
			// Skip to the next line
			if(NULL == fgets (lineBuffer, 256, hInput)) {
				throw runtime_error("Unexpected end of file.");
			}
		} else {
			// Not a comment line, so start over and read it in
			if (0 != fsetpos(hInput, &curLine)) {
				throw runtime_error("Could not read data line.");
			}
			nRead = fscanf(  hInput, " %lf %lf %lf\n", 
				&timeBuffer, &magBuffer, &errBuffer);
			if(nRead < 1 || ferror(hInput) || nRead == EOF) {
				throw runtime_error("Misformatted file.");
			}
			// Valid photometry only
			if (errBuffer <= errMax) {
				// use atomic guarantee for vector::reserve to ensure 
				// that a bad_alloc doesn't interrupt the addition 
				// halfway through
				timeVec.reserve(timeVec.size() + 1);
				fluxVec.reserve(fluxVec.size() + 1);
				 errVec.reserve( errVec.size() + 1);
				timeVec.push_back(timeBuffer);
				fluxVec.push_back( magBuffer);
				errVec .push_back( errBuffer);
			}
		}
	}
	//sort(timeVec.begin(), timeVec.end());

	// Clean up the dates to make them easier to work with
	if (timeVec.size() > 0) {
		//offset = 1000.0*floor(timeVec[0] / 1000.0);
		offset = 1000.0*floor(*min_element(timeVec.begin(), timeVec.end()) / 1000.0);
		for(DoubleVec::iterator it = timeVec.begin(); it != timeVec.end(); it++) {
			*it -= offset;
		}
	} else {
		offset = 0.0;
	}

	// Turn the magnitudes into fluxes, ditto with errors
	/*{
		const static double fourLnTen = 0.4 * log(10.0);
		DoubleVec::iterator flxIt = fluxVec.begin();
		DoubleVec::iterator errIt =  errVec.begin();
		while(flxIt != fluxVec.end()) {
			*flxIt = pow(10.0, -0.4* (*flxIt));
			*errIt = fourLnTen * (*flxIt) * (*errIt);
			flxIt++;
			errIt++;
		}
	}*/
}

/** Reads a file containing a list of obsids, julian days, magnitudes, errors, and limits
 * 
 * @param[in] hInput an open file handle to be read. The file represented by hInput 
 *	is assumed to be formatted as a 5xN space-delimited table, with 
 *	the first column a running index, the second a floating point 
 *	Julian date, the third a floating point magnitude, the fourth 
 *	a floating point magnitude error, and the fifth a limit. The 
 *	file may also contain comment lines preceded by '#'. If the 
 *	file does not match this format the function throws 
 *	std::runtime_error. The data are returned sorted by 
 *	increasing Julian date.
 * @param[in] errMax the maximum magnitude error to tolerate in a data point. Any 
 *	points with an error exceeding errMax are ignored.
 * @param[out] offset a Julian date to be added to all values in timeVec. This lets 
 *	subsequent functions work with smaller numbers.
 * @param[out] timeVec a [not yet sorted] vector containing the times of each 
 *	observation, minus offset
 * @param[out] fluxVec a vector containing the flux observed at each time
 * @param[out] errVec a vector containing the error on each flux
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the data.
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */
void readWg2LightCurve(FILE* hInput, double errMax, double &offset, DoubleVec &timeVec, 
		DoubleVec &fluxVec, DoubleVec &errVec) {
	timeVec.clear();
	fluxVec.clear();
	errVec .clear();
	
	// Read into dates
	double timeBuffer, magBuffer, errBuffer, limBuffer;
	char lineBuffer[256], commentTest;
	int nRead;
	fpos_t curLine;
	while(!feof(hInput)) {
		// Save start-of-line
		if (0 != fgetpos(hInput, &curLine)) {
			throw runtime_error("");
		}
		
		// Test whether it's a comment line
		// Grab the first NON-WHITESPACE character
		nRead = fscanf(  hInput, " %1c\n", &commentTest);
		if(nRead == EOF) {
			break;
		} else if(nRead < 1 || ferror(hInput)) {
			throw runtime_error("Unexpected end of file.");
		}
		
		if ('#' == commentTest) {
			// Skip to the next line
			if(NULL == fgets (lineBuffer, 256, hInput)) {
				throw runtime_error("Unexpected end of file.");
			}
		} else {
			// Not a comment line, so start over and read it in
			if (0 != fsetpos(hInput, &curLine)) {
				throw runtime_error("Could not read data line.");
			}
			nRead = fscanf(  hInput, " %*i %lf %lf %lf %lf", 
				&timeBuffer, &magBuffer, &errBuffer, &limBuffer);
			if(nRead < 1 || ferror(hInput) || nRead == EOF) {
				throw runtime_error("Misformatted file.");
			}
			// Valid photometry only
			if (errBuffer <= errMax && magBuffer < limBuffer) {
				// use atomic guarantee for vector::reserve to ensure 
				// that a bad_alloc doesn't interrupt the addition 
				// halfway through
				timeVec.reserve(timeVec.size() + 1);
				fluxVec.reserve(fluxVec.size() + 1);
				 errVec.reserve( errVec.size() + 1);
				timeVec.push_back(timeBuffer);
				fluxVec.push_back( magBuffer);
				errVec .push_back( errBuffer);
			}
		}
	}
	//sort(timeVec.begin(), timeVec.end());
	// Clean up the dates to make them easier to work with
	if (timeVec.size() > 0) {
		//offset = 1000.0*floor(timeVec[0] / 1000.0);
		offset = 1000.0*floor(*min_element(timeVec.begin(), timeVec.end()) / 1000.0);
		for(DoubleVec::iterator it = timeVec.begin(); it != timeVec.end(); it++) {
			*it -= offset;
		}
	} else {
		offset = 0.0;
	}

	// Turn the magnitudes into fluxes, ditto with errors
	/*{
		const static double fourLnTen = 0.4 * log(10.0);
		DoubleVec::iterator flxIt = fluxVec.begin();
		DoubleVec::iterator errIt =  errVec.begin();
		while(flxIt != fluxVec.end()) {
			*flxIt = pow(10.0, -0.4* (*flxIt));
			*errIt = fourLnTen * (*flxIt) * (*errIt);
			flxIt++;
			errIt++;
		}
	}*/
}

/** Reads a file containing a list of julian days and fluxes
 * 
 * @param[in] hInput an open file handle to be read. The file represented by hInput 
 *	is assumed to be formatted as a 3xN space-delimited table, 
 *	with the first column a floating point Julian date, the second 
 *	a floating point magnitude, and the third a floating point 
 *	magnitude error. The file may also contain comment lines 
 *	preceded by '#'. If the file does not match this format the 
 *	function throws std::runtime_error. The data are returned 
 *	sorted by increasing Julian date.
 * @param[out] offset a Julian date to be added to all values in timeVec. This lets 
 *	subsequent functions work with smaller numbers.
 * @param[out] timeVec a [not yet sorted] vector containing the times of each 
 *	observation, minus offset
 * @param[out] fluxVec a vector containing the flux observed at each time
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the data.
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */
void readMcLightCurve(FILE* hInput, double &offset, DoubleVec &timeVec, 
		DoubleVec &fluxVec) {
	timeVec.clear();
	fluxVec.clear();
	
	// Read into dates
	double timeBuffer, fluxBuffer;
	char lineBuffer[256], commentTest;
	int nRead;
	fpos_t curLine;
	// Skip 2 lines, no matter what
	for(int i = 0; i < 2; i++) {
		if(NULL == fgets (lineBuffer, 256, hInput)) {
			throw runtime_error("Unexpected end of header.");
		}
	}
	while(!feof(hInput)) {
		// Save start-of-line
		if (0 != fgetpos(hInput, &curLine)) {
			throw runtime_error("");
		}
		
		// Test whether it's a comment line
		// Grab the first NON-WHITESPACE character
		nRead = fscanf(  hInput, " %1c\n", &commentTest);
		if(nRead == EOF) {
			break;
		} else if(nRead < 1 || ferror(hInput)) {
			throw runtime_error("Unexpected end of file.");
		}
		
		if ('#' == commentTest) {
			// Skip to the next line
			if(NULL == fgets (lineBuffer, 256, hInput)) {
				throw runtime_error("Unexpected end of file.");
			}
		} else {
			// Not a comment line, so start over and read it in
			if (0 != fsetpos(hInput, &curLine)) {
				throw runtime_error("Could not read data line.");
			}
			nRead = fscanf(  hInput, " %lf %lf\n", 
				&timeBuffer, &fluxBuffer);
			if(nRead < 1 || ferror(hInput) || nRead == EOF) {
				throw runtime_error("Misformatted file.");
			}
			// use atomic guarantee for vector::reserve to ensure 
			// that a bad_alloc doesn't interrupt the addition 
			// halfway through
			timeVec.reserve(timeVec.size() + 1);
			fluxVec.reserve(fluxVec.size() + 1);
			timeVec.push_back(timeBuffer);
			fluxVec.push_back(fluxBuffer);
		}
	}
	//sort(timeVec.begin(), timeVec.end());

	// Clean up the dates to make them easier to work with
	offset = 1000.0*floor(*min_element(timeVec.begin(), timeVec.end()) / 1000.0);
	for(DoubleVec::iterator it = timeVec.begin(); it != timeVec.end(); it++) {
		*it -= offset;
	}
}

/** Reads a file containing a list of julian days and fluxes
 * 
 * @param[in] hInput an open file handle to be read. The file represented by hInput 
 *	is assumed to be formatted as a 3xN space-delimited table, 
 *	with the first column a floating point Julian date, the second 
 *	a floating point magnitude, and the third a floating point 
 *	magnitude error. The file may also contain comment lines 
 *	preceded by '#'. If the file does not match this format the 
 *	function throws std::runtime_error. The data are returned 
 *	sorted by increasing Julian date.
 * @param[out] offset a Julian date to be added to all values in timeVec. This lets 
 *	subsequent functions work with smaller numbers.
 * @param[out] timeVec a [not yet sorted] vector containing the times of each 
 *	observation, minus offset
 * @param[out] fluxVec a vector containing the flux observed at each time
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the data.
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */
void readCsvLightCurve(FILE* hInput, double &offset, DoubleVec &timeVec, 
		DoubleVec &fluxVec) {
	timeVec.clear();
	fluxVec.clear();
	
	// Read into dates
	double timeBuffer, fluxBuffer;
	char lineBuffer[256], commentTest;
	int nRead;
	fpos_t curLine;
	while(!feof(hInput)) {
		// Save start-of-line
		if (0 != fgetpos(hInput, &curLine)) {
			throw runtime_error("");
		}
		
		// Test whether it's a comment line
		// Grab the first NON-WHITESPACE character
		nRead = fscanf(  hInput, " %1c\n", &commentTest);
		if(nRead == EOF) {
			break;
		} else if(nRead < 1 || ferror(hInput)) {
			throw runtime_error("Unexpected end of file.");
		}
		
		if ('#' == commentTest) {
			// Skip to the next line
			if(NULL == fgets (lineBuffer, 256, hInput)) {
				throw runtime_error("Unexpected end of file.");
			}
		} else {
			// Not a comment line, so start over and read it in
			if (0 != fsetpos(hInput, &curLine)) {
				throw runtime_error("Could not read data line.");
			}
			if(NULL == fgets (lineBuffer, 256, hInput)) {
				throw runtime_error("Unexpected end of file.");
			}
			nRead = sscanf(  lineBuffer, " %lf , %lf", 
				&timeBuffer, &fluxBuffer);
			if(nRead < 1 || ferror(hInput) || nRead == EOF) {
				throw runtime_error(string("Misformatted file: ") + lineBuffer);
			}
			// use atomic guarantee for vector::reserve to ensure 
			// that a bad_alloc doesn't interrupt the addition 
			// halfway through
			timeVec.reserve(timeVec.size() + 1);
			fluxVec.reserve(fluxVec.size() + 1);
			timeVec.push_back(timeBuffer);
			fluxVec.push_back(fluxBuffer);
		}
	}
	//sort(timeVec.begin(), timeVec.end());

	// Clean up the dates to make them easier to work with
	offset = 1000.0*floor(*min_element(timeVec.begin(), timeVec.end()) / 1000.0);
	for(DoubleVec::iterator it = timeVec.begin(); it != timeVec.end(); it++) {
		*it -= offset;
	}
}

/** Prints a file containing a periodogram
 * 
 * @param[in] hOutput an open file handle to be written to
 * @param[in] freq a vector of frequencies at which the periodogram has been
 *	measured
 * @param[in] power a vector of powers, assumed to be the same length as 
 *	frequencies
 * @param[in] threshold the significance threshold
 * @param[in] fap the false alarm probability associated with threshold
 * 
 * @post Produces a text file containing two header lines in the format "THRESHOLD: #" 
 *	and "FAP: #", followed by two space-delimited columns 
 *	containing the frequencies and corresponding power.
 *
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */
void printPeriodogram(FILE* hOutput, const DoubleVec &freq, const DoubleVec &power, 
		double threshold, double fap) {
	// Print the FAP value
	if (fap < 0.05) {
		int status = fprintf(hOutput, "FAP %.1g%% above %7.1f\n", fap*100.0, threshold);
		if (status < 0) {
			throw std::runtime_error("Could not print periodogram header.");
		}
	} else {
		int status = fprintf(hOutput, "FAP %.0f%% above %7.1f\n", fap*100.0, threshold);
		if (status < 0) {
			throw std::runtime_error("Could not print periodogram header.");
		}
	}
	// Print the table
	int status = fprintf(hOutput, "Freq\tPower\n");
	if (status < 0) {
		throw std::runtime_error("Could not print periodogram column header.");
	}
	for(size_t i = 0; i < freq.size(); i++) {
		status = fprintf(hOutput, "%7.4f\t%7.4f\n", freq[i], power[i]);
		if (status < 0) {
			throw std::runtime_error("Could not print periodogram.");
		}
	}
}

/** Prints a file containing an autocorrelation function
 * 
 * @param[in] hOutput an open file handle to be written to
 * @param[in] times a vector of time offsets at which the ACF has been measured
 * @param[in] acf a vector of correlations, assumed to be the same length as times
 * 
 * @post Produces a text file containing two space-delimited columns containing the 
 *	offsets and corresponding autocorrelation.
 *
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */	
void printAcf(FILE* hOutput, const DoubleVec &times, const DoubleVec &acf)
{
	// Print the table
	int status = fprintf(hOutput, "Offset\tACF\n");
	if (status < 0) {
		throw std::runtime_error("Could not print ACF column header.");
	}
	for(size_t i = 0; i < times.size(); i++) {
		status = fprintf(hOutput, "%7.4f\t%7.4f\n", times[i], acf[i]);
		if (status < 0) {
			throw std::runtime_error("Could not print Acf.");
		}
	}
}

/** Prints a file containing a dmdt scatter plot
 * 
 * @param[in] hOutput an open file handle to be written to
 * @param[in] times a vector of time differences
 * @param[in] deltam a vector of time magnitude differences, assumed to be the 
 *	same length as deltam
 * 
 * @post Produces a text file containing two space-delimited columns containing the 
 *	time and magnitude offsets, respectively
 *
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */	
void printDmDt(FILE* hOutput, const DoubleVec &times, const DoubleVec &deltam)
{
	// Print the table
	int status = fprintf(hOutput, "Offset\tMag Diff.\n");
	if (status < 0) {
		throw std::runtime_error("Could not print dmdt column header.");
	}
	for(size_t i = 0; i < times.size(); i++) {
		status = fprintf(hOutput, "%7.4f\t%7.4f\n", times[i], deltam[i]);
		if (status < 0) {
			throw std::runtime_error("Could not print dmdt plot.");
		}
	}
}

/** Prints a file containing a histogram
 * 
 * @param[in] hOutput an open file handle to be written to
 * @param[in] binEdges a vector of starting and ending bin edges
 * @param[in] values a vector of bin heights, 1 shorter than binEdges
 * 
 * @post Produces a text file containing two space-delimited columns containing the 
 *	time and magnitude offsets, respectively
 *
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */	
void printHist(FILE* hOutput, const DoubleVec &binEdges, const DoubleVec &values) {
	// Print the table
	int status = fprintf(hOutput, "Bin Start\tValue\n");
	if (status < 0) {
		throw std::runtime_error("Could not print histogram column header.");
	}
	for(size_t i = 0; i < values.size(); i++) {
		status = fprintf(hOutput, "%7.4f\t%7.4f\n", binEdges[i], values[i]);
		if (status < 0) {
			throw std::runtime_error("Could not print histogram.");
		}
	}
	status = fprintf(hOutput, "%7.4f\n", binEdges[values.size()]);
	if (status < 0) {
		throw std::runtime_error("Could not print histogram.");
	}
}

/** Prints a file containing a rms vs. t scatter plot
 * 
 * @param[in] hOutput an open file handle to be written to
 * @param[in] times a vector of time differences
 * @param[in] rmsVals a vector of RMS scores, assumed to be the same length as times
 * 
 * @post Produces a text file containing two space-delimited columns containing the 
 *	time and RMS values, respectively
 *
 * @exception std::runtime_error Thrown if any file operation fails.
 *
 * @exceptsafe Program is in a consistent state in the event of an exception.
 */	
void printRmsT(FILE* hOutput, const DoubleVec &times, const DoubleVec &rmsVals)
{
	// Print the table
	int status = fprintf(hOutput, "Interval\tRMS\n");
	if (status < 0) {
		throw std::runtime_error("Could not print RMS header.");
	}
	for(size_t i = 0; i < times.size(); i++) {
		status = fprintf(hOutput, "%7.4f\t%7.4f\n", times[i], rmsVals[i]);
		if (status < 0) {
			throw std::runtime_error("Could not print RMS vs t plot.");
		}
	}
}

/** Filters a set of vectors to include only dates between date1 and date2
 *
 * @param[in] date1 the smallest Julian date to be allowed in times
 * @param[in] date2 the largest Julian date to be allowed in times
 * @param[in,out] times a vector of times to be trimmed to date1, date2, assumed 
 *	sorted in ascending order
 * @param[in,out] arr1 a vector of measurements corresponding to times, to be trimmed 
 *	in parallel
 * @param[in,out] arr2 a vector of measurements corresponding to times, to be trimmed 
 *	in parallel
 * 
 * @post times is updated to contain only items greater than or equal to date1 
 *	and less than or equal to date 2. Any entries corresponding to 
 *	deleted entries of times are deleted from arr1 and arr2
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	reprocess the vectors
 *
 * @exception The function arguments are unchanged in the event of an exception.
 */
void filterLightCurve(double date1, double date2, DoubleVec &times, 
		DoubleVec &arr1, DoubleVec &arr2) {
	DoubleVec::iterator timeStart=times.begin(), 
		arr1Start=arr1.begin(), arr2Start=arr2.begin();
	DoubleVec::iterator firstOk=times.end(), firstNotOk=times.end();
	
	// Find the range we want to keep
	for(DoubleVec::iterator curTime=timeStart; curTime != times.end(); curTime++) {
		if (*curTime >= date1) {
			firstOk = curTime;
			break;
		}
	}
	// firstOk may be equal to end(); that will work [giving no data]
	// date2 may be <= date1; that will also work [giving no data]
	for(DoubleVec::iterator curTime=firstOk; curTime != times.end(); curTime++) {
		if (*curTime > date2) {
			firstNotOk = curTime;
			break;
		}
	}
	
	// firstOk and lastOk are now random access iterators pointing 
	//	to the valid range of elements, 
	//	begin() <= firstOk <= firstNotOk <= end()
	// firstOk == firstNotOk ==> no valid numbers
	if (firstOk == firstNotOk) {
		throw runtime_error("No valid photometry");
	} else {
		using std::swap;
		
		size_t offsetNotOk = distance(timeStart, firstNotOk);
		size_t offsetOk    = distance(timeStart, firstOk   );
		
		// copy-and-swap
		vector<double> temp1(arr1), temp2(arr2), tempTimes(times);
		
		// Order matters to avoid invalidating the iterators
		//	Delete back, then front, and delete times last
		temp1    .erase(temp1    .begin()+offsetNotOk, temp1    .end());
		temp1    .erase(temp1    .begin()            , temp1    .begin()+offsetOk);
		temp2    .erase(temp2    .begin()+offsetNotOk, temp2    .end());
		temp2    .erase(temp2    .begin()            , temp2    .begin()+offsetOk);
		tempTimes.erase(tempTimes.begin()+offsetNotOk, tempTimes.end());
		tempTimes.erase(tempTimes.begin()            , tempTimes.begin()+offsetOk);
		
		swap(arr1, temp1);
		swap(arr2, temp2);
		swap(times, tempTimes);
	}
}
