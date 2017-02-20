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



#ifndef MO_PASSWD_H
#define	MO_PASSWD_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_TEXT_STREAM_H
#include	"mo_text_stream.h"
#endif
#ifndef MO_WORDS_H
#include	"mo_words.h"
#endif


namespace molib
{



class MO_DLL_EXPORT moPasswd : public moList
{
public:
	class moUser : public moWords
	{
	public:
					moUser(const moWCString& separators = ":");

		virtual unsigned long	SetWords(const moWCString& words);
		bool			IsComment(void) const;

	private:
		zbool_t			f_comment;
	};
	typedef moSmartPtr<moUser>	moUserSPtr;

	enum mode_t {
		MO_MODE_EQUAL,
		MO_MODE_CIEQUAL,
		MO_MODE_REGEX,
		MO_MODE_INTEGER,
		MO_MODE_IP
	};

				moPasswd(bool can_save = false);

	void			Load(moTextStream& text_stream);
	void			Save(moTextStream& text_stream) const;

	moUserSPtr		Find(const moWCString& key, int column = 0, mode_t mode = MO_MODE_EQUAL) const;
	moUserSPtr		FindNextUser(void) const;
	moUserSPtr		operator [] (const moWCString& name) const;
	moUserSPtr		Get(int index) const;
	moUserSPtr		operator [] (int index) const;

private:
	const mbool_t		f_can_save;
	mutable moWCString	f_key;
	mutable int		f_column;
	mutable mode_t		f_mode;
	mutable mint32_t	f_last_found;
};

typedef moSmartPtr<moPasswd>	moPasswdSPtr;




};		// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_PASSWD_H

