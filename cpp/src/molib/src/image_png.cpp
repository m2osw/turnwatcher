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




// The implementation is in image.c++
//#ifdef MO_PRAGMA_INTERFACE
//#pragma implementation "mo/mo_image.h"
//#endif

#include	"mo/mo_image.h"


#include	<png.h>

#if PNG_LIBPNG_VER <= 10005
#define	png_jmpbuf(png)		((png)->jmpbuf)
#endif

namespace molib
{


/************************************************************ DOC:

CLASS

	moImageFile_PNG

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

extern "C"
{


static void moRead(png_structp png, png_bytep data, png_size_t size)
{
	reinterpret_cast<moIStream *>(png_get_io_ptr(png))->Read(data, size);
}



static void moWrite(png_structp png, png_bytep data, png_size_t size)
{
	reinterpret_cast<moOStream *>(png_get_io_ptr(png))->Write(data, size);
}



};	// extern "C"




class moImageFile_PNG : public moImageFile
{




bool Load(moIStream& file, moImage& im)
{
	png_structp			png;
	png_infop			info;
	png_textp			text;
	double				gamma;
	png_uint_32			width, height, y;
	int				size, color, depth;
	png_byte			sig[sizeof(info->signature)], **v, *p;

/* check out the signature to make sure it works */
	if(file.Read(sig, sizeof(info->signature)) != sizeof(info->signature)) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}
	if(!png_check_sig(sig, sizeof(info->signature))) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

/* create a set of PNG & PNG info structures - we don't want to handle errors at this time */
	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if(png == 0) {
		throw std::bad_alloc();
	}

	info = png_create_info_struct(png);
	if(info == 0) {
		png_destroy_read_struct(&png, 0, 0);
		throw std::bad_alloc();
	}

/*
 * setup the error handler
 * define the function used to read data
 * tell the library we already read the signature
 * then read the header
 */
#if GCC_VERSION >= 4000
	// older gcc versions generate an error here
	v = 0;
#endif
	if(setjmp(png_jmpbuf(png))) {	/* another ugly one! */
		png_destroy_read_struct(&png, &info, 0);
#if GCC_VERSION >= 4000
		// older gcc versions generate an error here
		delete v;
#endif
		return false;
	}
	png_set_read_fn(png, &file, moRead);
	png_set_sig_bytes(png, sizeof(info->signature));
	png_read_info(png, info);

/* get the info we just got from the file header */
	png_get_IHDR(png, info, &width, &height, &depth, &color, 0, 0, 0);

/* now we change the setup so we directly read RGBA from the file */
	if(color == PNG_COLOR_TYPE_PALETTE			// un-palette the result
	|| (color == PNG_COLOR_TYPE_GRAY && depth < 8)		// 1, 2, 4 bits to 8 bits
	|| png_get_valid(png, info, PNG_INFO_tRNS)) {		// expand transparency
		png_set_expand(png);
	}

	if(depth == 16) {					// we are in the 8 bits image format
		png_set_strip_16(png);
	}

	if(color == PNG_COLOR_TYPE_GRAY				// gray expanded and to RGB
	|| color == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png);
	}

	if(png_get_gAMA(png, info, &gamma)) {			// any gamma correction to apply?
		png_set_gamma(png, (double) 2.0, gamma);
	}

	if(color != PNG_COLOR_TYPE_GRAY_ALPHA
	&& color != PNG_COLOR_TYPE_RGB_ALPHA) {
		png_set_filler(png, 255, PNG_FILLER_AFTER);
	}

/* reset the loader with the new parameters */
	png_read_update_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, 0, 0, 0);

	/* make sure it worked */
	if((color != PNG_COLOR_TYPE_RGB && color != PNG_COLOR_TYPE_RGB_ALPHA)
	|| depth != 8 || width == 0 || height == 0) {
		png_destroy_read_struct(&png, &info, 0);
		return false;
	}

	im.SetSize(width, height);

	v = new png_byte *[height];
	if(v == 0) {
		png_destroy_read_struct(&png, &info, 0);
		throw std::bad_alloc();
	}
	p = reinterpret_cast<png_byte *>(im.Data());
	for(y = 0; y < height; y++) {
		v[y] = p + (height - y - 1) * width * sizeof(moRGBA *);
	}
	png_read_image(png, v);
	delete v;
	v = 0;

/* check whether there are some user info */
	png_get_text(png, info, &text, &size);
	while(size > 0) {
		size--;
		moWCString data(text->text, static_cast<int>(text->text_length));
		im.Parameters().Set(text->key, data);
		text++;
	}

/* we're done */
	png_read_end(png, info);
	png_destroy_read_struct(&png, &info, 0);

	return true;
}










void FreeTexts(png_text *txt, long cnt)
{
	while(cnt > 0) {
		cnt--;
		if(txt->text != 0) {
			delete [] txt->text;
		}
		txt++;
	}
}





bool Save(moOStream& file, const moImage& im)
{
	static const char *common_png_texts[] = {
		"Author",
		"Comment",
		"Copyright",
		"Creation Time",
		"Description",
		"Disclaimer",
		"Software",
		"Source",
		"Title",
		"Warning"
	};
/* Flawfinder: ignore */
#define	MAX_TEXTS	(sizeof(common_png_texts) / sizeof(const char *))

	png_text	texts[MAX_TEXTS], *txt;
	long		idx, pass, color, interlace, x;
	png_structp	png;
	png_infop	info;
	png_byte	*v, *dst;
	const moRGBA	*d;
	moWCString	*p;

/* initialize PNG parameters */
	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if(png == 0) {
		throw std::bad_alloc();
	}

	info = png_create_info_struct(png);
	if(info == 0) {
		png_destroy_write_struct(&png, 0);
		throw std::bad_alloc();
	}

	v = 0;
	if(setjmp(png_jmpbuf(png))) {		// ugly stuff!!
		png_destroy_write_struct(&png, &info);
		delete v;
		return false;
	}

/* get parameters */
	/* DEPTH */
	switch(DefineDepth(im, true)) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
		color = PNG_COLOR_TYPE_GRAY;
		break;

	case MO_IMAGE_FILE_DEPTH_GREYA:
		color = PNG_COLOR_TYPE_GRAY_ALPHA;
		break;

	default:	// case MO_IMAGE_FILE_DEPTH_RGB: -- the default
		color = PNG_COLOR_TYPE_RGB;
		break;

	case MO_IMAGE_FILE_DEPTH_RGBA:
		color = PNG_COLOR_TYPE_RGB_ALPHA;
		break;

	}

	/* INTERLACE */
	interlace = PNG_INTERLACE_NONE;
	p = im.Parameters().Get("INTERLACE");
	if(p != 0) if(*p == "ADAM7") {
		interlace = PNG_INTERLACE_ADAM7;
	}

	/* different texts to be saved in the file */
	for(idx = 0, txt = texts; idx < static_cast<long>(MAX_TEXTS); idx++) {
		p = im.Parameters().Get(common_png_texts[idx]);
		if(p != 0) {
			// I'm wondering if the PNG author(s) heard about const...
			txt->key = const_cast<char *>(common_png_texts[idx]);
			txt->text = const_cast<char *>(p->MBData());		// watch out - we need to free these!
			txt->text_length = p->MBLength();
			txt->compression = txt->text_length >= 1024 ? PNG_TEXT_COMPRESSION_zTXt : PNG_TEXT_COMPRESSION_NONE;
			txt++;
		}
	}

	png_set_write_fn(png, &file, moWrite, 0);

	png_set_IHDR(png, info, im.Width(), im.Height(), 8, color, interlace, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	if(txt != texts) {
		png_set_text(png, info, texts, static_cast<int>(txt - texts));
	}

	png_write_info(png, info);

	if(color == PNG_COLOR_TYPE_RGB) {
		png_set_filler(png, 0, PNG_FILLER_AFTER);
	}

	pass = interlace == PNG_INTERLACE_NONE ? 1 : png_set_interlace_handling(png);

	if(color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA) {
		v = new png_byte[im.Width() * 2];
	}

	do {
		idx = im.Height();
		d = im.Data() + im.Size();
		switch(color) {
		case PNG_COLOR_TYPE_GRAY:
			do {
				x = im.Width();
				dst = v + x;
				do {
					d--;
					dst--;
					*dst = d->Grey();
					x--;
				} while(x > 0);
				png_write_rows(png, &v, 1);
				idx--;
			} while(idx > 0);
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			do {
				x = im.Width();
				dst = v + x * 2;
				do {
					d--;
					dst -= 2;
					dst[0] = d->Grey();
					dst[1] = d->alpha;
					x--;
				} while(x > 0);
				png_write_rows(png, &v, 1);
				idx--;
			} while(idx > 0);
			break;

		case PNG_COLOR_TYPE_RGB:
		case PNG_COLOR_TYPE_RGB_ALPHA:
			do {
				d -= im.Width();
				const png_byte *png_bytes = reinterpret_cast<const png_byte *>(d);
				png_write_rows(png, const_cast<png_byte **>(&png_bytes), 1);
				idx--;
			} while(idx > 0);
			break;

		}
		pass--;
	} while(pass > 0);

	png_write_end(png, 0);

	png_destroy_write_struct(&png, &info);

	FreeTexts(txt, static_cast<int>(txt - texts));

	delete v;

	return true;
}




virtual const char *MimeType(void) const
{
	return "image/png";
}

virtual long MatchPriority(void) const
{
	return 100;		// really not terrific, could be a text file...
}


virtual long MatchType(const void *buffer, unsigned long size) const
{
#if 0
    // TODO: this needs to point to something!
    //
	png_infop		info;

	if(size < sizeof(info->signature)) {
		return 0;
	}
	if(!png_check_sig(static_cast<png_byte *>(const_cast<void *>(buffer)), sizeof(info->signature))) {
		return 0;
	}
#endif

	return 100;
}



};		// class moImageFile_PNG


MAKE_IMAGEFILE_PUBLIC(PNG);


}			// namespace molib;

// vim: ts=8

