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
#pragma implementation "mo/mo_text_stream.h"
#endif

#include	"mo/mo_text_stream.h"

#ifndef MO_FILE_H
#include	"mo/mo_file.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{

/************************************************************ DOC:

CLASS

	moTextStream

NAME

	Contructors - create a new file object
	Destructor - gets rid of different resources

SYNOPSIS

	moTextStream(moIStream *input = 0, moOStream *output = 0, moOStream *error = 0);
	~moTextStream();

PARAMETERS

	input - the input stream
	output - the output stream
	error - the error stream

DESCRIPTION

	The text file object can be used to read, write, update, create
	text files.

	When the input and output streams are not defined yet, then the
	object has no effect. It can be changed at any time with the
	use of the SetInput() and SetOutput() functions.

	The input and output can be the same IO stream in which case
	reading and writing into it may affect the other.

	The error stream should always be another stream.

	All the streams can come with a filter to support all sorts of
	encodings (such as the mowc::moIConv). The filter should be
	installed before any moTextStream function are called (beside
	the constructor). Note that the GetC() always reads the input
	file as a string of long words (it expects characters defined
	as 4 bytes in internal order).

	When no filename is specified, the previously defined name is
	removed and the name of the input stream is used instead.

BUGS

	If no filename is specified with this function and the
	input stream has no name defined, then the output stream
	filename is queried. It is thus a good idea to always put
	a name, be it "<no name>" for the input stream.

SEE ALSO

	moStream

*/
moTextStream::moTextStream(moIStream *input, moOStream *output, moOStream *error)
{
	f_input = input;
	f_output = output;
	f_error = error;
	//f_std_error = ...	-- auto-init

	f_no_message = false;
	f_no_warning = false;
	f_warnings_to_errors = false;
	//f_warnings_off = ...  -- auto init
	f_warcnt = 0;
	f_errcnt = 0;
	f_last_error = 0;
	//f_filename = ...	-- auto init
	f_line = 0;
	//f_last_line = ...     -- auto init
	f_clean_nl = false;
	f_last_c = EOF;
}


moTextStream::~moTextStream()
{
}


/************************************************************ DOC:

CLASS

	moTextStream

NAME

	SetInput - change the input stream and filename
	SetOutput - change the output stream
	SetError - set the error stream

	Input - get the current input stream
	Output - get the current output stream
	Error - get the current error stream

SYNOPSIS

	void SetInput(moIStream *input, const moWCString *filename = 0);
	void SetOutput(moOStream *output);
	void SetError(moOStream *error);

	moIStream *Input(void) const;
	moOStream *Output(void) const;
	moOStream *Error(void) const;

PARAMETERS

	input - the input stream
	output - the output stream
	error - the error stream
	filename - name (of a file) written whenever an error occurs

DESCRIPTION

	These functions will be used to change the streams at any time.

	Note that changing the input stream forces the line number
	back to zero and clears the error & warning counters, the
	last error number, the last line read and the filename.

	When no filename is specified, the previously defined name is
	removed and the name of the input stream is used instead.

	The SetOutput() function changes the stream used to send data
	written with the different Print() & Repeat() functions.

	The SetError() function changes the stream used to send
	the errors written by the Warning() and Error() functions.

	All the Set...() functions generate a call to the OnNewStream()
	virtual function with the corresponding stream enum value:

		MO_WHICH_INPUT		SetInput()
		MO_WHICH_OUTPUT		SetOutput()
		MO_WHICH_ERROR		SetError()

	If you derive from this object you can create this OnNewStream()
	function and it will automatically be called whenever the user
	changes any one of the streams. Note that if you derive from
	an object which itself is derived from an moTextStream, make
	sure you call its function to ensure proper functionality.

	The Input(), Output() and Error() functions return the
	pointers to the corresponding streams.

BUGS

	If no filename is specified with the SetInput() function and the
	input stream has no name defined, then the output stream
	filename is queried. It is thus a good idea to always put
	a name, be it "<no name>" for the input stream.

RETURN VALUE

	Input() - return the last SetInput() stream pointer
	Output() - return the last SetOutput() stream pointer
	Error() - return the last SetError() stream pointer

SEE ALSO

	WarningCount, ErrorCount, Line, SetLine, SetFilename

*/
void moTextStream::SetInput(moIStream *input, const moWCString *filename)
{
	if(input) {
		f_errcnt = 0;
		f_warcnt = 0;
	}
	f_line = 0;
	f_last_line.Empty();

	f_input = input;

	if(filename != 0) {
		f_filename = *filename;
	}
	else {
		f_filename.Empty();
	}

	OnNewStream(MO_WHICH_INPUT);
}


void moTextStream::OnNewStream(int which)
{
// don't do anything by default
}


moIStreamSPtr moTextStream::Input(void) const
{
	return f_input;
}


void moTextStream::SetOutput(moOStream *output)
{
	f_output = output;

	OnNewStream(MO_WHICH_OUTPUT);
}


moOStreamSPtr moTextStream::Output(void) const
{
	return f_output;
}


void moTextStream::SetError(moOStream *error)
{
	f_error = error;

	OnNewStream(MO_WHICH_ERROR);
}


moOStreamSPtr moTextStream::Error(void) const
{
	return f_error;
}



/************************************************************ DOC:

CLASS

	moTextStream

NAME

	Line - return the line number of the last line read
	SetLine - change the line number of the last line read

SYNOPSIS

	long Line(void) const;
	long SetLine(long line);
	void SetFilename(const moWCString& filename);

PARAMETERS

	line - the new line number (useful when a #line <no> is encountered)
	filename - the filename for errors & warnings

DESCRIPTION

	The Line() function will be used any time the line number
	of the current line is necessary.

	The line number is set to zero when the file is opened or
	closed. A line number of zero should not be printed out.

	The line is increased whenever a line is read (see
	NextLine).

	It is possible to change the line number using the
	SetLine() function. This is usually done when a
	ReadPosition() is also called so the line number
	correspond to the new current file pointer.

	The SetFilename() function will be used to change the
	name to be displayed whenever an error or a warning is
	being displayed.

RETURN VALUE

	Line() returns the current line number

	SetLine() returns the line number which was defined
	before being changed

SEE ALSO

	ErrorCount, NextLine, ReadPosition

*/
long moTextStream::Line(void) const
{
	return f_line;
}


long moTextStream::SetLine(long line)
{
	long	old_line;

	old_line = f_line;
	f_line = line;

	return old_line;
}


void moTextStream::SetFilename(const moWCString& filename)
{
	f_filename = filename;
}



/************************************************************ DOC:

CLASS

	moTextStream

NAME

	WarningCount - return the number of warnings which happened so far
	ErrorCount - return the number of errors which happened so far
	LastError - return the last error code

SYNOPSIS

	long WarningCount(void) const;
	long ErrorCount(void) const;
	errcode_t LastError(void) const;

DESCRIPTION

	The WarningCount() function returns the number of times the
	Warning() function was called.

	The ErrorCount() function returns the number of times the
	Error() function was called.

	The LastError() function returns the last warning or error
	code used with Error() or Warning(). Note that a warning which
	was turned off is not saved as the last error.

SEE ALSO

	Error, Warning

*/
long moTextStream::ErrorCount(void) const
{
	return f_errcnt;
}


long moTextStream::WarningCount(void) const
{
	return f_warcnt;
}



moTextStream::errcode_t moTextStream::LastError(void) const
{
	return f_last_error;
}


/************************************************************ DOC:

CLASS

	moTextStream

NAME

	NoMessage - turn off all messages
	NoWarning - turn off all warnings
	WarningsToErrors - transform warnings in errors

SYNOPSIS

	void NoMessage(bool off = true);
	void NoWarning(bool off = true);
	void WarningsToErrors(bool on = true);

PARAMETERS

	off - when true, turns warnings off
	on - when true, transforms all warnings in errors

DESCRIPTION

	Use the NoMessages() function to turn off all messages from
	being printed out.

	Calling the NoWarning() function will ensure that none of the
	warnings are printed.

	The WarningsToErrors() function will be used to transform
	warnings in errors.

	By default warnings remains warnings.

NOTE

	The NoMessage() function sets a flag ON or OFF. There
	is no level of message used by any of the message functions.

	The NoWarning() flag has a higher precedence than the
	WarningsToErrors() flag. In other words, whatever the
	status of WarningsToErrors() no warning is generated.

	The WarningsToErrors() flag has a lower priority than the
	hidden warnings flags. In other words, warnings already
	hidden will not generate errors.

SEE ALSO

	Error, Warning

*/
void moTextStream::NoMessage(bool off)
{
	f_no_message = off;
}


void moTextStream::NoWarning(bool off)
{
	f_no_warning = off;
}


void moTextStream::WarningsToErrors(bool on)
{
	f_warnings_to_errors = on;
}


/************************************************************ DOC:

CLASS

	moTextStream::moWarning

NAME

	Constructor - creates an moWarning object
	Compare - compare two moWarnings

SYNOPSIS

	private:
	moWarning(errcode_t warcode);
	compare_t Compare(const moBase& object) const;

PARAMETERS

	warcode - the warning number
	object - another warning to compare

DESCRIPTION

	These functions will be used to know whether a warning needs to
	be transformed to an error or silenced.

	By default warnings remains warnings and they are printed out.

NOTE

	This flag has no effect on hidden warnings.

SEE ALSO

	Error, Warning

*/
moTextStream::moWarning::moWarning(errcode_t warcode)
{
	f_code = warcode;
}


moBase::compare_t moTextStream::moWarning::Compare(const moBase& object) const
{
	const moWarning& warning = dynamic_cast<const moWarning&>(object);

	if(f_code < warning.f_code) {
		return MO_BASE_COMPARE_SMALLER;
	}
	if(f_code > warning.f_code) {
		return MO_BASE_COMPARE_GREATER;
	}

	return MO_BASE_COMPARE_EQUAL;
}







/************************************************************ DOC:

CLASS

	moTextStream

NAME

	GetFilename - retrieve this stream filename

SYNOPSIS

	const moWCString moTextStream::GetFilename(void) const;

DESCRIPTION

	The GetFilename() function returns the name set in the text
	stream using SetFilename(). If that string is empty, it
	tries to get the input filename if there is an input stream
	specified. Finally, if neither the SetFilename() nor the
	input are defined, it tries with the output stream. When
	everything fails, it returns an empty string.

	This function is used by the different functions generating
	messages.

SEE ALSO

	Error, Warning, Message

*/
const moWCString moTextStream::GetFilename(void) const
{
	if(!f_filename.IsEmpty()) {
		return f_filename;
	}

	if(f_input) {
		return f_input->InputFilename();
	}
	if(f_output) {
		return f_output->OutputFilename();
	}

	// no known filename
	return "";
}


/************************************************************ DOC:

CLASS

	moTextStream

NAME

	Warning - prints out a warning message
	Error - prints out an error message

SYNOPSIS

	void FormatMessage(const char *type, const char *format, ...);
	void VFormatMessage(const char *type, const char *format, va_list args);
	void FormatWarning(int warcode, const char *format, ...);
	void VFormatWarning(int warcode, const char *format, va_list args);
	void FormatError(int errcode, const char *format, ...);
	void VFormatError(int errcode, const char *format, va_list args);
	void VPrintError(const char *format, va_list args);
	void PrintError(const char *format, ...);

PARAMETERS

	type - the text written before the ':', usually INFO or DEBUG
	warcode - a warning code
	errcode - an error code
	format - a printf(3) format string
	args - a list of arguments used with the format string

DESCRIPTION

	The Message() function can be called when an informational
	message is to be printed out. It is useful in order to use
	the same output file than the warnings and errors and yet
	print out informational or debug messages. One should use
	the name parameter to define the type of data being printed
	out in the message. No counter is incremented when this
	function is called.

	The Warning() function can be called every time a warning is
	generated. It will be printed if the warnings are not turned
	off. The warcode will be used to know whether that specific
	warning was turned off. If the "warning to errors" option was
	turned on, then this function will call the Error() function.
	Note that hidden warnings will not generate an error. For this
	reason, they are saved as the last generated error.

	The Error() function can be called every time an error is
	generated. It will print the error with the current line
	number and the last line read from the input file. The errcode
	can be anything the user wishes. Negative values can be used
	to generate fatal errors (i.e. the function will right away
	call exit(1); and print FATAL ERROR instead of ERROR.)

	The VPrintError() function can be used to print on the current
	error stream of the moTextStream object. It is used by the
	Warning() and Error() functions. This function uses a list of
	arguments specified on the stack.
	
	The PrintError() function is like the VPrintError() except that
	the list of arguments are given as is in the function call.

	By default, messages, warnings and errors are printed in the
	stderr stream unless the user attaches his own stream to this
	text stream object using the Error(moOStream *error) function.

NOTES

	The two functions can be called with a format of NULL. In
	this case the format parameter must be followed by the format
	string and a list of arguments (ap_list) as in this example:

		void my_error(const char *format, ...)
		{
			va_list	args;

			va_start(args, format);
			text_file->Error(45, NULL, format, args);
			va_end(args);
		}

SEE ALSO

	WarningStatus, ErrorCount, WarningCount, LastError,
	OnError, OnWarning, OnMessage

*/


/** \brief Print a formatted message to the output.
 *
 * This function prints a formatted message in the output.
 * This message is viewed as an informational message.
 *
 * The messages can be turned off using the NoMessage() function.
 *
 * This function does not increment any counter.
 *
 * One can overload the OnMessage() function in order to catch
 * calls to this function. The OnMessage() function
 * can return true to avoid the default printing of this
 * function.
 *
 * \bug
 * This function is not multi-thread aware. Messages will be mangled
 * if multiple threads call this, the FormatWarning() or FormatError()
 * functions simultaneously.
 *
 * \param[in] type    The type of message such as "info:", "debug:" or "trace:".
 * \param[in] format  A string format like printf().
 * \param[in] ...     Parameters to the format string.
 *
 * \sa NoMessage()
 * \sa OnMessage()
 * \sa VFormatMessage()
 * \sa FormatWarning()
 * \sa VFormatWarning()
 * \sa FormatError()
 * \sa VFormatError()
 */
void moTextStream::FormatMessage(const char *type, const char *format, ...)
{
	va_list		args;

	va_start(args, format);
	try {
		VFormatMessage(type, format, args);
	}
	catch(...)
	{
		// unfortunately we cannot have an RAII object for this one
		va_end(args);
		throw;
	}
	va_end(args);
}


/** \brief Print an informational message.
 *
 * Please, see FormatMessage() for more information.
 *
 * \param[in] type    The type of message such as "info:", "debug:" or "trace:".
 * \param[in] format  A string format like printf().
 * \param[in] args    The format arguments.
 *
 * \sa FormatMessage()
 */
void moTextStream::VFormatMessage(const char *type, const char *format, va_list args)
{
	moWCString	name;

	/* test if the messages were turned off */
	if(f_no_message || !f_error) {
		return;
	}

	if(!OnMessage(type, format, args)) {
		/* print out the warning */
		// TODO: add threading protection?
		PrintError("%s [%d]: %s ", f_progname.c_str(), (int) getpid(), type);
		name = GetFilename();
		if(!name.IsEmpty()) {
			PrintError(" in file \"%hhs\"", name.Data());
		}
		if(f_line != 0) {
			PrintError(" at line #%ld", f_line);
		}
		PrintError(": ");
		VPrintError(format, args);
		PrintError(".\n");
	}
}


/** \brief Print a formatted warning message to the output.
 *
 * This function prints a formatted warning message in the output.
 *
 * All the warning messages can be turned off using the NoWarning()
 * function. Specific warnings can be turned off using the WarningStatus()
 * function. Whenever this function detects that a specific warning
 * is turned off, it returns immediately. Specifically, it does not
 * turn that warning in an error nor write any message on the screen.
 *
 * Warnings can be turned to errors using the WarningsToErrors() function.
 * In that case, this function calls the VFormatError() error function.
 *
 * This function increments the warning counter as returned by the
 * WarningCount().
 *
 * One can overload the OnWarning() function in order to catch
 * calls to this function function. The OnWarning() function
 * can return true to avoid the default printing of this
 * function. Note that the OnWarning() function is not called
 * if the WarningsToErrors() function was called with true.
 *
 * \bug
 * This function is not multi-thread aware. Messages will be mangled
 * if multiple threads call this, the FormatMessage() or FormatError()
 * functions simultaneously.
 *
 * \param[in] warcode  The warning number as defined in your application.
 * \param[in] format   A string format like printf().
 * \param[in] ...      Parameters to the format string.
 *
 * \sa NoWarning()
 * \sa OnWarning()
 * \sa WarningStatus()
 * \sa FormatMessage()
 * \sa VFormatMessage()
 * \sa VFormatWarning()
 * \sa FormatError()
 * \sa VFormatError()
 */
void moTextStream::FormatWarning(errcode_t warcode, const char *format, ...)
{
	va_list		args;

	va_start(args, format);
	try {
		VFormatWarning(warcode, format, args);
	}
	catch(...)
	{
		// unfortunately we cannot have an RAII object for this one
		va_end(args);
		throw;
	}
	va_end(args);
}

/** \brief Format a warning message.
 *
 * Please, see FormatWarning() for more information.
 *
 * \param[in] warcode  The warning number as defined in your application.
 * \param[in] format   A string format like printf().
 * \param[in] args     The format arguments.
 *
 * \sa FormatWarning()
 */
void moTextStream::VFormatWarning(errcode_t warcode, const char *format, va_list args)
{
	moWCString	name;

	/* test if the warnings were turned off */
	if(f_no_warning) {
		return;
	}

	/* test if this specific warning was turned off */
	moWarning warning(warcode);
	if(f_warnings_off.Find(&warning) != moList::NO_POSITION) {
		return;
	}

	if(f_warnings_to_errors) {
		/*
		 * Transform the warning in an error
		 * Note that OnError() will be called from there
		 */
		VFormatError(warcode, format, args);
	}
	else {
		if(OnWarning(warcode, format, args)) {
			return;
		}

		/* print out the warning */
		// TODO: make it thread safe so a message is printed cleanly
		PrintError("%s [%d]: WARNING #%d", f_progname.c_str(), (int) getpid(), warcode);
		name = f_filename;
		if(name.IsEmpty()) {
			if(f_input) {
				name = f_input->InputFilename();
			}
			else if(f_output) {
				name = f_output->OutputFilename();
			}
		}
		if(!name.IsEmpty()) {
			PrintError(" in file \"%hhs\"", name.Data());
		}
		if(f_line != 0) {
			PrintError(" at line #%ld", f_line);
		}
		PrintError(": ");
		VPrintError(format, args);
		PrintError(".\n");

		f_last_error = warcode;
		f_warcnt++;
	}
}


/** \brief Print a formatted error message to the output.
 *
 * This function prints a formatted error message in the output.
 *
 * Errors cannot be turned off like the messages and errors.
 *
 * This function increments the error counter as returned by the
 * ErrorCount().
 *
 * One can overload the OnError() function in order to catch
 * calls to this function. The OnError() function
 * can return true to avoid the default printing of this
 * function. Note that if the OnError() function returns true,
 * then even negative errors are not viewed as fatal errors by
 * the function.
 *
 * By default, when the error code is negative, the function
 * prints "FATAL ERROR:" instead of "ERROR:" and exit the
 * program with exit(1).
 *
 * \todo
 * The exit(1) should be preceeded by a callback of some sort
 * to let the parser cleanup before exiting.
 *
 * \bug
 * This function is not multi-thread aware. Messages will be mangled
 * if multiple threads call this, the FormatMessage() or FormatError()
 * functions simultaneously.
 *
 * \param[in] errcode  The error number as defined in your application.
 * \param[in] format   A string format like printf().
 * \param[in] ...      Parameters to the format string.
 *
 * \sa OnError()
 * \sa FormatMessage()
 * \sa VFormatMessage()
 * \sa FormatWarning()
 * \sa VFormatWarning()
 * \sa VFormatError()
 */
void moTextStream::FormatError(errcode_t errcode, const char *format, ...)
{
	va_list		args;

	va_start(args, format);
	try {
		VFormatError(errcode, format, args);
	}
	catch(...)
	{
		// unfortunately we cannot have an RAII object for this one
		va_end(args);
		throw;
	}
	va_end(args);
}

/** \brief Print an error message.
 *
 * Please, see FormatError() for more information.
 *
 * \param[in] errcode  The error number as defined in your application.
 * \param[in] format   A string format like printf().
 * \param[in] args     The format arguments.
 *
 * \sa FormatError()
 */
void moTextStream::VFormatError(errcode_t errcode, const char *format, va_list args)
{
	moWCString	name;

	f_last_error = errcode;
	f_errcnt++;

	if(OnError(errcode, format, args)) {
		return;
	}

	/* print out the error */

//printf("Writing progname [%s] - %p\n", f_progname.MBData(), f_progname.MBData());

	PrintError("%s [%d]: ", f_progname.c_str(), (int) getpid());
	if(errcode < 0) {
		PrintError("FATAL ERROR #%d", -errcode);
	}
	else if(errcode != 0) {
		PrintError("ERROR #%d", errcode);
	}
	name = f_filename;
	if(name.IsEmpty()) {
		if(f_input) {
			name = f_input->InputFilename();
		}
		else if(f_output) {
			name = f_output->OutputFilename();
		}
	}
	if(!name.IsEmpty()) {
		PrintError(" in file \"%hhs\"", name.Data());
	}
	if(f_line != 0) {
		PrintError(" at line #%ld", f_line);
	}
	PrintError(": ");
	VPrintError(format, args);
	PrintError(".\n");

	// fatal errors break the whole process
	if(errcode < 0) {
		exit(1);
	}
}



/** \brief Print an error message.
 *
 * This function prints a message in what has been defined as
 * the error stream with SetError() in this text stream.
 *
 * If the error stream was never set, then stderr is used.
 *
 * \param[in] format    A format like in printf().
 * \param[in] ...       The parameters of the format string.
 *
 * \sa VPrintError()
 */
void moTextStream::PrintError(const char *format, ...)
{
	va_list		args;

	va_start(args, format);
	try {
		VPrintError(format, args);
	}
	catch(...)
	{
		va_end(args);
		throw;
	}
	va_end(args);
}


/** \brief Print an error message
 *
 * Please, see the PrintError() function for more information.
 *
 * \param[in] format    A format like in printf().
 * \param[in] args      The parameters of the format string.
 */
void moTextStream::VPrintError(const char *format, va_list args)
{
	// NOTE: we call Flush() here because otherwise the
	//	 output of these errors isn't synchronized
	//	 with the output of the direct fprintf(3)
	//	 function calls.
	if(!f_error) {
		if(!f_std_error) {
			f_std_error = new moStdOut(stderr);
		}
		mowc::vfprintf(f_std_error, format, args);	/* Flawfinder: ignore */
		f_std_error->Flush();
	}
	else {
		mowc::vfprintf(f_error, format, args);	/* Flawfinder: ignore */
		f_error->Flush();
	}
}







/************************************************************ DOC:

CLASS

	moTextStream

NAME

	WarningStatus - set a warning status

SYNOPSIS

	void WarningStatus(errcode_t warcode, bool on);

PARAMETERS

	warcode - a warning code
	on - true to have the warning, false to turn off the warning

DESCRIPTION

	The WarningStatus() function will be used to turn on and off
	the warnings.

SEE ALSO

	Warning

*/
void moTextStream::WarningStatus(errcode_t warcode, bool on)
{
	if(on) {
		/* we need a warning to find one in the list only */
		moWarning		warning(warcode);
		moList::position_t	position;

		position = f_warnings_off.Find(&warning);
		if(position != moList::NO_POSITION) {
			/* we found this warning, delete it */
			f_warnings_off.Delete(position);
		}
	}
	else {
		moWarningSPtr	warning(new moWarning(warcode));

		/* insert this new warning */
		f_warnings_off += *warning;
		/* note: if the warning was already in the list,
		 * then the new instance will simply be deleted here */
	}
}


/************************************************************ DOC:

CLASS

	moTextStream

NAME

	CleanNL - set whether '\r' and '\r\n' are transformed to '\n'

SYNOPSIS

	bool CleanNL(bool clean_nl = true);

PARAMETERS

	c - the character to push back

DESCRIPTION

	The GetC() function will count the '\r', '\r\n' and '\n' as
	a line separator (the line counter is increased by 1 each time).

	When the CleanNL() function is called with clean_nl set to true,
	then any sequence ('\r', '\r\n' and '\n') is transformed into a
	single '\n'.

	The UngetC() has no effect on the line counter (and thus, the
	following GetC() don't either).

RETURN VALUE

	the previous CleanNL flag status

SEE ALSO

	GetC, UngetC, NextLine

*/
bool moTextStream::CleanNL(bool clean_nl)
{
	bool		old;

	old = f_clean_nl;

	f_clean_nl = clean_nl;

	return old;
}



/************************************************************ DOC:

CLASS

	moTextStream

NAME

	GetC - return the next character
	GetLastC - return the last character read with GetC
	UngetC - push back one character

SYNOPSIS

	long GetC(void);
	long GetLastC(void);
	void UngetC(long c);

PARAMETERS

	c - the character to push back

DESCRIPTION

	When the moFileText object is used, one should use the GetC()
	and NextLine() functions to read characters. The GetC() function
	will automatically convert the bytes into wide characters
	(UCS-4).

	It is possible to unget characters which were read ahead using
	the UngetC() function. The maximum number of characters you can
	unget in this way is defined as MAX_UNGET_CHAR.

	Note that characters pushed back are read out in the reverse
	order they are pushed back.

	The GetLastC() function returns the last character read without
	moving the file pointer. If the last call to GetC() returned
	an EOF (or an error) then the GetLastC() function will return
	a nul char.

WARNINGS

	The GetC() can return the NUL character (0). This is a valid
	UCS-4 character.

NOTES

	The UngetC() function can unget any character. That character
	will be returned the next time the GetC() function is called.
	However, the character is never written in the input file.

RETURN VALUE

	GetC - the character read, EOF (-1) when the end of the file
		was reached or -2 when an error occurs
	GetLastC - the last character read by GetC() or 0 when EOF or
		an error was encountered
	UngetC - nothing

ERRORS

	If UngetC() is called too many times without calls to GetC()
	then an overflow is thrown (MO_ERROR_OVERFLOW).

	The Read() and Write() calls are not well synchronized with
	the GetC()/UngetC() functions.

BUGS

	These functions won't return an error if the input stream
	pointer is NULL. However, the UngetC() won't have any
	effect.

	The UngetC() won't count lines. The GetC() reading characters
	from UngetC() doesn't count lines either. This ensures a line
	number which doesn't vary from a user mistakes, however, it
	also means the current line number is not always what you would
	expect.

SEE ALSO

	NextLine, LastLine

*/
long moTextStream::GetC(void)
{
	int32_t		c;
	int		r;

	if(!f_input) {
		f_last_c = '\0';
		return EOF;
	}

	r = f_input->Get(c);

#if 0
fflush(stdout);
fprintf(stderr, "input Get() -> 0x%08lX  '%c'\n", c, c >= ' ' && c <= 126 ? (char) c : '?');
#endif

	if(r <= 0) {
		f_last_c = '\0';
		return r == 0 ? EOF : -2;
	}

	if((c == '\n' && f_last_c != '\r') || c == '\r') {
		f_line++;
	}
	f_last_c = c;

	if(f_clean_nl && c == '\r') {
		r = f_input->Get(c);
		if(r <= 0) {
			f_last_c = '\0';
			return r == 0 ? EOF : -2;
		}

		if(c != '\n') {
			// NOTE: if c is '\r' then the line was already counted!
			// and if we have two '\r' one after another, we should
			// not then have a '\n' (since ..\r\r.. means we're
			// reading a Mac file and no \n should be found in there.)
			if(c == '\r') {
				c = '\n';
			}
			f_input->Unget(c);
			c = '\n';
		}
	}

	return c;
}



long moTextStream::GetLastC(void) const
{
	return f_last_c;
}


void moTextStream::UngetC(int32_t c)
{
	// don't unget EOF, bad chars or errors
	if(c <= 0) {
		return;
	}

	if(f_input) {
		f_input->Unget(c);
	}
}



/************************************************************ DOC:

CLASS

	moTextStream

NAME

	NextLine - return the next line read
	LastLine - return the last line read again

SYNOPSIS

	bool SetLineSeparators(const moWCString& separators);
	bool NextLine(moWCString& string);
	const moWCString& LastLine(void) const;

PARAMETERS

	string - the read string
	separators - the characters accepted as line separators

DESCRIPTION

	The NextLine() function call will read another line in this
	file and save it in the user supplied string. It also keeps
	a copy internally so the user can call LastLine(); later on.
	If the end of file was reached (i.e. nothing is read before
	EOF is returned) then NextLine() returns false.

	The NextLine() function uses a string of separators. The
	user can specify that string using the SetLineSeparators()
	function. By default, the separators are defined with the
	following string:

		"\r\n?, \n"

	which means any new line (one of '\r' by itself, '\r\n' or
	'\n' by itself) and that new line is not kept in the
	resulting string.

	There are special characters used to declare the separators.
	These characters are defined as follow:

		. Plus (+)

		When the separator is preceeded by a plus, then it
		is not removed from the resulting string when
		found.

		. Question Mark (?)

		The last character of a separator can be optional.
		This is useful for the \r\n sequence which can then
		automatically be reduced to just \r.

		. Comma (,)

		The comma character seperate the different seperators.
		There can be multiple commas one after another, however
		this isn't a good practice.

		. Spaces (as defined in mowc::isspace(), space, tab...)

		These are ignored. Add spaces for readability.

		. Backslash (\)

		A backslash can be used before one of the special
		character in order to have that special character be
		part of the separator. Note that some special characters
		are not defined yet, but they may be added at a later
		time. It is therefore a good practice to escape any
		character which isn't a letter or a digit.

	The following is an example of a separator string for the XML
	parser which includes several possible separators:

		"+\r\n?, +\n, +>"

	The number of separators is not limited.

	If you call the SetLineSeparators() function with an invalid
	separator string, then false is returned and the error is set
	to:

		MO_ERROR_INVALID

RETURN VALUE

	true when a line was read, false otherwise (which means
	the EOF was reached in most cases)

ERRORS

	If NextLine() returns false an error may have happened. You
	can then check LastErrno() to know whether the end of file
	was reached or an error occured.

BUGS

	The NextLine() function uses the GetC() and UngetC() functions
	and thus is plagued with the problems linked to these two
	functions.

SEE ALSO

	Warning

*/
moTextStream::moSeparatorInfo::moSeparatorInfo(void)
{
	f_keep = false;
	f_last_is_optional = false;
	f_allocated = false;
	f_length = 0;
	f_separator = 0;
}


moTextStream::moSeparatorInfo::moSeparatorInfo(const moSeparatorInfo& info)
	: moBase(info)
{
	*this = info;
}

moTextStream::moSeparatorInfo::~moSeparatorInfo()
{
	if(f_allocated) {
		delete [] f_separator;
	}
}


moTextStream::moSeparatorInfo& moTextStream::moSeparatorInfo::operator = (const moSeparatorInfo& info)
{
	if(this != &info) {
		f_keep = info.f_keep;
		f_last_is_optional = info.f_last_is_optional;
		f_allocated = true;
		if(info.f_length > 0 && info.f_separator != 0) {
			f_length = info.f_length;
			f_separator = new mowc::wc_t[info.f_length + 1];
			memcpy(f_separator, info.f_separator, (info.f_length + 1) * sizeof(mowc::wc_t));
		}
		else {
			f_length = 0;
			f_separator = 0;
		}
		if(f_length < 2) {
			f_last_is_optional = false;
		}
	}

	return *this;
}


/** \brief Match a string with a separator.
 *
 * This function checks whether a string matches this separator. If
 * so, the function returns the length that matched.
 *
 * \param[in] str  The string to match with this separator
 *
 * \return 0 if nothing matches, the length of the match otherwise.
 */
int moTextStream::moSeparatorInfo::Match(const moWCString& str) const
{
	unsigned long		l;

	l = static_cast<unsigned long>(str.Length());
	if(l >= f_length) {
		if(str.Compare(f_separator, l - f_length) == MO_BASE_COMPARE_EQUAL) {
			return f_length;
		}
	}

	if(f_last_is_optional && l >= f_length - 1) {
		if(str.Compare(f_separator, l - f_length + 1, f_length - 1) == MO_BASE_COMPARE_EQUAL) {
			return f_length - 1;
		}
	}

	return 0;
}


/** \brief Defines a set of separators used by NextLine()
 *
 * By default, the separators are defined with the
 * following string:
 *
 * \code
 * "\r\n?, \n"
 * \endcode
 *
 * which means any new line (one of '\\r' by itself, '\\r\\n' or
 * '\\n' by itself.) Note that the characters that form the new
 * line are not kept in the resulting string.
 *
 * There are special characters used to declare the separators.
 * These characters are defined as follow:
 *
 * 	\li Plus (+)
 *
 *	\par
 * 	When the separator is preceeded by a plus, then it
 * 	is not removed from the resulting string when
 * 	found.
 *
 * 	\li Question Mark (?)
 *
 *	\par
 * 	The last character of a separator can be optional.
 * 	This is useful for the \\r\\n sequence which can then
 * 	automatically be reduced to just \\r.
 *
 * 	\li Comma (,)
 *
 *	\par
 * 	The comma character seperate the different seperators.
 * 	There can be multiple commas one after another, however
 * 	this isn't a good practice.
 *
 * 	\li Spaces (as defined in mowc::isspace(), space, tab...)
 *
 * 	\par
 * 	These are ignored. Add spaces for readability.
 *
 * 	\li Backslash (\)
 *
 * 	\par
 * 	A backslash can be used before one of the special
 * 	character in order to have that special character be
 * 	part of the separator. Note that some special characters
 * 	are not defined yet, but they may be added at a later
 * 	time. It is therefore a good practice to escape any
 * 	character which isn't a letter or a digit.
 *
 * The following is an example of a separator string for the XML
 * parser which includes several possible separators:
 *
 * \code
 * "+\r\n?, +\n, +>"
 * \endcode
 *
 * The number of separators is not limited.
 *
 * If you call the SetLineSeparators() function with an invalid
 * separator string, then false is returned and the text stream
 * error is set to:
 *
 * 	MO_ERROR_INVALID
 *
 * \param[in] separators   A string of separators.
 *
 * \return true if the input string looked correct.
 */
bool moTextStream::SetLineSeparators(const moWCString& separators)
{
	mowc::wc_t		c, *d, *sep;
	const mowc::wc_t	*s;
	moSeparatorInfo		info, *n;

	std::auto_ptr<mowc::wc_t> sepbuf(new mowc::wc_t[separators.Length() + 1]);
	sep = sepbuf.get();

	mowc::strcpy(sep, separators.Data());	/* Flawfinder: ignore */
	s = d = sep;
	c = *s;
	while(c != '\0') {
		if(mowc::isspace(c) || c == ',') {
			s++;
			c = *s;
			continue;
		}
		if(c == '+') {
			info.f_keep = true;
			s++;
		}
		else {
			info.f_keep = false;
		}
		info.f_separator = d;
		info.f_last_is_optional = false;	// default
		while(*s != '\0' && *s != ',') {
			if(*s == '?') {
				s++;
				while(mowc::isspace(*s)) {
					s++;
				}
				if(*s == ',' || *s == '\0') {
					info.f_last_is_optional = true;
				}
				else {
					// at this time, if ? is not at the end, include
					// it in the separator
					*d++ = '?';
				}
			}
			else if(*s == '\\') {
				s++;
				// accept all sorts of characters in this case
				if(*s != '\0') {
					*d++ = *s++;
				}
			}
			else if(!mowc::isspace(*s)) {
				*d++ = *s++;
			}
			else {
				s++;
			}
		}
		info.f_length = static_cast<unsigned long>(d - info.f_separator);
		c = *s;			// ha! it could be that the null terminator right after copy nul onto nul
		*d++ = '\0';		// always null terminate these strings

		if(info.f_length > 0) {
			n = new moSeparatorInfo(info);
			f_separators += *n;
		}
	}

	if(f_separators.IsEmpty()) {
		f_last_error = MO_ERROR_INVALID;
		return false;
	}

	return true;
}


/** \brief Read one line of text.
 *
 * The NextLine() function reads one line of text from the input
 * file and save it in the user supplied string. It also keeps
 * a copy internally so the user can call LastLine() later on.
 * If the end of file was reached (i.e. nothing is read before
 * EOF is returned) then NextLine() returns \p false.
 *
 * The NextLine() function uses a string of separators. The
 * user can specify that string using the SetLineSeparators()
 * function. Please, see that function for more information
 * about the syntax.
 *
 * \warning
 * If NextLine() returns false an error may have happened. You
 * can then check LastErrno() to know whether the end of file
 * was reached or an error occured.
 *
 * \bug
 * The NextLine() function uses the GetC() and UngetC() functions
 * and thus is plagued with the problems linked to these two
 * functions.
 *
 * \param[out] string  The string object where what is being read is saved
 *
 * \return true when a line was read, false otherwise (which means
 *         the EOF was reached, in most cases)
 *
 * \sa GetC()
 * \sa LastErrno()
 * \sa SetLineSeparators()
 * \sa UngetC()
 */
bool moTextStream::NextLine(moWCString& string)
{
	bool			result;
	mowc::wc_t		c;
	int			idx, count, l;
	const moSeparatorInfo	*info;

	/* restart with an empty string */
	string.Empty();

	if(f_separators.IsEmpty()) {
		/*
		 * If the following returns an error, then there is a
		 * problem in the algorithm!
		 */
		SetLineSeparators("\r\n?, \n");
	}

	count = f_separators.Count();

	/* read characters until a separator is found */
	result = false;
	while((c = GetC()) >= 0) {

#if 0
fflush(stdout);
fprintf(stderr, "GetC() in TextStream::NextLine() = 0x%04X  '%c'\n", c, c >= ' ' && c <= 126 ? c : '?');
#endif

		/* null are always skipped whatsoever */
		if(c == 0) {
			continue;
		}
		string += c;

		/* now search the end of the string in the list of separators */
		for(idx = 0; idx < count; idx++) {
			info = dynamic_cast<const moSeparatorInfo *>(f_separators.Get(idx));
			l = info->Match(string);
			if(l > 0) {
				/* we found it! */
				if(!info->f_keep) {
					/* in this case we have to delete the separator at the end of the string */
					string = string.Delete(static_cast<unsigned int>(string.Length() - l), static_cast<unsigned int>(string.Length() - 1));
				}
				if(info->f_last_is_optional) {
					/*
					 * Since we read the characters one by one, we will
					 * miss removing the following one if we don't do it
					 * now.
					 */
					c = GetC();
#if 0
fflush(stdout);
fprintf(stderr, "EXTRA ---> GetC() in TextStream::NextLine() = 0x%04X  '%c'\n", c, c >= ' ' && c <= 126 ? c : '?');
#endif
					if(c != info->f_separator[info->f_length - 1]) {
						/*
						 * Ha! That's not the case, keep that
						 * character for now.
						 */
						UngetC(c);
					}
					else if(info->f_keep) {
						/* we also have to keep it */
						string += c;
					}
				}
				/*
				 * ha! it could be an empty string but we found something
				 * so the function needs to return true
				 */
				result = true;
				goto done;
			}
		}
	}

done:
	f_last_line = string;

	return result || !string.IsEmpty();
}


/** \brief Returns the last line read by NextLine()
 *
 * This function returns a constant reference to a copy of
 * the last line read with NextLine().
 *
 * This is useful if you need to get a copy in many different
 * places and to avoid copying the string all the time.
 *
 * \return A reference to the internal copy of the last line read
 */
const moWCString& moTextStream::LastLine(void) const
{
	return f_last_line;
}



/************************************************************ DOC:

CLASS

	moTextStream

NAME

	Print - print a formatted string in this file

SYNOPSIS

	int Print(const char *format, ...);
	int VPrint(const char *format, va_list args);

PARAMETERS

	format - a standard C/C++ format string
	args - a standard list of arguments
	... - arguments to the format string

DESCRIPTION

	The Print() function uses the mowc::fprintf() function
	with this stream as the file parameter.

	The VPrint() function uses the mowc::vfprintf() function
	with this stream as the file parameter.

	Note that an output stream should have been defined with
	the constructor or with the SetOutput() function. When
	no such stream is defined, these functions always return
	zero (0).

RETURN VALUE

	This function returns the number of characters printed.

	-1 is returned if an error occurs.

BUGS

	The use of these functions isn't compatible with the GetC() and
	UngetC() (and thus the NextLine() function) when the same stream
	is used as the input and output. Please, read these function
	references for more information.

	The GetC() and UngetC() can be used if the input and output
	streams are two different files.

SEE ALSO

	moStream::Read, moStream::Write, Repeat, VRepeat,
	mowc::fprintf

*/
int moTextStream::Print(const char *format, ...)
{
	va_list		args;
	int		r;

	if(f_output) {
		va_start(args, format);
		r = mowc::vfprintf(f_output, format, args);	/* Flawfinder: ignore */
		va_end(args);
	}
	else {
		r = 0;
	}

	return r;
}

int moTextStream::VPrint(const char *format, va_list args)
{
	return !f_output ? 0 : mowc::vfprintf(f_output, format, args);	/* Flawfinder: ignore */
}




/************************************************************ DOC:

CLASS

	moTextStream

NAME

	Repeat - print a formatted string many times
	VRepeat - print a formatted string many times

SYNOPSIS

	int Repeat(long count, const char *format, ...);
	int VRepeat(long count, const char *format, va_list args);

PARAMETERS

	count - the number of time the formatted string(s) will be printed
	format - a standard C/C++ format string
	args - a standard list of arguments
	... - arguments to the format string

DESCRIPTION

	The Repeat() function uses the mowc::fprintf() function
	with this file as the file parameter. It prints out the
	corresponding format a number of times equal to the 'count'
	parameter.

	The VRepeat() function uses the mowc::fprintf() function
	call with this file as the file parameter. It prints out
	the corresponding format a number of times equal to the
	'count' parameter.

RETURN VALUE

	This function returns the number of characters printed when
	no error occurs.

	-1 is returned if an error occurs.

BUGS

	The use of these functions isn't compatible with the GetC() and
	UngetC() (and thus the NextLine() function). Please, read these
	functions references for more information.

SEE ALSO

	moFile::Read, moFile::Write, Print, VPrint

*/
int moTextStream::Repeat(long count, const char *format, ...)
{
	va_list		args;
	int		r, total;
	size_t		size;

	if(count < 0) {
		count = 0;
	}

	va_start(args, format);
	moWCString string = moWCString::VFormat(format, args);
	va_end(args);

	size = string.Length() * sizeof(mowc::wc_t);

	if(!f_output) {
		return static_cast<int>(size * count);
	}

	total = 0;
	while(count > 0) {
		count--;
		// TODO: what about the output encoding?!?
		r = f_output->Write(string.Data(), size);
		if(r < 0) {
			return -1;
		}
		total += r;
	}

	return total;
}

int moTextStream::VRepeat(long count, const char *format, va_list args)
{
	int		r, total;
	size_t		size;

	if(count < 0) {
		count = 0;
	}

	moWCString string = moWCString::VFormat(format, args);

	size = string.Length() * sizeof(mowc::wc_t);

	if(!f_output) {
		return static_cast<int>(size * count);
	}

	total = 0;
	while(count > 0) {
		count--;
		r = f_output->Write(string.Data(), size);
		if(r < 0) {
			return -1;
		}
		total += r;
	}

	return total;
}



/************************************************************ DOC:

CLASS

	moTextStream

NAME

	OnMessage - catch messages before they are output
	OnWarning - catch warnings before they are output
	OnError - catch errors before they are output

SYNOPSIS

	bool OnMessage(const char *type, const char *format, va_list args);
	bool OnWarning(errcode_t code, const char *format, va_list args);
	bool OnError(errcode_t code, const char *format, va_list args);

PARAMETERS

	type - the named type of message (INFO, DEBUG, etc.)
	code - the error code
	format - a standard C/C++ format string
	args - a standard list of arguments

DESCRIPTION

	The OnMessage(), OnWarning() & OnError() functions are called
	before the Message(), Warning() & Error() functions print out
	any error in the current error stream.

	Messages and warnings can be turned off with a call to NoMessage()
	and NoWarning(). If they are turned off, then corresponding
	OnMessage() and OnWarning() won't be called. Note that the
	warnings can be turned off individually in which case these
	also don't generate a call to OnWarning().

	The format parameter is a mostly standard C/C++ format string.
	You can use it to generate the message yourself. (possibly
	with the moWCString::VFormat() function or the low level
	mowc::fprintf() function). The arguments are those used
	with the format string. A message can also be totally
	ignored.

	The default functions will simple return false, letting
	the default functions print out the message as expected.

NOTE

	Since these functions are called before anything is printed
	they can be used to modify the error output stream.

	Thus, for instance, you could have three streams: one for
	errors, one for warnings and one for messages.

RETURN VALUE

	When the functions return true, then the message is not
	automatically printed by the moTextStream object.

	When false is returned, then the Message(), Error() and
	Warning() functions will behave as usual and print out
	the informational or error message.

SEE ALSO

	Message, Error, Warning, NoMessage, NoWarning

*/
bool moTextStream::OnMessage(const char *type, const char *format, const va_list args)
{
	return false;
}

bool moTextStream::OnWarning(errcode_t warcode, const char *format, const va_list args)
{
	return false;
}

bool moTextStream::OnError(errcode_t errcode, const char *format, const va_list args)
{
	return false;
}



}			// namespace molib;

// vim: ts=8

