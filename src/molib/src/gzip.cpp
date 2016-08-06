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
#pragma implementation "mo/mo_gzip.h"
#endif

#include	"mo/mo_gzip.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{


/************************************************************ DOC:

CLASS

	moGZip

NAME

	Contructor - creates a new GZip file
	Destructor - closes the GZip file

SYNOPSIS

	moGZip(void);
	moGZip(const char *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	moGZip(const mowc::mc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	moGZip(const mowc::wc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	moGZip(const wchar_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	moGZip(const moWCString& filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	virtual ~moGZip();

	private:
	void Init(void);

PARAMETERS

	filename - name of the output GZip file
	mode - how to compress (filter & level)
	encoding - how the filename is encoded

DESCRIPTION

	To create a GZip-ed file, use this object as the output stream.
	You can easer specify a filename on creation or use the Open()
	function.

	When deleted, this object ensures the GZip file is being properly
	closed.

	To write in the stream, use the usual stream functions.

	The Init() function is used internally to initialize the object.

	The mode is the level (0-9) and a filter flag:

		GZIP_FILTER_DEFAULT		best compression

		GZIP_FILTER_FAST		try quickly to compress
						very well

		GZIP_FILTER_HUFFMAN		compress using Huffman only

	The default mode is to compress everything to the best the
	library can do.

SEE ALSO

	Open, Close, moOStream

*/
moGZip::moGZip(void)
{
	Init();
}


moGZip::moGZip(const char *filename, gzip_mode_t mode, mowc::encoding_t encoding)
{
	Init();
	Open(filename, mode, encoding);
}


moGZip::moGZip(const mowc::mc_t *filename, gzip_mode_t mode, mowc::encoding_t encoding)
{
	Init();
	Open(filename, mode, encoding);
}


moGZip::moGZip(const mowc::wc_t *filename, gzip_mode_t mode, mowc::encoding_t encoding)
{
	Init();
	Open(filename, mode, encoding);
}


moGZip::moGZip(const wchar_t *filename, gzip_mode_t mode)
{
	Init();
	Open(filename, mode);
}


moGZip::moGZip(const moWCString& filename, gzip_mode_t mode)
{
	Init();
	Open(filename, mode);
}


moGZip::~moGZip()
{
	Close();
}


void moGZip::Init(void)
{
	f_gz_file = 0;
}









/************************************************************ DOC:

CLASS

	moGZip

NAME

	Open - opens a GZip file for writing
	Close - closes the GZip file attached to this moGZip object

SYNOPSIS

	bool Open(const char *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	bool Open(const mowc::mc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool Open(const mowc::wc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool Open(const wchar_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	bool Open(const moWCString& filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	void Close(void);

DESCRIPTION

	It is possible, with the same GZip object, to create multiple
	GZip-ed files. For this, call the Open() function to close
	the currently open GZip and open a new file.

	The Close() will be called once you have sent all the data
	via the moOStream functions.

SEE ALSO

	Constructor, moOStream

*/
bool moGZip::Open(const char *filename, gzip_mode_t mode, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, encoding), mode);
}


bool moGZip::Open(const mowc::mc_t *filename, gzip_mode_t mode, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, encoding), mode);
}


bool moGZip::Open(const mowc::wc_t *filename, gzip_mode_t mode, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, encoding), mode);
}


bool moGZip::Open(const wchar_t *filename, gzip_mode_t mode)
{
	return Open(moWCString(filename), mode);
}


bool moGZip::Open(const moWCString& filename, gzip_mode_t mode)
{
	char		m[10];	/* Flawfinder: ignore */
	const char	*filter;

	switch(mode.Filter()) {
	case GZIP_FILTER_DEFAULT:
		filter = "";
		break;

	case GZIP_FILTER_FAST:
		filter = "f";
		break;

	case GZIP_FILTER_HUFFMAN:
		filter = "h";
		break;

	default:
		// unknown filter
		errno = EINVAL;
		return false;

	}
	if(mode.Level() == GZIP_LEVEL_DEFAULT) {
		snprintf(m, sizeof(m), "wb%s", filter);				/* Flawfinder: ignore */
	}
	else {
		snprintf(m, sizeof(m), "wb%ld%s", mode.Level(), filter);	/* Flawfinder: ignore */
	}
	f_gz_file = gzopen(filename.c_str(), m);
	if(f_gz_file == 0) {
		return false;
	}

	OutputFilename(filename.c_str());

	return true;
}


void moGZip::Close(void)
{
	if(f_gz_file != 0) {
		gzclose(f_gz_file);
		f_gz_file = 0;
	}
}




/************************************************************ DOC:

CLASS

	moGZip

NAME

	RawWrite - overwritten RawWrite() function

SYNOPSIS

	private:
	int RawWrite(const void *buffer, size_t length);

PARAMETERS

	buffer - a pointer to a buffer where the data is copied from
	length - the number of bytes in buffer which will be copied to the output

DESCRIPTION

	This function will be called by any of the functions which
	write to the moOStream object (Put, Write).

SEE ALSO

	Constructor, moOStream

*/
int moGZip::RawWrite(const void *buffer, size_t length)
{
	if(length == 0) {
		return 0;
	}
	// this function returns zero when an error occurs
	int r = gzwrite(f_gz_file, const_cast<const voidp>(buffer), static_cast<unsigned int>(length));
	return r == 0 ? -1 : r;
}







/************************************************************ DOC:

CLASS

	moGunZip

NAME

	Contructor - creates an object used to read an existing GZip file
	Destructor - closes the GZip file

SYNOPSIS

	moGunZip(void);
	moGunZip(const char *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	moGunZip(const mowc::mc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	moGunZip(const mowc::wc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	moGunZip(const wchar_t *filename);
	moGunZip(const moWCString& filename);
	virtual ~moGunZip();

	private:
	void Init(void);

PARAMETERS

	filename - name of the existing GZip file
	encoding - the encoding of the filename

DESCRIPTION

	To read a GZip-ed file, use this object as the input stream.
	You can easer specify a filename on creation or use the Open()
	function.

	When deleted, this object ensures the GZip file is properly
	closed.

	To read from the stream, use the usual stream functions.

	The Init() function is used internally to initialize the object.

SEE ALSO

	Open, Close, moIStream

*/
moGunZip::moGunZip(void)
{
	Init();
}


moGunZip::moGunZip(const char *filename, mowc::encoding_t encoding)
{
	Init();
	Open(filename, encoding);
}


moGunZip::moGunZip(const mowc::mc_t *filename, mowc::encoding_t encoding)
{
	Init();
	Open(filename, encoding);
}


moGunZip::moGunZip(const mowc::wc_t *filename, mowc::encoding_t encoding)
{
	Init();
	Open(filename, encoding);
}


moGunZip::moGunZip(const wchar_t *filename)
{
	Init();
	Open(filename);
}


moGunZip::moGunZip(const moWCString& filename)
{
	Init();
	Open(filename);
}


moGunZip::~moGunZip()
{
	Close();
}


void moGunZip::Init(void)
{
	f_gz_file = 0;
}









/************************************************************ DOC:

CLASS

	moGunZip

NAME

	Open - opens a GZip-ed file for writing
	Close - closes the GZip-ed file attached to this moGunZip object

SYNOPSIS

	bool Open(const char *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	bool Open(const mowc::mc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool Open(const mowc::wc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool Open(const wchar_t *filename);
	bool Open(const moWCString& filename);
	void Close(void);

PARAMETERS

	filename - name of the existing GZip file
	encoding - the encoding of the filename

DESCRIPTION

	It is possible, with the same GunZip object, to read multiple
	GZip-ed files. For this, call the Open() function to close
	the currently open GZip-ed file and open a new one.

	The Close() can be called once you have read all the data
	via the moIStream functions.

SEE ALSO

	Constructor, moIStream

*/
bool moGunZip::Open(const char *filename, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, encoding));
}


bool moGunZip::Open(const mowc::mc_t *filename, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, encoding));
}


bool moGunZip::Open(const mowc::wc_t *filename, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, encoding));
}


bool moGunZip::Open(const wchar_t *filename)
{
	return Open(moWCString(filename));
}


bool moGunZip::Open(const moWCString& filename)
{
	f_gz_file = gzopen(filename.c_str(), "rb");
	if(f_gz_file == 0) {
		return false;
	}

	InputFilename(filename.c_str());

	return true;
}


void moGunZip::Close(void)
{
	if(f_gz_file != 0) {
		gzclose(f_gz_file);
		f_gz_file = 0;
	}
}




/************************************************************ DOC:

CLASS

	moGunZip

NAME

	RawRead - overwritten RawRead() function

SYNOPSIS

	private:
	int RawRead(void *buffer, size_t length);

PARAMETERS

	buffer - a pointer to a buffer where the data is read to
	length - the number of bytes in buffer which can be used

DESCRIPTION

	This function will be called by any of the functions which
	read from the moIStream object (Get, Read).

SEE ALSO

	Constructor, moIStream

*/
int moGunZip::RawRead(void *buffer, size_t length)
{
	if(length == 0) {
		return 0;
	}
	return gzread(f_gz_file, buffer, static_cast<unsigned int>(length));
}





/************************************************************ DOC:

CLASS

	moGunZip::gzip_mode_t

NAME

	Constructor - create and initialize the mode
	Set - initialize the mode

SYNOPSIS

	gzip_mode_t(void);
	gzip_mode_t(unsigned long mode);
	gzip_mode_t(unsigned long filter, unsigned long level);
	gzip_mode_t& operator = (unsigned long mode);
	void Set(unsigned long filter, unsigned long level);

PARAMETERS

	filter - the filter type, one of:

		GZIP_FILTER_DEFAULT
		GZIP_FILTER_FAST
		GZIP_FILTER_HUFFMAN

	level - the strength of the compression
	mode - the combined filter and level

DESCRIPTION

	These functions will set the mode to the specified filter
	and level. The constructor without a parameter will set
	the mode to the default (GZIP_MODE_DEFAULT).

SEE ALSO

	Filter, Level

*/
moGZip::gzip_mode_t::gzip_mode_t(void)
{
	f_mode = GZIP_MODE_DEFAULT;
}


moGZip::gzip_mode_t::gzip_mode_t(unsigned long mode)
{
	f_mode = mode;
}


moGZip::gzip_mode_t& moGZip::gzip_mode_t::operator = (unsigned long mode)
{
	f_mode = mode;
	return *this;
}


moGZip::gzip_mode_t::gzip_mode_t(unsigned long filter, unsigned long level)
{
	f_mode = filter | level;
}


void moGZip::gzip_mode_t::Set(unsigned long filter, unsigned long level)
{
	f_mode = filter | level;
}


unsigned long moGZip::gzip_mode_t::Filter(void) const
{
	return f_mode & GZIP_FILTER_MASK;
}


unsigned long moGZip::gzip_mode_t::Level(void) const
{
	return f_mode & GZIP_LEVEL_MASK;
}


};			// namespace molib;

// vim: ts=8

