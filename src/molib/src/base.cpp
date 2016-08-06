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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_base.h"
#endif

/*! \mainpage Made to Order Software Corporation Library (molib) Developer Documentation
 *
 * \section intro_sec Introduction
 *
 * Congratuations on your purchase of the molib, Made to Order Software Corporation's premier
 * crossplatform and developer tool library. This library was created over time by our professional
 * software engineers who found it useful to have common tasks done once and done right.
 *
 * Use molib to help insure crossplatform portability in your C++ applications. Don't reinvent the wheel--
 * it's possible we've already done it for you!
 *
 * \section xplat A Cross-Platform Toolkit
 *
 * A robust application toolkit library, molib gives you the power to develop
 * cross-platform applications quickly and easily. It abstracts the system
 * calls through C++ objects (such as the file system, database system and
 * thread implementation.) This allows for the creation of applications that
 * compile and run under MS-Windows, Mac OS/X and Linux without having to
 * modify anything in your code.
 *
 * \sa molib::moApplication
 *
 * \section security Use molib to Program in C++ with Many Added Secure Features
 *
 * A C++ compiler, by itself, offers only minor security measures, such as type
 * safety and proper syntax. This is about the extent of it. However, depending
 * on the code you write and the libraries you link against, C++ code can be
 * vastly improved, becoming as safe as Ada or Eiffel.

 * molib offers the base for this desired security and robustness, providing the use of smart pointers,
 * controlled integer and floating point types, a safe (and fast) memory
 * allocator for reference counted objects, safe buffer, list and array
 * management, and more!
 *
 * Our variable guard technology allows your developers to write the code while
 * the compiler detects the errors, such as forgetting to initialize class
 * variables. This added feature, coupled with our memory leak detection, helps
 * you bring stable and secure code to market in a timely manner!
 *
 * molib also has an integrated string buffer with built-in password
 * sensitivity; when the string is destroyed, any data is wiped from memory. 
 *
 * \sa molib::moSmartPtr, molib::auto_init, molib::moBase 
 *
 * \section thread_safety Thread Safety
 *
 * A C++ compiler, by itself, offers only minor security measures, such as type
 * safety and proper syntax. This is about the extent of it. However, depending
 * on the code you write and the libraries you link against, C++ code can be
 * vastly improved, becoming as safe as Ada or Eiffel.
 *
 * molib offers the base for this desired security and robustness, providing the
 * use of smart pointers, controlled integer and floating point types, a safe (and
 * fast) memory allocator for reference counted objects, safe buffer, list and
 * array management, and more!
 * 
 * Our variable guard technology allows your developers to write the code while
 * the compiler detects the errors, such as forgetting to initialize class
 * variables. This added feature, coupled with our memory leak detection, helps
 * you bring stable and secure code to market in a timely manner!
 * 
 * molib also has an integrated string buffer with built-in password sensitivity;
 * when the string is destroyed, any data is wiped from memory. 
 *
 * \sa molib::moThread
 *
 * \section web Power your Web Server with C++
 *
 * molib provides modb, a robust set of classes designed to * handle database access
 * and SQL queries against PostgresSQL and other popular database engines. Use
 * moCGI to lend instant CGI support. molib also provides excellent abstraction
 * for handling XML, HTML and other markup languages. You can "soup up" your web
 * applications by coding modules in C++, providing added speed for serving your
 * clients.
 *
 * \section properties Protect your Property!
 *
 * Our Property Bag technology gives your developers a robust, object-oriented,
 * and easy way to persist data—be it to user data files or global
 * configuration data. Coupled with our application management object, you can
 * save application settings easily no matter what the target environment is!
 *
 * \sa molib::moPropBag, molib::moPropBagRef
 *
 * \section features Technical Features
 *
 *  \li Supported platforms:
 *  \code
 *  	MS-Windows 2000/XP/Vista
 *  	GNU/Linux
 *  	Mac OS/X 10.4 and better
 * \endcode
 *
 * \li Safe integer and floating point types (mo_controlled.h)
 * \li Thread safe Property Bags (molib::moPropBag)
 * \li Thread safe events using true C++ function calls (molib::moEvents)
 * \li Cross-platform application managment (molib::moApplication)
 * \li Reference counted objects (molib::moBase) and smart pointer classes (molib::moSmartPointer)
 * \li Sorted lists (molib::moList)
 * \li Sorted arrays (molib::moArray)
 * \li String handling with built-in internationalization support (molib::moString)
 * \li C expressions parsing (molib::moExpr)
 * \li Configuration file parsing and makefile like variable reducer (molib::moVariable)
 * \li XML file and string parsing including the mo language and database support (molib::moXML)
 * \li Memory buffer management (molib::moBuffer)
 * \li Regular expression handling (molib::moRegExpr)
 * \li Command line handling (molib::moGetOpt)
 * \li Image processing (molib::moImage)
 * \li Compression handling (molib::moGzip)
 * \li File access abstraction (molib::moIOStream, moFile, moMemFile and moDirectory)
 * \li Thread handling classes (molib:(molib::moThread)
 * \li Database classes (molib::moDatabase)
 * \li Memory leak detection plus debug code analysis (molib::moBase and moError)
 * \li CGI script basic handling (molib::moCGI)
 * \li Many Image file format load and saving (molib::moImage)
 * \li Module/Plug-in support (molib::moModule)
 * \li Simple Unix-like Password file handling (molib::moPasswd)
 * \li Undo/Redo transaction management with simplified C++ (molib::moTransaction)
 * \li Integrates with the sandbox for GUI support (molib::motk)
 *
 */


#include	"mo/mo_base.h"

// Yes! base.c++ is dependent on the moMutex object.
#ifndef MO_MUTEX_H
#include	"mo/mo_mutex.h"
#endif

#include	<new>
#include	<stdexcept>

#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif

namespace molib
{

/// A unique number assigned to each object for tracking purposes.
mo_atomic_word_t		moBase::g_serial_counter;



namespace
{

const unsigned long		MO_CACHE_ENTRIES = 1024;
const unsigned long		MO_CACHE_ALIGNMENT = 64;



enum {
	TYPE_MALLOC,
	TYPE_CALLOC,
	TYPE_REALLOC,
	TYPE_FREE
};

enum cache_t {
	CACHE_NO,
	CACHE_YES
};


// on a BIG ENDIAN we may want to swap the bit fields
struct malloc_buf_t {
	malloc_buf_t	*f_next;
	malloc_buf_t	*f_previous;
	unsigned long	f_size : 29;
	unsigned long	f_type : 2;
	unsigned long	f_cache : 1;	// in the moBase cache (i.e. not allocated when set to 1)
	const char *	f_info;		// name of this buffer (static string)
};


// list of allocated buffers
malloc_buf_t *		g_malloc_bufs;


// stats
unsigned long		g_calloc_count;
unsigned long		g_malloc_count;
unsigned long		g_free_count;
unsigned long		g_realloc_count;



// TODO:
// In order to use the allocation/free counters in a useful way in a
// multi-thread environment, we need to have counters which count on
// a per thread basis. That way, you can ask the library to break on
// a per thread basis which makes more sense than having a single
// counter as we have right now. This requires a map... What are you
// talking about?!



/// Private function to get the memory mutex pointer in multithreading environment
moMutex *GetAllocMutex(void)
{
	static moMutex *g_alloc_mutex;

	// allocated the first time called
	// (we assume that you don't create threads in globals,
	// and really, who would do that?!?!)
	if(g_alloc_mutex == 0) {
		g_alloc_mutex = new moMutex;
		// WARNING: this is a leak!
	}

	return g_alloc_mutex;
}


/** \brief Mark a buffer as cached or not cached.
 *
 * This function mark a buffer previously allocated with one
 * of the mo_... allocation functions (i.e. mo_malloc(), mo_realloc()
 * or mo_calloc().)
 *
 * A buffer cannot be of type FREE to be marked as cached.
 *
 * The object allocator uses this function to keep deleted objects
 * in its caches.
 *
 * \param[in] ptr   The pointer of the buffer to mark
 * \param[in] cache  CACHE_YES (cached) or CACHE_NO (not cached)
 *
 * \sa mo_malloc()
 * \sa mo_realloc()
 * \sa mo_calloc()
 */
void mo_set_cache(void *ptr, cache_t cache)
{
	if(ptr == 0) {
		return;
	}

	malloc_buf_t *p;
	p = reinterpret_cast<malloc_buf_t *>(ptr) - 1;
	assert(p->f_type != TYPE_FREE);
	p->f_cache = cache;
}


};		// namespace



/** \brief Allocate and reset a buffer of memory.
 *
 * This function allocates a memory buffer using the system
 * calloc() call. It adds enough space to include a
 * malloc_buf_t at the beginning in order to link the
 * buffer in the list of newly allocated buffers.
 *
 * \note
 * All the buffers allocated with mo_... functions are
 * put in the same list to be able to track memory
 * leaks.
 *
 * \param[in] count  The number of items to allocate
 * \param[in] size   The size of one item to allocate
 * \param[in] info   A static constant string describing the buffer
 *
 * \return The newly allocated pointer.
 */
void *mo_calloc(size_t count, size_t size, const char *info)
{
	malloc_buf_t *p;
	size *= count;
	p = reinterpret_cast<malloc_buf_t *>(calloc(1, size + sizeof(malloc_buf_t)));
	if(p == 0) {
		throw std::bad_alloc();
	}
	p->f_previous = 0;
	p->f_type = TYPE_CALLOC;
	p->f_cache = CACHE_NO;
	p->f_size = static_cast<unsigned long>(size);
	p->f_info = info;
	moLockMutex lock(*GetAllocMutex());
	++g_calloc_count;
	p->f_next = g_malloc_bufs;
	g_malloc_bufs = p;
	if(p->f_next != 0) {
		p->f_next->f_previous = p;
	}
	return p + 1;
}


/** \brief Allocate a buffer of memory.
 *
 * This function allocates a memory buffer using the system
 * malloc() call. It adds enough space to include a
 * malloc_buf_t at the beginning in order to link the
 * buffer in the list of newly allocated buffers.
 *
 * \note
 * All the buffers allocated with mo_... functions are
 * put in the same list to be able to track memory
 * leaks.
 *
 * \param[in] size   The size of the memory block to allocate
 * \param[in] info   A static constant string describing the buffer
 *
 * \return The newly allocated pointer.
 */
void *mo_malloc(size_t size, const char *info)
{
	malloc_buf_t *p;
	p = reinterpret_cast<malloc_buf_t *>(malloc(size + sizeof(malloc_buf_t)));
	if(p == 0) {
		throw std::bad_alloc();
	}
	p->f_previous = 0;
	p->f_type = TYPE_MALLOC;
	p->f_cache = CACHE_NO;
	p->f_size = static_cast<unsigned long>(size);
	p->f_info = info;
	moLockMutex lock(*GetAllocMutex());
	++g_malloc_count;
	p->f_next = g_malloc_bufs;
	g_malloc_bufs = p;
	if(p->f_next != 0) {
		p->f_next->f_previous = p;
	}
#ifdef MO_DEBUG
	// mark the buffer as uninitialized (0xCD all over)
	memset(p + 1, 0xCD, size);
#endif
	return p + 1;
}


/** \brief Release a buffer of memory.
 *
 * This function releases a memory buffer previously
 * allocated with mo_malloc(), mo_calloc() or mo_realloc().
 *
 * The buffer cannot be reused aferward.
 *
 * The function checks to know whether the buffer was
 * previously allocated with an mo_... allocation function.
 * If not it fails (usually only debug mode; can be used
 * in release mode too.)
 *
 * \note
 * All the buffers allocated with mo_... functions are
 * put in the same list to be able to track memory
 * leaks.
 *
 * \param[in] ptr    The pointer to the memory to release
 */
void mo_free(void *ptr)
{
	// this is similar to delete ptr; so we want this test!
	if(ptr == 0) {
		return;
	}

	malloc_buf_t *old;
	old = reinterpret_cast<malloc_buf_t *>(ptr) - 1;
	moLockMutex lock(*GetAllocMutex());
	assert(old->f_type != TYPE_FREE);
	old->f_type = TYPE_FREE;  // make sure we don't free this twice
// put 0 or 1 at the end so as to have a check of the pointer being
// freed on every single call to mo_free()
#if defined(MO_DEBUG) || 1
{
	malloc_buf_t *p;
	p = g_malloc_bufs;
	while(p != 0) {
		if(p == old) {
			goto found;
		}
		p = p->f_next;
	}
	for(;;) assert(0);	// ERROR: can't find buffer being freed!
found:;
}
#endif
#ifdef MO_DEBUG
	// mark the buffer as freed (0xEE all over)
	memset(old + 1, 0xEE, old->f_size);
#endif

// to test whether there are some buffer overflow and such
// you can keep the buffers allocated at all time!
#if 1
	if(old->f_next != 0) {
		old->f_next->f_previous = old->f_previous;
	}
	if(old->f_previous != 0) {
		old->f_previous->f_next = old->f_next;
	}
	else {
		assert(old == g_malloc_bufs);
		g_malloc_bufs = old->f_next;
	}
	free(old);
#endif
	++g_free_count;
}


/** \brief Allocate, enlarge, reduce or free a buffer of memory.
 *
 * This function calls the system realloc() function to resize
 * a buffer, unless the old pointer is NULL in which case it
 * calls malloc().
 *
 * If ptr is NULL, then this function calls mo_malloc() instead.
 *
 * If the size is zero, then this function still allocates a
 * buffer, yet that buffer will be empty. It is therefore NOT
 * similar to mo_free().
 *
 * If ptr is not NULL and the size is not zero, the buffer is
 * resized if necessary and a new pointer (that may be the same
 * as ptr) is returned. Do not assume that ptr will not change
 * even if you are only reducing the size of your buffer.
 *
 * \note
 * All the buffers allocated with mo_... functions are
 * put in the same list to be able to track memory
 * leaks.
 *
 * \bug
 * When the buffer is enlarged, the extra bytes are not guaranteed
 * to be cleared even if the buffer was first allocated with a call
 * to mo_calloc().
 *
 * \param[in] ptr    The pointer to resize
 * \param[in] size   The new size of the memory block
 * \param[in] info   A static constant string describing the buffer
 *
 * \return The newly allocated pointer.
 */
void *mo_realloc(void *ptr, size_t size, const char *info)
{
	malloc_buf_t *p, *old;
	if(ptr == 0) {
		// on some system realloc(0, size) won't work!
		return mo_malloc(size, info);
	}
	old = reinterpret_cast<malloc_buf_t *>(ptr) - 1;
	assert(old->f_type != TYPE_FREE);
	moLockMutex lock(*GetAllocMutex());
#if defined(MO_DEBUG) || 1
if(old != 0) {
	malloc_buf_t *p;
	p = g_malloc_bufs;
	while(p != 0) {
		if(p == old) {
			goto found;
		}
		p = p->f_next;
	}
	for(;;) assert(0);	// ERROR: can't find buffer being reallocated!
found:;
}
#endif
	++g_realloc_count;
	p = reinterpret_cast<malloc_buf_t *>(realloc(old, size + sizeof(malloc_buf_t)));
	if(p == 0) {
		throw std::bad_alloc();
	}
#if defined(MO_DEBUG)
	if(size > p->f_size) {
		memset(reinterpret_cast<char *>(p + 1) + p->f_size, 0xF5, size - p->f_size);
	}
#endif
	p->f_type = TYPE_REALLOC;
	p->f_cache = CACHE_NO;
	p->f_size = static_cast<unsigned long>(size);
	p->f_info = info;

	// if p changed we need to relink it properly in the list
	if(p->f_next != 0) {
		p->f_next->f_previous = p;
	}
	if(p->f_previous != 0) {
		p->f_previous->f_next = p;
	}
	else {
		g_malloc_bufs = p;
	}

	return p + 1;
}




/** \brief Print a list of still allocated buffers.
 *
 * This function prints the list of still allocated buffers.
 *
 * Buffers marked as CACHE_YES are not displayed since they
 * have officially been freed.
 *
 * \note
 * This function uses fprintf(3C) with stderr to print out
 * the messages. This should prevent any lock up since this
 * function locks the memory while printing the entire list
 * of leaks.
 *
 * \bug
 * This function calls the still allocated buffers leaks
 * even though it can be called while running a software
 * and thus at a time allocated buffers should not be
 * considered leaks.
 */
void mo_show_allocated_buffers(void)
{
	malloc_buf_t *p;

	moLockMutex lock(*GetAllocMutex());

	fprintf(stderr, "============================== LEAKS (mo_malloc) =====================\n");
	fprintf(stderr, "Total number of calloc: %lu, malloc: %lu, realloc: %lu (total: %lu)\n",
				g_calloc_count, g_malloc_count, g_realloc_count,
				g_calloc_count + g_malloc_count + g_realloc_count);
	fprintf(stderr, "Total number of free: %lu\n", g_free_count);
	fprintf(stderr, "Total number of buffers still allocated: %lu\n", g_calloc_count + g_malloc_count + g_realloc_count - g_free_count);

	unsigned long cached = 0;
	try {
		p = g_malloc_bufs;
		assert(p == 0 || p->f_previous == 0);
		while(p != 0) {
			// ignore entries in the moBase cache
			if(p->f_cache == CACHE_NO) {
				fprintf(stderr, " * %s of %lu bytes at %p, info: \"%s\"\n",
					p->f_type == TYPE_MALLOC ? "malloc"
					: (p->f_type == TYPE_CALLOC ? "calloc"
					: (p->f_type == TYPE_REALLOC ? "realloc" : "free")),
					(unsigned long) p->f_size, p + 1, p->f_info);
			}
			else {
				cached++;
			}
			p = p->f_next;
		}
	}
	catch(...) {
		fprintf(stderr, "\r  ... error while printing the memory leaks out.\n");
	}

	fprintf(stderr, "Total number of buffer cached (not shown as leaks): %lu\n", cached);
}



namespace
{

struct free_buffer_t {
	struct free_buffer_t *		f_next;
};

struct cache_entry_t {
	unsigned long			f_count;
	free_buffer_t *			f_free;
};

struct used_buffer_t {
	struct used_buffer_t *		f_next;
	struct used_buffer_t *		f_previous;
	moBase *			f_base;
	size_t				f_size;
	bool				f_array;
	//bool				f_new; -- f_base == 0 -> new!
	unsigned int			f_magic;		// == MAGIC_VALUE
};

const unsigned int MAGIC_VALUE = 0x91827364;



cache_entry_t		g_cache_entries[MO_CACHE_ENTRIES];
used_buffer_t		*g_new_buffers;		// constructor not yet called
used_buffer_t		*g_init_buffers;	// some constructor called, maybe not all of them yet (i.e. ptr != base)
used_buffer_t		*g_used_buffers;	// constructor moved pointer here
void			*g_smallest;
void			*g_largest;
unsigned long		g_count;		// total number of buffers allocated
unsigned long		g_allocated;		// number of buffers currently allocated
unsigned long		g_used;			// number of used buffers
unsigned long		g_max_used;		// maximum number of objects used at any one time

};		// namespace



/** \brief Empty the object cache.
 *
 * This function is used to clear the cache. It is used
 * whenever a buffer allocation fails.
 *
 * Within a software, it is seldom a requirement to call
 * this function. It can be useful if you have loops allocating
 * a type of object, then another, then another... Clearing the
 * cache between each loop can help save some memory.
 */
void moBase::EmptyCache(void)
{
	unsigned int	idx;
	free_buffer_t	*f, *n;

	moLockMutex lock(*GetAllocMutex());

	for(idx = 0; idx < MO_CACHE_ENTRIES; ++idx) {
		g_cache_entries[idx].f_count = 0;
		f = g_cache_entries[idx].f_free;
		while(f != 0) {
			n = f->f_next;
			mo_free(f);
			f = n;
		}
		g_cache_entries[idx].f_free = 0;
	}
}



/** \brief Allocate a buffer for an object.
 *
 * The AllocObject() function allocates a buffer to be used
 * by an object. The size defines the total size to be used by
 * that object. The allocated object will also include a header.
 *
 * Objects are linked together so we can print the list of leaks
 * once we quit a program.
 *
 * \param[in] size   The size necessary for the buffer to allocate
 * \param[in] array  Whether the new object is an array (new[])
 *
 * \return A pointer to the newly allocated buffer.
 */
void *moBase::AllocObject(size_t size, bool array)
{
	used_buffer_t	*p;
	free_buffer_t	*f;
	cache_entry_t	*cache;
	unsigned int	idx;
	size_t		sz;

	idx = (unsigned int) (size + MO_CACHE_ALIGNMENT - 1) / MO_CACHE_ALIGNMENT;
	if(idx < MO_CACHE_ENTRIES) {
		cache = g_cache_entries + idx;
		sz = idx * MO_CACHE_ALIGNMENT;
	}
	else {
		cache = 0;
		sz = size;
	}

	p = 0;

	moLockMutex lock(*GetAllocMutex());

	if(cache != 0) {
		// use the cached entry
		// we only increase the counter (never decrease),
		// that's to know whether we should keep these
		// buffers or free them at once in the FreeObject()
		if(cache->f_count < INT_MAX) {
			cache->f_count++;
		}
		if(cache->f_free != 0) {
			f = cache->f_free;
			cache->f_free = f->f_next;
			p = reinterpret_cast<used_buffer_t *>(f);
			mo_set_cache(p, CACHE_NO);
		}
	}

	if(p == 0) {
		// no cached entry, allocate a new buffer
		g_count++;
		g_allocated++;
		p = static_cast<used_buffer_t *>(mo_malloc(sz + sizeof(used_buffer_t), "moBase object"));
		if(static_cast<void *>(p) < g_smallest || g_smallest == 0) {
			g_smallest = static_cast<void *>(p);
		}
		if(static_cast<void *>(reinterpret_cast<char *>(p) + size) > g_largest) {
			g_largest = static_cast<void *>(reinterpret_cast<char *>(p) + size);
		}
	}

	p->f_size  = size;
	p->f_array = array;
	p->f_base  = 0;		// constructor not called yet
	p->f_magic = MAGIC_VALUE;

	p->f_previous = 0;
	p->f_next = g_new_buffers;
	g_new_buffers = p;

	g_used++;
	if(g_used > g_max_used) {
		g_max_used = g_used;
	}

#ifdef MO_DEBUG
	// mark the buffer as not yet initialized (0xCC all over)
	memset(p + 1, 0xCC, size);
#endif

	return static_cast<void *>(p + 1);
}



/** \brief Release an object.
 *
 * This function frees an object previous allocated with AllocObject().
 *
 * The buffer used for the object will be saved in the cache if it fits
 * one of the predefined cache sizes and many objects of the same size
 * have been allocated. Otherwise, the buffer is restored to the heap
 * with a call to mo_free().
 *
 * \note
 * In debug, the buffer is set to 0xEE.
 *
 * \bug
 * The FreeObject() function checks whether the desallocator correspond
 * to the allocator. In other words, it expects delete to be called when
 * the object was created by a new; and delete[] when created by a new[].
 *
 * \param[in] object   The pointer to the object to free.
 * \param[in] array    Whether the object was an array (delete[]).
 *
 * \sa mo_free()
 * \sa moBase::AllocObject()
 */
void moBase::FreeObject(void *object, bool array)
{
	used_buffer_t	*p;
	free_buffer_t	*f;
	cache_entry_t	*cache;
	unsigned int	idx;

	p = reinterpret_cast<used_buffer_t *>(object) - 1;

	// if this assert() fails, that's really bad since it
	// means an object is being destroyed even before it
	// was initialized by its constructor
	assert(p->f_magic == MAGIC_VALUE);
	assert(p->f_base != 0);

	if(p->f_array ^ array) {
		if(p->f_array) {
			fprintf(stderr, "INTERNAL ERROR: allocated an array with new [] (...), deleting with delete obj\n");
		}
		else {
			fprintf(stderr, "INTERNAL ERROR: allocated an object with new (...), deleting with delete [] obj\n");
		}
	}

#ifdef MO_DEBUG
	// mark the buffer as freed (0xEE all over)
	memset(p + 1, 0xEE, p->f_size);
#endif

	moLockMutex lock(*GetAllocMutex());

	g_used--;

	if(p->f_previous == 0) {
		if(p == g_used_buffers) {
			g_used_buffers = p->f_next;
		}
		else {
			assert(g_init_buffers == p);
			g_init_buffers = p->f_next;
		}
	}
	else {
		p->f_previous->f_next = p->f_next;
	}

	if(p->f_next != 0) {
		p->f_next->f_previous = p->f_previous;
	}

	idx = (unsigned int) (p->f_size + MO_CACHE_ALIGNMENT - 1) / MO_CACHE_ALIGNMENT;
	if(idx < MO_CACHE_ENTRIES) {
		cache = g_cache_entries + idx;
		if(idx <= 16U || cache->f_count > idx / 3) {
			// ha! we shall cache this buffer
			f = reinterpret_cast<free_buffer_t *>(p);
			f->f_next = cache->f_free;
			cache->f_free = f;
			mo_set_cache(p, CACHE_YES);
			return;
		}
	}

	mo_free(p);
	g_allocated--;
}


/** \brief Search an object to know how to initialize it.
 *
 * Objects allocated on the heap with moAllocObject() are
 * called dynamic objects.
 *
 * Objects allocated on the stack or within another object
 * are called static or embedded (members or aggregates).
 *
 * Objects that are allocated on the heap can make use of
 * the reference counter handled by the moBase class.
 *
 * Objects that are not allocated on the heap cannot be
 * deleted and thus cannot have their reference counter
 * drop to zero. If that happens, an error is raised.
 *
 * This function is used to search for objects to determine
 * whether they have been allocated on the heap or not.
 *
 * It is called once by the moBase constructor to know whether
 * the new object was allocated on the heap or not. If
 * allocated on the heap, then the function returns
 * true which means the reference count needs to be initialized
 * with 0 and the f_dynamic_object is set to true. Otherwise,
 * the reference counter is set to 1 and f_dynamic_object is
 * set to false.
 *
 * \bug
 * The FindObject() function can really be called only once per
 * object. After the first time, it will always return false.
 * Since it is a private function called from the moBase
 * constructor, we should never have any problems.
 *
 * \bug
 * Under Unices (Linux & Mac OS/X) the memory is always allocated
 * in a clean way, increasing from right after the text section
 * up toward the stack. In other words, the FindObject() function
 * will always return right away if the object is a stack object.
 * Under Microsoft Windows, however, I'm not so sure that the stack
 * is way before or after all the possible address space used for
 * the heap memory. The algorithm to quickly test for a heap object
 * may need enhancements under that system.
 *
 * \bug
 * Note that since we use two lists, one with the new objects that
 * are to be initialized by their constructor and the other that
 * includes all the objects already initialized, it is unlikely
 * going to be very slow. However, that FindObject() function will
 * be called for every moBase object that is part of another
 * object as well as objects on the stack! The only reason for a
 * real slowdown would be if you have many threads allocating
 * objects and their constructors are now being called before
 * a switch to another thread. In all other cases, it should be
 * really fast.
 *
 * \param[in] object   A pointer to the object to search
 *
 * \return true if the object is dynamic
 *
 * \sa AllocObject()
 */
bool moBase::FindObject(moBase *object)
{
	used_buffer_t	*n, *p;

	p = reinterpret_cast<used_buffer_t *>(object) - 1;

	moLockMutex lock(*GetAllocMutex());

	// objects on the stack will have a pointer larger than g_largest
	// and that happens very often so we do test that first
	if(p > g_largest
	|| p < g_smallest
	|| g_smallest == 0		// not initialized yet?!
	/* || (g_new_buffers == 0 && g_init_buffers == 0) -- this is unlikely */ ) {
		return false;
	}

	n = 0;

	// if we have a pointer in the g_new_buffers, search there first (will be true 99% of the time)
	if(g_new_buffers != 0) {
		// in most cases, if it was allocated on the heap, the following
		// if() should be true
		if(static_cast<void *>(p) >= static_cast<void *>(g_new_buffers)
		&& reinterpret_cast<char *>(p) < reinterpret_cast<char *>(g_new_buffers) + g_new_buffers->f_size) {
			n = g_new_buffers;
			g_new_buffers = g_new_buffers->f_next;
			if(g_new_buffers != 0) {
				g_new_buffers->f_previous = 0;
			}
		}
		else {
			for(n = g_new_buffers->f_next; n != 0; n = n->f_next) {
				if(static_cast<void *>(p) >= static_cast<void *>(n)
				&& reinterpret_cast<char *>(p) < reinterpret_cast<char *>(n) + n->f_size) {
					if(n->f_previous != 0) {
						n->f_previous->f_next = n->f_next;
					}
					if(n->f_next != 0) {
						n->f_next->f_previous = n->f_previous;
					}
					break;
				}
			}
		}
	}
	if(n == 0 && g_init_buffers != 0) {
		// this happens whenever you have an object which doesn't first derive
		// from an moBase object (multi-derivation with some 3rd party being derived first)
		for(n = g_init_buffers; n != 0; n = n->f_next) {
			if(static_cast<void *>(p) >= static_cast<void *>(n)
			&& reinterpret_cast<char *>(p) < reinterpret_cast<char *>(n) + n->f_size) {
				break;
			}
		}
		if(n != 0 && static_cast<void *>(n) != static_cast<void *>(p)) {
			if(static_cast<void *>(n->f_base) > static_cast<void *>(p)) {
				const_cast<bool&>(n->f_base->f_dynamic_object) = false;
				n->f_base->f_reference_count++;
				n->f_base = object;
			}
			return true;
		}
		if(static_cast<void *>(n) == static_cast<void *>(p)) {
			// unlink if we are to move this object to the g_user_buffers
			if(n->f_previous != 0) {
				n->f_previous->f_next = n->f_next;
			}
			else {
				g_init_buffers = g_init_buffers->f_next;
				if(g_init_buffers != 0) {
					g_init_buffers->f_previous = 0;
				}
			}
			if(n->f_next != 0) {
				n->f_next->f_previous = n->f_previous;
			}
		}
	}
	if(n == 0) {
		return false;
	}


	// it is a heap object and we found it so it isn't new
	// anymore; also, we move it to the g_used_buffers where
	// it now belongs
	n->f_base = object;

	if(static_cast<void *>(n) == static_cast<void *>(p)) {
		n->f_next = g_used_buffers;
	}
	else {
		n->f_next = g_init_buffers;
	}
	if(n->f_next != 0) {
		n->f_next->f_previous = n;
	}
	n->f_previous = 0;
	if(static_cast<void *>(n) == static_cast<void *>(p)) {
		g_used_buffers = n;
	}
	else {
		g_init_buffers = n;
	}

	return true;
}




/** \brief Show the list of objects still allocated.
 *
 * This function prints out all the currently allocated objects.
 * Note that it won't display much about the cache. Just the
 * number of buffers still allocated.
 *
 * \bug
 * The function tries to catch errors that can happen whenever
 * an object was allocated but not yet initialized by another
 * thread. To avoid these problems, either terminate all your
 * threads or make sure they stop at a synchronization point.
 */
void moBase::ShowAllocatedObjects(void)
{
	used_buffer_t	*p = 0;
	moBase		*base;

	moLockMutex lock(*GetAllocMutex());

	fprintf(stderr, "================================ LEAKS (objects) =====================\n");
	fprintf(stderr, "Total number of new: %ld\n", g_count);
	fprintf(stderr, "Number of buffers currently allocated: %ld\n", g_allocated);
	fprintf(stderr, "Number of objects in use: %ld\n", g_used);
	fprintf(stderr, "Maximum number of objects in use at once: %ld\n", g_max_used);

	try {
		p = g_new_buffers;
		while(p != 0) {
			// We assume that new objects cannot have moGetClassName() and ReferenceCount() working
#ifdef _MSC_VER
			fprintf(stderr, " * New object of %d bytes at 0x%08I64X (raw ptr: 0x%08I64X).\n",
						p->f_size,
						reinterpret_cast<unsigned __int64>(p + 1),
						reinterpret_cast<unsigned __int64>(p));
#else
			fprintf(stderr, " * New object of %zd bytes at 0x%08lX (raw ptr: 0x%08lX).\n",
						p->f_size,
						reinterpret_cast<unsigned long>(p + 1),
						reinterpret_cast<unsigned long>(p));
#endif
			p = p->f_next;
		}
	}
	catch(...) {
		fprintf(stderr, "\r  ... an error occured while listing the new objects.\n");
	}

	try {
		p = g_init_buffers;
		while(p != 0) {
			// We assume that new objects cannot have moGetClassName() and ReferenceCount() working
#ifdef _MSC_VER
			fprintf(stderr, " * New object of %d bytes at 0x%08I64X.\n",
						p->f_size, reinterpret_cast<unsigned __int64>(p + 1));
#else
			fprintf(stderr, " * New object of %zd bytes at 0x%08lX.\n",
						p->f_size, reinterpret_cast<unsigned long>(p + 1));
#endif
			p = p->f_next;
		}
	}
	catch(...) {
		fprintf(stderr, "\r  ... an error occured while listing the multi-derived objects.\n");
	}

	try {
		p = g_used_buffers;
		while(p != 0) {
			try {
				//dynamic_cast<moBase *>(p + 1) ... -- this won't work because we don't even
				// know what object we're dealing with... and also saving the pointer to the
				// moBase object in the FindObject is enough to have access to it later!
				base = p->f_base;
				fprintf(stderr,
#ifdef _MSC_VER
					" * Object of %d bytes at 0x%08I64X of class \"%s\" still referenced %lu time%s.\n",
						p->f_size, reinterpret_cast<unsigned __int64>(p + 1),
#else
					" * Object of %zd bytes at 0x%08lX of class \"%s\" still referenced %lu time%s.\n",
						p->f_size, reinterpret_cast<unsigned long>(p + 1),
#endif
						base->moGetClassName(),
						base->ReferenceCount(),
						base->ReferenceCount() == 1 ? "" : "s");
			}
			catch(...) {
				fprintf(stderr,
#ifdef _MSC_VER
					"\r * Object of %d bytes at 0x%08I64X, cannot get the class and/or reference count.     \n",
						p->f_size, reinterpret_cast<unsigned __int64>(p + 1));
#else
					"\r * Object of %zd bytes at 0x%08lX, cannot get the class and/or reference count.     \n",
						p->f_size, reinterpret_cast<unsigned long>(p + 1));
#endif
			}
			p = p->f_next;
		}
	}
	catch(...) {
		fprintf(stderr, "\r  ... an error occured while listing the initialized objects.\n");
	}
}



moBase::moBase(void)
	: f_dynamic_object(FindObject(this)),
#ifdef MO_THREAD
	  f_serial(moAtomicAdd(&g_serial_counter, 1)),
#else
	  f_serial(++g_serial_counter),
#endif
	  f_reference_count(f_dynamic_object ? 0 : 1)
{
}


// NOTE: the input object is being ignored in the base
moBase::moBase(const moBase& object)
	: f_dynamic_object(FindObject(this)),
#ifdef MO_THREAD
	  f_serial(moAtomicAdd(&g_serial_counter, 1)),
#else
	  f_serial(++g_serial_counter),
#endif
	  f_reference_count(f_dynamic_object ? 0 : 1)
{
}


moBase& moBase::operator = (const moBase& object)
{
	// the respective reference counters
	// serial number and stack flag all
	// remain the same!
	return *this;
}


moBase::~moBase()
{
#ifdef MO_DEBUG
	// NOTE: objects on your stack will always have a reference count
	//	 of 1 on destruction, since we know whether an object is
	//	 on the stack or not (static or dynamic) we can test the
	//	 exact reference count on destruction.
	bool dynamic = IsDynamicObject();
	if(f_reference_count != (dynamic ? 0 : 1)) {
		fflush(stdout);
		fprintf(stderr, "INTERNAL ERROR: moBase::~moBase(): deleting a %s object at 0x%p with a reference count of %lu (current stack at 0x%p)\n", dynamic ? "dynamic" : "static", this, (unsigned long) f_reference_count, &dynamic);
		throw std::logic_error("moBase::~moBase(): deleting an object with an incompatible reference count");
	}
#endif
}


void *moBase::operator new (size_t size)
{
	return AllocObject(size, false);
}


void *moBase::operator new [] (size_t size)
{
	return AllocObject(size, true);
}


void moBase::operator delete (void *object)
{
	FreeObject(object, false);
}


void moBase::operator delete [] (void *object)
{
	FreeObject(object, true);
}


#ifdef _MSC_VER

void *moBase::operator new ( size_t size, void* foo )
{
	return foo;
}

void moBase::operator delete ( void *object, void* foo )
{
}

#endif


/*! \brief Returns the serial number of an object
 *
 *	All objects created from an moBase object will receive a unique
 *	serial number. That number can be retrieved using the Serial()
 *	function.
 *	
 *	Note that the serial number is unique among all the objects of
 *	a binary instance. It is not unique among the whole running
 *	operating system.
 *
 * \section Synopsis
 *
 * \code
 *	moAtomicWord Serial(void) const;
 * \endcode
 *
 * \return Returns a uint64_t unique number.
 *
 * \sa moBase::moBase
 *
 * \bug
 *	At this time, in a multi-thread environment, the serial number
 *	may not be unique. This is a problem in the constructor.
 */
mo_atomic_word_t moBase::Serial(void) const
{
	return f_serial;
}


/*! \brief Return true if the object was allocated with new.
 *
 *	This function returns the dynamic flag.
 *	
 *	Whenever an object is allocated using the new operator, then
 *	it is considered dynamic. This means it can safely use the
 *	AddRef() and Release() functions to fully manage the object.
 *	
 *	To the contrary, objects either allocated on the stack or
 *	which are part of another object (say, a variable member of
 *	moWCString type will be viewed as a non-dynamic object).
 *	
 *	A non-dynamic object can still be AddRef()'ed and Release()'d
 *	however, it won't be freed with the last Release().
 *	
 *	Note that if you create an object on the stack and give it
 *	to another which saves a pointer to it in a smart pointer,
 *	then return from your current function (meaning that the
 *	objects on the stack will be destroyed) would normally generate
 *	a dangling pointer. With this system, in debug we detect such
 *	errors and break the program at the time the destructor is
 *	hit since it will know that some other object need this object.
 *	Unfortunatly, there isn't much we can do in release mode other
 *	than generate an exception.
 *
 * \section Synopsis
 *
 * \code
 * 	bool IsDynamicObject(void) const;
 * \endcode
 *
 * \note
 *
 *	This function works even when the object pointer is NULL.
 *	This is useful in case you use an moSmartDynamicPtr.
 *
 * \return
 *	True when the object is truly dynamic.
 *
 * \sa moBase, ~moBase, AddRef, Release
 */
bool moBase::IsDynamicObject(void) const
{
	return this == 0 ? false : f_dynamic_object;
}


/** \brief Get the name of an object instantiated from an moBase derived class
 *
 * This function is expected to be overloaded in any object
 * derived from the moBase.
 *
 * \note
 * It is expected that the body of the function is simply a
 * return statement with a static string representing the
 * name of the class.
 *
 * \code
 * return "molib::moBase";
 * \endcode
 *
 * \bug
 * This is a virtual function and thus it won't work on null
 * pointers nor from within a constructor. The latter will not
 * recognize the full scope of the class, but it at least won't
 * crash.
 *
 * \return A pointer to a static constant string.
 */
const char *moBase::moGetClassName(void) const
{
	return "molib::moBase";
}


/*! \brief Returns the amount of currently allocated memory of this object.
 * 
 * 	This function is expected to be overloaded in any object
 * 	derived from the moBase and to return the exact amount of
 * 	bytes allocated by the object.
 * 
 * 	The total needs to include a sizeof() of the object and
 * 	the total SizeOf() of each sub-object and mo_malloc()
 * 	objects. In case a system object is allocated (i.e. a
 * 	FILE when you call fopen() for instance), you want to
 * 	try to return the size of that object.
 * 
 * 	Note that whenever an object includes sub-objects (i.e.
 * 	moFile includes an moWCString), it needs to call the
 * 	SizeOf() of that object and subtracts the sizeof() of
 * 	the object since it will be included in the returned
 * 	value.
 * 
 * 	The purpose of this function is only to be informative.
 * 	Thus, if an object SizeOf() function isn't implemented,
 * 	it is just fine. The size will of course be wrong in that
 * 	case.
 * 
 * 	The main idea behind creating such a function is to be
 * 	able to know the total amount of memory allocated at a
 * 	given point in time. This can be useful for different
 * 	debugging tasks.
 * 
 * \section Synopsis
 * \code
 * 	virtual size_t SizeOf(void) const;
 * \endcode
 *
 * \return The number of bytes allocated.
 *
 * \sa moGetClassName
 *
 * \bug The value returned is likely wrong if the object didn't
 *	properly implement its own SizeOf() function!
 *
 */
size_t moBase::SizeOf(void) const
{
	return sizeof(moBase);
}


/*! \brief Add one to the reference counter of the object.
 *
 *	Any object derived from moBase benefits from a reference counter.
 *	This counter can be used to know how many objects reference the
 *	object at a given time. When the last user calls Release(), the
 *	counter reaches 0 and the object deletes itself.
 *
 *	It is possible to check out the reference counter using the
 *	ReferenceCount() function.
 *
 *	The AddRef() should be called every time you make a copy of the
 *	object pointer. The Release() function will be called once you
 *	don't need that pointer any more. The following is the proper
 *	use of the calls when you are replacing a pointer by another.
 *	This ensures that if ptr == f_ptr everything works fine.
 *
 * \code
 *		ptr->AddRef();
 *		f_ptr->Release();
 *		f_ptr = ptr;
 * \endcode
 *
 *	All of these function calls are not virtual and thus work with
 *	NULL pointers.
 *
 * \return The current counter or the counter after the function was applied. If the object pointer
 * is NULL, 0 is returned.
 *
 * \bug At this time, in a multi-thread environment, counters will be wrong (they are not yet protected).
 *
 * \sa Release, ReferenceCount
 *
 */
unsigned long moBase::AddRef(void) const
{
	if(this != 0) {
		return moAtomicAdd(&f_reference_count, 1);
	}

	return 0;
}


/*! \brief Substract one to the reference counter of the object
 *
 * \return The current counter or the counter after the function was applied. If the object pointer is NULL,
 * 0 is returned.
 *
 * \note The Release() is not marked as const because it may delete the object. When Release() returns 0,
 * you know that the object was deleted.
 *
 * \bug At this time, in a multi-thread environment, counters will be wrong (they are not yet protected).
 *
 * \sa AddRef, ReferenceCount
 *
 */
unsigned long moBase::Release(void)
{
	if(this != 0) {
		unsigned long r = moAtomicAdd(&f_reference_count, -1);
		if(r == 0) {
			delete this;
			return 0;
		}
		return r;
	}

	return 0;
}


/*! \brief Return the current value of the object reference counter.
 *
 * \return The current counter or the counter after the function was applied. If the object pointer is NULL,
 * 0 is returned.
 *
 * \bug At this time, in a multi-thread environment, counters will be wrong (they are not yet protected).
 *
 * \sa AddRef, Release
 */
unsigned long moBase::ReferenceCount(void) const
{
	if(this != 0) {
		return f_reference_count;
	}

	return 0;
}


/*! \brief Comparisons -- compare moBase objects between each other.
 *
 *	The moBase class object has a compare function so all objects
 *	which the inherit moBase class can be compared against any
 *	other object which also inherit the moBase class.
 *
 *	When no compare function is declared in an object, then it
 *	will be considered as an object which can't be ordered.
 *
 *	The compare function must return one of the following value:
 *
 *	   MO_BASE_COMPARE_ERROR	an error occured while doing
 *					the comparison
 *
 *	   MO_BASE_COMPARE_SMALLER	'this' is smaller than 'object'
 *
 *	   MO_BASE_COMPARE_EQUAL	the two objects are equal
 *
 *	   MO_BASE_COMPARE_GREATER	'this' is larger than 'object'
 *
 *	   MO_BASE_COMPARE_UNORDERED	these two objects can't be
 *					compared (there is no solution)
 *
 *	Whenever you are comparing two integers (signed or not) you
 *	can use the CompareInt() or CompareUInt() functions instead
 *	of re-writing the same comparison over and over again.
 *
 *	The usual equality, inequality and order operators are all
 *	programmed with a call to the Compare() function. The equality
 *	will quickly test whether 'this' == 'object'. If true, it will
 *	return true right away. When any of the operators receives
 *	MO_BASE_COMPARE_ERROR or MO_BASE_COMPARE_UNORDERED as a result,
 *	a throw is generated. The following shows you which throw is
 *	used by OperatorCompare():
 *	
 * \code
 *	   MO_BASE_COMPARE_ERROR	throw runtime_error();
 *	   MO_BASE_COMPARE_UNORDERED	throw runtime_error();
 *	   <invalid result>		throw logic_error();
 * \endcode
 *
 *	The understood operators are:
 *
 * \code
 *		operator ==
 *		operator !=
 *		operator <
 *		operator >=
 *		operator >
 *		operator <=
 * \endcode
 *
 * \note
 *	Some objects, such as the moString, may have extraneous compare
 *	functions such as CompareNoCase() which will compare two strings
 *	together without taking the case in account. There is no support
 *	for these in the base object since the point was to enable all
 *	the operators (==, !=, <, <=, >, >=) to function without having
 *	to redefine them in each single object. These operators can't
 *	be used for extraneous compares anyway.
 *
 * \bug
 *	It is usually not true that any object can be compared with
 *	any object. The compare is rather intended to compare objects
 *	of the same type. The main idea is to avoid redefining the
 *	generic comparison operators (==, !=, <, <=, >, >=) since
 *	all of these can be computed from the result of Compare().
 *
 * \sa CompareInt, CompareUInt, operator ==, operator !=, operator <,
 * \sa operator <=, operator >, operator >=
 * \sa OperatorCompare
 *
 */
moBase::compare_t moBase::Compare(const moBase& /*object*/) const
{
	// objects are unordered by default!
	return MO_BASE_COMPARE_UNORDERED;
}


/*!
 * \sa Compare
 */
moBase::compare_t moBase::CompareInt(int a, int b)
{
	if(a == b) {
		return MO_BASE_COMPARE_EQUAL;
	}

	if(a < b) {
		return MO_BASE_COMPARE_SMALLER;
	}

	return MO_BASE_COMPARE_GREATER;
}


/*!
 * \sa Compare
 */
moBase::compare_t moBase::CompareUInt(unsigned int a, unsigned int b)
{
	if(a == b) {
		return MO_BASE_COMPARE_EQUAL;
	}

	if(a < b) {
		return MO_BASE_COMPARE_SMALLER;
	}

	return MO_BASE_COMPARE_GREATER;
}


/*!
 * \sa Compare
 */
moBase::compare_t moBase::ComparePtr(const moBase& object) const
{
	if(this == &object) {
		return MO_BASE_COMPARE_EQUAL;
	}

	if(this < &object) {
		return MO_BASE_COMPARE_SMALLER;
	}

	return MO_BASE_COMPARE_GREATER;
}


/*! \brief Comparision under-the-hood method.
 *
 * All the comparison operators are defined in the base since these are
 * equivalent for all objects
 *
 * \sa Compare
 */
moBase::compare_t moBase::OperatorCompare(const moBase& object) const
{
	compare_t	r;

	// fast equality comparison
	// (note that if some objects are unordered, this is actually
	// invalid since it should otherwise throw an error - however
	// it is a very nice acceleration)
	if(this == &object) {
		return MO_BASE_COMPARE_EQUAL;
	}

	r = Compare(object);

	switch(r) {
	case MO_BASE_COMPARE_ERROR:
		//MO_ERROR_COMPARE--should we represent this error # in the string?
		throw std::runtime_error("moBase::OperatorCompare(): The comparison of two objects yielded an error");

	case MO_BASE_COMPARE_UNORDERED:
		//MO_ERROR_UNORDERED--should we represent this error # in the string?
		throw std::runtime_error("moBase::OperatorCompare(): Two objects cannot be ordered");

	case MO_BASE_COMPARE_EQUAL:
	case MO_BASE_COMPARE_SMALLER:
	case MO_BASE_COMPARE_GREATER:
		return r;

	default:
		//MO_ERROR_BAD_COMPARE--should we represent this error # in the string?
		throw std::logic_error("moBase::OperatorCompare(): The Compare() function returned an invalid comparison result");

	}
	/*NOTREACHED*/
#ifdef MO_CONFIG_ALWAYS_NEED_RETURN
	return MO_BASE_COMPARE_ERROR;
#endif
}


/*!
 * \sa Compare
 */
bool moBase::operator == (const moBase& object) const
{
	return OperatorCompare(object) == MO_BASE_COMPARE_EQUAL;
}


/*!
 * \sa Compare
 */
bool moBase::operator != (const moBase& object) const
{
	return OperatorCompare(object) != MO_BASE_COMPARE_EQUAL;
}


/*!
 * \sa Compare
 */
bool moBase::operator < (const moBase& object) const
{
	return OperatorCompare(object) < MO_BASE_COMPARE_EQUAL;
}


/*!
 * \sa Compare
 */
bool moBase::operator <= (const moBase& object) const
{
	return OperatorCompare(object) <= MO_BASE_COMPARE_EQUAL;
}


/*!
 * \sa Compare
 */
bool moBase::operator > (const moBase& object) const
{
	return OperatorCompare(object) > MO_BASE_COMPARE_EQUAL;
}


/*!
 * \sa Compare
 */
bool moBase::operator >= (const moBase& object) const
{
	return OperatorCompare(object) >= MO_BASE_COMPARE_EQUAL;
}


/** \brief Check a pointer
 *
 * This function does nothing. AnyPtr means it can be anything
 * and thus it does not need to be tested.
 *
 * \param[in] ptr  Ignored.
 */
void Ptr::moAnyPtr::CheckPtr(const moBase *ptr)
{
}

/** \brief Ensure that the pointer is dynamic
 *
 * In some circumstances, you need a pointer to be dynamic
 * (instead of static.) A dynamic pointer is one that has been
 * allocated using the new operator.
 *
 * Note that the pointer of a variable member not defined as a
 * pointer will be viewed as a static pointer.
 *
 * This function uses assert() and aborts if the pointer is
 * not dynamic.
 *
 * \note
 * A null pointer is accepted as dynamic.
 *
 * \param[in] ptr   Check whether it is dynamic.
 */
void Ptr::moDynamicPtr::CheckPtr(const moBase *ptr)
{
	assert(ptr == 0 || ptr->IsDynamicObject());
}


/** \brief Ensure that the pointer is static
 *
 * In some circumstances, you need to know that a pointer is
 * static (instead of dynamic.) A static pointer is one that
 * has not been allocated using the new operator. It is either
 * an object on the stack, in the global scope or a variable
 * member.
 *
 * This function uses assert() and aborts if the pointer is
 * not static.
 *
 * \note
 * A null pointer is accepted as static.
 *
 * \param[in] ptr   Check whether it is static.
 */
void Ptr::moStaticPtr::CheckPtr(const moBase *ptr)
{
	assert(ptr == 0 || !ptr->IsDynamicObject());
}


}
// namespace molib

// vim: ts=8 sw=8 tw=79 syntax=cpp.doxygen

