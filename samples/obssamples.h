/** Type definitions for specific data samples
 * @file obssamples.h
 * @author Krzysztof Findeisen
 * @date Created May 4, 2012
 * @date Last modified April 3, 2013
 * 
 * These types represent specific data samples from which observations are to 
 * be drawn by the Observations class. The subsamples may overlap, and so may 
 * be extended as needed.
 */

#ifndef LCMCSAMPLESH
#define LCMCSAMPLESH

#include <string>
#include <vector>
#include "observations.h"

namespace lcmc { namespace inject {

/** Data generator for sources that are not IR excess sources in Spitzer and 
 * not variables in PTF.
 */
class NonSpitzerNonVar: public Observations {
public: 
	/** Initializes the object to represent a randomly selected light 
	 * curve meeting the sample criteria.
	 *
	 * @pre There exists a text file named "nonspitzernonvar.cat" in the 
	 * 	working directory. Each line in the file is the path to a text 
	 *	file containing the light curve of a source in thesample.
	 * 
	 * @exception FileIo Thrown if either the catalog file or the 
	 * 	selected light curve file do not exist.
	 * 
	 * @todo Fix exception spec to distinguish between catalog not found 
	 *	(which is unrecoverable) and source file not found (which is, 
	 *	at least in principle).
	 */
	NonSpitzerNonVar();
	
	// Virtual destructor following Effective C++
	virtual ~NonSpitzerNonVar() {};

private: 
	/** Returns the list of files from which the class may select sources
	 *
	 * @invariant getLcLibrary() is a constant expression
	 */
	static const std::vector<std::string>& getLcLibrary();
};

/** Data generator for sources that are IR excess sources in Spitzer but 
 * not variables in PTF.
 */
class SpitzerNonVar: public Observations {
public: 
	/** Initializes the object to represent a randomly selected light 
	 * curve meeting the sample criteria.
	 *
	 * @pre There exists a text file named "spitzernonvar.cat" in the 
	 * 	working directory. Each line in the file is the path to a text 
	 *	file containing the light curve of a source in thesample.
	 * 
	 * @exception FileIo Thrown if either the catalog file or the 
	 * 	selected light curve file do not exist.
	 * 
	 * @todo Fix exception spec to distinguish between catalog not found 
	 *	(which is unrecoverable) and source file not found (which is, 
	 *	at least in principle).
	 */
	SpitzerNonVar();
	
	// Virtual destructor following Effective C++
	virtual ~SpitzerNonVar() {};

private: 
	/** Returns the list of files from which the class may select sources
	 *
	 * @invariant getLcLibrary() is a constant expression
	 */
	static const std::vector<std::string>& getLcLibrary();
};

/** Data generator for sources that are not IR excess sources in Spitzer and 
 * variables in PTF.
 */
class NonSpitzerVar: public Observations {
public: 
	/** Initializes the object to represent a randomly selected light 
	 * curve meeting the sample criteria.
	 *
	 * @pre There exists a text file named "nonspitzervar.cat" in the 
	 * 	working directory. Each line in the file is the path to a text 
	 *	file containing the light curve of a source in thesample.
	 * 
	 * @exception FileIo Thrown if either the catalog file or the 
	 * 	selected light curve file do not exist.
	 * 
	 * @todo Fix exception spec to distinguish between catalog not found 
	 *	(which is unrecoverable) and source file not found (which is, 
	 *	at least in principle).
	 */
	NonSpitzerVar();
	
	// Virtual destructor following Effective C++
	virtual ~NonSpitzerVar() {};

private: 
	/** Returns the list of files from which the class may select sources
	 *
	 * @invariant getLcLibrary() is a constant expression
	 */
	static const std::vector<std::string>& getLcLibrary();
};

/** Data generator for sources that are both IR excess sources in Spitzer and 
 * variables in PTF.
 */
class SpitzerVar: public Observations {
public: 
	/** Initializes the object to represent a randomly selected light 
	 * curve meeting the sample criteria.
	 *
	 * @pre There exists a text file named "spitzervar.cat" in the 
	 * 	working directory. Each line in the file is the path to a text 
	 *	file containing the light curve of a source in thesample.
	 * 
	 * @exception FileIo Thrown if either the catalog file or the 
	 * 	selected light curve file do not exist.
	 * 
	 * @todo Fix exception spec to distinguish between catalog not found 
	 *	(which is unrecoverable) and source file not found (which is, 
	 *	at least in principle).
	 */
	SpitzerVar();
	
	// Virtual destructor following Effective C++
	virtual ~SpitzerVar() {};

private: 
	/** Returns the list of files from which the class may select sources
	 *
	 * @invariant getLcLibrary() is a constant expression
	 */
	static const std::vector<std::string>& getLcLibrary();
};

}}		// end lcmc::models

#endif		// end ifndef LCMCSAMPLESH
