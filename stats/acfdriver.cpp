/** Calculates ACFs and related statistics
 * @file lightcurveMC/stats/acfdriver.cpp
 * @author Krzysztof Findeisen
 * @date Created June 8, 2013
 * @date Last modified August 5, 2013
 */

#include <algorithm>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <timescales/timescales.h>
#include "cut.tmp.h"
#include "statcollect.h"
#include "statfamilies.h"
#include "../except/undefined.h"

namespace lcmc { namespace stats {

using boost::lexical_cast;
using std::vector;

/** Does all ACF-related computations for a given light curve.
 *
 * The flavor of ACF is specified using a callback function.
 *
 * @param[in] times The times at which the light curve was sampled.
 * @param[in] data The values of the light curve
 * @param[in] acfFunc Function for calculating the autocorrelation function. 
 *	@p acfFunc takes, in order, the times of the light curve, the values 
 *	of the light curve, the time step between lag values, the number of 
 *	lag values, and an output vector to store the ACF.
 * @param[in] getCut Flag indicating that cuts through the the 
 *	ACF should be extracted
 * @param[in] getPlot Flag indicating that the ACF should be stored
 * @param[out] cut9 The NamedCollection in which to record the 
 *	time offset at which the ACF crosses 1/9.
 * @param[out] cut4 The NamedCollection in which to record the 
 *	time offset at which the ACF crosses 1/4.
 * @param[out] cut2 The NamedCollection in which to record the 
 *	time offset at which the ACF crosses 1/2.
 * @param[out] acfPlot The NamedCollection in which to record the ACF.
 *
 * @pre @p times.size() = @p data.size()
 * @pre Neither @p times nor @p data may contain NaNs
 * 
 * @post if @p getCut, then new elements are appended to @p cut9, 
 *	@p cut4, and @p cut2. If any of the cuts are 
 *	undefined, NaN is appended to the corresponding collection.
 * @post if @p getPlot, then a new element is appended to @p acfPlot.
 *
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store more statistics.
 * @exception std::invalid_argument Thrown if @p times and @p data do not 
 *	have matching lengths or if @p times or @p data contains NaNs.
 * @exception lcmc::stats::except::NotEnoughData Thrown if @p times 
 *	and @p data are too short to calculate the desired statistics.
 *
 * @note Additional exceptions may be thrown by @p acfFunc.
 *
 * @exceptsafe The program is in a consistent state in the event of an 
 *	exception, provided @p acfFunc offers at least the basic exception 
 *	guarantee.
 */
void doAcf(const vector<double>& times, const vector<double>& data, 
		void (*acfFunc) (const vector<double>&, const vector<double>&, 
				double, size_t, vector<double>&), 
		bool getCut, bool getPlot, 
		CollectedScalars& cut9, CollectedScalars& cut4, CollectedScalars& cut2, 
		CollectedPairs& acfPlot) {

	if (times.size() != data.size()) {
		throw std::invalid_argument("Times and data must have the same length in analyzeLightCurve() (gave " 
			+ lexical_cast<string>(times.size()) + " for times and " 
			+ lexical_cast<string>( data.size()) + " for data).");
	}

	if (getCut || getPlot) {
		// copy-and-swap
		CollectedScalars temp9    = cut9;
		CollectedScalars temp4    = cut4;
		CollectedScalars temp2    = cut2;
		CollectedPairs   tempPlot = acfPlot;
		
		try {
			// Regular grid at which offsets are generated
			const static double offStep = 0.1;
			// Minimum difference between two offsets written to a log file
			const static double storeFactor = 1.05;
			
			double maxOffset = kpftimes::deltaT(times);
			DoubleVec offsets;
			for (double t = 0.0; t < maxOffset; t += offStep) {
				offsets.push_back(t);
			}
			
			DoubleVec acf;
			acfFunc(times, data, offStep, offsets.size(), acf);
			
			if (getPlot) {
				// Record only logarithmically spaced bins, for compactness
				/** @todo Reimplement using an iterator adapter for faster performance
				 */
				DoubleVec logOffs(1, offsets.front());
				DoubleVec logAcf (1,     acf.front());
				double lastOffset = offsets.front();
				for (size_t i = 1; i < offsets.size(); i++) {
					if (offsets[i] >= storeFactor*lastOffset) {
						logOffs.push_back(offsets[i]);
						logAcf .push_back(    acf[i]);
						
						lastOffset = offsets[i];
					}
					// else skip
				}
								
				tempPlot.addStat(logOffs, logAcf);
			}
			
			if (getCut) {
				// Key cuts
				temp9.addStat(cutFunction(offsets, acf, 
					LessThan(1.0/9.0)));
				temp4.addStat(cutFunction(offsets, acf, 
					LessThan(0.25)   ));
				temp2.addStat(cutFunction(offsets, acf, 
					LessThan(0.5)    ));
			}
		} catch (const except::NotEnoughData &e) {
			// The one kind of Undefined we don't want to ignore
			throw;
		} catch (const except::Undefined &e) {
			// Don't know how many of the collections were updated... revert to input
			temp9    = cut9;
			temp4    = cut4;
			temp2    = cut2;
			tempPlot = acfPlot;
			
			// May still throw bad_alloc
			if (getCut) {
				temp9.addNull();
				temp4.addNull();
				temp2.addNull();
			}
//			if (getPlot) {
//				tempPlot.addNull();
//			}
		}
		
		// IMPORTANT: no exceptions beyond this point

		using std::swap;		
		swap(cut9   , temp9);
		swap(cut4   , temp4);
		swap(cut2   , temp2);
		swap(acfPlot, tempPlot);
	}
}

}}		// end lcmc::stats
