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



#ifndef MO_UUENCODE_H
#define	MO_UUENCODE_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_WCSTRING_H
#include	"mo_string.h"
#endif
#ifndef MO_BUFFER_H
#include	"mo_buffer.h"
#endif


namespace molib
{


enum mo_uudecode_error_t {
	MO_UUDECODE_ERROR_NONE = 0,
	MO_UUDECODE_ERROR_NOBEGIN,
	MO_UUDECODE_ERROR_BADMODE,
	MO_UUDECODE_ERROR_BADNAME,
	MO_UUDECODE_ERROR_EMPTY,
	MO_UUDECODE_ERROR_BADLENGTH,
	MO_UUDECODE_ERROR_BADCHAR,
	MO_UUDECODE_ERROR_BADEND
};

MO_DLL_EXPORT_FUNC void moUUEncode(moWCString& out, const moBuffer& in, int mode, const moWCString& name);
MO_DLL_EXPORT_FUNC mo_uudecode_error_t moUUDecode(const moWCString& in, moBuffer& out, int& mode, moWCString& name);




};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_UUENCODE_H

