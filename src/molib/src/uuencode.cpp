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
#pragma implementation "mo/mo_uuencode.h"
#endif

#include	"mo/mo_uuencode.h"


namespace molib
{



/************************************************************ DOC:

NAME

	moUUEncode - encode a binary buffer to ASCII

SYNOPSIS

	void moUUEncode(moWCString& out, const moBuffer& in, int mode, const moWCString& name);

PARAMETERS

	out - the resulting ASCII buffer
	in - the input buffer
	mode - the permissions of the file (like chmod(1))
	name - the name of the file

DESCRIPTION

	This function will encode a binary buffer into an ASCII
	string which is printable in emails. This format is the
	same as the uuencode(1) and uudecode(1) tools understand.

SEE ALSO

	moUUDecode

*/
inline static unsigned char moEncodeChar(unsigned char c)
{
	c &= 0x3F;
	if(c == 0) {
		return '`';
	}

	return c + ' ';
}

void moUUEncode(moWCString& out, const moBuffer& in, int mode, const moWCString& name)
{
	void			*buffer;
	unsigned long		size, sz;
	const unsigned char	*data;

	out = moWCString::Format("begin %o ", mode);
	out += name;
	out += '\n';

	in.Get(buffer, size);
	data = static_cast<const unsigned char *>(buffer);

	while(size > 0) {
		// at most, save 45 bytes on a single line
		sz = size >= 45 ? 45 : size;
		size -= sz;
		// first put the size of this line
		out += moEncodeChar(static_cast<unsigned char>(sz));
		// then encode the line
		while(sz >= 3) {
			out += moEncodeChar(data[0] >> 2);
			out += moEncodeChar((data[0] << 4) | (data[1] >> 4) & 0x0F);
			out += moEncodeChar((data[1] << 2) | (data[2] >> 6) & 0x03);
			out += moEncodeChar(data[2]);
			data += 3;
			sz -= 3;
		}
		switch(sz) {
		case 1:
			// only data[0] is accessible
			out += moEncodeChar(data[0] >> 2);
			out += moEncodeChar(data[0] << 4);
			out += moEncodeChar(0);
			out += moEncodeChar(0);
			break;

		case 2:
			// only data[0] and data[1] are accessible
			out += moEncodeChar(data[0] >> 2);
			out += moEncodeChar((data[0] << 4) | (data[1] >> 4) & 0x0F);
			out += moEncodeChar(data[1] << 2);
			out += moEncodeChar(0);
			break;

		}
		// end the line
		out += '\n';
	}

	// the end of the buffer is marked with an empty line
	// (and empty lines have a size of 0)
	out += moEncodeChar(0);
	out += "\nend\n";
}


/************************************************************ DOC:

NAME

	moUUDecode - decode an ASCII buffer back to binary

SYNOPSIS

	mo_uudecode_error_t moUUDecode(const moWCString& in, moBuffer& out, int& mode, moWCString& name);

PARAMETERS

	in - the input ASCII string
	out - the resulting output buffer
	mode - the permissions of the file as read from the input (like chmod(1))
	name - the name of the file as read from the input

DESCRIPTION

	This function will decode an ASCII buffer back to binary
	This expected input format is the same as the uuencode(1)
	and uudecode(1) tools understand.

	The input string can start with blanks. These will be
	skipped (spaces, new-lines, carriage returns)

RETURN VALUE

	This function returns MO_UUDECODE_ERROR_NONE on success.

	The other error codes are describe below:

	 . MO_UUDECODE_ERROR_NOBEGIN

		The "begin ..." wasn't found.

	 . MO_UUDECODE_ERROR_BADMODE

		The file mode is invalid (not an octal value from
		0 to 07777)

	 . MO_UUDECODE_ERROR_BADNAME

		The filename isn't valid (usually empty.)

	 . MO_UUDECODE_ERROR_EMPTY

		The end of the string was found before the filename.

	 . MO_UUDECODE_ERROR_BADLENGTH

		The length of a line is not valid (too small or large)

	 . MO_UUDECODE_ERROR_BADCHAR

		An encoded character is invalid (not in the acceptable
		range for UUEncode).

	 . MO_UUDECODE_ERROR_BADEND

		The "end" wasn't found before the end of the string.

SEE ALSO

	moUUEncode

*/
inline static bool moDecodeChar(mowc::wc_t c, unsigned char& out)
{
	// valid character?
	if(c <= ' ' || c > 0x60) {
		return false;
	}

	// 0 is represented with ` which is 0x60
	// and the following does (0x60 - 0x20) & 0x3F -> 0x00
	out = (c - ' ') & 0x3F;

	return true;
}

inline static bool IsSpace(mowc::wc_t c)
{
	return c == ' ' || c == '\t' || c == '\f' || c == '\n' || c == '\r'
			|| c == 0xFEFF;
}

mo_uudecode_error_t moUUDecode(const moWCString& in, moBuffer& out, int& mode, moWCString& name)
{
	const mowc::wc_t	*s, *n;
	unsigned char		buf[4], tmp[3], sz;	/* Flawfinder: ignore */
	int			idx;

//fprintf(stderr, "Decoding: [%s]\n", in.MBData());

	out.Empty();
	mode = 0;
	name.Empty();

	// skip spaces at the start
	s = in.Data();
	while(IsSpace(*s)) {
		s++;
	}

	// do we have the begin?
	if(mowc::strcmp(s, "begin ", 6) != 0) {
		return MO_UUDECODE_ERROR_NOBEGIN;
	}

	s += 6;

	// get the mode
	while(*s >= '0' && *s <= '7') {
		mode = (mode << 3) + *s - '0';
		if(mode > 07777) {
			mode = 0;
			return MO_UUDECODE_ERROR_BADMODE;
		}
		s++;
	}
	if(*s != ' ') {
		return MO_UUDECODE_ERROR_BADMODE;
	}

	// we should have only one space, but just in case...
	do {
		s++;
	} while(*s == ' ');

	// what's left on this line is the name of the file
	n = s;
	while(*s != '\n' && *s != '\r' && *s != '\0') {
		s++;
	}
	if(*s == '\0') {
		return MO_UUDECODE_ERROR_EMPTY;
	}

	if(s == n) {
		return MO_UUDECODE_ERROR_BADNAME;
	}

	name.Set(n, static_cast<int>(s - n));

	// now we can read the lines of data
	// these are composed of one character which defines the size
	// followed by that many characters * 4 / 3 defining the data
	// with a maximum of 45 (representing 60 characters on a line)

	for(;;) {
		// skip the end of the line and the \n and \r
		while(*s != '\n' && *s != '\r' && *s != '\0') {
			s++;
		}
		while(*s == '\n' || *s == '\r') {
			s++;
		}

		// reached the end already?! [this shouldn't happen here]
		// NOTE: 'e' would represent a large size than what lines
		// are made of. This means 'end' can't happen in a well
		// formed line and thus the following test is value.
		if(mowc::strcmp(s, "end", 3) == 0) {
			break;
		}
		// get the length
		if(!moDecodeChar(*s, sz)) {
			return MO_UUDECODE_ERROR_BADCHAR;
		}
		s++;
		if(sz == 0) {
			break;
		}
		while(sz >= 3) {
			for(idx = 0; idx < 4; ++idx, ++s) {
				if(!moDecodeChar(*s, buf[idx])) {
					return MO_UUDECODE_ERROR_BADCHAR;
				}
			}
			tmp[0] = (buf[0] << 2) | (buf[1] >> 4);
			tmp[1] = (buf[1] << 4) | (buf[2] >> 2);
			tmp[2] = (buf[2] << 6) | buf[3];
			out.Append(tmp, 3);
			sz -= 3;
		}

		switch(sz) {
		case 1:
			if(!moDecodeChar(*s, buf[0])) {
				return MO_UUDECODE_ERROR_BADCHAR;
			}
			if(!moDecodeChar(*s, buf[1])) {
				return MO_UUDECODE_ERROR_BADCHAR;
			}
			tmp[0] = (buf[0] << 2) | (buf[1] >> 4);
			out.Append(tmp, 1);
			break;

		case 2:
			if(!moDecodeChar(*s, buf[0])) {
				return MO_UUDECODE_ERROR_BADCHAR;
			}
			if(!moDecodeChar(*s, buf[1])) {
				return MO_UUDECODE_ERROR_BADCHAR;
			}
			if(!moDecodeChar(*s, buf[2])) {
				return MO_UUDECODE_ERROR_BADCHAR;
			}
			tmp[0] = (buf[0] << 2) | (buf[1] >> 4);
			tmp[1] = (buf[1] << 4) | (buf[2] >> 2);
			out.Append(tmp, 2);
			break;

		}
	}

	while(IsSpace(*s)) {
		s++;
	}

	if(mowc::strcmp(s, "end", 3) != 0) {
		return MO_UUDECODE_ERROR_BADEND;
	}

	// NOTE: we don't test whether the end is followed by \n

	return MO_UUDECODE_ERROR_NONE;
}




};			// namespace molib;

// vim: ts=8

