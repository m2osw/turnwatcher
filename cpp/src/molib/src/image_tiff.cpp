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

#include	<tiffio.h>



namespace molib
{


/************************************************************ DOC:

CLASS

	moImageFile_TIFF

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

	none


	Save()

	none

SEE ALSO

	MatchType, MatchPosition, MimeType

*/


extern "C" {


static tsize_t moRead(thandle_t file, tdata_t data, tsize_t size)
{
	return reinterpret_cast<moIStream *>(file)->Read(data, size);
}


static tsize_t moWrite(thandle_t file, tdata_t data, tsize_t size)
{
	return reinterpret_cast<moOStream *>(file)->Write(data, size);
}


static toff_t moReadSeek(thandle_t file, toff_t offset, int hence)
{
	moIStream& f = *reinterpret_cast<moIStream *>(file);

	switch(hence) {
	case SEEK_SET:
		break;

	case SEEK_CUR:
		offset += static_cast<toff_t>(f.ReadPosition());
		break;

	case SEEK_END:
		offset += static_cast<toff_t>(f.InputSize());
		break;

	}
	f.ReadPosition(offset);
	return (toff_t) f.ReadPosition();
}


static toff_t moWriteSeek(thandle_t file, toff_t offset, int hence)
{
	moOStream &f = *reinterpret_cast<moOStream *>(file);

	switch(hence) {
	case SEEK_SET:
		break;

	case SEEK_CUR:
		offset += static_cast<toff_t>(f.WritePosition());
		break;

	case SEEK_END:
		offset += static_cast<toff_t>(f.OutputSize());
		break;

	}
	f.WritePosition(offset);
	return (toff_t) f.WritePosition();
}


static int moClose(thandle_t file)
{
	// we can't close the file here anyway...
	return 0;
}


static toff_t moInputSize(thandle_t file)
{
	return static_cast<toff_t>(reinterpret_cast<moIStream *>(file)->InputSize());
}


static toff_t moOutputSize(thandle_t file)
{
	return static_cast<toff_t>(reinterpret_cast<moOStream *>(file)->OutputSize());
}






}		// extern "C"



class moImageFile_TIFF : public moImageFile
{
public:
moImageFile_TIFF(void)
{
	//TIFFSetErrorHandler(0);
	//TIFFSetWarningHandler(0);
}


private:
virtual bool Load(moIStream& file, moImage& im)
{
	TIFF		*tiff;
	uint32		width;
	uint32		height;

	tiff = TIFFClientOpen(
			file.InputFilename(),
			"rm",
			reinterpret_cast<thandle_t>(&file),
			moRead,
			moWrite,
			moReadSeek,
			moClose,
			moInputSize,
			0,
			0);
	if(tiff == 0) {
		// couldn't open the TIFF file
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,  &width);
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);

	if(static_cast<long>(width) <= 0 && static_cast<long>(height) <= 0) {
		TIFFClose(tiff);
		im.LastErrno(MO_ERROR_INVALID);
		return false;
	}

	im.SetSize(width, height);

	// this call may return 0 meaning some error occured, but we
	// keep it as is so we get the most we can get
	TIFFReadRGBAImage(tiff, width, height, reinterpret_cast<uint32 *>(im.Data()), false);

	TIFFClose(tiff);

	return true;
}





virtual bool Save(moOStream& file, const moImage& im)
{
	TIFF		*tiff;
	const moRGBA	*d;
	unsigned long	y;

	tiff = TIFFClientOpen(
			file.OutputFilename(),
			"wm",
			reinterpret_cast<thandle_t>(&file),
			moRead,
			moWrite,
			moWriteSeek,
			moClose,
			moOutputSize,
			0,
			0);
	if(tiff == 0) {
		// couldn't open the TIFF file
		im.LastErrno(MO_ERROR_IO);
		return false;
	}

	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH,      static_cast<uint32>(im.Width()));
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH,     static_cast<uint32>(im.Height()));
	//TIFFSetField(tiff, TIFFTAG_IMAGEDEPTH,      static_cast<uint32>(4));
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE,   static_cast<uint16>(8));
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, static_cast<uint16>(4));
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG,    static_cast<uint16>(PLANARCONFIG_CONTIG));
	TIFFSetField(tiff, TIFFTAG_ORIENTATION,     static_cast<uint16>(ORIENTATION_BOTLEFT));
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC,     static_cast<uint16>(PHOTOMETRIC_RGB));
	TIFFSetField(tiff, TIFFTAG_COMPRESSION,     static_cast<uint16>(COMPRESSION_NONE));
	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP,    static_cast<uint32>(1));

	d = im.Data();
	y = 0;
	while(y < im.Height()) {
		TIFFWriteScanline(tiff, const_cast<moRGBA *>(d), y, 0);
		d += im.Width();
		y++;
	}

	TIFFClose(tiff);

	return true;
}




virtual const char *MimeType(void) const
{
	return "image/tiff";
}

virtual long MatchPriority(void) const
{
	return 90;
}

virtual long MatchType(const void *buffer, unsigned long size) const
{
	const unsigned char *s = static_cast<const unsigned char *>(buffer);

	if((s[0] == 0x49 && s[1] == 0x49 && s[2] == 0x2A && s[3] == 0x00)
	|| (s[0] == 0x4D && s[1] == 0x4D && s[2] == 0x00 && s[3] == 0x2A)) {
		// this is a TIFF V42
		return 100;
	}

	return 0;
}


};		// class moImageFile_TIFF

MAKE_IMAGEFILE_PUBLIC(TIFF);


};			// namespace molib;

// vim: ts=8

