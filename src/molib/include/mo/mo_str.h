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



#pragma once

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STREAM_H
#include	"mo_stream.h"
#endif

#ifdef MO_CONFIG_CTYPE_DEFINE
#undef isalnum
#undef isalpha
#undef iscntrl
#undef isdigit
#undef islower
#undef isprint
#undef ispunct
#undef isspace
#undef isupper
#undef isxdigit
#undef tolower
#undef toupper
//#undef isascii
#undef isblank
#endif


namespace molib
{

namespace mowc
{


/** \brief UCS-4 wide characters.
 *
 * This type represents a UCS-4 character.
 *
 * Note that the type is signed. The negative numbers do not represent characters.
 * Negative numbers are usually used for errors, except -1 that represents the end
 * of a file or string (i.e. EOF or EOS).
 *
 * This type can represent all the characters defined in Unicode.
 */
typedef int32_t			wc_t;

/** \brief UCS-2 wide characters.
 *
 * This type represents a UCS-2 character.
 *
 * Note that the type is unsigned. All the values represent characters, yet some
 * values are not valid characters. For instance 0xFFFE and 0xFFFF are not characters.
 * Often, 0xFFFE is viewed as 0xFEFF reversed (i.e. little versus big endian.)
 *
 * 0xFFFF is often viewed as the end of a file or string (i.e. EOF or EOS).
 */
typedef uint16_t		mc_t;

/** \brief Maximum size of a multi-byte character.
 *
 * This definition is used as the maximum number of bytes necessary to
 * represent one character in a multi-byte encoding. In most cases, the
 * limit is much smaller (2 bytes.)
 *
 * \note
 * UTF-8 has a maximum of 6 bytes.
 */
static const unsigned long	MAX_MB_SIZE = 8;



// the following encodings are used by different functions to
// internally convert strings; other encodings are available
// through iconv()
enum encoding_t {
	MO_ENCODING_UNKNOWN = 0,
	MO_ENCODING_ISO8859_1,			// ISO-8859-1 (the basic chars 0 to 255 in Unicode)
	MO_ENCODING_UTF8,			// multi-byte
	MO_ENCODING_UTF16_LE,			// 2 bytes in little endian
	MO_ENCODING_UTF16_BE,			// 2 bytes in big endian
	MO_ENCODING_UTF16_INTERNAL,		// machine dependent
	MO_ENCODING_UTF32_LE,			// 4 bytes in little endian
	MO_ENCODING_UTF32_BE,			// 4 bytes in big endian
	MO_ENCODING_UTF32_INTERNAL,		// machine dependent

	MO_ENCODING_max,

	MO_ENCODING_ASCII = MO_ENCODING_ISO8859_1
};

MO_DLL_EXPORT_FUNC extern const char *			g_ucs2_internal;
MO_DLL_EXPORT_FUNC extern const char *			g_ucs4_internal;
MO_DLL_EXPORT_FUNC extern const char *			g_utf32_internal;



MO_DLL_EXPORT_FUNC int		vfwprintf(moOStream *out, const wc_t *format, va_list args);
MO_DLL_EXPORT_FUNC int		fwprintf(moOStream *out, const wc_t *format, ...);
MO_DLL_EXPORT_FUNC int		vfprintf(moOStream *out, const char *format, va_list args);
MO_DLL_EXPORT_FUNC int		fprintf(moOStream *out, const char *format, ...);

MO_DLL_EXPORT_FUNC int		wctomblen(wc_t wc);
MO_DLL_EXPORT_FUNC int		wcstombslen(const wc_t *wc, size_t size = static_cast<size_t>(-1));

MO_DLL_EXPORT_FUNC int		wctomb(char *mb, wc_t wc);
MO_DLL_EXPORT_FUNC int		wcstombs(char *mb, const wc_t *s, unsigned long sz, size_t size = static_cast<size_t>(-1));
MO_DLL_EXPORT_FUNC int		mbtowc(wc_t& wc, const char *&mb, long& len);

MO_DLL_EXPORT_FUNC long		strtol(const char *mb, char **end = 0, int base = 10, int length = INT_MAX);
MO_DLL_EXPORT_FUNC int64_t		strtoll(const char *mb, char **end = 0, int base = 10, int length = INT_MAX);
MO_DLL_EXPORT_FUNC long		strtol(const wc_t *wc, wc_t **end = 0, int base = 0);
MO_DLL_EXPORT_FUNC int64_t		strtoll(const wc_t *wc, wc_t **end = 0, int base = 0);
MO_DLL_EXPORT_FUNC double		strtod(const wc_t *wc, wc_t **end = 0);
MO_DLL_EXPORT_FUNC bool		strtoipv4(const wc_t *wc, unsigned long& address, unsigned short& port, unsigned long& mask);

MO_DLL_EXPORT_FUNC int		strcmp(const wc_t *wc, const char *s, long length = LONG_MAX);
MO_DLL_EXPORT_FUNC int		strcmp(const wc_t *wc, const wc_t *s, long length = LONG_MAX);
MO_DLL_EXPORT_FUNC int		strcasecmp(const wc_t *wc, const char *s, long length = LONG_MAX);
MO_DLL_EXPORT_FUNC int		strcasecmp(const wc_t *wc, const wc_t *s, long length = LONG_MAX);

MO_DLL_EXPORT_FUNC void		strcpy(wc_t *wc, const char *s, long length = LONG_MAX, encoding_t encoding = MO_ENCODING_ISO8859_1);
MO_DLL_EXPORT_FUNC void		strcpy(wc_t *wc, const mc_t *s, long length = LONG_MAX, encoding_t encoding = MO_ENCODING_UTF16_INTERNAL);
MO_DLL_EXPORT_FUNC void		strcpy(wc_t *wc, const wc_t *s, long length = LONG_MAX, encoding_t encoding = MO_ENCODING_UTF32_INTERNAL);
MO_DLL_EXPORT_FUNC void		strcpy(wc_t *wc, const wchar_t *s, long length = LONG_MAX);	// always known

MO_DLL_EXPORT_FUNC int		strlen(const char *s, encoding_t encoding = MO_ENCODING_ISO8859_1);
MO_DLL_EXPORT_FUNC int		strlen(const mc_t *s);
MO_DLL_EXPORT_FUNC int		strlen(const wc_t *s);
MO_DLL_EXPORT_FUNC int		strlen(const wchar_t *s);

MO_DLL_EXPORT_FUNC wc_t *		strchr(const wc_t *s, wc_t c);
MO_DLL_EXPORT_FUNC wc_t *		strrchr(const wc_t *s, wc_t c);

MO_DLL_EXPORT_FUNC wc_t		backslash_char(const wc_t *& s);
MO_DLL_EXPORT_FUNC wc_t		backslash_char(wc_t *& s);

// wide char versions of the toupper(), tolower(), etc.
MO_DLL_EXPORT_FUNC wc_t		toupper(wc_t wc);
MO_DLL_EXPORT_FUNC wc_t		tolower(wc_t wc);
MO_DLL_EXPORT_FUNC long		tointeger(const wc_t *str, int base = 0);
MO_DLL_EXPORT_FUNC int64_t		tolargeinteger(const wc_t *str, int base = 0);
MO_DLL_EXPORT_FUNC double		tofloat(const wc_t *str);
MO_DLL_EXPORT_FUNC int		digit(wc_t wc);
MO_DLL_EXPORT_FUNC int		odigit(wc_t wc);
MO_DLL_EXPORT_FUNC int		xdigit(wc_t wc);
MO_DLL_EXPORT_FUNC int		zdigit(wc_t wc, int base);

MO_DLL_EXPORT_FUNC bool		isinteger(const wc_t *str, int base = 0);
MO_DLL_EXPORT_FUNC bool		isfloat(const wc_t *str);
MO_DLL_EXPORT_FUNC bool		isalnum(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isalpha(wc_t wc);
MO_DLL_EXPORT_FUNC bool		mo_isascii(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isblank(wc_t wc);
MO_DLL_EXPORT_FUNC bool		iscntrl(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isdigit(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isgraph(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isiso8859_1(wc_t wc);
MO_DLL_EXPORT_FUNC bool		islower(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isodigit(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isprint(wc_t wc);
MO_DLL_EXPORT_FUNC bool		ispunct(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isspace(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isupper(wc_t wc);
MO_DLL_EXPORT_FUNC bool		isxdigit(wc_t wc);
MO_DLL_EXPORT_FUNC bool		iszdigit(wc_t wc, int base);



class MO_DLL_EXPORT moIConv : public moFIFO
{
public:
				moIConv(void);
	virtual			~moIConv();

	virtual void		Reset(void);
	virtual int		SetEncodings(const char *from, const char *to);

	virtual int		Read(void *buffer, unsigned long size, bool peek = false);

private:
	moFIFO			f_output;
	encoding_t		f_input_type;
	iconv_t			f_input_convertor;
	iconv_t			f_output_convertor;
	zbool_t			f_skip_one_feff;
};



};			// namespace mowc;
};			// namespace molib;

// vim: ts=8 sw=8

