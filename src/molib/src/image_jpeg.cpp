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

	moImageFile_JPEG

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

	none yet

	Save()

	"DEPTH"		"GREY", "GREYSCALE", "GRAY", "GRAYSCALE"
			"BW", "BLACKNWHITE", "B&W"
			(other values are taken as RGB)

	"QUALITY"	<value>	(10 to 100)

SEE ALSO

	MatchType, MatchPosition, MimeType

*/





// this is really ugly and hopefully it will change one day!
extern "C" {
// NOTE: in older versions, the JPEG includes didn't support C++
#include	<jpeglib.h>
#include	<jerror.h>
#include	<setjmp.h>

typedef struct MO_JPG_ERROR {
	struct jpeg_error_mgr	pub;		/* "public" fields of the error manager */
	jmp_buf			setjmp_buffer;	/* for return to caller */
} mo_jpg_error;

#define	MO_JPG_BUFSIZE		4096
typedef struct MO_JPG_SOURCE {
	struct jpeg_source_mgr	src;
	moIStream *		file;
	JOCTET			buffer[MO_JPG_BUFSIZE];		/* temporary data */
} mo_jpg_source;

#define	jsrc	((mo_jpg_source *) cinfo->src)


typedef struct MO_JPG_DESTINATION {
	struct jpeg_destination_mgr	dst;
	moOStream *			file;
	JOCTET				buffer[MO_JPG_BUFSIZE];
} mo_jpg_destination;

#define	jdst	((mo_jpg_destination *) cinfo->dest)



/* if an error occurs, this function will be called and the JPEG process will be stopped */
static void moJPEGError(j_common_ptr cinfo)
{
	mo_jpg_error	*myerr;

	myerr = (mo_jpg_error *) cinfo->err;	/* cinfo->err really points to a img_jpg_error struct, so coerce pointer */
	longjmp(myerr->setjmp_buffer, 1);	/* Return control to the setjmp point */
/*NOTREACHED*/
}



static boolean moFillInputBuffer(j_decompress_ptr cinfo)
{
	int		r;

	jsrc->src.next_input_byte = jsrc->buffer;
	r = jsrc->file->Read(jsrc->buffer, MO_JPG_BUFSIZE);
	if(r < 0) {
		return FALSE;
	}
	else if(r == 0) {
		/* send an "end of image" - i.e. EOF... */
		jsrc->buffer[0] = JPEG_EOI;
		r = 1;
	}
	jsrc->src.bytes_in_buffer = r;

	return TRUE;
}


static void moInitSource(j_decompress_ptr cinfo)
{
	(void) moFillInputBuffer(cinfo);
}


static void moSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
	int		r;

	if(num_bytes > 0) {
		r = static_cast<int>(jsrc->src.bytes_in_buffer);
		r -= num_bytes;
		if(r > 0) {
			jsrc->src.next_input_byte += num_bytes;
			jsrc->src.bytes_in_buffer = r;
		}
		else {
			if(r < 0) {
				// skip these bytes and read in some more data
				jsrc->file->ReadPosition(jsrc->file->ReadPosition() - r);
			}
			(void) moFillInputBuffer(cinfo);
		}
	}
}


static boolean moResyncToRestart(j_decompress_ptr cinfo, int desired)
{
	return jpeg_resync_to_restart(cinfo, desired);
}


static void moTermSource(j_decompress_ptr cinfo)
{
}





static void moInitDestination(j_compress_ptr cinfo)
{
	jdst->dst.next_output_byte = jdst->buffer;
	jdst->dst.free_in_buffer = MO_JPG_BUFSIZE;
}


static boolean moEmptyOutputBuffer(j_compress_ptr cinfo)
{
	int		r;

	r = jdst->file->Write(jdst->buffer, MO_JPG_BUFSIZE);
	if(r != MO_JPG_BUFSIZE) {
		ERREXIT(cinfo, JERR_FILE_WRITE);
	}

	jdst->dst.next_output_byte = jdst->buffer;
	jdst->dst.free_in_buffer = MO_JPG_BUFSIZE;

	return TRUE;
}


static void moTermDestination(j_compress_ptr cinfo)
{
	int		sz, r;

	sz = static_cast<int>(MO_JPG_BUFSIZE - jdst->dst.free_in_buffer);
	r = jdst->file->Write(jdst->buffer, sz);
	if(r != sz) {
		ERREXIT(cinfo, JERR_FILE_WRITE);
	}
}



}		// extern "C"




class moImageFile_JPEG : public moImageFile
{


virtual const char *MimeType(void) const
{
	return "image/jpeg";
}

virtual long MatchPriority(void) const
{
	return 100;
}

virtual long MatchType(const void *buffer, unsigned long size) const
{
	const unsigned char *s = static_cast<const unsigned char *>(buffer);

	if(s[0] == 0xFF && s[1] == 0xD8
	&& s[2] == 0xFF && s[3] == 0xE0) {
		// this is a JPEG (be it a JFIF or not)
		return 100;
	}

	return 0;
}

virtual bool Load(moIStream& file, moImage& im)
{
	mo_jpg_error			jerr;
	struct jpeg_decompress_struct	cinfo;
	mo_jpg_source			src;
	moRGBA				*dst;
	unsigned char			*s;
	long				i;
	unsigned long			sl;

/* setup the ugly error handler */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = moJPEGError;
	if(setjmp(jerr.setjmp_buffer)) {
		/* an error occured! */
		jpeg_destroy_decompress(&cinfo);
		return (false);
	}

/* initialize the decompressor */
	jpeg_create_decompress(&cinfo);

/* setup the input callbacks */
	src.src.init_source       = moInitSource;
	src.src.fill_input_buffer = moFillInputBuffer;
	src.src.skip_input_data   = moSkipInputData;
	src.src.resync_to_restart = moResyncToRestart;
	src.src.term_source       = moTermSource;
	src.file = &file;

	cinfo.src = &src.src;

/* read the JPEG setup from the source file */
	jpeg_read_header(&cinfo, TRUE);

/* ask for an RGB image (which is the usual default) */
	cinfo.out_color_space = JCS_RGB;

/* start the decompressor (only after that call can we be sure of the output parameters) */
	jpeg_start_decompress(&cinfo);

/* make sure we're getting an RGB image */
	if(cinfo.output_components != 3) {
		jpeg_destroy_decompress(&cinfo);
		return false;
	}

/* allocate the image in which the JPEG is read */
	im.SetSize(cinfo.output_width, cinfo.output_height);

	dst = im.Data() + im.Size();
	sl = cinfo.output_scanline + 1;
	while(cinfo.output_scanline != sl && cinfo.output_scanline < cinfo.output_height) {
		i = cinfo.output_width;
		dst -= i;
		sl = cinfo.output_scanline;
		JSAMPROW samp = reinterpret_cast<JSAMPROW>(dst);
		jpeg_read_scanlines(&cinfo, &samp, 1);

		// transform the RGB in RGBA
		s = (unsigned char *) dst + i * 3;
		i--;		// don't do the last pixel yet
		dst += i;
		while(i > 0) {
			i--;
			s -= 3;
			dst->red =   s[0];
			dst->green = s[1];
			dst->blue =  s[2];
			dst->alpha = 255;
			dst--;
		}
		// the last pixel needs not be moved, only the alpha defined
		dst->alpha = 255;
	}

/* stop and free the decompressor */
	if(cinfo.output_scanline != sl) {
		jpeg_finish_decompress(&cinfo);
	}
	jpeg_destroy_decompress(&cinfo);

	return true;
}





virtual bool Save(moOStream& file, const moImage& im)
{
	mo_jpg_error			jerr;
	struct jpeg_compress_struct	cinfo;
	mo_jpg_destination		dst;
	const moRGBA			*src, *s;
	unsigned char			*d;
	std::auto_ptr<unsigned char>	row;
	moVariable			*p;
	long				sz;

/* create the JPEG error handler */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = moJPEGError;
	if(setjmp(jerr.setjmp_buffer)) {
		/* an error occured */
		jpeg_destroy_compress(&cinfo);
		return (false);
	}

/* create the compressor */
	jpeg_create_compress(&cinfo);

/* set the output callbacks */
	dst.dst.init_destination =    moInitDestination;
	dst.dst.empty_output_buffer = moEmptyOutputBuffer;
	dst.dst.term_destination =    moTermDestination;
	dst.file = &file;

	cinfo.dest = &dst.dst;

/* setup the compression parameters */
	cinfo.image_width      = im.Width();
	cinfo.image_height     = im.Height();
	cinfo.input_components = 3;				/* default to RGB 24 bits */
	cinfo.in_color_space   = JCS_RGB;			/* always required */

	/* DEPTH */
	p = im.Parameters().Get("DEPTH");
	if(p != 0) {
		if(*p == "GREY" || *p == "GREYSCALE" || *p == "GRAY" || *p == "GRAYSCALE"
		|| *p == "BW" || *p == "BLACKNWHITE" || *p == "B&W") {
			cinfo.input_components = 1;
			cinfo.in_color_space   = JCS_GRAYSCALE;
		}
	}

	jpeg_set_defaults(&cinfo);

/* retrieve the user parameters */
	p = im.Parameters().Get("QUALITY");
	if(p != 0) {				/* any quality definition? */
		jpeg_set_quality(&cinfo, p->Integer(), TRUE);
	}

/* create a temporary row buffer */
	row.reset(new unsigned char[cinfo.image_width * cinfo.input_components]);

/* compress the image now */
	jpeg_start_compress(&cinfo, TRUE);
	src = im.Data() + im.Size();
	if(cinfo.input_components == 3) {
		while(cinfo.next_scanline < cinfo.image_height) {
			sz = cinfo.image_width;
			src -= sz;
			d = row.get();
			s = src;
			do {
				d[0] = s->red;
				d[1] = s->green;
				d[2] = s->blue;
				d += 3;
				s++;
				sz--;
			} while(sz > 0);
			d = row.get();
			jpeg_write_scanlines(&cinfo, (JSAMPARRAY) &d, 1);
		}
	}
	else {
		while(cinfo.next_scanline < cinfo.image_height) {
			sz = cinfo.image_width;
			src -= sz;
			d = row.get();
			s = src;
			do {
				d[0] = s->Grey();
				d++;
				s++;
				sz--;
			} while(sz > 0);
			d = row.get();
			jpeg_write_scanlines(&cinfo, (JSAMPARRAY) &d, 1);
		}
	}

/* now we can free all the resources */
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	return true;
}




};		// class moImageFile_JPEG

MAKE_IMAGEFILE_PUBLIC(JPEG);




};			// namespace molib;

// vim: ts=8

