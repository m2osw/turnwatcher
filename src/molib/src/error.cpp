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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_error.h"
#endif

#include	"mo/mo_error.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{

namespace
{
	char *show_error;
}


/** \class moException
 *
 * \brief The low level exception class of the mo library.
 *
 * This object is the lowest level exception class defined in
 * the mo library. Any other exception defined in the mo library
 * indirectly derive from this class.
 *
 * It is expected that objects heavily using exceptions for
 * any error derive from the moError exception.
 *
 * If an exception is used to help debugging (as an assert) then
 * directly use moBug, do not derived from that class.
 *
 * Note that errors are not shown in your console by default.
 * In order to get the error message, set the MODEBUG environment
 * variable to ON:
 *
 * \code
 * 	export MODEBUG=ON
 * \endcode
 *
 * \note
 * An moException can be viewed as being equivalent to the
 * std::exception class.
 */


/** \brief Create an exception with no message.
 *
 * This constructor is used by the derived moError, moBug and
 * any other exception that accepts a C like list of arguments
 * (which is why it is protected.)
 *
 * These constructors are expected to call the Init() function
 * with the correct arguments.
 */
moException::moException(int err_num)
	: f_message(0),
	  f_errno(err_num)
{
}


/** \brief Create a new exception with a default error number.
 *
 * This function initializes a new exception object with an
 * error number set to MO_ERROR_UNDEFINED and a message composed
 * of the format and following optional parameters.
 *
 * If it is necessary to specify an error number, use the
 * constructor with an error number. It is not possible to
 * change the error number after the moException object
 * was initialized.
 *
 * Common usage:
 *
 * \code
 * throw new moBug("Cannot access data before it was initialized.");
 * \endcode
 *
 * \param[in] format The error message followed by the necessary parameters.
 */
moException::moException(const char *format, ...)
	: f_message(0),
	  f_errno(MO_ERROR_UNDEFINED)
{
	va_list		params;

	va_start(params, format);
	Init(format, params);
	va_end(params);
}


/** \brief Create a new exception with a valid error number.
 *
 * This function initializes a new exception object with an
 * error number and a message composed of the format and
 * following option parameters.
 *
 * Valid error numbers are defined in mo_base.h. For instance,
 * one can use MO_ERROR_NOT_IMPLEMENTED.
 *
 * Classes that implement their own errors need to start
 * numbering them at MO_ERROR_USER_NUMBER.
 *
 * \code
 * throw new moBug(MO_ERROR_OVERFLOW, "Index too large for current array size.");
 * \endcode
 *
 * \param[in] err_num The error number.
 * \param[in] format The format of the message followed by parameters
 */
moException::moException(int err_num, const char *format, ...)
	: f_message(0),
	  f_errno(err_num)
{
	va_list		params;

	va_start(params, format);
	Init(format, params);
	va_end(params);
}


/** \brief Copy an exception in a newly created exception
 *
 * This function copies the source exception error in this
 * exception error. The duplicate is a 1 to 1.
 *
 * \param[in] exception The source exception to copy.
 */
moException::moException(const moException& exception)
	: moBase(exception),
	  f_message(0),
	  f_errno(exception.f_errno)
{
	size_t l = strlen(exception.f_message) + 1;
	f_message = new char[l];
	memcpy(f_message, exception.f_message, l);
}


/** \brief The assignment operator copies an exception.
 *
 * This operator ensures that the source exception is duplicated
 * properly in the destination.
 *
 * \param[in] exception The source error to copy.
 *
 * \return A reference to this
 */
moException& moException::operator = (const moException& exception)
{
	if(this != &exception) {
		f_errno = exception.f_errno;
		size_t l = strlen(exception.f_message) + 1;
		f_message = new char[l];
		memcpy(f_message, exception.f_message, l);
	}

	return *this;
}


/** \brief Cleans up the exception object.
 *
 * This destructor ensures that the message is deleted
 * from memory.
 */
moException::~moException()
{
	delete [] f_message;
}


/** \brief Initialize the exception object.
 *
 * This function allocates a buffer and saves the message in it.
 *
 * If \p show is set to true, then the resulting message is shown
 * (i.e. print in stderr.) In order to make stdout and stderr
 * mostly consistent, this function calls fflush(stdout) before
 * the fprintf(stderr, message) call.
 *
 * \exception std::bad_alloc can occur if the exception memory
 * buffer cannot be allocated.
 *
 * \param[in] format   The message format string (as in printf()).
 * \param[in] params   The parameters to use with the message.
 * \param[in] show     Whether the show the resulting message when an exception occurs.
 */
void moException::Init(const char *format, va_list params, bool show)
{
	const char	*debug;
	char		buf[1024];	/* Flawfinder: ignore */
	size_t		len;

#ifdef WIN32
/* Flawfinder: ignore */
#define	vsnprintf	_vsnprintf
#endif
	vsnprintf(buf, sizeof(buf) - 1, format, params);	/* Flawfinder: ignore */
	buf[sizeof(buf) - 1] = '\0';	// ensure end is marked
	len = strlen(buf) + 1;		/* Flawfinder: ignore */
	f_message = new char[len];
	memcpy(f_message, buf, len);		/* Flawfinder: ignore */

	if(show) {
		debug = getenv("MODEBUG");	/* Flawfinder: ignore */
		if(debug != 0 && strcasecmp(debug, "ON") == 0) {
			fflush(stdout);
			fprintf(stderr, "moException:%d: %s.\n", f_errno, f_message);
		}
	}
}



/** \class moError
 *
 * \brief Defines an exception that should occur on system errors.
 *
 * In general, an moError object is thrown whenever an event occurs
 * that cannot be answered with a usual error code being returned
 * (i.e. EOF, duplicated element not inserted in a list, etc.)
 *
 * For instance, the moFile() constructor, when required to open a
 * file, will throw if the open fails. This is not considered a bug
 * since a missing file can be fixed and the software re-run and
 * work.
 *
 * \todo
 * We need to add a callback capability so we get a UI function
 * called that opens a window. This way, in a UI application
 * it will be possible to display an error window.
 *
 * \note
 * An moError can be viewed as being equivalent to the
 * std::runtime_error class.
 */


/** \brief Initializer defined so one can derive from the moError class.
 *
 * This constructor is defined so you can derive from the moError class
 * and call the moException::Init() function with the message once
 * properly defined.
 *
 * \param[in] err_num The error number of this exception.
 */
moError::moError(int err_num)
	: moException(err_num)
{
}


/** \brief Initializes an moError exception.
 *
 * This constructor defines an moError with the default
 * error number: MO_ERROR_UNDEFINED.
 *
 * \param[in] format The message string format followed by the necessary arguments.
 */
moError::moError(const char *format, ...)
	: moException(MO_ERROR_UNDEFINED)
{
	va_list		params;

	va_start(params, format);
	Init(format, params);
	va_end(params);
}


/** \brief Initializes an moError exception.
 *
 * This constructor defines an moError with the specified
 * error number and format string.
 *
 * \param[in] err_num The error number for this error.
 * \param[in] format The message string format followed by the necessary arguments.
 */
moError::moError(int err_num, const char *format, ...)
	: moException(err_num)
{
	va_list		params;

	va_start(params, format);
	Init(format, params);
	va_end(params);
}


/** \brief Initializes an moError exception.
 *
 * This constructor creates a clone of the input error.
 *
 * \param[in] err The error to copy.
 */
moError::moError(const moError& err)
	: moException(err)
{
}



/** \brief Destroys the moError object.
 *
 * This destructor ensures that the moError object is cleaned up.
 */
moError::~moError()
{
}





/** \class moBug
 *
 * \brief Defines an exception that should never occur.
 *
 * Whenever this exception occurs, your software has a bug.
 *
 * By default, all of these exception generate an assert and
 * show the error message on the screen.
 *
 * \todo
 * We need to add a callback capability so we get a UI function
 * called that will open a window. This way, in a UI application
 * it will be possible to display the bug in the error window.
 *
 * \note
 * An moBug can be viewed as being equivalent to the
 * std::logic_error class.
 */


/** \brief Initializes an moBug exception.
 *
 * This constructor defines an moBug with the default
 * error number: MO_ERROR_UNDEFINED.
 *
 * \param[in] format The message string format followed by the necessary arguments.
 */
moBug::moBug(const char *format, ...)
	: moException(MO_ERROR_UNDEFINED)
{
	va_list		params;

	// force the message to be displayed
	if(show_error == 0) {
		show_error = new char[11];
		strcpy(show_error, "MODEBUG=ON");
	}
	putenv(show_error);
	va_start(params, format);
	Init(format, params);
	va_end(params);

	assert(0);
}


/** \brief Initializes an moBug exception.
 *
 * This constructor defines an moBug with the specified
 * error number and format string.
 *
 * \param[in] err_num The error number for this error.
 * \param[in] format The message string format followed by the necessary arguments.
 */
moBug::moBug(int err_num, const char *format, ...)
	: moException(err_num)
{
	va_list		params;

	// force the message to be displayed
	if(show_error == 0) {
		show_error = new char[11];
		strcpy(show_error, "MODEBUG=ON");
	}
	putenv(show_error);
	va_start(params, format);
	Init(format, params);
	va_end(params);

	assert(0);
}


/** \brief Initializes an moBug exception.
 *
 * This constructor creates a clone of the input error.
 *
 * \param[in] err The error to copy.
 */
moBug::moBug(const moBug& err)
	: moException(err)
{
}



/** \brief Destroys the moBug object.
 *
 * This destructor ensures that the moBug object is cleaned up.
 */
moBug::~moBug()
{
}


};			// namespace molib;

// vim: ts=8

