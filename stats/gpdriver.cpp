/** Calculates periodograms and periods
 * @file lightcurveMC/stats/gpdriver.cpp
 * @author Krzysztof Findeisen
 * @date Created June 27, 2013
 * @date Last modified August 6, 2013
 */

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "../../common/nan.h"
#include "statcollect.h"
#include "statfamilies.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using std::string;
using std::vector;

/** Finds the best fit solution to a squared exponential Gaussian process model
 */
void fitGaussGp(const vector<double>& times, const vector<double>& data, 
		double& timescale, double& timeError);

/** Does all GP-related computations for a given light curve.
 *
 * @param[in] times The times at which the light curve was sampled.
 * @param[in] data The values of the light curve
 * @param[in] getGp Flag indicating that the best-fit timescale 
 *	should be extracted
 * @param[in] trueTime The value of the true time scale. NaN if not available.
 * @param[out] timescales The NamedCollection in which to record the 
 *	best-fit timescale, if any.
 * @param[out] timeErrors The NamedCollection in which to record the 
 *	uncertainty on the best-fit timescale, if any.
 * @param[out] normDevs The NamedCollection in which to record the normalized 
 *	deviation of the best-fit time scale from the true time scale, 
 *	if any.
 *
 * @pre @p times.size() = @p data.size()
 * @pre Neither @p times nor @p data may contain NaNs
 * 
 * @post if @p getGp, then a new element is appended to each of @p timescales, 
 *	@p timeErrors, and @p normDev. If no value is found, the appended 
 *	value is NaN.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store more statistics.
 * @exception std::invalid_argument Thrown if @p times and @p data do not 
 *	have matching lengths or if @p times or @p data contains NaNs.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times 
 *	and @p data are too short to calculate the desired statistics.
 *
 * @exceptsafe The program is in a consistent state in the event of an exception.
 */
void doGaussFit(const vector<double>& times, const vector<double>& data, 
		bool getGp, double trueTime, CollectedScalars& timescales, 
		CollectedScalars& timeErrors, CollectedScalars& normDevs) {
	if (times.size() != data.size()) {
		throw std::invalid_argument("Times and data must have the same length in doGaussFit() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>(data.size()) + " for data).");
	}

	if (getGp) {
		// copy-and-swap
		CollectedScalars tempTimes  = timescales;
		CollectedScalars tempErrors = timeErrors;
		CollectedScalars tempDevs   = normDevs;
		
		try {
			double bestTime, timeErr;
			fitGaussGp(times, data, bestTime, timeErr);
			
			// R code doesn't report errors, but lack of 
			//	convergence is usually pretty obvious...
			if (bestTime < 1e5 && bestTime > 0.0) {
				tempTimes .addStat(bestTime);
				tempErrors.addStat(timeErr);
				
				if (kpfutils::isNan(trueTime)) {
					tempDevs.addNull();
				} else {
					tempDevs.addStat((bestTime - trueTime)/timeErr);
				}
			} else {
				tempTimes .addNull();
				tempErrors.addNull();
				tempDevs  .addNull();
			}
		} catch (const std::runtime_error &e) {
			// Don't know how many of the collections were updated... revert to input
			tempTimes  = timescales;
			tempErrors = timeErrors;
			tempDevs   = normDevs;
			
			// May still throw bad_alloc
			tempTimes .addNull();
			tempErrors.addNull();
			tempDevs  .addNull();
		}
		
		// IMPORTANT: no exceptions beyond this point
		
		using std::swap;
		swap(timescales, tempTimes );
		swap(timeErrors, tempErrors);
		swap(normDevs  , tempDevs  );
	}
}

}}		// end lcmc::stats
