/** Drivers for major simulation steps
 * @file lightcurveMC/sims.cpp
 * @author Krzysztof Findeisen
 * @date Created May 25, 2013
 * @date Last modified June 18, 2013
 */

#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include "../common/fileio.h"
#include "gsl_compat.h"
#include "lightcurvetypes.h"
#include "mcio.h"
#include "samples/observations.h"
#include "sims.h"
#include "../common/alloc.tmp.h"

namespace lcmc { 

using std::auto_ptr;
using std::string;
using std::vector;
using boost::shared_ptr;
using models::ParamList;
using kpfutils::checkAlloc;

namespace models {

// Full spec given with the declaration because lcregistry.cpp is deliberately 
//	hidden in the documentation
// 
/** lcFactory() is a factory method that allocates and initializes a 
 *	@ref lcmc::models::ILightCurve "ILightCurve" object given its 
 * 	specification.
 *
 * @param[in] whichLc The type of light curve to be created.
 * @param[in] times The times at which the light curve will be sampled.
 * @param[in] lcParams The bulk properties of the light curve. The parameters 
 * 	needed depend on each individual light curve and are given in the 
 * 	light curve documentation.
 *
 * @return A smart pointer to an object of the subclass of 
 *	@ref lcmc::models::ILightCurve "ILightCurve" corresponding to the 
 *	value of @p whichLc. The object pointed to by the smart pointer has 
 *	been initialized with the data in @p lcParams.
 *
 * @pre No element of @p times is NaN
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	construct the object.
 * @exception std::invalid_argument Thrown if @p whichLc is invalid.
 * @exception lcmc::models::except::MissingParam Thrown if a required 
 *	parameter is missing from @p lcParams.
 * @exception lcmc::models::except::BadParam Thrown if any of the light 
 *	curve parameters are outside their allowed ranges.
 *
 * @exceptsafe Function arguments are unchanged in the event of an exception.
 */
auto_ptr<ILightCurve> lcFactory(LightCurveType whichLc, const vector<double> &times, 
		const ParamList &lcParams);

}	// end lcmc::models

/** Returns the time stamps corresponding to a particular input file.
 *
 * @param[in] dateList The name of a file containing a list of Julian dates.
 * @param[out] times The observation times in @p dateList.
 *
 * @post The data previously in @p times are erased
 * @post @p times contains a list of Julian dates, sorted in ascending order.
 *
 * @exception kpfutils::except::FileIo Thrown if @p dateList could 
 *	not be read or had the wrong format.
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store the output.
 *
 * @exceptsafe The program arguments are unchanged in the event of an exception.
 */
void makeTimes(const string& dateList, vector<double>& times) {
	using std::swap;

	// Cache time stamps to avoid unneccessary file I/O
	// invariant: oldTimeFile.empty() xor oldTimes contains the times within oldTimeFile
	static vector<double> oldTimes;
	static string oldTimeFile;

	if (oldTimeFile.empty() || oldTimeFile != dateList) {
		shared_ptr<FILE> hJulDates = kpfutils::fileCheckOpen(dateList, "r");

		// readTimeStamps() is not atomic
		// use copy-and-swap to ensure the cache doesn't get corrupted
		vector<double> tempTimes;
		readTimeStamps(hJulDates.get(), tempTimes);
		
		oldTimeFile = dateList;
		// IMPORTANT: no exceptions to the end of the block
		swap(oldTimes, tempTimes);
	}
	
	// vector::= only offers the basic guarantee
	// copy-and-swap to allow atomic guarantee
	vector<double> temp = oldTimes;
	
	swap(times, temp);
}

/** Generates white noise corresponding to a given cadence.
 *
 * @param[in] times The cadence over which to generate noise.
 * @param[in] sigma The amplitude of the noise.
 * @param[out] noise Stores the noise offsets.
 *
 * @post The data previously in @p noise are erased
 * @post @p noise.size() = @p times.size()
 * @post @p noise contains uncorrelated Gaussian noise with variance &sigma;<sup>2</sup>.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	generate random numbers or store the output.
 *
 * @exceptsafe The program is in a valid state in the event of an exception.
 * 
 * @internal @note No atomic guarantee because the RNG may be updated.
 */
void makeWhiteNoise(const vector<double>& times, double sigma, vector<double>& noise) {
	using std::swap;

	// Noise generator
	static shared_ptr<gsl_rng> mcDriver;
	// Separate instantiation to ensure gsl_rng_alloc() gets called until it succeeds
	if (mcDriver.get() == NULL) {
		mcDriver.reset(checkAlloc(gsl_rng_alloc(gsl_rng_mt19937)), &gsl_rng_free);
		gsl_rng_set(mcDriver.get(), 27);
	}

	// Generate the actual noise
	// copy-and-swap
	vector<double> tempNoise;
	tempNoise.reserve(times.size());
	
	for(size_t i = 0; i < times.size(); i++) {
		tempNoise.push_back(gsl_ran_gaussian(mcDriver.get(), sigma));
	}
	
	// IMPORTANT: no exceptions beyond this point
	
	swap(noise, tempNoise);
}

/** Generates an observed light curve for an injection analysis.
 *
 * @param[in] catalog The name of a file containing a list of light curves 
 *	to sample from.
 * @param[out] times, baseFlux Store a light curve randomly selected from @p catalog.
 *
 * @post The data previously in @p times and @p baseFlux are erased
 * @post @p times.size() = @p baseFlux.size()
 * @post The fluxes in the original light curve are scaled to a median flux 
 *	of one, then offset to a median flux of zero. 
 *
 * @exception lcmc::inject::except::NoCatalog Thrown if @p catalog does not exist.
 * @exception kpfutils::except::FileIo Thrown if the catalog or the light 
 *	curve could not be read.
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store the output.
 *
 * @exceptsafe The program is in a valid state in the event of an exception.
 * 
 * @internal @note No atomic guarantee because the RNG associated with 
 *	Observations will always be updated.
 */
void makeInjectNoise(const string& catalog, 
		vector<double>& times, vector<double>& baseFlux) {
	using namespace inject;
	using std::swap;

	auto_ptr<Observations> curData = dataSampler(catalog);

	// copy-and-swap to ensure times and baseFlux are updated together
	vector<double> tempFlux;
	curData->getFluxes(tempFlux);

	// Observations and ILightCurve both have a reference flux of 1
	// To add an Observations and an ILightCurve, need to subtract a 
	//	flux of 1 to avoid double-counting
	// This is the most convenient place to do it, since it lets the 
	//	Observations light curve be treated like correlated noise
	for(vector<double>::iterator it = tempFlux.begin(); it != tempFlux.end(); it++) {
		*it -= 1.0;
	}

	curData->getTimes(times);
	
	// IMPORTANT: no exceptions beyond this point
	
	swap(baseFlux, tempFlux);
}

/** Generates a random light curve, incorporating all the simulation settings.
 *
 * @param[in] curve The type of light curve to generate
 * @param[in] params The parameters for the light curve.
 * @param[in] times The times at which the light curve will be observed.
 * @param[in] noise The contaminating signal, in units of the median 
 *	source flux, to add to the generated light curve.
 * @param[out] lcFluxes The generated fluxes, including @p noise.
 *
 * @pre @p params contains a valid entry for each parameter required 
 *	by @p curve
 * @pre @p times.size() > 0
 * @pre @p times.size() = @p noise.size()
 *
 * @post Any data previously in @p lcFluxes is erased
 * @post @p lcFluxes.size() = @p times.size()
 * @post @p lcFluxes[i] = signal(@p times[i]) + @p noise[i]
 * 
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	calculate the light curve.
 * @exception std::logic_error Thrown if a bug is found in the flux calculations.
 * @exception std::invalid_argument Thrown if @p curve is invalid.
 * @exception lcmc::models::except::MissingParam Thrown if a required 
 *	parameter is missing from @p params.
 * @exception lcmc::models::except::BadParam Thrown if @p params contains an 
 *	illegal parameter value.
 *
 * @exceptsafe The program is in a consistent state in the event of an exception.
 * 
 * @internal @note Call to lcFactory() may update Stochastic::rng(), even if 
 *	getFluxes() throws an exception.
 */
void simLightCurve(const models::LightCurveType& curve, const models::ParamList& params, 
		const vector<double>& times, const vector<double>& noise, 
		vector<double>& lcFluxes) {
	// Generate the light curve...
	auto_ptr<models::ILightCurve> lcInstance = lcFactory(curve, times, params);
	lcInstance->getFluxes(lcFluxes);
	
	// No exceptions past this point
	size_t nObs = lcFluxes.size();
	
	for(size_t j = 0; j < nObs; j++) {
		lcFluxes[j] += noise[j];
	}
}

/** Randomly generates parameter values within the specified limits
 *
 * @param[in] limits A RangeList giving the parameters to generate, the 
 *	minimum and maximum values for each, and the distribution from which 
 *	to sample
 *
 * @return A ParamList giving a value for each parameter
 *
 * @post X is a parameter in the return value if and only if X is a parameter 
 *	in @c limits
 * @post for any X in the return value, 
 *	@p limits.@ref lcmc::models::RangeList::getMin() "getMin"(X) 
 *	&le; @p return.@ref lcmc::models::ParamList::get() "get"(X) 
 *	&le; @p limits.@ref lcmc::models::RangeList::getMax() "getMax"(X) 
 *
 * @post No element of the return value is NaN.
 *
 * @exception std::bad_alloc Thrown if not enough memory to generate random 
 *	values.
 * @exception std::logic_error Thrown if drawParams() does not support all 
 *	distributions in limits
 *
 * @exceptsafe Function parameters are unchanged in the event of an 
 *	exception.
 */
ParamList drawParams(const models::RangeList& limits) {
	using models::RangeList;
	
	// Need a random number generator
	static gsl_rng * randomizer = NULL;
	static bool seeded = false;

	// Separate initialization to ensure that drawParams() will try to 
	// allocate an RNG until it succeeds
	if (!randomizer) {
		randomizer = checkAlloc(gsl_rng_alloc(gsl_rng_mt19937));
	}
	if (!seeded) {
		gsl_rng_set(randomizer, 42);
		seeded = true;
	}
	
	ParamList returnValue;
	// Convert all parameters
	for(RangeList::const_iterator it = limits.begin(); it != limits.end(); it++) {
		double min                   = limits.getMin(*it);
		double max                   = limits.getMax(*it);
		RangeList::RangeType distrib = limits.getType(*it);
		
		double value;
		switch(distrib) {
			case RangeList::UNIFORM:
				value = (min == max ? min 
					: min + (max-min)*gsl_rng_uniform(randomizer));
				break;
			case RangeList::LOGUNIFORM:
				value = (min == max ? log10(min) 
					: log10(min) + (log10(max)-log10(min))*gsl_rng_uniform(randomizer));
				value = pow(10.0, value);
				break;
			default:
				throw std::invalid_argument("Unknown distribution!");
		};
		
		returnValue.add(*it, value);
	}
	// If limits is empty, then returnValue is empty as well
	
	return returnValue;
}

}	// end lcmc
