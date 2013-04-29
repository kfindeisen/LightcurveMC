/** Definition for NonSpitzerVar class
 * @file obs_nonspitzervar.cpp
 * @author Krzysztof Findeisen
 * @date Created May 9, 2012
 * @date Last modified April 3, 2013
 */

#include <string>
#include <vector>
#include <cstring>
#include <gsl/gsl_rng.h>
#include "obssamples.h"

namespace lcmc { namespace inject {

/** Initializes the object to represent a randomly selected light 
 * curve meeting the sample criteria.
 */
NonSpitzerVar::NonSpitzerVar() : Observations() {
	// Note: because sourcePicker is static, no memory management is needed
	static gsl_rng * const sourcePicker = gsl_rng_alloc(gsl_rng_mt19937);

	// Grab a reference to the light curves in the sample
	const std::vector<std::string> library = getLcLibrary();
	
	unsigned long int index = gsl_rng_uniform_int(sourcePicker, library.size());
	
	this->init(library.at(index));
}

/** Returns the list of files from which the class may select sources
 */
const std::vector<std::string>& NonSpitzerVar::getLcLibrary() {
	static bool initialized = false;
	static std::vector<std::string> fileList;
	
	if (!initialized) {
		FILE* hCatalog = fopen("nonspitzervar.cat", "r");
		if (hCatalog == NULL) {
			throw FileIo("Could not open file 'nonspitzervar.cat'.");
		}
		// Read the catalog line by line
		/** @todo Add some format checking later!
		 */
		while(!feof(hCatalog)) {
			char textBuffer[128];
			fgets(textBuffer, 128, hCatalog);
			if (ferror(hCatalog)) {
				throw FileIo(std::string("Error while reading nonspitzervar.cat: ") + strerror(ferror(hCatalog)));
			}
			// Remove trailing whitespace!
			std::string strBuffer(textBuffer);
			while(*(strBuffer.rbegin()) == '\n') {
				strBuffer.erase(--strBuffer.end());
			}
			fileList.push_back(strBuffer);
		}
		fclose(hCatalog);
		
		initialized = true;
	}
	
	// assert: fileList is properly defined
	return fileList;
}

}}		// end lcmc::models
