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




// The implementation is in image.c++
//#ifdef MO_PRAGMA_INTERFACE
//#pragma implementation "mo/mo_image.h"
//#endif

#include	"mo/mo_image.h"



#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{


/************************************************************ DOC:

CLASS

	moImageFile_PPM

NAME

	Load - load an image from a file
	Save - save an image to a file

SYNOPSIS

	virtual bool Load(moIStream& file, moImage& im) = 0;
	virtual bool Save(moOStream& file, const moImage& im) = 0;

PARAMETERS

	file - a file where the image is read or written
	im - the image loaded or to be saved

DESCRIPTION

	This documentation is supplemental to the main moImageFile
	documentation of the same functions. This is used to give
	a list of all the parameters understood by this handler.

	Load()

	"DEPTH"		"BLACKNWHITE", "GREYSCALE", "RGB"

	Save()

	"DEPTH"		"BLACKNWHITE", "B&W", "BW",
			"GREY", "GREYSCALE", "GRAY", "GRAYSCALE",
			(other values are taken as RGB)

	"COMMENT"	<any user string>

SEE ALSO

	MatchType, MatchPosition, MimeType

*/


class moImageFile_PPM : public moImageFile
{

/*
 * The PPM format uses a text header. This header quickly describs the
 * image file giving a Magic at the beginning of the file followed by
 * the size of the image (width & height). When the image isn't black
 * and white only, then there is also a 'maximum value'. This one will
 * be used to rescale all the read value up to 255.
 *
 * The following describ the formats quickly:
 *
 *	'1'		black and white, ASCII
 *	'2'		grey scale, ASCII
 *	'3'		RGB, ASCII
 *	'4'		black and white, binary
 *	'5'		grey scale, binary
 *	'6'		RGB, ASCII
 *	'7'		assumed 332 RGB palette, binary
 */


int GetC(moIStream& file)
{
	char		c;

	if(file.Read(&c, 1) != 1) {
		return -1;
	}

	while(c == '#') {
		// we found a comment (can be anywhere!)
		do {
			if(file.Read(&c, 1) != 1) {
				return -1;
			}
		} while(c != '\n' && c != '\r');
	}

	return (unsigned char) c;
}


long GetNextC(moIStream& file)
{
	char		c;

	// skip spaces
	do {
		c = GetC(file);
		if(c < 0) {
			return -1;
		}
	} while(isspace(c) || c == '\n' || c == '\r');

	return c;
}


long ReadValue(moIStream& file)
{
	int		c;
	long		result;

	// we must have a digit now
	c = GetNextC(file);
	if(c < '0' || c > '9') {
		return -1;
	}

	result = c - '0';
	c = GetC(file);
	while(c >= '0' && c <= '9') {
		result = result * 10 + c - '0';
		c = GetC(file);
	}

	return result;
}


long GetValue(moIStream& file, long max_value)
{
	long		value;

	value = ReadValue(file);
	if(value <= 0) {
		value = 0;
	}
	else if(value > 255) {
		value = 255;
	}
	else if(max_value != 255) {
		value = value * 255 / max_value;
	}

	return value;
}




bool Load(moIStream& file, moImage& im)
{
	char		buf[3], c;	/* Flawfinder: ignore */
	int		format, width, height, max_value, size, value, idx, x;
	moRGBA		*d;

/* load header */
	if(file.Read(buf, 2) != 2) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}
	if(buf[0] != 'P' || buf[1] < '1' || buf[1] > '6') {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}
	format = buf[1];

	width = ReadValue(file);
	height = ReadValue(file);
	if(format != '1' && format != '4') {
		// black and white images don't have a max_value
		max_value = ReadValue(file);
	}
	else {
		// TODO: we need to set the correct max. value from the format
		max_value = 255;
	}

	if(width <= 0 || height <= 0 || max_value <= 0 || max_value > 255) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	im.SetSize(width, height);

	size = im.Size();
	d = im.Data() + size - width;		// we need to swap vertically
	x = width;
	switch(format) {
	case '1':		// black and white ASCII
		im.Parameters().Set("DEPTH", "BLACKNWHITE");
		do {
			if(x == 0) {
				d -= width * 2;
				x = width;
			}
			value = GetNextC(file);
			if(value == '0') {
				d->red = d->green = d->blue = 255;
			}
			else if(value >= '1' && value <= '9') {
				d->red = d->green = d->blue = 0;
			}
			else {
				// invalid... well, just use a "possible" value
				d->red = d->green = d->blue = 128;
			}
			d->alpha = 255;
			d++;
			x--;
			size--;
		} while(size > 0);
		break;

	case '2':		// grey ASCII
		im.Parameters().Set("DEPTH", "GREYSCALE");
		do {
			if(x == 0) {
				d -= width * 2;
				x = width;
			}
			d->red = d->green = d->blue = static_cast<unsigned char>(GetValue(file, max_value));
			d->alpha = 255;
			d++;
			x--;
			size--;
		} while(size > 0);
		break;

	case '3':		// RGB ASCII
		im.Parameters().Set("DEPTH", "RGB");
		do {
			if(x == 0) {
				d -= width * 2;
				x = width;
			}
			d->red =   static_cast<unsigned char>(GetValue(file, max_value));
			d->green = static_cast<unsigned char>(GetValue(file, max_value));
			d->blue =  static_cast<unsigned char>(GetValue(file, max_value));
			d->alpha = 255;
			d++;
			x--;
			size--;
		} while(size > 0);
		break;

	case '4':		// black and white binary
		im.Parameters().Set("DEPTH", "BLACKNWHITE");
		do {
			if(x == 0) {
				d -= width * 2;
				x = width;
			}
			if(file.Read(&c, 1) != 1) {
				break;
			}
			idx = 8;
			do {
				if((char) c < 0) {
					d->red = d->green = d->blue = 255;
				}
				else {
					d->red = d->green = d->blue = 0;
				}
				d->alpha = 255;
				d++;
				x--;
				idx--;
				size--;
			} while(idx > 0 && size > 0 && x > 0);
		} while(size > 0);
		break;

	case '5':		// grey binary
		im.Parameters().Set("DEPTH", "GREYSCALE");
		do {
			if(x == 0) {
				d -= width * 2;
				x = width;
			}
			if(file.Read(&c, 1) != 1) {
				break;
			}
			d->red = d->green = d->blue = c;
			d->alpha = 255;
			d++;
			x--;
			size--;
		} while(size > 0);
		break;

	case '6':		// RGB binary
		im.Parameters().Set("DEPTH", "RGB");
		do {
			if(x == 0) {
				d -= width * 2;
				x = width;
			}
			if(file.Read(&buf, 3) != 3) {
				break;
			}
			d->red =   buf[0];
			d->green = buf[1];
			d->blue =  buf[2];
			d->alpha = 255;
			d++;
			x--;
			size--;
		} while(size > 0);
		break;

	}

	/* fill the rest of the image if it was not read from the file */
	while(size > 0) {
		if(x == 0) {
			d -= width * 2;
			x = width;
		}
		d->red = d->green = d->blue = 128;
		d->alpha = 255;
		x--;
		size--;
	}

	return true;
}













bool Save(moOStream& file, const moImage& im)
{
	moWCString	*p;
	char		buf[256], format, c;	/* Flawfinder: ignore */
	const char	*cmt; //, *start;
	long		size, x, cnt, r;
	const moRGBA	*s;

// can't save an empty file
	if(im.IsEmpty()) {
		return false;
	}

	/* FORMAT */
	format = '6';
	p = im.Parameters().Get("FORMAT");
	if(p != 0) {
		if(*p == "ASCII") {
			format = '3';
		}
	}
	/* DEPTH */
	p = im.Parameters().Get("DEPTH");
	if(p != 0) {
		if(*p == "GREY" || *p == "GREYSCALE" || *p == "GRAY" || *p == "GRAYSCALE") {
			format--;
		}
		else if(*p == "BLACKNWHITE" || *p == "B&W" || *p == "BW") {
			format -= 2;
		}
	}

	buf[0] = 'P';
	buf[1] = format;
	buf[2] = '\n';
	file.Write(buf, 3);

	/* COMMENT */
	p = im.Parameters().Get("COMMENT");
	if(p != 0) {
		file.Write("# ", 2);
		//start = cmt = p->SavedMBData();
		while(*cmt != '\0') {
			if(*cmt == '\r' && cmt[1] == '\n') {
				cmt++;
			}
			if(*cmt == '\n' || *cmt == '\r') {
				file.Write("\n# ", 3);
			}
			else {
				file.Write(cmt, 1);
			}
			cmt++;
		}
		file.Write("\n", 1);
	}

	size = snprintf(buf, sizeof(buf), "%ld %ld\n", im.Width(), im.Height());	/* Flawfinder: ignore */
	file.Write(buf, size);
	if(format != '1' && format != '4') {
		file.Write("255\n", 4);
	}

	size = im.Size();
	x = im.Width();
	s = im.Data() + size - x;
	switch(format) {
	case '1':
		cnt = 64;
		do {
			if(cnt == 0) {
				file.Write("\n", 1);
				cnt = 64;
			}
			if(x == 0) {
				x = im.Width();
				s -= x * 2;
			}
			if(s->Grey() >= 32) {
				file.Write("0 ", 2);
			}
			else {
				file.Write("1 ", 2);
			}
			cnt--;
			s++;
			x--;
			size--;
		} while(size > 0);
		file.Write("\n", 1);
		break;

	case '2':
		cnt = 32;
		do {
			if(cnt == 0) {
				file.Write("\n", 1);
				cnt = 32;
			}
			if(x == 0) {
				x = im.Width();
				s -= x * 2;
			}
			r = snprintf(buf, sizeof(buf), "%d ", s->Grey());	/* Flawfinder: ignore */
			file.Write(buf, r);
			cnt--;
			s++;
			x--;
			size--;
		} while(size > 0);
		file.Write("\n", 1);
		break;

	case '3':
		cnt = 16;
		do {
			if(cnt == 0) {
				file.Write("\n", 1);
				cnt = 16;
			}
			if(x == 0) {
				x = im.Width();
				s -= x * 2;
			}
			r = snprintf(buf, sizeof(buf), "%d %d %d ", s->red, s->green, s->blue);
			file.Write(buf, r);
			cnt--;
			s++;
			x--;
			size--;
		} while(size > 0);
		file.Write("\n", 1);
		break;

	case '4':
		c = 0;		// avoid warnings
		do {
			if(x == 0) {
				x = im.Width();
				s -= x * 2;
			}
			cnt = 8;
			do {
				c <<= 1;
				if(s->Grey() >= 32) {
					c += 1;
				}
				cnt--;
				s++;
				x--;
				size--;
			} while(size > 0 && x > 0 && cnt > 0);
			file.Write(&c, 1);
		} while(size > 0);
		break;

	case '5':
		do {
			if(x == 0) {
				x = im.Width();
				s -= x * 2;
			}
			c = s->Grey();
			file.Write(&c, 1);
			s++;
			x--;
			size--;
		} while(size > 0);
		break;

	case '6':
		do {
			if(x == 0) {
				x = im.Width();
				s -= x * 2;
			}
			buf[0] = s->red;
			buf[1] = s->green;
			buf[2] = s->blue;
			file.Write(buf, 3);
			s++;
			x--;
			size--;
		} while(size > 0);
		break;

	}

	return true;
}


virtual const char *MimeType(void) const
{
	return "image/ppm";
}

virtual long MatchPriority(void) const
{
	return 0;		// really not terrific, could be a text file...
}


virtual long MatchType(const void *buffer, unsigned long size) const
{
	const char	*s = static_cast<const char *>(buffer);
	const char	*e;
	int		c, idx, value;

#define	qgetc()		(s >= e ? -1 : (int) *s++)

	if(size < 10) {
		return 0;
	}
	e = s + size;

	if(qgetc() != 'P') {
		return 0;
	}
	c = qgetc();
	if(c < '1' || c > '6') {
		return 0;
	}

	// read the width & height at least
	for(idx = 0; idx < 2; idx++) {
		do {
			c = qgetc();
			while(c == '#') {
				c = qgetc();
				while(c != '\r' && c != '\n' && c >= 0) {
					c = qgetc();
				}
				c = qgetc();
			}
		} while(isspace(c) || c == '\r' || c == '\n');
		if(c < 0) {
			// doesn't seem right to me...
			return idx * 8 + 1;
		}
		if(c < '0' || c > '9') {
			// not a valid value
			return 0;
		}
		value = c - '0';
		for(;;) {
			c = qgetc();
			if(c < '0' || c > '9') {
				break;
			}
			value = value * 10 + c - '0';
		}
		if(value == 0) {
			// we don't like sizes of 0
			return 0;
		}
		if(c >= 0) {
			s--;
		}
	} 

	// it's better than nothing, we've got a "valid" width & height
	return 30;
}



};		// class moImageFile_PPM


MAKE_IMAGEFILE_PUBLIC(PPM);


};			// namespace molib;

// vim: ts=8

