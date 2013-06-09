/** Implementation of Observations
 * @file lightcurveMC/samples/observations.cpp
 * @author Krzysztof Findeisen
 * @date Created May 4, 2012
 * @date Last modified June 8, 2013
 */

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_rng.h>
#include "../fluxmag.h"
#include "observations.h"
#include "../except/fileio.h"
#include "../gsl_compat.tmp.h"
#include "../except/inject.h"
#include "../lcsio.h"
#include "../mcio.h"
#include "../nan.h"
#include "../utils.tmp.h"

namespace lcmc { namespace inject {

/** Initializes the object to represent a randomly selected light 
 * curve from the selected catalog.
 *
 * @param[in] catalogName A text file where each line is a path 
 *	to a text file containing the light curve of a source in 
 *	the sample.
 * 
 * @exception lcmc::inject::except::NoCatalog Thrown if the catalog file does not exist.
 * @exception lcmc::except::FileIo Thrown if the catalog or the light curve 
 *	could not be read.
 * @exception std::bad_alloc Thrown if there is not enough memory to create the object.
 *
 * @exceptsafe Object construction is atomic.
 */
Observations::Observations(const std::string& catalogName) : times(), fluxes() {
	// Note: because sourcePicker is static, no memory management is needed
	static gsl_rng * sourcePicker = NULL;
	if (sourcePicker == NULL) {
		sourcePicker = gsl_rng_alloc(gsl_rng_mt19937);
		gsl_rng_set(sourcePicker, 5489);
	}

	// Grab the light curves in the sample
	const std::vector<std::string> library = getLcLibrary(catalogName);
	
	if (library.size() == 0) {
		throw lcmc::except::FileIo("Catalog " + catalogName + " does not contain any light curves.");
	}
	
	// copy-and-swap the generator state, to ensure it only changes 
	//	if the object is successfully constructed
	gsl_rng * tempRng = utils::checkAlloc(gsl_rng_clone(sourcePicker));
	
	// gsl_rng_uniform_int() generates over [0, n), not [0, n]
	unsigned long int index = gsl_rng_uniform_int(tempRng, library.size());
	
	try {
		readFile(library.at(index));
	} catch (const except::BadFile& e) {
		throw lcmc::except::FileIo(e.what());
	} catch (const except::BadFormat& e) {
		throw lcmc::except::FileIo(e.what());
	}
	
	// IMPORTANT: no exceptions beyond this point
	
	gsl_rng_memcpy(sourcePicker, tempRng);
}

/** Initializes an object to the value of a particular light curve.
 *
 * @param[in] fileName Path to a text file containing a single light 
 * curve. Currently only space-delimited tables in the format (JD, 
 * flux, error) are supported. Support for additional formats will be 
 * added later.
 *
 * @post times does not contain NaNs
 * @post fluxes does not contain NaNs
 * 
 * @exception lcmc::inject::except::BadFile Thrown if the file could not 
 *	be opened.
 * @exception lcmc::inject::except::BadFormat Thrown if the file does not 
 *	conform to the expected format.
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the light curve.
 *
 * @exceptsafe This object and the function arguments are unchanged 
 *	in the event of an exception.
 */
void Observations::readFile(const std::string& fileName) {
	using std::swap;

	boost::shared_ptr<FILE> hLightCurve;
	try {
		hLightCurve = fileCheckOpen(fileName, "r");
	} catch(const lcmc::except::FileIo& e) {
		throw except::BadFile(e.what(), fileName);
	}
	
	// Use the existing interface
	// Temporary variables for exception safety
	double offset;
	std::vector<double> dummyFluxes, dummyTimes, dummyErrors;
	try {
		readWgLightCurve(hLightCurve.get(), 0.1, offset, dummyTimes, 
			dummyFluxes, dummyErrors);
	} catch (const std::runtime_error e) {
		// Convert to proper exception type
		// readWgLightCurve doesn't know the filename, so add that in
		throw except::BadFormat("While reading " + fileName + ": " 
			+ e.what(), "<not available>");
	}
	
	// Add the offset back in -- it was a crappy idea anyway
	for(std::vector<double>::iterator it = dummyTimes.begin(); 
			it != dummyTimes.end(); it++) {
		*it += offset;
	}
	
	// Convert from magnitudes to fluxes
	utils::magToFlux(dummyFluxes, dummyFluxes);
	
	// Remove NaNs
	std::vector<double> cleanTimes, cleanFluxes;
	for(size_t i = 0; i < dummyTimes.size(); i++) {
		if (!utils::isNan(dummyTimes[i]) && !utils::isNan(dummyFluxes[i])) {
			cleanTimes.push_back(dummyTimes[i]);
			cleanFluxes.push_back(dummyFluxes[i]);
		}
	}
	
	// Normalize to a median flux of 1
	// assert: all fluxes strictly positive because transformed from magnitudes
	double median = kpfutils::quantile(cleanFluxes.begin(), cleanFluxes.end(), 0.5);
	for(std::vector<double>::iterator it = cleanFluxes.begin(); 
			it != cleanFluxes.end(); it++) {
		(*it) /= median;
	}
	
	// Non-throwing swap
	swap(this->times , cleanTimes );
	swap(this->fluxes, cleanFluxes);
}

/** Returns the list of files from which the class may select sources
 *
 * @param[in] catalogName A text file where each line is a path 
 *	to a text file containing the light curve of a source in 
 *	the sample.
 *
 * @return A vector whose every entry is a path to a text file containing 
 *	the light curve of a source in the sample. The vector may be empty 
 *	if catalogName does not contain any valid lines.
 * 
 * @exception lcmc::inject::except::NoCatalog Thrown if the catalog file 
 *	does not exist.
 * @exception lcmc::except::FileIo Thrown if the catalog could not 
 *	be read.
 * @exception std::bad_alloc Thrown if there is not enough memory to store 
 *	the catalog.
 *
 * @exceptsafe The arguments are unchanged in the event of an exception.
 *
 * @bug Returns file paths relative to working directory, not relative to catalog.
 */
const std::vector<std::string> Observations::getLcLibrary(const std::string& catalogName) {

	std::vector<std::string> fileList;
	
	boost::shared_ptr<FILE> hCatalog;
	try {
		hCatalog = fileCheckOpen(catalogName, "r");
	} catch (const lcmc::except::FileIo& e) {
		throw except::NoCatalog(e.what(), catalogName);
	}

	// Read the catalog line by line
	/** @todo Add some format checking later!
	 */
	while(!feof(hCatalog.get())) {
		char textBuffer[128];
		fgets(textBuffer, 128, hCatalog.get());
		if (ferror(hCatalog.get())) {
			fileError(hCatalog.get(), "Error while reading " + catalogName + ": ");
		}
		// Remove trailing whitespace!
		std::string strBuffer(textBuffer);
		while(strBuffer.size() > 0 && *(strBuffer.rbegin()) == '\n') {
			// erase() only works with forward iterators
			// Size check guarantees that there is an iterator before end()
			strBuffer.erase(--strBuffer.end());
		}
		if (strBuffer.size() > 0) {
			fileList.push_back(strBuffer);
		}
	}
	
	return fileList;
}

/** Returns the timestamps associated with this source.
 *
 * @param[in] timeArray the array that will contain the timestamps
 * 
 * @post @p timeArray does not contain NaNs
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to copy 
 *	the timestamps.
 *
 * @exceptsafe Neither the function argument nor the object are changed in 
 *	the event of an exception.
 */
void Observations::getTimes(std::vector<double> &timeArray) const {
	using std::swap;

	// copy-and-swap to allow atomic guarantee
	// vector::= only offers the basic guarantee
	std::vector<double> temp = this->times;
	
	swap(timeArray, temp);
}

/** Returns the flux measurements associated with this source.
 *
 * @param[in] fluxArray the array that will contain the measurements
 * 
 * @post @p fluxArray does not contain NaNs
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to copy 
 *	the fluxes.
 *
 * @exceptsafe Neither the function argument nor the object are changed in 
 *	the event of an exception.
 */
void Observations::getFluxes(std::vector<double> &fluxArray) const {
	using std::swap;

	// copy-and-swap to allow atomic guarantee
	// vector::= only offers the basic guarantee
	std::vector<double> temp = this->fluxes;
	
	swap(fluxArray, temp);
}

}}		// end lcmc::inject
