/** Type definitions for lightcurveMC data representations
 * @file observations.h
 * @author Krzysztof Findeisen
 * @date Created May 4, 2012
 * @date Last modified May 7, 2012
 * 
 * These types represent different sets of observations that can be used to 
 * inject simulated signals into real data. The actual population of the sets 
 * is currently done using external catalog files, but this implementation is 
 * subject to change.
 */

#ifndef LCMCDATAH
#define LCMCDATAH

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/** This namespace identifies data types and functions that handle the loading 
 * of data so that it can be injected with simulated data.
 */
namespace lcmcinject {

class FileIo;

/** Interface class for generating data samples for signal injection testing. 
 * Subclasses represent particular samples from which example observations are 
 * to be drawn. Objects of (sub)class Observations represent individual 
 * sources drawn from the appropriate sample.
 * 
 * Observations objects are non-deterministic in the sense that the user only 
 * specifies which sample the light curve comes from, but not which light 
 * curve from within the sample gets used. However, any individual object is 
 * immutable, and can be can be used without worrying about determinism.
 *
 * @invariant Observations and all its subclasses should be immutable.
 * @invariant The light curve has a median flux of 1, so that synthetic light 
 *	curve amplitudes can be expressed in units of the source flux.
 * 
 * @todo Consider using Featurizer::LightCurve to streamline implementation
 *
 * @todo Switch to a strategy that relies less on hard-coding
 */
class Observations {
public: 
	/** Returns the timestamps associated with this simulation.
	 */
	void getTimes(std::vector<double>& timeArray) const;

	/** Returns the flux measurements associated with this simulation.
	 */
	void getFluxes(std::vector<double>& fluxArray) const;

	// Virtual destructor following Effective C++
	virtual ~Observations() {};

protected:
	/** Default constructor prevents Observations from being created 
	 * directly.
	 */
	Observations();
	
	/** Initializes an object to the value of a particular light curve. To 
	 * enforce nondeterminism, the choice of light curve should be made 
	 * within the constructor of the subtype 
	 *
	 * @param[in] fileName Path to a text file containing a single light 
	 * curve. Currently only space-delimited tables in the format (JD, 
	 * flux, error) are supported. Support for additional formats will be 
	 * added later.
	 * 
	 * @exception FileIo Thrown if the file could not be read.
	 *
	 * @todo Find a better way to deal with classes distinguishable only 
	 * by how they're constructed. A friend factory function, maybe?
	 */
	void init(const std::string& fileName);

private: 
	/** Store the light curve itself
	 */
	std::vector<double> times;
	std::vector<double> fluxes;
};

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type. Use of an auto_ptr prevents 
 *	client-side memory management issues.
 *
 * @param[in] whichSample The type of light curve to be read.
 *
 * @return A smart pointer to an object of the subclass of Observations 
 *	corresponding to the value of whichSample.
 *
 * @exception invalid_argument Thrown if whichSample is not the name of a 
 *	recognized sample.
 */
std::auto_ptr<Observations> dataSampler(const std::string& whichSample);


/** Exception thrown when a file could not be opened for reading or writing.
 */
class FileIo: public std::runtime_error {
public:
	/** Basic constructor.
	 * @param[in] what_arg The content for the value returned by member what.
	 */
	FileIo(const std::string& what_arg);
};

}		// end lcmcinject

#endif		// end ifndef LCMCDATAH
