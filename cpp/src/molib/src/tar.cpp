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
#pragma implementation "mo/mo_tar.h"
#endif

#include	"mo/mo_tar.h"

#ifndef MO_FILE_H
#include	"mo/mo_file.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#ifdef MO_WIN32
#include <time.h>
#endif

#include <cstddef>


namespace molib
{

#ifdef _MSC_VER
// -sizeof() generates that warning
#pragma warning(disable: 4146)
#endif


/************************************************************ DOC:

CLASS

	moTar::moTarHeader

NAME

	Contructor - creates an empty header

SYNOPSIS

	moTarHeader(void);

DESCRIPTION

	To insert a new file in a tar stream, you need to have a
	header. Use this object to create the header. Then use
	all the necessary Set...() functions to define all the
	necessary fields.

SEE ALSO

	moOTar, moITar

*/
moTar::moTarHeader::moTarHeader(void)
{
	Reset();
}


moTar::moTarHeader::moTarHeader(const moTarHeader& header)
	: moBase(header)
{
	if(this != &header) {
		f_flags = header.f_flags;
		f_name = header.f_name;
		f_linkto = header.f_linkto;
		f_username = header.f_username;
		f_groupname = header.f_groupname;
		f_type = header.f_type;
		f_mode = header.f_mode;
		f_uid = header.f_uid;
		f_gid = header.f_gid;
		f_size = header.f_size;
		f_mtime = header.f_mtime;
		f_device = header.f_device;
	}
}




/************************************************************ DOC:

CLASS

	moTar::moTarHeader

NAME

	Reset - resets a tar header so you can use it again

SYNOPSIS

	void Reset(void);

DESCRIPTION

	The Reset() function will be called once you are done with a
	file before to re-use the same tar header object to save
	another file.

SEE ALSO

	Constructor, Set...

*/
void moTar::moTarHeader::Reset(void)
{
	f_flags = 0;
}





/************************************************************ DOC:

CLASS

	moTar::moTarHeader

NAME

	SetName - defines the filename (in full)
	SetLinkTo - when defining a link, where it points to
	SetUserName - the name of the owner
	SetGroupName - the name of the group it belongs to
	SetID - set the user and group identifiers numerically
	SetMode - the standard mode (as with chmod())
	SetSize - set the size in bytes of the file
	SetModifTime - defines the last modification time
	SetDevice - sets the major and minor device numbers (block device file)

SYNOPSIS

	void SetName(const moWCString& name);
	void SetLinkTo(const moWCString& link_to);
	void SetUserName(const moWCString& user_name);
	void SetGroupName(const moWCString& group_name);
	void SetID(uid_t uid, gid_t gid);
	void SetMode(mode_t mode);
	void SetSize(size_t size);
	void SetModifTime(time_t time);
	void SetDevice(dev_t device);

PARAMETERS

	name - the filename
	link_to - a filename where the link points
	user_name - the name of the owner
	group_name - the name of the group
	uid - the user identification number
	gid - the group identification number
	mode - a mode specification
	size - the total size of the file in bytes
	time - the last modification time (Unix time)
	device - the major & minor block device number

DESCRIPTION

	Use all of the Set() functions that you can to determine
	a tar header as well as you can. This is useful to save
	as much information as possible. All the undefined
	information will automatically be filled in with defaults
	when not defined except for the name.

	It is usually not necessary to define both, the user and
	group names and IDs. The IDs will be searched by name when
	only the names are given and vice versa.

	When the tar object is used to save files, then most of
	the information will be gathered through the moIStream
	specified to the AppendFile() function. However, it is
	always a good idea to define the header to the best of
	your knowledge before to call the append functions.

	Once you are done with a header current setup, you can
	call the Reset() function to clear everything at once.

	If you have a major & minor device number, you can generate
	the device number using the following macro:

		makedev(dev_major, dev_minor);

	Similarly, you can extract these numbers with:

		dev_major = major(device);
		dev_minor = minor(device);

SEE ALSO

	Get...(), Reset()

*/
void moTar::moTarHeader::SetType(char type)
{
	f_flags |= TAR_HEADER_TYPE;
	f_type = type;
}


void moTar::moTarHeader::SetName(const moWCString& name)
{
	f_flags |= TAR_HEADER_NAME;
	f_name = name;
}


void moTar::moTarHeader::SetLinkTo(const moWCString& link_to)
{
	f_flags |= TAR_HEADER_LINKTO;
	f_linkto = link_to;
}


void moTar::moTarHeader::SetUserName(const moWCString& user_name)
{
	f_flags |= TAR_HEADER_USERNAME;
	f_username = user_name;
}


void moTar::moTarHeader::SetGroupName(const moWCString& group_name)
{
	f_flags |= TAR_HEADER_GROUPNAME;
	f_groupname = group_name;
}


void moTar::moTarHeader::SetID(uid_t uid, gid_t gid)
{
	f_flags |= TAR_HEADER_ID;
	f_uid = uid;
	f_gid = gid;
}


void moTar::moTarHeader::SetMode(mode_t mode)
{
	f_flags |= TAR_HEADER_MODE;
	f_mode = mode;
}


void moTar::moTarHeader::SetSize(size_t size)
{
	f_flags |= TAR_HEADER_SIZE;
	f_size = size;
}


void moTar::moTarHeader::SetModifTime(time_t time)
{
	f_flags |= TAR_HEADER_MTIME;
	f_mtime = time;
}


void moTar::moTarHeader::SetDevice(dev_t device)
{
	f_flags |= TAR_HEADER_DEVICE;
	f_device = device;
}





/************************************************************ DOC:

CLASS

	moTar::moTarHeader

NAME

	GetType - retrieve the type of this entry
	GetName - retrieve the filename
	GetLinkTo - retrieve the link to which it points
	GetUserName - retrieve the name of the owner
	GetGroupName - retrieve the name of the group
	GetMode - retrieve the mode
	GetID - retrieve the user and group identification numbers
	GetSize - retrieve the file size in bytes
	GetModifTime - retrieve the last modification time (a Unix time)
	GetDevice - retrieve the device major & minor numbers

SYNOPSIS

	bool GetName(moWCString& name) const;
	bool GetLinkTo(moWCString& link_to) const;
	bool GetUserName(moWCString& user_name) const;
	bool GetGroupName(moWCString& group_name) const;
	bool GetMode(mode_t& mode) const;
	bool GetID(uid_t& uid, gid_t& gid) const;
	bool GetSize(size_t& size) const;
	bool GetModifTime(time_t& time) const;
	bool GetDevice(dev_t& device) const;

DESCRIPTION

	The Get() functions will read the header data and return
	true if the corresponding data was defined by the user.

	Note that when a Get() function returns false, the input
	parameter remains unchanged. In other words, it is possible
	to define a default.

SEE ALSO

	Set, Reset

*/
bool moTar::moTarHeader::GetType(char& type) const
{
	if((f_flags & TAR_HEADER_TYPE) == 0) {
		return false;
	}
	type = f_type;
	return true;
}


bool moTar::moTarHeader::GetName(moWCString& name) const
{
	if((f_flags & TAR_HEADER_NAME) == 0) {
		return false;
	}
	name = f_name;
	return true;
}


bool moTar::moTarHeader::GetLinkTo(moWCString& link_to) const
{
	if((f_flags & TAR_HEADER_LINKTO) == 0) {
		return false;
	}
	link_to = f_linkto;
	return true;
}


bool moTar::moTarHeader::GetUserName(moWCString& user_name) const
{
	if((f_flags & TAR_HEADER_USERNAME) == 0) {
		return false;
	}
	user_name = f_username;
	return true;
}


bool moTar::moTarHeader::GetGroupName(moWCString& group_name) const
{
	if((f_flags & TAR_HEADER_GROUPNAME) == 0) {
		return false;
	}
	group_name = f_groupname;
	return true;
}


bool moTar::moTarHeader::GetID(uid_t& uid, gid_t& gid) const
{
	if((f_flags & TAR_HEADER_ID) == 0) {
		return false;
	}
	uid = f_uid;
	gid = f_gid;
	return true;
}


bool moTar::moTarHeader::GetMode(mode_t& mode) const
{
	if((f_flags & TAR_HEADER_MODE) == 0) {
		return false;
	}
	mode = f_mode;
	return true;
}


bool moTar::moTarHeader::GetSize(size_t& size) const
{
	if((f_flags & TAR_HEADER_SIZE) == 0) {
		return false;
	}
	size = f_size;
	return true;
}


bool moTar::moTarHeader::GetModifTime(time_t& time) const
{
	if((f_flags & TAR_HEADER_MTIME) == 0) {
		return false;
	}
	time = f_mtime;
	return true;
}


bool moTar::moTarHeader::GetDevice(dev_t& device) const
{
	if((f_flags & TAR_HEADER_DEVICE) == 0) {
		return false;
	}
	device = f_device;
	return true;
}



/************************************************************ DOC:

CLASS

	moTar::moTarHeader

NAME

	CopyMost - copy everything except the filename, link & type

SYNOPSIS

	void CopyMost(const moTarHeader& header);

PARAMETERS

	header - the header to copy from

DESCRIPTION

	This function will be used when it is necessary to force
	some parameters to a specific value while creating a tar
	file. For instance, one may want to make sure that all the
	files are owned by a specific user (such as root), that
	all have a mode of a+r, all have a modification time set
	to the exact same value without having to change the
	date on the input files, etc.

	The function will test for defined values in the header
	and copy what is necessary to this tar header object.

	Everything is copied except the prefix, name, link to,
	device IDs, the size and the type which are too specific
	to the file being saved.

	Note that the mode will be copied except if the destination
	is of type directory in which case the execution flags
	are merged instead of being copied.

SEE ALSO

	Set...()

*/
void moTar::moTarHeader::CopyMost(const moTarHeader& header)
{
	if((header.f_flags & TAR_HEADER_USERNAME) != 0) {
		f_username = header.f_username;
		f_flags |= TAR_HEADER_USERNAME;
	}
	if((header.f_flags & TAR_HEADER_GROUPNAME) != 0) {
		f_groupname = header.f_groupname;
		f_flags |= TAR_HEADER_GROUPNAME;
	}
	if((header.f_flags & TAR_HEADER_MODE) != 0) {
		if(f_type == DIRTYPE) {
#ifdef WIN32
			f_mode = (f_mode & S_IEXEC) | header.f_mode;
#else
			f_mode = (f_mode & (S_IXUSR|S_IXGRP|S_IXOTH)) | header.f_mode;
#endif
		}
		else {
			f_mode = header.f_mode;
		}
		f_flags |= TAR_HEADER_MODE;
	}
	if((header.f_flags & TAR_HEADER_ID) != 0) {
		f_uid = header.f_uid;
		f_gid = header.f_gid;
		f_flags |= TAR_HEADER_ID;
	}
	if((header.f_flags & TAR_HEADER_MTIME) != 0) {
		f_mtime = header.f_mtime;
		f_flags |= TAR_HEADER_MTIME;
	}
}







/************************************************************ DOC:

CLASS

	moOTar::moTarHeader

NAME

	Define - get file information for the tar header

SYNOPSIS

	bool Define(const moWCString& filename, const moWCString& root);

PARAMETERS

	filename - the name of the directory to put in the output tar
	root - the root path not to be included

DESCRIPTION

	This function can be used to initialize an moTarHeader from
	the information gathered via the system about the specified
	file.

	The root path is used to simulate a cd into that directory
	and thus that path will be removed from the path present
	in the header name (if it matches).

NOTE

	The user and group names are not determined here. This is
	anyway automatic in the tar_header_t.ConvertFrom() function.

RETURN VALUE

	the function returns true when successful

SEE ALSO

	AppendDirectory, AcceptFile

*/
bool moOTar::moTarHeader::Define(const moWCString& filename, const moWCString& root)
{
	struct stat	st;
	int		r;

	Reset();

	// we use lstat() since we don't want to follow links
#ifdef WIN32
	r = stat(filename.c_str(), &st);
#else
	r = lstat(filename.c_str(), &st);
#endif
	if(r != 0) {
		// file doesn't exist (probably) or we don't have permissions
		return false;
	}

	switch(st.st_mode & S_IFMT) {
	//case S_IFSOCK:
	default:
		// we don't support sockets in a tar file!
		// or any other type we don't know about.
		return false;

#ifndef WIN32
	case S_IFLNK:
		SetType(SYMTYPE);	// LNKTYPE is hard link
		break;
#endif

	case S_IFREG:			// AREGFILE is alternative
		SetType(REGTYPE);	// CONTTYPE is contiguous
		break;

#ifndef _MSC_VER
	case S_IFBLK:
		SetType(BLKTYPE);
		break;
#endif

	case S_IFDIR:
		SetType(DIRTYPE);
		break;

	case S_IFCHR:
		SetType(CHRTYPE);
		break;

#ifndef _MSC_VER
	case S_IFIFO:
		SetType(FIFOTYPE);
		break;
#endif

	}
	SetID(st.st_uid, st.st_gid);
	SetMode(st.st_mode);
	SetModifTime(st.st_mtime);
	SetSize(st.st_size);	// is reset for links & dirs below

	if(root.Compare(filename, 0, static_cast<int>(root.Length())) == MO_BASE_COMPARE_EQUAL) {
		moWCString name = filename.Get(static_cast<int>(root.Length()),
						static_cast<int>(filename.Length()));
		if(name.First() == '/') {
			name = name.Delete(0, 0);
		}
		SetName(name);
	}
	else {
		SetName(filename);
	}

	switch(st.st_mode & S_IFMT) {
#ifndef _MSC_VER
	case S_IFBLK:
#endif
	case S_IFCHR:
		SetDevice(st.st_rdev);
		break;

#ifndef WIN32
// TODO: here we need to detect links under MS-Windows! or use usys?
	case S_IFLNK:
		{
			char	lnk[st.st_size + 1];		/* Flawfinder: ignore */
			readlink(filename.c_str(), lnk, st.st_size);	/* Flawfinder: ignore */
			lnk[st.st_size] = '\0';
			SetLinkTo(lnk);
		}
		/*FALLTHROUGH*/
#endif
	case S_IFDIR:
		// NOTE: the correct way could also be to "unset" the size!
		SetSize(0);
		break;

	}

	return true;
}



/************************************************************ DOC:

CLASS

	moTar::tar_header_t

NAME

	ConvertFrom - convert from an moTagHeader to a tar_header_t
	ConvertTo - convert to an moTagHeader from a tar_header_t

SYNOPSIS

	void ConvertFrom(const moTarHeader& header);
	void ConvertTo(moTarHeader& header);

PARAMETERS

	header - an moTarHeader object to copy from or to

DESCRIPTION

	When the moOTar object is ready to save an input file, buffer,
	etc. it needs to convert a tar_header_t to an moTarHeader.
	This is done with the ConvertFrom() function.

	Similarly, the moITar converts the tar_header_t it reads from
	the input file into an moTarHeader with ConvertTo().

RETURNED VALUE

	When the convertion from an moTarHeader to a tar_header_t
	fails (name too long or no name in most cases), then it
	returns false, otherwise true is returned.

	The errno is set to one of the following errors:

		ENOENT		no name was defined

		ENAMETOOLONG	name can't fit in a tar file

		EINVAL		the file type is not acceptable
				for the information available

SEE ALSO

	moTarHeader functions

*/
bool moTar::tar_header_t::ConvertFrom(const moTarHeader& header)
{
	moWCString	name;
	bool		default_type, flag;
	mode_t		mode;
	uid_t		uid;
	gid_t		gid;
	size_t		size;
	time_t		now;
	dev_t		device;

	// clear what won't be fully overwritten
	memset(f_name, 0, sizeof(f_name));
	memset(f_linkname, 0, sizeof(f_linkname));
	memset(f_uname, 0, sizeof(f_uname));
	memset(f_gname, 0, sizeof(f_gname));
	memset(f_prefix, 0, sizeof(f_prefix));
	memset(f_unused, 0, sizeof(f_unused));

	f_typeflag = REGTYPE;
	default_type = !header.GetType(f_typeflag);

	if(!header.GetName(name)) {
		errno = ENOENT;
		return false;
	}
	// NOTE: the MBData() returns multibyte chars and the tar
	// documentation supposes that we only use ISO 646 which
	// includes characters from 20 to 0x7E...
	size = name.MBLength();
	if(size > sizeof(f_name)) {
		if(size > sizeof(f_name) + sizeof(f_prefix) + 1) {
			// name too long altogether!
			errno = ENAMETOOLONG;
			return false;
		}
		// NOTE: +2 because of (1) the nul terminator and
		// (2) the intermediate '/' which we don't save
		char fullname[sizeof(f_name) + sizeof(f_prefix) + 2], *s;	/* Flawfinder: ignore */
		name.MBData(fullname, sizeof(f_name));
		s = fullname + size - sizeof(f_name) - 1;
		while(*s != '/' && *s != '\0') {
			s++;
		}
		if(*s == '\0') {
			// the name part is too long
			errno = ENAMETOOLONG;
			return false;
		}
		if(static_cast<size_t>(s - fullname) > sizeof(f_prefix)) {
			// the prefix part is too long
			errno = ENAMETOOLONG;
			return false;
		}
		memcpy(f_name, s + 1, size - (s - fullname) - 1);		/* Flawfinder: ignore */
		memcpy(f_prefix, fullname, s - fullname);			/* Flawfinder: ignore */
	}
	else {
		// MBData() creates an always null terminated string
		name.MBData(f_name, sizeof(f_name) + 1);
	}

	if(header.GetLinkTo(name)) {
		if(default_type) {
			f_typeflag = SYMTYPE;
		}
		else if(f_typeflag != SYMTYPE && f_typeflag != LNKTYPE) {
			errno = EINVAL;
			return false;
		}
		if(name.MBLength() > sizeof(f_linkname)) {
			errno = ENAMETOOLONG;
			return false;
		}
		name.MBData(f_linkname, sizeof(f_linkname));
	}

	// read-only by user only, unless it's a directory in which
	// case we also want the execute bit.
	mode = f_typeflag == DIRTYPE ? 0500 : 0400;
	header.GetMode(mode);
	snprintf(f_mode, sizeof(f_mode), "%0*o", static_cast<int>(sizeof(f_mode) - 1), mode);	/* Flawfinder: ignore */

	if(header.GetUserName(name)) {
		name.MBData(f_uname, sizeof(f_uname));
	}
	flag = false;
	if(header.GetGroupName(name)) {
		name.MBData(f_gname, sizeof(f_gname));
	}
	else if(f_uname[0] != '\0') {
		// the default group when the user is defined
		// is a copy of the user name as the group name
		flag = true;
		memcpy(f_gname, f_uname, moMin(sizeof(f_gname), sizeof(f_uname)));	/* Flawfinder: ignore */
	}

	uid = 0;
	gid = 0;
#ifdef WIN32
	// TODO: should we look into getting the user/group info under Win32?!
	header.GetID(uid, gid);
#else
	struct passwd	*pw;
	struct group	*gr;

	if(!header.GetID(uid, gid)) {
		if(f_uname[0] != '\0') {
			// try determining the user ID from
			// the passwd file
			pw = getpwnam(f_uname);
			if(pw != 0) {
				uid = pw->pw_uid;
			}
		}
		if(f_gname[0] != '\0') {
			// try determining the group ID from
			// the group file
			gr = getgrnam(f_gname);
			if(gr != 0) {
				gid = gr->gr_gid;
			}
		}
	}
	else {
		if(f_uname[0] == '\0') {
			pw = getpwuid(uid);
			if(pw != 0) {
				strncpy(f_uname, pw->pw_name, sizeof(f_uname) - 1);	/* Flawfinder: ignore */
			}
		}
		if(f_gname[0] == '\0' || flag) {
			gr = getgrgid(gid);
			if(gr != 0) {
				strncpy(f_gname, gr->gr_name, sizeof(f_gname) - 1);	/* Flawfinder: ignore */
			}
		}
	}
#endif
	snprintf(f_uid, sizeof(f_uid), "%0*o", static_cast<int>(sizeof(f_uid) - 1), uid);	/* Flawfinder: ignore */
	snprintf(f_gid, sizeof(f_gid), "%0*o", static_cast<int>(sizeof(f_gid) - 1), gid);	/* Flawfinder: ignore */

	size = 0;
	if(header.GetSize(size) && size > 0) {
		switch(f_typeflag) {
		case REGTYPE:
		case AREGTYPE:
		case CONTTYPE:
			break;

		default:
			// at this time I don't think we can have a size
			// for anything else than a regular file!
			errno = EINVAL;
			return false;

		}
	}
	snprintf(f_size, sizeof(f_size), "%0*o", static_cast<int>(sizeof(f_size - 1)), (unsigned int) size);	/* Flawfinder: ignore */

	time(&now);
	header.GetModifTime(now);
	snprintf(f_mtime, sizeof(f_mtime), "%0*llo", static_cast<int>(sizeof(f_mtime) - 1), static_cast<unsigned long long>(now));	/* Flawfinder: ignore */

	device = 0;
	if(header.GetDevice(device)) {
		switch(f_typeflag) {
		case CHRTYPE:
		case BLKTYPE:
			break;

		default:
			errno = EINVAL;
			return false;

		}
	}
#ifdef WIN32
	// it seems that the rdev of the struct stat is never set anyway! (so we'll always get 0)
	snprintf(f_devmajor, sizeof(f_devmajor), "%0*o", static_cast<int>(sizeof(f_devmajor) - 1), static_cast<int>(device));		/* Flawfinder: ignore */
	snprintf(f_devminor, sizeof(f_devminor), "%0*o", static_cast<int>(sizeof(f_devminor) - 1), 0);					/* Flawfinder: ignore */
#else
	snprintf(f_devmajor, sizeof(f_devmajor), "%0*o", static_cast<int>(sizeof(f_devmajor) - 1), static_cast<int>(major(device)));	/* Flawfinder: ignore */
	snprintf(f_devminor, sizeof(f_devminor), "%0*o", static_cast<int>(sizeof(f_devminor) - 1), static_cast<int>(minor(device)));	/* Flawfinder: ignore */
#endif

	// we're ready, now we can set the magic number, version &
	// compute the checksum
	assert(TMAGLEN == sizeof(f_magic) && TVERSLEN == sizeof(f_version));
	memcpy(f_magic, TMAGIC, TMAGLEN);							/* Flawfinder: ignore */
	//memcpy(f_version, "  ", TVERSLEN);							/* Flawfinder: ignore */
	memcpy(f_version, TVERSION, TVERSLEN);							/* Flawfinder: ignore */
	snprintf(f_chksum, sizeof(f_chksum), "%0*lo", static_cast<int>(sizeof(f_chksum) - 1), CheckSum());	/* Flawfinder: ignore */

	// success!
	return true;
}


bool moTar::tar_header_t::ConvertTo(moTarHeader& header) const
{
	header.Reset();

	// make sure we support this tar format & version
	// NOTE: the Linux tar doesn't set these properly!
	if(memcmp(f_magic, TMAGIC, TMAGLEN - 1) != 0
	/*|| memcmp(f_version, TVERSION, TVERSLEN) != 0*/) {
		errno = EINVAL;
		return false;
	}

	// make sure the check sum is correct
	if(CheckSum() != static_cast<unsigned long>(mowc::strtol(f_chksum, 0, 8, sizeof(f_chksum)))) {
		errno = EINVAL;
		return false;
	}

	// now we take the data
	header.SetType(f_typeflag);
	if(f_prefix[0] == '\0') {
		header.SetName(f_name);
	}
	else {
		header.SetName(moWCString(f_prefix).FilenameChild(f_name));
	}
	header.SetLinkTo(f_linkname);
	header.SetUserName(f_uname);
	header.SetGroupName(f_gname);
	header.SetID(mowc::strtol(f_uid, 0, 8, sizeof(f_uid)), mowc::strtol(f_gid, 0, 8, sizeof(f_gid)));
	header.SetMode(mowc::strtol(f_mode, 0, 8, sizeof(f_mode)));
	header.SetSize(mowc::strtol(f_size, 0, 8, sizeof(f_size)));
	header.SetModifTime(mowc::strtol(f_mtime, 0, 8, sizeof(f_mtime)));
#ifdef WIN32
	header.SetDevice(mowc::strtol(f_devmajor, 0, 8, sizeof(f_devmajor)));
#else
	header.SetDevice(makedev(mowc::strtol(f_devmajor, 0, 8, sizeof(f_devmajor)), mowc::strtol(f_devminor, 0, 8, sizeof(f_devminor))));
#endif

	return true;
}



unsigned long moTar::tar_header_t::CheckSum(void) const
{
	assert(sizeof(tar_header_t) == 512 && sizeof(f_chksum) == 8);
	const unsigned char *s = reinterpret_cast<const unsigned char *>(this);
	// the f_chksum is assumed to be all spaces
	// so we replace the current value by 8 x blank
	unsigned long chksum = 8 * ' '
		- s[offsetof(tar_header_t, f_chksum) + 0]
		- s[offsetof(tar_header_t, f_chksum) + 1]
		- s[offsetof(tar_header_t, f_chksum) + 2]
		- s[offsetof(tar_header_t, f_chksum) + 3]
		- s[offsetof(tar_header_t, f_chksum) + 4]
		- s[offsetof(tar_header_t, f_chksum) + 5]
		- s[offsetof(tar_header_t, f_chksum) + 6]
		- s[offsetof(tar_header_t, f_chksum) + 7];
	size_t size = sizeof(tar_header_t);
	do {
		chksum += *s++;
		size--;
	} while(size > 0);

	return chksum;
}




/************************************************************ DOC:

CLASS

	moOTar

NAME

	Constructor - creates an moOTar object
	Destructor - cleans up an moOTar object

SYNOPSIS

	moOTar(void);
	moOTar(moOStream *output);
	~moOTar();

	private:
	void Init(void);

PARAMETERS

	output - the tar output stream

DESCRIPTION

	The constructors will initialize the output tar object.

	NOTE: the output stream will be handed to the tar object,
	the means the tar object should be the only one writing
	to it. Also, it will automatically close it on destruction.

	The destructor will ensure that the file is properly ended
	and then closed. This means the output file given by the
	user won't be usable afterward.

SEE ALSO

	SetOutput

*/
moOTar::moOTar(void)
{
	Init();
}


moOTar::moOTar(moOStream* output)
{
	Init();
	SetOutput(output);
}


void moOTar::Init(void)
{
	//f_output -- auto-init to 0
}

moOTar::~moOTar()
{
	// we need to call this function to ensure that the end block is saved
	SetOutput(0);
}





/************************************************************ DOC:

CLASS

	moOTar

NAME

	SetOutput - sets the output to the specified stream

SYNOPSIS

	void SetOutput(moOStream *output);
	moOStreamSPtr Output(void) const;

PARAMETERS

	output - the new output stream

DESCRIPTION

	The SetOutput() function assigns an output stream to the
	output tar object. After this calls, the stream should
	be viewed as the property of the tar object until the
	tar object is destroyed or another stream is assigned to
	that same tar object. After a call to the SetOutput() the
	caller can Release() the output stream safely.

	A previous output object will get it's Release() function
	called whenever it is removed from a tar object.

	The Output() function returns a pointer to the output object
	previously linked to the tar object using the SetOutput()
	function.

SEE ALSO

	Init, Constructors, Destructor, Output

*/
void moOTar::SetOutput(moOStream *output)
{
	if(f_output != 0) {
		char end[512];	/* Flawfinder: ignore */
		memset(end, 0, sizeof(end));
		f_output->Write(end, 512);
		f_output->Flush();
		// there is no function to actually close a stream!
	}

	f_output = output;
}


moOStreamSPtr moOTar::Output(void) const
{
	return f_output;
}






/************************************************************ DOC:

CLASS

	moOTar

NAME

	AppendHeader - append a header which doesn't require data

SYNOPSIS

	bool AppendHeader(const moTarHeader& header);

PARAMETERS

	header - an initialized tar file header

DESCRIPTION

	The AppendHeader() function creates an entry in the tar
	file for an object which doesn't require any data to
	be saved.

	It will be used for all the special files such as
	devices, directories and links.

	Note that you could also use the AppendBuffer() with a
	size of zero.

SEE ALSO

	AppendFile, AppendStream, AppendBuffer

*/
bool moOTar::AppendHeader(const moTarHeader& header)
{
	tar_header_t	h;
	moTarHeader	copy(header);

	// make sure the correct size is saved
	copy.SetSize(0);

	// convert and save the header
	if(!h.ConvertFrom(copy)) {
		return false;
	}

	return f_output->Write(&h, sizeof(h)) == sizeof(h);
}


/************************************************************ DOC:

CLASS

	moOTar

NAME

	AppendBuffer - append a buffer as if it were a file

SYNOPSIS

	bool AppendBuffer(const moTarHeader& header, const void *data, size_t size);

PARAMETERS

	header - an initialized tar file header
	data - a pointer to a buffer
	size - the size of the data buffer

DESCRIPTION

	The AppendBuffer() function writes the data pointed by data to
	the output tar file.

	The size can be zero (in which case the data pointer can be null)
	in which case the type of the file being added must be an empty
	type (such as SYMTYPE, LNKTYPE, DIRTYPE). It is also possible to
	include empty regular files, etc. in which case you can also have
	an empty buffer.

SEE ALSO

	AppendFile, AppendStream

*/
bool moOTar::AppendBuffer(const moTarHeader& header, const void *data, size_t size)
{
	tar_header_t	h;
	moTarHeader	copy(header);

	// make sure the correct size is saved
	copy.SetSize(size);

	// convert and save the header
	if(!h.ConvertFrom(copy)) {
		return false;
	}
	if(f_output->Write(&h, sizeof(h)) != sizeof(h)) {
		return false;
	}

	// save the data
	if(f_output->Write(data, size) != static_cast<int>(size)) {
		return false;
	}

	// save the necessary zeroes at the end of the file
	size_t l = sizeof(tar_header_t) - (size & -sizeof(tar_header_t));
	if(l != sizeof(tar_header_t)) {
#ifdef _MSC_VER
		std::auto_ptr<char> end(new char[l]);
		memset(end.get(), 0, l);
		f_output->Write(end.get(), l);
#else
		// avoid malloc+free with g++
		char end[l];
		memset(end, 0, l);
		f_output->Write(end, l);
#endif
	}

	return true;
}


/************************************************************ DOC:

CLASS

	moOTar

NAME

	AppendStream - copy the stream data to the output tar

SYNOPSIS

	bool AppendStream(const moTarHeader& header, moIStream& input);

PARAMETERS

	header - the tar file header representing input
	input - the input stream to read from

DESCRIPTION

	The AppendStream() function reads data from an input stream
	and writes it to this output tar file.

	It first converts the header into a tar file header and saves
	that. This header should be information about the input file
	being saved in the tape archive.

	The number of bytes which will be read from the input stream
	is the size defined in the header parameter, not the size of
	the input stream (thus the input stream doesn't need to be
	able to return a size).

	The input stream should at least have that many bytes
	available.

NOTE

	because you may be trying to save a directory structure from
	a set of files which may not all be placed in a similar
	directory tree, the header won't be changed by this function.

	We will have to have helper functions to setup headers from
	different sources such as a filename.

RETURN VALUE

	when the file is properly duplicated in the tar file, this
	function returns true; if an error occurs converting the
	header, reading the input stream or writing to this out
	tar file, then false is returned

SEE ALSO

	AppendBuffer, AppendFile

*/
bool moOTar::AppendStream(const moTarHeader& header, moIStream& input)
{
#define	BSZ		(BUFSIZ > sizeof(tar_header_t) ? BUFSIZ : sizeof(tar_header_t))
	tar_header_t	h;
	char		buffer[BSZ];	/* Flawfinder: ignore */
	size_t		l, m, size;

	if(!header.GetSize(size)) {
		errno = EINVAL;
		return false;
	}

	// convert and save the header
	if(!h.ConvertFrom(header)) {
		return false;
	}
	if(f_output->Write(&h, sizeof(h)) != sizeof(h)) {
		return false;
	}

	// save the data
	while(size > 0) {
		l = moMin(size, BSZ);
		if(input.Read(buffer, l) != static_cast<int>(l)) {
			return false;
		}
		size -= l;
		// clear the end of the buffer for the last block
		// the last block has to be properly aligned
		m = (l + sizeof(tar_header_t) - 1) & - (long) sizeof(tar_header_t);
		if(l != m) {
			memset(buffer + l, 0, m - l);
			l = m;
		}
		if(f_output->Write(buffer, l) != static_cast<int>(l)) {
			return false;
		}
	}

	return true;
}



/************************************************************ DOC:

CLASS

	moOTar

NAME

	AppendFile - append a file specified by filename

SYNOPSIS

	bool AppendFile(const moTarHeader& header, const moWCString& filename);

PARAMETERS

	header - the header saved in the tar file
	filename - the name of the file to copy in the output tar

DESCRIPTION

	The AppendFile() function will read an entire file from disk
	and copy it in the output tar file.

	The tar header should correspond to the file being saved in
	the file.

	This function is a wrapper of the AppendStream() function.
	It just opens the file for you before to call the
	AppendStream() function.

NOTE

	The header isn't modified by the function, thus it has to
	be properly setup before a call to this function.

	Please, see the AppendStream() function for more information
	about the header.

RETURN VALUE

	The function returns true when nothing goes wrong, false otherwise.

SEE ALSO

	AppendBuffer, AppendStream

*/
bool moOTar::AppendFile(const moTarHeader& header, const moWCString& filename)
{
	moFile		input;
	if(!input.Open(filename)) {
		return false;
	}
	return AppendStream(header, input);
}




/************************************************************ DOC:

CLASS

	moOTar

NAME

	AppendDirectory - append a directory and all the files within

SYNOPSIS

	bool AppendDirectory(const moTarHeader& header, const moWCString& path, const moWCString& root);
	bool AppendDir(const moTarHeader& header, const moDirectory& dir, const moWCString& root);

PARAMETERS

	header - the header saved in the tar file
	path - the name of the directory to copy to the output tar
	dir - if you already have a directory instead of just a path
	root - the root path not to be included

DESCRIPTION

	The AppendDirectory() function reads an entire directory tree
	from disk and copy all the files it in the output tar file.

	The AppendDir() function uses an existing directory and saves
	all the files present in it in the output tar file.

	The tar headers are created using the files inserted in the
	tar file for the names, most of the other parameters can be
	overwriten by passing a tar header with defined parameters
	(thus, you can force a specific user name and mode to be
	copied for all the files). For more information about which
	information is copied, please look at the CopyMost() function
	of the moTarHeader object.

	For each regular file, the AppendFile() function will be
	called. For other files, the AppendHeader() function is
	used instead. More than one directory can be included in
	the same tar file.

	The root path can be set to anything within the filename
	path. It will then be removed from all the filenames
	(in effect, it simulates a cd to the root path before the
	filenames are inserted, except ../ won't be generated).

	One can derive the moOTar object in order to apply a filter
	on the files to be included. You can then reject some files
	from being included (because these are backup files, object
	files which can be regenerated, etc.) The function to be
	overwritten is AcceptFile(). This can also be reached by
	creating your own moDirectory list.

NOTE

	Please, see the AppendStream() and moTarHeader::Define
	functions for more information about the header.

RETURN VALUE

	The function returns true when nothing goes wrong, false otherwise.

BUGS

	The errno value is usually invalid at the time the function
	returns. We may add a LastErrno() function at a later time
	if it seems to be necessary.

SEE ALSO

	AppendBuffer, AppendStream, AppendHeader
	moTarHeader::CopyMost, moTarHeader::Define

*/
bool moOTar::AppendDirectory(const moTarHeader& header, const moWCString& path, const moWCString& root)
{
	moDirectory dir(path, moDirectory::DIR_FLAG_HIDDEN_CHILDREN | moDirectory::DIR_FLAG_RECURSIVE);
	return AppendDir(header, dir, root);
}


bool moOTar::AppendDir(const moTarHeader& header, const moDirectory& dir, const moWCString& root)
{
	moList::position_t	pos, max;
	moTarHeader		head;
	char			type;

	max = dir.Count();
	pos = 0;
	while(pos < max) {
		moDirectory::moEntrySPtr entry = dir.Get(pos);
		head.Define(entry, root);
		head.CopyMost(header);
		// make sure the user wants this file
		if(AcceptFile(head, entry)) {
			// just in case, use a default (avoid the warnings too)
			type = REGTYPE;
			head.GetType(type);
			switch(type) {
			case REGTYPE:
			case AREGTYPE:
			case CONTTYPE:
				if(!AppendFile(head, entry)) {
					return false;
				}
				break;

			default:
				if(!AppendHeader(head)) {
					return false;
				}
				break;

			}
		}
		pos++;
	}

	return true;
}



bool moOTar::AcceptFile(const moTarHeader& header, const moWCString& filename)
{
	// accept all the files by default
	return true;
}



};			// namespace molib;

// vim: ts=8

