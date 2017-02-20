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



#ifndef MO_GZIP_H
#define	MO_GZIP_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STREAM_H
#include	"mo_stream.h"
#endif

#ifndef MO_STRING_H
#include	"mo_string.h"
#endif

namespace molib
{

// create a new GZip
class MO_DLL_EXPORT moGZip : public moOStream
{
public:
	static const unsigned long	GZIP_FILTER_MASK =    0x0F00;
	static const unsigned long	GZIP_FILTER_DEFAULT = 0x0000;
	static const unsigned long	GZIP_FILTER_FAST =    0x0100;
	static const unsigned long	GZIP_FILTER_HUFFMAN = 0x0200;

	static const unsigned long	GZIP_LEVEL_MASK    = 0x00FF;
	static const unsigned long	GZIP_LEVEL_MIN     = 0;
	static const unsigned long	GZIP_LEVEL_MAX     = 9;
	static const unsigned long	GZIP_LEVEL_DEFAULT = 0xFF;

	static const unsigned long	GZIP_MODE_DEFAULT = GZIP_FILTER_DEFAULT | GZIP_LEVEL_MAX;

	struct gzip_mode_t {
					gzip_mode_t(void);
					gzip_mode_t(unsigned long mode);
					gzip_mode_t(unsigned long filter, unsigned long level);
		void			Set(unsigned long filter, unsigned long level);
		unsigned long		Filter(void) const;
		unsigned long		Level(void) const;
		gzip_mode_t&		operator = (unsigned long mode);

		unsigned long		f_mode;
	};

				moGZip(void);
				moGZip(const char *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
				moGZip(const mowc::mc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
				moGZip(const mowc::wc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
				moGZip(const wchar_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
				moGZip(const moWCString& filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	virtual			~moGZip();

	bool			Open(const char *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	bool			Open(const mowc::mc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	bool			Open(const mowc::wc_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool			Open(const wchar_t *filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	bool			Open(const moWCString& filename, gzip_mode_t mode = GZIP_MODE_DEFAULT);
	void			Close(void);

private:
	void			Init(void);
	virtual int		RawWrite(const void *buffer, size_t length);

	gzFile			f_gz_file;
};



// uncompress an existing GZip file
class MO_DLL_EXPORT moGunZip : public moIStream
{
public:
				moGunZip(void);
				moGunZip(const char *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
				moGunZip(const mowc::mc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
				moGunZip(const mowc::wc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
				moGunZip(const wchar_t *filename);
				moGunZip(const moWCString& filename);
	virtual			~moGunZip();

	bool			Open(const char *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	bool			Open(const mowc::mc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	bool			Open(const mowc::wc_t *filename, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool			Open(const wchar_t *filename);
	bool			Open(const moWCString& filename);
	void			Close(void);

private:
	void			Init(void);
	virtual int		RawRead(void *buffer, size_t length);

	gzFile			f_gz_file;
};



};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_GZIP_H

