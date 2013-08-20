/** Functions for handling command line objects
 * @file lightcurveMC/cmd/cmdlinedyn.h
 * @author Krzysztof Findeisen
 * @date Created May 30, 2013
 * @date Last modified August 8, 2013
 */

#include <string>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/utility.hpp>

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

namespace lcmc { namespace parse {

using std::string;
using std::vector;
using namespace TCLAP;

// noncopyable uses a non-virtual destructor
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Weffc++"
#endif
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

/** Command-line parsing class that uses dynamically allocated arguments 
 *	rather than statically allocated ones. Otherwise, its behavior is 
 *	identical to @c TCLAP::CmdLine.
 *
 * This class is final.
 *
 * @invariant The memory for all arguments in CmdLineDyn is managed by the 
 *	CmdLineDyn object.
 */
class CmdLineDyn : public CmdLineInterface, boost::noncopyable {
public:
	/** Constructs a command-line object, using the standard arguments 
	 * 	for CmdLine.
	 */
	CmdLineDyn(const string& message, const char delimiter = ' ',
		const string& version = "none", bool helpAndVersion = true);
	
	/** Deallocates all arguments along with the command line.
	 */
	~CmdLineDyn();
	
	/** Adds a dynamically allocated argument to the command line.
	 */
	void add(Arg& a);

	/** Adds a dynamically allocated argument to the command line.
	 */
	void add(Arg* a);
	
	/** Adds multiple arguments with a xor constraint.
	 */
	void xorAdd(vector<Arg *>& xors);

	/** Adds two arguments with a xor constraint.
	 */
	void xorAdd(Arg& a, Arg& b);

	/** Parses the command line.
	 */
	void parse(int argc, const char* const *argv);

	/** Parses the command line.
	 */
	void parse(vector<string>& args);
	
	/** Returns the object that serves as the output buffer for messages 
	 *	from the command line
	 */
	CmdLineOutput* getOutput();
	
	/** Sets the output buffer to an externally allocated object
	 */
	void setOutput(CmdLineOutput* output);
	
	/** Returns the version string provided to the constructor.
	 */
	string& getVersion();

	/** Returns the name of the current program
	 */
	string& getProgramName();

	/** Returns the arguments on the command line
	 */
	std::list<Arg*>& getArgList();

	/** Returns the object that parses any arguments added with xorAdd()
	 */
	XorHandler& getXorHandler();

	/** Returns the delimiter provided to the constructor
	 */
	char getDelimiter();
	
	/** Returns the program description provided to the constructor
	 */
	string& getMessage();
	
	/** Reports whether or not the command line uses the default @c --help 
	 *	and @c --version arguments
	 */
	bool hasHelpAndVersion();
	
	/** Resets the instance as if it had just been constructed so that 
	 *	the instance can be reused. 
	 */
	void reset();
	
private:
	CmdLine* const pImpl;
	// std::auto_ptr isn't compatible with vector because it doesn't have 
	//	a copy-constructor from const
	vector<boost::shared_ptr<Arg> > args;
};

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

}}	// end lcmc::parse
