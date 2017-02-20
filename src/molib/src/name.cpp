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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_name.h"
#endif

#include	"mo/mo_name.h"

namespace molib
{


const moWCString		moNamePool::g_empty_name;


/************************************************************ DOC:

CLASS

	moNamePool::moUniqueName

NAME

	Constructor - initialize the name pool
	operator mo_name_t - return this name number
	CompareNumbers - for the numbers list

SYNOPSIS

	private:
	moUniqueName(mo_name_t name);
	moUniqueName(const char *name, mo_name_t number);
	moUniqueName(const moUniqueName& unique_name);
	operator mo_name_t (void) const { return f_number; }
	virtual compare_t CompareNumbers(const moBase& object) const;

PARAMETERS

	name - a name in the form of a string or a number
	unique_name - an moUniqueName to copy
	object - an moUniqueName to compare with 'this'

DESCRIPTION

	This private class is used by the moNamePool class to sort
	the names and assign them a unique number.

	An moUniqueName object can be created with a number, a name
	and number and another moUniqueName object.

	The constructor using just a number is used to create an
	instance to search for a name in the f_numbers list.

	The constructor using a name and number is used to create
	a valid instance to search for a name in the f_names list
	and to create new moUniqueName objects.

	The constructor using another moUniqueName object to create
	the new moUniqueName object can be used to create new objects
	to add to the f_names and f_numbers lists.

	The mo_name_t operator is just to ease the getting of the
	unique number used to represent that name.

	The CompareNumbers() function is used to sort the list of
	moUniqueName by number (f_numbers.)

RETURN VALUE

	The mo_name_t operator returns the name number.

	The Compare() function returns one of the MO_BASE_COMPARE_...

SEE ALSO

	moNamePool class

BUGS

	These functions are not multi-thread safe.

	The constructor has to be called before any thread (which
	would make use of the NamePool) was created.

	The destructor should be called after all the threads
	were terminated (we may still make the destructor
	thread safe at some point, but it should destroy the
	mutex...)

*/
moBase::compare_t moNamePool::moUniqueName::CompareNumbers(const moBase& object) const
{
	const moUniqueName& name = dynamic_cast<const moUniqueName&>(object);

// the following should never happen
	assert(IsName(f_number) && IsName(name.f_number));
	if(!IsName(f_number) || !IsName(name.f_number)) {
		return MO_BASE_COMPARE_UNORDERED;
	}

// compare the number, not the name
	return CompareInt(f_number, name.f_number);
}


/************************************************************ DOC:

CLASS

	moNamePool

NAME

	Constructor - initialize the name pool
	Destructor - deletes all the resources used by the name pool

SYNOPSIS

	moNamePool(void);
	~moNamePool();

DESCRIPTION

	The name pool will be used to generate unique numbers for
	names. This is similar to X11 atoms. At this time there is
	no default names in the pool.

	The mo_name_t numbers will always be non zero. Thus, one can
	use 0 as a null name. The names will always have bit 30 set.
	A name with bit 31 set represents an errorneous call or
	result (i.e. negative values.) The values with bit 30 and 31
	reset (set to 0) are reserved for the user (usually these
	represent an index.) The different type of names can be
	tested with the Is<test>() functions.

	Since the allocation of numbers for names is dynamic, the
	same name in two sessions may be assigned different values.

	In order to get the name pool of your process, use the
	GetNamePool() function. This function will create the pool
	for you if it doesn't exist yet.

	Once you are done with the name pool, call Done() and the
	pool will be deleted. This is important if you want to
	check for memory leaks, useless otherwise (you certainly
	only want to do the delete in debug mode.)

SEE ALSO

	GetNamePool, Done

BUGS

	These functions are not multi-thread safe.

	The constructor has to be called before any thread (which
	would make use of the NamePool) was created.

	The destructor should be called after all the threads
	were terminated (we may still make the destructor
	thread safe at some point, but it should destroy the
	mutex...)

*/
moNamePool::moNamePool(void)
{
	f_numbers.SetCompare(reinterpret_cast<moBase::compare_function_t>(&moUniqueName::CompareNumbers));
}


moNamePool::~moNamePool()
{
}


/************************************************************ DOC:

CLASS

	moNamePool

NAME

	GetNamePool - get the name pool singleton
	Done - tell the pool we won't use it anymore

SYNOPSIS

	static moNamePool *GetNamePool(void);
	static void Done(void);

DESCRIPTION

	The GetNamePool() static function gives you access to the
	name pool singleton. You can't yourself create a name pool
	nor destroy the existing name pool.

	A name pool associate a name (a string) and a number (mo_name_t
	which is an int). The object will be used to dynamically
	create names for internal use purposes.

	Once you are sure you won't call the pool functions anymore,
	you can call the Done() function which will take care of
	clearing up all the resources.

RETURN VALUE

	The GetNamePool() function returns a reference to the
	global name pool.

SEE ALSO

	GetNumber, GetName
	operator []

BUGS

	This function is not multi-thread safe when first called. We
	should anyway not create threads in global constructors.

*/
moNamePoolSPtr		moNamePool::g_name_pool;
bool			moNamePool::g_done = false;

const moNamePool& moNamePool::GetNamePool(void)
{
	if(!g_name_pool) {
		assert(!g_done);
		g_name_pool = new moNamePool;
	}
	return static_cast<moNamePool&>(g_name_pool);
}


void moNamePool::Done(void)
{
	g_name_pool = 0;
	g_done = true;
}



/************************************************************ DOC:

CLASS

	moNamePool

NAME

	Get - get the name or number of a name in the pool
	operator [] - get the name or number of a name in the pool

SYNOPSIS

	mo_name_t Get(const moWCString& name) const;
	mo_name_t operator [] (const moWCString& name) const;
	const moWCString& Get(mo_name_t no) const;
	const moWCString& operator [] (mo_name_t no) const;

PARAMETERS

	name - the name to get the number of
	no - the number to get the name of

DESCRIPTION

	The Get() function with a name transforms the name in a
	number. Each name is always given a unique number. Retrieving
	the number of the same name multiple times will always yield
	the same result in a process instance, however, another or
	restarting the same process can yield different numbers for
	the same name.

	The operator [] with a string also returns a unique number.

	Note that the string can't be empty. In debug mode, this will
	generate an assert. In release mode, the function returns -1.

	The Get() function with a number returns the name corresponding
	to that number. If the number can't be found in the list, then
	an empty string is returned.

	The operator [] with a number is like the Get() with a number
	and it returns a string with the corresponding name.

RETURN VALUE

	Get(string) and operator [] (string) both return an mo_name_t
	which is garantied to not be 0. If the string is empty, this
	function returns -1 (which is an invalid name number.)

	Get(name) and operator [] (name) both return an moWCString
	which is empty when the given name number is not a valid
	registred name.

SEE ALSO

	GetNamePool, Done

*/
mo_name_t moNamePool::Get(const moWCString& name) const
{
	assert(!g_done);
	assert(!name.IsEmpty());

	if(name.IsEmpty()) {
		return -1;
	}

	moList::position_t	pos;
	moUniqueName		*prop_name;

	moLockMutex		lock(f_mutex);

	moUniqueName		n(name, f_names.Count() + ((1 << 30) + 1));

	pos = f_names.Find(&n);
	if(pos == moList::NO_POSITION) {
		prop_name = new moUniqueName(n);
		f_names += *prop_name;
		f_numbers += *prop_name;
	}
	else {
		prop_name = dynamic_cast<moUniqueName *>(f_names.Get(pos));
	}

	return *prop_name;
}

const moWCString& moNamePool::Get(mo_name_t name) const
{
	assert(!g_done);

	moList::position_t	pos;
	moUniqueName		n(name);

	moLockMutex		lock(f_mutex);

	pos = f_numbers.Find(&n);
	if(pos == moList::NO_POSITION) {
		return g_empty_name;
	}

	return *dynamic_cast<moWCString *>(f_numbers.Get(pos));
}



/************************************************************ DOC:

CLASS

	moNamePool

NAME

	IsNull -- is this name the null name?
	IsName -- is this a valid mo_name_t?
	IsError -- is this an invalid mo_name_t representing an error?
	IsUser -- is this a user value?

SYNOPSIS

	static bool IsNull(mo_name_t name);
	static bool IsName(mo_name_t name);
	static bool IsError(mo_name_t name);
	static bool IsUser(mo_name_t name);

DESCRIPTION

	The mo_name_t variable type has different types which can be
	checked using these functions. The types are defined with the
	following table:

		0				Null & User
		1 to 0x3FFFFFFF			User
		0x40000000 to 0x7FFFFFFF	Name
		0x80000000 to 0xFFFFFFFF	Error

	This is used by the property set so the operator [] can
	distinguish between a name and an index with just a simple
	call such as IsName(name_or_index).

NOTE

	These functions are static so one doesn't need to get the name
	pool to get test a name.

RETURN VALUE

	All the functions return true or false; it returns true when
	the value is in the specified range.

SEE ALSO

	GetNamePool, Done

*/






}			// namespace molib;

// vim: ts=8

