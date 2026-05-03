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

	moImageFile_SGI

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

	[RELAX]		if this parameter exists, it makes this
			loader accept erroneous files and return
			a valid (but most probably incomplet)
			image

	"COMPRESSION"	"RLE", "NONE"

	"DEPTH"		"GREYSCALE", "GREYA", "RGB", "RGBA"

	"NAME"		<string>
			(internal image name if any)


	Save()

	"COMPRESSION"	"NONE"
			(other values are taken as RLE)

	"DEPTH"		"GREY", "GREYA", "RGBA",
			(other values are taken as RGB if the
			image is solid, RGBA otherwise)

	"NAME" or "FILENAME"
			<string>
			(internal image name)

SEE ALSO

	MatchType, MatchPosition, MimeType

*/
class moImageFile_SGI : public moImageFile
{


#define	SGI_HEADER_SIZE			512

struct sgi_image_file_header {
	unsigned short	magic;		/* whether the file is little or big endian */
	unsigned char	type;		/* verbatim or RLE compressed */
	unsigned char	bpp;		/* 8 or 16 bits */
	unsigned short	dim;		/* number of dimensions */
	unsigned short	xsize;		/* number of pixels by x */
	unsigned short	ysize;		/* number of pixels by y */
	unsigned short	zsize;		/* number of pixels by z */
	unsigned long	min;		/* minimum color (0) */
	unsigned long	max;		/* maximum color (255 or 65535) */
	unsigned long	wastebyte;	/* always zero */
	char		name[80];	/* name of the image */				/* Flawfinder: ignore */
	unsigned long	colormap;	/* unused */
	char		pad[SGI_HEADER_SIZE - 108];	/* header is 512 bytes */	/* Flawfinder: ignore */
};

/* the following isn't really used since the code has to work on
 * either a big or little endian!
 */
#define	SGI_MAGIC_BIGENDIAN		0x01DA
#define	SGI_MAGIC_LITTLEENDIAN		0xDA01

#define SGI_TYPE_VERBATIM               0       /* Not compacted */
#define SGI_TYPE_RLE                    1       /* RLE compacted */

#define SGI_BPP_CHAR                    1       /* Each color is 8 bits */
#define SGI_BPP_SHORT                   2       /* Each color is 16 bits */



/*
 * V - verbatim
 * R - RLE
 * C - char
 * S - short
 * E - same endian
 * W - opposite endian (swap)
 */
#define	SGI_MODE_V		0x0000
#define	SGI_MODE_R		0x0001
#define	SGI_MODE_C		0x0000
#define	SGI_MODE_S		0x0002
#define	SGI_MODE_E		0x0000
#define	SGI_MODE_W		0x0004

#define	SGI_MODE_VCE		(SGI_MODE_V | SGI_MODE_C | SGI_MODE_E)
#define	SGI_MODE_VSE		(SGI_MODE_V | SGI_MODE_S | SGI_MODE_E)
#define	SGI_MODE_VCW		(SGI_MODE_V | SGI_MODE_C | SGI_MODE_W)
#define	SGI_MODE_VSW		(SGI_MODE_V | SGI_MODE_S | SGI_MODE_W)
#define	SGI_MODE_RCE		(SGI_MODE_R | SGI_MODE_C | SGI_MODE_E)
#define	SGI_MODE_RSE		(SGI_MODE_R | SGI_MODE_S | SGI_MODE_E)
#define	SGI_MODE_RCW		(SGI_MODE_R | SGI_MODE_C | SGI_MODE_W)
#define	SGI_MODE_RSW		(SGI_MODE_R | SGI_MODE_S | SGI_MODE_W)


struct sgi_save_context {
	sgi_save_context()
		: scanlines(0), offset(0)
	{
	}

	~sgi_save_context()
	{
		delete [] scanlines;
		delete [] offset;
	}

	moOStream *		file;
	const moImage *		image;
	sgi_image_file_header	header;

	/* data to be saved in the file */
	long			mode;
	unsigned char *		scanlines;
	unsigned char *		c;
	unsigned char *		compress;
	unsigned char *		t;
	long			repeat;

	/* offsets and lengths of compressed data */
	unsigned long		line;
	unsigned long *		offset;
	unsigned long *		length;
};














bool Load(moIStream& file, moImage& im)
{
	sgi_image_file_header		header;
	moRGBA				*d;
	unsigned char			*scanlines, *c[4], *r, pixel;	/* Flawfinder: ignore */
	std::auto_ptr<unsigned char>	scanlines_buf;
	int				mode;
	bool				result, relax;
	unsigned long			x, y, z, m, cnt, i, len, sz, *length;
	std::auto_ptr<unsigned long>	offset;
	unsigned long			l[4];
	moVariable			*p;

/* retrieve the parameters we will use through this loading process */
	/* RELAX */
	p = im.Parameters().Get("RELAX");
	if(p == 0) {
		// by default we try to read anything
		relax = true;
	}
	else {
		mode = p->Integer();
		if(mode != 1 && mode != 0) {
			im.LastErrno(MO_ERROR_INVALID);
			return false;
		}
		relax = mode != 0;
	}

/* first, read the first 512 bytes (the header) */
	if(file.Read(&header, sizeof(header)) != sizeof(header)) {
		/* invalid file size already... */
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	/* check the magic number */
	if(header.magic == 0xDA01) {
		/* inverted endian, swap the header values */
		header.dim =   moSwap16Bits(header.dim);
		header.xsize = moSwap16Bits(header.xsize);
		header.ysize = moSwap16Bits(header.ysize);
		header.zsize = moSwap16Bits(header.zsize);
		header.min =   moSwap32Bits((uint32_t) header.min);
		header.max =   moSwap32Bits((uint32_t) header.max);
	}
	else if(header.magic != 0x01DA) {
		/* not an SGI image */
		im.LastErrno(MO_ERROR_UNKNOWN);
		return false;
	}

	/* this should never happen, but in case it does, fix it */
	if(header.dim == 2) {
		header.dim = 3;
		header.zsize = 1;
	}

	/* check the validity of the input data */
	if(header.dim != 3
	|| !moValidImageSize(header.xsize, header.ysize)
	|| header.zsize == 0 || header.zsize > 4
	|| (header.type != SGI_TYPE_VERBATIM && header.type != SGI_TYPE_RLE)
	|| (header.bpp != SGI_BPP_CHAR && header.bpp != SGI_BPP_SHORT)
	|| (!relax && (
		   header.min > header.max
		|| ((header.min > 255 || header.max > 255)
					&& header.bpp == SGI_BPP_CHAR)
		|| ((header.min > 65535 || header.max > 65535)
					&& header.bpp == SGI_BPP_SHORT)
	   ))
	) {
		/* it looks like an SGI image, but there are invalid values */
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	if(header.name[0] != '\0') {
		moWCString	name(header.name, sizeof(header.name));
		im.Parameters().Set("NAME", name);
	}

/* in RLE mode we need to load the offsets & lengths next */
	if(header.type == SGI_TYPE_RLE) {
		im.Parameters().Set("COMPRESSION=RLE");
		sz = header.ysize * header.zsize;
		offset.reset(new unsigned long[sz * 2]);
		if(file.Read(offset.get(), sz * 2 * sizeof(unsigned long)) != static_cast<long>(sz * 2 * sizeof(unsigned long))) {
			/* invalid file size... */
			im.LastErrno(MO_ERROR_INVALID);
			return false;
		}
		length = offset.get() + sz;
		if(header.magic != 0x01DA) {
			/* we need to swap the long words! */
			i = sz * 2;
			while(i > 0) {
				i--;
				offset.get()[i] = moSwap32Bits((uint32_t) offset.get()[i]);
			}
		}

		/*
		 * search for the largest offset + length, it is
		 * the length of the data we need to read from the
		 * input file
		 */
		len = 0;
		i = sz;
		while(i > 0) {
			i--;
			/* adjust the offset to internal buffer offsets */
			offset.get()[i] -= sz * 2 * sizeof(unsigned long) + 512;
			if(len < offset.get()[i] + length[i]) {
				len = offset.get()[i] + length[i];
			}
		}
	}
	else {
		/* this isn't compressed, yet it's upside down & colors are separated */
		im.Parameters().Set("COMPRESSION=NONE");
		len = header.xsize * header.ysize * header.zsize * header.bpp;
		length = 0;
	}

/* read the scanlines at once (so we can use stdin as input) */
	scanlines_buf.reset(new unsigned char[len]);
	scanlines = scanlines_buf.get();
	sz = file.Read(scanlines, len);
	if(sz != len) {
		if(!relax) {
			im.LastErrno(MO_ERROR_INVALID);
			return false;
		}
		// enable loading broken images
		memset(scanlines + sz, 0, len - sz);
	}

	/*
	 * We loose the lower byte when shorts are used, for
	 * this we need to know if we are loading the opposite
	 * endian - if so, add +1 to the source pointer
	 */
	if(header.magic != 0x01DA && header.bpp == SGI_BPP_SHORT) {
		scanlines++;
	}

	/* NOTE: in relax mode, the scanlines may not be complete
	 *	 you have to keep that in mind when decompressing
	 */

/* process the data in an image now */
	im.SetSize(header.xsize, header.ysize);
	switch(header.zsize) {
	case 1:
		im.Parameters().Set("DEPTH=GREYSCALE");
		break;

	case 2:
		im.Parameters().Set("DEPTH=GREYA");
		break;

	case 3:
		im.Parameters().Set("DEPTH=RGB");
		break;

	case 4:
		im.Parameters().Set("DEPTH=RGBA");
		break;

	}

	mode = header.zsize | header.type << 6;

	sz = header.xsize * header.bpp;
	len = sz * header.ysize;
		/*
		 * NOTE: the SGI images are already swapped
		 */
	result = true;
	d = im.Data() + im.Size();
	y = header.ysize;
	while(y > 0) {
		d -= header.xsize;
		y--;
		switch(mode) {
		case 1 | (SGI_TYPE_VERBATIM << 6):
			/* grey only */
			c[0] = c[1] = c[2] = scanlines + y * sz;
			c[3] = 0;
			l[0] = l[1] = l[2] = header.xsize;
			l[3] = 0;
			break;

		case 2 | (SGI_TYPE_VERBATIM << 6):
			/* grey and alpha */
			c[0] = c[1] = c[2] = scanlines + y * sz;
			c[3] = c[0] + len;
			l[0] = l[1] = l[2] = l[3] = header.xsize;
			break;

		case 3 | (SGI_TYPE_VERBATIM << 6):
			/* RGB */
			c[0] = scanlines + y * sz;
			c[1] = c[0] + len;
			c[2] = c[1] + len;
			c[3] = 0;
			l[0] = l[1] = l[2] = header.xsize;
			l[3] = 0;
			break;

		case 4 | (SGI_TYPE_VERBATIM << 6):
			/* RGBA */
			c[0] = scanlines + y * sz;
			c[1] = c[0] + len;
			c[2] = c[1] + len;
			c[3] = c[2] + len;
			l[0] = l[1] = l[2] = l[3] = header.xsize;
			break;

		case 1 | (SGI_TYPE_RLE << 6):
			/* grey only */
			c[0] = c[1] = c[2] = scanlines + offset.get()[y];
			c[3] = 0;
			l[0] = l[1] = l[2] = length[y];
			l[3] = 0;
			break;

		case 2 | (SGI_TYPE_RLE << 6):
			/* grey and alpha */
			c[0] = c[1] = c[2] = scanlines + offset.get()[y];
			c[3] = scanlines + offset.get()[y + header.ysize];
			l[0] = l[1] = l[2] = length[y];
			l[3] = length[y + header.ysize];
			break;

		case 3 | (SGI_TYPE_RLE << 6):
			/* RGB */
			c[0] = scanlines + offset.get()[y];
			c[1] = scanlines + offset.get()[y + header.ysize];
			c[2] = scanlines + offset.get()[y + header.ysize * 2];
			c[3] = 0;
			l[0] = length[y];
			l[1] = length[y + header.ysize];
			l[2] = length[y + header.ysize * 2];
			l[3] = 0;
			break;

		case 4 | (SGI_TYPE_RLE << 6):
			/* RGBA */
			c[0] = scanlines + offset.get()[y];
			c[1] = scanlines + offset.get()[y + header.ysize];
			c[2] = scanlines + offset.get()[y + header.ysize * 2];
			c[3] = scanlines + offset.get()[y + header.ysize * 3];
			l[0] = length[y];
			l[1] = length[y + header.ysize];
			l[2] = length[y + header.ysize * 2];
			l[3] = length[y + header.ysize * 3];
			break;

		}

		/* determine the depth here */
		if(c[3] == 0) {
			/* no alpha - use 'solid' by default */
			x = header.xsize;
			while(x > 0) {
				x--;
				d[x].alpha = 255;
			}
			z = 3;
		}
		else {
			z = 4;
		}
		if(header.bpp == SGI_BPP_SHORT) {
			l[0] *= 2;		// we will increment by header.bpp instead of 1
			l[1] *= 2;
			l[2] *= 2;
			l[3] *= 2;
		}
		while(z > 0) {
			z--;
			switch(z) {
			case 0:
				r = &d->red;
				break;

			case 1:
				r = &d->green;
				break;

			case 2:
				r = &d->blue;
				break;

			case 3:
			default:	/* avoid warnings, can't happen */
				r = &d->alpha;
				break;

			}
			x = 0;
			m = 0;		/* source in RLE have variable sizes */
			while(x < header.xsize && m < l[z]) {
				if(header.type == SGI_TYPE_VERBATIM) {
					r[x * 4] = c[z][m];
					m += header.bpp;
					x++;
				}
				else {
					cnt = c[z][m];
					m += header.bpp;
					if(cnt == 0) {
						/* a zero marks the end, yet we shouldn't find it here */
						break;
					}
					if(static_cast<signed char>(cnt) < 0) {
						cnt &= 0x7F;
						if(x + cnt > header.xsize || m + cnt * header.bpp > l[z]) {
							// slow "do as much as we can" loop
							while(cnt > 0 && x < header.xsize && m < l[z]) {
								cnt--;
								r[x * 4] = c[z][m];
								m += header.bpp;
								x++;
							}
							im.LastErrno(MO_ERROR_OVERFLOW);
							result = false;
							break;
						}
						while(cnt > 0) {
							cnt--;
							r[x * 4] = c[z][m];
							m += header.bpp;
							x++;
						}
					}
					else {
						if(x + cnt > header.xsize || m + header.bpp > l[z]) {
							im.LastErrno(MO_ERROR_OVERFLOW);
							result = false;
							break;
						}
						pixel = c[z][m];
						m += header.bpp;
						while(cnt > 0) {
							cnt--;
							r[x * 4] = pixel;
							x++;
						}
					}
				}
			}
			/* make sure the RLE compression is null terminated */
			if(!relax && header.type == SGI_TYPE_RLE && m < l[z]) {
				if((c[z][m] & 0x007F) != 0) {
					im.LastErrno(MO_ERROR_UNDERFLOW);
					result = false;
				}
				m += header.bpp;
			}
			/* make sure we've got a full line */
			if(!relax && (x != header.xsize || m != l[z])) {
				im.LastErrno(MO_ERROR_INVALID);
				result = false;
			}
		}

		/* we're done with this scanline, go to the next one */
	}

	return result;
}









inline void sgi_save_low_write(sgi_save_context *context, unsigned char pixel)
{
	*context->c++ = pixel;
}



void sgi_save_write(sgi_save_context *context, unsigned char pixel)
{
	unsigned char	*s, *e, *l, p;
	unsigned char	*start;
	unsigned long	i, length;

	if((context->mode & SGI_MODE_R) == 0) {
		/* verbatime, simply write that */
		sgi_save_low_write(context, pixel);
		return;
	}

	/* keep line in a buffer until end of line */
	*context->t++ = pixel;
	context->repeat++;

	/* end of line reached? */
	if(context->repeat != context->header.xsize) {
		/* not yet, wait for more data */
		return;
	}

	start = context->c;

	/* offset were we can find this line */
	context->offset[context->line] = static_cast<unsigned long>(start - context->scanlines);

	/* compress this line */
	s = context->compress;
	l = s + context->header.xsize;
	while(s < l) {
		/* 1. repeatitions (pixels are equal) */
		e = s + 1;
		p = s[0];
		while(e < l && e - s < 127 && p == *e) {
			e++;
		}
		if(e - s > 1) {
			/* we found a repeated color */
			sgi_save_low_write(context, static_cast<unsigned char>(e - s));
			sgi_save_low_write(context, p);
			/* we may have several repetition */
			s = e;
			continue;
		}
		/* 2. copy (pixels differ) */
		while(e + 1 < l && e - s < 127 && e[0] != e[1]) {
			e++;
		}
		/* worth keeping a repeatition after this copy? */
		if(e + 2 >= l) {
			e = l;
			if(e - s > 127) {	// make sure we don't overflow the counter
				e = s + 127;
			}
		}
		sgi_save_low_write(context, static_cast<unsigned char>((e - s) | 0x80));
		while(s < e) {
			sgi_save_low_write(context, *s++);
		}
	}
	/* RLE terminates with a zero!??!? */
	sgi_save_low_write(context, 0);

	/* reset parameters for the next line of data */
	context->t = context->compress;
	context->repeat = 0;

	/* length of this compressed line */
	length = static_cast<unsigned long>(context->c - start);
	context->length[context->line] = length;

	context->line++;

	/*
	 * to compress better we check to see whether our
	 * new line is not possibly equal to an older line
	 * (this can happen much in computer generated images)
	 */
	i = context->line - 1;	/* don't check itself */
	while(i > 0) {
		i--;
		if(context->length[i] == length
		&& memcmp(context->scanlines + context->offset[i], start, length) == 0) {
			/* we found an equal line */
			/* use its offset */
			context->offset[context->line - 1] = context->offset[i];
			/* reuse the space of the current line for the next compressed line */
			context->c = start;
		}
	}
}


void sgi_save_component(sgi_save_context *context, const unsigned char *comp)
{
	unsigned long	sz;

	/* this format takes bottom to top images - save it as is */
	sz = context->image->Size();
	while(sz > 0) {
		sz--;
		sgi_save_write(context, *comp);
		comp += sizeof(moRGBA);
	}
}




void sgi_save_grey(sgi_save_context *context)
{
	const moRGBA	*comp;
	unsigned long	sz;

	/* this format takes bottom to top images - save it as is */
	comp = context->image->Data();
	sz = context->image->Size();
	while(sz > 0) {
		sz--;
		sgi_save_write(context, comp->Grey());
		comp++;
	}
}




void sgi_save_send(sgi_save_context *context)
{
	switch(context->header.zsize) {
	case 1:
		sgi_save_grey(context);
		break;

	case 2:
		sgi_save_grey(context);
		sgi_save_component(context, &context->image->Data()->alpha);
		break;

	case 3:
		sgi_save_component(context, &context->image->Data()->red);
		sgi_save_component(context, &context->image->Data()->green);
		sgi_save_component(context, &context->image->Data()->blue);
		break;

	case 4:
		sgi_save_component(context, &context->image->Data()->red);
		sgi_save_component(context, &context->image->Data()->green);
		sgi_save_component(context, &context->image->Data()->blue);
		sgi_save_component(context, &context->image->Data()->alpha);
		break;

	}
}



bool Save(moOStream& file, const moImage& im)
{
	const moRGBA		*s;
	int			sz, t;
	unsigned long		min, max;
	sgi_image_file_header	h;
	sgi_save_context	context;
	moWCString		*p;

// can't save an empty file
	if(im.IsEmpty()) {
		return false;
	}

// prepare the context structure
	memset(&context, 0, sizeof(context));
	context.file = &file;
	context.image = &im;
	context.header.type = SGI_TYPE_RLE;	// by default, compress the file
	context.header.dim = 3;			// uses x/y/z
	context.header.xsize = static_cast<unsigned short>(im.Width());
	context.header.ysize = static_cast<unsigned short>(im.Height());
	context.header.bpp = SGI_BPP_CHAR;	// to save in shorts used an Image16

// get the parameters
#if SGI_MODE_C
#error "invalid magic test (SGI_MODE_C not zero?!?)"
#endif
	/* COMMENT */
	p = im.Parameters().Get("COMMENT");
	if(p != 0) {
		p->MBData(context.header.pad, sizeof(context.header.pad));
	}
	/* COMPRESSION */
	switch(DefineCompression(im)) {
	case MO_IMAGE_FILE_COMPRESSION_NONE:
		context.header.type = SGI_TYPE_VERBATIM;
		break;

	case MO_IMAGE_FILE_COMPRESSION_RLE:
		context.header.type = SGI_TYPE_RLE;
		break;

	default:
		// could this happen?
		throw moBug(MO_ERROR_INTERNAL, "Unknown compression");
		/*NOTREACHED*/

	}
	if(context.header.xsize < 16) {
		/* the RLE compression doesn't work well with too small a width */
		context.header.type = SGI_TYPE_VERBATIM;
	}
#if SGI_MODE_V
#error "invalid magic test (SGI_MODE_V not zero?!?)"
#endif
	if(context.header.type == SGI_TYPE_RLE) {
		context.mode |= SGI_MODE_R;
	}
	/* DEPTH */
	switch(DefineDepth(im, true)) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
		context.header.zsize = 1;
		break;

	case MO_IMAGE_FILE_DEPTH_GREYA:
		context.header.zsize = 2;
		break;

	default:	//case MO_IMAGE_FILE_DEPTH_RGB:
		context.header.zsize = 3;
		break;

	case MO_IMAGE_FILE_DEPTH_RGBA:
		context.header.zsize = 4;
		break;

	}
	/* ENDIAN */

	/*
	 * Though little endian is a perfectly valid entry all the time
	 * it seems SGI banned little endian with 8 bpp... but why?!?
	 */
#if BYTE_ORDER == BIG_ENDIAN
	context.header.magic = 0x01DA;
#else
	context.header.magic = 0xDA01;
#endif
#if 0
	context.header.magic = 0x01DA;	/* this machine byte order */
	p = im.Parameters().Get("ENDIAN");
	if(p != 0) {
		if(*p == "BIG") {
#if BYTE_ORDER == BIG_ENDIAN
			context.header.magic = 0x01DA;
#else
			context.header.magic = 0xDA01;
#endif
		}
		else if(*p == "LITTLE") {
#if BYTE_ORDER == BIG_ENDIAN
			context.header.magic = 0xDA01;
#else
			context.header.magic = 0x01DA;
#endif
		}
		else if(*p == "NATIVE") {
			context.header.magic = 0x01DA;
		}
	}
#endif
#if SGI_MODE_E
#error "invalid magic test (SGI_MODE_E not zero?!?)"
#endif
	if(context.header.magic != 0x01DA) {
		context.mode |= SGI_MODE_W;
	}
	/* NAME (or FILENAME) */
	p = im.Parameters().Get("NAME");
	if(p == 0) {
		p = im.Parameters().Get("FILENAME");
	}
	if(p != 0) {
		p->MBData(context.header.name, sizeof(context.header.name));
	}

/* other parameters are created by software */
	min = 65535;
	max = 0;
	s = im.Data();
	sz = im.Size();
	while(sz > 0) {
		sz--;
		switch(context.header.zsize) {
		case 1:
		case 2:
			t = s->Grey();
			if(t < (int) min) {
				min = t;
			}
			if(t > (int) max) {
				max = t;
			}
			break;

		case 3:
		case 4:
			if(s->red < min) {
				min = s->red;
			}
			if(s->green < min) {
				min = s->green;
			}
			if(s->blue < min) {
				min = s->blue;
			}
			if(s->red > max) {
				max = s->red;
			}
			if(s->green > max) {
				max = s->green;
			}
			if(s->blue > max) {
				max = s->blue;
			}
			break;

		}
		switch(context.header.zsize) {
		case 2:
		case 4:
			if(s->alpha < min) {
				min = s->alpha;
			}
			if(s->alpha > max) {
				max = s->alpha;
			}
			break;

		}
		s++;
	}
	context.header.min = min;
	context.header.max = max;

/* save the header */
	/* same endian as this machine? */
	if(context.header.magic == 0x01DA) {
		/* write the header as is */
		file.Write(&context.header, sizeof(sgi_image_file_header));
	}
	else {
		/* swap shorts and longs */
		h = context.header;
		h.dim =   moSwap16Bits(h.dim);
		h.xsize = moSwap16Bits(h.xsize);
		h.ysize = moSwap16Bits(h.ysize);
		h.zsize = moSwap16Bits(h.zsize);
		h.min =   moSwap32Bits((uint32_t) h.min);
		h.max =   moSwap32Bits((uint32_t) h.max);
		file.Write(&h, sizeof(sgi_image_file_header));
	}

/* prepare the internal buffers */
	/* a buffer to quickly create scanlines in memory (even for verbatim images, that's much faster this way) */
	/* it is made twice as large as the source so if the compression is really bad, it still doesn't overflow it */
	context.scanlines = new unsigned char[context.header.xsize
			* context.header.ysize * context.header.zsize
					* context.header.bpp * 2];
	context.c = context.scanlines;

	/* compressed images need additional information */
	if(context.header.type == SGI_TYPE_RLE) {
		/*
		 * 1 long per scanline per component for offsets
		 * 1 long per scanline per component for lengths
		 * 1 short per pixel on one scanline for compress
		 */
		context.offset = new unsigned long[2 * context.header.ysize * context.header.zsize
							+ context.header.xsize * 2];
		context.length = context.offset + context.header.ysize * context.header.zsize;
		context.compress = reinterpret_cast<unsigned char *>(context.length
					+ context.header.ysize * context.header.zsize);
		context.t = context.compress;
	}

/* send the image in the different buffers */
	sgi_save_send(&context);

/* once the image was sent in buffers, write the buffers in the file */
	if(context.header.type == SGI_TYPE_RLE) {
		/* fix offsets (add header & offsets/lengths buffers) */
		min = context.header.ysize * context.header.zsize;
		max = context.header.ysize * context.header.zsize * sizeof(unsigned long) * 2 + 512;
		while(min > 0) {
			min--;
			context.offset[min] += max;
		}
		/* verify that endian matches, if no, swap all the offsets & lengths */
		if(context.header.magic != 0x01DA) {
			min = context.header.ysize * context.header.zsize;
			while(min > 0) {
				min--;
				context.offset[min] = moSwap32Bits((uint32_t) context.offset[min]);
				context.length[min] = moSwap32Bits((uint32_t) context.length[min]);
			}
		}
		/* in RLE you first save the offsets and lengths */
		file.Write(context.offset, context.header.ysize * context.header.zsize * sizeof(unsigned long));
		file.Write(context.length, context.header.ysize * context.header.zsize * sizeof(unsigned long));
	}
	file.Write(context.scanlines, context.c - context.scanlines);

	return true;
}


virtual const char *MimeType(void) const
{
	return "image/sgi";
}

virtual long MatchPriority(void) const
{
	return 50;		// perfectly valid, not used that much
}


virtual long MatchType(const void *buffer, unsigned long size) const
{
	const unsigned char *s = static_cast<const unsigned char *>(buffer);

	if((s[0] == 0x01 && s[1] == 0xDA)
	|| (s[0] == 0xDA && s[1] == 0x01)) {
		return 100;
	}

	return 0;
}



};		// class moImageFile_SGI


MAKE_IMAGEFILE_PUBLIC(SGI);


};			// namespace molib;

// vim: ts=8

