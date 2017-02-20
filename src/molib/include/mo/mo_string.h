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



#ifndef MO_WCSTRING_H
#define	MO_WCSTRING_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_WCSTR_H
#include	"mo_str.h"
#endif


namespace molib
{


class MO_DLL_EXPORT moWCString;

typedef moSmartPtr<moWCString>				moWCStringSPtr;
typedef moTmplList<moWCString, moList>			moListOfWCStrings;
typedef moTmplList<moWCString, moSortedList>		moSortedListOfWCStrings;
typedef moTmplList<moWCString, moListUnique>		moListUniqueOfWCStrings;
typedef moTmplList<moWCString, moSortedListUnique>	moSortedListUniqueOfWCStrings;


class MO_DLL_EXPORT moWCString : public moBase
{
public:
	class MO_DLL_EXPORT moWCStringCharacter : public moBase
	{
	public:
					moWCStringCharacter(moWCString *string, int index);
					moWCStringCharacter(const moWCStringCharacter& string_character);
		moWCStringCharacter&	operator = (const moWCStringCharacter& string_character);

		virtual const char *	moGetClassName(void) const;

		moWCStringCharacter&	operator = (mowc::wc_t c);
					operator mowc::wc_t (void) const;
		bool			operator == (mowc::wc_t c) const;
		bool			operator != (mowc::wc_t c) const;
		bool			operator < (mowc::wc_t c) const;
		bool			operator <= (mowc::wc_t c) const;
		bool			operator > (mowc::wc_t c) const;
		bool			operator >= (mowc::wc_t c) const;

	private:
		moWCStringSPtr		f_string;
		int			f_index;
	};

	static const unsigned long	WC_STRING_BOOLEAN_TEST_INTEGER_C   = 0x0001;	// 0 = false, else true
	static const unsigned long	WC_STRING_BOOLEAN_TEST_INTEGER_ADA = 0x0002;	// 0 = false, 1 = true, other error
	static const unsigned long	WC_STRING_BOOLEAN_TEST_FLOAT       = 0x0004;	// 0.0 = false, else true
	static const unsigned long	WC_STRING_BOOLEAN_TEST_NAMED       = 0x0008;	// "false" = false, "true" = true
	static const unsigned long	WC_STRING_BOOLEAN_TEST_LETTER      = 0x0010;	// "f" = false, "t" = true
	static const unsigned long	WC_STRING_BOOLEAN_TEST_TEXT        = 0x0020;	// "" = false, else true
	static const unsigned long	WC_STRING_BOOLEAN_TEST_YESNO       = 0x0040;	// "no" = false, "yes" true

	static const unsigned long	WC_STRING_CLIP_START   = 0x0001;
	static const unsigned long	WC_STRING_CLIP_END     = 0x0002;
	static const unsigned long	WC_STRING_CLIP_NEWLINE = 0x0004;	// '\r' & '\n' are stripped too
	static const unsigned long	WC_STRING_CLIP_BOTH    = (WC_STRING_CLIP_START | WC_STRING_CLIP_END);

				moWCString(void);
				moWCString(const char *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
				moWCString(const mowc::mc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
				moWCString(const mowc::wc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
				moWCString(const wchar_t *str, int length = -1);	// encoding always as defined in the compiler!
				moWCString(const moWCString& str, int length = -1);
	virtual			~moWCString();

	virtual const char *	moGetClassName(void) const;
	virtual size_t		SizeOf(void) const;

	moWCString&		operator = (const moWCString& str);
	moWCString&		operator = (const char *str);
	moWCString&		operator = (const mowc::mc_t *str);
	moWCString&		operator = (const mowc::wc_t *str);
	moWCString&		operator = (const wchar_t *str);
	moWCString&		operator += (const moWCString& string);
	moWCString&		operator += (const char *str);
	moWCString&		operator += (const mowc::mc_t *str);
	moWCString&		operator += (const mowc::wc_t *str);
	moWCString&		operator += (const wchar_t *str);
	moWCString&		operator += (char c);
	moWCString&		operator += (mowc::mc_t c);
	moWCString&		operator += (mowc::wc_t c);
	moWCString&		operator += (wchar_t c);
	moWCString		operator + (const moWCString& string) const;
	moWCString		operator + (const char *str) const;
	moWCString		operator + (const mowc::mc_t *str) const;
	moWCString		operator + (const mowc::wc_t *str) const;
	moWCString		operator + (const wchar_t *str) const;
	friend moWCString	operator + (const char *str, const moWCString& string)
				{ // gcc 3.2 doesn't take the plain definition, but the inline works
					moWCString s(str);
					return s.Append(string);
				}
	friend moWCString	operator + (const wchar_t *str, const moWCString& string)
				{ // gcc 3.2 doesn't take the plain definition, but the inline works
					moWCString s(str);
					return s.Append(string);
				}
	moWCString		operator + (char c) const;
	moWCString		operator + (mowc::mc_t c) const;
	moWCString		operator + (mowc::wc_t c) const;
	moWCString		operator + (wchar_t c) const;
	moWCStringCharacter	operator [] (int index);
	mowc::wc_t		operator [] (int index) const;
	bool			operator == (const moBase& object) const;
	bool			operator != (const moBase& object) const;
	bool			operator <  (const moBase& object) const;
	bool			operator <= (const moBase& object) const;
	bool			operator >  (const moBase& object) const;
	bool			operator >= (const moBase& object) const;
	bool			operator == (const char *str) const;
	bool			operator != (const char *str) const;
	bool			operator <  (const char *str) const;
	bool			operator <= (const char *str) const;
	bool			operator >  (const char *str) const;
	bool			operator >= (const char *str) const;
	bool			operator == (const mowc::mc_t *str) const;
	bool			operator != (const mowc::mc_t *str) const;
	bool			operator <  (const mowc::mc_t *str) const;
	bool			operator <= (const mowc::mc_t *str) const;
	bool			operator >  (const mowc::mc_t *str) const;
	bool			operator >= (const mowc::mc_t *str) const;
	bool			operator == (const mowc::wc_t *str) const;
	bool			operator != (const mowc::wc_t *str) const;
	bool			operator <  (const mowc::wc_t *str) const;
	bool			operator <= (const mowc::wc_t *str) const;
	bool			operator >  (const mowc::wc_t *str) const;
	bool			operator >= (const mowc::wc_t *str) const;
	bool			operator == (const wchar_t *str) const;
	bool			operator != (const wchar_t *str) const;
	bool			operator <  (const wchar_t *str) const;
	bool			operator <= (const wchar_t *str) const;
	bool			operator >  (const wchar_t *str) const;
	bool			operator >= (const wchar_t *str) const;

	void			Password(void);

	bool			IsEmpty(void) const;
	moWCString&		Empty(void);
	moWCString&		Clear(void);
	size_t			ByteSize(void) const;
	size_t			Length(void) const;
	size_t			MBLength(void) const;
	size_t			CurrentMaxLength(void) const;

	// access the string data
	const mowc::wc_t *	Data(void) const;
	char *			MBData(char *string = 0, size_t size = 0) const;
	const char *		SavedMBData(void) const;
	mowc::wc_t		First(void) const;
	mowc::wc_t		Last(void) const;
	mowc::wc_t		Get(int index) const;
	moWCString		Get(int from, int to) const;

	// modify the string
	void			Set(int index, char c, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	void			Set(int index, mowc::mc_t c, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	void			Set(int index, mowc::wc_t c, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	void			Set(int index, wchar_t c);
	void			Set(const char *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	void			Set(const mowc::mc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	void			Set(const mowc::wc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	void			Set(const wchar_t *str, int length = -1);

	moWCString		Delete(unsigned int from, unsigned int to) const;

	moWCString		Insert(const char *str, unsigned int where = (unsigned int) -1, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8) const;
	moWCString		Insert(const mowc::mc_t *str, unsigned int where = (unsigned int) -1, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL) const;
	moWCString		Insert(const mowc::wc_t *str, unsigned int where = (unsigned int) -1, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL) const;
	moWCString		Insert(const wchar_t *str, unsigned int where = (unsigned int) -1, int length = -1) const;
	moWCString		Insert(const moWCString& string, unsigned int where = (unsigned int) -1, unsigned int pos = 0, int length = -1) const;

	moWCString		Concat(const char *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8) const;
	moWCString		Concat(const mowc::mc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL) const;
	moWCString		Concat(const mowc::wc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL) const;
	moWCString		Concat(const wchar_t *str, int length = -1) const;
	moWCString		Concat(const moWCString& string, unsigned int pos = 0, int length = -1) const;

	moWCString&		Append(const char *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	moWCString&		Append(const mowc::mc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
	moWCString&		Append(const mowc::wc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
	moWCString&		Append(const wchar_t *str, int length = -1);
	moWCString&		Append(const moWCString& string, unsigned int pos = 0, int length = -1);

	// compare strings together
	virtual compare_t	Compare(const moBase& object) const;
	compare_t		Compare(const moWCString& string, unsigned int pos = 0, int length = -1) const;
	compare_t		Compare(const char *str, unsigned int pos = 0, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8) const;
	compare_t		Compare(const mowc::mc_t *str, unsigned int pos = 0, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL) const;
	compare_t		Compare(const mowc::wc_t *str, unsigned int pos = 0, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL) const;
	compare_t		Compare(const wchar_t *str, unsigned int pos = 0, int length = -1) const;
	compare_t		CaseCompare(const moWCString& string, unsigned int pos = 0, int length = -1) const;
	compare_t		CaseCompare(const char *str, unsigned int pos = 0, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8) const;
	compare_t		CaseCompare(const mowc::mc_t *str, unsigned int pos = 0, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL) const;
	compare_t		CaseCompare(const mowc::wc_t *str, unsigned int pos = 0, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL) const;
	compare_t		CaseCompare(const wchar_t *str, unsigned int pos = 0, int length = -1) const;
	moWCString		SoundEx(bool full = false) const;

	// test strings for matching patterns
	// (see the moRegExpr for a full regular expression pattern matching)
	bool			Glob(const moWCString& pattern) const;
//#ifndef NO_MOSTRING_MATCH
	bool			Match(const moWCString& pattern) const;
//#endif

	// convertion to numbers
	bool			IsInteger(int base = 0) const;
	int32_t			Integer(int base = 0) const;
	int64_t			LargeInteger(int base = 0) const;
	bool			IsFloat(void) const;
	double			Float(void) const;
	int			IsTrue(unsigned long flags) const;
	int			IsFalse(unsigned long flags) const;
	bool			IPv4(unsigned long& address, unsigned short& port, unsigned long& mask) const;

	// filename related functions
	bool			FilenameHasExtension(void) const;
	moWCString		FilenameExtension(void) const;
	moWCString		FilenameBasename(const moWCString& extension = "") const;
	moWCString		FilenameDirname(void) const;
	moWCString		FilenameChild(const moWCString& child) const;
	bool			FilenameExpand(moListOfWCStrings& list, bool skip_variables = false) const;
	moWCString		FilenameClean(void) const;
	moWCString		NoNewLine(void) const;

	// string management
	int			ConvertFromEncoding(const moWCString& encoding);
	void			Strip(unsigned long sides = WC_STRING_CLIP_BOTH);
	moWCString		Clip(unsigned long sides = WC_STRING_CLIP_BOTH) const;
	moWCString		Replace(const moWCString& what) const;
	moWCString		Transpose(const moWCString& source, const moWCString& destination) const;
	moWCString		FromBackslash(void) const;
	moWCString		FindChar(mowc::wc_t c, long position = 0, long length = -1) const;
	moWCString		FindRChar(mowc::wc_t c, long position = -1, long length = -1) const;
	long			FindAny(const moWCString& string, long position = 0, long length = -1) const;
	long			FindRAny(const moWCString& string, long position = LONG_MAX, long length = -1) const;
	long			FindInString(const moWCString& string, long position = 0, long length = -1) const;
	long			FindInCaseString(const moWCString& string, long position = 0, long length = -1) const;
	long			FindRInString(const moWCString& string, long position = 0, long length = -1) const;
	long			FindRInCaseString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString		FindString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString		FindCaseString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString		FindRString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString		FindRCaseString(const moWCString& string, long position = 0, long length = -1) const;
	moWCString		Uppercase(void) const;
	moWCString		Lowercase(void) const;
	moWCString		Switchcase(void) const;
	moWCString		CapitalizeWords(void) const;
	moWCString		Capitalize(void) const;
	moWCString&		Reverse(void);

	static moWCString	VFormat(const char *format, mowc::encoding_t encoding, va_list args);
	static moWCString	VFormat(const mowc::mc_t *format, mowc::encoding_t encoding, va_list args);
	static moWCString	VFormat(const mowc::wc_t *format, mowc::encoding_t encoding, va_list args);
	static moWCString	VFormat(const wchar_t *format, va_list args);
	static moWCString	VFormat(const moWCString& format, va_list args);
	static moWCString	Format(const char *format, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8, ...);
	static moWCString	Format(const mowc::mc_t *format, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL, ...);
	static moWCString	Format(const mowc::wc_t *format, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL, ...);
	static moWCString	Format(const wchar_t *format, ...);
#ifdef _MSC_VER
	static moWCString	Format( moWCString format, ... );
#else
	static moWCString	Format( const moWCString& format, ... );
#endif

	static int		vswprintf(mowc::wc_t *str, size_t max, const mowc::wc_t *format, va_list args);
	static int		swprintf(mowc::wc_t *str, size_t max, const mowc::wc_t *format, ...);

	const mowc::wc_t *	wc_str(void) const { return Data(); }
	const char *		c_str(void) const { return SavedMBData(); }

private:
	void			Init(void);
	void			Size(int length);

	zbool_t			f_password;
	mutable zbool_t		f_string_changed; // whether SavedMBData() needs to recompute f_mb_string;
	zsize_t			f_length;
	size_t			f_max;
	mowc::wc_t *		f_string;	// always UTF32 internal
	mowc::wc_t		f_data[64];	// until string requires more than 64 chars (including the nul)
	mutable char *		f_mb_string;	// WARNING: if f_string changes, f_mb_string needs to be recomputed...
};



// "The" empty string (note that other strings can also be empty!)
// Use whenever you need to return an empty string to avoid the
// constructor/destructor every time!
MO_DLL_EXPORT extern const moWCString		g_empty_string;



MO_DLL_EXPORT extern std::ostream& operator << (std::ostream& output_stream, const moWCString& string);


};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_WCSTRING_H

