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



#ifndef MO_DIRECTORY_H
#define	MO_DIRECTORY_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_LIST_H
#include	"mo_list.h"
#endif
#ifndef MO_STRING_H
#include	"mo_string.h"
#endif


namespace molib
{

// a directory is a list of fully qualified file names
// (i.e. the name includes the path, filename, extension)
// use the moWCString object to extract the different
// parts of the filename (FilenameBasename, FilenameExtension,
// FilenameChild, etc.)
class MO_DLL_EXPORT moDirectory : public moSortedListUnique
{
public:
	static const unsigned long	DIR_FLAG_HIDDEN		= 0x00000001;
	static const unsigned long	DIR_FLAG_RECURSIVE	= 0x00000002;
	static const unsigned long	DIR_FLAG_HIDDEN_CHILDREN= 0x00000004;

	// The following are part of the flags but should not be used by
	// the caller. It is used internally to avoid infinite loops in
	// recursive mode.
	static const unsigned long	DIR_INCR_REENTRY	= 0x01000000;
	static const unsigned long	DIR_MASK_REENTRY	= 0xFF000000;
	static const unsigned long	DIR_MAX_REENTRY		= 0x32000000;
	// WARNING: the maximum for DIR_MAX_REENTRY is 0xFE000000

				moDirectory(void);
				moDirectory(const moWCString& path, unsigned long flags = 0);

	bool			Read(const moWCString& path, unsigned long flags = 0);
	bool			ReadExpand(const moWCString& path, unsigned long flags = 0);
	virtual void		Append(const moListBase *list);
	void			AppendEmpty(moDirectory& dir);

	class moEntry : public moWCString
	{
	public:
					moEntry(const moWCString& path, const moWCString& name);
					moEntry(const moEntry& entry);

		moWCString&		FullPath(void) const;

		bool			Stat(struct stat *st) const;
		bool			LStat(struct stat *st) const;

		bool			IsReg(void) const;
		bool			IsDir(void) const;
		bool			IsChr(void) const;
		bool			IsBlk(void) const;
		bool			IsFIFO(void) const;
		bool			IsLnk(void) const;
		bool			IsSock(void) const;

	private:
		// the mutable parameters are only place holders to
		// avoid recomputing their values each time the corresponding
		// function is called
		mutable bool		f_stat_defined;
		mutable struct stat	f_stat;
		mutable bool		f_lstat_defined;
		mutable struct stat	f_lstat;
		mutable moWCString	f_fullpath;
	};

	typedef moSmartPtr<moEntry>		moEntrySPtr;
	typedef moTmplList<moEntry, moList>	moListOfEntries;

	moEntrySPtr		Get(position_t position) const;
	const moEntry&		operator [] (int position) const;

private:
	bool			ReadDir(const moWCString& path, const moWCString& pattern, unsigned long flags, moList& list);
};



};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_DIRECTORY_H

