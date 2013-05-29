/** Generates a multivariate normal with a given covariance matrix
 * @file lightcurveMC/waves/multinormal.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 26, 2013
 */

#include <algorithm>
#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "generators.h"
#include "../gsl_compat.tmp.h"

namespace lcmc { namespace utils {

using std::vector;
using boost::lexical_cast;
using boost::shared_ptr;
using utils::checkAlloc;

/** Given a matrix A, returns a matrix B with the property 
 *	@f$ A = B B^\intercal @f$
 */
shared_ptr<gsl_matrix> getHalfMatrix(const shared_ptr<gsl_matrix>& a);

/** Replaces one matrix with the value of another.
 */
void matrixCopy(shared_ptr<gsl_matrix>& target, const shared_ptr<gsl_matrix>& newData);

// Include this function only if gsl_is_matrix_equal() is not available
#ifndef _GSL_HAS_MATRIX_EQUAL
/** Tests whether two matrices have approximately equal elements.
 */
bool matrixEqual(const gsl_matrix* const a, const gsl_matrix* const b);
#endif

/** Transforms an uncorrelated sequence of Gaussian random numbers into a 
 *	correlated sequence
 *
 * @param[in] indVec A vector of independent unit Gaussian random numbers.
 * @param[in] covar The desired covariance matrix.
 * @param[out] corrVec A vector of correlated Gaussian random numbers with 
 *	mean zero and covariance matrix covar.
 *
 * @pre @p indVec.size() = @p covar->size1 = @p covar->size2
 * @pre @p covar is symmetric and positive semidefinite
 * @pre @p corrVec may refer to the same vector as @p indVec
 *
 * @post @p corrVec.size() = @p indVec.size()
 *
 * @exception std::bad_alloc Thrown if there was not enough memory to 
 *	compute the transformation
 * @exception std::invalid_argument Thrown if the lengths do not match 
 *	or if the matrix is not symmetric or positive semidefinite.
 *
 * @exceptsafe The parameters are unchanged in the event of an exception.
 *
 * @internal @exceptsafe The internal cache in multiNormal() provides only the 
 *	basic exception guarantee. However, aside from run time this is not 
 *	visible to the rest of the program.
 */
void multiNormal(const vector<double>& indVec, const shared_ptr<gsl_matrix>& covar, 
		vector<double>& corrVec) {
	// invariant: covCache is empty xor 
	//	owns a deallocator gsl_matrix_free()
	// invariant: prefixCache is empty xor 
	//	owns a deallocator gsl_matrix_free()
	// invariant: covCache is empty <=> prefixCache is empty
	// invariant: if covCache is not empty, then 
	//	prefixCache == getHalfMatrix(covCache)
	static shared_ptr<gsl_matrix> covCache;
	static shared_ptr<gsl_matrix> prefixCache;

	const size_t N = indVec.size();
	if(covar->size1 != covar->size2) {
		try {
			std::string len1 = lexical_cast<std::string>(covar->size1);
			std::string len2 = lexical_cast<std::string>(covar->size2);
			throw std::invalid_argument(len1 + "×" + len2 
				+ " covariance matrix passed to multiNormal().");
		} catch (const boost::bad_lexical_cast &e) {
			throw std::invalid_argument(
				"Non-square covariance matrix passed to multiNormal().");
		}
	}
	if(N != covar->size1) {
		try {
			std::string lenV = lexical_cast<std::string>(N);
			std::string lenM = lexical_cast<std::string>(covar->size1);
			throw std::invalid_argument("Vector of length " + lenV 
				+ " cannot be multiplied by " 
				+ lenM + "×" + lenM + " covariance matrix in multiNormal().");
		} catch (const boost::bad_lexical_cast &e) {
			throw std::invalid_argument(
				"Length of input vector to multiNormal() does not match dimensions of covariance matrix.");
		}
	}

	// Is the cache valid?
	
	// Note: isMatrixClose() is no longer used because it's 
	// significantly slower than gsl_matrix_equal(). The extra 
	// overhead from approximate comparison exceeded the negligible 
	// risk of a spurious cache miss.
	#ifdef _GSL_HAS_MATRIX_EQUAL
	if(prefixCache.get() == NULL || gsl_matrix_equal(covCache.get(), covar.get()) != 1) {
	#else
	if(prefixCache.get() == NULL || !matrixEqual(covCache.get(), covar.get())) {
	#endif
		// copy-and-swap to ensure the cache is only updated 
		//	if there are no exceptions
		shared_ptr<gsl_matrix> temp;
		matrixCopy(temp, covar);
		
		// assert: prefixCache is empty xor owns gsl_matrix_free()
		// assert: covar is non-empty and owns gsl_matrix_free()

		// Last operation in this block that is allowed to throw
		prefixCache = getHalfMatrix(covar);
		
		// assert: prefixCache is non-empty and owns gsl_matrix_free
		// assert: covar is non-empty and owns gsl_matrix_free()
		// assert: temp is non-empty and owns gsl_matrix_free()
		// assert: covCache is empty xor owns gsl_matrix_free()
		using std::swap;
		swap(covCache, temp);
		// assert: covCache is non-empty and owns gsl_matrix_free()
		// assert: temp is empty xor owns gsl_matrix_free()
	}
	
	// In C++11, can directly return a vector_const_view of indVec
	// For now, make a copy
	shared_ptr<gsl_vector> temp(checkAlloc(gsl_vector_alloc(N)), &gsl_vector_free);
	for(size_t i = 0; i < N; i++) {
		// assert: i is a valid index for both temp and indVec
		gsl_vector_set(temp.get(), i, indVec[i]);
	}

	// Storage for the output
	// calloc to prevent weirdness in the call to gsl_blas_dgemv
	shared_ptr<gsl_vector> result(checkAlloc(gsl_vector_calloc(N)), &gsl_vector_free);
	
	// Multiply the prefix matrix by the uncorrelated vector to 
	//	get the correlated one
	// The product is stored in result
	gslCheck( gsl_blas_dgemv(CblasNoTrans, 1.0, prefixCache.get(), temp.get(), 
		0.0, result.get()), "While generating multivariate normal vector: ");
	
	// corrVec untouched up to this point -- atomic guarantee satisfied

	// We'd want to make a copy even in a C++11 implementation
	corrVec.reserve(N);
	// vector::reserve() is atomic since doubles are nothrow-copyable
	
	// No reallocations, therefore no exceptions past this point
	corrVec.clear();
	for(size_t i = 0; i < N; i++) {
		// gsl_vector_get() doesn't invoke the error handler since 0 <= i < N
		corrVec.push_back(gsl_vector_get(result.get(), i));
	}
}

/** Given a matrix A, returns a matrix B with the property 
 *	@f$ A = B B^\intercal @f$
 *
 * @param[in] a The matrix to decompose
 *
 * @return a pointer containing the newly allocated matrix B
 *
 * @pre @p a is a square, symmetric, positive semi-definite matrix
 * @post return value is a newly allocated matrix with the property that 
 *	multiplying it by its own transpose restores @p a
 *
 * @warning Returns different results on KPF-Hewlett4 and on cowling, 
 *	particularly for matrices with lots of zero elements. The root cause 
 *	is that a call to gsl_eigen_symmv() returns eigenvalues in a different 
 *	order. Since the order of the eigenvalues is left undefined by the 
 *	specification of gsl_eigen_symmv(), and since the output of 
 *	getHalfMatrix() passes all relevant statistical tests on both 
 *	computers, its variant behavior is no longer considered a bug.
 *
 * @exception std::bad_alloc Thrown if there was not enough memory to 
 *	compute the transformation
 * @exception std::invalid_argument Thrown if the matrix is not square, 
 *	symmetric, or positive semi-definite.
 *
 * @exceptsafe The parameters are unchanged in the event of an exception.
 *
 * @todo Find a faster implementation
 */
shared_ptr<gsl_matrix> getHalfMatrix(const shared_ptr<gsl_matrix>& a) {
	// Eigendecomposition: covar = eigenVecs * diag(eigenVals) * transpose(eigenVecs)
	//	Note: need normalization convention so that eigenVecs is orthogonal
	//	Making the individual eigenvectors normalized meets this
	
	const size_t N = a->size1;
	
	shared_ptr<gsl_eigen_symmv_workspace> eigenWork(
			checkAlloc(gsl_eigen_symmv_alloc(N)), &gsl_eigen_symmv_free);
	shared_ptr<gsl_vector> eigenVals(checkAlloc(gsl_vector_alloc(N)), &gsl_vector_free);
	shared_ptr<gsl_matrix> eigenVecs(checkAlloc(gsl_matrix_alloc(N, N)), &gsl_matrix_free);
	
	// Since gsl_eigen_symmv modifies a matrix in place, make a copy first
	shared_ptr<gsl_matrix> aCopy;
	matrixCopy(aCopy, a);
			
	gslCheck( gsl_eigen_symmv(aCopy.get(), eigenVals.get(), eigenVecs.get(), 
			eigenWork.get()), "While generating multivariate normal vector: ");
			
	// Multiply eigenVecs by sqrt(diag(eigenVals)) to get a matrix 
	//	that when multiplied by its transpose produces covar
	for(size_t i = 0; i < N; i++) {
		// assert: i is a valid index for eigenVecs and eigenVals
		gsl_vector_view curCol = gsl_matrix_column(eigenVecs.get(), i);
		double curVal = gsl_vector_get(eigenVals.get(), i);
		
		if (curVal < 0.0) {
			// Correct for rounding problems
			if (curVal > -1e-12) {
				curVal = 0.0;
			} else {
				throw std::invalid_argument("Matrix is not positive semidefinite.");
			}
		}
		gslCheck( gsl_vector_scale(&(curCol.vector), sqrt(curVal)), 
			"While generating multivariate normal vector: ");
	}
	
	return eigenVecs;
}

/** Replaces one matrix with the value of another.
 *
 * Unlike gsl_matrix_memcpy(), allows matrices to have different sizes.
 *
 * @param[in,out] target The matrix pointer to be updated with a 
 *	copy of newData
 * @param[in] newData The data to copy to target.
 *
 * @post @p target points to a newly allocated matrix with the same dimensions 
 *	and data as @p newData
 * @post any data previously occupying @p target is cleaned up
 *
 * @exception std::bad_alloc Thrown if the matrix could not be copied.
 *
 * @exceptsafe The parameters are unchanged in the event of an exception.
 */
void matrixCopy(shared_ptr<gsl_matrix>& target, const shared_ptr<gsl_matrix>& newData) {
	using std::swap;
	
	shared_ptr <gsl_matrix> temp(
		checkAlloc(gsl_matrix_alloc(newData->size1, newData->size2)), 
		          &gsl_matrix_free);

	gslCheck( gsl_matrix_memcpy(temp.get(), newData.get()), "While copying matrix: "); 
	
	// IMPORTANT: no exceptions past this point
	
	// Swapping smart pointers is a safe operation
	swap(temp, target);
	
	// Any matrix previously in target will be deallocated here
}

#ifndef _GSL_HAS_MATRIX_EQUAL
/** Tests whether two matrices have approximately equal elements.
 *
 * This function is less efficient than gsl_is_matrix_equal(), but is 
 *	provided for compatibility with older versions of the GSL
 *
 * @param[in] a The first matrix to compare
 * @param[in] b The second matrix to compare
 *
 * @return True iff @p a and @p b have the same dimensions, and each 
 *	corresponding element is equal.
 *
 * @note if either @p a or @p b is a null pointer, returns false
 *
 * @exceptsafe Does not throw exceptions
 */
bool matrixEqual(const gsl_matrix* const a, const gsl_matrix* const b) {
	// Invalid matrices
	if(a == NULL || b == NULL) {
		return false;

	// Mismatched matrices
	} else if (a->size1 != b->size1 || a->size2 != b->size2) {
		return false;
	
	// Element-by-element comparison
	} else {
		for(size_t i = 0; i < a->size1; i++) {
			for(size_t j = 0; j < a->size2; j++) {
				// assert: i and j are valid indices
				if (gsl_matrix_get(a, i, j) != gsl_matrix_get(b, i, j)) {
					return false;
				}
			}
		}
		
		// All elements compared equal
		return true;
	}
}
#endif

}}		// end lcmc::utils
