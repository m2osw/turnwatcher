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



#ifndef MO_BUFFER_H
#define	MO_BUFFER_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_BASE_H
#include	"mo_base.h"
#endif


namespace molib
{


// NOTE: a buffer needs to be a base object so it can be in lists
//	 such as a property bag's list
class MO_DLL_EXPORT moBuffer : public moBase
{
public:
				moBuffer(void *data = 0, unsigned long size = 0, unsigned long max = 0);
				moBuffer(const moBuffer& buffer);
	virtual			~moBuffer();

	void			Set(const moBuffer& buffer);
	void			Set(void *data = 0, unsigned long size = 0, unsigned long max = 0);
	void			SetSize(unsigned long size, bool clear = false);
	unsigned long		GetSize(void) const;
	void			Empty(void);
	bool			IsEmpty(void) const;
	void			Copy(unsigned long pos, const void *data, unsigned long size);
	void			Append(const void *data, unsigned long size);
	void			Insert(unsigned long pos, const void *data, unsigned long size);
	void			Fill(unsigned long pos, unsigned long length, const void *data, unsigned long size);
	void			Get(void *&data, unsigned long& size) const;
	template<typename T>
	void			GetPointer(T& data, unsigned long& size)
				{
					data = reinterpret_cast<T>(f_data);
					size = f_size;
				}

	moBuffer&		operator = (const moBuffer& buffer);
				operator void * (void) const;
				operator const void * (void) const;
				operator unsigned int (void) const;
				operator signed int (void) const;
				operator unsigned long (void) const;
				operator signed long (void) const;

private:
	unsigned long		f_max;		// actual (real) size of the data buffer
	unsigned long		f_size;		// current size of the valid data
	char *			f_data;		// pointer to the data buffer
};




};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_BUFFER_H

