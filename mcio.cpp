// mcio.cpp
// by Krzysztof Findeisen
// Created February 4, 2011
// Last modified February 4, 2011
// Input and output routines for lightcurveMC

#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <kpffileio.h>
#include "mcio.h"

using namespace std;

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
 */
void readTimeStamps(FILE* hInput, DoubleVec &dates, double &minDelT, double &maxDelT)
{
	dates.clear();

	// Read into dates
	double buffer;
	int nRead;
	while(!feof(hInput)) {
		#if _WIN32
		nRead = fscanf_s(hInput, "%lf\n", &buffer);
		#else
		nRead = fscanf(  hInput, "%lf\n", &buffer);
		#endif
		if(nRead < 1 || ferror(hInput) || nRead == EOF) {
			throw runtime_error("Misformatted time stamp file");
		}
		dates.push_back(buffer);
	}
	sort(dates.begin(), dates.end());

	// Clean up the dates to make them easier to work with
	// While we're at it, find the minimum and maximum intervals
//	offset = 1000.0*floor(dates[0] / 1000.0);
	double last = 0.0;
	int count = 0;
	// Default values in case dates.size() == 1
	minDelT = maxDelT = 0.0;
	for(DoubleVec::iterator it = dates.begin(); it != dates.end(); it++) {
//		*it -= offset;
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
 * @param[in] lcType, logP, logA a complete identifier for the model that generated 
 *	the lightcurve
 * @param[in] modelNum a unique identifier distinguishing different instances of 
 *	identical models
 * @param[in] period the true period of the lightcurve (for reference)
 * @param[in] timeGrid a vector containing timestamps to copy to disk
 * @param[in] fluxGrid a vector containing fluxes to copy to disk. Assumed to be the 
 *	same length as timeGrid.
 *
 * @pre timeGrid does not contain any NaNs
 * @pre fluxGrid does not contain any NaNs
 */	
void printLightCurve(size_t modelNum, string lcName, double logP, double logA, 
	double period, DoubleVec timeGrid, DoubleVec fluxGrid)
{
	char filename[50];
	#ifdef _WIN32
	sprintf_s(filename, 50, "lightcurve_%s_p%+0.1f_a%+0.1f_%02li.dat", 
		lcName.c_str(), logP, logA, modelNum);
	#else
	sprintf(filename, "lightcurve_%s_p%+0.1f_a%+0.1f_%02i.dat", 
		lcName.c_str(), logP, logA, modelNum);
	#endif
	FILE *hOutput = kpffileio::bestFileOpen(filename, "w");

	// Print the FAP value
	fprintf(hOutput, "Period %7.4f d\n", period);
	// Print the table
	fprintf(hOutput, "#Time\tFlux\n");
	for(size_t i = 0; i < timeGrid.size(); i++) {
		fprintf(hOutput, "%0.5f\t%7.4f\n", timeGrid[i], fluxGrid[i]);
	}

	fclose(hOutput);
}

/** Dumps the contents of a periodogram to a file
 *
 * @param[in] lcType, logP, logA a complete identifier for the model that generated 
 *	the periodogram
 * @param[in] modelNum a unique identifier distinguishing different instances of 
 *	identical models
 * @param[in] fap the false alarm probability used to search for significant 
 *	peaks (for reference)
 * @param[in] threshold the height of the smallest peak that would be 
 *	considered significant (for reference)
 * @param[in] omega the true frequency of the lightcurve (for reference)
 * @param[in] freqGrid a vector containing frequencies to copy to disk
 * @param[in] powerGrid a vector containing powers to copy to disk. Assumed to be the 
 *	same length as freqGrid.
 *
 * @pre timeGrid does not contain any NaNs
 * @pre fluxGrid does not contain any NaNs
 */	
void printPeriodogram(size_t modelNum, size_t lcType, double logP, double logA, 
	double fap, double threshold, double freq, DoubleVec freqGrid, DoubleVec powerGrid)
{
	char filename[40];
	#ifdef _WIN32
	sprintf_s(filename, 40, "periodogram_lc%1i_p%+.2f_a%+.2f_%1i.dat", 
		lcType, logP, logA, modelNum);
	#else
	sprintf(filename, "periodogram_lc%1i_p%+.2f_a%+.2f_%1i.dat", 
		lcType, logP, logA, modelNum);
	#endif
	FILE *hOutput = kpffileio::bestFileOpen(filename, "w");

	// Print the FAP value
	fprintf(hOutput, "True Frequency %7.4f\n", freq);
	fprintf(hOutput, "FAP %.1g%% above %7.1f\n", fap*100.0, threshold);
	// Print the table
	fprintf(hOutput, "Freq\tPower\n");
	for(size_t i = 0; i < freqGrid.size(); i++) {
		fprintf(hOutput, "%7.4f\t%7.4f\n", freqGrid[i], powerGrid[i]);
	}

	fclose(hOutput);
}
