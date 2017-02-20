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



#ifndef MO_MEMFILE_H
#define	MO_MEMFILE_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STREAM_H
#include	"mo_stream.h"
#endif

#ifndef MO_BUFFER_H
#include	"mo_buffer.h"
#endif


namespace molib
{


class MO_DLL_EXPORT moMemFile : public moIOStream
{
public:
				moMemFile(void);
	virtual			~moMemFile();

private:
	virtual size_t		InputSize(void) const;
	virtual size_t		OutputSize(void) const;
	virtual int		RawRead(void *buffer, size_t length);
	virtual int		RawWrite(const void *buffer, size_t length);

	moBuffer		f_buffer;
};

typedef moDualSmartPtr<moMemFile, moIStream>	moMemFileSPtr;





};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_MEMFILE_H

