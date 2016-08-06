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




//
// File:	include/mo/mo_direct.h
// Object:	
//
// Copyright:	Copyright (c) 2000-2008 Made to Order Software Corp.
//		All Rights Reserved.
//
//		This software and its associated documentation contains
//		proprietary, confidential and trade secret information
//		of Made to Order Software Corp. and except as provided by
//		written agreement with Made to Order Software Corp.
//
//		a) no part may be disclosed, distributed, reproduced,
//		   transmitted, transcribed, stored in a retrieval system,
//		   adapted or translated in any form or by any means
//		   electronic, mechanical, magnetic, optical, chemical,
//		   manual or otherwise,
//
//		and
//
//		b) the recipient is not entitled to discover through reverse
//		   engineering or reverse compiling or other such techniques
//		   or processes the trade secrets contained therein or in the
//		   documentation.
//

#ifndef MO_DIRENT_H
#define	MO_DIRENT_H

#include	<sys/stat.h>
#include	<dirent.h>

#ifdef MO_WIN32
#	include	<direct.h>
#   define getcwd _getcwd

#   include <io.h>
#   define isatty _isatty
#   define access _access
#   define open _open

#   undef S_ISDIR
#   undef S_ISFIFO
#   undef S_ISCHR
#   undef S_ISBLK
#   undef S_ISREG
#endif

#ifndef S_ISDIR
#	define	S_ISDIR(m)	(((m) & _S_IFMT) == _S_IFDIR)
#endif
#ifndef S_ISFIFO
#	define	S_ISFIFO(m)	(((m) & _S_IFMT) == _S_IFIFO)
#endif
#ifndef S_ISCHR
#	define	S_ISCHR(m)	(((m) & _S_IFMT) == _S_IFCHR)
#endif
//
// ISBLK makes no sense under win32...
//
#ifndef S_ISBLK
#	define	S_ISBLK(m)	(((m) & _S_IFMT) == _S_IFBLK)
#endif
#ifndef S_ISREG
#	define	S_ISREG(m)	(((m) & _S_IFMT) == _S_IFREG)
#endif

#endif //MO_DIRENT_H

// vim: ts=8 sw=8

