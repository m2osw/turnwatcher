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
#pragma implementation "mo/mo_module.h"
#endif

#include	"mo/mo_module.h"

#ifndef MO_DIRECTORY_H
#include	"mo/mo_directory.h"
#endif



namespace molib
{



// internally, we need a special load reference count
class moInternalModule : public moModule
{
public:
					moInternalModule(const moWCString& name, lt_dlhandle module);
					~moInternalModule();

	virtual const moWCString&	Name(void) const;

	virtual bool			FindSymbol(const moWCString& name, mo_symbol_ptr_t& pointer);

	virtual int			MakeResident(void) const;
	virtual bool			IsResident(void) const;

	virtual const moWCString&	GetLastError(void) const;

	unsigned long			Load_AddRef(void);
	unsigned long			Load_Release(void);

	virtual compare_t		Compare(const moBase& object) const;

private:
	lt_dlhandle		f_module;
	moWCString		f_name;
	moWCString		f_last_error;
	mo_atomic_word_t	f_load_reference_count;
};

typedef moSmartPtr<moInternalModule>			moInternalModuleSPtr;
typedef moTmplList<moInternalModule, moSortedList>	moSortedListOfInternalModules;



class moInternalModuleManager;
typedef moSmartPtr<moInternalModuleManager>	moInternalModuleManagerSPtr;

class moInternalModuleManager : public moModuleManager
{
public:
					moInternalModuleManager(void);
					~moInternalModuleManager(void);

	virtual moModuleSPtr		Load(const moWCString& name);
	virtual void			Unload(moModule *module);

	virtual bool			LoadModules(const moWCString& pattern);

	virtual const moWCString&	GetLastError(void) const;

private:
	mutable moMutex			f_mutex;	// ensure serialized access
	mutable moSortedListOfInternalModules f_modules; // sorted by names

	moWCString			f_last_error;	// last error trying to load/unload a module
	zbool_t				f_initialized;	// whether the lt_dlinit() succeeded
};




/************************************************************ DOC:

CLASS

	moModuleManager

NAME

	Constructor - initialize the module manager
	Destructor - free resources used by the module manager

	GetModuleManager - get a pointer to the module manager
	Done - close the module manager

SYNOPSIS

	static moModuleManagerSPtr GetModuleManager(void);
	static void Done(void);

	private:
	moModuleManager(void);
	~moModuleManager();

DESCRIPTION

	The module manager is a singleton. You can get a pointer to
	that object using the GetModuleManager. The first time the
	function is being called, the module manager is created.
	That very first call is NOT multi-thread safe. If you plan
	to use multiple threads with the module manager, then you
	most certainly want to call this function once before you
	create your threads.

	The constructor and destructor functions are both private
	because the module manager will be created using the
	GetModuleManager() function and destroyed once all the
	references to the object are severed. The reference within
	the module manager itself is removed when the Done() function
	is called.

	Brief example of usage:

		int main()
		{
			{
				moModuleManagerSPtr mm = moModuleManager::GetModuleManager();
				mm->Load("My Dynamic Library");
			}
			moModuleManager::Done();
		}

SEE ALSO

	moPropBag::Get(), moPropBag::Set(), moPropBag::Delete()

*/
// this is our singleton; note that we can't use a smart pointer since
// a static could be initialized AFTER the GetModuleManager() is called
static moInternalModuleManager *	g_module_manager = 0;
static bool				g_done = false;

moInternalModuleManager::moInternalModuleManager(void)
{
	f_initialized = lt_dlinit() == 0;
	if(!f_initialized) {
		f_last_error = "dl initialization failed";
	}
}

moInternalModuleManager::~moInternalModuleManager()
{
	if(f_initialized) {
		lt_dlexit();
	}
}



moModuleManagerSPtr moModuleManager::GetModuleManager(void)
{
	if(!g_module_manager) {
		assert(!g_done);
		if(g_done) {
			return 0;
		}
		g_module_manager = new moInternalModuleManager;
		g_module_manager->AddRef();
	}

	return g_module_manager;
}

void moModuleManager::Done(void)
{
	if(!g_done) {
		g_done = true;
		g_module_manager->Release();
		g_module_manager = 0;
	}
}





/************************************************************ DOC:

CLASS

	moModuleManager

NAME

	Load - load the named module
	Unload - unload the named module
	LoadModules - load the list of modules which match a pattern

SYNOPSIS

	moModuleSPtr Load(const moWCString& name);
	void Unload(moModule *module);
	bool LoadModules(const moWCString& pattern);

PARAMETERS

	name - the name of the module to be loaded
	pattern - a shell like pattern to specify a list of modules to load

DESCRIPTION

	The Load() function will search for the specified module.
	If it finds it, then it simply AddRef() and return its
	pointer. Otherwise, it will try to load it from disk.

	The Unload() function Release() the module once. If the
	module wasn't loaded more than once, then it is physically
	unloaded from memory.

	Note that trying to Unload() the same module more than once
	will result in an assert(). This is a really bad error.

	The Load() and Unload() functions are thread safe.

	The LoadModules() will use the moDirectory object to read a
	list of filenames. Then it will try to load each one of them
	using the Load() function.

RETURN VALUE

	The Load() function returns a pointer to the module loaded.
	If an error occurs, the Load() function returns a null pointer.

	The LoadModules() returns true when all the files matching the
	pattern were successfully loaded. It will always try to load all
	the modules. If one or more fails, the function returns false.

	If there are no files found which match the pattern, the method
	returns true. This means software which can run without any
	modules can start with no error even when no extension modules
	were installed.

BUGS

	The module manager assumes that you don't use the lt_dlopenext()
	and lt_dlclose() functions outside of this library.

SEE ALSO

	GetModuleManager, moModule class

*/
moModuleSPtr moInternalModuleManager::Load(const moWCString& name)
{
	if(!f_initialized) {
		f_last_error = "the module manager couldn't locate the dynamic loader";
		return 0;
	}

	moLockMutex lock(f_mutex);

	// first we search for the named module
	moInternalModuleSPtr module;
	moInternalModule find(name, 0);
	moList::position_t pos = f_modules.Find(&find);
	if(pos == moList::NO_POSITION) {
		lt_dlhandle mod = lt_dlopenext(name.SavedMBData());
		if(mod == 0) {
			f_last_error = lt_dlerror();
			return 0;
		}
		module = new moInternalModule(name, mod);
		f_modules += *module;
	}
	else {
		module = &f_modules[pos];
	}
	module->Load_AddRef();
	return static_cast<moInternalModule *>(module);
}


void moInternalModuleManager::Unload(moModule *module)
{
	moList::position_t	pos;

	if(module) {
		moLockMutex lock(f_mutex);

		pos = f_modules.Find(module);

		// ERROR: the module can't be found?! (unloading more than once?)
		assert(pos != moList::NO_POSITION);
		if(pos != moList::NO_POSITION) {
			moInternalModule *int_module = dynamic_cast<moInternalModule *>(module);
			if(int_module != 0) if(int_module->Load_Release() == 0) {
				f_modules.Delete(pos);
			}
		}
	}
}


bool moInternalModuleManager::LoadModules(const moWCString& pattern)
{
	bool		success;
	unsigned long	idx, max;

	moDirectory dir(pattern, moDirectory::DIR_FLAG_HIDDEN);

	// NOTE: This function returns true if no files are found!
	//	 This is intended since some software may come
	//	 with no extra module and still function.
	success = true;

	max = dir.Count();

	for(idx = 0; idx < max; idx++) {
		success &= static_cast<bool>(Load(dir.Get(idx)->FullPath()));
	}

	return success;
}



/************************************************************ DOC:

CLASS

	moModuleManager

NAME

	GetLastError - last error when trying to load a module

SYNOPSIS

	const moWCString& GetLastError(void) const;

DESCRIPTION

	The GetLastError() function returns a humain readable string
	which represents the error message that the last command which
	generated an error has returned.

	This error is not cleared. You can call this function multiple
	times and the same string will be returned.

WARNING

	This function returns a reference. If you don't copy the string,
	then calling the Load() function again can change that string.

RETURN VALUE

	A reference to the error string.

BUGS

	When the module manager is used in a multi-thread environment
	the last error can be the error that another thread generated.
	A future version of this object may save the last error on a
	per thread basis.

SEE ALSO

	Load, Unload

*/
const moWCString& moInternalModuleManager::GetLastError(void) const
{
	return f_last_error;
}






/************************************************************ DOC:

CLASS

	moModule

NAME

	Name - returns the name of the moModule object

SYNOPSIS

	const moWCString& Name(void) const;

DESCRIPTION

	The Name() function returns the name of the module as specified
	in the constructor parameter list.

RETURN VALUE

	Name() returns a constant reference to the module name string.

SEE ALSO

	FindSymbol, MakeResident, GetLastError

*/
moInternalModule::moInternalModule(const moWCString& name, lt_dlhandle handle)
{
	f_name = name;
	f_module = handle;
	f_load_reference_count = 0;
}


moInternalModule::~moInternalModule()
{
	if(f_module) {
		lt_dlclose(f_module);
	}
}


const moWCString& moInternalModule::Name(void) const
{
	return f_name;
}





/************************************************************ DOC:

CLASS

	moModule

NAME

	FindSymbol - search for the address of a specified symbol

SYNOPSIS

	bool FindSymbol(const moWCString& name, mo_symbol_ptr_t& pointer) const;

DESCRIPTION

	Search this module for the specified symbol.

	Note that the pointer to the symbol is specified as a parameter
	and the result of the function will be true if it succeeded.

RETURN VALUE

	The FindSymbol() function returns true whenever it finds the
	specified symbol and false otherwise

	If the symbol is not found, then a new error message will be
	available in the module. Use the GetLastError() to retrieve
	it.

SEE ALSO

	MakeResident, GetLastError

*/
bool moInternalModule::FindSymbol(const moWCString& name, mo_symbol_ptr_t& pointer)
{
	pointer = 0;

	if(f_module) {
		lt_dlerror();
		pointer = reinterpret_cast<mo_symbol_ptr_t>(lt_dlsym(f_module, name.SavedMBData()));
		if(pointer == 0) {
			f_last_error = lt_dlerror();
			return f_last_error.IsEmpty();
		}
		return true;
	}

	return false;
}


/************************************************************ DOC:

CLASS

	moModule

NAME

	MakeResident - force the module to stay in memory
	IsResident - whether the module is currently in memory

SYNOPSIS

	int MakeResident(void) const;
	bool IsResident(void) const;

DESCRIPTION

	The MakeResident() function ensures that the module stays in
	memory. This is a good idea especially when the module is being
	used over and over again.

	The IsResident() tells you whether the module currently is in
	memory or not.

RETURN VALUE

	MakeResident() returns 0 when no error occured.

	IsResident() returns true if the module is in memory.

BUGS

	The Macintosh version of ltdl available in fink doesn't
	support residency information.

SEE ALSO

	FindSymbol, GetLastError

*/
int moInternalModule::MakeResident(void) const
{
#ifdef MAC
	return 0;
#else
	return lt_dlmakeresident(f_module);
#endif
}


bool moInternalModule::IsResident(void) const
{
#ifdef MAC
	return false;
#else
	return lt_dlisresident(f_module) != 0;
#endif
}



/************************************************************ DOC:

CLASS

	moModule

NAME

	GetLastError - get a string about the last error

SYNOPSIS

	const moWCString& GetLastError(void) const;

DESCRIPTION

	The different functions in a module object can generate an
	error. The error message can be retrieved with this function.

	Calling this function more than once yields the same result.
	In other words, the error isn't cleared.

RETURN VALUE

	GetLastError() returns a constant reference to the last
	error message produced by one of the module functions.

SEE ALSO

	FindSymbol, MakeResident

*/
const moWCString& moInternalModule::GetLastError(void) const
{
	return f_last_error;
}




/************************************************************ DOC:

CLASS

	moInternalModule

NAME

	Load_AddRef - add one to the reference
	Load_Release - reduce the reference by one

SYNOPSIS

	unsigned long Load_AddRef(void);
	unsigned long Load_Release(void);

DESCRIPTION

	The module object has two reference counters. One counts
	the number of times it was loaded with the Load()
	instruction (see the moModuleManager) and one which is
	used by the smart pointer template.

	A module can be loaded multiple times using the Load()
	instruction. For each time it was loaded, one needs to
	call Unload() [WARNING: if the Done() function of the
	manager is called, then the list of modules are anyway
	released by the manager not doing anything specific
	about the load counter].

RETURN VALUE

	The Load_AddRef() returns the counter after it added
	one to it.

	The Load_Release() returns the counter after it
	subtracted one from it.

NOTES

	This counter is thread safe.

SEE ALSO

	moModuleManager::Load, moModuleManager::Unload

*/
unsigned long moInternalModule::Load_AddRef(void)
{
	return moAtomicAdd(&f_load_reference_count, 1);
}


unsigned long moInternalModule::Load_Release(void)
{
	return moAtomicAdd(&f_load_reference_count, -1);
}





/************************************************************ DOC:

CLASS

	moInternalModule

NAME

	Compare - compare two modules

SYNOPSIS

	virtual compare_t Compare(const moBase& object) const;

DESCRIPTION

	This is used to compare two internal modules to sort
	them in a list.

	Modules are sorted by name. You can't have two modules
	of the same name. Instead, when you try to load a module
	and an entry with that name is found, that entry is
	automatically returned (and it gets its Load() counter
	increment).

RETURN VALUE

	one of the compare_t results

SEE ALSO

	Load_AddRef, Load_Release

*/
moBase::compare_t moInternalModule::Compare(const moBase& object) const
{
	return f_name.Compare(dynamic_cast<const moInternalModule&>(object).f_name);
}











// vim: ts=8
};		// namespace molib

