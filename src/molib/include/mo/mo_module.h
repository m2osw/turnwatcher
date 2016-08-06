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



#ifndef MO_MODULE_H
#define	MO_MODULE_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_WCSTRING_H
#include	"mo_string.h"
#endif

#ifdef _MSC_VER
// ltdl.h redefines this value for Visual Studio
#undef R_OK
#endif
#include	<ltdl.h>


namespace molib
{



class MO_DLL_EXPORT moModule : public moBase
{
public:
	typedef	char *		mo_symbol_ptr_t;

	virtual const moWCString&	Name(void) const = 0;

	virtual bool			FindSymbol(const moWCString& name, mo_symbol_ptr_t& pointer) = 0;

	virtual int			MakeResident(void) const = 0;
	virtual bool			IsResident(void) const = 0;

	virtual const moWCString&	GetLastError(void) const = 0;
};

typedef moSmartPtr<moModule>	moModuleSPtr;






class MO_DLL_EXPORT moModuleManager;
typedef moSmartPtr<moModuleManager>	moModuleManagerSPtr;

class MO_DLL_EXPORT moModuleManager : public moBase
{
public:
	static moModuleManagerSPtr	GetModuleManager(void);
	static void			Done(void);

	virtual moModuleSPtr		Load(const moWCString& name) = 0;
	virtual void			Unload(moModule *module) = 0;

	virtual bool			LoadModules(const moWCString& pattern) = 0;

	virtual const moWCString&	GetLastError(void) const = 0;
};






};			// namespace molib

// vim: ts=8 sw=8
#endif		// #ifndef MO_MODULE_H

