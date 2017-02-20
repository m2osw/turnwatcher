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
#pragma implementation "mo/mo_words.h"
#endif

#include	"mo/mo_words.h"



namespace molib
{



/************************************************************ DOC:

CLASS

	moWords

NAME

	Constructor - create an moWords object
	Destructor - destroys an moWords object

SYNOPSIS

	moWords(void);
	moWords(const moWCString& char_separators);
	virtual ~moWords();

	private:
	void Init(void);

DESCRIPTION

	Creates a list of words. Use the SetWords() to reset the
	current list of words. Or AddWords() to increase the number
	of words present in this object.

	By default, words are not automatically clipped and empty
	words are discarded.

SEE ALSO

	NoNewLine

*/
void moWords::Init(void)
{
	f_auto_clip = false;
	f_empty_words = false;
}


moWords::moWords(void)
{
	Init();
}


moWords::moWords(const moWCString& char_separators)
{
	Init();
	f_char_separators = char_separators;
}


moWords::~moWords(void)
{
	ClearSeparators();
}



/************************************************************ DOC:

CLASS

	moWords

NAME

	AutoClip - whether words are automatically clipped

SYNOPSIS

	void AutoClip(bool status = true);

PARAMETERS

	status - new value of the auto clip flag

DESCRIPTION

	With this flag is set to 'true' then the resulting words are
	automatically clipped (starting and ending spaces are removed).

*/
void moWords::AutoClip(bool status)
{
	f_auto_clip = status;
}


/************************************************************ DOC:

CLASS

	moWords

NAME

	EmptyWords - whether empty words are accepted

SYNOPSIS

	void EmptyWords(bool status = true);

PARAMETERS

	status - new value of the empty words flag

DESCRIPTION

	When this flag is set to 'true', empty words are
	kept in the resulting list, otherwise (by default) they
	are discarded.

*/
void moWords::EmptyWords(bool status)
{
	f_empty_words = status;
}




/************************************************************ DOC:

CLASS

	moWords

NAME

	AllWords - returns a string with all the words

SYNOPSIS

	const moWCString& AllWords(void) const;

DESCRIPTION

	Return a string with all the words present in this list of
	words.

RETURN VALUE

	A string which can't be modified.

*/
const moWCString& moWords::AllWords(void) const
{
	return f_words;
}







/************************************************************ DOC:

CLASS

	moWords

NAME

	ClearSeparators - initialize a string object

SYNOPSIS

	void ClearSeparators(void);
	void CharSeparators(const moWCString& seps);
	void StringSeparators(const moWCString& separators);

PARAMETERS

	seps - a list of character separators
	separator - one string separator

DESCRIPTION

	These functions are used to handle the separators.

	Separators are of two categories:

	1. characters
	2. strings

	When no strings are defined, then only characters are used. Otherwise
	string separators are tested first. The first match is the one used
	(not the longest).

	When neither characters nor strings are defined, the internal default
	is used: spaces (any type).

	The characters and strings can include the following backslashes:

		\a		bell
		\e		escape
		\f		form feed
		\n		new line
		\r		carriage return
		\s		space (as isspace() defines)
		\t		horizontal tab
		\v		vertical tab
		\\		backslash

NOTE

	Using CharSeparator() with an empty strings clears out all stand
	alone characters.

	To use several string separators, call the StringSeparators()
	multiple times. Empty strings are ignored by StringSeparators().
	Also, this function can be used to test stand alone characters,
	yet it is much slower than the other one.

SEE ALSO

	SetWords, AddWords

*/
void moWords::ClearSeparators(void)
{
	// get rid of the character separators
	f_char_separators.Empty();

	// get rid of the string separators
	f_str_separators.Empty();
}


void moWords::CharSeparators(const moWCString& seps)
{
	// replace any previous separators with these once
	f_char_separators = seps;
}


void moWords::StringSeparators(const moWCString& separator)
{
	moWCString	*str;

	// ignore empty strings
	if(!separator.IsEmpty()) {
		str = new moWCString(separator);
		f_str_separators += *str;
	}
}



/************************************************************ DOC:

CLASS

	moWords

NAME

	Quotes - which quotes are accepted

SYNOPSIS

	void Quotes(const moWCString& quotes);

PARAMETERS

	status - set to true by default; new value of the auto clip flag

DESCRIPTION

	Setup the recognized set of quotes.

	The 'quotes' string is composed of a list of starting and ending
	quotes. There is no default. If you want to understand C strings
	then quotes would be: "\"\""; C characters: "''"; if you want
	to understand both: "\"\"''"; other strings with brackets or lower
	& larger than signs can be defined: "[]{}<>"; etc.

	If the C string includes "\\\\", then any quotes after that will
	also take in account escaped characters. Note that strings written
	between backslashes are therefore not available at this time.

ERRORS

	the quotes string must have an even size; if this isn't the case
	an error is thrown

*/
void moWords::Quotes(const moWCString& quotes)
{
	if(quotes.Length() % 2 == 1) {
		throw moError("the 'quotes' string must have an even size");
	}

	f_quotes = quotes;
}




/** \brief Add a new words.
 *
 * Replace the existing words with the newly specified words.
 *
 * \exception bad_alloc() is generated if a string cannot be allocated.
 *
 * \param[in] words    The new words to save in this moWords object.
 *
 * \return The number of words found in \p words.
 *
 * \sa Empty(), Quotes(), AutoClip(), CharSeparators(), StringSeparators()
 */
unsigned long moWords::SetWords(const moWCString& words)
{
	Empty();
	f_words.Empty();
	return AddWords(words);
}


/** \brief Add the specified words to this moWords object.
 *
 * This function adds words to the existing list of words.
 *
 * The new words are appened at the end of the existing
 * word list.
 *
 * The words are separated using the character and
 * string separators.
 *
 * \param[in] words   The list of words to append.
 *
 * \return The number of words in this moWords object.
 */
unsigned long moWords::AddWords(const moWCString& words)
{
	moWCString		str, *cmp;
	const mowc::wc_t	*s, *start, *seps;
	long			idx, max, skip;
	bool			found, accepts_backslash;

	if(!f_words.IsEmpty()) {
		if(!f_char_separators.IsEmpty()) {
			f_words += f_char_separators[0];
		}
		else if(f_str_separators.Count() != 0) {
			f_words += dynamic_cast<moWCString *>(f_str_separators.Get(0))[0];
		}
		else {
			// use a default separator (a space)
			f_words += " ";
		}
	}
	f_words += words;

	s = words.Data();
	while(*s != '\0') {
		// start of this new word
		start = s;
		found = false;
		do {
			skip = 1;

			// 1. test with strings
			max = f_str_separators.Count();
			idx = 0;
			while(idx < max) {
				cmp = dynamic_cast<moWCString *>(f_str_separators.Get(idx));
				if(cmp->Compare(s, 0, static_cast<int>(cmp->Length())) == MO_BASE_COMPARE_EQUAL) {
					// we found a separator
					found = true;
					skip = static_cast<long>(cmp->Length());
					break;
				}
				idx++;
			}

			// 2. test with characters (if not found yet)
			if(!found && !f_char_separators.IsEmpty()) {
				seps = f_char_separators.Data();
				while(*seps != '\0' && !found) {
					if(*seps == '\\') {
						seps++;
						if(*seps == 's') {
							found = mowc::isspace(*s);
							seps++;
						}
						else {
							found = *s == mowc::backslash_char(seps);
						}
					}
					else if(*seps == *s) {
						// we found a separator
						found = true;
						break;
					}
					else {
						seps++;
					}
				}
			}

			// 3. test with the default separators (spaces)
			if(!found && f_char_separators.IsEmpty() && f_str_separators.IsEmpty() && mowc::isspace(*s)) {
				// we found the default separator
				found = true;
			}

			// 4. check for quoted information
			if(!found) {
				accepts_backslash = false;
				seps = f_quotes.Data();
				while(*seps != '\0') {
					if(*seps == '\\') {
						accepts_backslash = true;
					}
					else if(*s == *seps) {
						// we found a quoted part - quoted parts can include separators
						s++;		// skip starting quote
						while(*s != seps[1] && *s != '\0') {
							if(accepts_backslash && *s == '\\' && s[1] != '\0') {
								s++;
							}
							s++;
						}
						if(*s == '\0') {
							s--;	// make sure the following s++ doesn't make the system crash
						}
						break;
					}
					seps += 2;		// the size of f_quotes is always even
				}
			}

			s++;
		} while(!found && *s != '\0');
		if(found) {
			s--;	// this character is part of the separator, not the word
		}
		str = moWCString(start, static_cast<int>(s - start));
		if(f_auto_clip) {
			str = str.Clip();
		}
		if(f_empty_words || !str.IsEmpty()) {
			// word accepted
			Insert(new moWCString(str));
		}
		// skip the word separator
		if(*s != '\0') {
			s += skip;
		}
	}

	return Count();
}


/************************************************************ DOC:

CLASS

	moWords

NAME

	Get -- get a word
	operator [] -- get a word

SYNOPSIS

	const moWCString& Get(int index) const;
	const moWCString& operator [] (int index) const;

PARAMETERS

	index - the word to be returned

DESCRIPTION

	The Get() function and the [] operator get the
	specified word and return a reference string to it.

	Note that the reference is a constant and it should
	be copied if it needs to be modified.

	This operator is an overload from the moList version
	of the same operator. Note that the operator is not
	virtual (it doesn't return the same data). It was
	created for practicallity since the moList returns
	an moBase reference and that's not as useful for a
	list of words.

BUGS

	Note that the operator [] of the moList class actually
	returns a list item which can be used for reading and
	writing of the item. Since the moWords implementation
	doesn't really allow writing, it can just return a
	constant reference at this time. This may change in
	the future.

SEE ALSO

	moList::operator [] ()

*/
const moWCString& moWords::Get(int index) const
{
	return dynamic_cast<moWCString&>(*moList::Get(index));
}


const moWCString& moWords::operator [] (int index) const
{
	return dynamic_cast<moWCString&>(*moList::Get(index));
}


};			// namespace molib;

// vim: ts=8

