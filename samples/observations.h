/** Type definitions for lightcurveMC data representations
 * @file lightcurveMC/samples/observations.h
 * @author Krzysztof Findeisen
 * @date Created May 4, 2012
 * @date Last modified May 9, 2012
 * 
 * These types represent different sets of observations that can be used to 
 * inject simulated signals into real data. The actual population of the sets 
 * is currently done using external catalog files, but this implementation is 
 * subject to change.
 */

#ifndef LCMCDATAH
#define LCMCDATAH

#include <memory>
#include <string>
#include <vector>

namespace lcmc { 

/** This namespace identifies data types and functions that handle the loading 
 * of data so that it can be injected with simulated data.
 */
namespace inject {

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
 */
class Observations {
public: 
	/** Initializes the object to represent a randomly selected light 
	 * curve from the selected catalog.
	 */
	Observations(const std::string& catalogName);
	
	/** Returns the timestamps associated with this source.
	 */
	void getTimes(std::vector<double>& timeArray) const;

	/** Returns the flux measurements associated with this source.
	 */
	void getFluxes(std::vector<double>& fluxArray) const;

	// Virtual destructor following Effective C++
	virtual ~Observations() {};

private:
	/** Initializes an object to the value of a particular light curve.
	 */
	void readFile(const std::string& fileName);

	/** Returns the list of files from which the class may select sources
	 */
	static const std::vector<std::string> getLcLibrary(const std::string& catalogName);

	/** Store the light curve itself
	 */
	std::vector<double> times;
	std::vector<double> fluxes;
};

/** dataSampler is a factory method that allocates and initializes an 
 *	Observations object of a particular type.
 */
std::auto_ptr<Observations> dataSampler(const std::string& whichSample);

}}		// end lcmc::inject

#endif		// end ifndef LCMCDATAH
