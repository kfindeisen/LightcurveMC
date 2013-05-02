/** Handy generic input and output routines
 * @file kpffileio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified February 4, 2011
 */

#ifndef KPFIOH
#define KPFIOH

#include <cstdio>
#include <string>

/** Namespace for custom I/O routines
 */
namespace kpffileio {

using std::string;

/** Opens a file using the best method available on the local system
 */
FILE* bestFileOpen(const char *filename, const char *mode);
/** Opens a file using the best method available on the local system
 */
FILE* bestFileOpen(const char *filename, const char *mode, const char *alert);

/** Creates a new filename by removing the extension from the old name, if 
 *	present, and appending the new extension
 */	
string replaceExtension(string oldFile, string newExt);

/* Creates a new string by removing leading and trailing whitespace from an 
 *	existing string
 */	
string strtrim(string original);

/* Creates a new string by removing leading and trailing characters from an 
 *	existing string
 */	
string strtrim(string original, string chars);

}		// end namespace kpffileio

#endif		// end ifndef KPFIOH
