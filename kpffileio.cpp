/** Handy generic input and output routines
 * @file kpffileio.cpp
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified February 4, 2011
 */

#include <stdexcept>
#include <string>
#include "kpffileio.h"

namespace kpffileio {

using namespace std;

/** Opens a file using the best method available on the local system
 *
 * @param[in] filename a null-terminated C-string containing the name of 
 *	the file to be opened
 * @param[in] mode a null-terminated C-string containing the file access 
 *	mode. See fopen() documentation for details.
 *
 * @return a file handle for the newly opened file. If the file cannot be 
 *	opened, an exception is thrown and the return value is not defined.
 */	
FILE* bestFileOpen(const char *filename, const char *mode)
{
	return bestFileOpen(filename, mode, NULL);
}

/** Opens a file using the best method available on the local system
 *
 * @param[in] filename a null-terminated C-string containing the name of 
 *	the file to be opened
 * @param[in] mode a null-terminated C-string containing the file access 
 *	mode. See fopen() documentation for details.
 * @param[in] alert a null-termianted C-string containing a message to be 
 *	returned as an exception. If the file opening attempt produced an 
 *	exception of its own, alert is appended to any message attached to 
 *	that exception.
 *
 * @return a file handle for the newly opened file. If the file cannot be 
 *	opened, an exception is thrown and the return value is not defined.
 */	
FILE* bestFileOpen(const char *filename, const char *mode, const char *alert)
{
	FILE *hFile;
	#ifdef _WIN32
	if(fopen_s(&hFile, filename, mode) != 0) {
		char errBuffer[80];
		// Can't feed alert to _strerror_s because the spec says to report 
		//	the alert last
		_strerror_s(errBuffer, 80, NULL);
		// Concatenate everything and send it
		string errMessage(errBuffer);
		if (alert != NULL) {
			errMessage += "\n";
			errMessage += alert;
		}
		throw runtime_error(errMessage);
	}
	#else
	hFile = fopen(filename, mode);
	if(NULL == hFile) {
		string errMessage("File open failed");
		if (alert != NULL) {
			errMessage += "\n";
			errMessage += alert;
		}
		throw runtime_error(errMessage);
	}
	#endif
	return hFile;
}

/** Creates a new filename by removing the extension from the old name, if 
 *	present, and appending the new extension
 *
 * @param[in] oldFile the filename to modify
 * @param[in] newExt the extension to append, which may or may not include the 
 *	leading "."
 *
 * @return if oldFile had one or more filename extensions, the 
 *	function will return a new filename with the last extension in 
 *	the original filename replaced by the contents of newExt. If 
 *	oldFile did not have an extension, the function will return a 
 *	new filename with the contents of newExt appended to oldFile
 */	
string replaceExtension(string oldFile, string newExt)
{
	// Standardize input
	if (newExt[0] != '.') {
		newExt = '.' + newExt;
	}
	
	#ifdef _WIN32
	size_t endOfDir = oldFile.rfind('\\');
	#else
	size_t endOfDir = oldFile.rfind('/');
	#endif
	// If there is a directory in the filename, end_of_dir gives the 
	//	character just before the start of the filename proper
	// If there is no directory, then end_of_dir = npos, which is the 
	//	code for "start at the start of the string"
	size_t dot = oldFile.rfind('.');
	
	// No extension if filename does not contain a ., or if the only . 
	//	is the first character of the filename, or if the last . is 
	//	located in the directories or just after the last /
	if (dot == string::npos || dot == 0 
		|| (endOfDir != string::npos && dot <= endOfDir+1))
	{
		return oldFile + newExt;
	} else {
		oldFile.replace(dot, oldFile.length(), newExt);
		return oldFile;
	}
}

/** Creates a new string by removing leading and trailing whitespace from an 
 *	existing string
 * 
 * @param[in] original the string to trim
 *
 * @return a string identical to the argument, but with all 
 *	spaces, tabs, and newlines removed from the front and end of 
 *	the string
 */	
string strtrim(string original)
{
	string chars(" \t\r\n");
	// Copy the code since it's a short function and I don't want to copy 
	//	the return value twice
	size_t startOk = original.find_first_not_of(chars);
	size_t endOk   = original.find_last_not_of(chars);
	if (endOk != string::npos) {
		original.erase(endOk+1);
	}
	if (startOk > 0) {
		original.erase(0, startOk);
	}
	return original;
}

/** Creates a new string by removing leading and trailing characters from an 
 *	existing string
 * 
 * @param[in] original the string to trim
 * @param[in] chars a string containing the characters to remove.
 *
 * @return a string identical to the argument, but with all 
 *	whitespace removed from the front and end of the string
 */	
string strtrim(string original, string chars)
{
	size_t startOk = original.find_first_not_of(chars);
	size_t endOk   = original.find_last_not_of(chars);
	if (endOk != string::npos) {
		original.erase(endOk+1);
	}
	if (startOk > 0) {
		original.erase(0, startOk);
	}
	return original;
}

}	// end kpffileio
