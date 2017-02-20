//===============================================================================
// Copyright (c) 2005-2017 by Made to Order Software Corporation
// 
// All Rights Reserved.
// 
// The source code in this file ("Source Code") is provided by Made to Order Software Corporation
// to you under the terms of the GNU General Public License, version 2.0
// ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
// 
// By copying, modifying or distributing this software, you acknowledge
// that you have read and understood your obligations described above,
// and agree to abide by those obligations.
// 
// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
// COMPLETENESS OR PERFORMANCE.
//===============================================================================



#ifndef MO_SMARTPTR_H
#define	MO_SMARTPTR_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

// read the configuration file first
#ifdef HAVE_CONFIG_H
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
#include	"config.h"
#endif
#endif
#include	"mo_config.h"



namespace molib
{





class MO_DLL_EXPORT moBase;
#include "mo_controlled.h"




/*!
 * \class moSmartPtr
 * \brief Handles reference counted object pointers.
 *
 * All the objects in the mo library which are derived
 * from the moBase object have a reference counter. In
 * order to manage that counter, we use smart pointers.
 * These templates enables you to avoid calling the AddRef()
 * and Release() yourself and hides that totally making it
 * really hard for you to forget any reference count.
 * 
 * There are two types of pointers and three types of
 * smart pointers:
 * 
 * \code
 * 	1. Static pointers
 * 	2. Dynamic pointers
 * \endcode
 * 
 * Whenever an object is created using the new operator, a
 * dynamic object is created. This means it needs to be
 * deleted either using a Release() or the delete operator
 * if you don't use smart pointers.
 * 
 * The smart pointers are based on three classes which are
 * the moAnyPtr, moDynamicPtr and moStaticPtr classes
 * available in the Ptr namespace. A smart pointer defined
 * with moAnyPtr (i.e. moSmartPtr) can access either a dynamic
 * or a static pointer. This should be used for stack smart
 * pointers. The dynamic smart pointer should be used in all
 * other cases. I have no idea when a static smart pointer
 * should be used but it is available for completness.
 * 
 * The following explains how to use the basic smart pointer
 * (the one using the moAnyPtr class) template. All the
 * instructions apply to the other smart pointers as is.
 * 
 * To declare your objects as smart pointers, use a typedef
 * as follow:
 * 
 * \code
 * 	typedef moSmartPtr<moBase>	moBaseSPtr;
 * \endcode
 * 
 * Then never ever even think of using 'moBase *' as a type
 * anywhere ever, except maybe as the parameter of a function
 * since the caller will have a reference to that object
 * until the function returns (this is true even if an
 * exception occurs).
 * 
 * The functions declared in the moSmartPtr are as follow:
 * 
 * \section constructors Constructors
 * 
 * \code
 * 	moSmartPtr(void);
 * 	template<class C> moSmartPtr(C *ptr);
 * 	moSmartPtr(const moSmartPtr<T>& sptr);
 * \endcode
 * 
 * Initialize a smart pointer to null, a bare pointer or copy
 * the pointer of another smart pointer. In case the pointer
 * is not null it's AddRef() function will be called.
 * 
 * \section assign_ops Assignment operators
 * 
 * \code
 * 	moSmartPtr<T>& operator = (int zero);
 * 	template<class C> moSmartPtr<T>& operator = (const C *ptr);
 * 	template<class C> moSmartPtr& operator = (const moSmartPtr<C>& sptr);
 * \endcode
 * 
 * Set a pointer to zero (case of an integer). This was added
 * because otherwise trying to set a pointer to NULL results
 * in the compiler complaining about having a choice between
 * the two other functions.
 * 
 * Otherwise, the two other function will either copy a bare
 * pointer or the pointer of another smart pointer in this
 * smart pointer. It first AddRef()'s the new pointer and then
 * releases the old pointer. The Smart Pointer then saves the
 * new pointer in itself.
 * 
 * \section ptr_section Pointer (Arrow/Asterisk) and reference operators
 * 
 * \code
 * 	T *operator -> (void);
 * 	const T *operator -> (void);
 * 	operator T * (void);
 * 	operator T * (void) const;
 * 	operator T& (void);
 * 	operator T& (void) const;
 * 
 * 	template<class C> operator C * (void);
 * 	template<class C> operator C * (void) const;
 * 	template<class C> operator C& (void);
 * 	template<class C> operator C& (void) const;
 * \endcode
 * 
 * Returns a bare pointer or reference to the object held by
 * this smart pointer (arrow and asterisk). The pointer is
 * neither AddRef()'d nor Release()'d in anyway when these
 * operators are used.
 * 
 * Note that the templatized version of these operators
 * are used whenever a dynamic cast is necessary on the
 * object prior to the use of the object. It is similar
 * to getting the pointer as T and dynamic casting it:
 * 
 * \code
 * 	FooSPtr foo;
 * 
 * 	tst = dynamic_cast<Test *>(foo);
 * \endcode
 * 
 * and
 * 
 * \code
 * 	tst = static_cast<Test *>(foo);
 * \endcode
 * 
 * are equivalent with these operators.
 * 
 * The following is a good example of usage:
 * 
 * \code
 *   T *t;
 *   t = new T;
 *   C *c;
 *   c = t;
 * \endcode
 * 
 * will automatically convert 't' to 'c' using a proper
 * dynamic_cast<> from the smart pointer. If the type T *
 * can't be dynamically casted to C *, then the compiler
 * will err at compile time.
 * 
 * \section test_section Test pointer
 * 
 * \code
 * 	operator bool (void);
 * 	operator bool (void) const;
 * 	operator ! (void);
 * 	operator ! (void) const;
 * \endcode
 * 
 * The boolean operator will in general be automatically used
 * by the C++ compiler whenever necessary (Except in assert()
 * calls). This means a smart pointer can be used wherever a
 * boolean is expected and the result will be true when the
 * pointer isn't null.
 * 
 * The not operator returns the opposite value that the bool
 * operator returns (when bool returns true the ! returns false
 * and vice versa). It is often used in assert() calls as
 * follow:
 * 
 * \code
 * 	assert(!!foo);
 * \endcode
 * 
 * This assert ensures that foo is not a null pointer.
 * 
 * It is important to be noted that an assert() expect an
 * integer and thus the bool operator won't automatically
 * be used with it. However, the if() statement will in
 * most cases work as is:
 * 
 * \code
 * 	if(foo) { ... }
 * \endcode
 * 
 * means whenever foo is not null, execute { ... }.
 * 
 * \section comp_section Comparison operators
 * 
 * \code
 * 	template<class C> bool operator == (const C *ptr) const;
 * 	template<class C> bool operator != (const C *ptr) const;
 * 	template<class C> bool operator <= (const C *ptr) const;
 * 	template<class C> bool operator <  (const C *ptr) const;
 * 	template<class C> bool operator >= (const C *ptr) const;
 * 	template<class C> bool operator >  (const C *ptr) const;
 * 	template<class C> bool operator == (const moSmartPtr<C>& sptr) const;
 * 	template<class C> bool operator != (const moSmartPtr<C>& sptr) const;
 * 	template<class C> bool operator <= (const moSmartPtr<C>& sptr) const;
 * 	template<class C> bool operator <  (const moSmartPtr<C>& sptr) const;
 * 	template<class C> bool operator >= (const moSmartPtr<C>& sptr) const;
 * 	template<class C> bool operator >  (const moSmartPtr<C>& sptr) const;
 * 	template<class C> friend bool operator == (const C *ptr, const moSmartPtr<C>& sptr);
 * 	template<class C> friend bool operator != (const C *ptr, const moSmartPtr<C>& sptr);
 * 	template<class C> friend bool operator <= (const C *ptr, const moSmartPtr<C>& sptr);
 * 	template<class C> friend bool operator <  (const C *ptr, const moSmartPtr<C>& sptr);
 * 	template<class C> friend bool operator >= (const C *ptr, const moSmartPtr<C>& sptr);
 * 	template<class C> friend bool operator >  (const C *ptr, const moSmartPtr<C>& sptr);
 * \endcode
 * 
 * All of the comparison operators return the comparitive
 * result of two pointers. It works with bare pointers as
 * well as smart pointers.
 * 
 * There is an equivalent set of functions with the input
 * pointer parameters not being constant which once in a
 * while is necessary (otherwise you get warnings with g++).
 * 
 * \section ptr_as_section Pointer as argument to function calls
 * 
 * \code
 * 	T *& ArgAsRefType(void);
 * 	T **ArgAsType(void);
 * 	void **ArgAsVoid(void);
 * \endcode
 * 
 * These functions request a reference or a pointer (possibly of
 * type void?!) to the pointer held by the smart pointer template.
 * This is useful whenever you need to pass a bare pointer reference
 * or direct pointer as a parameter to a function call.
 * 
 * The previous pointer held by the smart pointer is released
 * and then a pointer or reference to the smart pointer held
 * pointer is returned.
 * 
 * The function is responsible to save the pointer after it was
 * AddRef()'ed. There is an example:
 * 
 * \code
 * 	...
 * 	MyFunc(foo.ArgAsRefType());
 * 	...
 * 
 * 	void MyFunc(Foo *& foo)
 * 	{
 * 		...
 * 		foo = new Foo;
 * 		foo->AddRef();	// a bare pointer needs this!
 * 		...
 * 	}
 * \endcode
 * 
 * It is strongly recommanded that smart pointers be used in
 * all of these cases.
 * 
 * \bug
 * The Arg<name>() functions return a pointer or reference to
 * a pointer. This is passed to a sub-function as a parameter.
 * That sub-function receives that pointer or reference which
 * at that time is not linked to the smart pointer any more.
 * This means the function is free to do anything it wants,
 * including put a static pointer in a dynamic smart pointer
 * (and vice versa) or forget to AddRef() the object.
 * 
 * \section other_funcs Other functions
 * 
 * \code
 * 	template<class C> void Set(C *ptr);
 * \endcode
 * 
 * This function should never be used, but in the very few cases
 * where you get a pointer and need to put it in a smart pointer
 * without AddRef()'ing it. This function accomplishes just that.
 * 
 * \code
 * 	void New(void);
 * \endcode
 * 
 * This function creates an object of type T and puts it properly
 * AddRef()'ed in the smart pointer. The type T needs to have a
 * constructor with no parameters. Note that this calls fails
 * in moSmartStaticPtr objects since it creates a dynamic object.
 *
 * \bug
 * You should never use a smart pointer as a global variable.
 * This is because you can't know when it will be initialized
 * and if you started to use your smart pointer before it is
 * initialized, you will have a memory leak and you actually
 * will lose that first pointer you created. Look at it this
 * way:
 *
 * \code
 *	// Global 1
 *	class Foo
 *	{
 *		Foo()
 *		{
 *			// this initialize the pointer
 *			GetFactory()->Append();
 *		}
 *	}
 *	Foo g_foo;
 *
 *	// Global 2
 *	moListSPtr g_list;
 *
 *	moListSPtr GetFactory()
 *	{
 *		if(!g_list) {
 *			g_list = new List;
 *		}
 *		return g_list;
 *	}
 * \endcode
 * 
 * If Global 2 constructors are called first, then the g_list
 * smart pointer will be initialized properly. If Global 1
 * is called first, then GetFactory() gets called and the
 * g_list smart pointer is set to 'new List' without first
 * having been initialized. Later, when Global 2 is then
 * initialized, the smart pointer constructor is called and
 * that has the effect of reinitializing the smart pointer
 * to null not expecting that whatever was there before was
 * a valid pointer.
 * 
 * The easiest is to avoid smart pointers in this case.
 * There is no valid way to tell whether the constructor
 * was already called. The only thing which would work is
 * that the smart pointer somehow detects that it is defined
 * in the .bss section... And what if it ends up in the .data?!
 *
 * \sa moBase::AddRef, moBase::Release, Ptr::moAnyPtr, Ptr::moDynamicPtr, Ptr::moStaticPtr
 *
 */

/// SmartPointer implementation details
namespace Ptr {
class MO_DLL_EXPORT moAnyPtr
{
public:
//				moAnyPtr(void) {}
//				moAnyPtr(const moAnyPtr& src) {}
//	moAnyPtr&		operator = (const moAnyPtr& src) { return *this; }
	static void		CheckPtr(const moBase *ptr);
};
class MO_DLL_EXPORT moDynamicPtr
{
public:
//				moDynamicPtr(void) {}
//				moDynamicPtr(const moDynamicPtr& src) {}
//	moDynamicPtr&		operator = (const moDynamicPtr& src) { return *this; }
	static void		CheckPtr(const moBase *ptr);
};
class MO_DLL_EXPORT moStaticPtr
{
public:
//				moStaticPtr(void) {}
//				moStaticPtr(const moStaticPtr& src) {}
//	moStaticPtr&		operator = (const moStaticPtr& src) { return *this; }
	static void		CheckPtr(const moBase *ptr);
};
};		// namespace Ptr


#ifdef MO_DEBUG
template<class T>
class MO_DLL_EXPORT_TMPL moForbiddenAddRefRelease : public T
{
public:
	unsigned long		AddRef(void) const { fprintf(stderr, "do not AddRef() a smart pointer"); assert(0); return 0; }
	unsigned long		Release(void) { fprintf(stderr, "do not Release() a smart pointer"); assert(0); return 0; }
};
#endif

template<class T, class PTR>
class MO_DLL_EXPORT_TMPL moSmartPtrBase : private PTR
{
public:
				moSmartPtrBase(void)
					: PTR()
				{
					f_ptr = 0;
#ifdef MO_DEBUG
					f_initialized = true;
#endif
				}
				moSmartPtrBase(const T *ptr)
					: PTR()
				{
					PTR::CheckPtr(ptr);
					f_ptr = const_cast<T *>(ptr);
					if(f_ptr) {
						f_ptr->AddRef();
					}
#ifdef MO_DEBUG
					f_initialized = true;
#endif
				}
				moSmartPtrBase(const moSmartPtrBase<T, PTR>& sptr)
					: PTR()
				{
					PTR::CheckPtr(sptr.f_ptr);
					f_ptr = const_cast<T *>(sptr.f_ptr);
					if(f_ptr) {
						f_ptr->AddRef();
					}
#ifdef MO_DEBUG
					f_initialized = true;
#endif
				}
				~moSmartPtrBase()
				{
					if(f_ptr) {
						f_ptr->Release();
					}
#ifdef MO_DEBUG
					f_initialized = false;
#endif
				}



#ifdef MO_DEBUG
				void Validate(void) const
				{
					assert(f_initialized);
				}
#else
				void Validate(void) const
				{
				}
#endif

#if 0
	// special case to be able to set a smart pointer to NULL
	moSmartPtrBase<T, PTR>&	operator = (int zero)
				{
					assert(zero == 0);
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = 0;
					return *this;
				}
#endif

	moSmartPtrBase<T, PTR>&	operator = (const T *ptr)
				{
					Validate();
					PTR::CheckPtr(ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(ptr);
					if(f_ptr) {
						f_ptr->AddRef();
					}
					if(old_ptr) {
						old_ptr->Release();
					}
					return *this;
				}
	moSmartPtrBase<T, PTR>&	operator = (const moSmartPtrBase<T, PTR>& sptr)
				{
					Validate();
					PTR::CheckPtr(sptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(static_cast<const T *>(sptr));
					if(f_ptr) {
						f_ptr->AddRef();
					}
					if(old_ptr) {
						old_ptr->Release();
					}
					return *this;
				}

	T *			operator -> (void) { Validate(); return f_ptr; }
	const T *		operator -> (void) const { Validate(); return f_ptr; }
				operator T * (void) { Validate(); return f_ptr; }
				operator T * (void) const { Validate(); return f_ptr; }
				operator T& (void) { Validate(); return *f_ptr; }
				operator T& (void) const { Validate(); return *f_ptr; }

#if 0
// this breaks when you do
// Smart Pointer A = Smart Pointer B
// that's because it will try to cast the pointer of B
// to a Smart Pointer A and not to the type of A.
				template<class C>
				moSmartPtrBase(const C *ptr)
				{
					PTR::CheckPtr(ptr);
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
					if(f_ptr) {
						f_ptr->AddRef();
					}
				}
				template<class C>
				moSmartPtrBase(const moSmartPtrBase<C, PTR>& sptr)
				{
					PTR::CheckPtr(static_cast<const C *>(sptr));
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(static_cast<const C *>(sptr)));
					if(f_ptr) {
						f_ptr->AddRef();
					}
				}
	template<class C>
	moSmartPtrBase<T, PTR>&	operator = (const C *ptr)
				{
					PTR::CheckPtr(ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
					if(f_ptr) {
						f_ptr->AddRef();
					}
					if(old_ptr) {
						old_ptr->Release();
					}
					return *this;
				}
	template<class C>
	moSmartPtrBase<T, PTR>&	operator = (const moSmartPtrBase<C, PTR>& sptr)
				{
					PTR::CheckPtr(static_cast<const C *>(sptr));
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(static_cast<const C *>(sptr)));
					if(f_ptr) {
						f_ptr->AddRef();
					}
					if(old_ptr) {
						old_ptr->Release();
					}
					return *this;
				}
				template<class C>
				operator C * (void) const
				{
					return dynamic_cast<const C *>(f_ptr);
				}

				template<class C>
				operator C * (void)
				{
					return dynamic_cast<C *>(f_ptr);
				}

				template<class C>
				operator C& (void) const
				{
					return dynamic_cast<const C&>(*f_ptr);
				}

				template<class C>
				operator C& (void)
				{
	fprintf(stderr, "f_ptr = %p\n", f_ptr);
	const std::type_info& info = typeid(*f_ptr);
	fprintf(stderr, "Type = [%s]\n", info.name());
	fprintf(stderr, "But C = [%s]\n", typeid(C).name());
					return dynamic_cast<C&>(*f_ptr);
				}

				// the following is what's missing but
				// you actually can't write anything
				// like that; it won't work!
				template<class C>
				operator moSmartPtrBase<C, PTR>& (void)
				{
					return dynamic_cast<C&>(f_ptr);
				}
#endif

				operator bool (void) { Validate(); return f_ptr != 0; }
				operator bool (void) const { Validate(); return f_ptr != 0; }
	bool			operator ! (void) { Validate(); return f_ptr == 0; }
	bool			operator ! (void) const { Validate(); return f_ptr == 0; }

#if 0
// I don't see how this would be valid on a smart pointer?!
	moSmartPtrBase<T, PTR>&	operator ++ (void) const { ++f_ptr; return *this; }
	moSmartPtrBase<T, PTR>&	operator ++ (int) const { moSmartPtrBase<T, PTR> p(*this); ++f_ptr; return *this; }
	moSmartPtrBase<T, PTR>&	operator -- (void) const { return --f_ptr; }
	moSmartPtrBase<T, PTR>&	operator -- (int) const { T *p(f_ptr); --f_ptr; return p; }

	moSmartPtrBase<T, PTR>&	operator += (int offset) const { return f_ptr += offset; }
	moSmartPtrBase<T, PTR>&	operator -= (int offset) const { return f_ptr -= offset; }

	T *			operator + (int offset) const { return f_ptr + offset; }
	T *			operator - (int offset) const { return f_ptr - offset; }
	friend T *		operator + (int offset, const moSmartPtrBase<T, PTR>& sptr) const { return offset + sptr.f_ptr; }
#endif

	template<class C>
	bool			operator == (C *ptr) const { Validate(); return f_ptr == ptr; }
	template<class C>
	bool			operator == (const C *ptr) const { Validate(); return f_ptr == ptr; }
	template<class C>
	bool			operator != (C *ptr) const { Validate(); return f_ptr != ptr; }
	template<class C>
	bool			operator != (const C *ptr) const { Validate(); return f_ptr != ptr; }
	template<class C>
	bool			operator <= (const C *ptr) const { Validate(); return f_ptr <= ptr; }
	template<class C>
	bool			operator <  (const C *ptr) const { Validate(); return f_ptr <  ptr; }
	template<class C>
	bool			operator >= (const C *ptr) const { Validate(); return f_ptr >= ptr; }
	template<class C>
	bool			operator >  (const C *ptr) const { Validate(); return f_ptr >  ptr; }

	template<class C>
	bool			operator == (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr == static_cast<const C *>(sptr); }
	template<class C>
	bool			operator != (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr != static_cast<const C *>(sptr); }
	template<class C>
	bool			operator <= (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr <= static_cast<const C *>(sptr); }
	template<class C>
	bool			operator <  (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr <  static_cast<const C *>(sptr); }
	template<class C>
	bool			operator >= (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr >= static_cast<const C *>(sptr); }
	template<class C>
	bool			operator >  (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr >  static_cast<const C *>(sptr); }

	template<class C>
	friend bool		operator == (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr == sptr.f_ptr; }
	template<class C>
	friend bool		operator != (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr != sptr.f_ptr; }
	template<class C>
	friend bool		operator <= (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr <= sptr.f_ptr; }
	template<class C>
	friend bool		operator <  (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr <  sptr.f_ptr; }
	template<class C>
	friend bool		operator >= (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr >= sptr.f_ptr; }
	template<class C>
	friend bool		operator >  (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr >  sptr.f_ptr; }

	bool			operator && (bool b)
				{
					return f_ptr != 0 && b;
				}
	friend bool		operator && (bool b, const moSmartPtrBase<T, PTR>& sptr)
				{
					return sptr.f_ptr != 0 && b;
				}

	T *&			ArgAsRefType(void)
				{
					Validate();
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = 0;
					return f_ptr;
				}

	T **			ArgAsType(void)
				{
					Validate();
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = 0;
					return &f_ptr;
				}

	void **			ArgAsVoid(void)
				{
					Validate();
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = 0;
					return static_cast<void **>(&f_ptr);
				}

	// Whenever you have a problem with an assignment operator
	// then use the copy; especially useful if you need a cast!
	template<class C>
	void			Copy(const C *ptr)
				{
					Validate();
					PTR::CheckPtr(ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
					if(f_ptr) {
						f_ptr->AddRef();
					}
					if(old_ptr) {
						old_ptr->Release();
					}
				}

	template<class C>
	void			Copy(const moSmartPtrBase<C, PTR>& sptr)
				{
					Validate();
					PTR::CheckPtr(sptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(static_cast<const C *>(sptr)));
					if(f_ptr) {
						f_ptr->AddRef();
					}
					if(old_ptr) {
						old_ptr->Release();
					}
				}

	// The Set() function does NOT increment the reference counter.
	// I don't think this is even necessary! (it was until I wrote
	// the correct reference count whether an object is created
	// with the new operator or not)
	template<class C>
	void			Set(const C *ptr)
				{
					Validate();
					PTR::CheckPtr(ptr);
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
				}

	// create a new instance (releasing the previous one first)
	// works only if you have a constructor without any parameter
	void			New(void)
				{
					Validate();
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = new T;
					PTR::CheckPtr(f_ptr);
					if(f_ptr) {
						f_ptr->AddRef();
					}
				}

	bool			IsNull(void) const
				{
					Validate();
					return f_ptr == 0;
				}

private:
	T *			f_ptr;
#ifdef MO_DEBUG
	zbool_t			f_initialized;
#endif
};

template<class T>
class MO_DLL_EXPORT_TMPL moSmartPtr : public moSmartPtrBase<T, Ptr::moAnyPtr>
{
public:
				moSmartPtr(void)
					: moSmartPtrBase<T, Ptr::moAnyPtr>()
				{
				}
				moSmartPtr(const T *ptr)
					: moSmartPtrBase<T, Ptr::moAnyPtr>(ptr)
				{
				}
				moSmartPtr(const moSmartPtr<T>& sptr)
					: moSmartPtrBase<T, Ptr::moAnyPtr>(sptr)
				{
				}
#if 0
				template<class C>
				moSmartPtr(const C *ptr)
					: moSmartPtrBase<T, Ptr::moAnyPtr>(ptr)
				{
				}
				template<class C>
				moSmartPtr(const moSmartPtr<C>& sptr)
					: moSmartPtrBase<T, Ptr::moAnyPtr>(sptr)
				{
				}

	moSmartPtr<T>&		operator = (int zero)
				{
					moSmartPtrBase<T, Ptr::moAnyPtr>::operator = (zero);
					return *this;
				}
#endif
				~moSmartPtr()
				{
				}

	moSmartPtr<T>&		operator = (const T *ptr)
				{
					moSmartPtrBase<T, Ptr::moAnyPtr>::operator = (ptr);
					return *this;
				}
	moSmartPtr<T>&		operator = (const moSmartPtr<T>& sptr)
				{
					moSmartPtrBase<T, Ptr::moAnyPtr>::operator = (sptr);
					return *this;
				}

#if 0
	template<class C>
	moSmartPtr<T>&		operator = (const C *ptr)
				{
					moSmartPtrBase<T, Ptr::moAnyPtr>::operator = (ptr);
					return *this;
				}
	template<class C>
	moSmartPtr<T>&		operator = (const moSmartPtr<C>& sptr)
				{
					moSmartPtrBase<T, Ptr::moAnyPtr>::operator = (sptr);
					return *this;
				}
#endif
};



template<class T>
class MO_DLL_EXPORT_TMPL moSmartDynamicPtr : public moSmartPtrBase<T, Ptr::moDynamicPtr>
{
public:
				moSmartDynamicPtr(void)
					: moSmartPtrBase<T, Ptr::moDynamicPtr>()
				{
				}
				moSmartDynamicPtr(const T *ptr)
					: moSmartPtrBase<T, Ptr::moDynamicPtr>(ptr)
				{
				}
				moSmartDynamicPtr(const moSmartPtr<T>& sptr)
					: moSmartPtrBase<T, Ptr::moDynamicPtr>(sptr)
				{
				}

	moSmartDynamicPtr<T>&	operator = (const T *ptr)
				{
					moSmartPtrBase<T, Ptr::moDynamicPtr>::operator = (ptr);
					return *this;
				}
	moSmartDynamicPtr<T>&	operator = (const moSmartPtrBase<T, Ptr::moDynamicPtr>& sptr)
				{
					moSmartPtrBase<T, Ptr::moDynamicPtr>::operator = (sptr);
					return *this;
				}
};



template<class T>
class MO_DLL_EXPORT_TMPL moSmartStaticPtr : public moSmartPtrBase<T, Ptr::moStaticPtr>
{
public:
				moSmartStaticPtr(void)
					: moSmartPtrBase<T, Ptr::moStaticPtr>()
				{
				}
				moSmartStaticPtr(const T *ptr)
					: moSmartPtrBase<T, Ptr::moStaticPtr>(ptr)
				{
				}
				moSmartStaticPtr(const moSmartPtr<T>& sptr)
					: moSmartPtrBase<T, Ptr::moStaticPtr>(sptr)
				{
				}

	moSmartStaticPtr<T>&	operator = (const T *ptr)
				{
					moSmartPtrBase<T, Ptr::moStaticPtr>::operator = (ptr);
					return *this;
				}
	moSmartStaticPtr<T>&	operator = (const moSmartPtrBase<T, Ptr::moAnyPtr>& sptr)
				{
					moSmartPtrBase<T, Ptr::moStaticPtr>::operator = (sptr);
					return *this;
				}
};





template<class T, class PTR>
class MO_DLL_EXPORT_TMPL moNoNullSmartPtrBase : private PTR
{
public:
				moNoNullSmartPtrBase(const T *ptr)
					: PTR()
				{
					PTR::CheckPtr(ptr);
					f_ptr = const_cast<T *>(ptr);
					assert((bool) f_ptr);
					f_ptr->AddRef();
#ifdef MO_DEBUG
					f_initialized = true;
#endif
				}
				moNoNullSmartPtrBase(const moNoNullSmartPtrBase<T, PTR>& sptr)
					: PTR()
				{
					PTR::CheckPtr(sptr.f_ptr);
					f_ptr = const_cast<T *>(sptr.f_ptr);
					assert((bool) f_ptr);	// this one should never happen
					f_ptr->AddRef();
#ifdef MO_DEBUG
					f_initialized = true;
#endif
				}
				moNoNullSmartPtrBase(const moSmartPtrBase<T, PTR>& sptr)
					: PTR()
				{
					PTR::CheckPtr(sptr.f_ptr);
					f_ptr = const_cast<T *>(sptr.f_ptr);
					assert((bool) f_ptr);
					f_ptr->AddRef();
#ifdef MO_DEBUG
					f_initialized = true;
#endif
				}
				~moNoNullSmartPtrBase()
				{
					f_ptr->Release();
#ifdef MO_DEBUG
					f_initialized = false;
#endif
				}



#ifdef MO_DEBUG
				void Validate(void) const
				{
					assert(f_initialized);
				}
#else
				void Validate(void) const
				{
				}
#endif

#if 0
	// special case to be able to set a smart pointer to NULL
	moSmartPtrBase<T, PTR>&	operator = (int zero)
				{
					assert(zero == 0);
					if(f_ptr) {
						f_ptr->Release();
					}
					f_ptr = 0;
					return *this;
				}
#endif

	moNoNullSmartPtrBase<T, PTR>&	operator = (const T *ptr)
				{
					Validate();
					PTR::CheckPtr(ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(ptr);
					assert((bool) f_ptr);
					f_ptr->AddRef();
					old_ptr->Release();
					return *this;
				}
	moNoNullSmartPtrBase<T, PTR>&	operator = (const moNoNullSmartPtrBase<T, PTR>& sptr)
				{
					Validate();
					PTR::CheckPtr(sptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(static_cast<const T *>(sptr));
					assert((bool) f_ptr);
					f_ptr->AddRef();
					old_ptr->Release();
					return *this;
				}
	moNoNullSmartPtrBase<T, PTR>&	operator = (const moSmartPtrBase<T, PTR>& sptr)
				{
					Validate();
					PTR::CheckPtr(sptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(static_cast<const T *>(sptr));
					assert((bool) f_ptr);
					f_ptr->AddRef();
					old_ptr->Release();
					return *this;
				}

	T *			operator -> (void) { Validate(); return f_ptr; }
	const T *		operator -> (void) const { Validate(); return f_ptr; }
				operator T * (void) { Validate(); return f_ptr; }
				operator T * (void) const { Validate(); return f_ptr; }
				operator T& (void) { Validate(); return *f_ptr; }
				operator T& (void) const { Validate(); return *f_ptr; }

				operator bool (void) { Validate(); return true; }
				operator bool (void) const { Validate(); return true; }
	bool			operator ! (void) { Validate(); return false; }
	bool			operator ! (void) const { Validate(); return false; }

	template<class C>
	bool			operator == (C *ptr) const { Validate(); return f_ptr == ptr; }
	template<class C>
	bool			operator == (const C *ptr) const { Validate(); return f_ptr == ptr; }
	template<class C>
	bool			operator != (C *ptr) const { Validate(); return f_ptr != ptr; }
	template<class C>
	bool			operator != (const C *ptr) const { Validate(); return f_ptr != ptr; }
	template<class C>
	bool			operator <= (const C *ptr) const { Validate(); return f_ptr <= ptr; }
	template<class C>
	bool			operator <  (const C *ptr) const { Validate(); return f_ptr <  ptr; }
	template<class C>
	bool			operator >= (const C *ptr) const { Validate(); return f_ptr >= ptr; }
	template<class C>
	bool			operator >  (const C *ptr) const { Validate(); return f_ptr >  ptr; }

	template<class C>
	bool			operator == (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr == static_cast<const C *>(sptr); }
	template<class C>
	bool			operator != (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr != static_cast<const C *>(sptr); }
	template<class C>
	bool			operator <= (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr <= static_cast<const C *>(sptr); }
	template<class C>
	bool			operator <  (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr <  static_cast<const C *>(sptr); }
	template<class C>
	bool			operator >= (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr >= static_cast<const C *>(sptr); }
	template<class C>
	bool			operator >  (const moSmartPtrBase<C, PTR>& sptr) const { Validate(); return f_ptr >  static_cast<const C *>(sptr); }

	template<class C>
	friend bool		operator == (const C *ptr, const moNoNullSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr == sptr.f_ptr; }
	template<class C>
	friend bool		operator != (const C *ptr, const moNoNullSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr != sptr.f_ptr; }
	template<class C>
	friend bool		operator <= (const C *ptr, const moNoNullSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr <= sptr.f_ptr; }
	template<class C>
	friend bool		operator <  (const C *ptr, const moNoNullSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr <  sptr.f_ptr; }
	template<class C>
	friend bool		operator >= (const C *ptr, const moNoNullSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr >= sptr.f_ptr; }
	template<class C>
	friend bool		operator >  (const C *ptr, const moNoNullSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr >  sptr.f_ptr; }

	template<class C>
	friend bool		operator == (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr == sptr.f_ptr; }
	template<class C>
	friend bool		operator != (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr != sptr.f_ptr; }
	template<class C>
	friend bool		operator <= (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr <= sptr.f_ptr; }
	template<class C>
	friend bool		operator <  (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr <  sptr.f_ptr; }
	template<class C>
	friend bool		operator >= (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr >= sptr.f_ptr; }
	template<class C>
	friend bool		operator >  (const C *ptr, const moSmartPtrBase<T, PTR>& sptr) { Validate(); return ptr >  sptr.f_ptr; }

	// Whenever you have a problem with an assignment operator
	// then use the copy; especially useful if you need a cast!
	template<class C>
	void			Copy(const C *ptr)
				{
					Validate();
					PTR::CheckPtr(ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
					assert((bool) f_ptr);
					f_ptr->AddRef();
					old_ptr->Release();
				}

	template<class C>
	void			Copy(const moNoNullSmartPtrBase<C, PTR>& sptr)
				{
					Validate();
					PTR::CheckPtr(sptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(static_cast<const C *>(sptr)));
					assert((bool) f_ptr);	// just cannot happen
					f_ptr->AddRef();
					old_ptr->Release();
				}

	template<class C>
	void			Copy(const moSmartPtrBase<C, PTR>& sptr)
				{
					Validate();
					PTR::CheckPtr(sptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(static_cast<const C *>(sptr)));
					assert((bool) f_ptr);
					f_ptr->AddRef();
					old_ptr->Release();
				}

	// The Set() function does NOT increment the reference counter.
	// I don't think this is even necessary! (it was until I wrote
	// the correct reference count whether an object is created
	// with the new operator or not)
	template<class C>
	void			Set(const C *ptr)
				{
					Validate();
					PTR::CheckPtr(ptr);
					f_ptr->Release();
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
					assert((bool) f_ptr);
				}

	// create a new instance (releasing the previous one first)
	// works only if you have a constructor without any parameter
	void			New(void)
				{
					Validate();
					f_ptr->Release();
					f_ptr = new T;
					PTR::CheckPtr(f_ptr);
					assert((bool) f_ptr);
					f_ptr->AddRef();
				}

	bool			IsNull(void) const
				{
					Validate();
					return false;
				}

private:
	T *			f_ptr;
#ifdef MO_DEBUG
	zbool_t			f_initialized;
#endif
};

template<class T>
class MO_DLL_EXPORT_TMPL moNoNullSmartPtr : public moNoNullSmartPtrBase<T, Ptr::moAnyPtr>
{
public:
				moNoNullSmartPtr(void)
					: moNoNullSmartPtrBase<T, Ptr::moAnyPtr>()
				{
				}
				moNoNullSmartPtr(const T *ptr)
					: moNoNullSmartPtrBase<T, Ptr::moAnyPtr>(ptr)
				{
				}
				moNoNullSmartPtr(const moNoNullSmartPtr<T>& sptr)
					: moNoNullSmartPtrBase<T, Ptr::moAnyPtr>(sptr)
				{
				}
				moNoNullSmartPtr(const moSmartPtr<T>& sptr)
					: moNoNullSmartPtrBase<T, Ptr::moAnyPtr>(sptr)
				{
				}
				~moNoNullSmartPtr()
				{
				}

	moNoNullSmartPtr<T>&	operator = (const T *ptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moAnyPtr>::operator = (ptr);
					return *this;
				}
	moNoNullSmartPtr<T>&	operator = (const moNoNullSmartPtr<T>& sptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moAnyPtr>::operator = (sptr);
					return *this;
				}
	moNoNullSmartPtr<T>&	operator = (const moSmartPtr<T>& sptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moAnyPtr>::operator = (sptr);
					return *this;
				}
};



template<class T>
class MO_DLL_EXPORT_TMPL moNoNullSmartDynamicPtr : public moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>
{
public:
				moNoNullSmartDynamicPtr(void)
					: moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>()
				{
				}
				moNoNullSmartDynamicPtr(const T *ptr)
					: moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>(ptr)
				{
				}
				moNoNullSmartDynamicPtr(const moNoNullSmartPtr<T>& sptr)
					: moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>(sptr)
				{
				}
				moNoNullSmartDynamicPtr(const moSmartPtr<T>& sptr)
					: moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>(sptr)
				{
				}

	moNoNullSmartDynamicPtr<T>&	operator = (const T *ptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>::operator = (ptr);
					return *this;
				}
	moNoNullSmartDynamicPtr<T>&	operator = (const moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>& sptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>::operator = (sptr);
					return *this;
				}
	moNoNullSmartDynamicPtr<T>&	operator = (const moSmartPtrBase<T, Ptr::moDynamicPtr>& sptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moDynamicPtr>::operator = (sptr);
					return *this;
				}
};



template<class T>
class MO_DLL_EXPORT_TMPL moNoNullSmartStaticPtr : public moNoNullSmartPtrBase<T, Ptr::moStaticPtr>
{
public:
				moNoNullSmartStaticPtr(void)
					: moNoNullSmartPtrBase<T, Ptr::moStaticPtr>()
				{
				}
				moNoNullSmartStaticPtr(const T *ptr)
					: moNoNullSmartPtrBase<T, Ptr::moStaticPtr>(ptr)
				{
				}
				moNoNullSmartStaticPtr(const moNoNullSmartPtr<T>& sptr)
					: moNoNullSmartPtrBase<T, Ptr::moStaticPtr>(sptr)
				{
				}
				moNoNullSmartStaticPtr(const moSmartPtr<T>& sptr)
					: moNoNullSmartPtrBase<T, Ptr::moStaticPtr>(sptr)
				{
				}

	moNoNullSmartStaticPtr<T>&	operator = (const T *ptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moStaticPtr>::operator = (ptr);
					return *this;
				}
	moNoNullSmartStaticPtr<T>&	operator = (const moNoNullSmartPtrBase<T, Ptr::moAnyPtr>& sptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moStaticPtr>::operator = (sptr);
					return *this;
				}
	moNoNullSmartStaticPtr<T>&	operator = (const moSmartPtrBase<T, Ptr::moAnyPtr>& sptr)
				{
					moNoNullSmartPtrBase<T, Ptr::moStaticPtr>::operator = (sptr);
					return *this;
				}
};





template<class T, class S, class PTR>
class MO_DLL_EXPORT_TMPL moDualSmartPtrBase : private PTR
{
public:
				moDualSmartPtrBase(void)
					: PTR()
				{
					f_ptr = 0;
				}
				moDualSmartPtrBase(const T *ptr)
					: PTR()
				{
					PTR::CheckPtr(static_cast<const S *>(ptr));
					f_ptr = const_cast<T *>(ptr);
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
				}
				moDualSmartPtrBase(const moDualSmartPtrBase<T, S, PTR>& sptr)
					: PTR()
				{
					PTR::CheckPtr(static_cast<const S *>(sptr.f_ptr));
					f_ptr = const_cast<T *>(sptr.f_ptr);
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
				}
				~moDualSmartPtrBase()
				{
					if(f_ptr) {
						static_cast<S *>(f_ptr)->Release();
					}
				}

	moDualSmartPtrBase<T, S, PTR>&	operator = (const T *ptr)
				{
					PTR::CheckPtr(static_cast<const S *>(ptr));
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(ptr);
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
					if(old_ptr) {
						static_cast<S *>(old_ptr)->Release();
					}
					return *this;
				}
	moDualSmartPtrBase<T, S, PTR>&	operator = (const moDualSmartPtrBase<T, S, PTR>& sptr)
				{
					PTR::CheckPtr(static_cast<const S *>(sptr.f_ptr));
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(static_cast<const T *>(sptr));
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
					if(old_ptr) {
						static_cast<S *>(old_ptr)->Release();
					}
					return *this;
				}

#ifdef MO_DEBUG
	T *			operator -> (void) { return f_ptr; }
	const T *		operator -> (void) const { return f_ptr; }
#else
	T *			operator -> (void) { return f_ptr; }
	const T *		operator -> (void) const { return f_ptr; }
#endif
				operator T * (void) { return f_ptr; }
				operator T * (void) const { return f_ptr; }
				operator T& (void) { return *f_ptr; }
				operator T& (void) const { return *f_ptr; }

				operator bool (void) { return f_ptr != 0; }
				operator bool (void) const { return f_ptr != 0; }
	bool			operator ! (void) { return f_ptr == 0; }
	bool			operator ! (void) const { return f_ptr == 0; }

	template<class C>
	bool			operator == (C *ptr) const { return f_ptr == ptr; }
	template<class C>
	bool			operator == (const C *ptr) const { return f_ptr == ptr; }
	template<class C>
	bool			operator != (C *ptr) const { return f_ptr != ptr; }
	template<class C>
	bool			operator != (const C *ptr) const { return f_ptr != ptr; }
	template<class C>
	bool			operator <= (const C *ptr) const { return f_ptr <= ptr; }
	template<class C>
	bool			operator <  (const C *ptr) const { return f_ptr <  ptr; }
	template<class C>
	bool			operator >= (const C *ptr) const { return f_ptr >= ptr; }
	template<class C>
	bool			operator >  (const C *ptr) const { return f_ptr >  ptr; }

	template<class C>
	bool			operator == (const moDualSmartPtrBase<C, S, PTR>& sptr) const { return f_ptr == static_cast<const C *>(sptr); }
	template<class C>
	bool			operator != (const moDualSmartPtrBase<C, S, PTR>& sptr) const { return f_ptr != static_cast<const C *>(sptr); }
	template<class C>
	bool			operator <= (const moDualSmartPtrBase<C, S, PTR>& sptr) const { return f_ptr <= static_cast<const C *>(sptr); }
	template<class C>
	bool			operator <  (const moDualSmartPtrBase<C, S, PTR>& sptr) const { return f_ptr <  static_cast<const C *>(sptr); }
	template<class C>
	bool			operator >= (const moDualSmartPtrBase<C, S, PTR>& sptr) const { return f_ptr >= static_cast<const C *>(sptr); }
	template<class C>
	bool			operator >  (const moDualSmartPtrBase<C, S, PTR>& sptr) const { return f_ptr >  static_cast<const C *>(sptr); }

	template<class C>
	friend bool		operator == (const C *ptr, const moDualSmartPtrBase<T, S, PTR>& sptr) { return ptr == sptr.f_ptr; }
	template<class C>
	friend bool		operator != (const C *ptr, const moDualSmartPtrBase<T, S, PTR>& sptr) { return ptr != sptr.f_ptr; }
	template<class C>
	friend bool		operator <= (const C *ptr, const moDualSmartPtrBase<T, S, PTR>& sptr) { return ptr <= sptr.f_ptr; }
	template<class C>
	friend bool		operator <  (const C *ptr, const moDualSmartPtrBase<T, S, PTR>& sptr) { return ptr <  sptr.f_ptr; }
	template<class C>
	friend bool		operator >= (const C *ptr, const moDualSmartPtrBase<T, S, PTR>& sptr) { return ptr >= sptr.f_ptr; }
	template<class C>
	friend bool		operator >  (const C *ptr, const moDualSmartPtrBase<T, S, PTR>& sptr) { return ptr >  sptr.f_ptr; }

	T *&			ArgAsRefType(void)
				{
					if(f_ptr) {
						static_cast<S *>(f_ptr)->Release();
					}
					f_ptr = 0;
					return f_ptr;
				}

	T **			ArgAsType(void)
				{
					if(f_ptr) {
						static_cast<S *>(f_ptr)->Release();
					}
					f_ptr = 0;
					return &f_ptr;
				}

	void **			ArgAsVoid(void)
				{
					if(f_ptr) {
						static_cast<S *>(f_ptr)->Release();
					}
					f_ptr = 0;
					return static_cast<void **>(&f_ptr);
				}

	// Whenever you have a problem with an assignment operator
	// then use the copy; especially useful if you need a cast!
	template<class C>
	void			Copy(const C *ptr)
				{
					PTR::CheckPtr(ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
					if(old_ptr) {
						static_cast<S *>(old_ptr)->Release();
					}
				}

	template<class C>
	void			Copy(const moSmartPtrBase<C, PTR>& sptr)
				{
					PTR::CheckPtr(sptr.f_ptr);
					T *old_ptr = f_ptr;
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(static_cast<const C *>(sptr)));
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
					if(old_ptr) {
						static_cast<S *>(old_ptr)->Release();
					}
				}

	// The Set() function does NOT increment the reference counter.
	// I don't think this is even necessary! (it was until I wrote
	// the correct reference count whether an object is created
	// with the new operator or not)
	template<class C>
	void			Set(const C *ptr)
				{
					PTR::CheckPtr(ptr);
					if(f_ptr) {
						static_cast<S *>(f_ptr)->Release();
					}
					f_ptr = const_cast<T *>(dynamic_cast<const T *>(ptr));
				}

	// create a new instance (releasing the previous one first)
	// works only if you have a constructor without any parameter
	void			New(void)
				{
					if(f_ptr) {
						static_cast<S *>(f_ptr)->Release();
					}
					f_ptr = new T;
					PTR::CheckPtr(static_cast<const S *>(f_ptr));
					if(f_ptr) {
						static_cast<S *>(f_ptr)->AddRef();
					}
				}

private:
	T *			f_ptr;
};

template<class T, class S>
class MO_DLL_EXPORT_TMPL moDualSmartPtr : public moDualSmartPtrBase<T, S, Ptr::moAnyPtr>
{
public:
				moDualSmartPtr(void)
					: moDualSmartPtrBase<T, S, Ptr::moAnyPtr>()
				{
				}
				moDualSmartPtr(const T *ptr)
					: moDualSmartPtrBase<T, S, Ptr::moAnyPtr>(ptr)
				{
				}
				moDualSmartPtr(const moDualSmartPtr<T, S>& sptr)
					: moDualSmartPtrBase<T, S, Ptr::moAnyPtr>(sptr)
				{
				}

	moDualSmartPtr<T, S>&	operator = (const T *ptr)
				{
					moDualSmartPtrBase<T, S, Ptr::moAnyPtr>::operator = (ptr);
					return *this;
				}
	moDualSmartPtr<T, S>&	operator = (const moDualSmartPtr<T, S>& sptr)
				{
					moDualSmartPtrBase<T, S, Ptr::moAnyPtr>::operator = (sptr);
					return *this;
				}
};



template<class T, class S>
class MO_DLL_EXPORT_TMPL moDualSmartDynamicPtr : public moDualSmartPtrBase<T, S, Ptr::moDynamicPtr>
{
public:
				moDualSmartDynamicPtr(void)
					: moDualSmartPtrBase<T, S, Ptr::moDynamicPtr>()
				{
				}
				moDualSmartDynamicPtr(const T *ptr)
					: moDualSmartPtrBase<T, S, Ptr::moDynamicPtr>(ptr)
				{
				}
				moDualSmartDynamicPtr(const moDualSmartPtr<T, S>& sptr)
					: moDualSmartPtrBase<T, S, Ptr::moDynamicPtr>(sptr)
				{
				}

	moDualSmartDynamicPtr<T, S>&	operator = (const T *ptr)
				{
					moDualSmartPtrBase<T, S, Ptr::moDynamicPtr>::operator = (ptr);
					return *this;
				}
	moDualSmartDynamicPtr<T, S>&	operator = (const moDualSmartDynamicPtr<T, S>& sptr)
				{
					moDualSmartPtrBase<T, S, Ptr::moDynamicPtr>::operator = (sptr);
					return *this;
				}
};



template<class T, class S>
class MO_DLL_EXPORT_TMPL moDualSmartStaticPtr : public moDualSmartPtrBase<T, S, Ptr::moStaticPtr>
{
public:
				moDualSmartStaticPtr(void)
					: moDualSmartPtrBase<T, S, Ptr::moStaticPtr>()
				{
				}
				moDualSmartStaticPtr(const T *ptr)
					: moDualSmartPtrBase<T, S, Ptr::moStaticPtr>(ptr)
				{
				}
				moDualSmartStaticPtr(const moDualSmartPtr<T, S>& sptr)
					: moDualSmartPtrBase<T, S, Ptr::moStaticPtr>(sptr)
				{
				}

	moDualSmartStaticPtr<T, S>&	operator = (const T *ptr)
				{
					moDualSmartPtrBase<T, S, Ptr::moStaticPtr>::operator = (ptr);
					return *this;
				}
	moDualSmartStaticPtr<T, S>&	operator = (const moDualSmartPtrBase<T, S, Ptr::moStaticPtr>& sptr)
				{
					moDualSmartPtrBase<T, S, Ptr::moStaticPtr>::operator = (sptr);
					return *this;
				}
};







};		// namespace molib;

// vim: ts=8 sw=8 syntax=cpp.doxygen
#endif		// #ifndef MO_SMARTPTR_H

