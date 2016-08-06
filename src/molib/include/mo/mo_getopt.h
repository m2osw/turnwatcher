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



#ifndef MO_GETOPT_H
#define	MO_GETOPT_H

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

class MO_DLL_EXPORT moGetOpt : public moBase
{
public:
	static const unsigned long	MO_GETOPT_NO_IDENTIFIER   = 0;
	static const unsigned long	MO_GETOPT_USER_IDENTIFIER = 1000;

	class moOption : public moBase
	{
	public:
					moOption(const moWCString& name, bool isdefault = false, unsigned long identifier = MO_GETOPT_NO_IDENTIFIER);

		virtual compare_t	Compare(const moBase& object) const;
		compare_t		CompareID(const moOption& oject) const;

		const moWCString	f_name;		// name of this option (for search purposes)
		long			f_integer;	// the result in the form of an integer
		double			f_float;	// the result in the form of a floating point
		moWCString		f_result;	// the value of this option on the command line (possibly a list of values comma separated)
		moList			f_list;		// when the result is a list this is a list of the items already separated
		const bool		f_default;	// whether the value is the default value or a user defined value
		const unsigned long	f_identifier;	// identifier of this option (for search purposes)

	private:
					// would have to copy the list otherwise!
					moOption(const moOption& option);
	};

	typedef moSmartPtr<moOption>	moOptionSPtr;

				moGetOpt(const moWCString& accept);
	virtual			~moGetOpt();

	virtual bool		OnError(const moWCString& argv, const moWCString& message);
	virtual void		OnHelp(void);
	virtual void		OnVersion(long count);
	virtual void		OnCopyright(void);

	void			SetVersion(const moWCString& version);
	void			SetProgname(const moWCString& progname);
	void			SetCopyright(const moWCString& copyright);

	virtual int		Main(int argc, const char *argv[]);
	// The following is not necessary with MinGW since the arguments
	// are already properly defined as the usual argc/agrv pair.
	//int			Main(const char *command_line);

	moOptionSPtr		Option(const moWCString& name) const;
	moOptionSPtr		Option(unsigned long identifier) const;
	moOptionSPtr		operator [] (const moWCString& name) const;
	moOptionSPtr		operator [] (int identifier) const;
	void			AddOption(const moOption& option);

private:
	class moAccept;
	typedef moSmartPtr<moAccept> moAcceptSPtr;
	class moAccept : public moBase
	{
	public:
		static const unsigned long	MO_ACCEPT_FOLLOW_EQUAL    = 0x0001;
		static const unsigned long	MO_ACCEPT_FOLLOW_STICK    = 0x0002;
		static const unsigned long	MO_ACCEPT_FOLLOW_INTEGER  = 0x0004;
		static const unsigned long	MO_ACCEPT_FOLLOW_FLOAT    = 0x0008;
		static const unsigned long	MO_ACCEPT_FOLLOW_STRING   = 0x0010;
		static const unsigned long	MO_ACCEPT_FOLLOW_MULTIPLE = 0x0020;
		static const unsigned long	MO_ACCEPT_FOLLOW_OPTIONAL = 0x0040;	// whether the :/= are optional
		static const unsigned long	MO_ACCEPT_FOLLOW_LIST     = 0x0080;

					moAccept(const moWCString& what, bool user, bool optional);
					moAccept(const moWCString& name);
					moAccept(unsigned long identifier);

		virtual compare_t	Compare(const moBase& object) const;
		compare_t		CompareID(const moBase& object) const;

		bool			HasIdentifier(void) const;
		bool			HasReference(void) const;
		bool			HasDefault(void) const;
		bool			Optional(void) const;
		int			Matches(const char *introducer, long length, const char *option) const;
		const moWCString&	Name(void) const;
		unsigned long		Identifier(void) const;
		unsigned long		Reference(void) const;
		void			Referenced(moAccept *accept);
		moAcceptSPtr		Referenced(void) const;
		moOptionSPtr		Option(moOptionSPtr opt, int argc, const char *argv[], long& idx, const char *left, long& max) const;
		moOptionSPtr		Default(void) const;

	protected:
					// don't need the copy operator
					moAccept(const moAccept& accept);

		int			f_incremental;	// increment by the value when this option is encountered
		bool			f_optional;	// if not true needs to be defined in the result
		bool			f_has_default;	// in case the default is empty we can still distinguish it
		bool			f_once;		// repeating the option generates an error
		bool			f_repeat;	// repeating the option creates multiple entries
		bool			f_direct;	// no flag is expected, is the parameter as is (file & numeric)
		bool			f_numeric;	// must be numerical
		unsigned long		f_follow;	// see the follow flags
		unsigned long		f_identifier;	// a value identifier (default to zero)
		unsigned long		f_reference;	// references to other moAccept entries
		unsigned long		f_minimum;	// minimum number of file entries (0 for not required)
		unsigned long		f_maximum;	// maximum number of file entries (0 for infinite)
		moWCString		f_introducer;	// one of ACCEPT_INTRODUCER_...
		moWCString		f_name;		// the name of the command line option
		moWCString		f_default;	// a default value when the user didn't give one
		moWCString		f_set;		// what we set the option to when it should be specified by the user but isn't (like in -V vs -V0)

		mutable moAcceptSPtr	f_ref;		// direct reference (avoid searching each time)
	};

	// copy operators not available (too much trouble!)
				moGetOpt(const moGetOpt& option);
	moGetOpt&		operator = (const moGetOpt& option);

	moWCString		f_version;
	moWCString		f_progname;
	moWCString		f_copyright;
	long			f_errcnt;	// count the number of errors
	moSortedList		f_options;	// the options found on the command line & these with default
	moSortedList		f_options_id;	// options sorted by identifiers
	moList			f_files;	// usually filenames, may be anything (list of moWCString objects)
	moSortedList		f_accept;	// a list of moAccept object used to parse the command line
	moSortedList		f_accept_id;	// the entries which have a non-zero identifier
};

typedef moSmartPtr<moGetOpt>	moGetOptSPtr;





};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_GETOPT_H

