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



#ifndef MO_CONFIG_H
#define	MO_CONFIG_H

#undef	MO_CONFIG_NO_IN_PORT_T
#undef	MO_CONFIG_NO_MUTABLE
#undef	MO_CONFIG_NO_BOOL
#undef	MO_CONFIG_NO_CPP_STRING
#undef	MO_CONFIG_NO_DIRECT_STATIC_CONST
#undef	MO_CONFIG_NO_UNISTD
#undef	MO_CONFIG_NO_LONG_LONG
#undef	MO_CONFIG_NO_LONG_DOUBLE
#undef	MO_CONFIG_USE_STRI_FOR_STRCASE
#undef	MO_CONFIG_INCLUDE_PROCESS
#undef	MO_CONFIG_INCLUDE_STRSTREA
#undef	MO_CONFIG_HAS_INT64
#undef	MO_CONFIG_ALWAYS_NEED_RETURN
#undef	MO_CONFIG_USE__PRINTF
#undef	MO_CONFIG_INCLUDE_SYSTIME
#undef	MO_CONFIG_CONST_ICONV
#undef	MO_CONFIG_CTYPE_DEFINE


#ifdef WIN32
#	define	MO_CONFIG_NO_DIRECT_STATIC_CONST	1
#	define	MO_CONFIG_NO_UNISTD					1
#	define	MO_CONFIG_NO_LONG_LONG				1
#	define	MO_CONFIG_USE_STRI_FOR_STRCASE		1
#	define	MO_CONFIG_INCLUDE_PROCESS			1
//#	define	MO_CONFIG_INCLUDE_STRSTREA			1
#	define	MO_CONFIG_HAS_INT64					1
#	define	MO_CONFIG_USE__PRINTF				1
#	define	MO_CONFIG_CONST_ICONV				1
#	define	MO_DISPLAY_MSWINDOWS				1
#	ifdef MO_WIN32
#		pragma warning( disable: 4996 )
#	endif
#endif


#ifdef MAC
#	define	MO_CONFIG_NO_LONG_DOUBLE		1
#	define	MO_CONFIG_CTYPE_DEFINE			1
#	define	MO_CONFIG_CONST_ICONV			1
#endif

#if defined(LINUX) || defined(IRIX)
#	define	MO_CONFIG_INCLUDE_SYSTIME		1
#	define	MO_DISPLAY_X11				1
#endif



#ifdef LINUX
#	include	<linux/version.h>
#	if LINUX_VERSION_CODE <= 131598
#		define	MO_CONFIG_NO_IN_PORT_T			1
#	endif
#endif


#endif		// #ifndef MO_CONFIG_H

