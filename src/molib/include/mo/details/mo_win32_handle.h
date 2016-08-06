//
// File:	include/details/mo_win32_handle.h
// Object:	RAII handling of Win32 handles
// Copyright:	Copyright (c) 2007 Made to Order Software Corp.
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
#ifndef MO_WIN32_HANDLE_H
#define	MO_WIN32_HANDLE_H
// Note: you are welcome to include this file from the outside.
// Yet, it is expected that all the Win32 handles be managed
// internally and thus you should never have to include this
// yourself.

#include	<windows.h>





namespace molib
{


namespace details
{


template<typename NULL_HANDLE, typename CLOSE_HANDLE>
class MO_DLL_EXPORT moWin32Handle : NULL_HANDLE, CLOSE_HANDLE
{
public:
	typedef typename NULL_HANDLE::handle_t	handle_t;

			moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>()
				: f_handle(NULL_HANDLE::get_null())
			{
			}

			moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>(handle_t handle)
				: f_handle(handle)
			{
			}

			moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>(moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>& handle)
				: f_handle(NULL_HANDLE::get_null())
			{
				reset(handle);
			}

			~moWin32Handle()
			{
				reset(NULL_HANDLE::get_null());
			}

	moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>& operator = (const handle_t handle)
			{
				reset(handle);
				return *this;
			}

	moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>& operator = (moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>& handle)
			{
				reset(handle);
				return *this;
			}

	handle_t	get() const
			{
				return f_handle;
			}

			operator const handle_t () const
			{
				return f_handle;
			}

			operator handle_t ()
			{
				return f_handle;
			}

	void		reset(handle_t handle = NULL_HANDLE::get_null())
			{
				if(f_handle != NULL_HANDLE::get_null()) {
					CLOSE_HANDLE::reset(f_handle);
				}
				f_handle = handle;
			}

	void		reset(moWin32Handle<NULL_HANDLE, CLOSE_HANDLE>& handle)
			{
				if(f_handle != NULL_HANDLE::get_null()) {
					CLOSE_HANDLE::reset(f_handle);
				}
				f_handle = handle.f_handle;
				handle.f_handle = NULL_HANDLE::get_null();
			}

	bool		operator ! () const
			{
				return f_handle == NULL_HANDLE::get_null();
			}

			operator bool () const
			{
				return f_handle != NULL_HANDLE::get_null();
			}

private:
	handle_t	f_handle;
};



template<typename T>
class moWin32NullHandle
{
protected:
	// NULL handle (i.e. 0)
	typedef T handle_t;

	static handle_t get_null()
	{
		return reinterpret_cast<handle_t>(0);
	}
};


template<typename T>
class moWin32InvalidHandle
{
protected:
	// INVALID_HANDLE_VALUE handle (i.e. -1)
	typedef T handle_t;

	static handle_t get_null()
	{
		return reinterpret_cast<handle_t>(-1);
	}
};


template<typename T>
class moWin32CloseHandle
{
protected:
	typedef T handle_t;

	void reset(handle_t handle)
	{
		// we know here that handle is NOT null
		CloseHandle(handle);
	}
};



}


typedef details::moWin32Handle<details::moWin32NullHandle<HANDLE>, details::moWin32CloseHandle<HANDLE> >	mo_win32_null_handle_t;
typedef details::moWin32Handle<details::moWin32InvalidHandle<HANDLE>, details::moWin32CloseHandle<HANDLE> >	mo_win32_invalid_handle_t;

}



#endif		// #ifndef MO_WIN32_HANDLE_H
