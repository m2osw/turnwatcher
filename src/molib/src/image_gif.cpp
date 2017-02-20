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



namespace molib
{



/************************************************************ DOC:

CLASS

	moImageFile_GIF

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
class moImageFile_GIF : public moImageFile
{


struct gif_header {
	char		magic[6];	/* Flawfinder: ignore */
	short		width;
	short		height;
	char		colors;
	char		background;
	char		aspect;
};
#define	SIZEOF_HEADER		(13)


struct gif_image {
	short		x;
	short		y;
	short		width;
	short		height;
	char		flags;
};
#define	SIZEOF_IMAGE		9


struct gif_extension {
	char		extension;	// == '!' (until 0)
	char		code;		// the extension code
	struct array {			// this is repeated until size == 0
		char		size;
		//char		data[size];
	};
};


struct gif_decoder {
	long		length;
	void *		string;
};
#define	SIZEOF_DECODER		0x1000



unsigned char *CopyOutput(moRGBA *d, long screen_width, long width, long height,
					const unsigned char *cmap, unsigned char *out,
					bool background, long transparent_color)
{
	long		w, color;

	screen_width += width;
	while(height > 0) {
		w = width;
		while(w > 0) {
			w--;
			color = *out;
			if(transparent_color == color) {
				if(background) {
					/* in case this is the background, then draw it anyway */
					color *= 3;
					d->red   = cmap[color + 0];
					d->green = cmap[color + 1];
					d->blue  = cmap[color + 2];
					d->alpha = 0;
				}
			}
			else {
				color *= 3;
				d->red   = cmap[color + 0];
				d->green = cmap[color + 1];
				d->blue  = cmap[color + 2];
				d->alpha = 255;
			}
			out++;
			d++;
		}
		d -= screen_width;
		height--;
	}

	return out;
}



bool LoadImage(moIStream& file, moImage& im, unsigned char * const output, unsigned char *global_cmap, unsigned long main_cmap_size)
{
	gif_image	image;
	unsigned char	c;
	unsigned char	img_cmap[256 * 3], *cmap;	/* Flawfinder: ignore */
	unsigned char	*endout, *old, *out;
	long		cmap_size, startcs, cs, transparent_color;
	long		bc, reset, eoi, maxcs, code, cnt, mask, size;
	unsigned long	nc;
	moRGBA		*d;
	bool		err, background;
	gif_decoder	table[SIZEOF_DECODER];

	transparent_color = -1;
	background = true;		// the very first entry is the background

/* read until EOF or first EOI */
	while(file.Read(&c, 1) == 1) {
		if(c == ',') {		// there is a new image
			/* read the image info */
			if(file.Read(&image, SIZEOF_IMAGE) != SIZEOF_IMAGE) {
				im.LastErrno(MO_ERROR_IO);
				return false;
			}
#if BYTE_ORDER == BIG_ENDIAN
			image.x =      moSwap16Bits(image.x);
			image.y =      moSwap16Bits(image.y);
			image.width =  moSwap16Bits(image.width);
			image.height = moSwap16Bits(image.height);
#endif
			if(image.width  <= 0 || (unsigned long) (image.x + image.width)  > im.Width()
			|| image.height <= 0 || (unsigned long) (image.y + image.height) > im.Height()) {
				im.LastErrno(MO_ERROR_IO);
				return false;
			}

			/* define the colormap */
			if((image.flags & 0x80) != 0) {
				cmap_size = 2 << (image.flags & 7);
				if(file.Read(img_cmap, cmap_size * 3) != cmap_size * 3) {
					im.LastErrno(MO_ERROR_IO);
					return false;
				}
				cmap = img_cmap;
			}
			else {
				if(global_cmap == 0) {
					// no global nor local colormap!
					im.LastErrno(MO_ERROR_INVALID);
					return false;
				}
				cmap_size = main_cmap_size;
				cmap = global_cmap;
			}

			/* here we may have extensions to skip */
			if(file.Read(&c, 1) != 1) {
				im.LastErrno(MO_ERROR_IO);
				return false;
			}
			if(c == '!') {
				c = 1;
				do {
					file.ReadPosition(file.ReadPosition() + c);
					if(file.Read(&c, 1) != 1) {
						im.LastErrno(MO_ERROR_IO);
						return false;
					}
				} while(c != 0);
				if(file.Read(&c, 1) != 1) {
					im.LastErrno(MO_ERROR_IO);
					return false;
				}
			}

			startcs = cs = c + 1;
			reset = 1 << c;
			cnt = reset;
			while(cnt > 0) {
				cnt--;
				table[cnt].length = 1;
				//table[cnt].string = 0;
			}
			// cnt = 0;	-- already zero
			eoi = reset + 1;
			maxcs = reset + 2;
			mask = reset * 2 - 1;
			old = 0;
			out = output;
			endout = out + im.Size();	// in bytes!
			bc = 0;
			nc = 0;
			err = false;
			for(;;) {
				while(bc < cs) {
					if(cnt == 0) {
						if(file.Read(&c, 1) != 1) {
							im.LastErrno(MO_ERROR_IO);
							return false;
						}
						if(c == 0) {
							// if err is true then we found an
							// EOI but the whole image wasn't read
							err = out - output < static_cast<long>(im.Size());
							goto decompacted;
						}
						cnt = c;
					}
					cnt--;
					if(file.Read(&c, 1) != 1) {
						im.LastErrno(MO_ERROR_IO);
						return false;
					}
					nc |= c << bc;
					bc += 8;
				}
				// strange case in 89a
				if(cs == 13) {
					code = nc & 0x0FFF;
					if(code != reset && code != eoi) {
						// EOI missing
						im.LastErrno(MO_ERROR_INVALID);
						err = true;
						break;
					}
					cs = 12;
					mask = 0x0FFF;
				}
				bc -= cs;
				code = nc & mask;
				nc >>= cs;

				// restart?
				if(code == reset) {
					cs = startcs;
					mask = (1 << cs) - 1;
					maxcs = reset + 2;
					old = 0;
					continue;
				}

				// end of image?
				if(code == eoi) {
					do {
						// skip following blocks
						file.ReadPosition(file.ReadPosition() + cnt);
						if(file.Read(&c, 1) != 1) {
							im.LastErrno(MO_ERROR_IO);
							return false;
						}
						cnt = c + 1;
					} while(c != 0);
					break;
				}

				// verify that the code is valid
				if(code > maxcs) {
					// the code can't be larger or we can't know what it represents
					im.LastErrno(MO_ERROR_INVALID);
					err = true;
					break;
				}

				// reached the current limit
				if(code == maxcs) {		/* new code? */
					if(old == 0) {
						// old should never be zero here!
						im.LastErrno(MO_ERROR_INVALID);
						err = true;
						break;
					}
					size = static_cast<long>(out - old);
					if(out + size + 1 > endout) {
						// overflow
						im.LastErrno(MO_ERROR_INVALID);
						err = true;
						break;
					}
					memcpy(out, old, size);	/* Flawfinder: ignore */
					out[size] = *old;
					old = out;
					size++;
					out += size;
					table[maxcs].string = old;
					table[maxcs].length = size;
/*
      (no: [...] <- translation for <old>;
           K <- first character of [...];
           output [...]K to charstream and add it to string table;
           <old> <- <code>
      )
*/
				}
				else if(old == 0) {
					*out = static_cast<unsigned char>(code);
					old = out;
					out++;
					continue;
				}
				else {
					size = table[code].length;
					if(out + size > endout) {
						// overflow
						im.LastErrno(MO_ERROR_INVALID);
						err = true;
						break;
					}
					if(code < reset) {
						*out = static_cast<unsigned char>(code);
					}
					else {
						memcpy(out, table[code].string, size);	/* Flawfinder: ignore */
					}
					table[maxcs].string = old;
					table[maxcs].length = static_cast<long>(out - old + 1);
					old = out;
					out += size;
/*
      (yes: output the string for <code> to the charstream;
            [...] <- translation for <old>;
            K <- first character of translation for <code>;
            add [...]K to the string table;
            <old> <- <code>;
      )
*/
				}
				if(maxcs == mask) {
					mask = (mask << 1) + 1;
					cs++;
				}
				maxcs++;
			}
decompacted:
			d = im.Data() + image.x;
			image.y = static_cast<short>(im.Height() - image.y - 1);
			if((image.flags & 0x40) != 0) {
/*
		   GIF Interlacing Chart
   Image
   Row	Pass 1	Pass 2	Pass 3	Pass 4		Result
   ---------------------------------------------------
     0	**1a**					**1a**
     1				**4a**		**4a**
     2			**3a**			**3a**
     3				**4b**		**4b**
     4		**2a**				**2a**
     5				**4c**		**4c**
     6			**3b**			**3b**
     7				**4d**		**4d**
     8	**1b**					**1b**
     9				**4e**		**4e**
    10			**3c**			**3c**
    11				**4f**		**4f**
    12		**2b**				**2b**
   . . .
*/

/*
 * NOTE: we are deinterlacing backward since the images in a GIF image
 * are top to bottom
 */
					/* Pass 1 */
				out = CopyOutput(d + (image.y + 0) * im.Width(),
					im.Width() * 8, image.width, (image.height + 7) / 8,
					cmap, output, background, transparent_color);

					/* Pass 2 */
				out = CopyOutput(d + (image.y - 4) * im.Width(),
					im.Width() * 8, image.width, (image.height + 3) / 8,
					cmap, out, background, transparent_color);

					/* Pass 3 */
				out = CopyOutput(d + (image.y - 2) * im.Width(),
					im.Width() * 4, image.width, (image.height + 1) / 4,
					cmap, out, background, transparent_color);

					/* Pass 4 */
				CopyOutput(d + (image.y - 1) * im.Width(),
					im.Width() * 2, image.width, (image.height - 1) / 2,
					cmap, out, background, transparent_color);
			}
			else {
				CopyOutput(d + image.y * im.Width(),
					im.Width(), image.width, image.height,
					cmap, output, background, transparent_color);
			}

			if(err) {
				break;
			}
			background = false;	// any other image is not a background
		}
		else if(c == '!') {
			if(file.Read(&c, 1) != 1) {
				im.LastErrno(MO_ERROR_IO);
				return false;
			}
			cnt = c;
			c = 0;
			do {
				file.ReadPosition(file.ReadPosition() + c);
				if(file.Read(&c, 1) != 1) {
					im.LastErrno(MO_ERROR_IO);
					return false;
				}
				if(cnt == 0xF9 && c > 3) {
					if(file.Read(&cs, 4) != 4) {
						im.LastErrno(MO_ERROR_IO);
						return false;
					}
					c -= 4;
#if BYTE_ORDER == BIG_ENDIAN
					transparent_color = cs & 255;
#else
					transparent_color = (cs >> 24) & 255;
#endif
					continue;
				}
			} while(c != 0);
			if(file.Read(&c, 1) != 1) {
				im.LastErrno(MO_ERROR_IO);
				return false;
			}
		}
	}

	return true;
}




bool Load(moIStream& file, moImage& im)
{
	gif_header			header;
	//gif_image			image;
	unsigned char			main_cmap[256 * 3], *global_cmap;	/* Flawfinder: ignore */
	std::auto_ptr<unsigned char>	out;
	long				main_cmap_size;

	if(file.Read(&header, SIZEOF_HEADER) != SIZEOF_HEADER) {
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

/* really a GIF? */
	if(header.magic[0] != 'G' || header.magic[1] != 'I' || header.magic[2] != 'F'
	|| header.magic[3] != '8' || (header.magic[4] != '7' && header.magic[4] != '9')
	|| header.magic[5] != 'a') {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

/* get the image size */
#if BYTE_ORDER == BIG_ENDIAN
	header.width =  moSwap16Bits(header.width);
	header.height = moSwap16Bits(header.height);
#endif
	if(header.width <= 0 || header.height <= 0) {
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	im.SetSize(header.width, header.height);

/* a main colormap? */
	if((header.colors & 0x80) != 0) {
		// read the main colormap
		main_cmap_size = 2 << (header.colors & 7);
		if(file.Read(main_cmap, main_cmap_size * 3) != main_cmap_size * 3) {
			im.LastErrno(MO_ERROR_IO);
			return false;
		}
		global_cmap = main_cmap;
	}
	else {
		main_cmap_size = 0;
		global_cmap = 0;
	}

	out.reset(new unsigned char[header.width * header.height]);
	return LoadImage(file, im, out.get(), global_cmap, main_cmap_size);
}









bool Save(moOStream& file, const moImage& im)
{
	return false;
}



virtual const char *MimeType(void) const
{
	return "image/gif";
}

virtual long MatchPriority(void) const
{
	return 99;
}


virtual long MatchType(const void *buffer, unsigned long size) const
{
	const unsigned char	*s = static_cast<const unsigned char *>(buffer);
	short			width, height;

// check out the marker + header
	if(size < 13) {
		return 0;
	}

// is this a GIF87a or GIF89a magic number?
	if(s[0] != 'G' || s[1] != 'I' || s[2] != 'F'
	|| s[3] != '8' || (s[4] != '7' && s[4] != '9') || s[5] != 'a') {
		return 0;
	}

// get some header parameters
	width = moReadLE16Bits(s + 6);
	height = moReadLE16Bits(s + 8);
	if(width <= 0 || height <= 0) {
		return 0;
	}

	return 100;
}



};		// class moImageFile_GIF


MAKE_IMAGEFILE_PUBLIC(GIF);


}			// namespace molib;

// vim: ts=8

