/** Classes for tracking test statistics
 * @file lightcurveMC/stats/statcollect.h
 * @author Krzysztof Findeisen
 * @date Created June 6, 2013
 * @date Last modified August 5, 2013
 */

#ifndef LCMCSTATCOLH
#define LCMCSTATCOLH

#include <algorithm>
#include <string>
#include <vector>
#include <cstdio>

namespace lcmc { namespace stats {

using std::swap;
using std::string;
using std::vector;

/** Shorthand for a vector of doubles.
 */
typedef vector<double> DoubleVec;

/** Common interface for compilations of statistics.
 */
class IStats {
public:
	/** Prints a single family of statistics to the specified file
	 *
	 * The function will print, in order: the mean of the statistic, the 
	 * standard deviation of the statistic, the fraction of times each 
	 * statistic was defined, and the name of a file containing the 
	 * distribution of the statistics. The row is in tab-delimited 
	 * format, except that the mean and standard deviation are separated by 
	 * a ± sign for improved readability.
	 *
	 * @param[in] hOutput An open file handle representing the text file 
	 *	to write to.
	 *
	 * @exception std::runtime_error Thrown if there are difficulties writing 
	 *	to @p file
	 * @exception kpfutils::except::FileIo Thrown if there are difficulties writing 
	 *	to the distribution file.
	 *
	 * @exceptsafe The program is in a consistent state in the event 
	 *	of an exception.
	 */
	virtual void printStats(FILE* const hOutput) const = 0;

	/** Deletes all the simulation results from the object. 
	 * 
	 * @post The object reverts to its initial state after being constructed.
	 *
	 * @exceptsafe Does not throw exceptions.
	 */
	virtual void clear() = 0;
	
	virtual ~IStats() {}
};

/** Abstract base class defining infrastructure used by statistic collections.
 */
class NamedCollection : public IStats {
protected:
	/** Names a collection of statistics.
	 */
	NamedCollection(const string& statName, const string& distribFile);

	/** Returns the name of the statistic.
	 */
	const string& getStatName() const;

	/** Returns the file name of the statistic.
	 */
	const string& getFileName() const;
	
	/** Non-throwing swap
	 */
	void swap(NamedCollection& other);
	
private:
	string statName;
	string auxFile;
};

/** Defines a collection of scalar statistics.
 */
class CollectedScalars : public NamedCollection {
public:
	/** Constructs a collection of statistics.
	 */
	CollectedScalars(const string& statName, const string& distribFile);

	/** Records the value of a scalar statistic.
	 */
	void addStat(double value);

	/** Records an invalid scalar statistic.
	 */
	void addNull();

	// Inherit documentation from IStats
	void printStats(FILE* const hOutput) const;

	// Inherit documentation from IStats
	void clear();
	
	/** Returns a vector containing the data in the same order as 
	 * printed by printStats()
	 */
	void toVector(vector<double>& outVector) const;

	/** Prints a header row representing the statistics printed by 
	 *	printStats() to the specified file
	 */
	static void printHeader(FILE* const hOutput, const string& fieldName);

	/** Non-throwing swap
	 */
	void swap(CollectedScalars& other);
	
private:
	vector<double> stats;
};

/** Non-throwing swap
 */
void swap(CollectedScalars& a, CollectedScalars& b);

/** Defines a collection of vector statistics.
 */
class CollectedVectors : public NamedCollection {
public:
	/** Constructs a collection of statistics.
	 */
	CollectedVectors(const string& statName, const string& distribFile);

	/** Records the value of a vector statistic.
	 */
	void addStat(const DoubleVec& value);

	// Inherit documentation from IStats
	void printStats(FILE* const hOutput) const;

	// Inherit documentation from IStats
	void clear();

	/** Prints a header row representing the statistics printed by 
	 *	printStats() to the specified file
	 */
	static void printHeader(FILE* const hOutput, const string& fieldName);

	/** Non-throwing swap
	 */
	void swap(CollectedVectors& other);
	
private:
	vector<DoubleVec> stats;
};
/** Non-throwing swap
 */
void swap(CollectedVectors& a, CollectedVectors& b);

/** Defines a collection of statistics where each statistic represents 
 *	the sampling of a function, @f$\{(x_i, y_i)\}@f$.
 */
class CollectedPairs : public NamedCollection {
public:
	/** Constructs a collection of statistics.
	 */
	CollectedPairs(const string& statName, const string& distribFile);

	/** Records the value of a function statistic.
	 */
	void addStat(const DoubleVec& x, const DoubleVec& y);

	// Inherit documentation from IStats
	void printStats(FILE* const hOutput) const;

	// Inherit documentation from IStats
	void clear();

	/** Prints a header row representing the statistics printed by 
	 *	printStats() to the specified file
	 */
	static void printHeader(FILE* const hOutput, const string& fieldName);

	/** Non-throwing swap
	 */
	void swap(CollectedPairs& other);
	
private:
	vector<DoubleVec> x;
	vector<DoubleVec> y;
};
/** Non-throwing swap
 */
void swap(CollectedPairs& a, CollectedPairs& b);

}}		// end lcmc::stats

// Template overrides for clients that (incorrectly) call std::swap
namespace std {

template <>
void swap<lcmc::stats::CollectedScalars>(lcmc::stats::CollectedScalars& a, 
		lcmc::stats::CollectedScalars& b);

template <>
void swap<lcmc::stats::CollectedVectors>(lcmc::stats::CollectedVectors& a, 
		lcmc::stats::CollectedVectors& b);

template <>
void swap<lcmc::stats::CollectedPairs>(lcmc::stats::CollectedPairs& a, 
		lcmc::stats::CollectedPairs& b);

}		// end std

#endif		// End ifndef LCMCSTATCOLH
