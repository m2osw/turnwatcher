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
#pragma implementation "mo/mo_file.h"
#endif

#include	"mo/mo_file.h"

#ifndef MO_RANDOM_H
#include	"mo/mo_random.h"
#endif
#ifndef MO_WORDS_H
#include	"mo/mo_words.h"
#endif

#include	"mo/mo_dirent.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#ifdef MO_WIN32
#	include <time.h>
#endif


namespace molib
{


#ifdef _MSC_VER
#pragma warning(disable: 4267)
#endif


/************************************************************ DOC:

CLASS

	moFile

NAME

	Contructors - create a new file object

SYNOPSIS

	moFile(void);
	moFile(const char *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	moFile(const mowc::mc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UCS2_INTERNAL);
	moFile(const mowc::wc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UCS4_INTERNAL);
	moFile(const wchar_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
	moFile(const moWCString& filename, mo_file_mode_t mode = MO_FILE_MODE_READ);

PARAMETERS

	filename - the name of the file to be opened
	mode - the mode used to open the file, for reading by default
	encoding - the encoding the the filename string

DESCRIPTION

	The file object can be used to read, write, update, create files.

	The default constructor, when no parameters are used, creates an
	inactive file object. This means the object has no file descriptor
	attached to it and therefore instructions such as Read() and
	Write() can't be used until a file is attached to this object
	or a file is opened with the Open() instruction.

ERRORS

	In the constructors, when the file can't be opened an error
	is thrown. Use the Open() function member instead if you
	mind the throw.

SEE ALSO

	Open, Close, Attach, Read, Write

*/
moFile::moFile(void)
{
	Init("", 0);
}


moFile::moFile(const char *filename, mo_file_mode_t mode, mowc::encoding_t encoding)
{
	Init(moWCString(filename, -1, encoding), mode);
}


moFile::moFile(const mowc::mc_t *filename, mo_file_mode_t mode, mowc::encoding_t encoding)
{
	Init(moWCString(filename, -1, encoding), mode);
}


moFile::moFile(const mowc::wc_t *filename, mo_file_mode_t mode, mowc::encoding_t encoding)
{
	Init(moWCString(filename, -1, encoding), mode);
}


moFile::moFile(const wchar_t *filename, mo_file_mode_t mode)
{
	Init(filename, mode);
}


moFile::moFile(const moWCString& filename, mo_file_mode_t mode)
{
	Init(filename, mode);
}


moFile::~moFile()
{
	Close();
	if(f_internal_buffer) {
		delete f_buffer;
	}
}



/************************************************************ DOC:

CLASS

	moFile

NAME

	LastErrno - return the last error number
	ClearError - read the current error and return the previous value

SYNOPSIS

	int LastErrno(void) const;
	void ClearError(void);

DESCRIPTION

	LastErrno() can be used to know about the last I/O error.

	ClearError() will return the current error and reset it.

RETURN VALUE

	Either function returns the last error number.

*/
int moFile::LastErrno(void) const
{
	return f_errno;
}


int moFile::ClearError(void)
{
	int		err;

	err = f_errno;
	f_errno = 0;

	return err;
}




/************************************************************ DOC:

CLASS

	moFile

NAME

	WCFilename - return the name of this file

SYNOPSIS

	const moWCString& WCFilename(void) const;

DESCRIPTION

	This function returns the name of this file. The file has
	a filename only if it was opened with the Open() member
	function (or an equivalent constructor). Files which are
	attached to the moFile object don't have a name.

	An moFile object without a name will return an empty string.

SEE ALSO

	Open, Attach

*/
const moWCString& moFile::WCFilename(void) const
{
	return f_filename;
}





/************************************************************ DOC:

CLASS

	moFile

NAME

	NewFD - signal that a new file descriptor is available

SYNOPSIS

	protected:
	virtual void OnNewFD(int fd);

DESCRIPTION

	The OnNewFD() is called any time the f_fd field is set to a
	valid file descriptor or -1 (closed/detached).

	This function is used to signal derived classes.

SEE ALSO

	Open, Close, Attach

*/
void moFile::OnNewFD(int fd)
{
}







/************************************************************ DOC:

CLASS

	moFile

NAME

	private:
	Init - initialize a file object

SYNOPSIS

	void Init(const moWCString& filename, mo_file_mode_t mode);

PARAMETERS

	filename - a name of a file to open
	mode - a set of modes to open the file

DESCRIPTION

	This internal function will be called any time a file is created.
	It is possible that the filename will be empty.

ERRORS

	In the constructors, when the file can't be opened an error
	is thrown. Use the Open() function member instead if you
	mind the throw.

SEE ALSO

	Open, Close, Attach, Read, Write

*/
void moFile::Init(const moWCString& filename, mo_file_mode_t mode)
{
	f_mode =            mode;
	f_permissions =     0777;	// rwx for all
	f_file =            0;
	f_no_close =        false;

	f_errno =           0;

	f_stat_defined =    false;
	f_size =            BUFSIZ;
	f_start =           0;
	f_stop =            0;
	f_mem_start =       0;
	f_mem_stop =        0;
	f_internal_buffer = false;
	f_buffer =          0;

	if(!filename.IsEmpty() && !Open(filename, mode)) {
		throw moError("moFile: can't open file: %s", filename.MBData());
	}
}



/************************************************************ DOC:

CLASS

	moFile

NAME

	Stat - read stat about a file

SYNOPSIS

	bool Stat(struct stat& st) const;

PARAMETERS

	st - a stat structure

DESCRIPTION

	This function calls the stat(2) function.

	If the stat(2) succeeds, then this function returns true
	and fills the user supplied 'st' structure.

RETURN VALUE

	true when the call succeeds

SEE ALSO

	Size

*/
bool moFile::Stat(struct stat& st) const
{
	if(f_file == 0) {
		// file not even open!
		return false;
	}

	if(!f_stat_defined) {
		f_stat_defined = true;	// even if it fails!
#ifdef MO_CONFIG_NO_MUTABLE
		if(fstat(fileno(f_file), const_cast<struct stat *>(&f_stat)) != 0) {
			const_cast<int&>(f_errno) = errno;
			return false;
		}
#else
		if(fstat(fileno(f_file), &f_stat) != 0) {
			f_errno = errno;
			return false;
		}
#endif
	}

	st = f_stat;

	return true;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	Size - return the size of the file in bytes

SYNOPSIS

	size_t Size(void) const;

DESCRIPTION

	This function returns the size of the file in bytes.

	It uses the Stat() call to determine the size from these
	information instead of using the usual seek to the end
	and ftell().

RETURN VALUE

	the size of the file

SEE ALSO

	Stat

*/
size_t moFile::Size(void) const
{
	struct stat	st;

	if(!f_stat_defined && !Stat(st)) {
		return 0;
	}

	return f_stat.st_size;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	SetFilename -- defines the name of the attached file
	VOpen -- search for a file and open it
	Open -- open a file
	IsOpen -- check whether this moFile is attached to a file

SYNOPSIS

	void SetFilename(const moWCString& filename);
	bool VOpen(const moListBase& paths, const moWCString& parent, const moWCString& filename, const moWCString& extension, bool direct = true);
	bool Open(const char *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	bool Open(const mowc::mc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	bool Open(const mowc::wc_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	bool Open(const wchar_t *filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
	bool Open(const moWCString& filename, mo_file_mode_t mode = MO_FILE_MODE_READ);
	bool IsOpen(void) const;

PARAMETERS

	filename - name of the file to be opened
	pahts - a list of moWCString's representing paths to try with to open the file
	parent - the file including filename (i.e. #include ...)
	extension - an extension to append to try loading the filename
	direct - whether the VOpen() function should try opening the file in the current directory
	mode - the mode in which the file will be opened
	encoding - the name of the encoding for the input string filename
		(WARNING: this is NOT the encoding the data in the input file)

DESCRIPTION

	The SetFilename() function can be used in conjunction with the
	Attach() instructions to define the name of the file being attached
	(be it just "stdin" or some other virtual name). The Open()
	instructions will similarly name the moFile object.

	The VOpen() and Open() functions try to open a file. It returns
	true when it succeeds. The filename is kept in the moFile object.

	The Open() function expects one filename and tries to open that
	file. The VOpen() function accepts a list of paths, a parent
	filename (as if filename was being opened from a #include
	instruction), a filename to search for, an extension to try with
	if the filename by itself isn't found and a flag, named direct,
	to know whether the file should be searched in the current
	directory as well.

	If the filename parameter to the VOpen() function doesn't
	represent an absolute path, then the function tries to open
	the file as follow:

		1. if parent isn't an empty filename, use the path
		   of that filename and try with that path prepend
		   to the filename;

		2. if direct is true, try to load the file from
		   the current directory;

		3. if paths isn't an empty list, try with each one
		   of them to see if it is possible to open the
		   file in any directory; the paths are checked one
		   after another in the order there are defined in
		   the list;

	Each time a file is checked by VOpen(), it is checked first
	without the extension, and then with the extension. Note that
	the extension is used only if it isn't empty and if the
	filename doesn't already include an extension.

	The VOpen() function will usually be used when you need to load an
	object in a compiler which has a list of paths (as a C preprocessor
	which uses -I options on the command line).

	By default a file is opened for reading. For the Open() functions,
	the mode flags can be changed as required for reading or writing
	capabilities. The VOpen() function assumes that you want to read
	the file being searched. The available flags are as follow:

	MO_FILE_MODE_READ	reading mode

	MO_FILE_MODE_WRITE	writing mode; note that by default
				the existing file is truncated;
				use APPEND or UPDATE to keep the
				current data

	MO_FILE_MODE_APPEND	append anything written to the file;
				this flag requires the WRITE and CREATE
				modes and can't be used with UPDATE

	MO_FILE_MODE_CREATE	create the file if it doesn't exists;
				this flag requires the WRITE mode;

	MO_FILE_MODE_EXCLUSIVE	create a file only if it doesn't exists
				this flag requires the WRITE and CREATE
				modes;
				WARNING: flag is not currently implemented

	MO_FILE_MODE_UPDATE	open the file for update, don't delete
				the current contents; this flag requires
				the WRITE mode and can't be used with
				APPEND

	MO_FILE_MODE_ISATTY	prevent Seek() from working

	MO_FILE_MODE_CREATEDIR	try to open the file as is, if it fails,
				attempt creating missing directories
				and try opening the file again (see the
				CreateDir() function for more info);
				this flag is accepted only if the
				MO_FILE_MODE_CREATE is also set

	An moFile object may or not be attached to a file
	on the disk. If it is attached, then the IsOpen()
	function will return true.

RETURN VALUE

	VOpen() and Open(): true when a system file is succesfully
	attached to this moFile object

	IsOpen(): true when the moFile is attached to a system file

ERRORS

	if the filename is invalid (i.e. empty) or if the mode is
	not workable then an error is thrown

SEE ALSO

	Close, Constructors, SystemMode, CreateDir

*/
void moFile::SetFilename(const moWCString& filename)
{
	f_filename = filename;
	Filename(f_filename.SavedMBData());
}


bool moFile::VOpen(const moListBase& paths, const moWCString& parent, const moWCString& filename, const moWCString& extension, bool direct)
{
	moList::position_t	idx, max;
	mowc::wc_t		a, b, c;
	moWCString		name, ext;
	bool			check_with_extension;

	if(extension.IsEmpty()) {
		check_with_extension = false;
	}
	else {
		check_with_extension = !filename.FilenameHasExtension();
		if(check_with_extension) {
			if(extension[0] != '.') {
				ext = "." + extension;
			}
			else {
				ext = extension;
			}
		}
	}

// when the input filename is an absolute path, we don't check anything more than that path
	a = mowc::toupper(filename[0]);
	if(a != '\0') {
		b = filename[1];
	}
	else {
		b = '\0';
	}
	if(b != '\0') {
		c = filename[2];
	}
	else {
		c = '\0';
	}
	if(a == '/'
	|| a == '\\'
	|| (a >= 'A' && a <= 'Z' && b == ':' && (c == '\\' || c == '/'))) {
		// an absolute path -- try that one only
		if(Open(filename)) {
			return true;
		}
		return check_with_extension && Open(filename + ext);
	}

// if a parent was specified, we want to search in its directory
	if(!parent.IsEmpty()) {
		name = parent.FilenameDirname().FilenameChild(filename);
		if(Open(name)) {
			return true;
		}
		if(check_with_extension && Open(name + ext)) {
			return true;
		}
	}

// then try as is (if permitted)
	if(direct) {
		if(Open(filename)) {
			return true;
		}
		if(check_with_extension && Open(filename + ext)) {
			return true;
		}
	}

// then try each directory in the list of paths
	max = paths.Count();
	idx = 0;
	while(idx < max) {
		moWCStringSPtr path = dynamic_cast<moWCString *>(paths.Get(idx));
		name = path->FilenameChild(filename);
		if(Open(name)) {
			return true;
		}
		if(check_with_extension && Open(name + ext)) {
			return true;
		}
		idx++;
	}

	return false;
}



bool moFile::Open(const char *filename, mo_file_mode_t mode, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, -1, encoding), mode);
}


bool moFile::Open(const mowc::mc_t *filename, mo_file_mode_t mode, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, -1, encoding), mode);
}


bool moFile::Open(const mowc::wc_t *filename, mo_file_mode_t mode, mowc::encoding_t encoding)
{
	return Open(moWCString(filename, -1, encoding), mode);
}


bool moFile::Open(const wchar_t *filename, mo_file_mode_t mode)
{
	return Open(moWCString(filename), mode);
}


bool moFile::Open(const moWCString& filename, mo_file_mode_t mode)
{
	FILE		*f;

// close the current file, it's lost
	Close();

// open the file as required -- we use the mb version of the UNICODE string
	const char* name = filename.c_str();
	f = fopen(name, SystemMode(mode));	/* Flawfinder: ignore */
//fprintf(stderr, "Tried to open [%s] with [%s] got %p\n", name, SystemMode(mode), f);
	if(f == 0) {
		// if it fails, make sure the directory existed if the
		// user asked to automatically create it when possible
		if((mode & (MO_FILE_MODE_CREATEDIR | MO_FILE_MODE_CREATE)) == (MO_FILE_MODE_CREATEDIR | MO_FILE_MODE_CREATE)) {
			if(CreateDir(filename.FilenameDirname())) {
				f = fopen(name, SystemMode(mode));	/* Flawfinder: ignore */
				// the system may fail again...
			}
		}
		if(f == 0) {
			f_errno = errno;
			return false;
		}
	}

// the file was opened
	f_file =           f;
	f_no_close =       false;
	f_mode =           mode;
	f_stat_defined =   false;

	SetFilename(filename);
	ReadPosition(0);
	WritePosition(0);

// create the buffer by setting it (if not created yet)
 	if(f_buffer == 0) {
		SetBuffer(0, f_size);
	}

	OnNewFD(fileno(f));

	return true;
}



bool moFile::IsOpen(void) const
{
	return f_file != 0;
}



/************************************************************ DOC:

CLASS

	moFile

NAME

	Close - close a file

SYNOPSIS

	void Close(void);

DESCRIPTION

	This function closes the current file. This function can
	be called as many times as you want. Yet, after the
	first call the file can't be accessed anymore.

SEE ALSO

	Close, Constructors

*/
void moFile::Close(void)
{
	Flush();

	if(f_file != 0) {
		if(f_no_close) {
			// can't close this, just make sure data is flushed out
			fflush(f_file);
		}
		else {
			fclose(f_file);
		}
		f_file = 0;
	}

	f_mode = 0;
	f_no_close = false;
	SetFilename("");
	f_errno = 0;
	f_stat_defined = false;

	ReadPosition(0);
	WritePosition(0);

	return;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	private:
	SystemMode - open a file

SYNOPSIS

	static const char *SystemMode(mo_file_mode_t mode);

PARAMETERS

	mode - a set of mo_file_mode_t flags

DESCRIPTION

	This function transforms an moFile mode into a string mode
	understood by fopen(3).

NOTES

	The file will always be opened in binary mode.

RETURN VALUE

	This function will always return a static string. Writing
	in this string will make the following behave improperly.
	One of the following will be returned:

	    "rb", "rb+", "wb", "wb+", "ab", "ab+"

ERRORS

	Any invalid mode will generate a throw moError().

SEE ALSO

	Open

*/
const char *moFile::SystemMode(mo_file_mode_t mode)
{
	switch(mode & (MO_FILE_MODE_READ | MO_FILE_MODE_WRITE | MO_FILE_MODE_UPDATE | MO_FILE_MODE_CREATE | MO_FILE_MODE_APPEND)) {
	case MO_FILE_MODE_READ:
		return "rb";

	case MO_FILE_MODE_WRITE:
	case MO_FILE_MODE_WRITE | MO_FILE_MODE_UPDATE:
	case MO_FILE_MODE_READ | MO_FILE_MODE_WRITE | MO_FILE_MODE_UPDATE:
		return "rb+";

	case MO_FILE_MODE_WRITE | MO_FILE_MODE_CREATE:
	case MO_FILE_MODE_READ | MO_FILE_MODE_CREATE | MO_FILE_MODE_WRITE:
		return "wb";

	case MO_FILE_MODE_WRITE | MO_FILE_MODE_CREATE | MO_FILE_MODE_UPDATE:
	case MO_FILE_MODE_READ | MO_FILE_MODE_CREATE | MO_FILE_MODE_WRITE | MO_FILE_MODE_UPDATE:
		return "wb+";

	case MO_FILE_MODE_WRITE | MO_FILE_MODE_CREATE | MO_FILE_MODE_APPEND:
		return "ab";

	case MO_FILE_MODE_READ | MO_FILE_MODE_WRITE | MO_FILE_MODE_CREATE | MO_FILE_MODE_APPEND:
		return "ab+";

	default:
		throw moError("moFile: invalid mode to open file");
		/*NOTREACHED*/

	}
#ifdef MO_CONFIG_ALWAYS_NEED_RETURN
	return 0; 
#endif
}

#if 0
	mode_t	result;

	result = 0;
	switch(mode & (MO_FILE_MODE_READ | MO_FILE_MODE_WRITE)) {
	case MO_FILE_MODE_READ:
		result |= O_RDONLY;
		break;

	case MO_FILE_MODE_WRITE:
		result |= O_WRONLY;
		break;

	case MO_FILE_MODE_READ | MO_FILE_MODE_WRITE:
		result |= O_RDWR;
		break;

	}

	if((mode & MO_FILE_MODE_CREATE) != 0) {
		if((mode & MO_FILE_MODE_WRITE) == 0) {
			throw moError("moFile: can't create a file if not in WRITE mode");
		}
		result |= O_CREAT;

		if((mode & (MO_FILE_MODE_UPDATE | MO_FILE_MODE_APPEND
				| MO_FILE_MODE_CREATE)) == MO_FILE_MODE_CREATE) {
			// when CREATE is set, trunc by default
			result |= O_TRUNC;
		}
	}

	if((mode & (MO_FILE_MODE_UPDATE | MO_FILE_MODE_WRITE))
					== MO_FILE_MODE_UPDATE) {
		throw moError("moFile: can't update a file if not in WRITE mode");
	}

	if((mode & MO_FILE_MODE_APPEND) != 0) {
		if((mode & MO_FILE_MODE_WRITE) == 0) {
			throw moError("moFile: can't append to a file if not in WRITE mode");
		}
		if((mode & MO_FILE_MODE_UPDATE) != 0) {
			throw moError("moFile: can't update and append to a file at the same time");
		}
		result |= O_APPEND;
	}

	if((mode & MO_FILE_MODE_EXCLUSIVE) != 0) {
		if((mode & MO_FILE_MODE_CREATE) == 0) {
			throw moError("moFile: can't create a unique file if not in CREATE mode");
		}
		result |= O_EXCL;	// should really use a lock(2)
	}

	if((mode & ~MO_FILE_MODE_ALL) != 0) {
		throw moError("moFile: found some unrecognized mode flags");
	}

	return result;
#endif


/************************************************************ DOC:

CLASS

	moFile

NAME

	SetBuffer - close a file

SYNOPSIS

	void SetBuffer(void *buffer, size_t size);

PARAMETERS

	buffer - a user buffer pointer or 0
	size - the size of the buffer in bytes or zero for no buffer

DESCRIPTION

	This function allocates a buffer of the given size for
	this file.

	The buffer pointer can be set to NULL in which case the
	buffer is automatically allocated by this function.

	If the size is set to zero and there is buffer, it is
	freed and the file will not be bufferized.

	There is only one buffer for input and output so
	it is easy to manage (in the event the read and write
	pointers are given similar offsets).

	This function is automatically called whenever a file is
	opened and no buffer is defined.

NOTES

	Too small a size will be considered as no bufferization. This
	size limit will always be smaller or equal to BUFSIZ.

ERRORS

	When there is not enough memory for the buffer to be
	allocated, then a memory error is generated (std::bad_alloc).

BUGS

	moFile never deletes a buffer that you pass to the SetBuffer()
	function. So you must ensure that the buffer is being deleted
	at some point, and you must ensure that happens after the
	moFile was destroyed.

SEE ALSO

	Open

*/
void moFile::SetBuffer(void *buffer, size_t size)
{
	if(f_buffer != 0) {
		// in case the buffer was written to before this call
		Flush();
		if(f_internal_buffer) {
			delete [] f_buffer;
			f_internal_buffer = false;
		}
		f_buffer = 0;
	}

	f_start = 0;		// nothing bufferized
	f_stop = 0;

	if(size < 32) {
		size = 0;
	}
	if(size > 1024 * 1024) {
		throw moError("File: SetBuffer(): buffer size too large. Limited to 1Mb.");
	}

	if(size != 0) {
		if(buffer == 0) {
			f_buffer = new char[size];
			f_internal_buffer = true;
		}
		else {
			f_buffer = static_cast<char *>(buffer);
		}
	}

	f_size = size;

	return;
}



/************************************************************ DOC:

CLASS

	moFile

NAME

	Flush - flush the buffer to the file

SYNOPSIS

	virtual int Flush(void);

DESCRIPTION

	This function ensures that anything saved in the buffer only
	will be flushed to disk. It is necessary to call this function
	if one writes to the file and the data is bufferized and the
	data is necessary within the output file.

	This function is automatically called when the file is closed.

RETURN VALUE

	0 when no error occurs
	a non zero value if an error occurs

SEE ALSO

	SetBuffer, Write

*/
int moFile::Flush(void)
{
	size_t		l;
	int		r;

	r = moIOStream::Flush();
	if(r < 0) {
		return -1;
	}

// anything to flush?
	if(f_mem_stop <= f_mem_start || f_file == 0) {
		return 0;
	}

// unless not seekable, seek to the output position
	if((f_mode & MO_FILE_MODE_ISATTY) == 0) {
		fseek(f_file, f_mem_start, SEEK_SET);
	}

// write the buffer data to disk
	l = f_mem_stop - f_mem_start;
	r = fwrite(f_buffer + f_mem_start - f_start, 1, l, f_file);
	if(r < 0) {
		f_errno = errno;
		r = -1;
	}
	else if(static_cast<size_t>(r) != l) {
		f_errno = errno;
		r = -1;
		// here we may want to react with a throw since
		// we wrote only a partial part of the data
	}

	fflush(f_file);

// data was written
	f_mem_start = 0;
	f_mem_stop = 0;

	return r;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	Read - read data from the file

SYNOPSIS

	int Read(void *buffer, size_t length);

	private:
	int DirectRead(size_t position, const void *buffer, size_t length);
	int InternalRead(const char *buffer, size_t length);

PARAMETERS

	buffer - the buffer where the data is read
	length - the number of bytes which need to be read

DESCRIPTION

	This function reads 'length' bytes from the current file
	and writes them in 'buffer'.

	If the end of the file is reached before 'length' bytes
	are read, then the function stops and returns the number
	of bytes already read.

	The DirectRead() internal function is used to physically read
	the data from the file. The InternalRead() manages the buffers
	when the file object has one.

RETURN VALUE

	This function returns the number of bytes read when no
	error is encountered.

	It returns -1 if an error is encountered and unrecoverable
	and nothing was read. One can test the error code with the
	use of the LastErrno() or ClearError() functions.

SEE ALSO

	Write, ClearError, LastErrno

BUGS

	'buffer' needs to be at least 'length' bytes long otherwise
	this function will write after the limit and generate bugs.

	If errors occur while reading the data then 'buffer'
	may be partially modified.

*/
int moFile::RawRead(void *buffer, size_t length)
{
	int		r;

	// can we read in this file?
	if((f_mode & MO_FILE_MODE_READ) == 0) {
		throw moError("molib::file.c++: trying to read in a writeonly file");
	}

	// anything to read?
	if(length == 0) {
		return 0;
	}

	// make sure there is a correct buffer
	if(buffer == 0) {
		throw moError("molib::file.c++: no buffer pointer in moFile::Read() function");
	}

	// bufferized?
	if(f_size == 0) {
		r = DirectRead(f_input_position, buffer, length);
	}
	else {
		r = InternalRead(reinterpret_cast<char *>(buffer), length);
	}

#if 0
fprintf(stderr, "moFile::RawRead(0x%08X, %d) = %d\n", (int)buffer, (int)length, r);
for(int z=0;z<r;z++)
	fprintf(stderr, "0x%02X ", ((unsigned char *)buffer)[z]);	/* Flawfinder: ignore */
fprintf(stderr, "\n");
#endif

	if(r > 0) {
		f_input_position += r;
	}

	return r;
}


int moFile::DirectRead(size_t position, void *buffer, size_t length)
{
	int		r;

	errno = 0;
	if((f_mode & MO_FILE_MODE_ISATTY) == 0) {
		r = fseek(f_file, position, SEEK_SET);
		if(r < 0) {
			f_errno = errno;
			return -1;
		}
	}

	r = fread(buffer, 1, length, f_file);

	f_errno = errno;
	if(r < 0) {
		r = -1;		// make sure it's -1 (it should already be)
	}

	return r;
}


int moFile::InternalRead(char *buffer, size_t length)
{
	size_t		offset, l, total, pos;
	int		r;

	// anything bufferized yet?
	if(f_start == f_stop) {
		if(length > f_size) {
			return DirectRead(f_input_position, buffer, length);
		}
		// read as much as we can in the internal buffer
		Flush();		// in case the current buffer had been written!
		r = DirectRead(f_input_position, f_buffer, f_size);
		if(r > 0) {
			f_start = f_input_position;
			f_stop = f_start + r;
			if(static_cast<size_t>(r) > length) {
				r = length;
			}
			memcpy(buffer, f_buffer, r);	/* Flawfinder: ignore */
		}
		return r;
	}

	// anything we already bufferized?
	if(f_input_position + length > f_start && f_input_position < f_stop) {
		if(f_input_position >= f_start) {
			offset = f_input_position - f_start;
			l = f_stop - f_input_position;
			if(l > length) {
				l = length;
			}
			memcpy(buffer, f_buffer + offset, l);	/* Flawfinder: ignore */
		}
		else {
			offset = f_start - f_input_position;
			l = f_stop - f_start;
			if(l > length - offset) {
				l = length - offset;
			}
			memcpy(buffer + offset, f_buffer, l);	/* Flawfinder: ignore */
		}
		if(l >= length) {
			return l;
		}
		total = l;
	}
	else if(length > f_size) {
		// can't bufferize more than f_size bytes anyway...
		return DirectRead(f_input_position, buffer, length);
	}
	else {
		total = 0;
	}

// NOTE: here the data specified as 'in' was already read from the buffer

	// before, in & after
	if(f_input_position < f_start && f_input_position + length > f_start + f_size) {
		// read what's before as is
		r = DirectRead(f_input_position, buffer, f_start - f_input_position);
		if(r > 0) {
			total += r;
			// now checks what's after
			buffer += f_stop - f_input_position;
			l = f_input_position + length - f_stop;
			if(l > f_size) {
				// too large for the buffer, read it as is
				r = DirectRead(f_stop, buffer, l);
				if(r > 0) {
					total += r;
				}
			}
			else {
				Flush();		// save what was there before
				// read as much as we can in the internal buffer
				r = DirectRead(f_stop, f_buffer, f_size);
				if(r > 0) {
					f_start = f_stop;
					f_stop = f_start + r;
					if(static_cast<size_t>(r) > l) {
						r = l;
					}
					memcpy(buffer, f_buffer, r);	/* Flawfinder: ignore */
					total += r;
				}
			}
		}
	}
	// before & possibly in
	else if(f_input_position < f_start) {
		l = f_start - f_input_position;
		if(l > f_size) {
			// too large for the buffer, read it directly
			r = DirectRead(f_input_position, buffer, length - total);
			if(r > 0) {
				total += r;
			}
		}
		else {
			// memorize it
			Flush();		// save what was there before
			// read as much as we can in the internal buffer
			r = DirectRead(f_input_position, f_buffer, f_size);
			if(r > 0) {
				f_start = f_input_position;
				f_stop = f_start + r;
				if(static_cast<size_t>(r) > length) {
					r = length;
				}
				memcpy(buffer, f_buffer, r);	/* Flawfinder: ignore */
				total += r;
			}
		}
	}
	// after & possibly in
	else {
		if(f_input_position >= f_stop) {
			l = length;
			pos = f_input_position;
		}
		else {
			l = f_input_position + length - f_stop;
			buffer += f_stop - f_input_position;
			pos = f_stop;
		}
		if(l > f_size) {
			// too large for the buffer, read it directly
			r = DirectRead(pos, buffer, l);
			if(r > 0) {
				total += r;
			}
		}
		else {
			// memorize it
			Flush();		// save what was there before
			// read as much as we can in the internal buffer
			r = DirectRead(pos, f_buffer, f_size);
			if(r > 0) {
				f_start = pos;
				f_stop = pos + r;
				if(static_cast<size_t>(r) > l) {
					r = l;
				}
				memcpy(buffer, f_buffer, r);	/* Flawfinder: ignore */
				total += r;
			}
		}
	}

	// return the total number of bytes read
	//return total == 0 ? -1 : static_cast<int>(total);
	// NOTE: not too sure why, but we need to return 0 here
	//	 when the end of file is reached because EOF is not
	//	 returned by the RawRead() but rather some functions
	//	 such as GetC()
	return static_cast<int>(total);
}




/************************************************************ DOC:

CLASS

	moFile

NAME

	Write - write data to the file

SYNOPSIS

	int Write(void *buffer, size_t length);

	private:
	int DirectWrite(size_t position, const void *buffer, size_t length);
	int InternalWrite(const char *buffer, size_t length);

PARAMETERS

	buffer - the buffer the data is written
	length - the number of bytes which need to be written

DESCRIPTION

	This function writes 'length' bytes from the specified 'buffer'
	to the current file.

	If the end of the file is reached before 'length' bytes
	are written, then the file is enlarged if possible,
	otherwise the function blocks until it can write the
	extra bytes or fails.

	The DirectWrite() private function physically writes the data
	in the file. The InternalWrite() private function manages the
	bufferized writes.

NOTES

	When the function returns a value different than 'length' and
	the SetWritePosition() was used before this call then there is
	no way to know which bytes have already been written into the
	file if the file uses a buffer.

RETURN VALUE

	This function returns the number of bytes written when no
	error is encountered.

	It returns -1 if an error is encountered and unrecoverable
	and nothing was written. One can test the error code with the
	use of the LastErrno() or ClearError() functions.

SEE ALSO

	Read, ClearError, LastErrno

BUGS

	'buffer' needs to be at least 'length' bytes long otherwise
	this function will read after the limit and possibly
	generate bugs.

*/
int moFile::RawWrite(const void *buffer, size_t length)
{
	int		r;
	struct stat	st;

	// can we write in this file?
	if((f_mode & MO_FILE_MODE_WRITE) == 0) {
		throw moError(MO_ERROR_INVALID, "molib::file.c++: trying to write in a readonly file");
	}

	// anything to write?
	if(length == 0) {
		return 0;
	}

	// make sure there is a correct buffer
	if(buffer == 0) {
		throw moError(MO_ERROR_INVALID, "molib::file.c++: no buffer pointer in moFile::Write() function");
	}

	// the st_size parameter will possibly be modified later
	if(!f_stat_defined) {
		Stat(st);
	}

	// bufferized?
	if(f_size == 0) {
		r = DirectWrite(f_output_position, buffer, length);
	}
	else {
		r = InternalWrite(reinterpret_cast<const char *>(buffer), length);
	}

	if(r > 0) {
		f_output_position += r;
		if(f_output_position > static_cast<size_t>(f_stat.st_size)) {
			f_stat.st_size = f_output_position;
		}
	}

	return r;
}


int moFile::DirectWrite(size_t position, const void *buffer, size_t length)
{
	int		r;

	errno = 0;
	if(fseek(f_file, position, SEEK_SET) < 0) {
		f_errno = errno;
		return -1;
	}
	r = fwrite(buffer, 1, length, f_file);
	f_errno = errno;
	if(r < 0) {
		r = -1;		// make sure it's -1 (it should already be)
	}

	return r;
}







int moFile::InternalWrite(const char *buffer, size_t length)
{
	size_t		l, skip, pos;
	int		total, r;

	// anything bufferized yet?
	if(f_mem_start == f_mem_stop) {
		if(length > f_size) {
			return DirectWrite(f_output_position, buffer, length);
		}
		f_start = f_output_position;
		f_mem_start = f_output_position;
		f_stop = f_start + length;
		f_mem_stop = f_stop;
		memcpy(f_buffer, buffer, length);	/* Flawfinder: ignore */
		return length;
	}

	// write what's within the buffer 1st
	if(f_output_position + length > f_start && f_output_position < f_start + f_size) {
		if(f_output_position < f_start) {
			// the write starts before the current buffer
			skip = f_start - f_output_position;
			l = length - skip;
			if(l > f_size) {
				l = f_size;
			}
			memcpy(f_buffer, buffer + skip, l);	/* Flawfinder: ignore */
			f_mem_start = f_start;	// f_mem_start can't be smaller
			pos = f_start + l;
		}
		else {
			// the write starts after the current buffer
			skip = f_output_position - f_start;
			l = f_size - skip;
			if(l > length) {
				l = length;
			}
			memcpy(f_buffer + skip, buffer, l);	/* Flawfinder: ignore */
			if(f_mem_start > f_output_position) {
				f_mem_start = f_output_position;
			}
			pos = f_output_position + l;
		}
		if(pos > f_mem_stop) {
			f_mem_stop = pos;
			if(f_stop < pos) {
				f_stop = pos;
			}
		}
		if(l >= length) {
			return l;	// we're done
		}
		total = l;
	}
	else if(length > f_size) {
		return DirectWrite(f_output_position, buffer, length);
	}
	else {
		total = 0;
	}

// NOTE: here the data specified as 'in' was already saved in the buffer

	// before, in & after
	if(f_output_position < f_start && f_output_position + length > f_start + f_size) {
		// write what's before as is
		r = DirectWrite(f_output_position, buffer, f_start - f_output_position);
		if(r > 0) {
			total += r;
			// save the buffer because the DirectWrite() fseek() will fail otherwise
			Flush();
			// now checks what's after
			pos = f_stop;
			buffer += f_stop - f_output_position;
			l = f_output_position + length - f_stop;
			if(l > f_size) {
				// too large for the buffer, write it as is
				r = DirectWrite(f_stop, buffer, l);
				if(r > 0) {
					total += r;
				}
			}
			else {
				// memorize it
				f_start = f_stop;
				f_mem_start = f_stop;
				f_stop = f_start + l;
				f_mem_stop = f_stop;
				memcpy(f_buffer, buffer, l);	/* Flawfinder: ignore */
				total += l;
			}
		}
	}
	// before & possibly in
	else if(f_output_position < f_start) {
		l = f_start - f_output_position;
		if(l > f_size) {
			// too large for the buffer, write it directly
			r = DirectWrite(f_output_position, buffer, length - total);
			if(r > 0) {
				total += r;
			}
		}
		else {
			// memorize it
			Flush();		// save what was there before
			f_start = f_output_position;
			f_mem_start = f_start;
			f_stop = f_start + l;
			f_mem_stop = f_stop;
			memcpy(f_buffer, buffer, l);	/* Flawfinder: ignore */
			total += l;
		}
	}
	// after & possibly in
	else {
		// save the buffer because the DirectWrite() fseek() will fail otherwise
		Flush();
		skip = f_stop - f_output_position;
		l = length - skip;
		buffer += skip;
		if(l > f_size) {
			// too large for the buffer, write it directly
			r = DirectWrite(f_stop, buffer, l);
			if(r > 0) {
				total += r;
			}
		}
		else {
			// memorize it
			f_start = f_stop;
			f_mem_start = f_stop;
			f_stop = f_start + l;
			f_mem_stop = f_stop;
			memcpy(f_buffer, buffer, l);	/* Flawfinder: ignore */
			total += l;
		}
	}

	// return the total number of bytes written
	return total == 0 ? -1 : total;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	Attach - attach a file descriptor to this file

SYNOPSIS

	bool moFile::Attach(int fd);
	bool moFile::Attach(const iostream& stream);
	bool moFile::Attach(const istream& stream);
	bool moFile::Attach(const ostream& stream);
	bool moFile::Attach(FILE *file);

PARAMETERS

	fd - a file descriptor
	stream - a C++ iostream object
	file - a standard C file

DESCRIPTION

	This function attaches the specified file to this moFile object.

	The file descriptor is duplicated with the fdopen(3) function
	call.  It is therefore the caller responsability to close the
	file attached on his end.

	Any previous file attached to this file will be closed. Note that
	on a failure the previous file will be still closed.

RETURN VALUE

	These functions return true when they succeed.

SEE ALSO

	Read, ClearError, LastErrno

BUGS

	The attached files will be managed by the moFile. If accessed
	from their file descriptor from outside the moFile, the behavior
	is unknown.

*/
bool moFile::Attach(int fd)
{
	struct stat	st;

	Close();

	/* got a valid file descriptor? */
	if(fd == -1) {
		return false;
	}

	f_file = fdopen(fd, "rb+");
	if(f_file == 0) {
		f_errno = errno;
		return false;
	}
	f_mode =           MO_FILE_MODE_READ | MO_FILE_MODE_WRITE;
	f_no_close =       false;
	f_stat_defined =   false;

	SetFilename("<noname>");
	ReadPosition(0);
	WritePosition(0);

// create the buffer by setting it (if not created yet)
 	if(f_buffer == 0) {
		SetBuffer(0, f_size);
	}

	if(isatty(fd)) {
		f_mode |= MO_FILE_MODE_ISATTY;
	}
	else {
		if(!Stat(st)) {
			return false;
		}
		if(S_ISFIFO(st.st_mode)) {
			f_mode |= MO_FILE_MODE_ISATTY;
		}
	}

	OnNewFD(fileno(f_file));

	return true;
}


#ifdef _MSC_VER
// TODO: we need to be able to attach with stdio streams...
bool moFile::Attach(const std::iostream& file)
{
	throw moBug(MO_ERROR_NOT_IMPLEMENTED, "There is no way to extract the file descriptor from an iostream under VC++");
}


bool moFile::Attach(const std::istream& file)
{
	throw moBug(MO_ERROR_NOT_IMPLEMENTED, "There is no way to extract the file descriptor from an istream under VC++");
}


bool moFile::Attach(const std::ostream& file)
{
	throw moBug(MO_ERROR_NOT_IMPLEMENTED, "There is no way to extract the file descriptor from an ostream under VC++");
}
#else
bool moFile::Attach(const std::iostream& file)
{
	return Attach(dynamic_cast<__gnu_cxx::stdio_filebuf<char> *>(file.rdbuf())->fd());
}


bool moFile::Attach(const std::istream& file)
{
	return Attach(dynamic_cast<__gnu_cxx::stdio_filebuf<char> *>(file.rdbuf())->fd());
}


bool moFile::Attach(const std::ostream& file)
{
	return Attach(dynamic_cast<__gnu_cxx::stdio_filebuf<char> *>(file.rdbuf())->fd());
}
#endif


bool moFile::Attach(FILE *file)
{
	struct stat	st;

	Close();

	/* got a valid file descriptor? */
	if(file == 0) {
		return false;
	}

	f_file =           file;
	f_no_close =       true;
	f_mode =           MO_FILE_MODE_READ | MO_FILE_MODE_WRITE;
	f_stat_defined =   false;

	SetFilename("<noname>");
	ReadPosition(0);
	WritePosition(0);

// create the buffer by setting it (if not created yet)
 	if(f_buffer == 0) {
		SetBuffer(0, f_size);
	}

	if(isatty(fileno(file))) {
		f_mode |= MO_FILE_MODE_ISATTY;
	}
	else {
		if(!Stat(st)) {
			return false;
		}
		if(S_ISFIFO(st.st_mode)) {
			f_mode |= MO_FILE_MODE_ISATTY;
		}
	}

	OnNewFD(fileno(file));

	return true;
}




/************************************************************ DOC:

CLASS

	moFile

NAME

	FindFile - search for a file within a path and specified flags

SYNOPSIS

	static moWCString FindExec(const moWCString& path,
			const moWCString& filename,
			mo_access_t mode = MO_ACCESS_DEFAULT,
			const moWCString& separators = "");

PARAMETERS

	path - a list of directory names
	filename - the name of the file to search for
	mode - the access mode the file needs to have
	separators - the characters used to separate the directory names in path

DESCRIPTION

	This function is used to search for a file within a list of paths.

	The path is defined as the PATH variable. That's a list of directories
	separated by a colon (Unix) or semi-colons (Win32) by default. You can
	also specify your own separators. These are the same as what the moWord
	takes as the character separators.

	When a file is found, the function checks the access mode to make sure
	that the file is valid. For instance, you may just want the file to
	exist (MO_ACCESS_EXIST), you may want to have read permission
	(MO_ACCESS_READ) or execution permission (MO_ACCESS_EXECUTE.) The
	access is checked using the Access() function.

	The search goes in the order the path is defined. The first match is
	the only one returned.

	If no file is found, the function returns an empty string, otherwise
	it returns the concatenation of the path and filename.

RETURNED VALUE

	the path of the first file found to match the given access mode

SEE ALSO

	Access, moWord class

*/
molib::moWCString moFile::FindFile(const molib::moWCString& path, const moWCString& software, mo_access_t mode, const moWCString& separators)
{
	moWords		dirs;
	moWCString	fullname;
	int		idx, max;

	if(separators.IsEmpty()) {
#ifdef WIN32
		dirs.CharSeparators(";");
#else
		dirs.CharSeparators(":");
#endif
	}
	else {
		dirs.CharSeparators(separators);
	}

	dirs.SetWords(path);

	max = dirs.Count();
	for(idx = 0; idx < max; ++idx) {
		fullname = dirs[idx].FilenameChild(software);
		if(molib::moFile::Access(fullname, mode)) {
			// file exists and has proper access
			return fullname;
		}
	}

	// Oops! not found
	return "";
}



/************************************************************ DOC:

CLASS

	moFile

NAME

	FullPath - change a relative filename to an absolute filename

SYNOPSIS

	static moWCString FullPath(const moWCString& filename, bool real_path = false);

PARAMETERS

	filename - a relative or absolute filename
	real_path - also transform soft links in their real counter part

DESCRIPTION

	This function checks out a filename and changes it to an
	absolute filename.

	When the flag real_path is set to true, the realpath(3)
	function is used to also resolve soft links thus the
	new absolute path is a path with only "real" directories
	and two different paths pointing at the same file using
	soft links within themselves will be viewed as being
	equal after a call to this function with real_path set
	to true.

	NOTE (1): some systems will return the real path
	when the getcwd(3) is used and thus simulate the
	realpath(3) call even when the flag is kept set to false.

	NOTE (2): if the real path is longer than PATH_MAX then
	the function may return the non-real path.

	This function is a utility function (declared static) and
	thus it can be used without having to create an moFile
	object.

RETURNED VALUE

	always an absolute path

SEE ALSO

	Attach

*/
moWCString moFile::FullPath(const moWCString& filename, bool real_path)
{
#ifdef WIN32
	class FreeBuf
	{
	public:
		FreeBuf( char * buf ) : f_buf(buf) {}
		~FreeBuf() { free(f_buf); }
	private:
		char* f_buf;
	};
#endif
	// we don't break on an empty path, but we don't like it much either
	if(filename.IsEmpty()) {
		return "";
	}

	moWCString	fname(filename);

#ifndef WIN32
// TODO: we certainly want to look for an equivalent to a realpath() function
//	 under MS-Windows...
	if(real_path) {
		size_t l = fname.MBLength() + 1;
		// if realpath() is badly implemented, too long a filename here could crash
		if(l > PATH_MAX) {
			l = PATH_MAX;
		}
		char name[l];		/* Flawfinder: ignore */
		char rp[PATH_MAX];	/* Flawfinder: ignore */
		if(realpath(fname.MBData(name, l), rp) != NULL) {	/* Flawfinder: ignore */
			// we set the value in fname in order to
			// keep the f_password flag.
			fname.Set(rp, -1, mowc::MO_ENCODING_UTF8);
			return fname;
		}
	}
#endif

	// if already absolute, just clean the filename
	if(fname.First() == '/') {
		return fname.FilenameClean();
	}

	// that's a relative filename, generate the full path now
	// TODO: we most certainly want to transform the olddir
	// usage with f = fopen(".", "r"); & fchdir(fileno(f));
	// we first need to make sure it will work under MS-Win32
#ifdef WIN32
	char olddir[PATH_MAX];	/* Flawfinder: ignore */
	if( getcwd(olddir, PATH_MAX) == 0) {
		return "";
	}
	//
	FreeBuf _freebuf( olddir );
#else
#ifdef MAC
	std::auto_ptr<char> olddir(getcwd(0, 4096));
#else
	std::auto_ptr<char> olddir(get_current_dir_name());
#endif
	if(!olddir.get()) {
		return "";
	}
#endif
	moWCString dir = fname.FilenameDirname();
	if(chdir(dir.c_str()) != 0) {
		// just in case, go back to the right place
#ifdef WIN32
		chdir(olddir);
#else
		chdir(olddir.get());
#endif
		return "";
	}
#ifdef WIN32
	char fullpath[PATH_MAX];	/* Flawfinder: ignore */
	if(getcwd(fullpath, PATH_MAX) == 0) {
		chdir(olddir);
		return "";
	}
#else
#ifdef MAC
	char *fullpath = getcwd(0, 4096);
#else
	char *fullpath = get_current_dir_name();
#endif
	if(fullpath == 0) {
		chdir(olddir.get());
		return "";
	}
#endif
#ifdef WIN32
	chdir(olddir);
#else
	chdir(olddir.get());
#endif

	moWCString result = moWCString(fullpath, -1, mowc::MO_ENCODING_UTF8).FilenameChild(fname.FilenameBasename());
#ifndef WIN32
	free(fullpath);
#endif

	return result;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	CreateDir - create a directory

SYNOPSIS

	static bool CreateDir(const moWCString& path, mode_t perms = 0777);

PARAMETERS

	path - the path of the directory to be created
	perms - a set of Unix permissions

DESCRIPTION

	The CreateDir() function will check the specified path
	and create all the missing directories.

	By default, the new directories receive a mode (set of permission
	flags) of 0777 & ~umask.

	This function will recursively create all the missing directories
	within the given path.

	An error can occur if the directory already exists, one of
	the filenames specified is in fact not a directory or when
	you don't have permissions. For more information, please
	look at the man pages of the mkdir(2) function.

	This function is automatically called by the Open() function
	when the MO_FILE_MODE_CREATEDIR is set and the opening of
	the file fails the first time. The opening of the file will
	be attempted again if the directory creation succeeds.

	Once created, it is possible to remove a directory simply
	by calling the Remove() function with the proper mode
	(ie. at least MO_REMOVE_MODE_DIRECTORY).

BUGS

	At this time, if some directories are created, but the
	full path fails to be created, then the newly created
	directories are not automatically removed.

RETURNED VALUE

	true when the directory was successfully created

SEE ALSO

	Open, MO_FILE_MODE_CREATEDIR, Remove

*/
bool moFile::CreateDir(const moWCString& path, mode_t perms)
{
#ifdef WIN32
#define	__mkdir(a, b)	mkdir(a)
#else
#define	__mkdir(a, b)	mkdir(a, b)
#endif
	if(path.IsEmpty() || path == "." || path == "..") {
		return true;
	}

	if(__mkdir(path.c_str(), perms) == 0) {
		errno = 0;
		return true;
	}
	if(errno == ENOENT) {
		if(CreateDir(path.FilenameDirname(), perms)) {
			if(__mkdir(path.c_str(), perms) == 0) {
				errno = 0;
				return true;
			}
		}
	}

	return false;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	CopyFile - copy a file in another

SYNOPSIS

	static bool CopyFile(const moWCString& source, const moWCString& destination);

PARAMETERS

	source - the source file to copy
	destination - where the source file will be copied

DESCRIPTION

	The CopyFile() function proceeds as follow:
	
	1) try to open the source file for reading; if it fails, the function
	   returns at once

	2) try to open the destination file for reading; if it succeeds, compare
	   the source and destination files; if they are the same, the function
	   fails and returns [the current test for MS-Windows is weak!]

	3) try to open the destination file for writing; if it fails, the function
	   returns

	   Note: the function will set the CREATEDIR flag of the Open() function
	   	 and thus you will get new directories automatically if necessary

	4) read the source file by blocks (64Kb) and write these blocks in the
	   destination file; if one of these operations fail, the destination
	   file is deleted and the function returns

	5) close the files; return true; the copy succeeded

	Note that the function will overwrite an existing file. If you want
	to know whether the file exists, use the Access() function first.

BUGS

	The test to know whether the source and destination is not perfect.

RETURNED VALUE

	true when the source was successfully written to the destination

SEE ALSO

	Access, Remove

*/
bool moFile::CopyFile(const moWCString& source, const moWCString& destination)
{
	moFile		s, d;
	char		*p;
	int		l;

	// source needs to exist
	if(!s.Open(source)) {
		return false;
	}

	if(d.Open(destination)) {
		// copying file on itself is not allowed
#ifdef WIN32
		// TODO: add a Lock() on the moFile to protect the CopyFile() source file
		// wrong test under Windows for now (stat() doesn't work
		// properly there, but we could also lock the source and
		// then opening the destination for writing would fail)
		if(FullPath(source).CaseCompare(FullPath(destination)))
#else
		// better test under Unix: compare the device + inode
		struct stat ss, sd;
		// avoid warnings
		ss.st_dev = 0;
		ss.st_ino = 0;
		sd.st_dev = 0;
		sd.st_ino = 0;
		s.Stat(ss);
		d.Stat(sd);
		if(ss.st_dev == sd.st_dev && ss.st_ino == sd.st_ino)
#endif
		{
			return false;
		}
		d.Close();
	}

	// create new destination (truncate if it existed)
	if(!d.Open(destination, MO_FILE_MODE_WRITE | MO_FILE_MODE_CREATE | MO_FILE_MODE_CREATEDIR)) {
		return false;
	}

	// a large buffer
	p = new char[64 * 1024];

	for(;;) {
		l = s.Read(p, sizeof(p));
		if(l <= 0) {
			break;
		}
		d.Write(p, l);
	}

	// get rid of the destination if an error occurs
	if(l != 0) {
		Remove(destination);
		return false;
	}

	return true;
}



/************************************************************ DOC:

CLASS

	moFile

NAME

	Remove - remove a file and/or directory

SYNOPSIS

	static bool Remove(const moWCString& path, mo_remove_mode_t mode = MO_REMOVE_MODE_DEFAULT);

PARAMETERS

	path - the path to the file or directory to be unlinked
	mode - what type of a file, whether we have access, etc.
		as defined by the set of the following flags
		(OR them together as required)

		MO_REMOVE_MODE_PLAIN
			remove only plain file (this is the
			default)

		MO_REMOVE_MODE_DIRECTORY
			remove directories; if the recursive
			flag isn't set then the directory needs
			to be empty

		MO_REMOVE_MODE_RECURSIVE
			remove directories even if not empty

		MO_REMOVE_MODE_DEVICE
			accept to remove a device

		MO_REMOVE_MODE_FIFO
			accept to remove a FIFO

		MO_REMOVE_MODE_SOCKET
			accept to remove a socket

		MO_REMOVE_MODE_ACCESS
			delete the file system object only if
			the real user ID and group ID allow
			for remove (ie. write permissions)

		MO_REMOVE_MODE_DEFAULT
			the default mode in case you don't modify
			it; at this time the default is to allow
			deleting if possible (ie. access isn't
			tested) of a plain file (ie. any special
			file or directories can't be deleted by
			default)

DESCRIPTION

	The Remove() function is used to delete an object from the
	file system. By default it can be used to delete a plain
	file.
	
	By default, soft links are taken as plain files. However,
	if the soft link points to a particular type of file, then
	the corresponding remove mode needs to be set to be able
	to remove that file. Note that a dangling soft link (a link
	which doesn't point anywhere) is automatically taken as a
	plain file and removed.

	When the path points to a directory, then nothing is removed
	unless the MO_REMOVE_MODE_DIRECTORY flag is set. Also, if
	the directory isn't empty, it won't be removed unless the
	MO_REMOVE_MODE_RECURSIVE is also set. In this event, you
	don't need to set MO_REMOVE_MODE_PLAIN to have plain files
	removed from within the directory, however you will need
	to set the MO_REMOVE_MODE_DEVICE if any device exist within
	the directory.

	NOTE: a soft link which points to a directory is taken as
	a directory when MO_REMOVE_MODE_DIRECTORY is set, however
	it isn't followed and is always considered empty. Thus the
	MO_REMOVE_MODE_RECURSIVE flag has no effect on soft links.

	If you set the MO_REMOVE_MODE_ACCESS flag, then a test to
	see if the real uid/gid of the process give permission for
	the user to delete the object. If the real user doesn't have
	permission, then Remove() fails even thought the process
	could currently be able to remove the said object.

NOTES

	If path is empty, is equal to "." or ".." then nothing
	happens and the function returns false, in effect preventing
	a process to delete these special files.

BUGS

	The MO_REMOVE_MODE_RECURSIVE is implemented recursively
	which means deleting a directory with many sub-directories
	could take a lot of stack. Since the soft links are never
	followed, these won't cause any infinite loops.

	Also, the MO_REMOVE_RECURSIVE doesn't first check to see
	if the entire sub-directories can be fully removed before
	to remove files from them. In other words, even if a call
	to this function with the MO_REMOVE_MODE_DIRECTORY and
	MO_REMOVE_MODE_RECURSIVE flags set returns false, some
	files may have been deleted.

RETURNED VALUE

	true when all the files were removed

	true if the file doesn't exist

	false if the file couldn't be removed (in case of a directory
	with the RECURSIVE flag set, many of the files within the
	sub-directories may have been deleted even when the function
	returns false)

SEE ALSO

	CreateDir

*/
bool moFile::Remove(const moWCString& path, mo_remove_mode_t mode)
{
	if(path.IsEmpty() || path == "." || path == "..") {
		return false;
	}

	if((mode & MO_REMOVE_MODE_ACCESS) != 0) {
		if(access(path.c_str(), W_OK) != 0) {	/* Flawfinder: ignore */
			return false;
		}
	}

	struct stat st;
	int r;
#ifdef WIN32
	r = stat(path.c_str(), &st);
#else
	r = lstat(path.c_str(), &st);
#endif
	if(r != 0) {
		// if the file doesn't exist, it's just like it was removed!
		return errno == ENOENT;
	}

#ifdef WIN32
	// in case we have a link here...
	if(strcasecmp(path.c_str() + path.MBLength() - 4, ".lnk") == 0) {
		// we need to stat what it links to and delete the
		// link only if it matches accordingly!

		// TODO: we need to find the target
		return false;
	}
#endif

	switch(st.st_mode & S_IFMT) {
	case S_IFDIR:
		if((mode & MO_REMOVE_MODE_DIRECTORY) != 0) {
			if(rmdir(path.c_str()) == 0) {
				return true;
			}
			if(errno == ENOTEMPTY
			&& (mode & MO_REMOVE_MODE_RECURSIVE) != 0) {
				// ha! the user actually wants all the
				// files to be removed...
				DIR *d = opendir(path.c_str());
				if(d == NULL) {
					return false;
				}
				struct dirent *e = readdir(d);
				while(e != 0) {
					if(strcmp(e->d_name, ".") != 0
					&& strcmp(e->d_name, "..") != 0) {
						if(!Remove(path.FilenameChild(moWCString(e->d_name, -1, mowc::MO_ENCODING_UTF8)), mode)) {
							// if any deletion fails, then we can't remove
							// this directory
							closedir(d);
							return false;
						}
					}
					e = readdir(d);
				}
				closedir(d);
				return true;
			}
		}
		break;

	case S_IFCHR:
#ifndef _MSC_VER
	case S_IFBLK:
#endif
		if((mode & MO_REMOVE_MODE_DEVICE) != 0) {
			unlink(path.c_str());
		}
		break;

#ifndef WIN32
	case S_IFLNK:
		if(stat(path.c_str(), &st) == 0) {
			// a stat bypass softlinks so in the following
			// we won't have S_IFLNK.
			switch(st.st_mode & S_IFMT) {
			case S_IFREG:
				if((mode & MO_REMOVE_MODE_PLAIN) == 0) {
					return false;
				}
				break;

			case S_IFDIR:
				if((mode & MO_REMOVE_MODE_DIRECTORY) == 0) {
					return false;
				}
				break;

			case S_IFIFO:
				if((mode & MO_REMOVE_MODE_FIFO) == 0) {
					return false;
				}
				break;

			case S_IFSOCK:
				if((mode & MO_REMOVE_MODE_SOCKET) == 0) {
					return false;
				}
				break;

			case S_IFBLK:
			case S_IFCHR:
				if((mode & MO_REMOVE_MODE_DEVICE) == 0) {
					return false;
				}
				break;

			default:
				return false;

			}
		}
		mode |= MO_REMOVE_MODE_PLAIN;
#endif
		/*FALLTHROUGH*/
	case S_IFREG:
		if((mode & MO_REMOVE_MODE_PLAIN) != 0) {
			if(unlink(path.c_str()) == 0) {
				return true;
			}
		}
		break;

#ifndef _MSC_VER
	case S_IFIFO:
		if((mode & MO_REMOVE_MODE_FIFO) != 0) {
			if(unlink(path.c_str()) == 0) {
				return true;
			}
		}
		break;
#endif

#ifndef WIN32
	case S_IFSOCK:
		if((mode & MO_REMOVE_MODE_SOCKET) != 0) {
			if(unlink(path.c_str()) == 0) {
				return true;
			}
		}
		break;
#endif

	// anything else we don't know how to handle
	// at this time...
	}

	return false;
}


/************************************************************ DOC:

CLASS

	moFile

NAME

	Access - access a file and/or directory

SYNOPSIS

	static bool Access(const moWCString& path, mo_access_t mode = MO_ACCESS_DEFAULT);

PARAMETERS

	path - the path to the file or directory to be checked
	mode - what type of permissions we want to check
		as defined by the set of the following flags
		(OR them together as required)

		MO_ACCESS_READ
			check for read permissions

		MO_ACCESS_WRITE
			check for write permissions

		MO_ACCESS_EXECUTE
			check for execution flags

		MO_ACCESS_DELETE
			check whether we can delete the file

		MO_ACCESS_EXIST
			check for existance only

		MO_ACCESS_DEFAULT
			the default flag is currently set to
			MO_ACCESS_EXIST turning the Access()
			function to a function used to check
			whether a file exists

DESCRIPTION

	The Access() function will be used to test whether a file
	can be read, writting, executed or simply whether it exists.

	The flags can be or-ed together in which case the result
	is true only if the test succeeds for all the flags.

NOTE

	On some systems, some of the tests will either always
	fail (unlikely) or always succeed. Also, some system
	may implement some of the flags as similar (i.e. Unix
	sees the DELETE and WRITE as the same).

BUGS

	It may be considered a bug to have a result of true when
	checking whether you can delete a directory though you
	may not be able to remove the directory especially if
	there are files inside it which are not deletable by
	the caller. However, it would take a long time to test
	all the files within a directory before to return a
	valid (true) result.

	This function can generate a race condition since between
	the time you call it and the time you access the file
	(for reading or otherwise) another user may change its
	permissions.

RETURNED VALUE

	true all the flags checks pass

	false if any of the flags checks fail

SEE ALSO

	Remove

*/
bool moFile::Access(const moWCString& path, mo_access_t mode)
{
	int		m;

	// we can't check m later since F_OK is zero
	if(mode == 0) {
		return false;
	}

	m = 0;
	if((mode & MO_ACCESS_READ) != 0) {
		m |= R_OK;
	}
	if((mode & (MO_ACCESS_WRITE | MO_ACCESS_DELETE)) != 0) {
		m |= W_OK;
	}
	if((mode & MO_ACCESS_EXECUTE) != 0) {
		// TODO: for Win32 we need to check the extension (.exe, .com, .bat, ...)
		m |= X_OK;
	}
	if((mode & MO_ACCESS_EXIST) != 0) {
		m |= F_OK;
	}
	// NOTE: m may be 0 if only MO_ACCESS_EXIST is the only flag set

	return access(path.c_str(), m) == 0;	/* Flawfinder: ignore */
}




/************************************************************ DOC:

CLASS

	moFile

NAME

	TemporaryFilename -- generates a temporary filename which is unique

SYNOPSIS

	static moWCString TemporaryFilename(const moWCString& path, const moWCString& extension);

PARAMETERS

	path - the path to the temporary directory to be used
	extension - an extension to append to the filename

DESCRIPTION

	The TemporaryFilename() function generates a filename which
	points to a file that doesn't yet exist.

BUGS

	The filename will really be unique within a single computer.
	If multiple computers use the same directory on the same
	hard disk the creation of the file may succeed on two
	computers simultaneously.

RETURNED VALUE

	the name of the temporary file if the function succeeds
	an empty string if the function fails

SEE ALSO

	FullPath

*/
static void GetRandomName(char *out)
{
	/* Flawfinder: ignore */
	static char cvt[64] = {		// 64 chars valid in a URL
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
		'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'-', '.'
	};
	FILE		*f;
	/* Flawfinder: ignore */
	char		rnd[33], *r;		// 256 + 8 bits
	//char		out[45];		// 44 + '\0'
	unsigned int	idx;

	f = fopen("/dev/random", "r");	/* Flawfinder: ignore */
	if(f == 0) {
		// TODO: fix the seeding of this thing!
		// some weak(er) random solution for MS-Windows for now...
		moRandom random;	/* Flawfinder: ignore */
		long seed[moRandom::MO_RANDOM_SEED_N];
		for(idx = 0; idx < moRandom::MO_RANDOM_SEED_N; ++idx) {
			seed[idx] = static_cast<long>(time(0));
		}
		random.SetSeed(seed);	/* Flawfinder: ignore */
		for(idx = 0; idx < sizeof(rnd); ++idx) {
			rnd[idx] = static_cast<char>(random.Random() >> (idx & 0x17));	/* Flawfinder: ignore */
		}
	}
	else {
		fread(rnd, 1, sizeof(rnd), f);
		fclose(f);
	}

	// map the 33 bytes to 44 chars
	for(idx = 0, r = rnd; idx < 45 / 4; idx++, r += 3) {
		out[idx * 4 + 0] = cvt[r[0] & 0x3F];
		out[idx * 4 + 1] = cvt[r[1] & 0x3F];
		out[idx * 4 + 2] = cvt[r[2] & 0x3F];
		out[idx * 4 + 3] = cvt[
			  (r[0] >> 6) & 0x03
			+ (r[1] >> 4) & 0x0C
			+ (r[2] >> 2) & 0x30];
	}

	out[44] = '\0';
}



moWCString moFile::TemporaryFilename(const moWCString& path, moWCString extension)
{
	char		out[45];	/* Flawfinder: ignore */

	if(!extension.IsEmpty()) {
		if(extension[0] != static_cast<mowc::wc_t>('.')) {
			extension = "." + extension;
		}
	}

	// WARNING: the only way to break out of the following loop
	//	    is by getting an error other than EEXIST or by finding
	//	    a temporary filename which is not already in use.
	for(;;) {
		GetRandomName(out);
		moWCString filename = path.FilenameChild(out) + extension;
		int fd = open(filename.c_str(), O_WRONLY | O_EXCL | O_CREAT, 0600);	/* Flawfinder: ignore */
		if(fd >= 0) {
			// the opening succeeded so we know we're the
			// only one to have it (unless you're using
			// NFS on your temporary directory?!?)
			// we can close the file, the caller will
			// take care of deleting it (right?).
			close(fd);
			return filename;
		}
		if(errno != EEXIST) {
			// cannot create a file in that directory?
			return "";
		}
	}
	/*NOTREACHED*/
}




/************************************************************ DOC:

CLASS

	moStdIn

NAME

	moStdIn - create an input moFile attached to a standard C file
	~moStdIn - disconnect the filter

SYNOPSIS

	moStdIn(FILE *file, const char *input_encoding = "UTF-8",
				const char *output_encoding = "UTF-32");

PARAMETERS

	file - a standard C file
	input_encoding - the name of the input encoding in ASCII (default UTF-8)
	output_encoding - the name of the output encoding in ASCII (default UTF-32)

DESCRIPTION

	This function creates a stream which will read anything
	from a standard file (usually stdin).

	Because our streams are usually used with UTF-32 characters,
	by default, this object ensures that the input is converted
	to UTF-32.

SEE ALSO

	moFile::Attach

*/
moStdIn::moStdIn(FILE *file, const char *input_encoding, const char *output_encoding)
{
	// we expect UTF-8 as the input
	// the user can change that if he wishes to do so
	f_convertor.SetEncodings(input_encoding, output_encoding);
	SetInputFilter(&f_convertor);
	Attach(file);
}


moStdIn::~moStdIn()
{
	SetInputFilter(0);
}



/************************************************************ DOC:

CLASS

	moStdOut

NAME

	moStdOut - create an output moFile attached to a standard C file
	~moStdOut - disconnect the filter

SYNOPSIS

	moStdOut(FILE *file, const char *input_encoding = "*",
				const char *output_encoding = "UTF-32");

PARAMETERS

	file - a standard C file
	input_encoding - the name of the input encoding in ASCII (default any)
	output_encoding - the name of the output encoding in ASCII (default UTF-32)

DESCRIPTION

	This function creates a stream which will print everything
	in a standard file (usually stdout or stderr).

	Because our streams are usually used with UTF-32 characters,
	this object can be used to ensure that all of the UTF-32
	characters are converted to UTF-8 or some other encoding
	for different terminal compatibility.

SEE ALSO

	moFile::Attach

*/
moStdOut::moStdOut(FILE *file, const char *input_encoding, const char *output_encoding)
{
	// though we expect UTF-32 as the input, we let the convertor
	// determine that by itself at this time
	f_convertor.SetEncodings(input_encoding, output_encoding);
	SetOutputFilter(&f_convertor);
	Attach(file);
}


moStdOut::~moStdOut()
{
	Close();
	SetOutputFilter(0);
}






/************************************************************ DOC:

CLASS

	moAutoDelete

NAME

	Constructor -- initialize an auto-delete object
	Destructor -- delete the specified file

SYNOPSIS

	AutoDelete(const char *filename, moFile::mo_remove_mode_t mode = moFile::MO_REMOVE_MODE_DEFAULT);
	~AutoDelete();

PARAMETERS

	filename - the name of a file to automatically delete
	mode - how to delete the specified file

DESCRIPTION

	The moAutoDelete object can be used in other classes
	and on the stack to automatically have a file deleted
	when the object is deleted or the stack is being unfolded.

	The destructor will delete the file or directory which
	name was specified on construction.

	If necessary, you can call the NoDelete() function to
	avoid the deletion.

	This is quite useful if you run a function with many
	return statements. This ensures that the file will be
	deleted no matter what return you enter.

	It is not an error to pass an empty string as the filename
	in the constructor. However, it is not possible (at this
	time) to change the filename on the fly and thus it will
	simply have no effect.

NOTES

	This class isn't derived from the moBase class. This is mainly
	because it isn't expected to be used as a dynamic object. Most
	of the time, it will be on the stack.

SEE ALSO

	NoDelete()

*/
moAutoDelete::moAutoDelete(const moWCString& filename, moFile::mo_remove_mode_t mode)
	: f_mode(mode),
	  f_filename(filename)
{
}

moAutoDelete::~moAutoDelete()
{
	if(!f_filename.IsEmpty()) {
		moFile::Remove(f_filename, f_mode);
	}
}




/************************************************************ DOC:

CLASS

	moAutoDelete

NAME

	NoDelete -- clears the filename

SYNOPSIS

	void NoDelete(void);

DESCRIPTION

	The NoDelete() function clears the name of the file to
	be deleted. This means the destructor of the moAutoDelete
	object will have no effect.

	This is useful if you have a long function which creates
	a file, try to work with it, and keep it only if the
	function succeeds. This means you will have the following
	code:

		void foo()
		{
			filename = ...;
			...
			moAutoDelete auto_delete(filename);
			...
			if(error) {
				// the file will be deleted
				return;
			}
			...
			auto_delete.NoDelete();
			// now the file is safe
		}

NOTES

	This only works with files which aren't directories.

SEE ALSO

	NoDelete()

*/
void moAutoDelete::NoDelete(void)
{
	f_filename.Empty();
}








};			// namespace molib;

// vim: ts=8

