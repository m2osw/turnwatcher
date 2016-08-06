//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
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



#ifndef MO_BASE_H
#define	MO_BASE_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifdef _MSC_VER
// make sure this comes first, just in case
#include	<windows.h>
#pragma		warning(disable: 4251)
#endif

// read the configuration file first
#ifdef HAVE_CONFIG_H
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
#include	"config.h"
#endif
#endif
#include	"mo_config.h"
#include	"mo_export.h"

// common includes first
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<stdint.h>
#include	<stdarg.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<dirent.h>
#include	<math.h>
#include	<ctype.h>
#include	<assert.h>
#ifdef MO_THREAD
#include	<semaphore.h>
#endif
#ifndef _MSC_VER
#	include	<ext/stdio_filebuf.h>
#endif
#include	<iconv.h>
#include	<zlib.h>
#include	<regex.h>
#include	<tar.h>			// on MinGW you need to take the tar.h from the tar source tar ball and copy it to the include directory
#include	<sys/stat.h>
#ifndef _MSC_VER
#include	<sys/param.h>
#else
// this we are missing if we don't include param.h
#define BIG_ENDIAN	4321
#define LITTLE_ENDIAN	1234
#define BYTE_ORDER	LITTLE_ENDIAN
#endif
#ifdef MO_THREAD
#	include	<pthread.h>
#endif

#include	<memory>
#include	<algorithm>
#include	<string>
#include	<iostream>
#include	<sstream>

#ifndef WIN32
#include	<syslog.h>
#include	<pwd.h>
#include	<grp.h>
#include	<netdb.h>
#include	<arpa/inet.h>
#include	<netinet/in_systm.h>
#include	<netinet/in.h>
#include	<netinet/ip.h>
#include	<sys/socket.h>
#include	<sys/wait.h>
#endif

#ifndef MO_CONFIG_NO_UNISTD
#include	<unistd.h>
#endif

#ifdef MO_CONFIG_INCLUDE_PROCESS
#include	<process.h>
#endif

#ifdef MO_CONFIG_INCLUDE_STRSTREA
#include	<strstrea.h>
#endif

#ifdef MO_CONFIG_INCLUDE_SYSTIME
#include	<sys/time.h>
#endif

#ifdef MO_WIN32
#	include <mo/mo_stdint.h>
#endif


#ifdef MO_CONFIG_NO_IN_PORT_T
typedef unsigned short int	in_port_t;
#endif

#ifdef MO_CONFIG_NO_BOOL
// define the bool type and false/true values
typedef	int			bool;
enum { false, true };
#endif

#ifdef MO_CONFIG_USE_STRI_FOR_STRCASE
#define		strcasecmp		stricmp
#define		strncasecmp		strnicmp
#endif

#ifdef MO_CONFIG_USE__PRINTF
#ifdef __NO_ISOCEXT
#define		snprintf		_snprintf
#define		vsnprintf		_vsnprintf
#endif
#endif

#ifdef MO_CONFIG_NO_DIRECT_STATIC_CONST
#define	STATIC_CONST(type, name, value)		static const type name
#else
#define	STATIC_CONST(type, name, value)		static const type name = (type) (value)
#endif


#if !defined(PATH_MAX) && defined(_MAX_PATH)
#define	PATH_MAX	_MAX_PATH
#endif


#ifdef _MSC_VER
#define	F_OK	00
#define	W_OK	02
#define	R_OK	04
// W_OK | R_OK to text read & write
#define	X_OK	0	// not available under Win32--check the extension instead
#ifndef snprintf
#	define	snprintf	_snprintf
#endif
#define	rint(x)		floor(x + 0.5)
typedef int		pid_t;
#define MO_CONFIG_CTYPE_DEFINE	1
#define	va_copy(d, s)		(d = s)
// stdint.h defines WCHAR_MAX as ((wchar_t)-1) which we cannot reuse in #if expressions!
#undef WCHAR_MAX
#define WCHAR_MAX	65535
#endif

#ifndef BYTE_ORDER
#error "The macro BYTE_ORDER is not defined."
#endif
#ifndef BIG_ENDIAN
#error "The macro BIG_ENDIAN is not defined."
#endif
#ifndef LITTLE_ENDIAN
#error "The macro LITTLE_ENDIAN is not defined."
#endif



#include	"mo_smartptr.h"
#include	"details/mo_atomic.h"


namespace molib
{


// Please, use these to allocate memory so we can track it
// at all time; this is useful to make sure that we don't
// have leaks and also to make sure we don't free something
// twice
MO_DLL_EXPORT_FUNC void *mo_calloc(size_t count, size_t size, const char *info);
MO_DLL_EXPORT_FUNC void *mo_malloc(size_t size, const char *info);
MO_DLL_EXPORT_FUNC void mo_free(void *ptr);
MO_DLL_EXPORT_FUNC void *mo_realloc(void *ptr, size_t size, const char *info);
MO_DLL_EXPORT_FUNC void mo_show_allocated_buffers(void);










#if 1
/** \brief Manages the lifetime of a temporary buffer.
 *
 *	Whenever you need a temporary buffer that needs to be deleted
 *	whenever your function returns or block of statements ends,
 *	you should use one of these templates (moDeletePtr or
 *	moDeleteArrayPtr), to make sure that this temporary buffer
 *	is deleted.
 *	
 *	The constructors register the specified pointer.
 *	
 *	The destructors ensure the pointers are deleted.
 *	
 *	The Delete() function can be used to prematurely delete the
 *	pointer.
 *	
 *	The Ptr() function is a convenience function which returns
 *	the pointer. In general, you won't need it since you will
 *	already have access to your pointer.
 *	
 *	The moDeletePtr will be used for objects you allocate as
 *	single objects (no []). The moDeleteArrayPtr will be used
 *	for arrays (new T[\<count>]). The only difference is the
 *	delete operator which is either used with or without the
 *	square brackets.
 *
 * \section Synopsis
 *
 *	moDeletePtr:
 *	moDeletePtr(T *ptr);
 *	~moDeletePtr();
 *	void Delete(void);
 *	T *Ptr(void) const;
 *	
 *	moDeleteArrayPtr:
 *	moDeleteArrayPtr(T *ptr);
 *	~moDeleteArrayPtr();
 *	void Delete(void);
 *	T *Ptr(void) const;
 *
 * \section Examples
 *
 * \code
 *	Foo(void)
 *	{
 *		MyObject *object = new MyObject;
 *		moDeletePtr<MyObject> dptr_object(object);
 *		...
 *		// not necessary moDeletePtr() does it for us
 *		//delete object;
 *	}
 *	
 *	Foo(int size)
 *	{
 *		int *data = new int[size];
 *		moDeleteArrayPtr<int> dptr_data(data);
 *		...
 *		// not necessary moDeleteArrayPtr() does it for us
 *		//delete [] data;
 *	}
 *
 * \endcode
 *
 * \sa molib::moSmartPtr
 */
template<class T>
class MO_DLL_EXPORT_TMPL moDeletePtr
{
public:
			moDeletePtr(T *ptr)
			{
				f_ptr = ptr;
			}
			~moDeletePtr()
			{
				delete f_ptr;
			}

	void		Delete(void)
			{
				delete f_ptr;
				f_ptr = 0;
			}

	T *		Ptr(void) const
			{
				return const_cast<T *>(f_ptr);
			}

private:
	T *		f_ptr;
};


/** \brief Delete an array pointer
 *
 * This class is an equivalent to the std::auto_ptr\<> STL class
 * except that it will properly use delete [] whenever deleting
 * the specified pointer.
 *
 * You can also use the moDeletePtr\<> instead of the std::auto_ptr\<>
 * STL class.
 *
 * \sa template<class T> class MO_DLL_EXPORT_TMPL moDeletePtr
 * \sa molib::moSmartPtr
 */
template<class T>
class MO_DLL_EXPORT_TMPL moDeleteArrayPtr
{
public:
			moDeleteArrayPtr(T *ptr)
			{
				f_ptr = ptr;
			}
			~moDeleteArrayPtr()
			{
				delete [] f_ptr;
			}

	void		Delete(void)
			{
				delete [] f_ptr;
				f_ptr = 0;
			}

	T *		Ptr(void) const
			{
				return const_cast<T *>(f_ptr);
			}

private:
	T *		f_ptr;
};
#endif





/************************************************************/


/*
 * The following is a list of error numbers for low level throws.
 * 
 * example:
 *		throw long(MO_ERROR_BAD_COMPARE);
 */
/* lower numbers are reserved for internal use */
#define	MO_ERROR_UNDEFINED	0	/* an undefined error number */
#define	MO_ERROR_MEMORY		1	/* out of memory */
#define	MO_ERROR_COMPARE	2	/* a comparison operator got an error */
#define	MO_ERROR_UNORDERED	3	/* a comparison operator got an unordered result */
#define	MO_ERROR_BAD_COMPARE	4	/* a comparison operator got an invalid result value */
#define	MO_ERROR_UNKNOWN	5	/* something is unknown */
#define	MO_ERROR_OVERFLOW	6	/* too many items, too large a value */
#define	MO_ERROR_UNDERFLOW	7	/* too few items, too little a value */
#define	MO_ERROR_INVALID	8	/* invalid data, call, etc. */
#define	MO_ERROR_IO		9	/* some I/O failed */
#define	MO_ERROR_NO_FUNCTION	10	/* a call to a function can't be accomplished because a pointer is missing */
#define	MO_ERROR_ZERO_DIVIDE	11	/* a math error involving zero */
#define	MO_ERROR_NOT_IMPLEMENTED 12	/* a function or part of a function is not yet implemented */
#define	MO_ERROR_INTERNAL	13	/* some internal assertion failed */
#define	MO_ERROR_DONE		14	/* that object was marked 'done' meaning you can't allocate it again */
#define	MO_ERROR_EMPTY		15	/* a list or reference was expected but is empty */
#define	MO_ERROR_END_NOT_EXPECTED 16	/* the end of a file or string was not expected yet */
#define	MO_ERROR_IN_USE		17	/* the object or resource is already used in some other way */
#define	MO_ERROR_BAD_PARAMETER	18	/* a function parameter cannot be used as defined */

/* from this number upward it's reserved to the end user */
#define	MO_ERROR_USER_NUMBER	10000


/*! \class moBase
 *
 * \brief Reference counted base-object for the entire molib library.
 *
 * \section mobase Understanding molib::moBase
 *
 * Every object in molib is derived from molib::moBase (with a few exceptions).
 * This is nothing new--look at many C++ class libraries that exist (both
 * commercial and open source) and you find this technique used. But what does
 * it buy you? For one thing, every class is always a molib::moBase type, so this
 * means you can use a moList to host a myriad of different objects, all with a
 * common ancestor.
 *
 * \subsection mobase_allocation Allocating on stack versus heap
 *
 * But molib takes this a step further. molib::moBase is not just a base class
 * with a few common methods on it--molib::moBase is also reference counted. And this
 * reference counting is intelligent. If your object is allocated on the stack,
 * then molib::moBase considers itself a non-dynamically allocated object and
 * reference counting does not apply (in fact, the debug build of this library
 * will cause a assert in the destructor if the reference count has anything
 * but zero in it).
 *
 * \subsection mobase_reference_counting Reference counted objects with molib
 *
 * However, if you create your molib::moBase-derived object on the heap, it
 * will turn on the reference counting feature, allowing you to call
 * molib::moBase::AddRef() and
 * molib::moBase::Release(), which increment and decrement the reference count, respectively.
 * When the reference count reaches zero, molib::moBase::Release() will cause the
 * object to be deleted. If you combine this with our moSmartPtr object,
 * you will not have to worry about memory managment in most cases.
 *
 * \subsection mobase_memory_leaks Memory leak detection with molib
 *
 * The biggest advantage? You may use molib::moBase::ShowAllocatedObjects() to dump out
 * the object still remaining in memory, allowing you to check for memory
 * leaks without any special tools.
 *
 * \note Win32 COM developers may take advantage of this by deriving each of
 * their C++ COM classes from molib::moBase.
 *
 * \section desc In depth description of moBase object
 *
 * The moBase class object is always constructed without parameters.
 * It gives the new objects a unique serial number and increase the
 * internal serial number counter.
 * 
 * The destructor doesn't do anything at this time. However, it was
 * made virtual so virtual objects can still use this base object.
 * 
 * The error numbers are also defined within this object. This way
 * all the errors are common to all objects. The moError(3) object
 * can use these error numbers. The following is the current list:
 * 
 * 	MO_ERROR_UNDEFINED	used as a default number (it is
 * 				zero) when no specific error
 * 				number was defined for an error
 * 
 * 	MO_ERROR_MEMORY		any time a memory error occurs,
 * 				use this error number with a:
 * 					throw std::bad_alloc;
 * 				You should never use this error with
 * 				an moError() object unless you were
 * 				trying to allocate a really large
 * 				buffer (several megabytes)
 * 
 * 	MO_ERROR_COMPARE	the Compare() function failed for
 * 				some reasons and generated an error
 * 
 * 	MO_ERROR_UNORDERED	the Compare() function has been applied
 * 				on objects which can't be ordered
 * 				properly
 * 
 * 	MO_ERROR_BAD_COMPARE	the Compare() function returned an
 * 				invalid value
 * 
 * 	MO_ERROR_UNKNOWN	something made the software generate
 * 				an error; we don't know what though
 * 
 * 	MO_ERROR_OVERFLOW	something's full or too large
 * 
 * 	MO_ERROR_UNDERFLOW	something's empty or too small
 * 
 * The user can also declare his own errors with the use of the
 * MO_ERROR_USER_NUMBER macro. The following is an example:
 *	
 * \code
 *	  #define GD_ERROR_MISSING_CHAPTER	(MO_ERROR_USER_NUMBER + 0)
 *	  #define GD_ERROR_INVALID_CHAPTER	(MO_ERROR_USER_NUMBER + 1)
 * \endcode
 *
 * \note
 *
 * The serial number of an moBase object is assigned at the
 * time it is constructed and is unique until over 4 billion
 * objects are created in the same program instance.
 * 
 * Whenever you compile with the MO_THREAD defined, the serial
 * number is managed in a multi-thread safe way.
 *
 * \bug
 *	You have to watch out! The reference count of an object is
 *	initialized to 1 whenever it isn't dynamic and 0 when it is
 *	dynamic. The destructor which always check to know whether
 *	the exact reference count is present in your objects.
 *	
 *	The fact is that a new operator is not supposed to create
 *	a referenced object however, a stack object is referenced
 *	by the stack and a variable member is referenced by its
 *	parent (the object which contains that variable member).
 *	
 *	The result is that you can write the following code and
 *	it all works fine as long as the AddRef() and Release()
 *	do match:
 *	
 * \code
 *	Foo *foo;
 *	
 *	foo = new Foo;
 *	...
 *	delete foo;
 * \endcode
 *	
 *	Or with smart pointers (i.e. exception safe):
 *	
 * \code
 *	FooSPtr foo;
 *	
 *	foo = new Foo;
 *	...
 * \endcode
 *	
 * Note that with the smart pointer you don't want to delete
 * the object, instead the smart pointer will release it whenever
 * the pointer (FooSPtr here) is being destroyed. This will
 * have the effect of destroying foo as well.
 * 
 * It is strongly adviced that you always use Smart Pointers
 * except in a very few exceptional cases. Smart pointers ensure
 * that you get allocated objects released whenever return
 * statements or throws are encountered.
 *
 * \sa molib::moSmarPtr, Serial, Compare, operator ==, operator !=, operator <, operator >, operator >=
 *
 */
class MO_DLL_EXPORT moBase
{
public:
	enum compare_t {
		MO_BASE_COMPARE_ERROR = -2,
		MO_BASE_COMPARE_SMALLER = -1,
		MO_BASE_COMPARE_EQUAL = 0,
		MO_BASE_COMPARE_GREATER = 1,
		MO_BASE_COMPARE_UNORDERED = 2
	};

	/* The following is a function type used to have more than one Compare().
	 * It's primary use is in the moList object which can then sort objects
	 * using different parameters.
	 */
	typedef compare_t (moBase::*compare_function_t)(const moBase& object) const;

				moBase(void);
				moBase(const moBase& object);
	virtual			~moBase();

	mo_atomic_word_t	Serial(void) const;
	virtual compare_t	Compare(const moBase& object) const;
	static compare_t	CompareInt(int a, int b);
	static compare_t	CompareUInt(unsigned int a, unsigned int b);
	compare_t		ComparePtr(const moBase& object) const;
	virtual const char *	moGetClassName(void) const;
	virtual size_t		SizeOf(void) const;
	unsigned long		AddRef(void) const;
	unsigned long		Release(void);
	unsigned long		ReferenceCount(void) const;
	bool			IsDynamicObject(void) const;

	static void *		operator new (size_t size);
	static void *		operator new [] (size_t size);
	static void		operator delete (void *object);
	static void		operator delete [] (void *object);
	static void		EmptyCache(void);		// mainly for dmalloc()
	static void		ShowAllocatedObjects(void);

#ifdef _MSC_VER
	// For some reason, the MS version of STL winds up forcing a second parameter. 
	//
	static void *		operator new    ( size_t size , void* foo );
	static void		operator delete ( void *object, void* foo );
#endif

	moBase&			operator = (const moBase& object);

	bool			operator == (const moBase& object) const;
	bool			operator != (const moBase& object) const;
	bool			operator <  (const moBase& object) const;
	bool			operator <= (const moBase& object) const;
	bool			operator >  (const moBase& object) const;
	bool			operator >= (const moBase& object) const;

private:
	compare_t		OperatorCompare(const moBase& object) const;
	static void *		AllocObject(size_t size, bool array);
	static void		FreeObject(void *object, bool array);
	static bool		FindObject(moBase *object);

	static mo_atomic_word_t	g_serial_counter;

	const bool		f_dynamic_object;
	const mo_atomic_word_t	f_serial;
	mutable mo_atomic_word_t f_reference_count;
};

typedef moSmartPtr<moBase>	moBaseSPtr;


};		// namespace molib;

// vim: ts=8 sw=8 syntax=cpp.doxygen
#endif		// #ifndef MO_BASE_H

