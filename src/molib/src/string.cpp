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
#pragma implementation "mo/mo_string.h"
#endif

#include	"mo/mo_string.h"


namespace molib
{


/************************************************************ DOC:

CLASS

	moWCString

NAME

	g_empty_string -- the empty string

DESCRIPTION

	Use this string whenever you need an empty string. This way
	you avoid a call to the constructor and the destructor.


*/
const moWCString		g_empty_string;



/************************************************************ DOC:

CLASS

	moWCString

NAME

	Constructor - create an moWCString object
	Copy operator - copy an moWCString object
	Destructor - destroys an moWCString object

SYNOPSIS

	moWCString(void);
	moWCString(const char *str, int length = -1, encoding_t encoding);
	moWCString(const mc_t *str, int length = -1, encoding_t encoding);
	moWCString(const wch_t *str, int length = -1, encoding_t encoding);
	moWCString(const wchar_t *str, int length = -1);
	moWCString(const moWCString& string, int length = -1);
	virtual ~moWCString();

PARAMETERS

	str - a standard null terminated C string
	string - an moWCString object to copy
	length - the maximum number of characters to copy
	         or -1 for all
	encoding - the encoding of the input string

DESCRIPTION

	These constructors initialize a string object from the
	given parameters. The default operator creates an empty
	string.

	The copy operators create a new string with data equal to
	what was defined in the source string.

ERRORS

	These functions call the Init() function member which can
	generate a memory error long(MO_ERROR_MEMORY).

SEE ALSO

	mowc library

*/
const char *moWCString::moGetClassName(void) const
{
	return "molib::moBase::moWCString";
}


size_t moWCString::SizeOf(void) const
{
	if(f_string != f_data) {
		return (f_length + (size_t) 1) * sizeof(mowc::wc_t) + sizeof(moWCString);
	}

	// At this time moBase doesn't use mo_alloc() or anything of the sort...
	//return moBase::SizeOf() + sizeof(moWCString) - sizeof(moBase);
	return sizeof(moWCString);
}


moWCString::moWCString(void)
{
	Init();
}


moWCString::moWCString(const char *str, int length, mowc::encoding_t encoding)
{
	Init();
	Set(str, length, encoding);
}


moWCString::moWCString(const mowc::mc_t *str, int length, mowc::encoding_t encoding)
{
	Init();
	Set(str, length, encoding);
}


moWCString::moWCString(const mowc::wc_t *str, int length, mowc::encoding_t encoding)
{
	Init();
	Set(str, length, encoding);
}


moWCString::moWCString(const wchar_t *str, int length)
{
	Init();
	Set(str, length);
}


moWCString::moWCString(const moWCString& string, int length)
	: moBase(string)
{
	Init();
	f_password = string.f_password;
	Set(string.f_string, length);
}


moWCString::~moWCString(void)
{
	if(f_password) {
		// we want to destroy this, just in case...
		// that's more secure
		Clear();
	}
	if(f_string != f_data) {
		mo_free(f_string);
	}

	delete [] f_mb_string;
}




/************************************************************ DOC:

CLASS

	moWCString

NAME

	private:
	Init - initialize a string object

SYNOPSIS

	void Init(void);

DESCRIPTION

	This function is used internally to initialize a new string
	object. It will allocate a buffer to hold the string data.

ERRORS

	When the string allocation fails, a throw std::bad_alloc()
	is raised.

SEE ALSO

	Size, Set

*/
void moWCString::Init(void)
{
	//f_password = false; -- auto-init now
	//f_length = 0; -- auto-init now
	f_max = sizeof(f_data) / sizeof(mowc::wc_t);
	f_string = f_data;
	f_data[0] = '\0';
	f_mb_string = 0;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	IsEmpty -- returns true if a string is empty
	Empty -- makes a string empty
	Clear -- make the string empty and clear the memory

SYNOPSIS

	bool IsEmpty(void) const;
	moWCString& Empty(void);
	moWCString& Clear(void);

DESCRIPTION

	The IsEmpty() function returns true if the tested string
	is empty (has a Length() equal to zero or this pointer
	is NULL).

	Empty() empties a string.

	Clear() ensures the string is empty and cleared (all the
	characters are zeroes). This is similar to the Empty() of
	strings marked as passwords.

	It is much more efficient to use the same string over and over
	again instead of creating a new string each time you need one.
	To clear the string for reuse, use Empty() which is equivalent
	to the default constructor: moWCString(void);.

NOTES

	When either of the Empty() or Clear() functions are called,
	it also deletes the buffer used to save the string created
	when calling the SavedMBData() function. The string can't
	be used after this call.

RETURN VALUE

	IsEmpty returns a boolean value.

	Empty & Clear return a reference to this string.

SEE ALSO

	NoNewLine

BUGS

	The buffer allocated to hold the string stays allocated as is
	in memory. In other words, if one used a really large string
	(several kilobytes or even megabytes) then all the memory
	remains allocated.

*/
bool moWCString::IsEmpty(void) const
{
	return this == 0 || f_length == 0UL;
}


moWCString& moWCString::Empty(void)
{
	f_string_changed = true;

	// make the string empty, yet keep the same buffer
	if(f_password) {
		Clear();
	}
	else {
		*f_string = '\0';
	}
	f_length = 0;

	delete [] f_mb_string;
	f_mb_string = 0;

	return *this;
}


moWCString& moWCString::Clear(void)
{
	f_string_changed = true;
	memset(f_string, 0, f_max * sizeof(mowc::wc_t));
	f_length = 0;

	delete [] f_mb_string;
	f_mb_string = 0;

	return *this;
}

/************************************************************ DOC:

CLASS

	moWCString

NAME

	CurrentMaxLength - the current maximum length of the string
	Length - returns the current length in characters
	MBLength - returns the length in bytes to encode in UTF-8

SYNOPSIS

	size_t CurrentMaxLength(void) const;
	size_t Length(void) const;
	size_t MBLength(void) const;

DESCRIPTION

	The CurrentMaxLength() defines the number of characters which can be
	held by the string before its buffer will be enlarged. This can be
	used when a list of available strings is kept and a larger or smaller
	one is needed.

	The Length() function returns the length in characters of the string
	(NOT including the nul terminator).

	The MBLength() function returns the number of bytes necessary to
	convert the UCS-4 string in a UTF-8 string of characters (NOT
	including the nul terminator).

RETURN VALUE

	Length()	the number of characters in this string, not including the
			nul terminator.

	MBLength()	the number of bytes necessary to save the string in UTF-8,
			not including the nul terminator.

SEE ALSO

	IsEmpty, Empty

*/
size_t moWCString::CurrentMaxLength(void) const
{
	return f_max;
}


size_t moWCString::Length(void) const
{
	return f_length;
}


size_t moWCString::ByteSize(void) const
{
	return f_length * sizeof(mowc::wc_t);
}


size_t moWCString::MBLength(void) const
{
	return mowc::wcstombslen(f_string);
}





/************************************************************ DOC:

CLASS

	moWCString

NAME

	Password - mark the string as a string holding a password

SYNOPSIS

	void Password(void);

DESCRIPTION

	The Password() function marks the givens string as a password
	string. Such strings need to be cleared from memory before the
	corresponding buffer is returned to the system.

	Whenever possible, this flag is duplicated from one string to
	another. It is not possible to clear this flag once set.

SEE ALSO

	Empty, Destructor

*/
void moWCString::Password(void)
{
	f_password = true;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Data - returns a pointer on the string data
	MBData - returns a pointer to a newly allocate buffer with
		the string data converted to UTF-8
	SavedMBData - returns a pointer to a UTF-8 version of the string

	// STL like inline functions
	wc_str - like Data
	c_str - like SavedMBData

SYNOPSIS

	const wc_t *Data(void) const;
	char *MBData(char *string = 0, size_t size = 0) const;
	const char *SavedMBData(void) const;

	// STL like functions
	const wc_t *wc_str(void) const;
	const char *c_str(void) const;

PARAMETERS

	string - a user defined string buffer
	size - the size of the user string buffer

DESCRIPTION

	The Data() function returns a pointer to the internal buffer
	used to hold the null terminated C string. The buffer is
	always garantied to be null terminated.

	The pointer returned by this function will change when a
	non-constant function is called.

	The MBData() function returns a string encoded in UTF-8. The
	user can specify its own string, in which case the function
	doesn't allocate a string. Otherwise the function will
	determine the necessary length and convert this string
	from INTERNAL UCS-4 in a newly allocated buffer.

	It is the user responsability to delete the returned string
	when he didn't supply ones own buffer. One way to call this
	function is by using the stack as in the following example:

		unsigned long len = string.MBLength() + 1;
		char str[len];
		string.MBData(str, len);
		... // use str until you exit this block

	The SavedMBData() or c_str() function will convert the string
	to multibyte (UTF-8) and return a pointer to an internal
	buffer. This buffer will be valid until SavedMBData() is
	called again and the string has changed. The string returned
	by this function needs to be used right away and not saved
	in a variable.

RETURN VALUE

	Data()		the pointer of the internal string buffer

	MBData()	the user string pointer or a newly allocated
			buffer; use delete [] <str> once you are done
			with a newly created buffer

	SavedMBData()	an internally allocated string with the string
			converted to UTF-8; do NOT delete this string
			and don't use it after you modified the string

	wc_str()	same as Data()

	c_str()		same as SavedMBData()

SEE ALSO

	IsEmpty

ERRORS

	The function will throw std::bad_alloc() if a new string buffer
	cannot be allocated.

BUGS

	The buffer pointer will most probably change between calls.
	Do not store it and reuse it. Always get the pointer using
	this function as required.

	The buffer internally allocated by SavedMBData() is private
	to the moWCString and can't be deleted. Also, it can't safely
	be used after the string changed and SavedMBData() is called
	again.

*/
const mowc::wc_t *moWCString::Data(void) const
{
	return f_string;
}


char *moWCString::MBData(char *string, size_t size) const
{
	if(string == 0 || size == 0) {
		size = MBLength() + 1;
		string = new char[size];
	}
	mowc::wcstombs(string, f_string, static_cast<unsigned long>(size));
	return string;
}


const char *moWCString::SavedMBData(void) const
{
	size_t		size;

	if(f_mb_string == 0 || f_string_changed) {
		delete [] f_mb_string;
		f_mb_string = 0;
		size = MBLength() + 1;
		f_mb_string = new char[size];
		mowc::wcstombs(f_mb_string, f_string, static_cast<unsigned long>(size));
		f_string_changed = false;
	}

	return f_mb_string;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Get - get a character
	Get - get a string
	operator [] - get and/or set a character in an moWCString
	FirstChar - get the first character
	LastChar - get the last character

SYNOPSIS

	mowc::wc_t Get(int index) const;
	moWCString Get(int from, int to) const;

	moWCStringCharacter operator [] (int index);
	mowc::wc_t operator [] (int index) const;

	mowc::wc_t FirstChar(void) const;
	mowc::wc_t LastChar(void) const;

PARAMETERS

	index - position of the character to be overwritten or read
	from - the starting position (inclusive)
	to - the ending position (inclusive)

DESCRIPTION

	The array operator ([]) can be used to read and write characters
	in a string.

	The specified indexes must be between 0 (first character) and
	the length of the string minus one (Length() - 1), except for
	the Get(from, to) function where the parameters 'from' and 'to'
	are being clamped. Note that 'to' can be set to -1 to specify
	the end of the string.

RETURN VALUE

	Get()		return the character at specified index

	Get()		return a string composed of the characters from
			the specified character to the specified character
			(both included); when the parameter from is larger
			than the parameter to, then resulting string will
			also be reversed (see the Reverse() function);
			you can use really large values for the position
			in which case they are clamped to the maximum
			length; WARNING: negative values are viewed as
			really large values (i.e. -1 means the end of
			the string)

	operator []	the constant version returns the character value
			at the given character position

			the non constant version returns an moWCStringCharacter
			object which can subsequently be used to modify the
			moWCString (with the operator =)

	FirstChar()	returns the first character, equivalent to Get(0).

	LastChar()	returns the last character, equivalent to
			Get(Length() - 1) but usually more efficient.

ERRORS

	When the index is out of bounds an moError is thrown.

*/
moWCString::moWCStringCharacter moWCString::operator [] (int index)
{
	return moWCStringCharacter(this, index);
}


mowc::wc_t moWCString::operator [] (int index) const
{
	return Get(index);
}


mowc::wc_t moWCString::Get(int index) const
{
	if((size_t) index >= f_length) {
		throw moError("moWCString::Get(): index out of bounds (%d !E [0..%d)", index, (size_t) f_length);
	}

	return f_string[index];
}


moWCString moWCString::Get(int from, int to) const
{
	moWCString	result;

	// to test whether 'to' is larger than 'from' we need to
	// fix it in case the user sets it to -1
	if(to < 0) {
		to = f_length;
	}

	if(to >= from) {
		to++;

		// we use result because the Password wouldn't be set
		// otherwise
		if(f_password) {
			result.Password();
		}

		if((size_t) from >= f_length) {
			return result;
		}
		if((size_t) to > f_length) {
			to = f_length;
		}

		result = moWCString(f_string + from, to - from);
		return result;
	}

	/* in this case the string is inverted */
	return Get(to, from).Reverse();
}



mowc::wc_t moWCString::First(void) const
{
	return Get(0);
}


mowc::wc_t moWCString::Last(void) const
{
	return Get(f_length - (size_t) 1);
}







/************************************************************ DOC:

CLASS

	moWCString

NAME

	private:
	Size - ensure the string can hold data for the given size

SYNOPSIS

	void Size(int length);

DESCRIPTION

	This function is used internally to resize the string buffer.

NOTES

	The coherency of the string object is always kept. It simply
	won't be modified if an error occurs.

ERRORS

	When the string allocation fails, a throw std::bad_alloc()
	is raised.

SEE ALSO

	Init, Set

*/
void moWCString::Size(int length)
{
	long		l;
	mowc::wc_t	*s;

	f_string_changed = true;
	if((size_t) length >= f_max) {
		l = length + 256;

		// The f_string pointer can't be null or there
		// is an error somewhere (the Init() already
		// sets up a pointer)
		//if(f_string == 0) {
		//}

		if(f_password) {
			// reallocation isn't safe for passwords!
			s = static_cast<mowc::wc_t *>(mo_malloc(l * sizeof(mowc::wc_t), "moWCString: password string buffer"));
			memcpy(s, f_string, (f_length + (size_t) 1) * sizeof(mowc::wc_t));	/* Flawfinder: ignore */
			memset(f_string, 0, f_max * sizeof(mowc::wc_t));
			if(f_string != f_data) {
				mo_free(f_string);
			}
		}
		else {
			if(f_string == f_data) {
				s = static_cast<mowc::wc_t *>(mo_malloc(l * sizeof(mowc::wc_t), "moWCString: large string buffer"));
				memcpy(s, f_string, (f_length + (size_t) 1) * sizeof(mowc::wc_t));	/* Flawfinder: ignore */
			}
			else {
				s = static_cast<mowc::wc_t *>(mo_realloc(f_string, l * sizeof(mowc::wc_t), "moWCString: enlarge string buffer"));
			}
		}
		f_string = s;
		f_max = l;
	}
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Set - copy a constant C string in the string object

SYNOPSIS

	void Set(const char *str, int length = -1, encoding_t encoding = MO_ENCODING_UTF8);
	void Set(const mc_t *str, int length = -1, encoding_t encoding = MO_ENCODING_UCS2_INTERNAL);
	void Set(const wc_t *str, int length = -1, encoding_t encoding = MO_ENCODING_UCS4_INTERNAL);
	void Set(const wchar_t *str, int length = -1);
	moWCString& operator = (const char *str);
	moWCString& operator = (const mc_t *str);
	moWCString& operator = (const wc_t *str);
	moWCString& operator = (const wchar_t *str);
	moWCString& operator = (const moWCString& string);

DESCRIPTION

	These functions can be used to set the string buffer equal
	to a C null terminated string or another moWCString.

	When length is set to -1, the whole source string is copied.
	Otherwise, a maximum of length characters are copied.

ERRORS

	When the string allocation fails, a throw std::bad_alloc()
	is raise.

SEE ALSO

	Init, Size

*/
void moWCString::Set(const char *str, int length, mowc::encoding_t encoding)
{
	int		len;

	len = mowc::strlen(str, encoding);		/* Flawfinder: ignore */
	if(len < length || length == -1) {
		length = len;
	}
	Size(length);
	mowc::strcpy(f_string, str, length, encoding);	/* Flawfinder: ignore */
	f_length = length;
}



void moWCString::Set(const mowc::mc_t *str, int length, mowc::encoding_t encoding)
{
	int		len;

	len = mowc::strlen(str);			/* Flawfinder: ignore */
	if(len < length || length == -1) {
		length = len;
	}
	Size(length);
	mowc::strcpy(f_string, str, length, encoding);	/* Flawfinder: ignore */
	f_length = length;
}



void moWCString::Set(const mowc::wc_t *str, int length, mowc::encoding_t encoding)
{
	int		len;

	len = mowc::strlen(str);			/* Flawfinder: ignore */
	if(len < length || length == -1) {
		length = len;
	}
	Size(length);
	mowc::strcpy(f_string, str, length, encoding);	/* Flawfinder: ignore */
	f_length = length;
}



void moWCString::Set(const wchar_t *str, int length)
{
// under MS-Windows the L"..." are 16 bits instead of 32
#if WCHAR_MAX == 65535
	Set(reinterpret_cast<const mowc::mc_t *>(str), length);
#else
	Set(reinterpret_cast<const mowc::wc_t *>(str), length);
#endif
}



void moWCString::Set(int index, mowc::wc_t c, mowc::encoding_t encoding)
{
	if((size_t) index >= f_length) {
		throw moError("moWCString::Set(): index out of bounds (%d !E [0..%d)", index, (size_t) f_length);
	}

	f_string_changed = true;
	f_string[index] = c;
}



void moWCString::Set(int index, char c, mowc::encoding_t encoding)
{
	Set(index, static_cast<mowc::wc_t>(c), encoding);
}


void moWCString::Set(int index, mowc::mc_t c, mowc::encoding_t encoding)
{
	Set(index, static_cast<mowc::wc_t>(c), encoding);
}



void moWCString::Set(int index, wchar_t c)
{
#if WCHAR_MAX == 65535
	Set(index, static_cast<mowc::mc_t>(c));
#else
	Set(index, static_cast<mowc::wc_t>(c));
#endif
}



moWCString& moWCString::operator = (const moWCString& string)
{
	f_password = string.f_password;
	Set(string.f_string, -1);

	return *this;
}


moWCString& moWCString::operator = (const char *str)
{
	Set(str, -1);

	return *this;
}


moWCString& moWCString::operator = (const mowc::mc_t *str)
{
	Set(str, -1);

	return *this;
}


moWCString& moWCString::operator = (const mowc::wc_t *str)
{
	Set(str, -1);

	return *this;
}


moWCString& moWCString::operator = (const wchar_t *str)
{
	Set(str, -1);

	return *this;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Compare - compare two strings together

SYNOPSIS

	compare_t Compare(const moBase& object) const;
	compare_t Compare(const moWCString& string, unsigned int pos = 0, int length = -1) const;
	compare_t Compare(const char *str, unsigned int pos = 0, int length = -1) const;
	compare_t CaseCompare(const char *str, unsigned int pos = 0, int length = -1) const;

	bool operator == (const moBase& object) const;
	bool operator != (const moBase& object) const;
	bool operator <  (const moBase& object) const;
	bool operator <= (const moBase& object) const;
	bool operator >  (const moBase& object) const;
	bool operator >= (const moBase& object) const;

	bool operator == (const char *str) const;
	bool operator != (const char *str) const;
	bool operator <  (const char *str) const;
	bool operator <= (const char *str) const;
	bool operator >  (const char *str) const;
	bool operator >= (const char *str) const;

PARAMETERS

	object - another string or inherited string object
	string - a string object
	str - a null terminated C string
	pos - the position at which the comparison starts in this string
	length - number of characters compared

DESCRIPTION

	These functions compare two strings.

	The comparison of the Compare() function is case sensitive. The
	comparison of the CaseCompare() function is not.

SEE ALSO


BUGS

	At this time, even when a length is given a null terminated
	string must be specified since strlen() is used on it.

*/
moBase::compare_t moWCString::Compare(const char *str, unsigned int pos, int length, mowc::encoding_t encoding) const
{
	return Compare(moWCString(str, length, encoding), pos, length);
}


moBase::compare_t moWCString::Compare(const mowc::mc_t *str, unsigned int pos, int length, mowc::encoding_t encoding) const
{
	return Compare(moWCString(str, length, encoding), pos, length);
}


moBase::compare_t moWCString::Compare(const mowc::wc_t *str, unsigned int pos, int length, mowc::encoding_t encoding) const
{
	return Compare(moWCString(str, length, encoding), pos, length);
}


moBase::compare_t moWCString::Compare(const wchar_t *str, unsigned int pos, int length) const
{
	return Compare(moWCString(str, length), pos, length);
}


moBase::compare_t moWCString::CaseCompare(const char *str, unsigned int pos, int length, mowc::encoding_t encoding) const
{
	return CaseCompare(moWCString(str, length, encoding), pos, length);
}


moBase::compare_t moWCString::CaseCompare(const mowc::mc_t *str, unsigned int pos, int length, mowc::encoding_t encoding) const
{
	return CaseCompare(moWCString(str, length, encoding), pos, length);
}


moBase::compare_t moWCString::CaseCompare(const mowc::wc_t *str, unsigned int pos, int length, mowc::encoding_t encoding) const
{
	return CaseCompare(moWCString(str, length, encoding), pos, length);
}


moBase::compare_t moWCString::CaseCompare(const wchar_t *str, unsigned int pos, int length) const
{
	return CaseCompare(moWCString(str, length), pos, length);
}


moBase::compare_t moWCString::Compare(const moBase& object) const
{
	return Compare(dynamic_cast<const moWCString&>(object));
}


moBase::compare_t moWCString::Compare(const moWCString& string, unsigned int pos, int length) const
{
	int			r;
	const mowc::wc_t	*s, *str;

	// special case we have better to handle here
	if(length == 0) {
		return MO_BASE_COMPARE_EQUAL;
	}

	// special case of an empty string which is an empty pointer
	if(string.f_length == (size_t) 0) {
		return f_length <= pos ? MO_BASE_COMPARE_EQUAL : MO_BASE_COMPARE_GREATER;
	}

	// special case of too large a position
	if(pos > f_length) {
		return string.f_length == (size_t) 0 ? MO_BASE_COMPARE_EQUAL : MO_BASE_COMPARE_SMALLER;
	}

	s = f_string + pos;
	str = string.f_string;
	while(length != 0 && *str != L'\0' && *s != L'\0') {
		r = *str - *s;
		if(r != 0) {
			return r > 0 ? MO_BASE_COMPARE_SMALLER : MO_BASE_COMPARE_GREATER;
		}
		length--;	// when -1 was used it will never reach 0
		str++;
		s++;
	}

	if(length == 0 || (*str == L'\0' && *s == L'\0')) {
		return MO_BASE_COMPARE_EQUAL;
	}

	return *str == L'\0' ? MO_BASE_COMPARE_GREATER : MO_BASE_COMPARE_SMALLER;
}


moBase::compare_t moWCString::CaseCompare(const moWCString& string, unsigned int pos, int length) const
{
	int			r;
	mowc::wc_t		a, b;
	const mowc::wc_t	*s, *str;

	// special case we have better to handle here
	if(length == 0) {
		return MO_BASE_COMPARE_EQUAL;
	}

	// special case of an empty string which is an empty pointer
	if(string.f_length == (size_t) 0) {
		return (long) (f_length - (size_t) pos) <= 0 ? MO_BASE_COMPARE_EQUAL : MO_BASE_COMPARE_GREATER;
	}

	// special case of a position too large
	if(pos > f_length) {
		return string.f_length == 0 ? MO_BASE_COMPARE_EQUAL : MO_BASE_COMPARE_SMALLER;
	}

	s = f_string + pos;
	str = string.f_string;
	while(length != 0 && *str != L'\0' && *s != L'\0') {
		a = mowc::toupper(*str);
		b = mowc::toupper(*s);
		r = a - b;
		if(r != 0) {
			return r > 0 ? MO_BASE_COMPARE_SMALLER : MO_BASE_COMPARE_GREATER;
		}
		length--;	// when -1 was used it will never reach 0
		str++;
		s++;
	}

	if(length == 0 || (*str == L'\0' && *s == L'\0')) {
		return MO_BASE_COMPARE_EQUAL;
	}

	return *str == L'\0' ? MO_BASE_COMPARE_GREATER : MO_BASE_COMPARE_SMALLER;
}



bool moWCString::operator == (const moBase& object) const
{
	return moBase::operator == (object);
}


bool moWCString::operator != (const moBase& object) const
{
	return moBase::operator != (object);
}


bool moWCString::operator < (const moBase& object) const
{
	return moBase::operator < (object);
}


bool moWCString::operator <= (const moBase& object) const
{
	return moBase::operator <= (object);
}


bool moWCString::operator > (const moBase& object) const
{
	return moBase::operator > (object);
}


bool moWCString::operator >= (const moBase& object) const
{
	return moBase::operator >= (object);
}


bool moWCString::operator == (const char *str) const
{
	return *this == moWCString(str);
}


bool moWCString::operator != (const char *str) const
{
	return *this != moWCString(str);
}


bool moWCString::operator < (const char *str) const
{
	return *this < moWCString(str);
}


bool moWCString::operator <= (const char *str) const
{
	return *this <= moWCString(str);
}


bool moWCString::operator > (const char *str) const
{
	return *this > moWCString(str);
}


bool moWCString::operator >= (const char *str) const
{
	return *this >= moWCString(str);
}


bool moWCString::operator == (const mowc::mc_t *str) const
{
	return *this == moWCString(str);
}


bool moWCString::operator != (const mowc::mc_t *str) const
{
	return *this != moWCString(str);
}


bool moWCString::operator < (const mowc::mc_t *str) const
{
	return *this < moWCString(str);
}


bool moWCString::operator <= (const mowc::mc_t *str) const
{
	return *this <= moWCString(str);
}


bool moWCString::operator > (const mowc::mc_t *str) const
{
	return *this > moWCString(str);
}


bool moWCString::operator >= (const mowc::mc_t *str) const
{
	return *this >= moWCString(str);
}


bool moWCString::operator == (const mowc::wc_t *str) const
{
	return *this == moWCString(str);
}


bool moWCString::operator != (const mowc::wc_t *str) const
{
	return *this != moWCString(str);
}


bool moWCString::operator < (const mowc::wc_t *str) const
{
	return *this < moWCString(str);
}


bool moWCString::operator <= (const mowc::wc_t *str) const
{
	return *this <= moWCString(str);
}


bool moWCString::operator > (const mowc::wc_t *str) const
{
	return *this > moWCString(str);
}


bool moWCString::operator >= (const mowc::wc_t *str) const
{
	return *this >= moWCString(str);
}


bool moWCString::operator == (const wchar_t *str) const
{
	return *this == moWCString(str);
}


bool moWCString::operator != (const wchar_t *str) const
{
	return *this != moWCString(str);
}


bool moWCString::operator < (const wchar_t *str) const
{
	return *this < moWCString(str);
}


bool moWCString::operator <= (const wchar_t *str) const
{
	return *this <= moWCString(str);
}


bool moWCString::operator > (const wchar_t *str) const
{
	return *this > moWCString(str);
}


bool moWCString::operator >= (const wchar_t *str) const
{
	return *this >= moWCString(str);
}






/************************************************************ DOC:

CLASS

	moWCString

NAME

	SoundEx - transform a string to it's SoundEx (phonetic representation)

SYNOPSIS

	moWCString SoundEx(bool full = false) const;

PARAMETERS

	full - whether the entire word is converted (true)
		or the first letter is kepts as is (false)

DESCRIPTION

	This function computes the SoundEx of the words in a string.

	The resulting string can then be compared with another to
	know whether two strings have the same English phonetic.

	The result only returns lowercase characters.

	Note that only the 26 non-accentuated latin characters are
	taken in account at this time.

NOTES

	Source: http://physics.nist.gov/cuu/Reference/soundex.html
	Author: Kevin Setter, 8/27/97

	SoundEx Coding Guide

	1 = B,P,F,V
	2 = C,S,G,J,K,Q,X,Z
	3 = D,T
	4 = L
	5 = M,N
	6 = R

	The letters A,E,I,O,U,Y,H, and W are not coded.

SEE ALSO

	Compare, CaseCompare

BUGS

	This algorithm should be changed to support Unicode characters.

*/
moWCString moWCString::SoundEx(bool full) const
{
	char trans[26] = {	/* Flawfinder: ignore */
		/* A */	'*',
		/* B */	'1',
		/* C */	'2',
		/* D */	'3',
		/* E */	'*',
		/* F */	'1',
		/* G */	'2',
		/* H */	'*',
		/* I */	'*',
		/* J */	'2',
		/* K */	'2',
		/* L */	'4',
		/* M */	'5',
		/* N */	'5',
		/* O */	'*',
		/* P */	'1',
		/* Q */	'2',
		/* R */	'6',
		/* S */	'2',
		/* T */	'3',
		/* U */	'*',
		/* V */	'1',
		/* W */	'*',
		/* X */	'2',
		/* Y */	'*',
		/* Z */	'2'
	};
	char			c, p;
	const mowc::wc_t	*s;
	moWCString		result;
	int			cnt;

	p = '*';
	cnt = 0;
	s = f_string;
	while((c = *s) != '\0') {
		if((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')) {
			if(!full && p == '*') {
				if(!result.IsEmpty()) {
					result += ' ';
				}
				// always lower case
				result += c | 0x20;
				cnt++;
			}
			else {
				c = trans[(c & 0x5F) - 'A'];
				if(c != p && c != '*') {
					if(p == '*') {
						if(!result.IsEmpty()) {
							result += ' ';
						}
					}
					p = c;
					result += c;
					cnt++;
				}
			}
		}
		else if(c != '-') {	// are dashes not viewed as full separators?
			if(cnt > 0) {
				// create words of 4 characters minimum
				while(cnt < 4) {
					result += '0';
					cnt++;
				}
				cnt = 0;
			}
			p = '*';
		}
		s++;
	}

	return result;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	NoNewLine - return a string without an ending new line character

SYNOPSIS

	moWCString NoNewLine(void) const;

DESCRIPTION

	This function copies this string and return the copy without
	a new line as the last character.

SEE ALSO

	Clip, Strip, Replace

*/
moWCString moWCString::NoNewLine(void) const
{
	moWCString	str(*this);

	if(str.f_length > (size_t) 0) {
		if(str.f_string[str.f_length - (size_t) 1] == '\n') {
			str.f_length--;
			str.f_string[str.f_length] = '\0';
		}
	}

	return str;
}






/************************************************************ DOC:

CLASS

	moWCString

NAME

	ConvertFromEncoding - convert this string from given encoding

SYNOPSIS

	int ConvertFromEncoding(const moWCString& encoding);

DESCRIPTION

	The ConvertFromEncoding() function assumes that the current
	moWCString was created as a stream of bytes (i.e. using
	mowc::MO_ENCODING_ISO8859_1 as the encoding of the string--
	i.e. the string will be untouched by the constructor).

	Then, once the string is fully defined, you call the convertion
	function to transform the stream of bytes to the corresponding
	Unicode encoding.

	Internally, the string uses an moIConv object to do the
	necessary conversions.

BUGS

	This function is destructive and thus it can be applied only
	once to a string, until you empty the string and put a new
	stream of bytes to convert.

	The string needs to be defined with a stream of bytes which
	don't represent Unicode characters. This is not very clean,
	but on the other hand, it gives you the flexibility of using
	an moWCString object to create the input buffer for the
	convertion function.

RETURN VALUE

	This function returns the new length of the string (0 or more)
	or -1 when an error occurs.

	A usual error is to specify the name of an unknown encoding.
	See the iconv utility to know what encodings are supported
	on your system (or type 'iconv -l' in your console).

SEE ALSO

	Constructor, mowc::moIConv object

*/
int moWCString::ConvertFromEncoding(const moWCString& encoding)
{
// initialize the convertor (destination is UTF-32 so we use "*" to avoid the iconv() calls)
	mowc::moIConv convertor;
	if(convertor.SetEncodings(encoding.c_str(), "*") < 0) {
		return -1;
	}

// convert the input to a stream of bytes (without testing the chars... ouch!)
	int l = f_length;
	std::auto_ptr<char> in(new char[l]);
	char *input = in.get();
	for(int idx = 0; idx < l; ++idx) {
		input[idx] = f_string[idx];
	}

	// if it fails now, we'll have lost it!
	Empty();

	std::auto_ptr<mowc::wc_t> out(new mowc::wc_t[l + 1]);
	mowc::wc_t *output = out.get();
	int sz, inp;
	for(inp = 0; inp < l; inp += sz) {
		// write the input data
		sz = convertor.Write(input + inp, l - inp);
		if(sz <= 0) {
			// FIFO couldn't take the input data?!
			return -1;
		}

		// NOTE: I don't think this is necessary...
		//convertor.Flush();

		// now read the result

		for(;;) {
			int len = convertor.Read(output, l * sizeof(mowc::wc_t));
			if(len < 0) {
				return -1;
			}
			if(len == 0) {
				break;
			}
			len /= sizeof(mowc::wc_t);

			// null terminate and then append to ourself
			output[len] = '\0';
			Append(output, len);
		}
	}

// if no error, return the new length
	return f_length;
}






/************************************************************ DOC:

CLASS

	moWCString

NAME

	Clip - return a string without leading and ending spaces
	Strip - apply the Clip() function on this string

SYNOPSIS

	moWCString Clip(unsigned long sides = WC_STRING_CLIP_BOTH) const;
	void Strip(unsigned long sides = WC_STRING_CLIP_BOTH);

PARAMETERS

	sides -- which of the start and end to strip

DESCRIPTION

	The Clip() function copies this string and return a copy without
	any spaces at the beginning and at the end.

	The Strip() function doesn't copy this string and remove the
	spaces at the beginning and at the end.

	The sides parameter can be used to specify which sides you want
	to clip:

		WC_STRING_CLIP_START	only strip the start
		WC_STRING_CLIP_END	only strip the end
		WC_STRING_CLIP_NEWLINE	also remove '\r' and '\n' characters
		WC_STRING_CLIP_BOTH	strip both sides

RETURN VALUE

	Clip() returns a copy of the input string clipped

SEE ALSO

	NoNewLine

*/
moWCString moWCString::Clip(unsigned long sides) const
{
	const mowc::wc_t	*s, *e;

	s = f_string;
	if((sides & WC_STRING_CLIP_START) != 0) {
		if((sides & WC_STRING_CLIP_NEWLINE) != 0) {
			while(mowc::isspace(*s) || *s == '\r' || *s == '\n') {
				s++;
			}
		}
		else {
			while(mowc::isspace(*s)) {
				s++;
			}
		}
	}
	e = f_string + f_length;
	if((sides & WC_STRING_CLIP_END) != 0) {
		if((sides & WC_STRING_CLIP_NEWLINE) != 0) {
			while(e > s && mowc::isspace(e[-1]) || e[-1] == '\r' || e[-1] == '\n') {
				e--;
			}
		}
		else {
			while(e > s && mowc::isspace(e[-1])) {
				e--;
			}
		}
	}

	return moWCString(s, static_cast<int>(e - s));
}



void moWCString::Strip(unsigned long sides)
{
	const mowc::wc_t	*s, *e;

	s = f_string;
	if((sides & WC_STRING_CLIP_START) != 0) {
		if((sides & WC_STRING_CLIP_NEWLINE) != 0) {
			while(mowc::isspace(*s) || *s == '\r' || *s == '\n') {
				s++;
			}
		}
		else {
			while(mowc::isspace(*s)) {
				s++;
			}
		}
	}
	e = f_string + f_length;
	if((sides & WC_STRING_CLIP_END) != 0) {
		if((sides & WC_STRING_CLIP_NEWLINE) != 0) {
			while(e > s && mowc::isspace(e[-1]) || e[-1] == '\r' || e[-1] == '\n') {
				e--;
			}
		}
		else {
			while(e > s && mowc::isspace(e[-1])) {
				e--;
			}
		}
	}

	if(static_cast<size_t>(f_length) != static_cast<size_t>(e - s)) {
		f_string_changed = true;
		f_length = static_cast<int32_t>(e - s);
		memmove(f_string, s, f_length * sizeof(mowc::wc_t));
		f_string[f_length] = '\0';
	}
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Delete - remove one or more characters from a string

SYNOPSIS

	moWCString Delete(unsigned int from, unsigned int to) const;

PARAMETERS

	from - delete from this character
	to - delete up to this character

DESCRIPTION

	This function creates a new string which is the source string
	with the characters 'from' to 'to' removed.

	If to is small than from, the function automatically swaps
	from and to before applying the operation.

	If you want to apply this function to a string, then use
	the following syntax:

		my_str = my_str.Delete(from, to);

SEE ALSO

	Insert, Append, Concat

*/
moWCString moWCString::Delete(unsigned int from, unsigned int to) const
{
	moWCString	result;
	unsigned int	swap;

	if(from > to) {
		swap = from;
		from = to;
		to = swap;
	}

	if(from > 0) {
		result.Set(f_string, from);
	}

	to++;
	if(to < f_length) {
		result += f_string + to;
	}

	if(f_password) {
		result.Password();
	}

	return result;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Concat - concatenate two strings and return the result

SYNOPSIS

	moWCString Concat(const char *str, int length = -1) const;
	moWCString Concat(const moWCString& str, unsigned int pos = 0, int length = -1) const;

PARAMETERS

	str - a C null terminated string
	string - an moWCString object
	pos - copy from this position in the source
	length - copy at most this many characters

DESCRIPTION

	This function creates a new string which is the concatenation
	of this string and the string parameter.

	When length is -1, the whole source string is copied.

SEE ALSO

	Insert

*/
moWCString moWCString::Concat(const char *str, int length, mowc::encoding_t encoding) const
{
	return Concat(moWCString(str, length, encoding));
}


moWCString moWCString::Concat(const mowc::mc_t *str, int length, mowc::encoding_t encoding) const
{
	return Concat(moWCString(str, length, encoding));
}


moWCString moWCString::Concat(const mowc::wc_t *str, int length, mowc::encoding_t encoding) const
{
	return Concat(moWCString(str, length, encoding));
}


moWCString moWCString::Concat(const wchar_t *str, int length) const
{
	return Concat(moWCString(str, length));
}


moWCString moWCString::Concat(const moWCString& string, unsigned int pos, int length) const
{
	unsigned int	l;

	l = string.f_length;

// anything to do?
	if(pos >= l || length == 0) {
		return *this;
	}
	l -= pos;
	if(l > (unsigned int) length) {
		l = length;
	}

	moWCString	result;

	if(f_password || string.f_password) {
		result.Password();
	}

	result.Size(f_length + l);

	/* do the concatenation */
	memcpy(result.f_string, f_string, f_length * sizeof(mowc::wc_t));				/* Flawfinder: ignore */
	memcpy(result.f_string + f_length, string.f_string + pos, (l + 1) * sizeof(mowc::wc_t));	/* Flawfinder: ignore */

	result.f_length = f_length + l;

	return result;
}





/************************************************************ DOC:

CLASS

	moWCString

NAME

	Append - append a string at the end of this string

SYNOPSIS

	moWCString& Append(const char *str, int length = -1, encoding_t encoding = MO_ENCODING_UTF8) const;
	moWCString& Append(const wchar_t *str, int length = -1) const;
	moWCString& Append(const moWCString& str, unsigned int pos = 0, int length = -1) const;

	moWCString& operator += (const moWCString& string);
	moWCString& operator += (const char *str);
	moWCString& operator += (const mowc::mc_t *str);
	moWCString& operator += (const mowc::wc_t *str);
	moWCString& operator += (const wchar_t *str);
	moWCString& operator += (char c);
	moWCString& operator += (mowc::mc_t c);
	moWCString& operator += (mowc::wc_t c);
	moWCString& operator += (wchar_t c);

	moWCString operator + (const moWCString& string) const;
	moWCString operator + (const char *str) const;
	moWCString operator + (const mowc::mc_t *str) const;
	moWCString operator + (const mowc::wc_t *str) const;
	moWCString operator + (const wchar_t *str) const;
	friend moWCString operator + (const char *str, const moWCString& string);
	friend moWCString operator + (const wchar_t *str, const moWCString& string);
	moWCString operator + (char c) const;
	moWCString operator + (mowc::mc_t c) const;
	moWCString operator + (mowc::wc_t c) const;
	moWCString operator + (wchar_t c) const;

PARAMETERS

	str - a C null terminated string
	string - an moWCString object
	pos - copy from this position in the source
	length - copy at most this many characters

DESCRIPTION

	This function creates a new string which is the concatenation
	of this string and the string parameter.

	When length is -1, the whole source string is copied.

	The += operators automatically copy the whole parameter string.

SEE ALSO

	Insert

*/
moWCString& moWCString::Append(const char *str, int length, mowc::encoding_t encoding)
{
	return Append(moWCString(str, length, encoding));
}


moWCString& moWCString::Append(const mowc::mc_t *str, int length, mowc::encoding_t encoding)
{
	return Append(moWCString(str, length, encoding));
}


moWCString& moWCString::Append(const mowc::wc_t *str, int length, mowc::encoding_t encoding)
{
	return Append(moWCString(str, length, encoding));
}


moWCString& moWCString::Append(const wchar_t *str, int length)
{
	return Append(moWCString(str, length));
}


moWCString& moWCString::Append(const moWCString& string, unsigned int pos, int length)
{
	unsigned int	l;

	if(string.f_password) {
		Password();
	}

	l = string.f_length;

	// anything to append?
	if(pos >= l || length == 0) {
		return *this;
	}

	// don't copy more than what we have
	l -= pos;
	if(l > (unsigned int) length) {
		l = length;
	}

	Size(f_length + l);

	mowc::strcpy(f_string + f_length, string.f_string + pos, l);	/* Flawfinder: ignore */

	f_length += l;

	return *this;
}


moWCString& moWCString::operator += (const moWCString& string)
{
	return Append(string);
}


moWCString& moWCString::operator += (const char *str)
{
	return Append(str);
}


moWCString& moWCString::operator += (const mowc::mc_t *str)
{
	return Append(str);
}


moWCString& moWCString::operator += (const mowc::wc_t *str)
{
	return Append(str);
}


moWCString& moWCString::operator += (const wchar_t *str)
{
	return Append(str);
}


moWCString& moWCString::operator += (char c)
{
	char		str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	return Append(str);
}


moWCString& moWCString::operator += (mowc::mc_t c)
{
	mowc::mc_t	str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	return Append(str);
}


moWCString& moWCString::operator += (mowc::wc_t c)
{
	mowc::wc_t	str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	return Append(str);
}


moWCString& moWCString::operator += (wchar_t c)
{
	wchar_t		str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	return Append(str);
}


moWCString moWCString::operator + (const moWCString& string) const
{
	moWCString s(*this);
	return s.Append(string);
}


moWCString moWCString::operator + (const char *str) const
{
	moWCString s(*this);
	return s.Append(str);
}


moWCString moWCString::operator + (const mowc::mc_t *str) const
{
	moWCString s(*this);
	return s.Append(str);
}


moWCString moWCString::operator + (const mowc::wc_t *str) const
{
	moWCString s(*this);
	return s.Append(str);
}


moWCString moWCString::operator + (const wchar_t *str) const
{
	moWCString s(*this);
	return s.Append(str);
}


#if 0
// With gcc 3.2 this doesn't work!
moWCString moWCString::operator + (const char *str, const moWCString& string)
{
	moWCString s(str);
	return s.Append(string);
}


moWCString moWCString::operator + (const wchar_t *str, const moWCString& string)
{
	moWCString s(str);
	return s.Append(string);
}
#endif


moWCString moWCString::operator + (char c) const
{
	char		str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	moWCString s(*this);
	return s.Append(str);
}


moWCString moWCString::operator + (mowc::mc_t c) const
{
	mowc::mc_t	str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	moWCString s(*this);
	return s.Append(str);
}


moWCString moWCString::operator + (mowc::wc_t c) const
{
	mowc::wc_t	str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	moWCString s(*this);
	return s.Append(str);
}


moWCString moWCString::operator + (wchar_t c) const
{
	wchar_t		str[2];		/* Flawfinder: ignore */

	str[0] = c;
	str[1] = 0;

	moWCString s(*this);
	return s.Append(str);
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Insert - insert a string within this string

SYNOPSIS

	moWCString Insert(const char *str, unsigned int where = (unsigned int) -1, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8) const;
	moWCString Insert(const mowc::mc_t *str, unsigned int where = (unsigned int) -1, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL) const;
	moWCString Insert(const mowc::wc_t *str, unsigned int where = (unsigned int) -1, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL) const;
	moWCString Insert(const wchar_t *str, unsigned int where = (unsigned int) -1, int length = -1) const;
	moWCString Insert(const moWCString& string, unsigned int where = (unsigned int) -1, unsigned int pos = 0, int length = -1) const;

PARAMETERS

	str - a C null terminated string
	string - an moWCString object
	where - the position at which the string is inserted
	pos - copy from this position in the source
	length - copy at most this many characters

DESCRIPTION

	This function creates a new string which is this string with
	the string parameter inserted at the specified position (where).
	If 'where' is set to the size of this string (or any larger
	size) then the Insert() has the same effect as the Concat()
	function.

	At most 'length' characters are inserted, unless 'length' is set to
	-1 in which case the whole parameter string is copied.

	When an moWCString object is inserted in another, the position
	at which the copy starts in the source can be specified with the
	'pos' parameter (with other strings, you can yourself add the
	position to your string).

SEE ALSO

	Insert

*/
moWCString moWCString::Insert(const char *str, unsigned int where, int length, mowc::encoding_t encoding) const
{
	return Insert(moWCString(str, length, encoding), where);
}


moWCString moWCString::Insert(const mowc::mc_t *str, unsigned int where, int length, mowc::encoding_t encoding) const
{
	return Insert(moWCString(str, length, encoding), where);
}


moWCString moWCString::Insert(const mowc::wc_t *str, unsigned int where, int length, mowc::encoding_t encoding) const
{
	return Insert(moWCString(str, length, encoding), where);
}


moWCString moWCString::Insert(const wchar_t *str, unsigned int where, int length) const
{
	return Insert(moWCString(str, length), where);
}


moWCString moWCString::Insert(const moWCString& string, unsigned int where, unsigned int pos, int length) const
{
	unsigned int	l;

	l = string.f_length;

// anything to insert?
	if(pos >= l || length == 0) {
		return *this;
	}

	if(where > f_length) {
		where = f_length;
	}

	if(l > (unsigned int) length) {
		l = length;
	}

	moWCString	result;

	result.Size(f_length + l);
	if(f_password) {
		result.Password();
	}

	/* do the insertion */
	memcpy(result.f_string, f_string, where * sizeof(mowc::wc_t));						/* Flawfinder: ignore */
	memcpy(result.f_string + where, string.f_string, l * sizeof(mowc::wc_t));				/* Flawfinder: ignore */
	memcpy(result.f_string + where + l, f_string + where, (f_length - where + 1) * sizeof(mowc::wc_t));	/* Flawfinder: ignore */

	return result;
}


/************************************************************ DOC:

CLASS

	moWCString

NAME

	Integer - transform the string in an integer
	Float - transform the string in a double

SYNOPSIS

	bool IsInteger(int base = 0) const;
	long Integer(int base = 0) const;
	long long LargeInteger(int base = 0) const;
	double Float(void) const;
	bool IsFloat(void) const;
	int IsTrue(unsigned long flags) const;
	int IsFalse(unsigned long flags) const;
	bool IPv4(unsigned long& address, unsigned long& port,
				unsigned long& mask) const;

PARAMETERS

	base - the base used to test the value (0 = automatic selection)
	flags - a set of flags to tell which type of true & false is supported
	address - placeholder for the IP version 4 address
	port - placeholder for the port number (0 if undefined)
	mask - placeholder for the mask address (-1 by default)

DESCRIPTION

	These function members can be used to get the value the
	string represents.

	The Integer() and IsInteger() functions accept a base parameter
	which by default is 0. When base is set to 0, the function tries
	to determine the base in which the value was written. The rules
	are the same as in C/C++: if the value starts with 0x or 0X it
	must be an hexadecimal value; if the value starts with 0, it
	must be an octal value; otherwise try to read the value as a
	decimal value.

	The Float() function expect a value float definition. It accepts
	a sign (+ or -) an integral part, a fractional part separated
	with a period (.) and a power of 10 exponent separated with
	the letter 'e' or 'E' and an optional sign (+ or -).

	The IsInteger() and IsFloat() functions will test the
	string and return 'true' only if the entire string
	represents a valid value.

	The IsTrue() and IsFalse() functions test for a valid boolean
	value. They can return an error (-1). The type of tests done
	on this string to determine the result depends on the flags
	passed to the functions. These flags are as follow and can
	be ored (|) together.

		. WC_STRING_BOOLEAN_TEST_INTEGER_C

		If the whole string clipped (i.e. removing the spaces)
		represent an integer (as per IsInteger() definition)
		then the IsTrue() function returns true if the
		represented integer is not null.

		. WC_STRING_BOOLEAN_TEST_INTEGER_ADA

		If the whole string clipped represent an integer, then
		the IsTrue() function returns true if the represented
		integer is 1 and false if the integer is 0. All other
		values generate an error.

		This flag has no effect if the INTEGER_C flag was also
		used.

		. WC_STRING_BOOLEAN_TEST_FLOAT

		If the whole string clipped represent a float, then
		the IsTrue() function returns true if the represented
		float is not 0.0 and false in all other cases.

		This flag has priority over the INTEGER flags since
		a float can be represented as a decimal integer.
		Note that integer will still be checked out since the
		value may have been written in hexadecimal (octal values
		are taken as floats when the FLOAT flag is set...).

		. WC_STRING_BOOLEAN_TEST_NAMED

		Expects the one of 'true' or 'false' spelled as is
		in any case ('True', 'TRUE', 'truE' are all the same).
		The function returns an error if the string is neither
		'true' nor 'false'.

		. WC_STRING_BOOLEAN_TEST_TEXT

		This flag is tested last, thus if another flag has
		an effect, it will be ignored. Otherwise, it will
		return true whenever the string isn't empty and
		false when the string is empty.

	The IPv4() function takes a string with one of the following
	formats:

		a.b.c.d[:p][/w.x.y.z]
		a.b.c.d[:p][/m]

	The port and mask are optional. The port is set to 0 if
	undefined. The mask is set to -1 if undefined.

	The values represented by the letters a, b, c, d, w, x, y and z
	can be any value between 0 and 255. The letter p representing
	the port can be defined between 0 and 65535 (note that 0 will
	no generate an error). The letter m represents the number of
	bits set to zero in the mask. It can be any value from 0 to 32
	inclusive. 32 means that the mask is 0 (i.e. all addresses are
	acceptable).

	Note that the address can be preceeded and followed by spaces.

ERRORS

	Integer - returns -1 if the string is not a valid value
		  and sets errno to EINVAL

	Float - retuns -1.0 if the string is not a valid float and
		sets errno to EINVAL

	IsTrue & IsFalse - return 0 if the expression represents
			   false, return 1 f the expression represents
			   true and -1 if the expression is viewed
			   as a non boolean value

	IsInteger & IsFloat - can't return an error

	Note: errno is always set to a value (0 or an error) and
	it can directly be used to check the outcome of this
	function call

	IPv4 - returns true when the address is valid and false if
		an error occurs; the entire string needs to represent
		the address; if anything other than spaces preceeds
		or follows the address, then an error is generated

BUGS

	These functions don't check for range overflow. This is
	because many times a value is written in such a way it
	represents an unsigned value which wouldn't overflow
	if read as an unsigned value instead of a signed long.
	The result, thought, is the same.

SEE ALSO

	strtol(3), strtod(3)

*/
int64_t moWCString::LargeInteger(int base) const
{
	return mowc::tolargeinteger(f_string, base);
}


int32_t moWCString::Integer(int base) const
{
	return mowc::tointeger(f_string, base);
}


bool moWCString::IsInteger(int base) const
{
	return mowc::isinteger(f_string, base);
}


double moWCString::Float(void) const
{
	return mowc::tofloat(f_string);
}


bool moWCString::IsFloat(void) const
{
	return mowc::isfloat(f_string);
}


int moWCString::IsTrue(unsigned long flags) const
{
	long		v;

	if((flags & WC_STRING_BOOLEAN_TEST_FLOAT) != 0) {
		if(mowc::isfloat(f_string)) {
			return mowc::tofloat(f_string) != 0.0 ? 1 : 0;
		}
	}

	if((flags & WC_STRING_BOOLEAN_TEST_INTEGER_C) != 0) {
		if(mowc::isinteger(f_string)) {
			return mowc::tointeger(f_string) != 0 ? 1 : 0;
		}
	}

	if((flags & WC_STRING_BOOLEAN_TEST_INTEGER_ADA) != 0) {
		if(mowc::isinteger(f_string)) {
			v = mowc::tointeger(f_string);
			if(v == 0) {
				return 0;
			}
			if(v == 1) {
				return 1;
			}
			errno = EINVAL;
			return -1;
		}
	}

	if((flags & WC_STRING_BOOLEAN_TEST_NAMED) != 0) {
		if(mowc::strcasecmp(f_string, "true") == 0) {
			return 1;
		}
		if(mowc::strcasecmp(f_string, "false") == 0) {
			return 0;
		}
	}

	if((flags & WC_STRING_BOOLEAN_TEST_YESNO) != 0) {
		if(mowc::strcasecmp(f_string, "yes") == 0) {
			return 1;
		}
		if(mowc::strcasecmp(f_string, "no") == 0) {
			return 0;
		}
	}

	if((flags & WC_STRING_BOOLEAN_TEST_LETTER) != 0 && f_length == (size_t) 1) {
		if(mowc::toupper(f_string[0]) == L'T') {
			return 1;
		}
		if(mowc::toupper(f_string[0]) == L'F') {
			return 0;
		}
	}

	if((flags & WC_STRING_BOOLEAN_TEST_TEXT) != 0) {
		return f_length == (size_t) 0 ? 0 : 1;
	}

	errno = EINVAL;
	return -1;
}

int moWCString::IsFalse(unsigned long flags) const
{
	int	r;

	r = IsTrue(flags);
	if(r == 0) {
		return 1;
	}
	if(r == 1) {
		return 0;
	}

	return -1;
}



bool moWCString::IPv4(unsigned long& address, unsigned short& port, unsigned long& mask) const
{
	return mowc::strtoipv4(f_string, address, port, mask);
}




/************************************************************ DOC:

CLASS

	moWCString

NAME

	FindChar - find the first instance of a character in a string
	FindRChar - find the last instance of a character in a string

SYNOPSIS

	moWCString FindChar(mowc::wc_t c, long position = 0, long length = -1) const;
	moWCString FindRChar(mowc::wc_t c, long position = -1, long length = -1) const;

PARAMETER

	c - the character searched

DESCRIPTION

	These functions search for the character c and
	return the rest of the string (i.e. equivalent to a
	Get(pos, -1) call) from that character.

	Note that the character is included in the result.

	If the character doesn't exist in the string,
	then an empty string is returned.

	FindChar() will search for the first instance of the
	given character.

	FindRChar() will search for the last instance of
	the given character.

	Both functions accept a position where the search starts
	(position included in case of FindChar() and not included
	int case of the FindRChar() function). This is why we have
	a special value of -1 as the default FindRChar() position
	which is automatically changed to the length of the
	string (i.e. search the entire string).

	The 'length' parameter can be used to avoid searching
	the entire string.

RETURNED VALUE

	A copy of the string from the character c or
	an empty string.

*/
moWCString moWCString::FindChar(mowc::wc_t c, long position, long length) const
{
	const mowc::wc_t	*s;
	long			len;

	len = f_length - (size_t) position;
	if(length > len || length == -1) {
		length = len;
	}
	s = f_string + position;
	while(length > 0) {
		if(*s == c) {
			return moWCString(s);
		}
		length--;
		s++;
	}

	return moWCString();	// empty
}


moWCString moWCString::FindRChar(mowc::wc_t c, long position, long length) const
{
	const mowc::wc_t	*s;

	if((size_t) position >= f_length) {
		position = f_length;
	}
	else {
		position = f_length - position;
	}
	if(length > position || length == -1) {
		length = position;
	}
	s = f_string + position;
	while(length > 0) {
		s--;
		if(*s == c) {
			return moWCString(s);
		}
		length--;
	}

	return moWCString();	// empty
}



/************************************************************ DOC:

CLASS

	moWCString

NAME

	FindAny - find the first instance of any one character in a string
	FindRAny - find the last instance of any one character in a string

SYNOPSIS

	long FindAny(const moWCString& string, long position = 0, long length = -1) const;
	long FindRAny(const moWCString& string, long position = 0, long length = -1) const;

PARAMETER

	string - the set of characters to search
	position - the position to start the seach in this string
	length - the maximum number of characters to search in this string

DESCRIPTION

	These functions search for any one character from the
	parameter 'string' in this string and return the position
	at which it was found. Note that the position is the position
	of the found character in this string.

	The length can be set to -1 in which case the search will go on
	up to the end (start) of the string.

	If the character doesn't exist in the string, then -1 is returned.

	FindAny() will search for the first instance of the
	given characters moving foward. Using the position, one
	can call the FindAny() function multiple times. The next time,
	call it with the last result + 1 until the function returns -1.

	FindRAny() will search for the last instance of
	the given characters (starting at position and moving backward).
	In this case the position should start at the end of the string
	(i.e. this->Length()). The length parameter is the number of
	characters to test going backward from the position. If you want
	to call this function multiple times, use the length
	of the string the first time and then the result - 1
	until the function returns -1.

RETURNED VALUE

	The position where one of the characters was found
	or -1 when none were found.

*/
long moWCString::FindAny(const moWCString& string, long position, long length) const
{
	const mowc::wc_t	*s, *t;
	mowc::wc_t		c;

	if((unsigned long) position >= f_length) {
		return -1;
	}
	if(position < 0) {
		position = 0;
	}

	if(length < 0) {
		length = f_length;
	}

	s = f_string + position;
	while((c = *s) != '\0' && length > 0) {
		t = string.f_string;
		while(*t != '\0') {
			if(*t == c) {
				return static_cast<long>(s - f_string);
			}
			t++;
		}
		length--;
		s++;
	}

	return -1;
}


long moWCString::FindRAny(const moWCString& string, long position, long length) const
{
	const mowc::wc_t	*s, *t;
	mowc::wc_t		c;

	if(position <= 0) {
		return -1;
	}
	if((size_t) position > f_length) {
		position = f_length;
	}

	if(length < 0) {
		length = f_length;
	}

	s = f_string + position;
	while(s > f_string && length > 0) {
		s--;
		c = *s;
		t = string.f_string;
		while(*t != '\0') {
			if(*t == c) {
				return static_cast<long>(s - f_string);
			}
			t++;
		}
		length--;
	}

	return -1;
}



/************************************************************ DOC:

CLASS

	moWCString

NAME

	FindInString - find the first instance of a string in a string
	FindInCaseString - find the first instance of a string in a string
	FindInRString - find the last instance of a string in a string
	FindInRCaseString - find the last instance of a string in a string
	FindString - find the first instance of a string in a string
	FindCaseString - find the first instance of a string in a string
	FindRString - find the last instance of a string in a string
	FindRCaseString - find the last instance of a string in a string

SYNOPSIS

	long FindInString(const moWCString& string, long position = 0, long length = -1) const;
	long FindInCaseString(const moWCString& string, long position = 0, long length = -1) const;
	long FindRInString(const moWCString& string, long position = 0, long length = -1) const;
	long FindRInCaseString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString FindString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString FindCaseString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString FindRString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString FindRCaseString(const moWCString& string, long position = 0, long length = -1) const;

PARAMETER

	string - the string searched
	position - the position to start the search
	length - the maximum number of characters to be compared

DESCRIPTION

	The Find...() functions search for 'string' in 'this' string
	and return either a string or the position at which 'string'
	was found.

	The functions including [In] will return a position between
	0 and 'this->Length() - 1' when 'string' is found and -1
	when the string isn't found.

	The other functions return a string composed of 'this'
	string starting at the found 'string'. When 'string' can't
	be found, an empty string is returned.

	The [Case] in the name of these functions means the
	CaseCompare() is used instead of the Compare(). In other
	words, these functions are case insensitive.

	The [R] in the name means "reverse". In other words, the
	search starts at the end of the word and moves toward the
	start.

	A 'position' can be specified in which case the search
	starts at that position. A positive position is taken
	from the start to the end of the string. A negative
	position is taken from the end of the string to the
	start. -1 is taken as the position right after the
	last character.

	A 'length' can be specified in which case at most that
	many characters from 'this' string will be tested against
	the input 'string'.

NOTES

	The input 'string' is always checked in full within
	'this' string. If 'this' string is smaller the user
	specified 'string' then these functions will always
	fail to find it and return -1 or the empty string.

RETURNED VALUE

	Find[R][Case]String
		A copy of the string from 'string' or an empty string.

	Find[R]In[Case]String
		The position at which the string was found
		or -1 when not found

*/
long moWCString::FindInString(const moWCString& string, long position, long length) const
{
	long		max;

	// we currently refuse to find an empty string in another string
	if(string.f_length > f_length || string.f_length == (size_t) 0 || f_length == (size_t) 0) {
		return -1;
	}

	// at this time we silently clamp the position
	if(position < 0) {
		position = labs(position + 1);
		if((size_t) position >= f_length) {
			position = 0;
		}
		else {
			position = f_length - (size_t) position;
		}
	}
	else if((size_t) position > f_length) {
		position = f_length;
	}

	max = f_length - (size_t) position - string.f_length;
	if(length < 0 || length > max) {
		length = max;
	}
	while(length >= 0) {
		if(Compare(string, position, string.f_length) == MO_BASE_COMPARE_EQUAL) {
			return position;
		}
		length--;
		position++;
	}

	return -1;
}


long moWCString::FindInCaseString(const moWCString& string, long position, long length) const
{
	long		max;

	// we currently refuse to find an empty string in another string
	if(string.f_length > f_length || string.f_length == (size_t) 0 || f_length == (size_t) 0) {
		return -1;
	}

	// at this time we silently clamp the position
	if(position < 0) {
		position = labs(position + 1);
		if((size_t) position >= f_length) {
			position = 0;
		}
		else {
			position = f_length - (size_t) position;
		}
	}
	else if((size_t) position > f_length) {
		position = f_length;
	}

	max = f_length - (size_t) position - string.f_length;
	if(length < 0 || length > max) {
		length = max;
	}
	while(length >= 0) {
		if(CaseCompare(string, position, string.f_length) == MO_BASE_COMPARE_EQUAL) {
			return position;
		}
		length--;
		position++;
	}

	return -1;
}


moWCString moWCString::FindString(const moWCString& string, long position, long length) const
{
	position = FindInString(string, position, length);
	return position >= 0 ? Get(position, f_length) : moWCString();
}


moWCString moWCString::FindCaseString(const moWCString& string, long position, long length) const
{
	position = FindInCaseString(string, position, length);
	return position >= 0 ? Get(position, f_length) : moWCString();
}


long moWCString::FindRInString(const moWCString& string, long position, long length) const
{
	long		max;

	// in these cases we can't find anything
	if(string.f_length > f_length || string.f_length == (size_t) 0 || f_length == (size_t) 0) {
		return -1;
	}

	// at this time we silently clamp the position
	if(position < 0) {
		position = labs(position + 1);
		if((size_t) position >= f_length) {
			position = 0;
		}
		else {
			position = f_length - (size_t) position;
		}
	}
	else if((size_t) position > f_length) {
		position = f_length;
	}

	max = f_length - (size_t) position - string.f_length;
	if(length < 0 || length > max) {
		length = max;
	}
	while(length >= 0) {
		if(Compare(string, position, string.f_length) == MO_BASE_COMPARE_EQUAL) {
			return position;
		}
		length--;
		position--;
	}

	return -1;
}


long moWCString::FindRInCaseString(const moWCString& string, long position, long length) const
{
	long		max;

	// in these cases we can't find anything
	if(string.f_length > f_length || string.f_length == (size_t) 0 || f_length == (size_t) 0) {
		return -1;
	}

	// at this time we silently clamp the position
	if(position < 0) {
		position = labs(position + 1);
		if((size_t) position >= f_length) {
			position = 0;
		}
		else {
			position = f_length - (size_t) position;
		}
	}
	else if((size_t) position > f_length) {
		position = f_length;
	}

	max = f_length - (size_t) position - string.f_length;
	if(length < 0 || length > max) {
		length = max;
	}
	while(length >= 0) {
		if(CaseCompare(string, position, string.f_length) == MO_BASE_COMPARE_EQUAL) {
			return position;
		}
		length--;
		position--;
	}

	return -1;
}


moWCString moWCString::FindRString(const moWCString& string, long position, long length) const
{
	position = FindRInString(string, position, length);
	return position >= 0 ? Get(position, f_length) : moWCString();
}


moWCString moWCString::FindRCaseString(const moWCString& string, long position, long length) const
{
	position = FindRInCaseString(string, position, length);
	return position >= 0 ? Get(position, f_length) : moWCString();
}




/** \brief reverse the characters in a string
 *
 * This function reverse all the characters in a string.
 * The first character becomes the last and the last
 * becomes the first. The second character becomes the
 * one before the last, and vice versa. And so on with
 * each character.
 *
 * \note
 * This function doesn't make a copy of this string.
 * The reverse is applied directly to this string.
 *
 * \return this string reversed.
 */
moWCString& moWCString::Reverse()
{
	mowc::wc_t	*s, *e, c;

	f_string_changed = true;

	s = f_string;
	e = f_string + f_length - 1;
	while(e > s) {
		c = *s;
		*s = *e;
		*e = c;
		s++;
		e--;
	}

	return *this;
}



// internal function applying the Glob feature.
static bool moWCStringGlob(const mowc::wc_t *s, const mowc::wc_t *p)
{
	bool			invert;
	mowc::wc_t	from, to;

	while(*p != '\0') {
		switch(*p) {
		case '*':
			do {
				p++;
			} while(*p == '*');
			if(*p == '\0') {
				return true;
			}
			while(*s != '\0') {
				if(moWCStringGlob(s, p)) {
					return true;
				}
				s++;
			}
			return false;

		case '?':
			// accept any character except end of string
			if(*s == '\0') {
				return false;
			}
			s++;
			p++;
			break;

		case '[':
			// accept any character (or not) defined between [...]
			if(*s == '\0') {
				return false;
			}
			p++;
			invert = *p == '!' || *p == '^';
			if(invert) {
				p++;
			}
			if(*p != '\0') {
				do {
					to = from = *p;
					if(p[1] == '-') {
						if(p[2] != ']') {
							to = p[2];
							p += 3;
						}
						else {
							p += 2;
						}
					}
					else {
						p++;
					}
					if((*s >= from && *s <= to) ^ invert) {
						// match!
						s++;
						// skip the rest of the [...]
						while(*p != ']' && *p != '\0') {
							p++;
						}
						if(*p == ']') {
							p++;
						}
						goto cont;
					}
				} while(*p != ']' && *p != '\0');
			}
			// couldn't match this character
			return false;

		default:
			// direct comparison
			// TODO: under DOS we probably want to check case insensitivly...
			if(*s != *p) {
				return false;
			}
			s++;
			p++;
			break;

		}
cont:;
	}

	return *s == '\0';
}


/** \brief Compare a shell pattern against this string.
 *
 * This function compares this string with the given
 * pattern.
 *
 * The pattern must be a shell compatible pattern and
 * include the following special elements:
 *
 * \code
 *   * - an asterisk, meaning one or more of any
 *       character
 *   ? - a question mark, meaning any one character
 *   [...] - a set of characters between square
 *       brackets meaning any one character;
 *       ranges are allowed, syntax: smaller character,
 *       dash and largest character;
 *       you can use the ! or ^ characters as the
 *       very first to inverse the meaning of the
 *       range (i.e. any character that does not match)
 * \endcode
 *
 * \param[in] pattern   A shell pattern (such as *.c)
 *
 * \return true when the pattern matched
 *
 * \sa Match
 * \sa moRegExpr
 */
bool moWCString::Glob(const moWCString& pattern) const
{
	return moWCStringGlob(f_string, pattern.f_string);
}





/** \brief Match this string against a regular expression
 *
 * This function uses the regex interface to test whether this
 * string matches the user specified pattern. You should use
 * the ^ and $ if you want to make sure that the entire string
 * is a match.
 *
 * Please, try 'man 7 regex' to have more info about the pattern.
 *
 * The whole string & pattern must match. The . won't accept
 * the NUL and new line characters. The class characters are
 * understood. The pipe (|) is used to seperate two possible
 * alternatives. The parenthesis are used to group expressions.
 *
 * \todo
 * Add support for case insensitive matching.
 *
 * \bug
 * The function always returns false when an invalid pattern
 * is specified.
 *
 * \bug
 * At this time the wide characters are not tested directly.
 * Instead, the strings are changed to multi-characters which
 * may or may not be fully compatible with the regcomp/regexec
 * functions.
 *
 * \bug
 * 'this' string and the pattern must both be small enough to
 * fit in static strings of 256 characters once converted to
 * multi-byte characters.
 * 
 * \param[in] pattern  A regular expression pattern (such as "^a.*(b|c)$")
 *
 * \return true when the pattern matched
 *
 * \sa Glob
 * \sa moRegExpr
 */
bool moWCString::Match(const moWCString& pattern) const
{
	regex_t		re;
	int		ec;
	char		p[256], q[256];		/* Flawfinder: ignore */

	// TODO: we probably want a way to test case insensitive!

	// NOTE: the regcomp() + regexec() will most certainly not
	// understand the multi-byte characters...
	mowc::wcstombs(p, pattern.f_string, sizeof(p));
	mowc::wcstombs(q, f_string, sizeof(q));

	//regcomp(&re, pattern.Data(), RE_CHAR_CLASSES | RE_DOT_NOT_NULL | RE_HAT_LISTS_NOT_NEWLINE | RE_NO_BK_PARENS | RE_NO_BK_REFS | RE_NO_BK_VBAR);
	ec = regcomp(&re, p, REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
	if(ec == 0) {
		ec = regexec(&re, q, 0, 0, 0);
		regfree(&re);
	}

	return ec == 0 ? true : false;
}






/** \brief Replace some strings in a srting.
 *
 * This function searches for a set of strings and
 * replaces them as defined in \p what.
 *
 * The \p what string syntax is as follow:
 *
 * \code
 *	<string>=<replace>';'
 * \endcode
 *
 * If replace needs to include a ';' then it must be
 * escaped with a backslash.
 *
 * At this time the source \<string> is case sensitive
 * and there is no way to make it otherwise.
 *
 * This function can be used to replace backslashed
 * characters with their corresponding ASCII equivalents:
 *
 * \code
 *	\\n=\n;\\r=\r;\\\\=\\...
 * \endcode
 *
 * Or to the contrary, used to add backslashes:
 *
 * \code
 *	\n=\\\\n;\r=\\\\r;\\\\=\\\\\\\\...
 * \endcode
 *
 * Because a backslash is escaped and also require to
 * be escaped by a backslash in a C string, you need
 * to use many as the previous example shows if the
 * replacements involve backslashes. Note that the
 * following C string:
 *
 * \code
 *	'=\\'
 * \endcode
 *
 * has no effect in Replace(). However, the following:
 *
 * \code
 *	'=\\\\'
 * \endcode
 *
 * will add a backslash in front of any single quote (').
 * 
 * \note
 * This function applies the replacement strings only
 * once. In order to apply replacement strings recursively,
 * you will need to call this function multiple times.
 *
 * \note
 * The source strings (in \p what) used to match the input
 * string will support regular expressions later. Similarly,
 * the result strings could include \(1\). This isn't
 * available yet.
 *
 * \bug
 * Terminate a string with a backslash will include that
 * backslash in the result. This is a side effect which
 * shouldn't cause trouble. One can suppress the extra
 * backslash or maybe add the missing character...
 *
 * \param[in] what  A list of strings to replace and how.
 *
 * \return a copy of the source string with the given strings replaced
 *
 * \sa FromBackslash
 * \sa Strip
 * \sa Clip
 * \sa NoNewLine
 * \sa Transpose
 */
moWCString moWCString::Replace(const moWCString& what) const
{
	moWCString		result;
	const mowc::wc_t	*str, *cmp, *s; //, *start;
	bool			found;
	//size_t			l;

	if(f_password) {
		result.Password();
	}
	if(what.f_length == 0) {
		// no replacement, make it quick
		return result = *this;
	}

	str = f_string;
	while(*str != '\0') {
		s = what.f_string;
		//start = s;
		cmp = str;
		found = true;
		while(*s != '\0') {
			if(*s == L'=') {
				//l = cmp - str;
				//if(l <= f_length - (size_t) (str - f_string) && memcmp(str, start, l * sizeof(mowc::wc_t)) == 0)
				if(found) {
					// we found a string to replace
					str = cmp;
					s++;	// skip the '=' sign
					while(*s != ';' && *s != '\0') {
						if(*s == '\\' && s[1] != '\0') {
							s++;
						}
						result += *s++;
					}
					goto found;
				}
				while(*s != ';' && *s != '\0') {
					if(*s == '\\' && s[1] != '\0') {
						s++;
					}
					s++;
				}
				if(*s == ';') {
					s++;
				}
				cmp = str;
				found = true;
				//start = s;
			}
			else if(*s == '\\') {
				s++;
				if(*s != '\0') {
					if(*cmp != *s) {
						found = false;
					}
					else {
						cmp++;
					}
					s++;
				}
			}
			else {
				if(*cmp != *s) {
					found = false;
				}
				else {
					cmp++;
				}
				s++;
			}
		}
		// this was not meant for replacement, keep as is
		result += *str++;
found:
		;		// label needs a statement...
	}

	return result;
}


/** \brief Replace some characters by others
 *
 * This function replaces each character of this string found
 * in the source string with the corresponding character in
 * the destination string.
 *
 * For instance, to convert all digits to the character '#' one
 * can use:
 *
 * \code
 * 	nodigit = str.Transpose("0123456789", "##########");
 * \endcode
 *
 * At this time, ranges are not supported.
 *
 * \note
 * Both input strings (source & destination) are expected to be
 * of the exact same length. If not, an exception may be thrown.
 * 
 * \param[in] source        A list of character to replace.
 * \param[in] destination   The corresponding replacement characters.
 *
 * \return a copy of the source string with the given characters transposed
 *
 * \sa Replace
 * \sa FromBackslash
 * \sa Strip
 * \sa Clip
 * \sa NoNewLine
 */
moWCString moWCString::Transpose(const moWCString& source, const moWCString& destination) const
{
	const mowc::wc_t	*s;
	mowc::wc_t		c[2];
	long			p;
	moWCString		result;

	c[1] = '\0';
	s = f_string;
	while((c[0] = *s) != '\0') {
		p = source.FindAny(c);
		if(p >= 0) {
			// found character, replace it
			c[0] = destination.Get(p);
		}
		result += c[0];
	}

	return result;
}





/** \brief Replace backslashed characters.
 *
 * This function creates a duplicate of the input string and transform
 * the backslashed characters as expected (i.e. \\r becomes the
 * character 0x0D).
 * 
 * The understood backslash characters are all described in the
 * mowc::backslash_char() function.
 * 
 * The characters written in UNICODE notation are also converted
 * automatically. It accepts any digit written in this way:
 *
 * \code
 *	"U+####"
 * \endcode
 *
 * Each digit is part of an hexadecimal value. One to eight digits
 * will be accepted (though the standard is to read 4 to 8). There
 * is no restriction to the accepted characters except that it has
 * to be in the range 0 to 0x7FFFFFFF.
 *
 * \return A copy of the source string without backslashed characters
 *
 * \bug
 * Terminate a string with a backslash will include that
 * backslash in the result. This is a side effect which
 * shouldn't cause to much trouble. One can suppress the
 * backslash or add the missing character.
 * 
 * \bug
 * All the entries generating '\\0' are actually not
 * used (i.e. these characters are skipped).
 *
 * \sa Replace
 * \sa mowc::backslash_char()
 */
moWCString moWCString::FromBackslash(void) const
{
	moWCString		result;
	const mowc::wc_t	*str;
	mowc::wc_t		value;
	int			cnt;

	if(f_password) {
		result.Password();
	}

	str = f_string;
	while(*str != '\0') {
		if(*str == '\\') {
			str++;
			result += mowc::backslash_char(str);
		}
		else if(*str == 'U' && str[1] == '+' && mowc::isxdigit(str[2])) {
			// this is a unicode character notation
			str += 2;
			value = 0;
			cnt = 8;
			do {
				value = value * 16 + mowc::xdigit(*str);
				str++;
				cnt--;
			} while(cnt > 0 && mowc::isxdigit(*str));
			result += value;
		}
		else {
			result += *str++;
		}
	}

	return result;
}





/** \brief Check whether there is a filename extension.
 *
 * This function searches for a filename extension in this string.
 * 
 * \note
 * A name starting with a period is considered as being a hidden
 * filename and therefore is never returned as an extension of a
 * filename.
 *
 * \bug
 * An empty extension (i.e. a lone period) is considered to be an
 * extension and this function returns true.
 *
 * \return true if the filename has an extension; false otherwise.
 *
 * \sa FileNameExtension
 */
bool moWCString::FilenameHasExtension(void) const
{
	const mowc::wc_t	*s;

	s = f_string + f_length;
	while(s > f_string) {
		s--;
		if(s == f_string || s[-1] == '/' || s[-1] == '\\' || s[-1] == ':') {
			break;
		}
		if(*s == '.') {
			return true;
		}
	}

	return false;
}



/** \brief Extract a filename extension.
 *
 * This function extracts the extension from a filename. If none is
 * found it returns an empty string.
 *
 * \note
 * A name starting with a period is considered as being a hidden
 * filename and therefore is never returned as an extension of a
 * filename.
 *
 * \bug
 * An empty extension will be returned as a lone period (when a
 * filename ends with a period.)
 *
 * \return A copy of the extension including the period or an empty string
 *
 * \sa FilenameHasExtension
 */
moWCString moWCString::FilenameExtension(void) const
{
	const mowc::wc_t	*s;

	s = f_string + f_length;
	while(s > f_string) {
		s--;
		if(s == f_string || s[-1] == '/' || s[-1] == '\\' || s[-1] == ':') {
			break;
		}
		if(*s == '.') {
			if(f_password) {
				moWCString result(s);
				result.Password();
				return result;
			}
			return s;
		}
	}

	return moWCString();
}




/** \brief Extract the basename out of a filename.
 *
 * This function removes all the directory names and eventually
 * the specified extension.
 *
 * The extension must include the period if it is to be
 * removed (i.e. ".c++")
 *
 * The special ".*" extension can be used to the extension
 * whatever it is.
 *
 * The special "" extension can be used to not remove any
 * extension.
 *
 * \note
 * If the extension doesn't match the specified extension
 * then nothing is removed from the end of the filename.
 *
 * \bug
 * At this time, the extensions are tested in a case sensitive
 * manner. This will be fixed at a later time.
 *
 * \param[in] extension   The extension to be removed.
 *
 * \return a copy of the filename without path and eventually without extension.
 *
 * \sa FilenameDirname
 * \sa FilenameChild
 * \sa FilenameClean
 */
moWCString moWCString::FilenameBasename(const moWCString& extension) const
{
	const mowc::wc_t	*s, *e;
	unsigned long		l;

	s = f_string + f_length;
	e = s;

	if(extension == ".*") {
		// special case, search for any extension
		while(s > f_string && s[-1] != '/' && s[-1] != '\\') {
			s--;
			if(*s == '.') {
				// avoid hidden files vs extensions
				if(s > f_string && s[-1] != '/' && s[-1] != '\\') {
					e = s;
				}
				break;
			}
		}
	}
	else {
		// NOTE: if the extension include slashes (/) then
		// some directory names will be removed in the following
		// code...
		l = extension.f_length;
		if(l > 0 && l < f_length
		&& memcmp(s - l, extension.f_string, l * sizeof(mowc::wc_t)) == 0) {
			s -= l;
			e = s;
		}
	}

	while(s > f_string) {
		if(s[-1] == '/' || s[-1] == '\\') {
			/* we stop here! */
			break;
		}
		s--;
	}

	moWCString result(s, static_cast<int>(e - s));
	if(f_password) {
		result.Password();
	}

	return result;
}




/** \brief Extract the path to a file.
 *
 * This function removes the basename from a filename. In
 * effect leaving only the path to the directory containing
 * the file (assuming it exists).
 *
 * No trailing "/" is kept in the resulting pathname
 * except for the root directory (ie. "/" remains "/").
 *
 * Note that this function can be called multiple times in
 * order to remove some trailing directory names from a
 * path.
 *
 * When there is no directory path (not even the root directory,)
 * then "." is returned.
 *
 * If the filename ends with "/" then that character is
 * ignore. Thus one directory name will always be removed.
 *
 * \return a copy of the filename with only the directory names
 * or "." when no name is defined.
 *
 * \sa FilenameBasename
 * \sa FilenameChild
 * \sa FilenameClean
 */
moWCString moWCString::FilenameDirname(void) const
{
	const mowc::wc_t	*s;

	s = f_string + f_length;

	// remove trailing "/"'s
	while(s > f_string && (s[-1] == '/' || s[-1] == '\\')) {
		s--;
	}

	// search for the previous "/"
	while(s > f_string && s[-1] != '/' && s[-1] != '\\') {
		s--;
	}

	// we don't want trailing "/" in the result
	if(s > f_string && (s[-1] == '/' || s[-1] == '\\')) {
		while(s > f_string && (s[-1] == '/' || s[-1] == '\\')) {
			s--;
		}
		// we need to keep at least the root
		// otherwise it would become "."!!!
		if(s == f_string) {
			s++;
		}
	}

	// do we have an empty string now?
	// if so, return "."
	if(s == f_string) {
		moWCString result(".");
		if(f_password) {
			result.Password();
		}
		return result;
	}

	moWCString result(f_string, static_cast<int>(s - f_string));
	if(f_password) {
		result.Password();
	}

	return result;
}




/** \brief Appends a filename or directory to a path.
 *
 * This function appends a filename to a path making sure
 * there is a valid separator between both.
 *
 * If this string (the path) is empty, then \p child is returned
 * as is.
 *
 * If the child filename is empty, then the function will at
 * least ensure that the path ends with a slash (unless the
 * path is empty in which case an empty string is returned).
 *
 * If the child string starts with a slash, it is not duplicated in
 * the resulting string.
 *
 * \return The concatenation of this string (the path) and the child filename.
 *
 * \sa FilenameBasename
 * \sa FilenameDirname
 * \sa FilenameClean
 */
moWCString moWCString::FilenameChild(const moWCString& child) const
{
	if(f_length == (size_t) 0) {
		return child;
	}

	moWCString result(*this);

	if(f_string[f_length - (size_t) 1] != '/'
	&& f_string[f_length - (size_t) 1] != '\\') {
		result += '/';
	}

	if(child.f_length > (size_t) 0) {
		// TODO: canonilize? (i.e. skip all starting '/' in child?)
		if(child.f_string[0] == '/' || child.f_string[0] == '\\') {
			// skip the extra slash!
			result += child.f_string + 1;
		}
		else {
			result += child;
		}
	}

	// NOTE: the copy of *this already copied f_password
	if(child.f_password) {
		result.Password();
	}

	return result;
}




/** \brief Braces expension of filename.
 *
 * This function parses a filename path from this string and
 * transforms it in a list of filenames by expanding brace
 * expressions.
 *
 * The new names are appended to the supplied user list.
 * That list is not cleared first.
 *
 * The expension rules are the same as for the brace expension
 * of bash. A brace expension can include an optional prefix,
 * a list of brace expension and an optional postfix. Note that
 * braces can be nested and several can be included in the
 * same string. Also, path separators are totally ignored
 * and thus the expension can be used to expend names including
 * such separators.
 *
 * Like for the separators, pattern characters (*, ?, [ and ])
 * are ignored when the brace expension is applied.
 *
 * Braces within the input string can be taken as regular
 * characters when preceeded by a backslash. Backslashes
 * are preserved in the resulting list of strings.
 *
 * For example:
 *
 * \code
 * 	abc{d,e,f}ghi
 * \endcode
 *
 * expands as the three names:
 *
 * \code
 * 	abcdghi abceghi abcfghi
 * \endcode
 *
 * And you can write complicated expressions such as:
 *
 * \code
 * /usr/{etc/lib*{/exec*,c/exec*},lib/{module,plugins}/{exec*.so.*}}
 * \endcode
 *
 * The order expected when reading the braces is kept.
 *
 * The function does not test for valid variables ("${...}").
 * The start of the variable can be skipped (when skip_variable
 * is set to true), but the closing brace won't be searched.
 * If there are braces within variables, they will be expended
 * as expected. You could first expand variables, then braces
 * to avoid this side effect.
 *
 * \note
 * Though this should be an error, if there are missing closing
 * braces (}), the function works as if the end of the string was
 * the missing closing braces. This can be detected testing the
 * returned value (which is false when there are missing braces).
 *
 * \bug
 * At this time, the resulting list may include duplicates.
 * Especially if the braces include duplicates as in:
 *
 * \par
 * \code
 * 	a{b,b,b}c -> abc abc abc
 * \endcode
 *
 * \par
 * This is actually a feature, but you have to keep it in mind!
 *
 * \bug
 * The input list can't be sorted because of the way this function
 * rely on the moList::Append() putting new names at the end
 * of the list.
 *
 * \bug
 * The \ is used to escape special characters. This will NOT work
 * well under MS-Windows if your path uses \ as the directory
 * separator character.
 *
 * \param[in] list             A user list where the filenames will be saved.
 * \param[in] skip_variables   Whether ${...} should be skipped (not taken as a brace expension.)
 *
 * \return If there was one closing brace for each opening brace, then
 *         the function returns true, otherwise it returns false.
 */
bool moWCString::FilenameExpand(moListOfWCStrings& list, bool skip_variables) const
{
	mowc::wc_t		*s, *start;
	long			missing, l;
	moList::position_t	i, j, max;
	moWCStringSPtr		str;
	moWCString		n;

	missing = 0;

	// if the string doesn't include any '{' then the list will
	// only have one additional string appended -- this one
	list += *new moWCString(*this);
	for(i = list.Count() - 1; i < list.Count(); i++) {
		str = list.Get(i);
		s = str->f_string;
		while(*s != '\0') {
			if(*s == L'{') {
				// Expand!
				moWCString prefix(str->f_string, static_cast<int>(s - str->f_string));
				l = 0;
				j = list.Count();
				start = s + 1;
				do {
					if(*s == L'{') {
						l++;
					}
					else if(*s == L'}') {
						l--;
						if(l == 0) {
							missing--;
							break;
						}
					}
					else if(l == 1 && *s == ',') {
						n = prefix + moWCString(start, static_cast<int>(s - start));
						list += *new moWCString(n);
						start = s + 1;
					}
					s++;
				} while(*s != L'\0');
				missing++;
				n = prefix + moWCString(start, static_cast<int>(s - start));
				list += *new moWCString(n);
				if(*s != '\0') {
					s++;
				}
				// add the postfix now that we know where it is
				// (note that these entries are all AFTER the
				// current index (i) thus they all will be parsed
				// again in case the postfix includes braces)
				max = list.Count();
				while(j < max) {
					*list.Get(j) += s;
					j++;
				}
				list.Delete(i);
				i--;		// the for() does ++
				//delete str; -- the Delete() does it for us now
				break;
			}
			else if(skip_variables && *s == '$' && s[1] == '{') {
				s++;
			}
			else if(*s == '\\' && s[1] != '\0') {
				s++;
			}
			s++;
		}
	}

	return missing == 0;
}




/** \brief Remove useless entries in a filename (canonicalization.)
 *
 * This function looks within a filename and remove things
 * such as "//", "/./" and "/../" as possible without
 * changing the final path making it canonical.
 *
 * It will remove a starting "./" or ending "/.".
 *
 * Note that a path such as "test/.." is transformed into
 * the path "./". Also, the path "/.." is equivalent to
 * "/".
 *
 * When the input is an empty string, the path "." is
 * returned. If you need to know if the input string
 * is empty, use the IsEmpty() function on the input.
 *
 * \note
 * The resulting path may or may not end with a slash.
 * There is currently no rule about that. (i.e in some
 * cases you could get "." and others "./" as an answer)
 *
 * \bug
 * By cleaning a path before testing whether the file
 * pointing to exists will eventually result in a
 * valid path even thought the original path wasn't
 * valid. Thus, a path such "/I-don't-exist/.." is
 * transformed to "/" which is valid.
 *
 * \return A canonical version of the path and filename.
 *
 * \sa FilenameBasename
 * \sa FilenameChild
 * \sa FilenameDirname
 */
moWCString moWCString::FilenameClean(void) const
{
	moWCString		result(*this);
	const mowc::wc_t	*s;
	mowc::wc_t		*d, *p;

	if(f_password) {
		result.Password();
	}

	s = f_string;
	d = result.f_string;

	// if empty, we're done
	if(*s == '\0') {
		d[0] = '.';
		d[1] = '\0';
		result.f_length = 1;
		return result;
	}

	// remove any leading "./" (or ".//", ".///", etc.)
	while(s[0] == '.' && s[1] == '/') {
		s += 2;
		while(*s == '/') {
			s++;
		}
	}

	// remove the "//", "/./" and "/../"
	while(*s != '\0') {
		*d++ = *s++;
		if(s[-1] == '/') {
			// remove multiple /
repeat:
			while(*s == '/') {
				s++;
			}
			// remove "/./"
			if(s[0] == '.' && s[1] == '/') {
				s += 2;
				// skip extra '/', '/./' and '/../'
				goto repeat;
			}
			// remove "/." at the end
			if(s[0] == '.' && s[1] == '\0') {
				s++;
			}
			// remove "/../" if possible
			if(s[0] == '.' && s[1] == '.' && (s[2] == '/' || s[2] == '\0')) {
				s += s[2] == '\0' ? 2 : 3;
				p = d - 1;
				while(p > result.f_string && p[-1] != '/') {
					p--;
				}
				if(p[0] == '.' && p[1] == '/') {
					// keep the "/../" (when preceeded by a "/./")
					if(p == result.f_string) {
						// ha! we had "./" at the start!
						d = p;
					}
					*d++ = '.';
					*d++ = '.';
					*d++ = '/';
				}
				else if(p[0] == '.' && p[1] == '.' && p[2] == '/') {
					// keep the "/../" (when preceeded by a "/../")
					*d++ = '.';
					*d++ = '.';
					*d++ = '/';
				}
				else {
					// remove the "/../" and previous directory
					if(p == result.f_string) {
						if(p[0] != '/') {
							d = p;
							*d++ = '.';
							*d++ = '/';
						}
					}
					else {
						d = p;
					}
				}
				// skip extra '/', '/./' and '/../'
				goto repeat;
			}
		}
	}
	result.f_length = static_cast<int32_t>(d - result.f_string);
	if(result.f_length == (size_t) 0) {
		*d++ = '.';
		result.f_length = 1;
	}
	*d = '\0';

	return result;
}





/** \brief Generate a string with only uppercase characters.
 *
 * This function creates a new string from this string
 * with all the lowercase characters transformed to uppercase
 * characters.
 *
 * \return A copy of this string in uppercase
 */
moWCString moWCString::Uppercase() const
{
	moWCString	result(*this);
	mowc::wc_t	*s;

	if(f_password) {
		result.Password();
	}

	s = result.f_string;
	while(*s != '\0') {
		*s = mowc::toupper(*s);
		s++;
	}

	return result;
}


/** \brief Generate a string with only lowercase characters.
 *
 * This function creates a new string from this string
 * with all the uppercase characters transformed to lowercase
 * characters.
 *
 * \return A copy of this string in lowercase
 */
moWCString moWCString::Lowercase() const
{
	moWCString	result(*this);
	mowc::wc_t	*s;

	if(f_password) {
		result.Password();
	}

	s = result.f_string;
	while(*s != '\0') {
		*s = mowc::tolower(*s);
		s++;
	}

	return result;
}




/** \brief Switch the case of each character.
 *
 * This function creates a new string from this string
 * with all the uppercase characters transformed to lowercase
 * characters and vice versa.
 *
 * \return A copy of this string with the opposite case for each character.
 */
moWCString moWCString::Switchcase() const
{
	moWCString	result(*this);
	mowc::wc_t	*s, c;

	s = result.f_string;
	while(*s != '\0') {
		c = *s;
		if(mowc::isupper(c)) {
			*s = mowc::tolower(c);
		}
		else {
			*s = mowc::toupper(c);
		}
		s++;
	}

	return result;
}


/** \brief Capitalize this string.
 *
 * This function creates a new string from this string
 * with the very first character forced to uppercase
 * and all the others to lowercase.
 *
 * \return A copy of this string with only the first character in uppercase.
 */
moWCString moWCString::Capitalize() const
{
	moWCString	result(*this);
	mowc::wc_t	*s;
	bool		capitalize;

	if(f_password) {
		result.Password();
	}

	capitalize = true;

	s = result.f_string;
	while(*s != '\0') {
		if(capitalize) {
			if(mowc::isalpha(*s)) {
				*s = mowc::toupper(*s);
				capitalize = false;
			}
		}
		else if(*s == '.') {
			capitalize = true;
		}
		else {
			*s = mowc::tolower(*s);
		}
		s++;
	}

	return result;
}


/** \brief Capitalize the words in this string.
 *
 * This function creates a new string from this string
 * with the first character of each word forced to
 * uppercase and all the others to lowercase.
 *
 * \bug
 * For this to work properly, we would need to have a
 * list of words that should not be capitalized.
 *
 * \return A copy of this string with each word capitalized.
 */
moWCString moWCString::CapitalizeWords() const
{
	moWCString	result(*this);
	mowc::wc_t	*s;
	bool		capitalize;

	if(f_password) {
		result.Password();
	}

	capitalize = true;

	s = result.f_string;
	while(*s != '\0') {
		if(capitalize) {
			if(mowc::isalpha(*s)) {
				*s = mowc::toupper(*s);
				capitalize = false;
			}
		}
		else if(!mowc::isalpha(*s)) {
			capitalize = true;
		}
		else {
			*s = mowc::tolower(*s);
		}
		s++;
	}

	return result;
}









// internal class used to implement the format string feature
class moOStreamString : public moOStream {
public:

moOStreamString(mowc::wc_t *str, size_t max)
{
	f_str = str;
	f_max = max * sizeof(mowc::wc_t);	// in bytes
}
virtual ~moOStreamString()
{
}

virtual int RawWrite(const void *buffer, size_t length)
{
	// note that the position is in bytes
	if(static_cast<size_t>(f_output_position) + length > f_max) {
		length = f_max - f_output_position;
	}
	if(length > 0) {
		memcpy(reinterpret_cast<char *>(f_str) + f_output_position, buffer, length);	/* Flawfinder: ignore */
		f_output_position += static_cast<uint32_t>(length);
	}
	return static_cast<int>(length);
}


private:
	mowc::wc_t *		f_str;
	size_t			f_max;
};



/** \brief Format a string.
 *
 * This function generates the string str using the specified format.
 *
 * The \p max parameter defines the size of the output string. If the format
 * and parameters generate a longer string, then it will not be written in the
 * output.
 *
 * The function can be used to know the size of the output string by calling
 * the function with str set to NULL.
 *
 * \param[in] str     A pointer to the output string.
 * \param[in] max     The size of the output buffer.
 * \param[in] format  The format used to generate the output string.
 * \param[in] args    The array of arguments.
 *
 * \return The size required
 *
 * \sa VFormat(const char *format, mowc::encoding_t encoding, va_list args)
 */
int moWCString::vswprintf(mowc::wc_t *str, size_t max, const mowc::wc_t *format, va_list args)	/* Flawfinder: ignore */
{
	moOStreamString s(str, max);
	int		result;

	result = mowc::vfwprintf(&s, format, args);	/* Flawfinder: ignore */
	s.Put('\0');		// null terminated string
	return result;
}


/** \brief Format a string.
 *
 * Please, see vswprintf(mowc::wc_t *str, size_t max, const mowc::wc_t *format, va_list args);
 * for more information.
 *
 * \param[in] str     A pointer to the output string.
 * \param[in] max     The size of the output buffer.
 * \param[in] format  The format used to generate the output string.
 * \param[in] ...     The arguments.
 *
 * \return The size required
 *
 * \sa VFormat(const char *format, mowc::encoding_t encoding, va_list args)
 */
int moWCString::swprintf(mowc::wc_t *str, size_t max, const mowc::wc_t *format, ...)	/* Flawfinder: ignore */
{
	va_list		args;
	int		result;
	moOStreamString s(str, max);

	va_start(args, format);
	result = mowc::vfwprintf(&s, format, args);	/* Flawfinder: ignore */
	va_end(args);
	s.Put('\0');		// null terminated string

	return result;
}


/** \brief Format a string with a standard C format string.
 *
 * The format functions can be used to create a string with the
 * result of a formatted set of arguments.
 *
 * The VFormat() accepts a variable list of arguments as
 * obtained by va_start(args, <last function parameter>).
 *
 * It does not use the standard wprintf() function of the C library
 * because otherwise it would transform the different characters
 * depending on environment defined settings. Instead, the whole
 * function format was reimplemented.
 *
 * Please, though it does not use the wprintf(), you certainly
 * want to see this function documentation for more information
 * about the default formats supported by these functions.
 *
 * The following is a list of the flags supported by this
 * implementation:
 *
 * The format of a conversion is as follow:
 *
 * \code
 * '%' [<position>'$'] [<flags>] [<width>] ['.'<precision>] [<length>] <conversion>
 * \endcode
 *
 * There is one special case: "%%" will be used to insert a percent
 * in the output.
 *
 * Position:
 * 
 *	\par
 *	Defines the position of the argument in the list of
 *	parameters. This is useful to write the parameters
 *	in run-time varying order.
 * 
 *	\par
 *	The positions start at 1 from the first parameter
 *	after the format.
 * 
 * Flags:
 * 
 * \li #
 *
 *	\par
 *	introducer; when used as in %\#o, prepend a "0"; when used
 *	as in %\#x or %\#X, include a "0x" or "0X" respectively;
 *	when used with one of the floating conversions, ensures
 *	that a decimal point always appears.
 * 
 * \li ' '
 *
 *	\par
 *	(space) writes a space wherever a signed and positive
 *	value is written
 * 
 * \li +
 *
 *	\par
 *	writes a plus for positive and signed values (unsigned
 *	value conversions ignore this flag).
 * 
 * \li 0
 *
 *	\par
 *	zero pad on the left up to either precision or width
 *	digits
 * 
 * \li -
 *
 *	\par
 *	left justify (default to right justification).
 * 
 * \li `
 *
 *	\par
 *	write out thousands in decimal conversions.
 * 
 * Width:
 * 
 *	\par
 *	The width is a decimal value which can be defined as a direct
 *	value an asterisk or an asterisk followed by a position
 *	(\<position>'$'). When an asterisk is specified, the width is
 *	taken from the list of parameters as an 'int'.
 * 
 * Precision:
 * 
 *	\par
 *	The precision is a decimal value which can be defined as a
 *	direct value, an asterisk or an asterisk followed by a position
 *	(\<position>'$'). When an asterisk is specified, the precision
 *	is taken from the list of parameters as an 'int'.
 * 
 * Length:
 * 
 * \li hh
 *
 *	\par
 *	A character.
 * 
 * \li h
 *
 *	\par
 *	A short.
 * 
 * \li \<none>
 *
 *	\par
 *	An integer.
 * 
 * \li l
 *
 *	\par
 *	A long.
 * 
 * \li ll
 *
 *	\par
 *	A long long.
 * 
 * \li L
 *
 *	\par
 *	A long double.
 * 
 * \li q
 *
 *	\par
 *	A long long (like ll).
 * 
 * \li j
 *
 *	\par
 *	An intmax_t.
 * 
 * \li z or Z
 *
 *	\par
 *	A size_t (the 'Z' is deprecated and should never be used).
 * 
 * \li t
 *
 *	\par
 *	A ptrdiff_t.
 * 
 * Conversions:
 * 
 * \li D,d,i
 *
 *	\par
 *	Writes out a decimal number. 'D' is deprecated. It can be
 *	used to write out a long value (i.e. use %ld instead).
 *	All the sizes, except 'L' can be used with this conversion.
 *	'#' has no effect.
 * 
 * \li U,u,O,o,X,x
 *
 *	\par
 *	Writes out an unsigned number in decimal (u,U), octal (o,O)
 *	or hexadecimal (x,X) notation. The U and O are deprecated.
 *	It can be used to write out a long value (i.e. use %lu
 *	and %lo instead). All the sizes, except the 'L' can be
 *	used with this conversion. '#' has no effect on the u and
 *	U conversions. With the o and O is ensures the value
 *	always starts with a zero. With the x and X is introduces
 *	the value with 0x and 0X respectively.
 * 
 * \li e,E,f,F,g,G,a,A
 *
 *	\par
 *	Writes out a floating point number. The 'e' and 'E' can
 *	be used to write number in scientific notation (always
 *	including an exponent). The 'f' can be used to always
 *	avoid an exponent (watch out for really large or
 *	small values!). The 'F' is deprecated and works like 'f'.
 *	The 'g' and 'G' will select between 'f' and 'e' or 'E'
 *	depending on the value to be written. When the value
 *	can be written in a reasonable width using 'f', then
 *	it is used. The 'a' and 'A' print out the value in
 *	hexadecimal floating point. In all cases, the capital
 *	version makes the exponent letter be written in capital.
 *	The only two length accepted are \<none> and 'L'.
 * 
 * li c,C
 *
 *	\par
 *	Writes out one character. The C notation is deprecated.
 *	It can be used to output of a long character (use %lc
 *	instead). The lengths accepted are 'hh' (char),
 *	'h' (mc_t), \<none> or 'll' (wc_t) and 'l' (wchar_t).
 * 
 * \li s,S
 *
 *	\par
 *	Writes out a string of characters. The S notation is
 *	deprecated. It can be used to output of a string of
 *	long characters (use %ls instead). The lengths accepted
 *	are 'hh' (char), 'h' (mc_t), \<none> or 'll' (wc_t) and
 *	'l' (wchar_t).
 * 
 * \li p,P
 *
 *	\par
 *	Writes out a pointer. The format used is "%#x" with the
 *	adequate size. The capital 'P' can be used to have the
 *	hexadecimal letters (and the 0X) in uppercase. This
 *	conversion doesn't accept a length.
 * 
 * \li n
 *
 *	\par
 *	Used to query the number of characters written at the
 *	point where this "conversion" appears. By default, the
 *	type expected for this parameter is an 'int'. You can
 *	use the length argument to change the type to any
 *	length except 'L'.
 * 
 * \li m
 *
 *	\par
 *	Writes out a string as returned by strerror(\<arg>);
 *	where \<arg> is an integer. The string will automatically
 *	be transformed to unicode as required.
 *
 * \bug
 * A common mistake is to type something like this:
 *
 * \code
 * moWCString	string;
 *
 * string.VFormat(...);
 * \endcode
 *
 * \par
 * in which case the result is lost. One needs to use the function
 * as a static function instead:
 *
 * \code
 * moWCString	string;
 *
 * string = moWCString::VFormat(...);
 * \endcode
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] encoding The encoding of the format string.
 * \param[in] args     An array of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::VFormat(const char *format, mowc::encoding_t encoding, va_list args)
{
	return VFormat(moWCString(format, encoding), args);
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] encoding The encoding of the format string.
 * \param[in] args     An array of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::VFormat(const mowc::mc_t *format, mowc::encoding_t encoding, va_list args)
{
	return VFormat(moWCString(format, encoding), args);
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] encoding The encoding of the format string.
 * \param[in] args     An array of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::VFormat(const mowc::wc_t *format, mowc::encoding_t encoding, va_list args)
{
	return VFormat(moWCString(format, encoding), args);
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] args     An array of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::VFormat(const wchar_t *format, va_list args)
{
	return VFormat(moWCString(format), args);
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] args     An array of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::VFormat(const moWCString& format, va_list args)
{
	int		r;
	moWCString	result;

	r = mowc::vfwprintf(0, format.f_string, args);	/* Flawfinder: ignore */
	if(r < 0) {		// totally invalid format string, can't deal with it!
		return result;
	}

	// size buffer properly and generate the string
	result.Size(r);
	result.f_length = vswprintf(result.f_string, result.f_max, format.f_string, args);	/* Flawfinder: ignore */

	return result;
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] encoding The encoding of the format string.
 * \param[in] ...      A set of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::Format(const char *format, mowc::encoding_t encoding, ...)
{
	va_list		args;
	moWCString	result;

	va_start(args, encoding);
	result = VFormat(moWCString(format, -1, encoding), args);
	va_end(args);

	return result;
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] encoding The encoding of the format string.
 * \param[in] ...      A set of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::Format(const mowc::mc_t *format, mowc::encoding_t encoding, ...)
{
	va_list		args;
	moWCString	result;

	va_start(args, encoding);
	result = VFormat(moWCString(format, -1, encoding), args);
	va_end(args);

	return result;
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] encoding The encoding of the format string.
 * \param[in] ...      A set of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::Format(const mowc::wc_t *format, mowc::encoding_t encoding, ...)
{
	va_list		args;
	moWCString	result;

	va_start(args, encoding);
	result = VFormat(moWCString(format, -1, encoding), args);
	va_end(args);

	return result;
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] ...      A set of arguments
 *
 * \return The formatted string
 */
moWCString moWCString::Format(const wchar_t *format, ...)
{
	va_list		args;
	moWCString	result;

	va_start(args, format);
	result = VFormat(moWCString(format), args);
	va_end(args);

	return result;
}


/** \brief Format a string with a standard C format string.
 *
 * Please, see VFormat(const char *format, mowc::encoding_t encoding, va_list args);
 * for more information.
 *
 * \param[in] format   The format string to apply against the arguments.
 * \param[in] ...      A set of arguments
 *
 * \return The formatted string
 */
#ifdef _MSC_VER
//
// Once again we must hand-hold VC++ because trying to pass a const-reference into va_start()
// really breaks and cannot figure out where it is on the argument stack. GCC, of course,
// does it right.
//
moWCString moWCString::Format( moWCString format, ... )
#else
moWCString moWCString::Format( const moWCString& format, ... )
#endif
{
	va_list		args;
	moWCString	result;

#ifdef _MSC_VER
	// Note: VC++ cannot handle the pointer (says it requires an l-value), where
	// the superior GCC handles it brilliantly. But I'm not biased or anything. :-)
	//
	va_start(args, format);
#else
	// The above generates a warning under gcc (must be first arg).
	//
	va_start(args, &format);
#endif
	result = VFormat(format, args);
	va_end(args);

	return result;
}










// documented in moBase
const char *moWCString::moWCStringCharacter::moGetClassName() const
{
	return "molib::moBase::moWCString::moWCStringCharacter";
}


/** \brief Initializes a character string object.
 *
 * This object is used as a reference to a string character.
 *
 * \param[in] string   The string being referenced
 * \param[in] index    The index of the character being referenced
 */
moWCString::moWCStringCharacter::moWCStringCharacter(moWCString *string, int index)
{
	f_string = string;
	f_index = index;
}


/** \brief Copy a string character object.
 *
 * This function copies a string character in another.
 *
 * \note
 * This function does not modify the string being referenced.
 *
 * \param[in] string_character   The source string character.
 */
moWCString::moWCStringCharacter::moWCStringCharacter(const moWCString::moWCStringCharacter& string_character)
	: moBase(string_character)
{
	f_string = string_character.f_string;
	f_index = string_character.f_index;
}


/** \brief Copy a string character object.
 *
 * This function copies a string character in another.
 *
 * If you need to copy an moWCStringCharacter in another,
 * you need to cast the right hand side to an mo::wc_t
 * character.
 *
 * \note
 * This function does not modify the string being referenced.
 * (i.e. it is not the character that is copied, but the
 * reference.)
 *
 * \param[in] string_character   The source string character.
 *
 * \return A reference to this string character object.
 */
moWCString::moWCStringCharacter& moWCString::moWCStringCharacter::operator = (const moWCString::moWCStringCharacter& string_character)
{
	if(this != &string_character) {
		f_string = string_character.f_string;
		f_index = string_character.f_index;
	}

	return *this;
}

/** \brief Copy a string character object.
 *
 * This operator copies the input character in this string character.
 *
 * This function writes in the string linked with this string character.
 *
 * \param c The character to save in the string.
 *
 * \return A reference to this string character object.
 */
moWCString::moWCStringCharacter& moWCString::moWCStringCharacter::operator = (mowc::wc_t c)
{
	f_string->Set(f_index, c);

	return *this;
}


/** \brief Read the string character.
 *
 * This cast operator returns a copy of the character being referenced
 * by this string character object.
 *
 * \return The character referenced.
 */
moWCString::moWCStringCharacter::operator mowc::wc_t () const
{
	return f_string->Get(f_index);
}



/** \brief Compare the string character with another character.
 *
 * This function reads the string character being referenced and
 * compares it with the specified character.
 *
 * \param[in] c   The character to compare with.
 *
 * \return true if both characters are the same.
 */
bool moWCString::moWCStringCharacter::operator == (mowc::wc_t c) const
{
	return f_string->Get(f_index) == c;
}


/** \brief Compare the string character with another character.
 *
 * This function reads the string character being referenced and
 * compares it with the specified character.
 *
 * \param[in] c   The character to compare with.
 *
 * \return true if both characters are not the same.
 */
bool moWCString::moWCStringCharacter::operator != (mowc::wc_t c) const
{
	return f_string->Get(f_index) != c;
}



/** \brief Compare the string character with another character.
 *
 * This function reads the string character being referenced and
 * compares it with the specified character.
 *
 * \param[in] c   The character to compare with.
 *
 * \return true if both string characters is smaller than the input character \p c.
 */
bool moWCString::moWCStringCharacter::operator < (mowc::wc_t c) const
{
	return f_string->Get(f_index) < c;
}



/** \brief Compare the string character with another character.
 *
 * This function reads the string character being referenced and
 * compares it with the specified character.
 *
 * \param[in] c   The character to compare with.
 *
 * \return true if both string characters is smaller or equal to the input character \p c.
 */
bool moWCString::moWCStringCharacter::operator <= (mowc::wc_t c) const
{
	return f_string->Get(f_index) <= c;
}



/** \brief Compare the string character with another character.
 *
 * This function reads the string character being referenced and
 * compares it with the specified character.
 *
 * \param[in] c   The character to compare with.
 *
 * \return true if both string characters is larger than the input character \p c.
 */
bool moWCString::moWCStringCharacter::operator > (mowc::wc_t c) const
{
	return f_string->Get(f_index) > c;
}



/** \brief Compare the string character with another character.
 *
 * This function reads the string character being referenced and
 * compares it with the specified character.
 *
 * \param[in] c   The character to compare with.
 *
 * \return true if both string characters is larger or equal to the input character \p c.
 */
bool moWCString::moWCStringCharacter::operator >= (mowc::wc_t c) const
{
	return f_string->Get(f_index) >= c;
}



/** \brief Prints a string in a specified output.
 *
 * This function is used to print out an moWCString in a
 * standard C++ stream (such as cerr or cout.)
 *
 * \param[in] output_stream  The stream where the string will be written.
 * \param[in] string         The string to write in the output.
 *
 * \return The output_stream reference.
 */
std::ostream& operator << (std::ostream& output_stream, const moWCString& string)
{
	// NOTE: at this time we print UTF-8 which may or may not be
	//	 compatible with the current terminal setup!?
	char buf[256];		/* Flawfinder: ignore */

	if(string.MBLength() < sizeof(buf) - 1) {
		// we use the stack, that's much faster
		output_stream << string.MBData(buf, sizeof(buf));
	}
	else {
		// our stack buffer is too small, use a dynamically allocated buffer
		output_stream << string.SavedMBData();;
	}

	return output_stream;
}


}			// namespace molib;

// vim: ts=8

