/** Code for parsing the command line interface
 * @file cmd.cpp
 * @author Krzysztof Findeisen
 * @date Created April 12, 2013
 * @date Last modified April 26, 2013
 */

#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>
#include "lightcurveparser.h"
#include "lightcurvetypes.h"
#include "paramlist.h"
#include "projectinfo.h"

#include "warnflags.h"

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

namespace TCLAP {
/** Defines the behavior of a pair of doubles so that TCLAP can parse range 
 * input
 */
template<class T, class U> struct ArgTraits<std::pair<T, U> > {
	typedef ValueLike ValueCategory;
};

}	// end TCLAP

/** Extracts formatted input into a pair of double precision floating point 
 * values.
 * 
 * The string is assumed to be formatted as two real literals, separated 
 * by whitespace.
 *
 * @param[in] str The stream to read from.
 * @param[out] val The pair in which to store the values.
 *
 * @return A reference to str.
 *
 * @exception std::ios_base::failure Thrown if the input cannot be read and 
 *	str.exceptions() is set to throw exceptions. If thrown, str remains in 
 *	a valid state.
 *
 * @todo Add support for reading a single number
 */
std::istream& operator>> (std::istream& str, std::pair<double, double>& val) {
	str >> val.first >> val.second;
	return str;
}

class KeywordConstraint : public TCLAP::ValuesConstraint<std::string> {
public:
	KeywordConstraint(std::vector<std::string>& allowed) 
			: TCLAP::ValuesConstraint<std::string>(allowed) {
	}
	
	virtual std::string shortID() const {
		return "keyword";
	}

        virtual ~KeywordConstraint() {}
};

template<class T>
class PositiveNumber : public TCLAP::Constraint<T> {
public:
	virtual std::string description() const {
		return std::string("positive ") + typeid(T).name();
	}

	virtual std::string shortID() const {
		return std::string("positive ") + typeid(T).name();
	}

	virtual bool check(const T& value) const {
		return (value > 0);
	}

        virtual ~PositiveNumber() {}
};

template<>
class PositiveNumber <long> : public TCLAP::Constraint<long> {
public:
	virtual std::string description() const {
		return "positive integer";
	}

	virtual std::string shortID() const {
		return "positive integer";
	}

	virtual bool check(const long& value) const {
		return (value > 0);
	}

        virtual ~PositiveNumber() {}
};

template<>
class PositiveNumber <double> : public TCLAP::Constraint<double>{
public:
	virtual std::string description() const {
		return "positive real number";
	}

	virtual std::string shortID() const {
		return "positive real number";
	}

	virtual bool check(const double& value) const {
		return (value > 0.0);
	}

        virtual ~PositiveNumber() {}
};

template<class T>
class NonNegativeNumber : public TCLAP::Constraint<T> {
public:
	virtual std::string description() const {
		return std::string("non-negative ") + typeid(T).name();
	}

	virtual std::string shortID() const {
		return std::string("non-negative ") + typeid(T).name();
	}

	virtual bool check(const T& value) const {
		return (value >= 0);
	}

        virtual ~NonNegativeNumber() {}
};

template<>
class NonNegativeNumber <long> : public TCLAP::Constraint<long> {
public:
	virtual std::string description() const {
		return "non-negative integer";
	}

	virtual std::string shortID() const {
		return "non-negative integer";
	}

	virtual bool check(const long& value) const {
		return (value >= 0);
	}

        virtual ~NonNegativeNumber() {}
};

template<>
class NonNegativeNumber <double> : public TCLAP::Constraint<double> {
public:
	virtual std::string description() const {
		return "non-negative real number";
	}

	virtual std::string shortID() const {
		return "non-negative real number";
	}

	virtual bool check(const double& value) const {
		return (value >= 0.0);
	}

        virtual ~NonNegativeNumber() {}
};

class PositiveRange : public TCLAP::Constraint<std::pair<double, double> > {
public:
	virtual std::string description() const {
		return "both numbers in the range must be positive, and the second must be no smaller than the first";
	}

	virtual std::string shortID() const {
		return "positive interval";
	}

	virtual bool check(const std::pair<double, double>& value) const {
		return (value.first > 0.0 && value.second >= value.first);
	}

        virtual ~PositiveRange() {}
};

class NonNegativeRange : public TCLAP::Constraint<std::pair<double, double> > {
public:
	virtual std::string description() const {
		return "both numbers in the range must be non-negative, and the second must be no smaller than the first";
	}

	virtual std::string shortID() const {
		return "non-negative interval";
	}

	virtual bool check(const std::pair<double, double>& value) const {
		return (value.first >= 0.0 && value.second >= value.first);
	}

        virtual ~NonNegativeRange() {}
};

class UnitSubrange : public TCLAP::Constraint<std::pair<double, double> > {
public:
	virtual std::string description() const {
		return "both numbers in the range must be in [0, 1], and the second must be no smaller than the first";
	}

	virtual std::string shortID() const {
		return "unit subinterval";
	}

	virtual bool check(const std::pair<double, double>& value) const {
		return (value.first >= 0.0 && value.second <= 1.0&& value.second >= value.first);
	}

        virtual ~UnitSubrange() {}
};


namespace lcmc { namespace parse {

using std::string;
using std::vector;
using models::LightCurveType;
using models::RangeList;

/** Converts the input arguments to a set of variables.
 *
 * @param[in] argc,argv an array of C-strings denoting the arguments, using the C 
 *	convention
 * @param[out] sigma the root-mean-square amount of noise to add to all flux 
 *	measurements
 * @param[out] nTrials the number of simulations to execute per bin
 * @param[out] toPrint if > 0, the program will print the first few 
 *	light curves generated. This setting is intended primarily for 
 *	debugging.
 * @param[out] paramRanges the minimum and maximum values of light curve 
 *	parameters allowed by the user
 * @param[out] jdList the name of a file containing the Julian dates of the 
 *	simulated observations
 * @param[out] lcNameList the list of light curve types to simulate, one 
 *	after the other
 * @param[out] lcList the list of light curve types to simulate, one after 
 *	the other
 * @param[out] dataSet the hardcoded name of an observational data set into 
 *	which to inject light curves
 * @param[out] injectMode if true, the program will carry out an injection 
 *	analysis rather than merely generating theoretical light curves.
 *
 * @todo Make dataSet something better than stringly typed.
 */
void parseArguments(int argc, char* argv[], 
		double &sigma, long &nTrials, long &toPrint, 
		RangeList& paramRanges, 
		string &jdList, vector<string> &lcNameList, 
		vector<LightCurveType> &lcList, 
		string& dataSet, bool& injectMode) {
	using namespace TCLAP;
	typedef std::pair<double, double> Range;
	
  	// Start by defining the command line
	CmdLine cmd(PROG_SUMMARY, ' ', VERSION_STRING);

	//--------------------------------------------------
	// Common constraints
	   PositiveNumber<double>    posReal;
	   PositiveNumber<long>      posInt;
	NonNegativeNumber<double> nonNegReal;
	NonNegativeNumber<long>   nonNegInt;
	PositiveRange                posRange;
	UnitSubrange                unitRange;

	//--------------------------------------------------
	// Mandatory simulation settings
	ValueArg<double> argNoise("", "noise", "Gaussian error added to each photometric measurement, in units of the typical source flux. REQUIRES that <date file> is provided.", 
		false, 
		0.0, &nonNegReal);

	vector<string> injectSamples;
	injectSamples.push_back("NonSpitzerNonVar");
	injectSamples.push_back("NonSpitzerVar");
	injectSamples.push_back("SpitzerNonVar");
	injectSamples.push_back("SpitzerVar");
	KeywordConstraint injAllowed(injectSamples);
	ValueArg<string> argInject("", "add", 
		"Name of the dataset to which to add the simulated signal. Allowed values are: " + injAllowed.description(), 
		false, 
		"", &injAllowed);

	UnlabeledValueArg<string> argDateFile("jdlist", 
		"Text file containing a list of Julian dates, one per line.", 
		true, 
		"", "date file");
	cmd.xorAdd(argInject, argDateFile);
	// argNoise is only allowed if argDateFile is used, but since 
	//	argDateFile is already in a xor relationship we'll have to 
	//	enforce this constraint manually after parsing
	cmd.add(argNoise);

	//--------------------------------------------------
	// Optional simulation settings
	ValueArg<long> argRepeat("", "ntrials", "Number of light curves generated per bin. 1000 if omitted.", 
		false, 
		1000, &posInt, cmd);

	ValueArg<long> argPrint("", "print", "Number of light curves to print. 0 if omitted.", 
		false, 
		0, &nonNegInt, cmd);

	//--------------------------------------------------
	// Light curve list
	const std::list<string> lcNames = lightCurveTypes();
	vector<string> lcNames2(lcNames.begin(), lcNames.end());
	KeywordConstraint lcAllowed(lcNames2);
	UnlabeledMultiArg<string> argLCList("lclist", 
		"List of light curves to model, in order. Allowed values are: " + lcAllowed.description(), 
		true, 
		&lcAllowed, cmd);
	
	//--------------------------------------------------
	// Model parameters
	ValueArg<Range> argPer ("p", "period", 
		"the smallest and largest periods, in days, to be tested. The period will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange, cmd);

	ValueArg<Range> argAmp ("a", "amp", 
		"the smallest and largest amplitudes to be tested. The amplitude will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange, cmd);

	ValueArg<Range> argPhi ("", "ph", 
		"the smallest and largest initial phases to be tested. The phase will be drawn from a uniform distribution. MUST be a subinterval of [0.0, 1.0]. Set to \"0.0 1.0\" if unspecified.", 
		false, 
		Range(0.0, 1.0), &unitRange, cmd);

	ValueArg<Range> argDiffus ("d", "diffus", 
		"the smallest and largest diffusion constants to be tested. The constant will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange, cmd);

	ValueArg<Range> argWidth ("w", "width", 
		"the smallest and largest event widths to be tested. The width will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange, cmd);

	ValueArg<Range> argWidth2 ("", "width2",   "the smallest and largest secondary widths to be tested. The width will be drawn from a log-uniform distribution.", 
		false, 
		Range(), &posRange, cmd);

	//--------------------------------------------------
	// Read from the line

	cmd.parse( argc, argv );
	
	//--------------------------------------------------
	// Export values

	//--------------------
	// Mandatory simulation settings
	// constraint: --noise only valid if jdList defined
	if (argNoise.isSet() && !argDateFile.isSet()) {
		try {
			CmdLineParseException error("Mutually exclusive argument already set!", 
				"(--noise)");
			cmd.getOutput()->failure(cmd, error);
		} catch (ExitException &e) {
			exit(e.getExitStatus());
		}
	} else {
		sigma = argNoise.getValue();
	}
	dataSet  = argInject  .getValue();
	jdList   = argDateFile.getValue();
	// Test if --add argument was used
	injectMode = (dataSet.size() > 0);

	//--------------------
	// Optional simulation settings
	nTrials  = argRepeat  .getValue();
	toPrint  = argPrint   .getValue();

	//--------------------
	// Light curve list
	
	// Light curve list is a bit more complicated
	// Iterate over the argument list, preserving the user-requested execution order
	vector<string> fullLCList(argLCList.getValue());
	lcNameList.clear();
	lcList.clear();
	for(vector<string>::const_iterator it=fullLCList.begin(); 
			it != fullLCList.end(); it++) {
		try {
			const LightCurveType curLC = parseLightCurve(*it);
			// Don't count light curves multiple times
			if (find(lcList.begin(), lcList.end(), curLC) == lcList.end())
			{
				lcNameList.push_back(*it);
				lcList.push_back(curLC);
			}
		} catch(std::domain_error e) {
			// If there's an invalid light curve, print a warning but keep going
			fprintf(stderr, "WARNING: %s\n", e.what());
		}
	}
	if (lcList.size() <= 0) {
		char errbuf[80];
		sprintf(errbuf, "No valid light curves given, type %s -h for a list \
				of choices.", argv[0]);
		throw std::runtime_error(errbuf);
	}

	//--------------------
	// Model parameters

	paramRanges.clear();
	// Require user to set amplitude and period, where applicable
	if (argAmp.isSet()) {
		paramRanges.add("a", argAmp.getValue(), RangeList::LOGUNIFORM);
	}
	if (argPer.isSet()) {
		paramRanges.add("p", argPer.getValue(), RangeList::LOGUNIFORM);
	}
	// Include default phase if no user input
	paramRanges.add("ph", argPhi.getValue(), RangeList::UNIFORM);
	// Require user to set event widths, where applicable
	if (argWidth.isSet()) {
		paramRanges.add("width",   argWidth.getValue(), RangeList::LOGUNIFORM);
	}
	if (argWidth2.isSet()) {
		paramRanges.add("width2", argWidth2.getValue(), RangeList::LOGUNIFORM);
	}
	// Require user to set diffusion constant for random walks
	if (argDiffus.isSet()) {
		paramRanges.add("d",      argDiffus.getValue(), RangeList::LOGUNIFORM);
	}
}

}}	// end lcmc::parse
