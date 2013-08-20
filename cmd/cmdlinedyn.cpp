/** Command line object that manages the memory for its own arguments
 * @file lightcurveMC/cmd/cmdlinedyn.cpp
 * @author Krzysztof Findeisen
 * @date Created August 8, 2013
 * @date Last modified August 8, 2013
 */

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "cmdlinedyn.h"

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
using boost::shared_ptr;
using namespace TCLAP;

/** Constructs a command-line object, using the standard arguments 
 * 	for CmdLine.
 */
CmdLineDyn::CmdLineDyn(const string& message, const char delimiter,
		const string& version, bool helpAndVersion) : CmdLineInterface(), 
		pImpl(new CmdLine(message, delimiter, version, helpAndVersion)), args() {
	// CmdLineDyn specs assume exceptions are propagated
	pImpl->setExceptionHandling(false);
}

/** Deallocates all arguments along with the command line.
 */
CmdLineDyn::~CmdLineDyn() {
	delete pImpl;
}

/** Adds a dynamically allocated argument to the command line.
 *
 * @param[in] a A @c TCLAP::Arg to add.
 *
 * @pre @p a was allocated using the @c new operator
 * @pre @p a is not being managed by a smart pointer or other object
 *
 * @post @p a is now an argument on the command line
 * @post @p a will be deallocated when @p this is destroyed
 *
 * @exception TCLAP::SpecificationException Thrown if @p this 
 *	already contains an argument with a name equal to @p a.name()
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store the argument.
 *
 * @exceptsafe The object and the function argument are unchanged in 
 *	the event of an exception.
 */
void CmdLineDyn::add(Arg& a) {
	args.push_back(shared_ptr<Arg>(&a));
	pImpl->add(a);
}

/** Adds a dynamically allocated argument to the command line.
 *
 * @param[in] a A @c TCLAP::Arg to add.
 *
 * @pre @p a was allocated using the @c new operator
 * @pre @p a is not being managed by a smart pointer or other object
 *
 * @post @p a is now an argument on the command line
 * @post @p a will be deallocated when @p this is destroyed
 *
 * @exception TCLAP::SpecificationException Thrown if @p this 
 *	already contains an argument with a name equal to @p a->name()
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store the argument.
 *
 * @exceptsafe The object and the function argument are unchanged in 
 *	the event of an exception.
 */
void CmdLineDyn::add(Arg* a) {
	args.push_back(shared_ptr<Arg>(a));
	pImpl->add(a);
}

/** Adds multiple arguments with a xor constraint.
 * 
 * @param[in] xors A list of @c TCLAP::Arg to add.
 *
 * @pre Each element of @p xors was allocated using the @c new operator
 * @pre No element of @p xors is being managed by a smart pointer 
 *	or other object
 *
 * @post Each element of @p xors is now an argument on the command line
 * @post Each element of @p xors will be deallocated when @p this is destroyed
 * @post Unless exactly one of the arguments in @p xors is given on the 
 *	command line, parse() will throw an exception.
 *
 * @exception TCLAP::SpecificationException Thrown if @p this 
 *	already contains an argument with a name equal any element of @p xors
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store the arguments.
 * 
 * @bug This function is not exception-safe!
 */
void CmdLineDyn::xorAdd(vector<Arg *>& xors) {
	for(vector<Arg*>::const_iterator it = xors.begin(); it != xors.end(); it++) {
		args.push_back(shared_ptr<Arg>(*it));
	}
	pImpl->xorAdd(xors);
}

/** Adds two arguments with a xor constraint.
 * 
 * @param[in] a A @c TCLAP::Arg to add.
 * @param[in] b A @c TCLAP::Arg to add.
 *
 * @pre Both @p a and @p b were allocated using the @c new operator
 * @pre Neither @p a nor @p b is being managed by a smart pointer 
 *	or other object
 *
 * @post @p a and @p b are now arguments on the command line
 * @post @p a and @p b will be deallocated when @p this is destroyed
 * @post Unless only @p a or @p only b is given on the 
 *	command line, parse() will throw an exception.
 *
 * @exception TCLAP::SpecificationException Thrown if @p this 
 *	already contains an argument with a name equal to @p a->name() 
 *	or @p b->name()
 * @exception std::bad_alloc Thrown if there is not enough memory to 
 *	store the arguments.
 * 
 * @bug This function is not exception-safe!
 */
void CmdLineDyn::xorAdd(Arg& a, Arg& b) {
	args.push_back(shared_ptr<Arg>(&a));
	args.push_back(shared_ptr<Arg>(&b));
	pImpl->xorAdd(a, b);
}

/** Parses the command line.
 * 
 * @param[in] argc,argv The program arguments, following 
 *	the C convention.
 *
 * @post The arguments in @p this now store the values passed 
 *	on the command line
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to process the command line arguments
 * @exception ArgParseException Thrown if the command line string 
 *	could not be parsed as an argument. 
 * @exception CmdLineParseException Thrown if the command line input 
 *	could not be consistently separated into arguments. 
 * @exception ExitException Thrown if a command line argument says 
 *	to abort the program.
 *
 * @note Additional exceptions may be thrown by @c TCLAP::Visitor.
 *
 * @exceptsafe The object and function arguments are in a consistent 
 *	state in the event of an exception.
 */
void CmdLineDyn::parse(int argc, const char* const *argv) {
	pImpl->parse(argc, argv);
}

/** Parses the command line.
 * 
 * @param[in] args The program arguments, following 
 *	the C convention.
 *
 * @post The arguments in @p this now store the values passed 
 *	on the command line
 *
 * @exception std::bad_alloc Thrown if there is not enough memory 
 *	to process the command line arguments
 * @exception ArgParseException Thrown if the command line string 
 *	could not be parsed as an argument. 
 * @exception CmdLineParseException Thrown if the command line input 
 *	could not be consistently separated into arguments. 
 * @exception ExitException Thrown if a command line argument says 
 *	to abort the program.
 *
 * @note Additional exceptions may be thrown by @c TCLAP::Visitor.
 *
 * @exceptsafe The object and function arguments are in a consistent 
 *	state in the event of an exception.
 */
void CmdLineDyn::parse(vector<string>& args) {
	pImpl->parse(args);
}

/** Returns the object that serves as the output buffer for messages 
 *	from the command line
 *
 * @return A handle for the internal CmdLineOutput object
 *
 * @exceptsafe Does not throw exceptions.
 */
CmdLineOutput* CmdLineDyn::getOutput() {
	return pImpl->getOutput();
}

/** Sets the output buffer to an externally allocated object
 *
 * @param[in] output 
 *
 * @pre @p output must not be deallocated before the command line 
 *	is deallocated
 *
 * @exceptsafe Does not throw exceptions.
 */
void CmdLineDyn::setOutput(CmdLineOutput* output) {
	pImpl->setOutput(output);
}

/** Returns the version string provided to the constructor.
 *
 * @return A copy of the version string.
 *
 * @exceptsafe Does not throw exceptions.
 */
string& CmdLineDyn::getVersion() {
	return pImpl->getVersion();
}

/** Returns the name of the current program
 *
 * @return The name of the program as represented in @c argv[0]
 *
 * @pre parse() has been called on this command line
 *
 * @exceptsafe Does not throw exceptions.
 */
string& CmdLineDyn::getProgramName() {
	return pImpl->getProgramName();
}

/** Returns the arguments on the command line
 *
 * @return A list of pointers to the command line arguments. These 
 *	pointers are equal to previous arguments of add() or xorAdd().
 *
 * @exceptsafe Does not throw exceptions.
 */
std::list<Arg*>& CmdLineDyn::getArgList() {
	return pImpl->getArgList();
}

/** Returns the object that parses any arguments added with xorAdd()
 *
 * @return A handle for the internal XorHandler object
 *
 * @exceptsafe Does not throw exceptions.
 */
XorHandler& CmdLineDyn::getXorHandler() {
	return pImpl->getXorHandler();
}

/** Returns the delimiter provided to the constructor
 *
 * @return The character separating a a command line from its 
 *	argument, such as '=' if the command line expects input of 
 *	the form "--arg=value"
 *
 * @exceptsafe Does not throw exceptions.
 */
char CmdLineDyn::getDelimiter() {
	return pImpl->getDelimiter();
}

/** Returns the program description provided to the constructor
 *
 * @return A copy of the string summarizing the program's purpose.
 *
 * @exceptsafe Does not throw exceptions.
 */
string& CmdLineDyn::getMessage() {
	return pImpl->getMessage();
}

/** Reports whether or not the command line uses the default @c --help 
 *	and @c --version arguments
 *
 * @return True if the default arguments are enabled, false otherwise.
 *
 * @exceptsafe Does not throw exceptions.
 */
bool CmdLineDyn::hasHelpAndVersion() {
	return pImpl->hasHelpAndVersion();
}

/** Resets the instance as if it had just been constructed so that 
 *	the instance can be reused. 
 *
 * @post Each argument in the command line is in an unparsed state
 * @post getProgramName() returns an empty string
 *
 * @exceptsafe Does not throw exceptions.
 */
void CmdLineDyn::reset() {
	pImpl->reset();
}

}}	// end lcmc::parse
