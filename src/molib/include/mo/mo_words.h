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



#ifndef MO_WORDS_H
#define	MO_WORDS_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STRING_H
#include	"mo_string.h"
#endif
#ifndef MO_LIST_H
#include	"mo_list.h"
#endif



namespace molib
{


class MO_DLL_EXPORT moWords : public moList
{
public:
				moWords(void);
				moWords(const moWCString& char_separators);
	virtual			~moWords(void);

	void			CharSeparators(const moWCString& str);
	void			StringSeparators(const moWCString& str);
	void			ClearSeparators(void);

	void			AutoClip(bool status = true);
	void			EmptyWords(bool status = true);
	void			Quotes(const moWCString& str);

	virtual unsigned long	SetWords(const moWCString& words);
	unsigned long		AddWords(const moWCString& words);

	const moWCString&	AllWords(void) const;

	const moWCString&	Get(int index) const;
	const moWCString&	operator [] (int index) const;

private:
	void			Init(void);

	bool			f_auto_clip;
	bool			f_empty_words;
	moWCString		f_quotes;
	moWCString		f_char_separators;
	moList			f_str_separators;
	moWCString		f_words;
};






};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_WORDS_H

