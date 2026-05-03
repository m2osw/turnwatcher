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



#ifndef MO_FILE_H
#define	MO_FILE_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#include	"mo_types.h"

#ifndef MO_STRING_H
#include	"mo_string.h"
#endif

namespace molib
{


// we can accept FILE *f for the Accept function


class MO_DLL_EXPORT moFile : public moIOStream
{
public:
	typedef int			mo_file_mode_t;
	typedef int			mo_remove_mode_t;
	typedef int			mo_access_t;

	static const mo_file_mode_t	MO_FILE_MODE_READ      = 0x0001;
	static const mo_file_mode_t	MO_FILE_MODE_WRITE     = 0x0002;
	static const mo_file_mode_t	MO_FILE_MODE_APPEND    = 0x0004;
	static const mo_file_mode_t	MO_FILE_MODE_CREATE    = 0x0008;
	static const mo_file_mode_t	MO_FILE_MODE_EXCLUSIVE = 0x0010;
	static const mo_file_mode_t	MO_FILE_MODE_UPDATE    = 0x0020;
	static const mo_file_mode_t	MO_FILE_MODE_ISATTY    = 0x0040;
	static const mo_file_mode_t	MO_FILE_MODE_CREATEDIR = 0x0080;

	static const mo_file_mode_t	MO_FILE_MODE_ALL       = 0x00FF;

	static const mo_remove_mode_t	MO_REMOVE_MODE_PLAIN     = 0x0001;
	static const mo_remove_mode_t	MO_REMOVE_MODE_DIRECTORY = 0x0002;
	static const mo_remove_mode_t	MO_REMOVE_MODE_RECURSIVE = 0x0004;
	static const mo_remove_mode_t	MO_REMOVE_MODE_DEVICE    = 0x0008;
	static const mo_remove_mode_t	MO_REMOVE_MODE_FIFO      = 0x0010;
	static const mo_remove_mode_t	MO_REMOVE_MODE_SOCKET    = 0x0020;
	static const mo_remove_mode_t	MO_REMOVE_MODE_ACCESS    = 0x0040;

	static const mo_remove_mode_t	MO_REMOVE_MODE_DEFAULT   = MO_REMOVE_MODE_PLAIN;

	static const mo_access_t	MO_ACCESS_READ    = 0x0001;
	static const mo_access_t	MO_ACCESS_WRITE   = 0x0002;
	static const mo_access_t	MO_ACCESS_EXECUTE = 0x0004;
	static const mo_access_t	MO_ACCESS_DELETE  = 0x0008;
	static const mo_access_t	MO_ACCESS_EXIST   = 0x0010;

	static const mo_access_t	MO_ACCESS_DEFAULT = MO_ACCESS_EXIST;

				moFile(void);
				moFile(const char *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
				moFile(const mowc::mc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
				moFile(const mowc::wc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
				moFile(const wchar_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
				moFile(const moWCString& filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
	virtual			~moFile();

	void			SetFilename(const moWCString& filename);
	bool			VOpen(const moListBase& paths, const moWCString& parent, const moWCString& filename, const moWCString& extension, bool direct = true);
	bool			Open(const char *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	bool			Open(const mowc::mc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	bool			Open(const mowc::wc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool			Open(const wchar_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
	bool			Open(const moWCString& filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
	bool			IsOpen(void) const;
	void			Close(void);
	bool			Attach(int fd);
	bool			Attach(const std::iostream& file);
	bool			Attach(const std::istream& file);
	bool			Attach(const std::ostream& file);
	bool			Attach(FILE *file);
	virtual int		Flush(void);
	void			SetBuffer(void *buffer, size_t size);
	size_t			Size(void) const;
	int			LastErrno(void) const;
	int			ClearError(void);
	const moWCString&	WCFilename(void) const;
	bool			Stat(struct stat& st) const;

	static moWCString	FindFile(const moWCString& path, const moWCString& filename, mo_access_t mode = MO_ACCESS_DEFAULT, const moWCString& separators = "");
	static moWCString	FullPath(const moWCString& filename, bool real_path = false);
	static bool		CreateDir(const moWCString& path, mode_t perms = 0777);
	static bool		Remove(const moWCString& path, mo_remove_mode_t mode = MO_REMOVE_MODE_DEFAULT);
	static bool		Access(const moWCString& path, mo_access_t mode = MO_ACCESS_DEFAULT);
	static moWCString	TemporaryFilename(const moWCString& path, moWCString extension = ".tmp");
	static bool		CopyFile(const moWCString& source, const moWCString& destination);

protected:
	virtual void		OnNewFD(int fd);

private:
	void			Init(const moWCString& filename, mo_file_mode_t mode);
	static const char *	SystemMode(mo_file_mode_t mode);
	int			DirectRead(size_t position, void *buffer, size_t length);
	int			InternalRead(char *buffer, size_t length);
	int			DirectWrite(size_t position, const void *buffer, size_t length);
	int			InternalWrite(const char *buffer, size_t length);

	virtual int		RawRead(void *buffer, size_t length);
	virtual int		RawWrite(const void *buffer, size_t length);

	moWCString		f_filename;	// name of this file (when available)
	mo_file_mode_t		f_mode;		// read/write mode
	long			f_permissions;	// permissions used to create files
	FILE *			f_file;		// file descriptor
	bool			f_no_close;	// whether we can close(2) f_fd

	mutable int		f_errno;	// last I/O error number

	mutable bool		f_stat_defined;	// stat() was called when true
	mutable struct stat	f_stat;		// stat() called on this file
	//size_t		f_read_pos;	// next read appends here
	//size_t		f_write_pos;	// next write appends here
	size_t			f_size;		// size of the buffer
	size_t			f_start;	// absolute position from source file at which the buffer was read (included)
	size_t			f_stop;		// absolute position from source file up to which the buffer was read (excluded)
	size_t			f_mem_start;	// absolute position from source file at which writes started (included)
	size_t			f_mem_stop;	// absolute position from source file up to which writes went (excluded)
	bool			f_internal_buffer;	// true if the file buffer was internally allocated
	char *			f_buffer;	// pointer to a buffer
};

typedef moDualSmartPtr<moFile, moIStream>	moFileSPtr;





class MO_DLL_EXPORT moStdIn : public moFile
{
public:
				moStdIn(FILE *file, const char *input_encoding = "UTF-8", const char *output_encoding = "UTF-32");
	virtual			~moStdIn();

private:
	mowc::moIConv		f_convertor;	// UTF-8 to UTF-32
};

typedef moSmartPtr<moStdIn>	moStdInSPtr;




class MO_DLL_EXPORT moStdOut : public moFile
{
public:
				moStdOut(FILE *file, const char *input_encoding = "*", const char *output_encoding = "UTF-8");
	virtual			~moStdOut();

private:
	mowc::moIConv		f_convertor;	// UTF-32 to UTF-8
};

typedef moSmartPtr<moStdOut>	moStdOutSPtr;





class MO_DLL_EXPORT moAutoDelete
{
public:
			moAutoDelete(const moWCString& filename, moFile::mo_remove_mode_t mode = moFile::MO_REMOVE_MODE_DEFAULT);
			~moAutoDelete();

	void		NoDelete(void);

private:
	moFile::mo_remove_mode_t	f_mode;
	moWCString			f_filename;
};






};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_FILE_H

