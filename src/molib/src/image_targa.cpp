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

	moImageFile_TARGA

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

	"DEPTH"		"PALETTED", "GREYSCALE", "RGB555", "RGB5551", "RGB", "RGBA"

	"COMMENT"	<string>
			(arbitrary user comment)


	Save()

	"COMMENT"	<string>
			(arbitrary user comment)

	"COMPRESSION"	"RLE"
			(other values are taken as NONE)

	"DEPTH"		"GREY", "RGB555", "RGB", "RGBA"
			(other values are taken as RGB if the
			image is solid, RGBA otherwise)

	"INTERLACE"	1, 2, 4 or 8
			(the number of lines to skip)

SEE ALSO

	MatchType, MatchPosition, MimeType

*/





class moImageFile_TARGA : public moImageFile
{


/* The targa header is 18 bytes which are not properly aligned
 * for processors such as MIPS or M680x0. Thus we read the
 * header in a buffer and expand it quickly.
 * The info below is what results from this expansion.
 * (see also the ToInfo() function below)
 */

// WARNING: the following is NOT the order in which the file is saved
struct tga_image_file_info {
	unsigned char	image_type;		// how the image was saved (0-15)
	unsigned char	compression;		// 0 - uncompressed, 1 - RLE compressed
	unsigned char	orientation;		// 0 - bottom to top, 1 - top to bottom
	unsigned char	attributes_size;	// 0 to 15 bits of attributes
	unsigned char	interleave;		// 1, 2, 4 or 8 scanlines (8 not supposed to be supported)
	unsigned char	interleave_shift;	// the interleave shift value

	unsigned char	colormap_type;		// the type of colormap if any
	unsigned char	colormap_bits_per_pixel;// the size of one pixel in the colormap
	unsigned char	colormap_bpp;		// bytes per pixels
	unsigned short	colormap_index;		// offset used to load colormap (a.k.a 1st color in colormap has this index)
	unsigned short	colormap_length;	// the number of entries in the colormap

	unsigned short	origin_x;		// position in the destination (?)
	unsigned short	origin_y;		// can be used as a hotspot as well
	unsigned short	width;			// the width of the image
	unsigned short	height;			// the height of the image
	unsigned char	bits_per_pixel;		// the depths in bits (8, 15, 16, 24, 32)
	unsigned char	bpp;			// bytes per pixels

	unsigned char	identifier_length;	// length of the comment
};

// to read/write the data from/to the header use these offsets
enum tga_offset_t {
	TGA_OFFSET_IDENTIFIER_LENGTH = 0,
	TGA_OFFSET_COLORMAP_TYPE,
	TGA_OFFSET_IMAGE_TYPE,			// includes compression flag
	TGA_OFFSET_COLORMAP_INDEX_LO,
	TGA_OFFSET_COLORMAP_INDEX_HI,
	TGA_OFFSET_COLORMAP_LENGTH_LO,
	TGA_OFFSET_COLORMAP_LENGTH_HI,
	TGA_OFFSET_COLORMAP_SIZE,
	TGA_OFFSET_ORIGIN_X_LO,
	TGA_OFFSET_ORIGIN_X_HI,
	TGA_OFFSET_ORIGIN_Y_LO,
	TGA_OFFSET_ORIGIN_Y_HI,
	TGA_OFFSET_WIDTH_LO,
	TGA_OFFSET_WIDTH_HI,
	TGA_OFFSET_HEIGHT_LO,
	TGA_OFFSET_HEIGHT_HI,
	TGA_OFFSET_BITS_PER_PIXEL,
	TGA_OFFSET_FLAGS,

	TGA_HEADER_SIZE			// the size of the targa files header
};



// the following returns true if the header is valid
// and therefore was fully decomposed in the info structure
static bool ToInfo(tga_image_file_info& info, const unsigned char *buffer, long length)
{
	if(length < TGA_HEADER_SIZE) {
		return false;
	}

	info.image_type = buffer[TGA_OFFSET_IMAGE_TYPE] & 7;
	info.compression = (buffer[TGA_OFFSET_IMAGE_TYPE] & 8) != 0;
	info.attributes_size = buffer[TGA_OFFSET_FLAGS] & 15;
	info.orientation = (buffer[TGA_OFFSET_FLAGS] & 32) != 0;
	info.interleave_shift = buffer[TGA_OFFSET_FLAGS] >> 6;
	info.interleave = 1 << info.interleave_shift;

	info.colormap_type = buffer[TGA_OFFSET_COLORMAP_TYPE];
	info.colormap_bits_per_pixel = buffer[TGA_OFFSET_COLORMAP_SIZE];
	info.colormap_index = buffer[TGA_OFFSET_COLORMAP_INDEX_LO]
				+ buffer[TGA_OFFSET_COLORMAP_INDEX_HI];
	info.colormap_length = buffer[TGA_OFFSET_COLORMAP_LENGTH_LO]
				+ buffer[TGA_OFFSET_COLORMAP_LENGTH_HI];

	info.origin_x = buffer[TGA_OFFSET_ORIGIN_X_LO]
				+ buffer[TGA_OFFSET_ORIGIN_X_HI] * 256;
	info.origin_y = buffer[TGA_OFFSET_ORIGIN_Y_LO]
				+ buffer[TGA_OFFSET_ORIGIN_Y_HI] * 256;
	info.width = buffer[TGA_OFFSET_WIDTH_LO]
				+ buffer[TGA_OFFSET_WIDTH_HI] * 256;
	info.height = buffer[TGA_OFFSET_HEIGHT_LO]
				+ buffer[TGA_OFFSET_HEIGHT_HI] * 256;
	info.bits_per_pixel = buffer[TGA_OFFSET_BITS_PER_PIXEL];

	info.identifier_length = buffer[TGA_OFFSET_IDENTIFIER_LENGTH];

	// make sure the image isn't empty
	if(info.width == 0 || info.height == 0) {
		return false;
	}

	// limit the color map to 16 bits
	if(info.colormap_index + info.colormap_length > 65535) {
		return false;
	}

	// for colormap bit depths we support only the following
	switch(info.colormap_bits_per_pixel) {
	case 15:
	case 16:
	case 24:
	case 32:
		break;

	case 0:			// valid if we don't have a colormap
		if(info.colormap_type == 0) {
			break;
		}
	default:
		return false;

	}

	// we need to cross reference many parameters to make sure that
	// they are valid. The fastest is to concatenate them in a long
	// and do a switch as follow
#define	TGA_MODE(image_type, colormap_type, bits_per_pixel)	\
			(image_type) + (colormap_type << 4) + (bits_per_pixel << 12)
	switch(TGA_MODE(info.image_type, info.colormap_type, info.bits_per_pixel)) {
	// paletted images
	case TGA_MODE(1, 1, 8):
	case TGA_MODE(1, 1, 15):
	case TGA_MODE(1, 1, 16):

	// RGB images
	case TGA_MODE(2, 0, 15):
	case TGA_MODE(2, 0, 16):
	case TGA_MODE(2, 0, 24):
	case TGA_MODE(2, 0, 32):

	case TGA_MODE(2, 1, 15):	// accept palettes - ignored but must be skipped
	case TGA_MODE(2, 1, 16):
	case TGA_MODE(2, 1, 24):
	case TGA_MODE(2, 1, 32):

	// gray scale images
	case TGA_MODE(3, 0, 8):
	case TGA_MODE(3, 1, 8):		// accept palettes - ignored but must be skipped
		break;

	default:
		return false;

	}

	info.colormap_bpp = (info.colormap_bits_per_pixel + 7) / 8;
	info.bpp = (info.bits_per_pixel + 7) / 8;

	return true;
}




void RawToRGBA(tga_image_file_info& info, moRGBA *d, const unsigned char *s, unsigned long length, unsigned int bits_per_pixel, const moRGBA *palette)
{
	long		idx;

	switch(bits_per_pixel) {
	case 0:				// we change the "bits_per_pixel" to zero when we have a palleted format
		do {
			idx = *s++ - info.colormap_index;
			if(idx >= 0 && idx < info.colormap_length) {
				*d++ = palette[idx];
			}
			length--;
		} while(length > 0);
		break;

	case 1:				// we change the "bits_per_pixel" to zero when we have a palleted format
		do {
			idx = s[0] + (s[1] & 0x7F) * 256 - info.colormap_index;
			if(idx >= 0 && idx < info.colormap_length) {
				*d++ = palette[idx];
			}
			length--;
		} while(length > 0);
		break;

	case 2:				// we change the "bits_per_pixel" to zero when we have a palleted format
		do {
			idx = s[0] + s[1] * 256 - info.colormap_index;
			if(idx >= 0 && idx < info.colormap_length) {
				*d++ = palette[idx];
			}
			length--;
		} while(length > 0);
		break;

	case 8:
		do {
			d->red = d->green = d->blue = s[0];
			s++;
			d++;
			length--;
		} while(length > 0);
		break;

	case 15:
		do {
			d->red   = mo5BitsTo8Bits[(s[1] >> 2) & 31];
			d->green = mo5BitsTo8Bits[((s[1] << 3) | (s[0] >> 5)) & 31];
			d->blue  = mo5BitsTo8Bits[s[0] & 31];
			d->alpha = 255;
			s += 2;
			d++;
			length--;
		} while(length > 0);
		break;

	case 16:
		do {
			d->red   = mo5BitsTo8Bits[(s[1] >> 2) & 31];
			d->green = mo5BitsTo8Bits[((s[1] << 3) | (s[0] >> 5)) & 31];
			d->blue  = mo5BitsTo8Bits[s[0] & 31];
			d->alpha = (signed char) s[1] < 0 ? 128 : 255;	// save attribute as alpha 128
			s += 2;
			d++;
			length--;
		} while(length > 0);
		break;

	case 24:
		do {
			d->red   = s[2];
			d->green = s[1];
			d->blue  = s[0];
			d->alpha = 255;
			s += 3;
			d++;
			length--;
		} while(length > 0);
		break;

	case 32:
		do {
			d->red   = s[2];
			d->green = s[1];
			d->blue  = s[0];
			d->alpha = s[3];
			s += 4;
			d++;
			length--;
		} while(length > 0);
		break;

	}
}


bool Load(moIStream& file, moImage& im)
{
	tga_image_file_info		info;
	unsigned char			buf[TGA_HEADER_SIZE], *b;	/* Flawfinder: ignore */
	std::auto_ptr<unsigned char>	bitmap;
	moRGBA				*d;
	std::auto_ptr<moRGBA>		palette;
	unsigned long			size, l, r, q, y;
	bool				relax, result;
	moVariable			*p;

// relax?
	p = im.Parameters().Get("RELAX");
	if(p != 0) {
		relax = p->Integer() != 0;
	}
	else {
		relax = true;
	}

// read and validate header
	l = file.Read(buf, TGA_HEADER_SIZE);
	if(!ToInfo(info, buf, l)) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

// get the depth to the end user
	switch(info.bits_per_pixel) {
	case 8:
		if(info.image_type == 1) {
			im.Parameters().Set("DEPTH", "PALETTED");
		}
		else {
			im.Parameters().Set("DEPTH", "GREYSCALE");
		}
		break;

	case 15:
		im.Parameters().Set("DEPTH", "RGB555");
		break;

	case 16:
		im.Parameters().Set("DEPTH", "RGB5551");
		break;

	case 24:
		im.Parameters().Set("DEPTH", "RGB");
		break;

	case 32:
		im.Parameters().Set("DEPTH", "RGBA");
		break;

	}

// header is valid, check for an identifier
	if(info.identifier_length > 0) {
		l = file.Read(buf, info.identifier_length);
		if(l != info.identifier_length) {
			im.LastErrno(MO_ERROR_IO);
			return false;
		}
		buf[l] = '\0';
		im.Parameters().Set("COMMENT", reinterpret_cast<char *>(buf));
	}

// allocate a row (also used to read the palette)
	l = size = info.width * info.height * info.bpp;	// allocate the entire bitmap at once!
	if(l < 256 * 4) {
		l = 256 * 4;		// at least enough room for the palette
	}
	bitmap.reset(new unsigned char[l]);

// is there a colormap?
// if the image type is not 1, then just skip it
	if(info.colormap_type != 0) {
		l = info.colormap_bpp * info.colormap_length;
		if(info.image_type == 1) {
			r = file.Read(bitmap.get(), l);
			if(r != l) {
				im.LastErrno(MO_ERROR_IO);
				return false;
			}
			palette.reset(new moRGBA[info.colormap_length]);
			RawToRGBA(info, palette.get(), bitmap.get(), info.colormap_length, info.colormap_bits_per_pixel, 0);
			if(info.bits_per_pixel == 8) {
				info.bits_per_pixel = 0;	// we know it's 8
			}
			else {
				info.bits_per_pixel -= 14;	// we know it's 15 or 16
			}
		}
		else {
			// quick skip
			file.ReadPosition(file.ReadPosition() + l);
		}
	}

// read the bitmap
	if(info.compression == 1) {
		// compressed data need a bit of work
		im.Parameters().Set("COMPRESSION=RLE");
		result = true;
		b = bitmap.get();
		y = info.height * info.width;
		do {
			if(file.Read(buf, 1) == 0) {
				result = false;
				break;
			}
			if(static_cast<char>(*buf) < 0) {
				// repeat the next pixel multiple times
				l = *buf - 127;
				if(l > y) {
					if(!relax) {
						result = false;
						break;
					}
					l = y;
				}
				if(file.Read(b, info.bpp) != info.bpp) {
					result = false;
					break;
				}
				y -= l;
				l--;
				while(l > 0) {
					memcpy(b + info.bpp, b, info.bpp);
					b += info.bpp;
					l--;
				}
				b += info.bpp;	// we didn't skip that one yet!
			}
			else {
				// read pixels at once
				l = *buf + 1;
				if(l > y) {
					if(!relax) {
						result = false;
						break;
					}
					l = y;
				}
				r = l * info.bpp;
				if(file.Read(b, r) != static_cast<int>(r)) {
					result = false;
					break;
				}
				b += r;
				y -= l;
			}
		} while(y > 0);
	}
	else {
		im.Parameters().Set("COMPRESSION=NONE");
		result = file.Read(bitmap.get(), size) == static_cast<int>(size);
	}
	if(result) {
		im.SetSize(info.width, info.height);
		b = bitmap.get();
		y = info.height;
		l = info.width * info.bpp;
		if(info.interleave == 1) {
			if(info.orientation == 0) {
				d = im.Data();
				do {
					RawToRGBA(info, d, b, info.width, info.bits_per_pixel, palette.get());
					b += l;
					d += info.width;
					y--;
				} while(y > 0);
			}
			else {
				d = im.Data() + im.Size();
				do {
					d -= info.width;
					RawToRGBA(info, d, b, info.width, info.bits_per_pixel, palette.get());
					b += l;
					y--;
				} while(y > 0);
			}
		}
		else {
			// interleave must be really rare, use a "slow" loop
			q = im.Size() / info.interleave;
			do {
				if(info.orientation == 0) {
					size = y;
				}
				else {
					size = info.height - 1 - y;
				}
				r = size & (info.interleave - 1);
				size /= info.interleave;
				d = im.Data() + q * r + size * info.width;
				RawToRGBA(info, d, b, info.width, info.bits_per_pixel, palette.get());
				b += l;
				y--;
			} while(y > 0);
		}
	}
	else {
		im.LastErrno(MO_ERROR_INVALID);
	}

	return result;
}





void RGBAToRaw(unsigned char *s, const moRGBA *d, unsigned long length, unsigned int bits_per_pixel)
{
	long		l;

	switch(bits_per_pixel) {
	case 8:
		do {
			s[0] = d->Grey();
			s++;
			d++;
			length--;
		} while(length > 0);
		break;

	case 15:		// we shouldn't use this... just in case!
	case 16:
		do {
			l = ((d->red & 0xF8) << 7) + ((d->green & 0xF8) << 2) + (d->blue >> 3);
			s[0] = static_cast<unsigned char>(l);
			s[1] = static_cast<unsigned char>(l >> 8);
			s += 2;
			d++;
			length--;
		} while(length > 0);
		break;

	case 24:
		do {
			s[2] = d->red;
			s[1] = d->green;
			s[0] = d->blue;
			s += 3;
			d++;
			length--;
		} while(length > 0);
		break;

	case 32:
		do {
			s[2] = d->red;
			s[1] = d->green;
			s[0] = d->blue;
			s[3] = d->alpha;
			s += 4;
			d++;
			length--;
		} while(length > 0);
		break;

	}
}








bool Save(moOStream& file, const moImage& im)
{
	moVariable			*p, *comment;
	tga_image_file_info		info;
	unsigned char			header[TGA_HEADER_SIZE], *b, *e, *s, c;	/* Flawfinder: ignore */
	std::auto_ptr<unsigned char>	bitmap;
	char				cmt[256];				/* Flawfinder: ignore */
	unsigned long			size, line, l, r, q, y, limit;
	const moRGBA			*d;
	bool				result;

	if(im.IsEmpty()) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	memset(&info, 0, sizeof(info));

	info.compression = 0;		// by default, keep file uncompressed (most loaders don't understand compressed TARGA's)
	info.width = static_cast<unsigned short>(im.Width());
	info.height = static_cast<unsigned short>(im.Height());
	info.interleave = 1;

/* used wants compression? */
	/* COMPRESSION */
	switch(DefineCompression(im)) {
	case MO_IMAGE_FILE_COMPRESSION_NONE:
	case MO_IMAGE_FILE_COMPRESSION_UNKNOWN:
		break;

	default:
		info.compression = 1;
		break;

	}

/* any comment? */
	/* COMMENT */
	comment = im.Parameters().Get("COMMENT");
	if(comment != 0) {
		l = static_cast<unsigned long>(comment->MBLength());
		if(l > 255) {
			l = 255;
		}
		info.identifier_length = static_cast<unsigned char>(l);
	}

/* asked for interleave!?!? */
	/* INTERLACE */
	p = im.Parameters().Get("INTERLACE");
	if(p != 0) {
		switch(p->Integer()) {
		//case 1:	-- the default
			//info.interleave_shift = 0;
			//info.interleave = 1;
			//break;

		case 2:
			info.interleave_shift = 1;
			info.interleave = 2;
			break;

		case 4:
			info.interleave_shift = 2;
			info.interleave = 4;
			break;

		case 8:
			info.interleave_shift = 3;
			info.interleave = 8;
			break;

		}
	}

/* define the depth */
	switch(DefineDepth(im, true)) {
	case MO_IMAGE_FILE_DEPTH_GREYSCALE:
		info.bits_per_pixel = 8;
		break;

	case MO_IMAGE_FILE_DEPTH_RGB555:
	case MO_IMAGE_FILE_DEPTH_RGB5551:	/* this is 16 bits... */
		info.bits_per_pixel = 16;
		break;

	default:		//case MO_IMAGE_FILE_DEPTH_RGB:
		info.bits_per_pixel = 24;
		break;

	case MO_IMAGE_FILE_DEPTH_RGBA:
		info.bits_per_pixel = 32;
		break;

	}

	info.image_type = info.bits_per_pixel == 8 ? 3 : 2;

/* put the info in a TARGA header to be saved */
	header[TGA_OFFSET_IMAGE_TYPE] = info.image_type + (info.compression == 1 ? 8 : 0);
	header[TGA_OFFSET_FLAGS] = (info.orientation == 0 ? 0 : 32) + (info.interleave_shift << 6);
	header[TGA_OFFSET_COLORMAP_TYPE] = info.colormap_type;
	header[TGA_OFFSET_COLORMAP_SIZE] = info.colormap_bits_per_pixel;
	header[TGA_OFFSET_COLORMAP_INDEX_LO] = static_cast<unsigned char>(info.colormap_index);
	header[TGA_OFFSET_COLORMAP_INDEX_HI] = static_cast<unsigned char>(info.colormap_index / 256);
	header[TGA_OFFSET_COLORMAP_LENGTH_LO] = static_cast<unsigned char>(info.colormap_length);
	header[TGA_OFFSET_COLORMAP_LENGTH_HI] = static_cast<unsigned char>(info.colormap_length / 256);
	header[TGA_OFFSET_ORIGIN_X_LO] = static_cast<unsigned char>(info.origin_x);
	header[TGA_OFFSET_ORIGIN_X_HI] = static_cast<unsigned char>(info.origin_x / 256);
	header[TGA_OFFSET_ORIGIN_Y_LO] = static_cast<unsigned char>(info.origin_y);
	header[TGA_OFFSET_ORIGIN_Y_HI] = static_cast<unsigned char>(info.origin_y / 256);
	header[TGA_OFFSET_WIDTH_LO] = static_cast<unsigned char>(info.width);
	header[TGA_OFFSET_WIDTH_HI] = static_cast<unsigned char>(info.width / 256);
	header[TGA_OFFSET_HEIGHT_LO] = static_cast<unsigned char>(info.height);
	header[TGA_OFFSET_HEIGHT_HI] = static_cast<unsigned char>(info.height / 256);
	header[TGA_OFFSET_BITS_PER_PIXEL] = info.bits_per_pixel;
	header[TGA_OFFSET_IDENTIFIER_LENGTH] = info.identifier_length;

	if(file.Write(header, TGA_HEADER_SIZE) != TGA_HEADER_SIZE) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	if(info.identifier_length != 0) {
		// the null terminator is not saved
		file.Write(comment->MBData(cmt, sizeof(cmt)), info.identifier_length);
	}

	info.bpp = (info.bits_per_pixel + 7) / 8;
	size = info.width * info.height * info.bpp;
	bitmap.reset(new unsigned char[size]);

	b = bitmap.get();
	y = info.height;
	l = info.width * info.bpp;
	if(info.interleave == 1) {
		if(info.orientation == 0) {
			d = im.Data();
			do {
				RGBAToRaw(b, d, info.width, info.bits_per_pixel);
				b += l;
				d += info.width;
				y--;
			} while(y > 0);
		}
		else {
			d = im.Data() + im.Size();
			do {
				d -= info.width;
				RGBAToRaw(b, d, info.width, info.bits_per_pixel);
				b += l;
				y--;
			} while(y > 0);
		}
	}
	else {
		// who wants to save a targa in interleave mode?!?
		q = im.Size() / info.interleave;
		do {
			if(info.orientation == 0) {
				line = y;
			}
			else {
				line = info.height - 1 - y;
			}
			r = line & (info.interleave - 1);
			line /= info.interleave;
			d = im.Data() + q * r + line * info.width;
			RGBAToRaw(b, d, info.width, info.bits_per_pixel);
			b += l;
			y--;
		} while(y > 0);
	}

	if(info.compression == 0) {
		// verbatim, save at once
		result = file.Write(bitmap.get(), size) == static_cast<int>(size);
		if(!result) {
			im.LastErrno(MO_ERROR_IO);
		}
	}
	else {
		// RLE compression - do it at once
		result = true;
		limit = 128 * info.bpp;
		b = bitmap.get();
		e = b + size;
		while(b < e) {
			/* 1. repeatitions (pixels are equal) */
			s = b + info.bpp;
			c = *s;
			while(s < e && s - b < 128 && memcmp(b, s, info.bpp) == 0) {
				s += info.bpp;
			}
			if(s - b > info.bpp) {
				/* worth it! */
				c = static_cast<unsigned char>(((s - b) / info.bpp - 1) | 0x80);
				if(file.Write(&c, 1) != 1) {
					im.LastErrno(MO_ERROR_IO);
					result = false;
					break;
				}
				if(file.Write(b, info.bpp) != info.bpp) {
					im.LastErrno(MO_ERROR_IO);
					result = false;
					break;
				}
				/* well, try again! we may have been limited by the count */
				b = s;
				continue;
			}
			/* 2. copy (pixels differ) */
			while(s + info.bpp < e && s - b < static_cast<long>(limit) && memcmp(s, s + info.bpp, info.bpp) != 0) {
				s += info.bpp;
			}
			/* worth keeping a repeatition after this copy? */
			if(s + info.bpp * 2 >= e) {
				s = e;
				// make sure we don't overflow the counter
				if(s - b > static_cast<long>(limit)) {
					s = b + limit;
				}
			}
			l = static_cast<unsigned long>(s - b);
			c = static_cast<unsigned char>(l / info.bpp - 1);
			if(file.Write(&c, 1) != 1) {
				im.LastErrno(MO_ERROR_IO);
				result = false;
				break;
			}
			if(file.Write(b, l) != static_cast<int>(l)) {
				im.LastErrno(MO_ERROR_IO);
				result = false;
				break;
			}
			b = s;
		}
	}

	return true;
}


virtual const char *MimeType(void) const
{
	return "image/tga";
}

virtual long MatchPriority(void) const
{
	return -75;		// not really that great... test at last if possible
}


virtual long MatchType(const void *buffer, unsigned long size) const
{
	tga_image_file_info	info;

	// if it looks valid, give it a 20%
	return ToInfo(info, static_cast<const unsigned char *>(buffer), size) ? 20 : 0;
}



};		// class moImageFile_TARGA


MAKE_IMAGEFILE_PUBLIC(TARGA);


};			// namespace molib;

// vim: ts=8

