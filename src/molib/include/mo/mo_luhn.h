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



#ifndef MO_LUHN_H
#define	MO_LUHN_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STRING_H
#include	"mo_string.h"
#endif


namespace molib
{

namespace luhn
{

enum {
	ERROR_NOT_A_DIGIT = -1,
	ERROR_TOO_SHORT = -2,
	ERROR_TOO_LONG = -3
};

MO_DLL_EXPORT_FUNC bool moIsValid(const moWCString& number);
MO_DLL_EXPORT_FUNC int  moSum(const moWCString& number);

};			// namespace luhn;

};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_LUHN_H

