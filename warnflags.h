/** Defines macros for overriding compiler warnings
 * @file lightcurveMC/warnflags.h
 * @author Krzysztof Findeisen
 * @date Created June 11, 2013
 * @date Last modified April 11, 2013
 */

// The version of G++ being used determines how much finesse I can use to 
//	disable warnings
#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#define GNUC_FINEWARN
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#define GNUC_COARSEWARN
#endif
// Will need a separate set of flags if we're using a different compiler
