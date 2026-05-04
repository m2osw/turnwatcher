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
#pragma implementation "mo/mo_passwd.h"
#endif

#include	"mo/mo_passwd.h"


#ifndef MO_REGEXPR_H
#include	"mo/mo_regexpr.h"
#endif


namespace molib {








/************************************************************ DOC:

CLASS

	moPasswd

NAME

	Constructor -- initialize an moPasswd object

SYNOPSIS

	moPasswd(bool can_save = false);

PARAMETERS

	can_save -- wether the current password list can be saved

DESCRIPTION

	The constructor initializes an moPasswd object so you
	can use it to read the read a password file.

	The can_save parameter can be used to allow the Save()
	instruction to work. Otherwise, the Save() instruction
	will throw an error.

SEE ALSO

	Load, Save, Find, Get

*/
moPasswd::moPasswd(bool can_save)
	: f_can_save(can_save),
	  f_last_found(-1)
{
}











/************************************************************ DOC:

CLASS

	moPasswd

NAME

	Load -- load a password file
	Save -- saves the current password info in a file

SYNOPSIS

	void Load(moTextStream& text_stream);
	void Save(moTextStream& text_stream);

PARAMETERS

	text_stream - a valid text stream to load/save from

DESCRIPTION

	The Load() instruction reads the specified text stream,
	line by line until it reaches the end of the file. The
	entire file is always read. It is not expected to be
	used on extremely large files. If you have a really
	large number of users (or expect to have such a large
	number), you may want to use either a database or one
	file per letter or two letters of the beginning of the
	login name of the users.

	The previous list of passwords will be lost after the
	Load() instruction was called even if the input file
	can't be read.

	Once a file was loaded, you can get the users using
	one of the Find() or Get() functions.

	The Save() instruction can be used to save the password
	in a text stream. The text stream needs to have been
	opened for writing. The Save() will not seek anywhere.
	It is your responsability to seek at the beginning of
	the file and resize it as required.

	It is up to you to define the output stream with the
	proper filter to save the text in UTF-8. Don't forget
	that by default our text streams use UCS-32.

ERRORS

	The constructor must have been called with 'true'
	otherwise the Save() instruction will throw an error.

SEE ALSO

	Find, Get, Constructor

*/
void moPasswd::Load(moTextStream& text_stream)
{
	moWCString		line;

	Empty();

	while(text_stream.NextLine(line)) {
		moUserSPtr user = new moUser(":");
		user->SetWords(line);
		Insert(user);
	}
}


void moPasswd::Save(moTextStream& text_stream) const
{
	moList::position_t	idx, max, j, cnt;
	moWCString		line;

	max = Count();
	for(idx = 0; idx < max; ++idx) {
		moUserSPtr user = Get(idx);
		if(user->IsComment()) {
			line = user->AllWords();
		}
		else {
			// rebuild the line because the user may
			// have changed some entries (i.e. password)
			cnt = user->Count();
			if(cnt > 0UL) {
				line = user->Get(0);
				for(j = 1; j < cnt; ++j) {
					line += moWCString(":") + user->Get(j);
				}
			}
			else {
				line.Empty();
			}
		}
		text_stream.Print("%S\n", line.Data());
	}
}




/************************************************************ DOC:

CLASS

	moPasswd

NAME

	Find -- find an entry by name
	Get -- get an entry by index
	operator [] -- get a password by key or index

SYNOPSIS

	moUserSPtr Find(const moWCString& key, int column = 0, mode_t mode = MO_MODE_EQUAL);
	moUserSPtr FindNextUser(void) const;
	moUserSPtr operator [] (const moWCString& key);
	moUserSPtr Get(int index);
	moUserSPtr operator [] (int index);

PARAMETERS

	key - a user name or some other entry string
	column - the column to compare the key with
	mode - the mode of comparison
	index - the index to get

DESCRIPTION

	The Find() function searches the list of entries and
	returns the first one which matches the key. By default
	it assumes that the key is compared as is and that the
	first column has to be compared (i.e. column 0).

	If you use a key which can match more than one entries,
	then you can use the FindNextUser() function to search for
	the following entries. You don't need to specify the
	key, column and mode again. However, this feature is
	not multi-thread safe in the even you want to modify
	the password file. You will be responsible for proper
	synchronization.

	The mode is defined as follow:

		MO_MODE_EQUAL		compare the key and user
					column entry letter for
					letter

		MO_MODE_CIEQUAL		compare the key and user
					column entry letter for
					letter in case insensitive
					manner

		MO_MODE_REGEX		assumes that the key is a
					regular expression

		MO_MODE_INTEGER		assumes that the key and
					column to be compared
					are both integers

		MO_MODE_IP		assumes that the key and
					column to be compared
					are both IP addresses;
					the key can include a
					mask (x.x.x.x/m.m.m.m)

	Empty entries, entries which are comments and entries
	which have less columns than the column indicated for
	search with Find() are all skipped.

	The [] operator with a string searches like the default
	Find() function. It assumes you want to search column
	zero and that the key has to match exactly.

	The Get() and [] operator with an integer both return
	the entry at the specified index. The number of entries
	can be queried with the Count() instruction. Trying to
	get an entry which doesn't exist results in a throw.
	Note that in this way you will get any moUser entry
	including comments and empty lines.

	We assume that you will use these instructions after
	you loaded a password file. You can also create new
	user entries and simply add them to the list before
	to search for them later.

RETURN VALUE

	All of these instructions return an moUser smart pointer
	which can be null when the user isn't found.

ERRORS

	The Get() instruction used with an index out of range
	throws an error.

BUGS

	The Find() and FindNextUser() functions are not
	thread safe. If you need to handle the same
	password file in two different files, you
	certainly want to synchronize them properly.
	If you will only read the password file, then
	you can simply use to moPasswd objects.

SEE ALSO

	Load

*/
moPasswd::moUserSPtr moPasswd::Find(const moWCString& key, int column, mode_t mode) const
{
	f_key = key;
	f_column = column;
	f_mode = mode;

	return FindNextUser();
}


moPasswd::moUserSPtr moPasswd::FindNextUser(void) const
{
	moList::position_t	idx, max;
	moRegularExpression	re;
	moUserSPtr		user;
	long			integer;
	unsigned long		address, mask, user_address, user_mask;
	unsigned short		port;

	integer = 0;

	switch(f_mode) {
	case MO_MODE_EQUAL:
	case MO_MODE_CIEQUAL:
		// no initialization required
		break;

	case MO_MODE_REGEX:
		re = f_key;
		break;

	case MO_MODE_INTEGER:
		integer = f_key.Integer();
		break;

	case MO_MODE_IP:
		if(!f_key.IPv4(address, port, mask)) {
			return 0;
		}
		break;

	}

	max = Count();
	for(idx = 0; idx < max; ++idx) {
		user = Get(idx);
		// is empty line or comment?
		if(user->IsComment()) {
			continue;
		}
		// column exists?
		if(static_cast<uint32_t>(f_column) >= user->Count()) {
			continue;
		}

		const moWCString& text = user->Get(f_column);
		switch(f_mode) {
		case MO_MODE_EQUAL:
			if(text == f_key) {
				f_last_found = idx;
				return user;
			}
			break;

		case MO_MODE_CIEQUAL:
			if(text.CaseCompare(f_key) == MO_BASE_COMPARE_EQUAL) {
				f_last_found = idx;
				return user;
			}
			break;

		case MO_MODE_REGEX:
			if(re.MatchExpression(text)) {
				f_last_found = idx;
				return user;
			}
			break;

		case MO_MODE_INTEGER:
			if(integer == text.Integer()) {
				f_last_found = idx;
				return user;
			}
			break;

		case MO_MODE_IP:
			if(text.IPv4(user_address, port, user_mask)) {
				if((address & user_mask) == (user_address & mask)) {
					f_last_found = idx;
					return user;
				}
			}
			break;

		}
	}

	f_last_found = -1;
	return 0;
}


moPasswd::moUserSPtr moPasswd::operator [] (const moWCString& key) const
{
	return Find(key);
}


moPasswd::moUserSPtr moPasswd::Get(int index) const
{
	return dynamic_cast<moUser *>(moList::Get(index));
}


moPasswd::moUserSPtr moPasswd::operator [] (int index) const
{
	return dynamic_cast<moUser *>(moList::Get(index));
}






/************************************************************ DOC:

CLASS

	moPasswd::moUser

NAME

	Constructor -- initializes the moUser object

SYNOPSIS

	moPasswd::moUser(const moWCString& separators = ":");

PARAMETERS

	separators - a list of separators

DESCRIPTION

	Prepare an moUser object.

	Please, see the moWords constructor for more information
	about the separators.

SEE ALSO

	SetWords()

*/
moPasswd::moUser::moUser(const moWCString& separators)
	: moWords(separators)
{
	// keep empty entries (quite important)
	EmptyWords(true);
	// accept strings with backslashed entries
	Quotes("\\\\\"\"''");
	// automatically clip strings (remove spaces at the start & end)
	AutoClip(true);
}




/************************************************************ DOC:

CLASS

	moPasswd::moUser

NAME

	SetWords -- defines the different words from a single string

SYNOPSIS

	virtual unsigned long SetWords(const moWCString& words);

PARAMETERS

	words - a list of words to cut into columns

DESCRIPTION

	Accepts a string with the different entries you expect for
	a user definition. The number of words and/or what they
	contain is not constrained in any way. Words can also be
	empty.

	By default, the ':' is used as the separator. This can be
	changed if you wish.

	The strings are defined as in:

		<word><sep><word><sep><word>...

	By default, the words are clipped meaning that spaces at
	the beginning and the end of the words are removed.

	Words can also be quoted with single (') or double (")
	quotes. In this case, they can include spaces and
	separators.

NOTES

	You should not use the AddWords() with an moUser
	object though it is permitted. Adding words doesn't
	change the comment flag status. Thus you can append
	a comment to another or add columns for a user
	description.

EXAMPLE

	The following are examples of what will be parsed:

		# My password file
		john:123:ZZPassword:?:"Smith, John"
		foo:566:PassyWordy:123.11.23.123:"Wow"

	In this case, the 1st line is taken as a comment
	and the following are taken as users with 5 columns.

SEE ALSO

	SetWords() of the moWords class

*/
unsigned long moPasswd::moUser::SetWords(const moWCString& words)
{
	const mowc::wc_t	*s;

	s = words.Data();
	while(*s != '\0' && mowc::isspace(*s)) {
		s++;
	}

	f_comment = *s == L'#' || *s == L'\0';

	// we always do a SetWords() since we want to save the
	// comments in case we do a Save()
	return moWords::SetWords(words);
}




/************************************************************ DOC:

CLASS

	moPasswd::moUser

NAME

	IsComment -- return true if this entry is a comment

SYNOPSIS

	bool IsComment(void) const;

DESCRIPTION

	A line in the input password file may be a comment.
	Comments are kept in moUser objects too so that way
	we can save it back in the password file is required.
	Yet, at times, a line is a comment. In this case, the
	SetWords() function will set the comment flag to true.

	Thought comments will also be cut in words, the words
	shouldn't be used. Instead you should use the AllWords()
	function to retrieve a comment.

EXAMPLE

	The following are examples of what will be parsed:

		# My password file
		john:123:ZZPassword:?:"Smith, John"
		foo:566:PassyWordy:123.11.23.123:"Wow"

	In this case, the 1st line is taken as a comment
	and the following are taken as users with 5 columns.

SEE ALSO

	SetWords() of the moWords class

*/
bool moPasswd::moUser::IsComment(void) const
{
	return f_comment;
}



// vim: ts=8
};		// namespace molib;

