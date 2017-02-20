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



#ifndef MO_TEXT_STREAM_H
#define	MO_TEXT_STREAM_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_WCSTRING_H
#include	"mo_string.h"
#endif
#ifndef MO_LIST_H
#include	"mo_list.h"
#endif

namespace molib
{

class MO_DLL_EXPORT moTextStream : public moBase
{
public:
	typedef	int		errcode_t;
	enum {
		MO_WHICH_INPUT = 0,
		MO_WHICH_OUTPUT,
		MO_WHICH_ERROR
	};

				moTextStream(moIStream *input = 0, moOStream *output = 0, moOStream *error = 0);
	virtual			~moTextStream();

	void			SetInput(moIStream *input, const moWCString *filename = 0);
	void			SetOutput(moOStream *output);
	void			SetError(moOStream *error);

	void			SetProgname(const moWCString& progname);

	moIStreamSPtr		Input(void) const;
	moOStreamSPtr		Output(void) const;
	moOStreamSPtr		Error(void) const;

	long			GetC(void);
	long			GetLastC(void) const;
	void			UngetC(int32_t c);
	bool			CleanNL(bool clean_nl = true);
	bool			NextLine(moWCString& string);
	bool			SetLineSeparators(const moWCString& separators);
	const moWCString&	LastLine(void) const;
	int			Print(const char *format, ...);
	int			VPrint(const char *format, va_list args);
	int			Repeat(long count, const char *format, ...);
	int			VRepeat(long count, const char *format, va_list args);

	long			Line(void) const;
	long			SetLine(long line);
	const moWCString	GetFilename(void) const;
	void			SetFilename(const moWCString& filename);
	void			FormatMessage(const char *type, const char *format, ...);
	void			VFormatMessage(const char *type, const char *format, va_list args);
	void			FormatWarning(errcode_t warcode, const char *format, ...);
	void			VFormatWarning(errcode_t warcode, const char *format, va_list args);
	void			FormatError(errcode_t errcode, const char *format, ...);
	void			VFormatError(errcode_t errcode, const char *format, va_list args);
	void			VPrintError(const char *format, va_list args);
	void			PrintError(const char *format, ...);
	long			WarningCount(void) const;
	long			ErrorCount(void) const;
	errcode_t		LastError(void) const;
	void			NoMessage(bool off = true);
	void			NoWarning(bool off = true);
	void			WarningsToErrors(bool on = true);
	void			WarningStatus(errcode_t warcode, bool on);

protected:
	virtual void		OnNewStream(int which);
	virtual bool		OnMessage(const char *type, const char *format, const va_list args);
	virtual bool		OnWarning(errcode_t warcode, const char *format, const va_list args);
	virtual bool		OnError(errcode_t errcode, const char *format, const va_list args);

private:
	class moSeparatorInfo : public moBase
	{
	public:
					moSeparatorInfo(void);
					moSeparatorInfo(const moSeparatorInfo& info);
					~moSeparatorInfo();
		moSeparatorInfo&	operator = (const moSeparatorInfo& info);
		int			Match(const moWCString& str) const;

		bool			f_keep;			// whether we keep separator in output
		bool			f_last_is_optional;	// last character isn't required
		bool			f_allocated;		// whether the separator string was allocated
		unsigned long		f_length;		// # of chars in separator (for faster testing)
		mowc::wc_t *		f_separator;		// the separator (null terminated string)
	};

	class moWarning : public moBase
	{
	public:
					moWarning(errcode_t code);

	protected:
		virtual compare_t	Compare(const moBase& object) const;

	private:
		errcode_t		f_code;
	};
	typedef moSmartPtr<moWarning>				moWarningSPtr;

	moIStreamSPtr		f_input;
	moOStreamSPtr		f_output;
	moOStreamSPtr		f_error;
	moOStreamSPtr		f_std_error;

	moWCString		f_progname;	// name printed in case of errors
	bool			f_no_message;	// turn off all messages (but not warnings nor errors)
	bool			f_no_warning;	// turn off all warnings
	bool			f_warnings_to_errors;
	moSortedListUnique	f_warnings_off;	// list of warnings turned off
	mutable long		f_warcnt;	// the number of warnings
	mutable long		f_errcnt;	// the number of errors
	mutable errcode_t	f_last_error;	// last error or warning number
	moWCString		f_filename;	// this text filename
	long			f_line;		// the current line number
	moWCString		f_last_line;	// last line read
	bool			f_clean_nl;	// transform '\r' and '\r\n' into one '\n'
	long			f_last_c;	// last character read
	moList			f_separators;	// a list of moSeparatorInfo for the NextLine()
};

typedef moSmartPtr<moTextStream>	moTextStreamSPtr;



};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_TEXT_STREAM_H

