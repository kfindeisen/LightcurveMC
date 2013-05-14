/** Definitions for parsing ranges of values
 * @file cmd/cmd_ranges.tmp.h
 * @author Krzysztof Findeisen
 * @date Created April 27, 2013
 * @date Last modified May 11, 2013
 */

#ifndef LCMCCMDRANGEH
#define LCMCCMDRANGEH

#include <string>
#include <utility>

#include "../warnflags.h"

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
/** Defines the behavior of a pair of numbers so that TCLAP can parse range 
 * input
 */
template<class T, class U> 
struct ArgTraits<std::pair<T, U> > {
	/** Specifies the way that values gets assigned to this type during 
	 *	parsing. The two supported types are StringLike and 
	 *	ValueLike. 
	 */
	typedef ValueLike ValueCategory;
};

}		// end TCLAP

#endif 		// LCMCCMDRANGEH
