/** Template class for allocating/deallocating generic GSL structures
 *  @file raiigsl.tmp.h
 *  @author Krzysztof Findeisen
 *  @date Created December 29, 2011
 *  @date Last modified May 5, 2013
 */

#ifndef RAIIGSLH
#define RAIIGSLH

#include <boost/utility.hpp>
#include "warnflags.h"

// GCC isn't clever enough to realize that I've overridden the copy-constructor 
//	and assignment operator, so it complains about the danger
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// GCC isn't clever enough to realize that I've overridden the copy-constructor 
//	and assignment operator, so it complains about the danger
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

/** RAII wrapper for gsl structures
 * 
 * RaiiGsl manages the pointer using the *_alloc() and *_free() commands, 
 *	rather than new and delete. Otherwise, its functionality is similar to 
 *	std::auto_ptr and std::unique_ptr
 *
 * @tparam T a structure in the GSL, such as gsl_vector or gsl_rng.
 */
template <typename T> class RaiiGsl : boost::noncopyable {
public:
	/** Deallocator represents a function that can free the memory in an 
	 * object of type T
	 */
	typedef void Deallocator(T*);
	
	/** Constructs a RaiiGsl object from a gsl_* pointer and a pointer 
	 * to the appropriate deallocation function
	 */
	RaiiGsl (T* p, Deallocator* freeFunc);
	/** Deallocates the gsl struct being managed by this object
	 */
	~RaiiGsl();
	
	/** Returns a pointer to the struct managed by this object
	 */
	T* get() const;
	/** Returns a reference to the struct managed by this object
	 */
	T& operator*() const;
	/** Returns the struct managed by this object in order to access one of its members.
	 */
	T* operator->() const;
	
	/** Stops managing the currently managed struct
	 */
	T* release();

	/** Deallocates the currently managed struct.
	 */
	void reset();

	/** Begins managing a new struct, deallocating any currently 
	 *	managed struct first
	 */
	void reset(T* p);

private:
	T* internal;
	Deallocator* internalDestructor;
};

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif







//-----------------------------------------------------------------------------
// 
// Definitions start here
// 

/** Constructs a RaiiGsl object from a gsl_* pointer and a pointer 
 * to the appropriate deallocation function
 *
 * @param[in] p A pointer to the struct whose memory is to be managed by 
 *	the RaiiGsl object
 * @param[in] freeFunc A pointer to a function that takes a single argument, 
 *	a pointer, and deallocates the struct pointed to by the pointer
 *
 * @pre *p has no smart pointers managing it
 * @pre the caller does not attempt to deallocate p manually
 *
 * @post *p will be correctly deallocated when RaiiGsl is destroyed
 */
template <typename T> RaiiGsl<T>::RaiiGsl(T* p, RaiiGsl<T>::Deallocator* freeFunc)
		: internal(p), internalDestructor(freeFunc) {
}

/** Deallocates the gsl struct being managed by this object
 */
template <typename T> RaiiGsl<T>::~RaiiGsl() {
	if (internal != NULL) {
		(*internalDestructor)(internal);	// equivalent to calling delete
		internal = NULL;
	}
}

/** Returns a pointer to the struct managed by this object
 *
 * @return an rvalue pointer that can be passed to GSL functions.
 */
template <typename T> T* RaiiGsl<T>::get() const {
	return internal;
}

/** Returns a reference to the value pointed by this 
 *
 * @return an lvalue that can be used to directly manipulate 
 * 	the struct
 */
template <typename T> T& RaiiGsl<T>::operator*() const {
	return *internal;
}

/** Returns the struct managed by this object in order to access one 
 * of its members.
 *
 * @return a pointer p that behaves as if one called p->()
 */
template <typename T> T* RaiiGsl<T>::operator->() const {
	return internal;
}

/** Stops managing the internal object without deallocating it
 *
 * @return a pointer to the formerly managed struct
 *
 * @post Responsibility for managing the struct passes to the caller
 */
template <typename T> T* RaiiGsl<T>::release() {
	T* foo = internal;
	internal = NULL;
	return foo;
}

/** Deallocates the currently managed struct.
 *
 * @post The RaiiGsl object is no longer associated with a GSL struct.
 * @post Any memory associated with a previously managed struct is freed. 
 *	If the RaiiGsl object was not previously managing a struct, nothing 
 *	happens.
 */
template <typename T> void RaiiGsl<T>::reset() {
	if (internal != NULL) {
		(*internalDestructor)(internal);	// equivalent to calling delete
		internal = NULL;
	}
}

/** Begins managing a new struct, deallocating any currently 
 *	managed struct first
 *
 * @param[in] p A pointer to the struct whose memory is to be managed by 
 *	the RaiiGsl object
 *
 * @pre *p has no smart pointers managing it
 * @pre the caller does not attempt to deallocate p manually
 *
 * @post The RaiiGsl object manages the GSL struct pointed to by p.
 * @post Any memory associated with a previously managed struct is freed. 
 *	If the RaiiGsl object was not previously managing a struct, nothing 
 *	happens.
 */
template <typename T> void RaiiGsl<T>::reset(T* p) {
	if (internal != NULL) {
		(*internalDestructor)(internal);	// equivalent to calling delete
	}
	internal = p;
}

#endif		// RAIIGSLH
