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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_directory.h"
#endif

#include	"mo/mo_directory.h"
#include 	"mo/mo_dirent.h"

#ifdef _MSC_VER
#   pragma warning(disable: 4996)
#endif


namespace molib
{


/** \class moDirectory
 *
 * \brief Read a directory with support of Unix like patterns.
 *
 * The Directory object can be used to read a directory which
 * is automatically saved in a sorted list of strings.
 * 
 * You can use the same directory object to read multiple
 * directories cumulatively.
 */



/** \brief Initialize an empty directory.
 *
 * The default constructor creates an empty directory object.
 *
 * \sa moDirectory::Read(const moWCString& path, unsigned long flags)
 * \sa moDirectory::ReadExpand(const moWCString& path, unsigned long flags)
 */
moDirectory::moDirectory(void)
{
}


/** \brief Creates a new directory object.
 *
 * When used with a path and a set of flags, the constructor calls
 * the Read() function. Please, see that function reference for
 * more information.
 * 
 * \param[in] path The path to the directory to read.
 * \param[in] flags Whether to read recursively and hidden files.
 *
 * \sa moDirectory::Read(const moWCString& path, unsigned long flags)
 * \sa moDirectory::ReadExpand(const moWCString& path, unsigned long flags)
 */
moDirectory::moDirectory(const moWCString& path, unsigned long flags)
{
	Read(path, flags);
}



/** \brief Expand a path and read all the resulting paths.
 *
 * This function calls the moString::FilenameExpand() function to
 * expand the names defined between { and }. This generates an array
 * of strings that represent all the possibilities. This list is then
 * used to call the moDirectory::Read() function with each possible
 * path.
 *
 * \param[in] path The path to expand.
 * \param[in] flags The flags to define what needs to be read (see the Read() function).
 * \return true when at least one path was valid.
 *
 * \sa moDirectory::Read(const moWCString& path, unsigned long flags)
 */
bool moDirectory::ReadExpand(const moWCString& path, unsigned long flags)
{
	bool			result;
	moListOfWCStrings	list;
	moList::position_t	p;
	moWCStringSPtr		name;

	result = path.FilenameExpand(list);

	// NOTE: the order in which the reads are done doesn't matter
	//	 since the resulting list is sorted and duplicates
	//	 removed.
	p = list.Count();
	while(p > 0UL) {
		p--;
		name = list.Get(p);
		if(!Read(name, flags)) {
			result = false;
		}
	}

	return result;
}

/** \var moDirectory::DIR_FLAG_HIDDEN
 *
 * \brief Read the hidden files.
 *
 * This flag is used to ask the Read() function to read all the files,
 * including hidden files. By default, hidden files are ignored.
 */


/** \var moDirectory::DIR_FLAG_RECURSIVE
 *
 * \brief Read directories recursively.
 *
 * When this flag is set, the files read are checked to know whether they
 * are directory. If so, then all the files within are read and included
 * in the moDirectory list.
 *
 * \bug
 * Note that the recursivity is limited to 50 levels because it uses the
 * processor stack.
 */


/** \var moDirectory::DIR_FLAG_HIDDEN_CHILDREN
 *
 * \brief Used along the RECURSIVE flag	so hidden files are read within sub-directories.
 *
 * By default, the DIR_FLAG_HIDDEN flag is only effective in the first level
 * directory. In order to read hidden files at all levels, use both flags:
 * DIR_FLAG_HIDDEN and DIR_FLAG_HIDDEN_CHILDREN.
 */



/** \brief Reads a directory.
 *
 * This function reads a directory using the path including Unix
 * like patterns passed as its \p path parameter.
 *
 * The function can be called multiple times. The files found are
 * added to the list (the previous files are not cleared automatically.)
 *
 * Note that if the same path is read twice, new files found in the
 * given directory will be added, but files that were deleted in
 * that directory will not be removed from the moDirectory object.
 * 
 * In order to read a new directory with an empty list, you
 * need to call the Empty() function prior to the Read() or
 * ReadExpand() functions.
 * 
 * Note that the pattern matching is done as in a Unix shell.
 * Thus *.* does not match files not including a period (.).
 * A pattern can appear anywhere within the path. We support
 * the asterisk (*), question mark (?) and square brackets
 * ([...]). If no file matches, then nothing is added to the
 * list.
 * 
 * IMPORTANT NOTE: without a pattern within your path nothing
 * will be read, the path is checked for existance and if it
 * exists, it is added as is in the directory list.
 * 
 * If you want to get support for braces expension, you need
 * to use the ReadExpand() function instead.
 * 
 * If the hidden flag is set, then files that start with a
 * period (.) will also be included in the result. Under
 * systems other than Unix, we may use the file attributes to know
 * whether or not a file is hidden, which may generate a slowdown
 * reading really large folders.
 * 
 * When the RECURSIVE flag is set, the function reads the
 * directory at the given path. Then it checks each file to
 * determine whether or not there are directories within this
 * directory. If so, these are read. Note that the function is
 * written in such a way that if a top directory is already
 * present in the moDirectory list, then its contents will not
 * be read again. This means new files within sub-directories
 * will not appear in the resulting list unless you call Empty()
 * before you call Read() or ReadExpand().
 * 
 * The RECURSIVE flag uses the stack and thus it should be
 * used with caution. Only 50 levels of sub-directories will
 * be read. If there are more levels, it is ignored, but the
 * function returns false since it will have failed to read
 * all the possible files.
 * 
 * Note that in some cases, it is possible to avoid the
 * RECURSIVE by using patterns such as:
 * 
 * 	/my/web/pages/\?\?/abc*.html
 * 
 * This will search all the directories with two letters
 * that include HTML files starting with "abc". If you
 * can use such a path instead of recursivity, the function
 * will certainly be much faster.
 *
 * \note
 * A returned value of false does not mean nothing was read.
 * You need to check the size of the list to know whether
 * it grows between calls to have a definite answer.
 *
 * \bug
 * At this time the errno is lost when an error occurs.
 * 
 * \param[in] path The path to the directory to read and patterns
 * 	(without at least one pattern, the path existance is checked)
 * \param[in] flags A set of DIR_FLAG_... bits.
 * 
 * \return this function returns true when the entire directory was
 *	   read successfully, false otherwise; when the RECURSIVE flag
 *	   is turned on, then all the directories that can be read will
 *	   be read, but the function will return false if an error is
 *	   detected in any one of these directories.
 *
 * \sa moSortedListUnique::Empty()
 */	   
bool moDirectory::Read(const moWCString& path, unsigned long flags)
{
	const mowc::wc_t	*s, *start, *last;
	moWCStringSPtr		str;
	moWCString		name, pattern, prefix;
	moEntrySPtr		entry;
	moListOfEntries		list;
	bool			result, too_many_subdir;
	unsigned long		cnt;
	struct stat		st;

	list += *new moEntry("", "");	// start with an empty entry
	result = true;
	s = path.Data();
	last = start = s;
	while(*s != '\0') {
		switch(*s++) {
		case '/':
			last = s;
			break;

		case '*':
		case '?':
		case '[':
			while(*s != '/' && *s != '\0') {
				s++;
			}
			prefix.Set(start, static_cast<int>(last - start));
			pattern.Set(last, static_cast<int>(s - last));
			cnt = list.Count();
			while(cnt > 0) {
				cnt--;
				str = list.Get(0);
				name = str->FilenameChild(prefix);
				list.Delete(0);
				if(!ReadDir(name, pattern, flags, list)) {
					result = false;
				}
			}
			// if the list is empty we don't have to continue
			if(*s == '\0' || list.IsEmpty()) {
				goto done;
			}
			last = start = s;
			break;

		}
	}

	// TODO:
	// the following ensures the existance of the "pattern"
	// (file) but we could certainly make it a bit faster
	// with a stat(3) instead
	prefix.Set(start, static_cast<int>(last - start));
	pattern.Set(last, static_cast<int>(s - last));
	cnt = list.Count();
	while(cnt > 0) {
		cnt--;
		str = list.Get(0);
		name = str->FilenameChild(prefix);
		list.Delete(0);
		if(!ReadDir(name, pattern, flags, list)) {
			result = false;
		}
	}

done:
	// now we can move the resulting list of files to our
	// directory list
	cnt = list.Count();
	while(cnt > 0) {
		cnt--;
		entry = list.Get(cnt);
		if(!Insert(entry)) {
			// we didn't get it inserted because it already
			// exists in the list! so get rid of it.
			//delete entry; -- auto-released

			// we've got to remove it from the list object
			// so we can go through it again as shown below
			// (NOTE: directories which will be read recursively
			// are also included in this moDirectory object)
			list.Delete(cnt);
		}
	}

	// entries left in the list were inserted in the moDirectory and
	// thus we need to check them for directories in case the RECURSIVE
	// flag is ON
	if((flags & DIR_FLAG_RECURSIVE) != 0) {
		// read hidden files in sub-directories?
		if((flags & DIR_FLAG_HIDDEN_CHILDREN) != 0) {
			flags |= DIR_FLAG_HIDDEN;
		}
		too_many_subdir = (flags & DIR_MASK_REENTRY) == DIR_MAX_REENTRY;
		flags += DIR_INCR_REENTRY;
		cnt = list.Count();
		while(cnt > 0) {
			cnt--;
			entry = list.Get(cnt);
			if(entry->LStat(&st) && S_ISDIR(st.st_mode)) {
				// NOTE: we are testing the re-entry counter
				//	 here since in the lowest accepted
				//	 level there may be only regular files!
				// too many sub-directories?
				if(too_many_subdir) {
					result = false;
					break;
				}
				if(!Read(entry->FilenameChild("*"), flags)) {
					result = false;
				}
			}
		}
	}

	return result;
}






/** \brief Read a directory.
 *
 * This internal function is the one really reading a directory.
 *
 * The moDirectory::Read() searches for a directory first, then calls
 * this function to get all the files that math the specified pattern
 * in that directory.
 *
 * \param[in] path The exact path to the directory to read (no pattern.)
 * \param[in] pattern The pattern used to match the files found in the directory.
 * \param[in] flags Determine whether hidden files will be included.
 * \param[in] list List where all the directory entries are saved.
 *
 * \return true if no error occurs; false otherwise.
 */
bool moDirectory::ReadDir(const moWCString& path, const moWCString& pattern, unsigned long flags, moList& list)
{
	const char	*p;
	DIR		*d;
	struct dirent	*e;
	bool		hidden;

	hidden = (flags & DIR_FLAG_HIDDEN) != 0;

	// here, empty strings are taken as the current
	// directory (which happens in a pattern such
	// as "test*" which doesn't include any path)
	p = path.SavedMBData();
	if(p[0] == '\0') {
		p = ".";
	}

	d = opendir(p);
	if(d == 0) {
		return false;
	}

	errno = 0;
	e = readdir(d);
	while(e != 0) {
		// always skip "." and "..", they are not really useful;
		// skip Unix hidden files (files starting with '.')
		// unless the user requested them
		if(strcmp(e->d_name, ".") != 0
		&& strcmp(e->d_name, "..") != 0
		&& (hidden || e->d_name[0] != '.')) {
			moWCString name(e->d_name, -1, mowc::MO_ENCODING_UTF8);
			// matching the user specified pattern?
			if(name.Glob(pattern)) {
				list += *new moEntry(path, name);
			}
		}
		e = readdir(d);
	}
	closedir(d);

	return errno == 0;
}







/** \brief Append a list of entries to this directory.
 *
 * This function accepts a list of entries (usually another moDirectory)
 * and appends all the entries to this directory.
 *
 * The list can include other types of objects in which case they are
 * ignored.
 *
 * \param[in] list A list of moEntry objects
 */
void moDirectory::Append(const moListBase *list)
{
	position_t	i, max;
	const moEntry	*entry;

// anything to insert?
	if(list == 0) {
		return;
	}

// insert items one by one at the end (append) using the Insert() function
	max = list->Count();
	for(i = 0; i < max; ++i) {
		entry = dynamic_cast<const moEntry *>(list->Get(i));
		if(entry != 0) {
			Insert(new moEntry(*entry));
		}
	}
}



/** \brief Append a directory to another and clear the source.
 *
 * This function appends the entries defined in the specified
 * input directory to this directory.
 *
 * Then it clears all the entry defined in the input \p dir
 * and returns.
 *
 * \param[in,out] dir The directory to copy and then clear.
 */
void moDirectory::AppendEmpty(moDirectory& dir)
{
	position_t	i;

// insert items one by one using the Insert function
	i = dir.Count();
	while(i > 0UL) {
		i--;
		Insert(dir.Get(i));
	}

	dir.Empty();
}




/************************************************************ DOC:

CLASS

	moDirectory

NAME

	Get - get the specified entry
	operator [] - get the specified entry

SYNOPSIS

	moEntry *Get(position_t position) const;
	const moEntry& operator [] (int position) const;

PARAMETER

	position - the item to retrieve

DESCRIPTION

	These functions are both overloads of the lower level moList
	functions. This enables us to have an moEntry object instead
	of an moBase (to avoid casting all the time).

SEE ALSO

	moList

*/
moDirectory::moEntrySPtr moDirectory::Get(position_t position) const
{
	return dynamic_cast<moEntry *>(moSortedListUnique::Get(position));
}



const moDirectory::moEntry& moDirectory::operator [] (int position) const
{
	return dynamic_cast<const moEntry&>(moSortedListUnique::operator [] (position));
}



/************************************************************ DOC:

CLASS

	moDirectory::moEntry

NAME

	Constructors - initialize a new directory entry

SYNOPSIS

	moEntry(const moWCString& path, const char *name);
	moEntry(const moEntry& entry);

PARAMETER

	path - the path where the file was found
	name - the new filename in UTF-8
	entry - an entry to duplicate

DESCRIPTION

	This function initializes a new directory entry or duplicates
	and existing entry.

SEE ALSO

	Read

*/
moDirectory::moEntry::moEntry(const moWCString& path, const moWCString& name)
	: moWCString(path.FilenameChild(name))
{
	f_stat_defined = false;
	f_lstat_defined = false;
}



moDirectory::moEntry::moEntry(const moEntry& entry)
	: moWCString(entry)
{
	f_stat_defined = entry.f_stat_defined;
	f_stat = entry.f_stat;
	f_lstat_defined = entry.f_lstat_defined;
	f_lstat = entry.f_lstat;
	f_fullpath = entry.f_fullpath;
}




/************************************************************ DOC:

CLASS

	moDirectory::moEntry

NAME

	FullPath - defines the absolute path starting with /

SYNOPSIS

	moWCString FullPath(void) const;

DESCRIPTION

	This function determines the full path of the
	given entry. The full path is the path starting
	from the root directory up to the given file.

	The function will save the path so multiple calls
	to it will be very fast.

	Note that this can very well be equal to the
	entry string.

	If you need the real path (by removing symbolic
	links), you should use the FullPath() function
	present in the moFile instead:

		full_path = moFile::FullPath(*entry);

RETURNED VALUE

	the full path of this entry.

BUGS

	This function assumes that the current working
	directory didn't change from the time the
	directory was read and this call happens.

	The current path can't be mode than 4000 UTF-8
	characters.

SEE ALSO

	Stat, LStat

*/
moWCString& moDirectory::moEntry::FullPath(void) const
{
	if(f_fullpath.IsEmpty()) {
#ifdef WIN32
		mowc::wc_t c;
#endif
		if(
			this->IsEmpty()
#ifdef WIN32
			|| (c = mowc::toupper(Get(0)), c < 'A' || c > 'Z')
			|| Get(1) != ':'
			|| (c = Get(2), c != '/' && c != '\\')
#else
			|| Get(0) != '/'
#endif
		)
		{
			char buf[4096];	/* Flawfinder: ignore */
			if(getcwd(buf, sizeof(buf)) == 0) {
				// if 4096 isn't enough...
				return f_fullpath;
			}
			moWCString cwd(buf);
			f_fullpath = cwd.FilenameChild(*this);
		}
		else {
			f_fullpath = *this;
		}
		f_fullpath = f_fullpath.FilenameClean();
	}

	return f_fullpath;
}


/************************************************************ DOC:

CLASS

	moDirectory::moEntry

NAME

	Stat - retrieve the statistics about the file

SYNOPSIS

	bool Stat(struct stat *st) const;
	bool LStat(struct stat *st) const;

PARAMETER

	st - the statistics are copied in this structure

DESCRIPTION

	The Stat() function reads the statistics about
	the file using the low level stat(2) function.
	Note that the low level stat(2) function will
	be used only once. This means changes to the
	file won't be detected.

	The LStat() function is similar except it won't
	follow soft links. This is the function used
	to read directories recursively. See the low
	level lstat(2) for more information. Like the
	stat(2) function, the lstat(2) will be called
	only the first time the LStat() function is
	called.

RETURNED VALUE

	these functions return true unless the statistics
	can't be read (file was deleted since read from
	the directory, permission denied...)

SEE ALSO

	Constructor, FullPath

*/
bool moDirectory::moEntry::Stat(struct stat *st) const
{
// TODO: I think this function should resolve links before to stat() so
//	 that way it would return the stats of the target, not the link!
	if(!f_stat_defined) {
		if(stat(c_str(), &f_stat) < 0) {
			return false;
		}
		f_stat_defined = true;
	}

	memcpy(st, &f_stat, sizeof(f_stat));	/* Flawfinder: ignore */

	return true;
}


bool moDirectory::moEntry::LStat(struct stat *st) const
{
	if(!f_lstat_defined) {
		int r;
#ifdef WIN32
		// Windows doesn't know how to follow softlinks!
		r = stat(c_str(), &f_lstat);
#else
		r = lstat(c_str(), &f_lstat);
#endif
		if(r < 0) {
			return false;
		}
		f_lstat_defined = true;
	}

	memcpy(st, &f_lstat, sizeof(f_lstat));	/* Flawfinder: ignore */

	return true;
}




/************************************************************ DOC:

CLASS

	moDirectory::moEntry

NAME

	IsReg - check whether the file is a regular file
	IsDir - check whether the file is a directory
	IsChr - check whether the file is a character device
	IsBlk - check whether the file is a block device
	IsFIFO - check whether the file is a pipe
	IsLnk - check whether the file is a link
	IsSock - check whether the file is a socket

SYNOPSIS

	bool IsReg(void) const;
	bool IsDir(void) const;
	bool IsChr(void) const;
	bool IsBlk(void) const;
	bool IsFIFO(void) const;
	bool IsLnk(void) const;
	bool IsSock(void) const;

DESCRIPTION

	All of these functions can be used to know what type of a
	file this entry points to.

	These functions returns true whenever the type corresponds
	to the name of the function.

RETURNED VALUE

	true whenever the test succeeds

SEE ALSO

	Stat, stat(2)

*/
bool moDirectory::moEntry::IsReg(void) const
{
	struct stat	st;

	if(!Stat(&st)) {
		// file deleted since or added by the end
		return false;
	}

	return S_ISREG(st.st_mode);
}


bool moDirectory::moEntry::IsDir(void) const
{
	struct stat	st;

	if(!Stat(&st)) {
		// file deleted since or added by the end
		return false;
	}

	return S_ISDIR(st.st_mode);
}


bool moDirectory::moEntry::IsChr(void) const
{
	struct stat	st;

	if(!Stat(&st)) {
		// file deleted since or added by the end
		return false;
	}

	return S_ISCHR(st.st_mode);
}


bool moDirectory::moEntry::IsBlk(void) const
{
#ifdef _MSC_VER
	return false;
#else
	struct stat	st;

	if(!Stat(&st)) {
		// file deleted since or added by the end
		return false;
	}

	return S_ISBLK(st.st_mode);
#endif
}


bool moDirectory::moEntry::IsFIFO(void) const
{
	struct stat	st;

	if(!Stat(&st)) {
		// file deleted since or added by the end
		return false;
	}

	return S_ISFIFO(st.st_mode);
}


bool moDirectory::moEntry::IsLnk(void) const
{
#ifdef WIN32
	size_t	len = Length();
	if(len < 5) {
		return false;
	}

	// under win32 links end with .lnk
	return CaseCompare(".lnk", static_cast<unsigned int>(len - 4)) == MO_BASE_COMPARE_EQUAL;
#else
	struct stat	st;

	if(!LStat(&st)) {
		// file deleted since or added in the list by the user
		return false;
	}

	return S_ISLNK(st.st_mode);
#endif
}


bool moDirectory::moEntry::IsSock(void) const
{
#ifdef WIN32
	return false;
#else
	struct stat	st;

	if(!Stat(&st)) {
		// file deleted since or added by the end
		return false;
	}

	return S_ISSOCK(st.st_mode);
#endif
}


}			// namespace molib;

// vim: ts=8 sw=8

