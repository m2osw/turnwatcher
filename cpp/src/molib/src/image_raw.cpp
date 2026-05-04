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


namespace molib
{



/************************************************************ DOC:

CLASS

	moImageFile_RAW

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

	[OFFSET]	skip this many bytes before to read the image

	[WIDTH]		give the width of the image (decimal value)
			this parameter is required

	[HEIGHT]	give the height of the image (decimal value)
			this parameter is required

	[DEPTH]		defines the depth of the image, one of:
			"GREY", "GREYA", "RGB555", "RGB", "RGBA"


	Save()

	"DEPTH"		"GREY", "GREYA", "RGB555", "RGB", "RGBA"
			(other values are taken as RGB if the
			image is solid, RGBA otherwise)


SEE ALSO

	MatchType, MatchPosition, MimeType

*/



class moImageFile_RAW : public moImageFile
{



mo_image_file_depth_t Depth(const moImage& im, long& bpp, bool test_solid = false)
{
	mo_image_file_depth_t	depth;

	depth = DefineDepth(im, test_solid);

	switch(depth) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
	case MO_IMAGE_FILE_DEPTH_RED:
	case MO_IMAGE_FILE_DEPTH_GREEN:
	case MO_IMAGE_FILE_DEPTH_BLUE:
	case MO_IMAGE_FILE_DEPTH_ALPHA:
		bpp = 1;
		break;

	case MO_IMAGE_FILE_DEPTH_GREYA:
	case MO_IMAGE_FILE_DEPTH_RGB555:
		bpp = 2;
		break;

	default:		// RGB is taken as the default
		depth = MO_IMAGE_FILE_DEPTH_RGB;
	case MO_IMAGE_FILE_DEPTH_RGB:
		bpp = 3;
		break;

	case MO_IMAGE_FILE_DEPTH_RGBA:
		bpp = 4;
		break;

	}

	return depth;
}



bool Load(moIStream& file, moImage& im)
{
	mo_image_file_depth_t	depth;
	moWCString		*p;
	const unsigned char	*s;
	moRGBA			*d;
	long			l, size, width, height, bpp;

// get parameters
	/* OFFSET */
	p = im.Parameters().Get("OFFSET");
	if(p != 0) {
		/* skip that many bytes (like if the caller couldn't do it...) */
		file.ReadPosition(file.ReadPosition() + p->Integer());
	}

	/* WIDTH & HEIHT */
	p = im.Parameters().Get("WIDTH");
	if(p == 0) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}
	width = p->Integer();

	p = im.Parameters().Get("HEIGHT");
	if(p == 0) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}
	height = p->Integer();

	if(width <= 0 || height <= 0) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	/* DEPTH */
	depth = Depth(im, bpp);

// read the image now
	im.SetSize(width, height);
	size = im.Size() * bpp;
	l = file.Read(im.Data(), size);
	if(l != size) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	d = im.Data() + im.Size();
	s = reinterpret_cast<const unsigned char *>(im.Data()) + size;
	size = im.Size();		// avoid having to use bpp in each loop
	switch(depth) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
		do {
			s--;
			d--;
			d->red =   *s;
			d->green = *s;
			d->blue =  *s;
			d->alpha = 255;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_RED:
		do {
			s--;
			d--;
			d->red =   *s;
			d->green = 0;
			d->blue =  0;
			d->alpha = 255;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_GREEN:
		do {
			s--;
			d--;
			d->red =   0;
			d->green = *s;
			d->blue =  0;
			d->alpha = 255;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_BLUE:
		do {
			s--;
			d--;
			d->red =   0;
			d->green = 0;
			d->blue =  *s;
			d->alpha = 255;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_ALPHA:
		do {
			s--;
			d--;
			d->red =   0;
			d->green = 0;
			d->blue =  0;
			d->alpha = *s;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_GREYA:
		do {
			s -= 2;
			d--;
			d->red =   s[0];
			d->green = s[0];
			d->blue =  s[0];
			d->alpha = s[1];
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_RGB555:
		do {
			s -= 2;
			d--;
			d->red =   s[0] << 1;
			d->green = (s[0] << 6) | (s[1] >> 5);
			d->blue =  s[1] << 3;
			d->alpha = 255;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_RGB:
		do {
			s -= 3;
			d--;
			d->red =   s[0];
			d->green = s[1];
			d->blue =  s[2];
			d->alpha = 255;
			size--;
		} while(size > 0);
		break;

#if 0
// the following does nothing! so we don't do it...
	case MO_IMAGE_FILE_DEPTH_RGBA:
		do {
			s -= 4;
			d--;
			d->red =   s[0];
			d->green = s[1];
			d->blue =  s[2];
			d->alpha = s[3];
			size--;
		} while(size > 0);
		break;
#endif

	default:
		// this can certainly occur!
		throw moError(MO_ERROR_INTERNAL, "Invalid depth for a RAW image");
		/*NOTREACHED*/

	}

	return true;
}













bool Save(moOStream& file, const moImage& im)
{
	long				bpp, size, l;
	mo_image_file_depth_t		depth;
	unsigned char			*d;
	std::auto_ptr<unsigned char>	raw;
	const moRGBA			*s;

	if(im.IsEmpty()) {
		return false;
	}

	depth = Depth(im, bpp, true);

	size = im.Size();
	if(depth == MO_IMAGE_FILE_DEPTH_RGBA) {
		// we can save this one as is!
		size *= 4;
		return file.Write(im.Data(), size) != size;
	}

	l = size * bpp;
	raw.reset(new unsigned char[l]);
	d = raw.get();
	s = im.Data();

	switch(depth) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
		do {
			*d = s->Grey();
			s++;
			d++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_RED:
		do {
			*d = s->red;
			s++;
			d++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_GREEN:
		do {
			*d = s->green;
			s++;
			d++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_BLUE:
		do {
			*d = s->blue;
			s++;
			d++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_ALPHA:
		do {
			*d = s->alpha;
			s++;
			d++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_GREYA:
		do {
			d[0] = s->Grey();
			d[1] = s->alpha;
			d += 2;
			s++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_RGB555:
		do {
			d[0] = ((s->red >> 1) & 0x7C) | (s->green >> 6);
			d[1] = (s->green << 5) | (s->blue >> 3);
			d += 2;
			s++;
			size--;
		} while(size > 0);
		break;

	case MO_IMAGE_FILE_DEPTH_RGB:
		do {
			d[0] = s->red;
			d[1] = s->green;
			d[2] = s->blue;
			d += 3;
			s++;
			size--;
		} while(size > 0);
		break;

	default:
		// this can certainly occur - need to support all
		// the types
		throw moBug(MO_ERROR_INTERNAL, "Invalid depth in moImageRaw Save()");
		/*NOTREACHED*/

	}

	return file.Write(raw.get(), l) == l;
}


virtual const char *MimeType(void) const
{
	return "image/raw";
}

virtual long MatchPriority(void) const
{
	return -100;		// a raw image has absolutly NO magic number...
}


virtual long MatchType(const void * /*buffer*/, unsigned long /*size*/) const
{
	// any file is legal, though... you know...
#ifdef MO_DEBUG
printf("moImageFile_RAW::MatchType() called! (always a match, but returns 1%%)\n");
#endif
	return 1;
}



};		// class moImageFile_RAW


MAKE_IMAGEFILE_PUBLIC(RAW);


};			// namespace molib;

// vim: ts=8

