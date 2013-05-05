/** Generates a multivariate normal with a given covariance matrix
 * @file multinormal.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 4, 2013
 */

#include <vector>
#include <cmath>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_version.h>
#include "generators.h"
#include "../raiigsl.tmp.h"

namespace lcmc { namespace utils {

/** Tests whether two matrices have approximately equal elements.
 *
 * This function is less efficient than gsl_is_matrix_equal(), but is 
 *	provided for compatibility with older versions of the GSL
 *
 * @param[in] a The first matrix to compare
 * @param[in] b The second matrix to compare
 *
 * @return True iff a and b have the same dimensions, and each 
 *	corresponding element is equal.
 *
 * @note if either a or b is a null pointer, returns false
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
				if (gsl_matrix_get(a, i, j) != gsl_matrix_get(b, i, j)) {
					return false;
				}
			}
		}
		
		// All elements compared equal
		return true;
	}
}

using std::vector;

/** Replaces one matrix with another
 *
 * @param[in,out] target The matrix pointer to be updated with a 
 *	copy of newData
 * @param[in] newData The data to copy to target.
 *
 * @post target points to a newly allocated matrix with the same dimensions 
 *	and data as newData
 * @post any data previously occupying target is cleaned up
 */
void replaceMatrix(gsl_matrix* & target, const gsl_matrix* const newData) {
	if (target != NULL) {
		gsl_matrix_free(target);
	}
	target = gsl_matrix_alloc(newData->size1, newData->size1);
	gsl_matrix_memcpy(target, newData);
}

/** Given a matrix A, returns a matrix B with the property 
 *	A = B * transpose(B)
 *
 * @param[in] a The matrix to decompose
 * @param[out] b A pointer to which the result matrix B is assigned
 *
 * @pre a is a square, symmetric, positive semi-definite matrix
 * @post b points to a newly allocated matrix with the property that 
 *	multiplying it by its own transpose restores A
 * @post any data previously pointed to by b is deleted
 *
 * @todo Benchmark which of several possible implementations is faster
 *
 * @warning Returns different results on KPF-Hewlett4 and on cowling, 
 *	particularly for matrices a with lots of zero elements. The root cause 
 *	is that a call to gsl_eigen_symmv() returns eigenvalues in a different 
 *	order. Since the order of the eigenvalues is left undefined by the 
 *	specification of gsl_eigen_symmv(), and since the output of 
 *	getHalfMatrix() passes all relevant statistical tests on both 
 *	computers, its variant behavior is no longer considered a bug.
 */
void getHalfMatrix(const gsl_matrix* const a, gsl_matrix* & b) {
	// Eigendecomposition: covar = eigenVecs * diag(eigenVals) * transpose(eigenVecs)
	//	Note: need normalization convention so that eigenVecs is orthogonal
	//	Making the individual eigenvectors normalized meets this
	
	const size_t N = a->size1;
	
	RaiiGsl<gsl_eigen_symmv_workspace> eigenWork(
			gsl_eigen_symmv_alloc(N), &gsl_eigen_symmv_free);
	RaiiGsl<gsl_vector> eigenVals(gsl_vector_alloc(N), &gsl_vector_free);
	RaiiGsl<gsl_matrix> eigenVecs(gsl_matrix_alloc(N, N), &gsl_matrix_free);
	
	// Since gsl_eigen_symmv modifies a matrix in place, 
	//	make a copy first
	RaiiGsl<gsl_matrix> aCopy(gsl_matrix_alloc(N, N), &gsl_matrix_free);
	gsl_matrix_memcpy(aCopy.get(), a);
			
	gsl_eigen_symmv(aCopy.get(), eigenVals.get(), eigenVecs.get(), eigenWork.get());
			
	// Multiply eigenVecs by sqrt(diag(eigenVals)) to get a matrix 
	//	that when multiplied by its transpose produces covar
	for(size_t i = 0; i < N; i++) {
		gsl_vector_view curCol = gsl_matrix_column(eigenVecs.get(), i);
		double curVal = gsl_vector_get(eigenVals.get(), i);
		// Correct for rounding problems
		if (curVal < 0.0 && curVal > -1e-12) {
			curVal = 0.0;
		}
		gsl_vector_scale(&(curCol.vector), sqrt(curVal));
	}
	
	// Export the result
	replaceMatrix(b, eigenVecs.get());
}

/** Transforms an uncorrelated sequence of Gaussian random numbers into a 
 *	correlated sequence
 *
 * @param[in] indVec A vector of independent unit Gaussian random numbers.
 * @param[in] covar The desired covariance matrix.
 * @param[out] corrVec A vector of correlated Gaussian random numbers with 
 *	mean zero and covariance matrix covar.
 *
 * @pre indVec.size() == covar.size1 == covar.size2
 * @pre corrVec may refer to the same vector as indVec
 *
 * @post corrVec.size() == indVec.size()
 *
 * @todo Add input validation
 */
void multiNormal(const vector<double>& indVec, const gsl_matrix& covar, 
		vector<double>& corrVec) {
	static gsl_matrix*    covCache = NULL;
	static gsl_matrix* prefixCache = NULL;

	const size_t N = indVec.size();

	// Is the cache valid?
	
	// Note: isMatrixClose() is no longer used because it's 
	// significantly slower than gsl_matrix_equal(). The extra 
	// overhead from approximate comparison exceeded the negligible 
	// risk of a spurious cache miss.
	#if GSL_MINOR_VERSION >= 15
	if(prefixCache == NULL || gsl_matrix_equal(covCache, &covar) != 1) {
	#else
	if(prefixCache == NULL || !matrixEqual(covCache, &covar)) {
	#endif
		replaceMatrix(covCache, &covar);
		
		getHalfMatrix(&covar, prefixCache);
	}
	
	// In C++11, can directly return a vector_const_view of indVec
	// For now, make a copy
	RaiiGsl<gsl_vector> temp(gsl_vector_alloc(N), &gsl_vector_free);
	for(size_t i = 0; i < N; i++) {
		gsl_vector_set(temp.get(), i, indVec[i]);
	}
	// Storage for the output
	// calloc to prevent weirdness in the call to gsl_blas_dgemv
	RaiiGsl<gsl_vector> result(gsl_vector_calloc(N), &gsl_vector_free);
	
	// Multiply the prefix matrix by the uncorrelated vector to 
	//	get the correlated one
	// The product is stored in result
	gsl_blas_dgemv(CblasNoTrans, 1.0, prefixCache, temp.get(), 0.0, result.get());

	// We'd want to make a copy even in a C++11 implementation
	corrVec.clear();
	corrVec.reserve(N);
	for(size_t i = 0; i < N; i++) {
		corrVec.push_back(gsl_vector_get(result.get(), i));
	}
}

}}		// end lcmc::utils
