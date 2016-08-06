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



#ifndef MO_ERROR_H
#define	MO_ERROR_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_BASE_H
#include	"mo_base.h"
#endif

namespace molib
{

// More or less, an moException is the equivalent to std::exception in the mo library
// To see the message, use the MODEBUG environment variable
// Note: we do not derive from std::exception simple because we cannot easily set the
//	 what() message (we want to look into a way to fix that though!)
class MO_DLL_EXPORT moException : public moBase
{
public:
				moException(const moException& err);
	virtual			~moException();

	moException&		operator = (const moException& err);

	int			Errno() const;
	const char *		Message() const;

protected:
	explicit		moException(int err_num);
	explicit		moException(const char *format, ...);
	explicit		moException(int err_num, const char *format, ...);
	void			Init(const char *format, va_list params, bool show = true);

private:
	char *			f_message;
	int			f_errno;
};

// More or less, an moError is similar to an std::runtime_error
class MO_DLL_EXPORT moError : public moException
{
public:
				moError(const char *format, ...);
				moError(int err_num, const char *format, ...);
				moError(const moError& err);
	virtual			~moError();

// You probably didn't realize the side effects of this one, eh Alexis? :-)
// Check out +451 of list.c++ in the Find() method
//protected:
	explicit		moError(int err_num);
};

// More or less, an moBug is similar to an std::logic_error
class MO_DLL_EXPORT moBug : public moException
{
public:
				moBug(const char *format, ...);
				moBug(int err_num, const char *format, ...);
				moBug(const moBug& err);
	virtual			~moBug();
};



/** \brief Return the error number.
 *
 * Returns the error number used while creating this error object.
 *
 * \return This error number.
 *
 * \sa moException::Message() const
 */
inline int moException::Errno() const
{
	return f_errno;
}


/** \brief Return the error message.
 *
 * Returns the error message created within this error object.
 *
 * \bug
 * The error message is a C string that is deleted whenever this
 * object is deleted. It is suggested that you copy this string
 * in an std::string or moWCString if you want to keep it around
 * but not the exception object.
 *
 * \return A C null terminated string pointer.
 *
 * \sa moException::Errno() const
 */
inline const char *moException::Message() const
{
	return f_message;
}




};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_ERROR_H

