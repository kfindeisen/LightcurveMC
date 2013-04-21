/** Uncopyable interface
 * @file uncopyable.h
 * @author Krzysztof Findeisen
 * @date Created July 22, 2011
 * @date Last modified April 21, 2013
 */
 
#ifndef UNCOPYABLE_H
#define UNCOPYABLE_H

/** Interface for classes that cannot be meaningfully copied. Attempting to 
 * perform copy-construction of or assignment to an object of a class that 
 * implements this interface will lead to a compiler error. Likewise, 
 * attempting to define either a copy-constructor or an assignment operator 
 * for a class that implements this interface will cause a compiler error.
 * 
 * Shamelessly copied from Effective C++, 3rd ed.
 *
 * @see IUnassignable
 */

class IUncopyable {
public:
	virtual ~IUncopyable() = 0;	// Explicitly defined because class doesn't auto-compile

protected:
	/** Generic constructor
	 */
	// Protected because implementing classes must be constructable
	IUncopyable() {};
private:
	/** Copy-constructor cannot be called by subclasses
	 */
	IUncopyable(const IUncopyable&);
	/** Assignment operator cannot be called by subclasses
	 */
	IUncopyable& operator=(const IUncopyable&) const;
};

/** Interface for classes that can be meaningfully copied but not overwritten. 
 * Attempting to assign to an object of a class that implements this 
 * interface will lead to a compiler error. Likewise, attempting to define an 
 * assignment operator for a class that implements this interface will cause a 
 * compiler error.
 *
 * @see IUncopyable
 */
class IUnassignable {
public:
	virtual ~IUnassignable() = 0;	// Explicitly defined because class doesn't auto-compile

protected:
	/** Generic constructor
	 */
	// Protected because implementing classes must be constructable
	IUnassignable() {};
	/** Generic copy-constructor
	 */
	IUnassignable(const IUnassignable&) {};
private:
	/** Assignment operator cannot be called by subclasses
	 */
	IUnassignable& operator=(const IUnassignable&) const;
};

#endif
