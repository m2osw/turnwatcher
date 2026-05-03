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



#ifndef MO_NAME_H
#define	MO_NAME_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_LIST_H
#include	"mo_list.h"
#endif
#ifndef MO_WCSTRING_H
#include	"mo_string.h"
#endif
#ifndef MO_MUTEX_H
#include	"mo_mutex.h"
#endif




namespace molib
{


// a valid name from the property name pool has its
// bit 30 set to 1. (0x40000000 + value)
// an error has bit 31 set (i.e. a negative value)
// when neither bit 30 nor 31 are set, then it is an index
//
// WARNING: use the moNamePool::Is<func>() functions to
// check for the different types (IsNull, IsName, IsError, IsUser)
typedef rint32_t		mo_name_t;

class MO_DLL_EXPORT moNamePool;
typedef moSmartPtr<moNamePool>	moNamePoolSPtr;

class MO_DLL_EXPORT moNamePool : public moBase
{
public:
	static const moNamePool& GetNamePool(void);
	static void		Done(void);

	// find a named number (this creates a new entry when the name can't be found)
	mo_name_t		Get(const moWCString& name) const;
	mo_name_t               operator [] (const moWCString& name)  const { return Get(name);             }
	mo_name_t               operator [] (const char        *name) const { return Get(moWCString(name)); }
	mo_name_t               operator [] (const wchar_t     *name) const { return Get(moWCString(name)); }

	// find a name given a number (this can fail)
	const moWCString&	Get(mo_name_t name) const;
	const moWCString&	operator [] (mo_name_t name) const { return Get(name); }

	static bool		IsNull(mo_name_t name)  { return name == 0; }
	static bool		IsName(mo_name_t name)  { return (name & (1 << 30)) != 0; }
	static bool		IsError(mo_name_t name) { return name < 0; }
	static bool		IsUser(mo_name_t name)  { return (name & (3 << 30)) == 0; }

private:
	// prevent users from creating/destroying this object
	moNamePool(void);
	~moNamePool();

	class moUniqueName : public moWCString
{
	public:
		moUniqueName(mo_name_t name) : f_number(name) {}
		moUniqueName(const moWCString& name, mo_name_t number)
			: moWCString(name), f_number(number) {}
		moUniqueName(const char *name, mo_name_t number)
			: moWCString(name), f_number(number) {}
		moUniqueName(const wchar_t *name, mo_name_t number)
			: moWCString(name), f_number(number) {}
		moUniqueName(const moUniqueName& unique_name)
			: moWCString(unique_name), f_number(unique_name) {}

		operator mo_name_t (void) const { return f_number; }

		virtual compare_t	CompareNumbers(const moBase& object) const;

	private:
		mo_name_t		f_number;
};

	mutable moMutex		f_mutex;	// ensure serialized access
	mutable moSortedList	f_names;	// sorted by names
	mutable moSortedList	f_numbers;	// sorted by numbers

	// we have a singleton
	static moNamePoolSPtr	g_name_pool;
	static bool		g_done;
	static const moWCString	g_empty_name;	// when requesting an alphanumeric name of a non-existant name
};



template<class T, bool do_assert>
class MO_DLL_EXPORT moNameBase
{
public:
	moNameBase(const moNameBase& name)  { f_name =  name.f_name;                     }
	moNameBase(const T           name)  { assert(!do_assert || moNamePool::IsName(name));       f_name = name; }
	moNameBase(const moWCString& name)  { f_name =  moNamePool::GetNamePool()[name]; }
	moNameBase(const QString&    name)  { f_name =  moNamePool::GetNamePool()[name]; }
	moNameBase(const char        *name) { f_name =  moNamePool::GetNamePool()[name]; }
	moNameBase(const wchar_t     *name) { f_name =  moNamePool::GetNamePool()[name]; }

	moNameBase& operator = (const moNameBase& name)  { f_name = name.f_name;                     return *this; }
	moNameBase& operator = (const T           name)  { assert(!do_assert || moNamePool::IsName(name));       f_name =      name; return *this; }
	moNameBase& operator = (const moWCString& name)  { f_name = moNamePool::GetNamePool()[name]; return *this; }
	moNameBase& operator = (const QString&    name)  { f_name = moNamePool::GetNamePool()[name]; return *this; }
	moNameBase& operator = (const char        *name) { f_name = moNamePool::GetNamePool()[name]; return *this; }
	moNameBase& operator = (const wchar_t     *name) { f_name = moNamePool::GetNamePool()[name]; return *this; }

	operator T (void) const                 { return f_name; }
	operator T (void)                       { return f_name; }
	operator const moWCString& (void) const { return moNamePool::GetNamePool()[f_name]; }

private:
	T       f_name;
};



typedef moNameBase<mo_name_t, true>	moName;
typedef moNameBase<int, false>		moIndexOrName;




};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_NAME_H

