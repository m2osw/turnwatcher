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




#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_str.h"
#endif

#include	"mo/mo_str.h"

#ifndef MO_BUFFER_H
#include	"mo/mo_buffer.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{


#if BYTE_ORDER == BIG_ENDIAN
const char *		mowc::g_ucs2_internal = "UCS-2BE";
const char *		mowc::g_ucs4_internal = "UCS-4BE";
const char *		mowc::g_utf32_internal = "UTF-32BE";
#else
const char *		mowc::g_ucs2_internal = "UCS-2LE";
const char *		mowc::g_ucs4_internal = "UCS-4LE";
const char *		mowc::g_utf32_internal = "UTF-32LE";
#endif



/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	wctomblen - compute the number of bytes required to encode a wc_t char
	wcstombslen - compute the number of bytes required to encode a wc_t string

SYNOPSIS

	static int wctomblen(wc_t wc);
	static int wcstombslen(const wc_t *wcs, size_t size = static_cast<size_t>(-1));

PARAMETERS

	wc - an INTERNAL UCS-4 character
	wcs - an INTERNAL UCS-4 string of characters

DESCRIPTION

	The wctomblen() function will compute the number of bytes required to
	convert one wide character (wc) into a multy-byte character.

	The size will be the same as when wctomb() is called.

	wctomblen() will return 0 for any invalid character. Note that '\0'
	is taken as a valid character.

	wcstombslen() computes the number of bytes required to convert a string
	of UCS-4 characters in a string of multi-byte characters. The
	computation stops whenever a nul terminator is found. (This means no
	nul will be included in the length computation.)

	When a size parameter is specified, the computation will stop after
	that many characters were read even if the nul terminator wasn't
	found.

RETURN VALUE

	the number of bytes necessary for the multi-byte character(s)

NOTES

	when the wc char is L'\0', a size of 1 is returned

*/
int mowc::wctomblen(wc_t wc)
{
	if(wc < 0x80) {
		return 1;
	}
	if(wc < 0x800) {
		return 2;
	}
	if(wc < 0x10000) {
		return 3;
	}
	if(wc < 0x200000) {
		return 4;
	}
	if(wc < 0x4000000) {
		return 5;
	}
	if(wc > 0) {	// <=> (unsigned long) wc < 0x80000000
		return 6;
	}

	return 0;
}


int mowc::wcstombslen(const wc_t *wcs, size_t size)
{
	int		len;

	len = 0;
	if(wcs != 0) {
		while(*wcs != '\0' && size > 0) {
			len += wctomblen(*wcs);
			size--;
			wcs++;
		}
	}

	return len;
}




/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	wctomb - transforms a wide character to a multi-byte character

SYNOPSIS

	static int wctomb(char *mb, wc_t wc);
	int wcstombs(char *mb, const wc_t *s, unsigned long sz, size_t size = static_cast<size_t>(-1));

PARAMETERS

	mb - the multi-byte output string pointer
	wc - the character to define
	s - the wide character input string pointer
	sz - the size of the output string buffer
	size - the maximum number of character to copy from the s string

DESCRIPTION

	The wctomb() transforms one wide character (wc) to a multi-byte
	character (mb). The mb pointer needs to be at least 7 characters
	long. Please, use the MAX_MB_SIZE define to declare your buffer.

	Any valid wide character will be encoded. Invalid values are
	all the negative values (bit 31 set) and a few characters such
	as 0xFFFD.

	The wcstombs() will transform a whole string composed of
	wide characters (wc_t) into a multi-byte string of chars.
	The maximum size (sz) will be used to ensure the destination
	isn't overrun and for this reason, you should use an output
	buffer much larger than MAX_MB_SIZE (say, 256 bytes).

RETURN VALUE

	the number of characters saved in the mb string

NOTES

	the mb string can be "\0\0" in case the wc is L'\0', yet,
	in this case, a size of 1 is returned.

*/
int mowc::wctomb(char *mb, wc_t wc)
{
	if(wc < 0x80) {
		/* this will also encode '\0'... */
		mb[0] = (char) wc;
		mb[1] = '\0';
		return 1;
	}
	if(wc < 0x800) {
		mb[0] = (wc >> 6) | 0xC0;
		mb[1] = (wc & 0x3F) | 0x80;
		mb[2] = '\0';
		return 2;
	}
	if(wc < 0x10000) {
		mb[0] = (wc >> 12) | 0xE0;
		mb[1] = ((wc >> 6) & 0x3F) | 0x80;
		mb[2] = (wc & 0x3F) | 0x80;
		mb[3] = '\0';
		return 3;
	}
	if(wc < 0x200000) {
		mb[0] = (wc >> 18) | 0xF0;
		mb[1] = ((wc >> 12) & 0x3F) | 0x80;
		mb[2] = ((wc >> 6) & 0x3F) | 0x80;
		mb[3] = (wc & 0x3F) | 0x80;
		mb[4] = '\0';
		return 4;
	}
	if(wc < 0x4000000) {
		mb[0] = (wc >> 24) | 0xF8;
		mb[1] = ((wc >> 18) & 0x3F) | 0x80;
		mb[2] = ((wc >> 12) & 0x3F) | 0x80;
		mb[3] = ((wc >> 6) & 0x3F) | 0x80;
		mb[4] = (wc & 0x3F) | 0x80;
		mb[5] = '\0';
		return 5;
	}
	if(wc > 0) {	// <=> (unsigned long) wc < 0x80000000
		mb[0] = (wc >> 30) | 0xFC;
		mb[1] = ((wc >> 24) & 0x3F) | 0x80;
		mb[2] = ((wc >> 18) & 0x3F) | 0x80;
		mb[3] = ((wc >> 12) & 0x3F) | 0x80;
		mb[4] = ((wc >> 6) & 0x3F) | 0x80;
		mb[5] = (wc & 0x3F) | 0x80;
		mb[6] = '\0';
		return 6;
	}

	/* an invalid wide character (negative!) simply not encoded */
	mb[0] = '\0';
	return 0;
}


int mowc::wcstombs(char *mb, const wc_t *s, unsigned long sz, size_t size)
{
	size_t		l;
	char		*d;
	wc_t		c;

	if(mb == 0 || sz == 0) {
		return 0;
	}
	if(s == 0) {
		*mb = 0;		// at least we can nul terminate this one
		return 0;
	}

	sz--;			// the null terminator!
	d = mb;
	while(*s != '\0' && size > 0) {
		c = *s;
		l = wctomblen(c);
		if(sz < l) {		// too many bytes?
			break;
		}
		wctomb(d, c);
		sz -= static_cast<unsigned long>(l);
		d += l;
		size--;
		s++;
	}
	*d = '\0';

	return static_cast<int>(d - mb);		// total number of bytes used
}



/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	mbtowc - transforms a multi-byte character in a wide character

SYNOPSIS

	static int mbtowc(long& wc, const char *&mb, long& len);

PARAMETERS

	wc - a reference where the resulting character is written
	mb - the multi-byte input string pointer
	len - the number of bytes in the input string

DESCRIPTION

	The mbtowc() transforms a multi-byte character (mb) into a
	wide character (wc). The mb pointer needs to be up to 7
	characters long for the widest multi-byte characters.

	Any valid multi-byte character will be decoded.

	Though this isn't proper, all the bytes which form a wide
	character will be skipped even if they are not part of the
	wide character being read.

RETURN VALUE

	the number of bytes read from the mb string
	0 when the input string is empty
	-1 when an invalid byte is found (0x80 to 0xBF and 0xFE/0xFF
			or too many/not enough characters found)

	The mb and len parameters will have been modified accordingly
	including when errors are encountered.

*/
int mowc::mbtowc(wc_t& wc, const char *&mb, long& len)
{
	unsigned char		c;
	long			cnt, l;
	wc_t			_wc;

	// define a default output character of NUL
	_wc = wc = L'\0';

	if(len <= 0) {
		return 0;
	}

	// we eat one character from the source
	c = *mb++;
	len--;

	if(c < 0x80) {
		wc = c;
		return 1;
	}

	// we catch the errors early so things are easier!
	if((c >= 0x80 && c <= 0xBF) || c == 0xFE || c == 0xFF) {
		// this is a bad char!
		while(len > 0 && (c = *mb, (c >= 0x80 && c < 0xBF) || c == 0xFE || c == 0xFF)) {
			mb++;
			len--;
		}
		return -1;
	}


	if(c >= 0xFC) {
		_wc = c & 0x01;
		cnt = 5;
	}
	else if(c >= 0xF8) {
		_wc = c & 0x03;
		cnt = 4;
	}
	else if(c >= 0xF0) {
		_wc = c & 0x07;
		cnt = 3;
	}
	else if(c >= 0xE0) {
		_wc = c & 0x0F;
		cnt = 2;
	}
	else /*if(c >= 0xC0)*/ {	// always true so we don't check
		_wc = c & 0x1F;
		cnt = 1;
	}

	if(len < cnt) {
		while(len > 0 && (c = *mb, c >= 0x80 && c <= 0xBF)) {
			len--;
			mb++;
		}
		return -1;
	}
	len -= cnt;

	l = cnt;
	while(l > 0) {
		c = *mb;
		if(c < 0x80 || c > 0xBF) {
			len += l;
			return -1;
		}
		_wc = (_wc << 6) | (c & 0x3F);
		l--;
		mb++;
	}

	wc = _wc;

#if 0
// I think we want to know that there are bad characters
// so we actually keep them for the next call to report
// the error
	if(len > 0 && (c = *mb, c >= 0x80 && c <= 0xBF)) {
		// too many bytes to encode character!!!
		// skip them now to avoid an error on next call...
		// (the character is kept defined as it was)
		while(len > 0 && (c = *mb, c >= 0x80 && c <= 0xBF)) {
			mb++;
			len--;
		}
	}
#endif

	return cnt + 1;
}




/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	strlen - count the number of characters in a string

SYNOPSIS

	static int strlen(const char *s, encoding_t encoding = MO_ENCODING_ISO8859_1);
	static int strlen(const mc_t *s);
	static int strlen(const wc_t *s);
	static int strlen(const wchar_t *s);

PARAMETERS

	s - a chain of characters defined in bytes
	encoding - one of MO_ENCODING_ISO8859_1 or MO_ENCODING_UTF8
	wc - a reference where the resulting character is written
	mb - the multi-byte input string pointer
	len - the number of bytes in the input string

DESCRIPTION

	The different strlen() functions compute the length in
	characters of a string. Multibytes characters are return
	as a count of 1 (when represented in a wc_t, it will be
	1 character).

	Note that the first version (the one taking a char * as
	parameter) is the only one which takes an encoding as an
	additional parameter. This function accepts the ISO8859
	or any other 1 byte per character format (use the ISO8859
	encoding for all of these) and the UTF8 encoding. When
	UTF8 is specified, all the multibytes are counted as
	one character.

RETURN VALUE

	the number of characters in the string

*/

int mowc::strlen(const char *s, encoding_t encoding)	/* Flawfinder: ignore */
{
	const char	*e;
	unsigned char	c;
	size_t		cnt;

	if(s == 0) {
		return 0;
	}

	switch(encoding) {
	case MO_ENCODING_ISO8859_1:
		e = s;
		while(*e != '\0') {
			e++;
		}
		return static_cast<int>(e - s);

	case MO_ENCODING_UTF8:
		cnt = 0;
		while(*s != '\0') {
			// NOTE: 0xFE and 0xFF are invalid bytes and will be ignored later
			c = *s;
			if(c < 0x80 || (c > 0xBF && c != 0xFE && c != 0xFF)) {
				cnt++;
			}
			s++;
		}
		return static_cast<int>(cnt);

	default:
		throw moError("mowc::strlen(): invalid 8 bit encoding");

	}
	/*NOTREACHED*/
#ifdef MO_CONFIG_ALWAYS_NEED_RETURN
	return result;
#endif
}

int mowc::strlen(const mc_t *s)		/* Flawfinder: ignore */
{
	const mc_t	*e;

	if(s == 0) {
		return 0;
	}

	e = s;
	while(*e != static_cast<mc_t>('\0')) {
		e++;
	}

	return static_cast<int>(e - s);
}

int mowc::strlen(const wc_t *s)		/* Flawfinder: ignore */
{
	const wc_t	*e;

	if(s == 0) {
		return 0;
	}

	e = s;
	while(*e != static_cast<wc_t>(L'\0')) {
		e++;
	}

	return static_cast<int>(e - s);
}

int mowc::strlen(const wchar_t *s)
{
	const wchar_t	*e;

	if(s == 0) {
		return 0;
	}

	e = s;
	while(*e != L'\0') {
		e++;
	}

	return static_cast<int>(e - s);
}



/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	strtol - return the integer represented by the specified string
	strtoll - return the integer represented by the specified string
	strtod - return the double represented by the specified string

SYNOPSIS

	int strtol(const wc_t *wc, wc_t **end = 0, int base = 0);
	int64_t strtoll(const wc_t *wc, wc_t **end = 0, int base = 0);
	double strtod(const wc_t *wc, wc_t **end = 0);
	bool strtoipv4(const wc_t *wc, unsigned long& address, unsigned short& port, unsigned long& mask);

PARAMETERS

	wc - a string to parse
	end - a pointer where the input string is left
	base - the base used to parse the input
	address - the read address
	port - the port or zero
	mask - the mask or -1

DESCRIPTION

	The strtol() function will transform the input string in
	a signed decimal value. The strtoll() is similar but
	it also supports 64 bits integers.

	If the value starts with 0x or 0X, then the base parameter
	is ignored and 16 is used.

	The strtod() function transforms the input string in a
	double value.

	The end parameter can be used to get a copy of the source
	pointer where the parsing stopped.

	The strtoipv4() function reads the string and converts it
	to an address, an optional port and an optional mask. The
	expected syntax is as follow:

		a.b.c.d[:p][/w.x.y.z]
		a.b.c.d[:p][/m]

	where a, b, c, m, p, w, x, y and z and decimal numbers from
	0 to 255, except for m which can be between 0 and 32 and
	for p which can be between 0 and 65535. When just m is
	specified, then the mask is built as (-1 << m) re-ordered
	properly for your computer endian (i.e. up-side-down for
	little endian computers).

	The address can be preceeded and followed by spaces.

RETURN VALUE

	all but strtoipv4:
		the parsed value
		-1 or -1.0 an error occured (check errno as well!)

	stripv4:
		true when a valid address, port and mask was read

*/
long mowc::strtol(const wc_t *str, wc_t **end, int base)
{
	long		result, sign;
	const wc_t	*start;

	if(end != 0) {
		// Changed on Aug 06, 2003
		//*end = const_cast<wc_t *>(str);
		*end = 0;
	}

	if(str == 0 || base == 1 || base < 0 || base > 36) {
		errno = EINVAL;
		return -1;
	}

	while(isspace(*str)) {
		str++;
	}

	sign = 1;
	if(*str == '+') {
		str++;
	}
	else if(*str == '-') {
		str++;
		sign = -1;
	}
	start = str;

	errno = 0;		// by default, no error!
	result = 0;
	if(*str == '0') {
		if(str[1] == 'x' || str[1] == 'X') {
			// hexadecimal string
			str += 2;
			if(*str == L'\0') {
				errno = EINVAL;
				return -1;
			}
			base = 16;
		}
		if(base == 0) {
			// search the string for digit 8 or 9
			do {
				str++;
			} while(isodigit(*str));
			if(*str != '8' && *str != '9') {
				base = 8;
			}
			str = start;
		}
	}

	if(base == 0) {
		base = 10;
	}

	// decimal number
	while(iszdigit(*str, base)) {
		result = result * base + zdigit(*str, base);
		str++;
	}

	if(end != 0) {
		*end = const_cast<wc_t *>(str);
	}

	return result * sign;
}


int64_t mowc::strtoll(const wc_t *str, wc_t **end, int base)
{
	int64_t		result, sign;
	const wc_t	*start;

	if(end != 0) {
		*end = 0;
	}

	if(str == 0 || base == 1 || base < 0 || base > 36) {
		errno = EINVAL;
		return -1;
	}

	while(isspace(*str)) {
		str++;
	}

	sign = 1;
	if(*str == '+') {
		str++;
	}
	else if(*str == '-') {
		str++;
		sign = -1;
	}
	start = str;

	errno = 0;		// by default, no error!
	result = 0;
	if(*str == '0') {
		if(str[1] == 'x' || str[1] == 'X') {
			// hexadecimal string
			str += 2;
			if(*str == L'\0') {
				errno = EINVAL;
				return -1;
			}
			base = 16;
		}
		if(base == 0) {
			// search the string for digit 8 or 9
			do {
				str++;
			} while(isodigit(*str));
			if(*str != '8' && *str != '9') {
				base = 8;
			}
			str = start;
		}
	}

	if(base == 0) {
		base = 10;
	}

	// convert the string now
	while(iszdigit(*str, base)) {
		result = result * base + zdigit(*str, base);
		str++;
	}

	if(end != 0) {
		*end = const_cast<wc_t *>(str);
	}

	return result * sign;
}


double mowc::strtod(const wc_t *str, wc_t **end)
{
	double		sign, value, divisor, exponent, expsign;

	if(end != 0) {
		*end = 0;
	}

	if(str == 0) {
		errno = EINVAL;
		return -1.0;
	}

	while(isspace(*str)) {
		str++;
	}

	sign = 1.0;
	if(*str == '+') {
		str++;
	}
	else if(*str == '-') {
		str++;
		sign = -1.0;
	}

	value = 0.0;
	while(isdigit(*str)) {
		value = value * 10.0 + digit(*str);
		str++;
	}

	if(*str == '.') {
		divisor = 1.0;
		str++;
		while(isdigit(*str)) {
			divisor /= 10.0;
			value += digit(*str) * divisor;
			str++;
		}
	}

	if(*str == 'e' || *str == 'E') {
		str++;
		expsign = 1.0;
		if(*str == '+') {
			str++;
		}
		else if(*str == '-') {
			str++;
			expsign = -1.0;
		}
		exponent = 0.0;
		while(isdigit(*str)) {
			exponent = exponent * 10.0 + digit(*str);
			str++;
		}
		if(exponent != 0.0) {
			value *= pow(10.0, exponent * expsign);
		}
	}

	if(end != 0) {
		*end = const_cast<wc_t *>(str);
	}

	return value * sign;
}


bool mowc::strtoipv4(const wc_t *str, unsigned long& address, unsigned short& port, unsigned long& mask)
{
	unsigned long	p;
	unsigned long	addr[4];
	unsigned char	hton[4];	/* Flawfinder: ignore */

	address = 0;		// usually considered undefined, though 0 is a valid address
	port = 0;		// undefined
	mask = static_cast<unsigned long>(-1);		// default mask is no mask

	while(isspace(*str)) {
		str++;
	}

	// read the address
	addr[0] = strtol(str, const_cast<wc_t **>(&str), 10);
	if(errno != 0 || *str != '.' || addr[0] >= 0x100) {
		return false;
	}
	addr[1] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
	if(errno != 0 || *str != '.' || addr[1] >= 0x100) {
		return false;
	}
	addr[2] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
	if(errno != 0 || *str != '.' || addr[2] >= 0x100) {
		return false;
	}
	addr[3] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
	if(errno != 0 || addr[3] >= 0x100) {
		return false;
	}
	hton[0] = static_cast<unsigned char>(addr[0]);
	hton[1] = static_cast<unsigned char>(addr[1]);
	hton[2] = static_cast<unsigned char>(addr[2]);
	hton[3] = static_cast<unsigned char>(addr[3]);
	address = *reinterpret_cast<unsigned long *>(hton);

	// a port definition?
	if(*str == ':') {
		p = strtol(str + 1, const_cast<wc_t **>(&str), 10);
		if(errno != 0 || p >= 0x10000) {
			return false;
		}
		port = static_cast<unsigned short>(p);
	}

	// a mask definition?
	if(*str == '/') {
		addr[0] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
		if(errno == 0 && *str == '\0') {
			if(addr[0] > 32) {
				return false;
			}
			if(addr[0] == 32) {
				mask = 0;
			}
			else {
#if BYTE_ORDER == BIG_ENDIAN
				mask = static_cast<unsigned long>(-1) << addr[0];
#else
				mask = moSwap32Bits(static_cast<uint32_t>(static_cast<unsigned long>(-1) << addr[0]));
#endif
			}
		}
		else if(errno != 0 || *str != '.' || addr[0] >= 0x100) {
			return false;
		}
		else {
			addr[1] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
			if(errno != 0 || *str != '.' || addr[1] >= 0x100) {
				return false;
			}
			addr[2] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
			if(errno != 0 || *str != '.' || addr[2] >= 0x100) {
				return false;
			}
			addr[3] = strtol(str + 1, const_cast<wc_t **>(&str), 10);
			if(errno != 0 || *str != '\0' || addr[3] >= 0x100) {
				return false;
			}

			hton[0] = static_cast<unsigned char>(addr[0]);
			hton[1] = static_cast<unsigned char>(addr[1]);
			hton[2] = static_cast<unsigned char>(addr[2]);
			hton[3] = static_cast<unsigned char>(addr[3]);
			mask = *reinterpret_cast<unsigned long *>(hton);
		}
	}

	while(isspace(*str)) {
		str++;
	}

	if(*str != '\0') {
		errno = EINVAL;
		return false;
	}

	return true;
}




/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	strtol - returns the integer value read in the input string
	strtoll - returns the integer value read in the input string

SYNOPSIS

	long strtol(const char *mb, char **end, int base = 10, int length = INT_MAX);
	int64_t strtoll(const char *mb, char **end, int base = 10, int length = INT_MAX);

PARAMETERS

	mb - the source string to parse for a value
	end - where a non digit character is found or NULL
	base - the base of the value being read
	length - the maximum number of characters to read from mb

DESCRIPTION

	This function will convert a string to an integer using the
	specified base and reading a maximum number of characters
	as indicated by the length.

	The end parameter will receive the pointer of mb where the
	first non digit character is found.

	Note that the base is defaulted to 10. This function doesn't
	currently check and try to figure out which base should be
	used. It is up to you to find out. Note that any base from
	2 to 36 can be used.

	The leading spaces are skipped before any value is being
	parsed. These spaces could in the total length which will
	be parsed.

BUGS

	These versions of strtol() and strtoll() don't support
	the octal and hexadecimal syntax (i.e. 077 or 0x3F).

SEE ALSO

	strtol & tointeger (wide character versions)

*/
long mowc::strtol(const char *mb, char **end, int base, int length)
{
	long		sign, result;

	if(end != 0) {
		*end = 0;
	}

	if(mb == 0 || base < 2 || base > 36 || length == 0) {
		errno = EINVAL;
		return -1;
	}

	while(isspace(*mb) && length > 0) {
		mb++;
		length--;
	}
	if(length == 0) {
		errno = EINVAL;
		return -1;
	}

	sign = 1;
	if(*mb == '+') {
		mb++;
		length--;
	}
	else if(*mb == '-') {
		mb++;
		sign = -1;
		length--;
	}
	if(length == 0) {
		errno = EINVAL;
		return -1;
	}

	result = 0;
	while(length > 0 && iszdigit(*mb, base)) {
		result = result * base + zdigit(*mb, base);
		length--;
		mb++;
	}

	if(end != 0) {
		*end = const_cast<char *>(mb);
	}

	return result * sign;
}



int64_t mowc::strtoll(const char *mb, char **end, int base, int length)
{
	int64_t		sign, result;

	if(end != 0) {
		*end = 0;
	}

	if(mb == 0 || base < 2 || base > 36 || length == 0) {
		errno = EINVAL;
		return -1;
	}

	while(isspace(*mb) && length > 0) {
		mb++;
		length--;
	}
	if(length == 0) {
		errno = EINVAL;
		return -1;
	}

	sign = 1;
	if(*mb == '+') {
		mb++;
		length--;
	}
	else if(*mb == '-') {
		mb++;
		sign = -1;
		length--;
	}
	if(length == 0) {
		errno = EINVAL;
		return -1;
	}

	result = 0;
	while(length > 0 && iszdigit(*mb, base)) {
		result = result * base + zdigit(*mb, base);
		length--;
		mb++;
	}

	if(end != 0) {
		*end = const_cast<char *>(mb);
	}

	return result * sign;
}




/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	strcmp - compare two strings, case sensitive
	strcasecmp - compare two strings, case insensitive

SYNOPSIS

	static int strcmp(const wc_t *a, const char *b, long length = LONG_MAX);
	static int strcmp(const wc_t *a, const wc_t *b, long length = LONG_MAX);
	static int strcasecmp(const wc_t *a, const char *b, long length = LONG_MAX);
	static int strcasecmp(const wc_t *a, const wc_t *b, long length = LONG_MAX);

PARAMETERS

	a - a string
	b - a string

DESCRIPTION

	The strcmp() function compares two strings together. Two letters with
	a different case will not be recognized as being the same letter.

	The strcasecmp() function compares two strings together. Two letters
	which are the same except for their case are viewed as being the
	same. Note that is yet not similar to the collate version (i.e. the
	accentuated letters different among each others).

	The comparison is by value, none of the characters are
	collated.

	It is acceptable to call the function with a or b or
	both pointers being 0. A null pointer is considered
	equivalent to an empty string.

RETURN VALUE

	0 when the strings are equal
	-1 when the a string is before the b string (value wise)
	1 when the a string is after the b string (value wise)

BUGS

	At this time the char * version doesn't include an
	encoding. It should primarily be used with the ISO8859-1
	character set.

*/
int mowc::strcmp(const wc_t *a, const char *b, long length)
{
	long		d;

	if(a == 0) {
		if(b == 0) {
			return 0;
		}
		if(*b == '\0') {
			return 0;
		}
		return -1;
	}
	else if(b == 0) {
		if(*a == '\0') {
			return 0;
		}
		return 1;
	}

	while(*a != '\0' && length > 0) {
		d = *a - *b;
		if(d < 0) {
			return -1;
		}
		if(d > 0) {
			return 1;
		}
		a++;
		b++;
		length--;
	}

	if(*b == '\0' || length == 0) {
		return 0;
	}

	return -1;
}

int mowc::strcmp(const wc_t *a, const wc_t *b, long length)
{
	long		d;

	if(a == 0) {
		if(b == 0) {
			return 0;
		}
		if(*b == '\0') {
			return 0;
		}
		return -1;
	}
	else if(b == 0) {
		if(*a == '\0') {
			return 0;
		}
		return 1;
	}

	while(*a != '\0' && length > 0) {
		d = *a - *b;
		if(d < 0) {
			return -1;
		}
		if(d > 0) {
			return 1;
		}
		a++;
		b++;
		length--;
	}

	if(*b == '\0' || length == 0) {
		return 0;
	}

	return -1;
}



int mowc::strcasecmp(const wc_t *a, const char *b, long length)
{
	long		d;

	if(a == 0) {
		if(b == 0) {
			return 0;
		}
		if(*b == '\0') {
			return 0;
		}
		return -1;
	}
	else if(b == 0) {
		if(*a == '\0') {
			return 0;
		}
		return 1;
	}

	while(*a != '\0' && length > 0) {
		d = toupper(*a) - toupper(*b);
		if(d < 0) {
			return -1;
		}
		if(d > 0) {
			return 1;
		}
		a++;
		b++;
		length--;
	}

	if(*b == '\0' || length == 0) {
		return 0;
	}

	return -1;
}

int mowc::strcasecmp(const wc_t *a, const wc_t *b, long length)
{
	long		d;

	if(a == 0) {
		if(b == 0) {
			return 0;
		}
		if(*b == '\0') {
			return 0;
		}
		return -1;
	}
	else if(b == 0) {
		if(*a == '\0') {
			return 0;
		}
		return 1;
	}

	while(*a != '\0' && length > 0) {
		d = toupper(*a) - toupper(*b);
		if(d < 0) {
			return -1;
		}
		if(d > 0) {
			return 1;
		}
		a++;
		b++;
		length--;
	}

	if(*b == '\0' || length == 0) {
		return 0;
	}

	return -1;
}




/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	strcpy - copy a string in another

SYNOPSIS

	static void strcpy(wc_t *d, const char *s, long length = LONG_MAX, encoding_t encoding = MO_ENCODING_ISO8859_1);
	static void strcpy(wc_t *d, const mc_t *s, long length = LONG_MAX, encoding_t encoding = MO_ENCODING_UTF16_INTERNAL);
	static void strcpy(wc_t *d, const wc_t *s, long length = LONG_MAX, encoding_t encoding = MO_ENCODING_UTF32_INTERNAL);
	static void strcpy(wc_t *d, const wchar_t *s, long length = LONG_MAX);

PARAMETERS

	wc - the destination string
	s - the source string
	length - the maximum number of character copied (not including
		the null terminator which IS ALWAYS INSERTED)
	encoding - specify the encoding used to copy the string

DESCRIPTION

	These functions are used internally (though they are available
	to the end user) to copy any format string to the wc_t * type
	of strings.

	The destination wc string is always in the UTF32 INTERNAL format
	(endian of the currently running processor).

*/
void mowc::strcpy(wc_t *d, const char *s, long length, encoding_t encoding)	/* Flawfinder: ignore */
{
	long		len;

	if(d == 0 || length < 0) {
		return;
	}

	if(s != 0) switch(encoding) {
	case MO_ENCODING_ISO8859_1:
		// copy as is since UNICODE starts with the ISO8859-1 chars
		while(*s != '\0' && length > 0) {
			*d++ = (unsigned char) *s++;
			length--;
		}
		break;

	case MO_ENCODING_UTF8:
		// we need the total length in bytes of the input string
		// TODO: shall we use lstrlen()?
		len = static_cast<long>(::strlen(s));	/* Flawfinder: ignore */
		while(len > 0 && length > 0) {
			if(mbtowc(*d, s, len) > 0) {
				d++;
				length--;
			}
		}
		break;

	default:
		throw moError("mowc::strcpy(): invalid 8 bit encoding");

	}

	// terminate the string with a nul
	*d = 0;
}

void mowc::strcpy(wc_t *d, const mc_t *s, long length, encoding_t encoding)	/* Flawfinder: ignore */
{
	if(d == 0 || length < 0) {
		return;
	}

	if(s != 0) switch(encoding) {
	case MO_ENCODING_UTF16_INTERNAL:
#if BYTE_ORDER == BIG_ENDIAN
	case MO_ENCODING_UTF16_BE:
#else
	case MO_ENCODING_UTF16_LE:
#endif
		while(*s != 0 && length > 0) {
			*d++ = *s++;		// transformed from shorts to longs
			length--;
		}
		break;

#if BYTE_ORDER == BIG_ENDIAN
	case MO_ENCODING_UTF16_LE:
#else
	case MO_ENCODING_UTF16_BE:
#endif
		while(*s != 0 && length > 0) {
			*d++ = moSwap16Bits(*s++);
			length--;
		}
		break;

	default:
		throw moError("mowc::strcpy(): invalid encoding with an mc_t string");

	}

	// terminate the string with a nul
	*d = 0;
}

void mowc::strcpy(wc_t *d, const wc_t *s, long length, encoding_t encoding)	/* Flawfinder: ignore */
{
	if(d == 0 || length < 0) {
		return;
	}

	if(s != 0) switch(encoding) {
	case MO_ENCODING_UTF32_INTERNAL:
#if BYTE_ORDER == BIG_ENDIAN
	case MO_ENCODING_UTF32_BE:
#else
	case MO_ENCODING_UTF32_LE:
#endif
		while(*s != 0 && length > 0) {
			*d++ = *s++;
			length--;
		}
		break;

#if BYTE_ORDER == BIG_ENDIAN
	case MO_ENCODING_UTF32_LE:
#else
	case MO_ENCODING_UTF32_BE:
#endif
		while(*s != 0 && length > 0) {
			*d++ = moSwap32Bits(static_cast<int32_t>(*s++));
			length--;
		}
		break;

	default:
		throw moError("mowc::strcpy(): invalid encoding with an mc_t string");

	}

	// terminate the string with a nul
	*d = 0;
}

void mowc::strcpy(wc_t *d, const wchar_t *s, long length)	/* Flawfinder: ignore */
{
#if WCHAR_MAX == 65535
	strcpy(d, reinterpret_cast<const mc_t *>(s), length);	/* Flawfinder: ignore */
#else
	strcpy(d, reinterpret_cast<const wc_t *>(s), length);	/* Flawfinder: ignore */
#endif
}










/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	strchr - find the first instance of a character in a string
	strrchr - find the last instance of a character in a string

SYNOPSIS

	mowc strchr(const wc_t *s, wc_t c);
	mowc strrchr(const wc_t *s, wc_t c);

PARAMETER

	s - the string to scan
	c - the character searched

DESCRIPTION

	These functions search for the character c and
	return a pointer on the last found instance of
	it found in the input string.

	Note that if the character exists in the string,
	it will be included in the result.

	If the character doesn't exist in the string,
	then NULL is returned.

	strchr() will search for the first instance of the
	given character.

	strrchr() will search for the last instance of
	the given character.

RETURNED VALUE

	A pointer on the last found character or NULL.

*/
mowc::wc_t *mowc::strchr(const wc_t *s, wc_t c)
{
	while(*s != c && *s != '\0') {
		s++;
	}
	return *s == '\0' ? 0 : const_cast<wc_t *>(s);
}


mowc::wc_t *mowc::strrchr(const wc_t *s, wc_t c)
{
	const wc_t	*e;

	e = 0;
	while(s != '\0') {
		if(*s == c) {
			e = s;
		}
		s++;
	}

	return const_cast<wc_t *>(e);
}






/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	backslash_char - replace backslashed characters with the
		C/C++ corresponding character

SYNOPSIS

	wc_t backslash_char(const wc_t *& s);
	wc_t backslash_char(wc_t *& s);

PARAMETERS

	s - the input string where one character is read

DESCRIPTION

	The backslash_char() function accepts a C string as input
	and transforms the letters and digits in the corresponding
	backslashed character. The input string must point right
	after the backslash character (which is thus already eaten)
	and it will point after all the characters used to generate
	the backslashed character once the function returns. When
	an error occurs, the pointer may not be moved.

	The understood characters are as follow:

		\a		audible bell (0x07)
		\b		backspace (0x08)
		\c.		the control character (i.e. \cA to \cZ)
		\e or \?	escape (0x1B)
		\f		form feed (0x0C)
		\n		new line (0x0A)
		\r		carriage return (0x0D)
		\t		horizontal tab (0x09)
		\v		vertical tab (0x0B)

	The \ can also be followed by an octal or hexadecimal value:

		\x##		hexadecimal value
		\X##		hexadecimal value
		\L#...		hexadecimal value (any number of digits)
		\#...		octal value (any number of digits)

	All other characters are kept as is, thus "\\" becomes
	"\", etc.

RETURNED VALUE

	the value of the backslashed character

SEE ALSO

	Replace

*/
mowc::wc_t mowc::backslash_char(mowc::wc_t *& s)
{
	return backslash_char(const_cast<const wc_t *&>(s));
}

mowc::wc_t mowc::backslash_char(const mowc::wc_t *& s)
{
	int		digit, cnt;
	wc_t		value;

	if(s == 0) {
		return '\0';
	}

	switch(*s++) {
	case '\0':
		s--;
		return 0x00;

	case 'a':
		return 0x07;

	case 'b':
		return 0x08;

	case 'c':
		if(*s >= 0x40 && *s <= 0x7F) {
			s++;
			return s[-1] & 0x1F;
		}
		// if not a valid control, return 'c'
		return 'c';

	case 'e':
	case '?':
		return 0x1B;

	case 'f':
		return 0x0C;

	case 'n':
		return 0x0A;

	case 'r':
		return 0x0D;

	case 's':
		return ' ';

	case 't':
		return 0x09;

	case 'v':
		return 0x0B;

	case 'x':
	case 'X':
		/* hexadecimal - 1 or 2 digits only */
		digit = xdigit(s[0]);
		if(digit < 0) {
			return '?';		// invalid!
		}
		value = digit;
		digit = xdigit(s[1]);
		if(digit < 0) {
			s++;
			return value;
		}
		s += 2;
		return value * 16 + digit;

	case 'L':
		/* hexadecimal */
		value = 0;
		cnt = 8;
		do {
			digit = xdigit(*s);
			if(digit < 0) {
				return value;
			}
			value = value * 16 + digit;
			s++;
			cnt--;
		} while(*s != '\0' && cnt > 0);
		return value;

	case '0':		// This includes the '\0' entry
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		/* octal */
		value = 0;
		cnt = 11;
		do {
			digit = odigit(*s);
			if(digit < 0) {
				return value;
			}
			value = value * 8 + digit;
			s++;
			cnt--;
		} while(*s != '\0' && cnt > 0);
		return value;

	default:
		// no transformation
		return s[-1];

	}
}



/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	toupper - returns the upper case of the specified character
	tolower - returns the lower case of the specified character
	digit - transform a character in a decimal digit
	xdigit - transform a character in an hexadecimal digit
	tointeger - transform a string in an integer
	tolargeinteger - transform a string in a 64 bits integer
	tofloat - transform a string in a float

	isinteger - check whether a whole string represents an integer
	isfloat - check whether a whole string represents a float
	isalnum - test for alphanumeric characters
	isalpha - test for alphabetic characters
	isascii - test for ASCII characters
	isblank - test for blanks
	iscntrl - test for controls
	isdigit - test for a decimal digit
	isiso8859_1 - test for ISO 8859-1 characters
	isodigit - test for an octal digit
	isxdigit - test for an hexadecimal digit
	isgraph - test for graphical characters
	islower - test for lower case
	isprint - test for printable characters
	ispunct - test for punctuation
	isspace - test for space
	isupper - test for upper case


SYNOPSIS

	long		tointeger(const wc_t *str, int base = 0);
	int64_t		tolargeinteger(const wc_t *str, int base = 0);
	double		tofloat(const wc_t *str);
	wc_t		toupper(wc_t wc);
	wc_t		tolower(wc_t wc);
	int		digit(wc_t wc);
	int		xdigit(wc_t wc);

	bool		isinteger(const wc_t *str, int base = 0);
	bool		isfloat(const wc_t *str);
	bool		isalnum(wc_t wc);
	bool		isalpha(wc_t wc);
	bool		isascii(wc_t wc);
	bool		isblank(wc_t wc);
	bool		iscntrl(wc_t wc);
	bool		isdigit(wc_t wc);
	bool		isodigit(wc_t wc);
	bool		isxdigit(wc_t wc);
	bool		isgraph(wc_t wc);
	bool		islower(wc_t wc);
	bool		isprint(wc_t wc);
	bool		ispunct(wc_t wc);
	bool		isspace(wc_t wc);
	bool		isupper(wc_t wc);


DESCRIPTION

	These are wide character versions of the ctype functions.

SEE ALSO

	CaseCompare

*/
mowc::wc_t mowc::toupper(wc_t c)
{
	// TODO: fix me so any character is "upperized"
	if(c >= 'a' && c <= 'z') {
		return c & 0x5F;
	}

	return c;
}


mowc::wc_t mowc::tolower(wc_t c)
{
	// TODO: fix me so any character is "lowerized"
	if(c >= 'A' && c <= 'Z') {
		return c | 0x20;
	}

	return c;
}


int mowc::digit(wc_t c)
{
	if(c >= '0' && c <= '9') {
		return c - '0';
	}

	return -1;
}


int mowc::odigit(wc_t c)
{
	if(c >= '0' && c <= '7') {
		return c - '0';
	}

	return -1;
}


int mowc::xdigit(wc_t c)
{
	if(c >= '0' && c <= '9') {
		return c - '0';
	}
	if(c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	if(c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}

	return -1;
}


int mowc::zdigit(wc_t c, int base)
{
	int		v;

	if(c >= '0' && c <= '9') {
		v = c - '0';
	}
	else if(c >= 'A' && c <= 'Z') {
		v = c - 'A' + 10;
	}
	else if(c >= 'a' && c <= 'z') {
		v = c - 'a' + 10;
	}
	else {
		return -1;
	}

	return v >= base ? -1 : v;
}


long mowc::tointeger(const wc_t *str, int base)
{
	wc_t		*end;
	long		r;

	r = strtol(str, &end, base);

	if(*end != '\0') {
		errno = EINVAL;
		return -1;
	}

	return r;
}



int64_t mowc::tolargeinteger(const wc_t *str, int base)
{
	wc_t		*end;
	int64_t		r;

	r = strtoll(str, &end, base);

	if(*end != '\0') {
		errno = EINVAL;
		return -1LL;
	}

	return r;
}



double mowc::tofloat(const wc_t *str)
{
	wc_t		*end;
	double		r;

	r = strtod(str, &end);

	if(*end != '\0') {
		errno = EINVAL;
		return -1.0;
	}

	return r;
}



bool mowc::isinteger(const wc_t *s, int base)
{
	if(*s == '+' || *s == '-') {
		s++;
	}

	// force the hexadecimal base if value starts with 0x or 0X
	if(*s == '0') {
		if(s[1] == 'x' || s[1] == 'X') {
			base = 16;
		}
		else if(base == 0) {
			base = 8;
		}
	}

	if(base == 0) {
		base = 10;
	}

	while(*s != '\0') {
		if(!iszdigit(*s, base)) {
			return false;
		}
		s++;
	}

	return true;
}



bool mowc::isfloat(const wc_t *str)
{
	if(*str == '+' || *str == '-') {
		str++;
	}

	while(isdigit(*str)) {
		str++;
	}

	if(*str == '.') {
		str++;
		while(isdigit(*str)) {
			str++;
		}
	}

	if(*str == 'e' || *str == 'E') {
		str++;
		if(*str == '+' || *str == '-') {
			str++;
		}
		while(isdigit(*str)) {
			str++;
		}
	}

	return *str == '\0';
}


bool mowc::isalnum(wc_t c)
{
	return isdigit(c) || isalpha(c);
}


bool mowc::isalpha(wc_t c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}



bool mowc::mo_isascii(wc_t c)
{
	return c <= 127;
}


bool mowc::isblank(wc_t c)
{
	return c == ' ' || c == '\t' || c == '\b' || c == '\f' || c == '\v' || c == '\n' || c == '\r' || c == 0xFEFF;
}


bool mowc::iscntrl(wc_t c)
{
	// TODO: make sure all controls are included and that '\0'
	// is taken as a control!
	return (c >= 0 && c <= 0x1F) || (c >= 127 && c <= 0x9F);
}


bool mowc::isdigit(wc_t c)
{
	return c >= '0' && c <= '9';
}


bool mowc::isgraph(wc_t c)
{
	return c > 160 && c <= 255;
}


bool mowc::isiso8859_1(wc_t c)
{
	return c <= 255;
}


bool mowc::islower(wc_t c)
{
	// TODO: fix me so we return true for all lower chars
	if(c >= 'A' && c <= 'Z') {
		return false;
	}

	return true;
}


bool mowc::isodigit(wc_t c)
{
	return c >= '0' && c <= '7';
}


bool mowc::isprint(wc_t c)
{
	return c >= ' ' && (c < 127 || c > 160);
}


bool mowc::ispunct(wc_t c)
{
	return !isspace(c) && !isalpha(c) && !isdigit(c);
}


bool mowc::isspace(wc_t c)
{
	return c == ' ' || c == '\t' || c == '\f' || c == 0xFEFF;
}


bool mowc::isupper(wc_t c)
{
	// TODO: fix me so we return true for all upper chars
	if(c >= 'a' && c <= 'z') {
		return false;
	}

	return true;
}


bool mowc::isxdigit(wc_t c)
{
	return	   (c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'F')
		|| (c >= 'a' && c <= 'f');
}


bool mowc::iszdigit(wc_t c, int base)
{
	int	v;
	
	if(c >= '0' && c <= '9') {
		v = c - '0';
	}
	else if(c >= 'A' && c <= 'Z') {
		v = c - 'A' + 10;
	}
	else if(c >= 'a' && c <= 'z') {
		v = c - 'a' + 10;
	}
	else {
		return false;
	}

	return v < base;
}






/************************************************************ DOC:

NAMESPACE

	mowc

NAME

	fwprintf - write a formated string to a stream
	vfwprintf - write a formated string to a stream

SYNOPSIS

	int fwprintf(moOStream *out, const wc_t *format, ...);
	int vfwprintf(moOStream *out, const wc_t *format, va_list args);
	int fprintf(moOStream *out, const char *format, ...);
	int vfprintf(moOStream *out, const char *format, va_list args);

DESCRIPTION

	The format function can be used to create a string with the
	result of a formatted set of arguments.

	The vfwprintf() accepts a variable list of arguments as
	obtained by va_start(args, <last parameter>).

	It doesn't use the standard wprintf() function of the C library
	because otherwise it would transform the different characters
	depending on environment defined settings. Instead, the whole
	function format was reprogrammed.

	Please, though it doesn't use the wprintf(), you certainly
	want to see this function documentation for more information
	about the default formats supported by the function.

	The following is a list of the flags supported by this
	implementation:

	The format of a conversion is as follow:

		'%' [<position>'$'] [<flags>] [<width>] ['.'<precision>] [<length>] <conversion>

	There is one special case: "%%" will be used to insert a percent
	in the output.

	Position:

		Defines the position of the argument in the list of
		parameters. This is useful to write the parameters
		in run-time varying order.

		The positions start at 1 from the first parameter
		after the format.

	Flags:

	#	introducer; when used as in %#o, prepend a "0"; when used
		as in %#x or %#X, include a "0x" or "0X" respectively;
		when used with one of the floating conversions, ensures
		that a decimal point always appears.

	' '	(space) writes a space wherever a signed and positive
		value is written

	+	writes a plus for positive and signed values (unsigned
		value conversions ignore this flag).

	0	zero pad on the left up to either precision or width
		digits

	-	left justify (default to right justification).

	`	write out thousands in decimal conversions.

	Width:

		The width is a decimal value which can be defined as a direct
		value an asterisk or an asterisk followed by a position
		(<position>'$'). When an asterisk is specified, the width is
		taken from the list of parameters as an 'int'.

	Precision:

		The precision is a decimal value which can be defined as a
		direct value, an asterisk or an asterisk followed by a position
		(<position>'$'). When an asterisk is specified, the precision
		is taken from the list of parameters as an 'int'.

	Length:

	hh	A character.

	h	A short.

	<none>	An integer.

	l	A long.

	ll	A long long (int64_t).

	L	A long double. (not currently available on Macs)

	q	A long long (like ll, int64_t).

	j	An intmax_t.

	z or Z	A size_t (the 'Z' is deprecated and should never be used).

	t	A ptrdiff_t.

	Conversions:


	D,d,i	Writes out a decimal number. 'D' is deprecated. It can be
		used to write out a long value (i.e. use %ld instead).
		All the sizes, except 'L' can be used with this conversion.
		'#' has no effect.


	U,u,O,o,X,x
		Writes out an unsigned number in decimal (u,U), octal (o,O)
		or hexadecimal (x,X) notation. The U and O are deprecated.
		It can be used to write out a long value (i.e. use %lu
		and %lo instead). All the sizes, except the 'L' can be
		used with this conversion. '#' has no effect on the u and
		U conversions. With the o and O is ensures the value
		always starts with a zero. With the x and X is introduces
		the value with 0x and 0X respectively.

	e,E,f,F,g,G,a,A
		Writes out a floating point number. The 'e' and 'E' can
		be used to write number in scientific notation (always
		including an exponent). The 'f' can be used to always
		avoid an exponent (watch out for really large or
		small values!). The 'F' is deprecated and works like 'f'.
		The 'g' and 'G' will select between 'f' and 'e' or 'E'
		depending on the value to be written. When the value
		can be written in a reasonable width using 'f', then
		it is used. The 'a' and 'A' print out the value in
		hexadecimal floating point. In all cases, the capital
		version makes the exponent letter be written in capital.
		The only two length accepted are <none> and 'L'.

	c,C	Writes out one character. The C notation is deprecated.
		It can be used to output of a long character (use %lc
		instead). The lengths accepted are 'hh' (char),
		'h' (mc_t), <none> or 'll' (wc_t) and 'l' (wchar_t).

		  Note: if the fprintf() and vfprintf() versions are used,
			then the meaning of 'll' (or <none>) and 'hh' are
			exchanged.

	s,S	Writes out a string of characters. The S notation is
		deprecated. It can be used to output of a string of
		long characters (use %ls instead). The lengths accepted
		are 'hh' (char), 'h' (mc_t), <none> or 'll' (wc_t) and
		'l' (wchar_t).

		  Note: if the fprintf() and vfprintf() versions are used,
			then the meaning of 'll' (or <none>) and 'hh' are
			exchanged.

	p,P	Writes out a pointer. The format used is "%#x" with the
		adequate size. The capital 'P' can be used to have the
		hexadecimal letters (and the 0X) in uppercase. This
		conversion doesn't accept a length.

	n	Used to query the number of characters written at the
		point where this "conversion" appears. By default, the
		type expected for this parameter is an 'int'. You can
		use the length argument to change the type to any
		length except 'L'.

	m	Writes out a string as returned by strerror(<arg>);
		where <arg> is an integer. The string will automatically
		be transformed to unicode as required.

SEE ALSO

	backslash_char

BUGS

	A common mistake is to type something like this:

		moWCString	string;

		string.VFormat(...);

	in which case the result is lost. One needs to use the function
	as a static function instead:

		moWCString	string;

		string = moWCString::VFormat(...);

*/
class printf_info_t {
public:
	enum conversion_t {
		CONVERSION_UNKNOWN = 0,
		CONVERSION_DIRECT,		// copy from source string
		CONVERSION_INTEGER,		// d or D or i
		CONVERSION_UNSIGNED,		// u or U
		CONVERSION_OCTAL,		// o or O
		CONVERSION_HEXADECIMAL,		// x or X
		CONVERSION_SCIENTIFIC,		// e or E
		CONVERSION_FLOAT,		// f or F
		CONVERSION_SELECT,		// g or G
		CONVERSION_HEXAFLOAT,		// a or A
		CONVERSION_CHAR,		// c or C
		CONVERSION_STRING,		// s or S
		CONVERSION_POINTER,		// p or P
		CONVERSION_COUNT,		// n
		CONVERSION_STRERROR		// m
	};
	enum length_t {
		LENGTH_UNKNOWN = 0,
		LENGTH_CHAR,
		LENGTH_SHORT,
		LENGTH_INTEGER,
		LENGTH_LONG,
		LENGTH_LONG_LONG,
		// LENGTH_DOUBLE -- infortunatly, this uses LENGTH_INTEGER
#ifndef MO_CONFIG_NO_LONG_DOUBLE
		LENGTH_LONG_DOUBLE,
#endif
		LENGTH_INTMAX,
		LENGTH_SIZE,
		LENGTH_PTRDIFF,

		LENGTH_WIDTH,
		LENGTH_PRECISION
	};

///////////////////////////////////////////////////////////////
// we use 2 passes to read the arguments properly
// (since args need to be read in order)
// the 1st pass generates an array of entries
// and the 2nd (when f_max && f_d aren't null)
// prints out the result to the user specified stream
class entry_t
{
public:
	conversion_t		f_conversion;	// what to do with this entry in the 2nd pass
	const mowc::wc_t *	f_start;	// inclusive
	const mowc::wc_t *	f_end;		// exclusive
	int			f_arg_pos;
	int			f_width_pos;
	int			f_prec_pos;
	int			f_width;
	int			f_precision;
	length_t		f_length;
	unsigned int		f_uppercase       : 1;		// XEFGACS vs xefgacs
	unsigned int		f_zero_pad        : 1;
	unsigned int		f_introducer      : 1;
	unsigned int		f_sign            : 1;
	unsigned int		f_space           : 1;
	unsigned int		f_left_align      : 1;
	unsigned int		f_thousands       : 1;

	// on the 2nd pass we will fill the following in the
	// order given by the f_arg_pos
	union {
		/*
		short		f_short;
		int		f_int;
		long		f_long;
		int64_t		f_long_long;
		size_t		f_size;
		ptrdiff_t	f_ptrdiff;
		*/
		char			f_char;
		mowc::mc_t		f_mc;
		mowc::wc_t		f_wc;
		wchar_t			f_wchar;
		intmax_t		f_intmax;
		double			f_double;
#ifndef MO_CONFIG_NO_LONG_DOUBLE
		long double		f_long_double;
#endif
		void *			f_ptr;
	} f_value;

entry_t(void)
{
	f_conversion      = CONVERSION_UNKNOWN;
	f_start           = 0;
	f_end             = 0;
	f_arg_pos         = -1;
	f_width_pos       = -1;
	f_prec_pos        = -1;
	f_width           = -1;
	f_precision       = -1;
	f_length          = LENGTH_UNKNOWN;
	f_uppercase       = 0;
	f_zero_pad        = 0;
	f_introducer      = 0;
	f_sign            = 0;
	f_space           = 0;
	f_left_align      = 0;
	f_thousands       = 0;
}

void bad(void)
{
	f_conversion = CONVERSION_DIRECT;
}

void conversion(conversion_t c)
{
	if(f_conversion == CONVERSION_UNKNOWN) {
		f_conversion = c;
	}
}



};
// entry_t definition
///////////////////////////////////////////////////////////////


	// entry args are typed and can be int for args_pos, width_pos and precision_pos
	struct ref_t {
		entry_t *	f_entry;
		length_t	f_length;
	};




	int			f_count;
	moOStream *		f_out;
	const mowc::wc_t *	f_format;
	va_list			f_args;			// where we're reading when no $ used
	bool			f_bad;
	bool			f_swap;			// whether to swap the %s vs %lls & %c vs %llc
	int			f_pos;			// current position (also possibly last <value>$ + 1)

	int			f_entry_count;		// the number of entries
	entry_t *		f_entry;		// the array of entries
	int			f_entry_args_count;	// this is "only" for the args positions
	ref_t *			f_entry_args;		// entries sorted by args (first of)
	int			f_entry_args_max;	// the last f_entry_args (exclusive) used
	entry_t *		f_current;		// the entry we're working on


printf_info_t(void)
{
	f_count = 0;
	f_out = 0;
	f_format = 0;
	//va_copy(f_args, va_list()); -- if f_format == 0, f_args is not initialized
	f_bad = false;
	f_pos = 0;

	f_entry_count = 0;
	f_entry = 0;
	f_entry_args_count = 0;
	f_entry_args = 0;
	f_entry_args_max = 0;
	f_current = 0;

}


~printf_info_t()
{
	delete [] f_entry;
	delete [] f_entry_args;

	// The arguments are defined if the format is defined
	if(f_format != 0) {
		// we used va_copy() and thus need to call va_end()
		// (in case memory was allocated.)
		va_end(f_args);
	}
}


void count_percents(void)
{
	const mowc::wc_t	*s;

	f_entry_count = 1;
	f_entry_args_count = 0;
	s = f_format;
	while(*s != '\0') {
		if(*s == '%') {
			if(s[1] == '%') {
				// this is an entry!
				s++;
				f_entry_count += 2;
			}
			else {
				f_entry_count += 2;
				// each format can have up to three references
				f_entry_args_count += 3;
			}
		}
		s++;
	}

	// f_entry_count may be only 1 larger than necessary
}


void setup_entries(void)
{
	f_entry = new entry_t[f_entry_count];
	f_entry_args = new ref_t[f_entry_args_count];

	// we need to initialize the f_entry_args now
	for(int i = 0; i < f_entry_args_count; i++) {
		f_entry_args[i].f_entry = 0;
		f_entry_args[i].f_length = LENGTH_UNKNOWN;
	}
}




static int decimal_value(const mowc::wc_t *&format)
{
	int		digit, value, sign;

	sign = 1;
	if(*format == '+') {
		format++;
	}
	else if(*format == '-') {
		sign = -1;
		format++;
	}

	value = 0;
	digit = mowc::digit(*format);
	if(digit > 0) {
		do {
			value = value * 10 + digit;
			format++;
			digit = mowc::digit(*format);
		} while(digit >= 0);
		// the returned value can never be zero (unless there is an overflow)
		return value * sign;
	}

	return 0;
}


void put_char(mowc::wc_t c)
{
	if(f_out != 0) {
		f_out->Put(c);
	}
	f_count++;
}



void setref(int pos, length_t l)
{
	// the user has to give a position starting from 1
	pos--;

	if((unsigned int) pos >= (unsigned int) f_entry_args_count) {
		// overflow, we can't do anything more!
		f_bad = true;
		return;
	}

	if(f_entry_args[pos].f_entry != 0) {
		if(f_entry_args[pos].f_length != l) {
			f_bad = true;
			return;
		}
	}
	else {
		f_entry_args[pos].f_entry = f_current;
		f_entry_args[pos].f_length = l;
		f_entry_args_max = pos + 1;
	}

	// +1 because we did a -1 and +1 because we're done here
	f_pos = pos + 2;
}



const mowc::wc_t *parse_conversion(const mowc::wc_t *format)
{
	int			value;

#if 0
{
const mowc::wc_t *s;
s = format;
printf("Format: [");
while(*s != '\0') {
	printf("%c", *s++);
}
printf("]\n");
}
#endif

// ARG POSITION (could also be the width)
	if(*format >= '1' && *format <= '9') {
		// width or arg_pos
		value = decimal_value(format);
		if(*format != '$') {
			f_current->f_width = value;
			goto check_precision;
		}
		// that's the position of an argument to read
		format++;
		f_current->f_arg_pos = value;
	}


// FLAGS
	for(;;) {
		switch(*format) {
		case '#':
			f_current->f_introducer = 1;
			break;

		case '0':
			f_current->f_zero_pad = 1;
			break;

		case '-':
			f_current->f_left_align = 1;
			break;

		case '\'':
			f_current->f_thousands = 1;
			break;

		case ' ':
			f_current->f_space = 1;
			break;

		case '+':
			// if f_sign is true, ignore f_space
			f_current->f_sign = 1;
			break;

		default:
			goto not_a_flag;

		}
		format++;
	}

not_a_flag:

// variable WIDTH
	switch(*format) {
	case '*':
		format++;
		switch(*format) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '+':
		case '-':
			value = decimal_value(format);
			if(*format == '$') {
				format++;
			}
			else {
				// it's actually an error if the '$' is not found...
				f_current->bad();
			}
			break;

		default:
			value = f_pos;
			break;

		}
		f_current->f_width_pos = value;
		setref(value, LENGTH_WIDTH);
		break;

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		f_current->f_width = decimal_value(format);
		break;

	}

// PRECISION
check_precision:
	if(*format == '.') {
		format++;
		if(*format == '*') {
			format++;
			switch(*format) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '+':
				value = decimal_value(format);
				if(*format == '$') {
					format++;
				}
				else {
					// it's actually an error if the '$' is not found...
					f_current->bad();
				}
				break;

			default:
				value = f_pos;
				break;

			}
			f_current->f_prec_pos = value;
			setref(value, LENGTH_PRECISION);
		}
		else {
			value = decimal_value(format);
			f_current->f_precision = value < 0 ? 0 : value;
		}
	}


// SIZES
	switch(*format) {
	case 'h':
		format++;
		if(*format == 'h') {
			// got 'hh'
			f_current->f_length = LENGTH_CHAR;
			format++;
		}
		else {
			// got 'h'
			f_current->f_length = LENGTH_SHORT;
		}
		break;

	case 'l':
		format++;
		if(*format == 'l') {
			// got 'll'
			f_current->f_length = LENGTH_LONG_LONG;
			format++;
		}
		else {
			// got 'l'
			f_current->f_length = LENGTH_LONG;
		}
		break;

#ifndef MO_CONFIG_NO_LONG_DOUBLE
	case 'L':
		format++;
		f_current->f_length = LENGTH_LONG_DOUBLE;
		break;
#endif

	case 'q':
		format++;
		f_current->f_length = LENGTH_LONG_LONG;
		break;

	case 'j':
		format++;
		f_current->f_length = LENGTH_INTMAX;
		break;

	case 'z':
	case 'Z':		// deprecated
		format++;
		f_current->f_length = LENGTH_SIZE;
		break;

	case 't':
		format++;
		f_current->f_length = LENGTH_PTRDIFF;
		break;

	}
	if(f_current->f_length == LENGTH_UNKNOWN) {
		f_current->f_length = LENGTH_INTEGER;
	}

// CONVERSION SPECIFIER
	switch(*format) {
	case 'D':		// deprecated
		f_current->f_length = LENGTH_LONG;
	case 'd':
	case 'i':
		f_current->conversion(CONVERSION_INTEGER);
		break;

	case 'U':		// deprecated
		f_current->f_length = LENGTH_LONG;
	case 'u':
		f_current->conversion(CONVERSION_UNSIGNED);
		break;

	case 'O':		// deprecated
		f_current->f_length = LENGTH_LONG;
	case 'o':
		f_current->conversion(CONVERSION_OCTAL);
		break;

	case 'X':
		f_current->f_uppercase = 1;
	case 'x':
		f_current->conversion(CONVERSION_HEXADECIMAL);
		break;

	case 'E':
		f_current->f_uppercase = 1;
	case 'e':
		f_current->conversion(CONVERSION_SCIENTIFIC);
		break;

	case 'F':		// deprecated
	case 'f':
		f_current->conversion(CONVERSION_FLOAT);
		break;

	case 'G':
		f_current->f_uppercase = 1;
	case 'g':
		f_current->conversion(CONVERSION_SELECT);
		break;

	case 'A':
		f_current->f_uppercase = 1;
	case 'a':
		f_current->conversion(CONVERSION_HEXAFLOAT);
		break;

	case 'C':		// deprecated
		f_current->f_length = LENGTH_CHAR;
		f_current->conversion(CONVERSION_CHAR);
		break;

	case 'c':
		if(f_current->f_length == LENGTH_INTEGER) {
			f_current->f_length = LENGTH_LONG_LONG;		// wc_t by default
		}
		f_current->conversion(CONVERSION_CHAR);
		break;

	case 'S':		// deprecated
		f_current->f_length = LENGTH_CHAR;
		f_current->conversion(CONVERSION_STRING);
		break;

	case 's':
		if(f_current->f_length == LENGTH_INTEGER) {
			f_current->f_length = LENGTH_LONG_LONG;		// wc_t by default
		}
		f_current->conversion(CONVERSION_STRING);
		break;

	case 'P':		// m2osw specific (capital P)
		f_current->f_uppercase = 1;
	case 'p':
		f_current->conversion(CONVERSION_POINTER);
		break;

	case 'n':
		f_current->conversion(CONVERSION_COUNT);
		break;

	case 'm':
		f_current->conversion(CONVERSION_STRERROR);
		break;

	default:
		f_current->bad();
		return format;

	}

// if position not defined yet, we define it now
	if(f_current->f_arg_pos < 0) {
		value = f_pos;
	}
	else {
		value = f_current->f_arg_pos;
	}

	setref(value, f_current->f_length);

	return format + 1;
}



void parse_entries(void)
{
	const mowc::wc_t	*format;

	f_current = f_entry;
	f_pos = 1;

	format = f_format;
	while(*format != '\0') {
		if(format[0] == '%') {
			if(format[1] != '%') {
				// this is a conversion format
				f_current->f_start = format + 1;
				format = parse_conversion(format + 1);
				f_current->f_end = format;
			}
			else {
				// add one '%'
				f_current->conversion(CONVERSION_DIRECT);
				f_current->f_start = format;
				f_current->f_end = format + 1;
				format += 2;
			}
		}
		else {
			// anything except '%'
			f_current->conversion(CONVERSION_DIRECT);
			f_current->f_start = format;
			do {
				format++;
			} while(*format != '\0' && *format != '%');
			f_current->f_end = format;
		}
		f_current++;
	}
}


void read_args(void)
{
	int		i;

	// once all the entries are parsed, we can safely read
	// the arguments since now we have their positional order
	for(i = 0; i < f_entry_args_max; i++) {
		if(f_entry_args[i].f_entry == NULL) {
			// missing entry, we cannot safely read the args!
			f_bad = true;
			return;
		}
		if(f_entry_args[i].f_length == LENGTH_WIDTH) {
			f_entry_args[i].f_entry->f_width = va_arg(f_args, int);
		}
		else if(f_entry_args[i].f_length == LENGTH_PRECISION) {
			f_entry_args[i].f_entry->f_precision = va_arg(f_args, int);
		}
		else switch((f_entry_args[i].f_length << 8) | f_entry_args[i].f_entry->f_conversion) {
		case (LENGTH_CHAR << 8) | CONVERSION_INTEGER:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, int /*signed char*/ );
			break;

		case (LENGTH_CHAR << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_CHAR << 8) | CONVERSION_OCTAL:
		case (LENGTH_CHAR << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, int /*unsigned char*/ );
			break;

		case (LENGTH_CHAR << 8) | CONVERSION_CHAR:
			f_entry_args[i].f_entry->f_value.f_char = va_arg(f_args, int /*unsigned char*/ );
			break;

		case (LENGTH_CHAR << 8) | CONVERSION_STRING:
			f_entry_args[i].f_entry->f_value.f_ptr = va_arg(f_args, unsigned char *);
			break;

		case (LENGTH_SHORT << 8) | CONVERSION_INTEGER:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, int /*short*/ );
			break;

		case (LENGTH_SHORT << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_SHORT << 8) | CONVERSION_OCTAL:
		case (LENGTH_SHORT << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, int /*unsigned short*/ );
			break;

		case (LENGTH_SHORT << 8) | CONVERSION_CHAR:
			f_entry_args[i].f_entry->f_value.f_mc = va_arg(f_args, int /*mowc::mc_t*/ );
			break;

		case (LENGTH_SHORT << 8) | CONVERSION_STRING:
			f_entry_args[i].f_entry->f_value.f_ptr = va_arg(f_args, mowc::mc_t *);
			break;

		case (LENGTH_INTEGER << 8) | CONVERSION_INTEGER:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, signed int);
			break;

		case (LENGTH_INTEGER << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_INTEGER << 8) | CONVERSION_OCTAL:
		case (LENGTH_INTEGER << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, unsigned int);
			break;

		case (LENGTH_LONG << 8) | CONVERSION_CHAR:
			// wchar_t should be unsigned (we can't qualify it that way...)
			f_entry_args[i].f_entry->f_value.f_wchar = va_arg(f_args, int /*wchar_t*/ );
			break;

		case (LENGTH_LONG << 8) | CONVERSION_STRING:
			f_entry_args[i].f_entry->f_value.f_ptr = va_arg(f_args, wchar_t *);
			break;

		case (LENGTH_LONG << 8) | CONVERSION_INTEGER:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, signed long);
			break;

		case (LENGTH_LONG << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_LONG << 8) | CONVERSION_OCTAL:
		case (LENGTH_LONG << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, unsigned long);
			break;

		case (LENGTH_LONG_LONG << 8) | CONVERSION_INTEGER:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, int64_t);
			break;

		case (LENGTH_LONG_LONG << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_LONG_LONG << 8) | CONVERSION_OCTAL:
		case (LENGTH_LONG_LONG << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, uint64_t);
			break;

		case (LENGTH_LONG_LONG << 8) | CONVERSION_CHAR:
			f_entry_args[i].f_entry->f_value.f_wc = va_arg(f_args, mowc::wc_t);
			break;

		case (LENGTH_LONG_LONG << 8) | CONVERSION_STRING:
			f_entry_args[i].f_entry->f_value.f_ptr = va_arg(f_args, mowc::wc_t *);
			break;

		case (LENGTH_INTMAX << 8) | CONVERSION_INTEGER:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, intmax_t);
			break;

		case (LENGTH_INTMAX << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_INTMAX << 8) | CONVERSION_OCTAL:
		case (LENGTH_INTMAX << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, uintmax_t);
			break;

		case (LENGTH_SIZE << 8) | CONVERSION_INTEGER:
			// a size_t is always unsigned...
			f_entry_args[i].f_entry->f_conversion = CONVERSION_UNSIGNED;
		case (LENGTH_SIZE << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_SIZE << 8) | CONVERSION_OCTAL:
		case (LENGTH_SIZE << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, size_t);
			break;

		case (LENGTH_PTRDIFF << 8) | CONVERSION_INTEGER:
		case (LENGTH_PTRDIFF << 8) | CONVERSION_UNSIGNED:
		case (LENGTH_PTRDIFF << 8) | CONVERSION_OCTAL:
		case (LENGTH_PTRDIFF << 8) | CONVERSION_HEXADECIMAL:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, std::ptrdiff_t);
			break;

		// NOTE: the LENGTH_INTEGER is actually the default size
		case (LENGTH_INTEGER << 8) | CONVERSION_SCIENTIFIC:
		case (LENGTH_INTEGER << 8) | CONVERSION_FLOAT:
		case (LENGTH_INTEGER << 8) | CONVERSION_SELECT:
		case (LENGTH_INTEGER << 8) | CONVERSION_HEXAFLOAT:
			f_entry_args[i].f_entry->f_value.f_double = va_arg(f_args, double);
			break;

#ifndef MO_CONFIG_NO_LONG_DOUBLE
		case (LENGTH_LONG_DOUBLE << 8) | CONVERSION_SCIENTIFIC:
		case (LENGTH_LONG_DOUBLE << 8) | CONVERSION_FLOAT:
		case (LENGTH_LONG_DOUBLE << 8) | CONVERSION_SELECT:
		case (LENGTH_LONG_DOUBLE << 8) | CONVERSION_HEXAFLOAT:
			f_entry_args[i].f_entry->f_value.f_long_double = va_arg(f_args, long double);
			break;
#endif

		case (LENGTH_INTEGER << 8) | CONVERSION_POINTER:
			f_entry_args[i].f_entry->f_value.f_intmax = reinterpret_cast<intmax_t>(va_arg(f_args, void *));
			break;

		case (LENGTH_CHAR      << 8) | CONVERSION_COUNT:
		case (LENGTH_SHORT     << 8) | CONVERSION_COUNT:
		case (LENGTH_INTEGER   << 8) | CONVERSION_COUNT:
		case (LENGTH_LONG      << 8) | CONVERSION_COUNT:
		case (LENGTH_LONG_LONG << 8) | CONVERSION_COUNT:
		case (LENGTH_INTMAX    << 8) | CONVERSION_COUNT:
		case (LENGTH_SIZE      << 8) | CONVERSION_COUNT:
			f_entry_args[i].f_entry->f_value.f_ptr = va_arg(f_args, void *);
			break;

		case (LENGTH_INTEGER << 8) | CONVERSION_STRERROR:
			f_entry_args[i].f_entry->f_value.f_intmax = va_arg(f_args, int);
			break;

		default:
			f_bad = true;
			break;

		}
	}
}

	typedef void (printf_info_t::*conversion_func_t)(const entry_t *entry);




void write_unknown(const entry_t *entry)
{
	// do nothing
}

void write_direct(const entry_t *entry)
{
	const mowc::wc_t	*s;

	s = entry->f_start;
	while(s < entry->f_end) {
		put_char(*s);
		s++;
	}
}


void write_int(const entry_t *entry, const char *introducer, const char *format, bool is_unsigned)
{
	// the maxium is 19 characters for a signed integer on 64 bits
	// with the format of %d and 64 bits computers

	char		*s, *d, buf[32];	/* Flawfinder: ignore */
	long		i, l, r, z, m, width;

	// determine whether a sign should be included
	d = buf;
	if(entry->f_value.f_intmax >= 0 && !is_unsigned) {
		if(entry->f_sign == 1) {
			*d++ = '+';
		}
		else if(entry->f_space == 1 && entry->f_left_align == 1) {
			*d++ = ' ';
		}
	}

	if(entry->f_precision == 0 && entry->f_value.f_intmax == 0) {
		*d = '\0';
	}
	else {
		snprintf(d, sizeof(buf) - 2, format, entry->f_value.f_intmax);	/* Flawfinder: ignore */
		buf[sizeof(buf) - 1] = '\0';
	}

	m = (long) strlen(buf);			/* Flawfinder: ignore */
	i = (long) strlen(introducer);		/* Flawfinder: ignore */
	if(entry->f_thousands == 1 && !is_unsigned && m > 3) {
		// we want to insert the thousand separator
		// TODO: should we get the local separator?!?
		// Is this correct at all?!? (should it be supported for integers?)
		l = m + (m - 1) / 3;
		if(l < (long) sizeof(buf) - 1) {
			// NOTE: l should always be smaller than sizeof(buf) - 1
			s = buf + m;
			d = buf + l;
			while(m > 3) {
				d[-1] = s[-1];
				d[-2] = s[-2];
				d[-3] = s[-3];
				d[-4] = ',';
				d -= 4;
				s -= 3;
				m -= 3;
			}
			if(m >= 3) {
				d[-2] = s[-2];
			}
			if(m >= 2) {
				d[-2] = s[-2];
			}
			d[-1] = s[-1];
			m = l;
		}
	}

	width = moMax(entry->f_width, entry->f_precision);
	if(width < 0) {
		width = 0;
	}

	// determine the number of spaces & zeroes on the left, and spaces on the right
	if(entry->f_left_align == 1) {
		l = 0;
		z = m < entry->f_precision ? entry->f_precision : 0;
		if(z > 0 && i == 1) {
			i = 0;		// no need for an extra octal introducer
		}
		r = m < width - z - i ? width - z - m - i : 0;
	}
	else {
		if(entry->f_zero_pad == 1 && entry->f_precision < 0) {
			if(i == 1) {
				// we need to have at least 1 in z
				z = m < width ? width - m : 1;
				i = 0;
			}
			else {
				z = m < width - i ? width - i - m : 0;
			}
			l = r = 0;
		}
		else {
			z = m < entry->f_precision ? entry->f_precision : 0;
			if(z > 0 && i == 1) {
				i = 0;		// no need for an extra octal introducer
			}
			l = m < width - z - i ? width - z - m - i : 0;
			if(entry->f_sign == 0 && entry->f_space == 1 && entry->f_value.f_intmax >= 0 && l == 0) {
				l = 1;
			}
			r = 0;
		}
	}

	while(l > 0) {
		l--;
		put_char(' ');
	}

	// put the sign before all the other digits
	// (and the introducer, though when there is an introducer, there isn't a sign)
	s = buf;
	if(*s == '+' || *s == '-' || *s == ' ') {
		put_char(*s);
		s++;
	}

	while(i > 0) {
		i--;
		put_char(*introducer);
		introducer++;
	}

	while(z > 0) {
		z--;
		put_char('0');
	}

	//s = buf; -- we already skipped introducers, don't start from the start again!
	while(*s != '\0') {
		put_char(*s);
		s++;
	}

	while(r > 0) {
		r--;
		put_char(' ');
	}
}


void write_integer(const entry_t *entry)
{
	write_int(entry, "", "%lld", false);
}


void write_unsigned(const entry_t *entry)
{
	write_int(entry, "", "%llu", true);
}


void write_octal(const entry_t *entry)
{
	if(entry->f_introducer == 1) {
		write_int(entry, "0", "%llo", true);
	}
	else {
		write_int(entry, "", "%llo", true);
	}
}


void write_hexadecimal(const entry_t *entry)
{
	switch(entry->f_introducer * 2 + entry->f_uppercase) {
	case 0:
		write_int(entry, "", "%llx", true);
		break;

	case 1:
		write_int(entry, "", "%llX", true);
		break;

	case 2:
		write_int(entry, "0x", "%llx", true);
		break;

	case 3:
		write_int(entry, "0X", "%llX", true);
		break;

	}
}


void write_flt(const entry_t *entry, char format, double base)
{
	int		max;
	char		*r, *s, buf[256], *f, fmt[256];		/* Flawfinder: ignore */
	moBuffer	buffer;

#ifndef MO_CONFIG_NO_LONG_DOUBLE
	if(entry->f_length == LENGTH_LONG_DOUBLE) {
		max = moMax(entry->f_width, entry->f_precision, (int) (log(entry->f_value.f_long_double) / log(base) + 8.0)) + 16;
	}
	else {
#endif
		max = moMax(entry->f_width, entry->f_precision, (int) (log(entry->f_value.f_double) / log(base) + 8.0)) + 16;
#ifndef MO_CONFIG_NO_LONG_DOUBLE
	}
#endif

	if(max > (long) sizeof(buf)) {
		// in 99% of the cases, this will be unused
		buffer.SetSize(max * sizeof(buf[0]));
		s = reinterpret_cast<char *>(static_cast<void *>(buffer));
	}
	else {
		s = buf;
		max = sizeof(buf);
	}

	if(entry->f_uppercase == 1) {
		format &= 0x5F;
	}

	f = fmt + snprintf(fmt, sizeof(fmt) - 2, "%%%s%s%s",	/* Flawfinder: ignore */
		entry->f_introducer == 1 ? "#" : "",
#ifndef MO_CONFIG_NO_LONG_DOUBLE
		entry->f_length == LENGTH_LONG_DOUBLE ? "l" : "",
#else
		"",
#endif
		entry->f_sign == 1 ? "+" : (entry->f_space == 1 ? " " : ""));
	if(entry->f_width > 0) {
		f += snprintf(f, sizeof(fmt) - 2 - (f - fmt), "%d", entry->f_width);		/* Flawfinder: ignore */
	}
	if(entry->f_precision >= 0) {
		f += snprintf(f, sizeof(fmt) - 2 - (f - fmt), ".%d", entry->f_precision);	/* Flawfinder: ignore */
	}
	*f++ = format;
	*f = '\0';

#ifndef MO_CONFIG_NO_LONG_DOUBLE
	if(entry->f_length == LENGTH_LONG_DOUBLE) {
		snprintf(s, max, fmt, entry->f_value.f_long_double);	/* Flawfinder: ignore */
	}
	else {
#endif
		snprintf(s, max, fmt, entry->f_value.f_double);	/* Flawfinder: ignore */
#ifndef MO_CONFIG_NO_LONG_DOUBLE
	}
#endif

	r = s;
	while(*r != '\0') {
		put_char(*r);
		r++;
	}
}


void write_scientific(const entry_t *entry)
{
	write_flt(entry, 'e', 10.0);
}


void write_float(const entry_t *entry)
{
	write_flt(entry, 'f', 10.0);
}


void write_select(const entry_t *entry)
{
	write_flt(entry, 'g', 10.0);
}


void write_hexafloat(const entry_t *entry)
{
	write_flt(entry, 'a', 16.0);
}



void write_char(const entry_t *entry)
{
	int		l;

	l = entry->f_length;

	if(f_swap) {
		if(l == LENGTH_CHAR) {
			l = LENGTH_LONG_LONG;
		}
		else if(l == LENGTH_LONG_LONG) {
			l = LENGTH_CHAR;
		}
	}

	switch(l) {
	case LENGTH_CHAR:
		// TODO: we certainly need a conversion using some specified encoding?
		put_char(entry->f_value.f_char);
		break;

	case LENGTH_SHORT:
		// TODO: we certainly need a conversion using some specified encoding?
		put_char(entry->f_value.f_mc);
		break;

	case LENGTH_LONG:
		// NOTE: on MS-Windows the wchar_t is only 16 bits...
		put_char(entry->f_value.f_wchar);
		break;

	case LENGTH_LONG_LONG:
		// TODO: we certainly need a conversion using some specified encoding?
		put_char(entry->f_value.f_wc);
		break;

	default:; // avoid warnings
	}
}


void write_string(const entry_t *entry)
{
	int		l, length, max, width;
	const void	*start;

	max = entry->f_precision;
	if(max == 0) {
		return;
	}
	if(max < 0) {
		max = INT_MAX;
	}

	width = entry->f_width;

	if(entry->f_value.f_ptr == 0) {
		if(max < 6) {
			start = "null";
			length = moMin(4, max);
		}
		else {
			start = "(null)";
			length = moMin(6, max);
		}
		l = LENGTH_CHAR;
	}
	else {
		start = entry->f_value.f_ptr;
		length = 0;	// unknown length

		l = entry->f_length;
		if(f_swap) {
			if(l == LENGTH_CHAR) {
				l = LENGTH_LONG_LONG;
			}
			else if(l == LENGTH_LONG_LONG) {
				l = LENGTH_CHAR;
			}
		}

		if(width > 0) {
			// compute the length
			switch(l) {
			case LENGTH_CHAR:
			{
				// TODO: we certainly need a conversion using some specified encoding?
				const char *s = reinterpret_cast<const char *>(start);
				while(*s != '\0' && length < max) {
					s++;
					length++;
				}
			}
				break;

			case LENGTH_SHORT:
			{
				// TODO: we certainly need a conversion using some specified encoding?
				const mowc::mc_t *s = reinterpret_cast<const mowc::mc_t *>(start);
				while(*s != '\0' && length < max) {
					s++;
					length++;
				}
			}
				break;

			case LENGTH_LONG:
			{
				// NOTE: on MS-Windows the wchar_t is only 16 bits...
				const wchar_t *s = reinterpret_cast<const wchar_t *>(start);
				while(*s != '\0' && length < max) {
					s++;
					length++;
				}
			}
				break;

			case LENGTH_LONG_LONG:
			{
				// TODO: we certainly need a conversion using some specified encoding?
				const mowc::wc_t *s = reinterpret_cast<const mowc::wc_t *>(start);
				while(*s != '\0' && length < max) {
					s++;
					length++;
				}
			}
				break;

			default:; // avoid warnings
			}
		}
	}

	if(width > length && entry->f_left_align == 0) {
		while(width > length) {
			put_char(' ');
			length++;
		}
	}

	switch(l) {
	case LENGTH_CHAR:
	{
		// TODO: we certainly need a conversion using some specified encoding?
		const char *s = reinterpret_cast<const char *>(start);
		while(*s != '\0' && max > 0) {
			put_char(*s);
			s++;
			max--;
		}
	}
		break;

	case LENGTH_SHORT:
	{
		// TODO: we certainly need a conversion using some specified encoding?
		const mowc::mc_t *s = reinterpret_cast<const mowc::mc_t *>(start);
		while(*s != '\0' && max > 0) {
			put_char(*s);
			s++;
			max--;
		}
	}
		break;

	case LENGTH_LONG:
	{
		// NOTE: on MS-Windows the wchar_t is only 16 bits...
		const wchar_t *s = reinterpret_cast<const wchar_t *>(start);
		while(*s != '\0' && max > 0) {
			put_char(*s);
			s++;
			max--;
		}
	}
		break;

	case LENGTH_LONG_LONG:
	{
		// TODO: we certainly need a conversion using some specified encoding?
		const mowc::wc_t *s = reinterpret_cast<const mowc::wc_t *>(start);
		while(*s != '\0' && max > 0) {
			put_char(*s);
			s++;
			max--;
		}
	}
		break;

	default:; // avoid warnings
	}

	if(width > length && entry->f_left_align == 1) {
		while(width > length) {
			put_char(' ');
			length++;
		}
	}
}



void write_pointer(const entry_t *entry)
{
	if(entry->f_uppercase == 1) {
		write_int(entry, "0X", "%llX", true);
	}
	else {
		write_int(entry, "0x", "%llx", true);
	}
}



void write_count(const entry_t *entry)
{
	switch(entry->f_length) {
	case LENGTH_CHAR:
		* reinterpret_cast<char *>(entry->f_value.f_ptr) = f_count;
		break;

	case LENGTH_SHORT:
		* reinterpret_cast<short *>(entry->f_value.f_ptr) = f_count;
		break;

	case LENGTH_INTEGER:
		* reinterpret_cast<int *>(entry->f_value.f_ptr) = f_count;
		break;

	case LENGTH_LONG:
		* reinterpret_cast<long *>(entry->f_value.f_ptr) = f_count;
		break;

	case LENGTH_LONG_LONG:
		* reinterpret_cast<int64_t *>(entry->f_value.f_ptr) = f_count;
		break;

	case LENGTH_INTMAX:
		* reinterpret_cast<intmax_t *>(entry->f_value.f_ptr) = f_count;
		break;

	case LENGTH_SIZE:
		* reinterpret_cast<size_t *>(entry->f_value.f_ptr) = f_count;
		break;

	default:; // avoid warnings
	}
}


void write_strerror(const entry_t *entry)
{
	char		*s;

	// TODO: in a multi-thread environment we need to protect the
	//	 returned string!
	s = strerror(static_cast<int>(entry->f_value.f_intmax));
	while(*s != '\0') {
		put_char(*s);
		s++;
	}
}




void write(void)
{
	static const conversion_func_t		funcs[] = {
		&printf_info_t::write_unknown,
		&printf_info_t::write_direct,		// no % format (or invalid % format)
		&printf_info_t::write_integer,		// %d
		&printf_info_t::write_unsigned,		// %u
		&printf_info_t::write_octal,		// %o
		&printf_info_t::write_hexadecimal,	// %x
		&printf_info_t::write_scientific,	// %e
		&printf_info_t::write_float,		// %f
		&printf_info_t::write_select,		// %g
		&printf_info_t::write_hexafloat,	// %a
		&printf_info_t::write_char,		// %c
		&printf_info_t::write_string,		// %s
		&printf_info_t::write_pointer,		// %p
		&printf_info_t::write_count,		// %n
		&printf_info_t::write_strerror		// %m
	};

	size_t		max;
	const entry_t	*e;

	max = f_current - f_entry;
	e = f_entry;
	while(max > 0) {
		(this->*funcs[e->f_conversion])(e);
		e++;
		max--;
	}
}


int parse(bool swap, moOStream *out, const mowc::wc_t *format, va_list args)
{
	f_swap = swap;

	// we must reset the counter each time we're called
	f_count = 0;

	// it is possible to call the function once without output string to
	// only count the number of characters which are to be output
	f_out = out;

	if(format != 0) {
		f_format = format;
		va_copy(f_args, args);

		count_percents();
		setup_entries();
		parse_entries();
		if(!f_bad) {
			read_args();
			if(!f_bad) {
				write();
			}
		}
	}

	//put_char('\0'); -- we don't include the null terminator here
	//because we're actually writing in a stream, not a string. If
	//a string object uses this function, then you have to add the
	//null terminator yourselve.

	return f_count;
}


};


int mowc::vfwprintf(moOStream *out, const wc_t *format, va_list args)	/* Flawfinder: ignore */
{
	printf_info_t	info;

	errno = 0;
	return info.parse(false, out, format, args);
}


int mowc::fwprintf(moOStream *out, const wc_t *format, ...)	/* Flawfinder: ignore */
{
	va_list		args;
	int		result;

	va_start(args, format);
	result = vfwprintf(out, format, args);		/* Flawfinder: ignore */
	va_end(args);

	return result;
}



int mowc::vfprintf(moOStream *out, const char *format, va_list args)	/* Flawfinder: ignore */
{
	printf_info_t	info;
	wc_t		*fmt, *s, buf[256];
	size_t		l;
	moBuffer	buffer;

	if(format == 0) {
		return 0;
	}

	// we make a temporary copy of the format so it is in wc_t
	// instead of char
	l = strlen(format) + 1;		/* Flawfinder: ignore */
	// TODO: we certainly would need an encoding for this function
	if(l <= 256) {
		s = fmt = buf;
		while(*format != '\0') {
			*s++ = *format++;
		}
	}
	else {
		buffer.SetSize(static_cast<unsigned long>(l * sizeof(buf[0])));
		s = fmt = reinterpret_cast<wc_t *>(static_cast<void *>(buffer));
		while(*format != '\0') {
			*s++ = *format++;
		}
	}
	*s = '\0';

	errno = 0;

	return info.parse(true, out, fmt, args);
}


int mowc::fprintf(moOStream *out, const char *format, ...)	/* Flawfinder: ignore */
{
	va_list		args;
	int		result;

	va_start(args, format);
	result = vfprintf(out, format, args);	/* Flawfinder: ignore */
	va_end(args);

	return result;
}




/************************************************************ DOC:

CLASS

	mowc::moIConv

NAME

	moIConv - initializes an iconv(3) filter
	~moIConv - release resources allocated for this convertor filter

SYNOPSIS

	moIConv(void);
	~moIConv();

DESCRIPTION

	This function creates an international character convertor to use as
	a filter for a I/O stream or some other object supporting the moFIFO
	interface.

	It expects a call to the SetEncodings() function to initialize the
	exact filtering. Without such a call, only a few encodings are internally
	supported as input in an automated manner:

		ISO8859-1		default
		UTF-8			detects a valid UTF-8 sequence
		UTF-16			detects zeroes and 0xFEFF
		UTF-32			detects zeroes and 0xFEFF

	The output will always be UTF-32.

	Reading a string such as encoding="..." can also trigger a specific
	encoding/decoding process.

*/
mowc::moIConv::moIConv(void)
	: moFIFO(moMax(MO_FIFO_DEFAULT_SIZE, MAX_MB_SIZE)),
	  f_output(moMax(MO_FIFO_DEFAULT_SIZE, MAX_MB_SIZE) * sizeof(wc_t) * 2)
{
	f_input_type = MO_ENCODING_UNKNOWN;

	f_input_convertor = (iconv_t) -1;
	f_output_convertor = (iconv_t) -1;

	//f_skip_one_feff = false; -- automatic
}


mowc::moIConv::~moIConv()
{
	if(f_input_convertor != (iconv_t) -1) {
		iconv_close(f_input_convertor);
	}
	if(f_output_convertor != (iconv_t) -1) {
		iconv_close(f_output_convertor);
	}
}


/************************************************************ DOC:

CLASS

	mowc::moIConv

NAME

	Reset - reset the state of the convertor

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	Resets the current state of the convertor.

	This is useful if you want to reuse the same convertor
	again for a different stream of data. This ensures that
	any intermediate state will be cleared.

BUGS

	You can't know whether the function worked or the Lock()
	function failed.

SEE ALSO

	moFIFO::Reset

*/
void mowc::moIConv::Reset(void)
{
	if(!Lock()) {
		return;
	}

	// reset the low level iconv() descriptors
	f_input_type = MO_ENCODING_UNKNOWN;

	if(f_input_convertor != (iconv_t) -1) {
		iconv(f_input_convertor, 0, 0, 0, 0);
		//f_skip_one_feff = true; -- needed here?
	}
	if(f_output_convertor != (iconv_t) -1) {
		iconv(f_output_convertor, 0, 0, 0, 0);
	}

	// also reset the lower level FIFOs
	moFIFO::Reset();
	f_output.Reset();

	Unlock();
}



/************************************************************ DOC:

CLASS

	mowc::moIConv

NAME

	SetEncodings - defines the source and destination encodings

SYNOPSIS

	virtual int SetEncodings(const char *from, const char *to);

PARAMETERS

	from - name of the source encoding
	to - name of the destination encoding

DESCRIPTION

	Defines the input and output encodings to use to parse the
	input.

	By default, the output is defined as UTF-32 and the input
	is guessed from the data being read.

	Using the special name "*" as the input (from) keeps the filter
	in a guessing mode for the input data.

	Using the special name "*" as the output (to) is similar to
	asking for UTF-32.

BUGS

	Changing the encoding format from ASCII to another format
	is safe. Changing the encoding midway from any format other
	than ASCII to another may have unknown side effects. Use
	Reset() between changes.

	If the function fails, the previous in place convertions
	will be lost.

RETURN VALUE

	0 when the function succeeds
	-1 when it fails (unrecognized encoding?)

*/
int mowc::moIConv::SetEncodings(const char *from, const char *to)
{
	if(!Lock()) {
		return -1;
	}

	f_input_type = MO_ENCODING_UNKNOWN;

	if(f_input_convertor != (iconv_t) -1) {
		iconv_close(f_input_convertor);
		f_input_convertor = (iconv_t) -1;
	}
	if(f_output_convertor != (iconv_t) -1) {
		iconv_close(f_output_convertor);
		f_output_convertor = (iconv_t) -1;
	}

	if(::strcmp(from, "*") != 0) {
		// note: when 'from' == UCS-4/UTF-32 then we could avoid this
		f_input_convertor = iconv_open(g_ucs4_internal, from);
		if(f_input_convertor == (iconv_t) -1) {
			// this is usually an equivalent
			f_input_convertor = iconv_open(g_utf32_internal, from);
		}
		if(f_input_convertor == (iconv_t) -1) {
			Unlock();
			return -1;
		}
		// the iconv() function will automatically generate
		// an 0xfeff code first and we usually don't want it
		f_skip_one_feff = true;
	}

	if(::strcmp(to, "*") != 0) {
		// note: when 'to' == UCS-4/UTF-32 then we could avoid this
		f_output_convertor = iconv_open(to, g_ucs4_internal);
		if(f_output_convertor == (iconv_t) -1) {
			// this is usually an equivalent
			f_output_convertor = iconv_open(to, g_utf32_internal);
		}
		if(f_output_convertor == (iconv_t) -1) {
			if(f_input_convertor != (iconv_t) -1) {
				iconv_close(f_input_convertor);
				f_input_convertor = (iconv_t) -1;
			}
			Unlock();
			return -1;
		}
	}

	Unlock();

	return 0;
}



/************************************************************ DOC:

CLASS

	mowc::moIConv

NAME

	Read - the filter convertion always happens on read

SYNOPSIS

	virtual int Read(void *buffer, unsigned long size, bool peek = false);

PARAMETERS

	buffer - where the data will be written for the user to use
	size - the number of bytes the user is interested in
	peek - whether the position pointer will be moved

DESCRIPTION

	This overloaded virtual function is the one taking care
	of the filtering. We expect that the user wrote data in the FIFO
	before the Read() is being called. This function reads that
	data, converts it as defined by the encoding currently in
	force and writes it in the user buffer.

	It handles many cases or "too many bytes converted" and
	not enough data found in the source buffer.

	In any event the call never blocks and returns the number
	of bytes transformed.

BUGS

	Not writing enough bytes in the FIFO will results in no characters
	being output here. It is important to know whether the end of
	the stream was not reached yet and data is missing or the convertion
	of the last sequence was not possible.

RETURN VALUE

	the number of bytes written in the user buffer when the function
	succeeds; this can be zero when nothing can be written or the end
	of the input is found

	and -1 when an error occurs

*/
int mowc::moIConv::Read(void *buffer, unsigned long size, bool peek)
{
	wc_t		s[(MO_FIFO_DEFAULT_SIZE > MAX_MB_SIZE ? MO_FIFO_DEFAULT_SIZE : MAX_MB_SIZE) * 2];
	wc_t		w[MO_FIFO_DEFAULT_SIZE > MAX_MB_SIZE ? MO_FIFO_DEFAULT_SIZE : MAX_MB_SIZE];
	wc_t		*d, *in, *out, c;
	unsigned char	*p;
	size_t		in_sz, out_sz;
	long		sz;
	int		total;

	// first we read all the left over in the output FIFO
	sz = f_output.Read(buffer, size, peek);
	if(sz < 0) {
		return sz;
	}

	total = sz;
	size -= sz;
	if(size == 0) {		// finished?

#if 0
if(sz == 4) {
fprintf(stderr, "Read %08lX\n", * (long *) buffer);
}
#endif

		return total;
	}

	// skip what was already read
	buffer = static_cast<char *>(buffer) + sz;

	while(size != 0) {
		sz = moFIFO::Read(s, moMax(MO_FIFO_DEFAULT_SIZE, MAX_MB_SIZE), true);
		if(sz <= 0) {
			return total;
			//return peek || total == 0 ? -1 : total;
		}

		if(f_input_convertor == (iconv_t) -1) {
			p = reinterpret_cast<unsigned char *>(s);
			if(f_input_type == MO_ENCODING_UNKNOWN) {
				if((p[0] !=    0 && p[1] ==    0 && p[2] != 0 && p[3] == 0)
				|| (p[0] == 0xFF && p[1] == 0xFE && p[2] != 0 && p[3] == 0)) {
					f_input_type = MO_ENCODING_UTF16_LE;
				}
				else if((p[0] ==    0 && p[1] !=    0 && p[2] == 0 && p[3] != 0)
				     || (p[0] == 0xFE && p[1] == 0xFF && p[2] == 0 && p[3] != 0)) {
					f_input_type = MO_ENCODING_UTF16_BE;
				}
				else if((p[0] == 0xFF && p[1] == 0xFE && p[2] == 0 && p[3] == 0)
				     || (p[0] !=    0 && p[1] ==    0 && p[2] == 0 && p[3] == 0)) {
					f_input_type = MO_ENCODING_UTF32_LE;
				}
				else if((p[0] == 0 && p[1] == 0 && p[2] == 0xFE && p[3] == 0xFF)
				     || (p[0] == 0 && p[1] == 0 && p[2] ==    0 && p[3] !=    0)) {
					f_input_type = MO_ENCODING_UTF32_BE;
				}
				else {
					/*
					 * In this case we assume ISO-8859-1
					 * this is useful to read the starting
					 * comment as if it were read with
					 * ReadByte() calls!
					 */
					f_input_type = MO_ENCODING_ISO8859_1;
				}
			}

			out = w;
			while(sz > 0 && static_cast<size_t>(out - w) < sizeof(w)) {
				switch(f_input_type) {
				case MO_ENCODING_UTF16_LE:
					if(sz < 2) {
						sz = 0;
						break;
					}
					c = moReadLE16Bits(p);
					if(c == 0x0000FFFE) {
						// endian changes
						c = 0x0000FEFF;
						f_input_type = MO_ENCODING_UTF16_BE;
					}
					*out++ = c;
					p += 2;
					sz -= 2;
					break;

				case MO_ENCODING_UTF16_BE:
					if(sz < 2) {
						sz = 0;
						break;
					}
					c = moReadBE16Bits(p);
					if(c == 0x0000FFFE) {
						// endian changes
						c = 0x0000FEFF;
						f_input_type = MO_ENCODING_UTF16_LE;
					}
					*out++ = c;
					p += 2;
					sz -= 2;
					break;

				case MO_ENCODING_UTF32_LE:
					if(sz < 4) {
						sz = 0;
						break;
					}
					c = moReadLE32Bits(p);
					if(c == static_cast<wc_t>(0xFFFE0000)) {
						// endian changes
						c = 0x0000FEFF;
						f_input_type = MO_ENCODING_UTF32_BE;
					}
					*out++ = c;
					p += 4;
					sz -= 4;
					break;

				case MO_ENCODING_UTF32_BE:
					if(sz < 4) {
						sz = 0;
						break;
					}
					c = moReadBE32Bits(p);
					if(c == static_cast<wc_t>(0xFFFE0000)) {
						// endian changes
						c = 0x0000FEFF;
						f_input_type = MO_ENCODING_UTF32_LE;
					}
					*out++ = c;
					p += 4;
					sz -= 4;
					break;

				default:	/* case MO_ENCODING_ISO8859_1: */
					*out++ = p[0];
					p++;
					sz--;
					break;

				}
			}

			// skip what was eaten by the output
			// (even if the write below fails!)
			moFIFO::Read(s, static_cast<unsigned long>(p - reinterpret_cast<unsigned char *>(s)));

			// compute the output buffer byte size
			sz = static_cast<long>(reinterpret_cast<char *>(out) - reinterpret_cast<char *>(w));
		}
		else {
			d = s;
			do {
				in = d;
				in_sz = sz;
				out = w;
				out_sz = sizeof(w);
				char *in_ptr = reinterpret_cast<char *>(in);
				char *out_ptr = reinterpret_cast<char *>(out);
				iconv(f_input_convertor,
#ifdef MO_CONFIG_CONST_ICONV
						const_cast<const char **>(&in_ptr), &in_sz,
#else
						&in_ptr, &in_sz,
#endif
						&out_ptr, &out_sz);
				in = reinterpret_cast<wc_t *>(in_ptr);
				out = reinterpret_cast<wc_t *>(out_ptr);
				if(out != w) {
					goto save_iconv;
				}
				// either we got a failure or the input sequence
				// is too short... in either case we can't go on
				// with this so we skip one character, reset the
				// state and try again!
				iconv(f_input_convertor, 0, 0, 0, 0);
				d++;
				sz--;
			} while(sz > 0);
			// we failed to do anything...
			return total;
			//return peek || total == 0 ? -1 : total;
save_iconv:

//fprintf(stderr, "mowc::IConv::Read() -- used iconv() [input] -- %08lX\n", * (long *) s);


			// skip what was eaten by the output
			// (even if the write below fails!)
			moFIFO::Read(s, static_cast<unsigned long>(sz - in_sz + d - s));

			// compute the number of bytes generated in sz
			sz = static_cast<long>(sizeof(w) - out_sz);

			if(f_skip_one_feff && sz > 0 && w[0] == 0xFEFF) {
				f_skip_one_feff = false;
				sz -= sizeof(w[0]);
				memmove(w, w + 1, sz);
			}
		}

		if(f_output_convertor != (iconv_t) -1) {
			in = w;
			in_sz = sz;
			out = s;
			out_sz = sizeof(s);
			char *in_ptr = reinterpret_cast<char *>(in);
			char *out_ptr = reinterpret_cast<char *>(out);
			iconv(f_output_convertor,
#ifdef MO_CONFIG_CONST_ICONV
					const_cast<const char **>(&in_ptr), &in_sz,
#else
					&in_ptr, &in_sz,
#endif
					&out_ptr, &out_sz);
			in = reinterpret_cast<wc_t *>(in_ptr);
			out = reinterpret_cast<wc_t *>(out_ptr);
			sz = static_cast<long>(sizeof(s) - out_sz);
			if(sz > 0) {
				out_sz = f_output.Write(s, sz);
//fprintf(stderr, "mowc::IConv::Read() -- used iconv() [output] -- %08lX, %ld vs %ld, left over %ld\n", * (long *) s, (long) out_sz, sz, (long) in_sz);
			}
		}
		else {
			f_output.Write(w, sz);
		}

		// read the result in the user output buffer
		sz = f_output.Read(buffer, size, peek);
		if(sz < 0) {
			// if we read some data, return it -- the
			// next call should return the error again
			// as we're getting here
			return total > 0 ? total : sz;
			//return peek || total == 0 ? -1 : total;
		}
		assert((unsigned long) sz <= size);

		// skip what was already read
		total += sz;
		size -= sz;
		buffer = static_cast<char *>(buffer) + sz;
	}

	return total;
}



};			// namespace molib;

// vim: ts=8

