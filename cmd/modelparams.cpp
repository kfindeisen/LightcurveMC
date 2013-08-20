/** Definitions for model parameter command line arguments
 * @file lightcurveMC/cmd/modelparams.cpp
 * @author Krzysztof Findeisen
 * @date Created August 19, 2013
 * @date Last modified August 19, 2013
 */

#include <string>
#include "cmd.tmp.h"
#include "cmd_constraints.tmp.h"
#include "cmd_ranges.tmp.h"

#include "../../common/warnflags.h"

// TCLAP uses C-style casts
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

// TCLAP uses C-style casts
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <tclap/CmdLine.h>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

/** Extracts formatted input into a pair of double precision floating point 
 * values.
 */
// Must be declared in the global namespace to be identified by TCLAP
std::istream& operator>> (std::istream& str, std::pair<double, double>& val);

namespace lcmc { namespace parse {

using std::string;
using namespace TCLAP;

typedef std::pair<double, double> Range;

/** Specifies the command line parameters that describe model parameter ranges
 *
 * @param[in,out] cmd The command-line parser used by the program
 * 
 * @post @p cmd now contains the optional command-line parameters 
 *	for Lightcurve MC
 * 
 * @exceptsafe Does not throw exceptions.
 */
void logModelParams(CmdLineInterface& cmd) {
	static PositiveNumber<Range> posRange;
	static UnitSubrange         unitRange;

	ValueArg<Range>* argPer = new ValueArg<Range>("p", "period", 
		"the smallest and largest periods, in days, to be tested. The period will be drawn from a log-uniform distribution.", 
		false, Range(), &posRange);
	cmd.add(argPer);
	ValueArg<Range>* argAmp = new ValueArg<Range>("a", "amp", 
		"the smallest and largest amplitudes to be tested. The amplitude will be drawn from a log-uniform distribution.", 
		false, Range(), &posRange);
	cmd.add(argAmp);
	ValueArg<Range>* argPhi = new ValueArg<Range>("", "ph", 
		"the smallest and largest initial phases to be tested. The phase will be drawn from a uniform distribution. MUST be a subinterval of [0.0, 1.0]. Set to \"0.0 1.0\" if unspecified.", 
		false, Range(0.0, 1.0), &unitRange);
	cmd.add(argPhi);
	ValueArg<Range>* argDiffus = new ValueArg<Range>("d", "diffus", 
		"the smallest and largest diffusion constants to be tested. The constant will be drawn from a log-uniform distribution.", 
		false, Range(), &posRange);
	cmd.add(argDiffus);
	ValueArg<Range>* argWidth = new ValueArg<Range>("w", "width", 
		"the smallest and largest event widths to be tested. The width will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange);
	cmd.add(argWidth);
	ValueArg<Range>* argWidth2 = new ValueArg<Range>("", "width2", 
		"the smallest and largest secondary widths to be tested. The width will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange);
	cmd.add(argWidth2);
	ValueArg<Range>* argPer2 = new ValueArg<Range>("", "period2", 
		"the smallest and largest secondary periods, in days, to be tested. The secondary period will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange);
	cmd.add(argPer2);
	ValueArg<Range>* argAmp2 = new ValueArg<Range>("", "amp2", 
		"the smallest and largest secondary amplitudes to be tested. The secondary amplitude will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange);
	cmd.add(argAmp2);
}

/** Parses the command line parameters that describe model parameter ranges
 *
 * @param[in] cmd The command-line parser used by the program
 * @param[out] range A rangelist to contain the parameter ranges.
 * 
 * @exception std::logic_error Thrown if the expected parameters are missing from @p cmd.
 *
 * @exceptsafe All arguments are left in valid states in the event 
 *	of an exception.
 */
void parseModelParams(CmdLineInterface& cmd, RangeList& range) {
	range.clear();
	addParam(range, "a", getParam<ValueArg<Range> >(cmd, "amp"), 
		RangeList::LOGUNIFORM);
	addParam(range, "p", getParam<ValueArg<Range> >(cmd, "period"), 
		RangeList::LOGUNIFORM);
	// Include default phase if no user input
	range.add("ph", getParam<ValueArg<Range> >(cmd, "ph").getValue(), 
		RangeList::UNIFORM);
	addParam(range, "width", 
		getParam<ValueArg<Range> >(cmd, "width"), 
		RangeList::LOGUNIFORM);
	addParam(range, "width2", 
		getParam<ValueArg<Range> >(cmd, "width2"), 
		RangeList::LOGUNIFORM);
	addParam(range, "d", getParam<ValueArg<Range> >(cmd, "diffus"), 
		RangeList::LOGUNIFORM);
	addParam(range, "amp2", getParam<ValueArg<Range> >(cmd, "amp2"), 
		RangeList::LOGUNIFORM);
	addParam(range, "period2", getParam<ValueArg<Range> >(cmd, "period2"), 
		RangeList::LOGUNIFORM);
}

}}	// end lcmc::parse
