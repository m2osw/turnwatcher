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



#pragma once

#if 0
#if defined(_MSC_VER) && defined(_DLL)
#	define MO_EXPORT	__declspec(dllexport)
#	define MO_IMPORT	__declspec(dllimport)
#else
#	define MO_EXPORT
#	define MO_IMPORT
#endif

#define MO_EXPORT_FUNC	MO_EXPORT
#define MO_IMPORT_FUNC

#ifdef MO_EXPORTS
#    define MO_DLL_EXPORT	MO_EXPORT
#    define MO_DLL_EXPORT_FUNC	MO_EXPORT_FUNC
#else
#    define MO_DLL_EXPORT	MO_IMPORT
#    define MO_DLL_EXPORT_FUNC	MO_IMPORT_FUNC
#endif
// nothing for template classes, we ignore warning 4251 and voila
#define MO_DLL_EXPORT_TMPL

#else

#define MO_DLL_EXPORT
#define MO_DLL_EXPORT_FUNC
#define MO_DLL_EXPORT_TMPL

#endif


// vim: ts=8 sw=8

