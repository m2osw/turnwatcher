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
#pragma implementation "mo/mo_getopt.h"
#endif

#include	"mo/mo_getopt.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{


/************************************************************ DOC:

CLASS

	moOption

NAME

	Constructors - create an option object

SYNOPSIS

	moOption(const moWCString& name, bool isdefault = false,
					unsigned long identifier = 0);

PARAMETERS

	name - the name of this option
	isdefault - whether this option is given a default value
	identifier - the identifier of the option

DESCRIPTION

	Options found on the command line are saved in an moOption
	object. This object has a name (the option name as described
	in the moGetOpt object), a value (the value found on the
	command line or the default value), a flag determining
	whether the default value is used or not and an identifier
	(MO_GETOPT_NO_IDENTIFIER by default).

	All of these can be tested by the end programmers using
	the following read-only fields:

		f_name
		f_identifier
		f_default

	The value of the option is saved in a read-write set of
	values, a string and when necessary in a list defined
	as follow:

		f_integer
		f_float
		f_result
		f_list		-- option used several times

	The result & value can be modified at any time so as to
	provide the necessary behavior. The f_integer is a 'long'
	value and the f_float is a 'double'. The f_result is
	the same value defined in a string. f_integer and f_float
	will usually be set to -1 or 0 when f_result doesn't
	represent a value. The f_list will be used when the option
	can be accepted more than once (see the '*'/multiple flags).
	The objects in the f_list are only moWCString objects.

	Internally, the moOption() objects are sorted by name and
	identifiers.

*/
moGetOpt::moOption::moOption(const moWCString& name, bool isdefault, unsigned long identifier)
	: f_name(name),
	  f_default(isdefault),
	  f_identifier(identifier)
{
	f_integer = 0;
	f_float = 0.0;
}



moGetOpt::moOption::moOption(const moOption& option)
	: moBase(option),
	  f_name(option.f_name),
	  f_default(option.f_default),
	  f_identifier(option.f_identifier)
{
	f_integer = option.f_integer;
	f_float = option.f_float;
}



/************************************************************ DOC:

CLASS

	private:
	moGetOpt::moAccept

NAME

	Compare - compare accepted options to sort them for fast access

SYNOPSIS

	virtual compare_t Compare(const moBase& object) const;
	compare_t CompareID(const moAccept& object) const;

PARAMETERS

	object - another moAccept object

DESCRIPTION

	Determine whether an accepted option is larger, smaller or equal to
	another accepted option.

RETURN VALUES

	These functions will return the comparison result as one of:

		MO_BASE_COMPARE_EQUAL
		MO_BASE_COMPARE_SMALLER
		MO_BASE_COMPARE_GREATER

*/
moBase::compare_t moGetOpt::moAccept::Compare(const moBase& object) const
{
	return f_name.Compare(dynamic_cast<const moAccept&>(object).f_name);
}

moBase::compare_t moGetOpt::moAccept::CompareID(const moBase& object) const
{
	const moAccept& accept = dynamic_cast<const moAccept&>(object);
	return CompareInt(f_identifier, accept.f_identifier);
}



/************************************************************ DOC:

CLASS

	private:
	moGetOpt::moAccept

NAME

	HasIdentifier -- check whether the identifier was defined
	HasReference -- check whether the reference was defined

SYNOPSIS

	bool HasIdentifier(void) const;
	bool HasReference(void) const;

DESCRIPTION

	This function returns true if the accept object has a
	non-zero identifier.

*/
bool moGetOpt::moAccept::HasIdentifier(void) const
{
	return f_identifier != MO_GETOPT_NO_IDENTIFIER;
}

bool moGetOpt::moAccept::HasReference(void) const
{
	return f_reference != MO_GETOPT_NO_IDENTIFIER;
}

bool moGetOpt::moAccept::HasDefault(void) const
{
	return f_has_default;
}

bool moGetOpt::moAccept::Optional(void) const
{
	return f_optional;
}



/************************************************************ DOC:

CLASS

	private:
	moGetOpt::moAccept

NAME

	Name -- return the name of the of the moAccept object

SYNOPSIS

	const moWCString& Name(void) const;
	const unsigned long Identifier(void) const;
	const unsigned long Reference(void) const;
	void Referenced(moAccept *accept);
	moAccept *Referenced(void) const;

DESCRIPTION

	These functions return the name, identifier and
	reference of the moAccept object respectively.

*/
const moWCString& moGetOpt::moAccept::Name(void) const
{
	return f_name;
}


unsigned long moGetOpt::moAccept::Identifier(void) const
{
	return f_identifier;
}


unsigned long moGetOpt::moAccept::Reference(void) const
{
	return f_reference;
}


void moGetOpt::moAccept::Referenced(moAccept *accept)
{
	f_ref = accept;
}


moGetOpt::moAcceptSPtr moGetOpt::moAccept::Referenced(void) const
{
	return f_ref;
}


/************************************************************ DOC:

CLASS

	moGetOpt::moOption

NAME

	Compare - compare options to sort them for fast access

SYNOPSIS

	virtual compare_t Compare(const moBase& object) const;
	compare_t CompareID(const moOption& option) const;

PARAMETERS

	object - an moOption object
	option - an option object

DESCRIPTION

	Determine whether an option is larger, smaller or equal to
	another option.

RETURN VALUE

	These function return one of:

		MO_BASE_COMPARE_EQUAL
		MO_BASE_COMPARE_SMALLER
		MO_BASE_COMPARE_GREATER

*/
moBase::compare_t moGetOpt::moOption::Compare(const moBase& object) const
{
	return f_name.Compare(dynamic_cast<const moOption&>(object).f_name);
}

moBase::compare_t moGetOpt::moOption::CompareID(const moOption& option) const
{
	return f_identifier == option.f_identifier ? MO_BASE_COMPARE_EQUAL :
		(f_identifier < option.f_identifier ? MO_BASE_COMPARE_SMALLER : MO_BASE_COMPARE_GREATER);
}





/************************************************************ DOC:

CLASS

	private:
	moGetOpt::moAccept

NAME

	Constructor - parse the accepted string

SYNOPSIS

	moAccept(const moWCString& what, bool optional);

PARAMETERS

	what - the option to be parsed now
	optional - force the optional flag to true if true

DESCRIPTION

	This class is used internally to manage options.

	Please, see the moGetOpt constructor for more information
	about the strings.

NOTES

	When this object is created, the input strings have no
	semi-colons (;).

SEE ALSO

	

*/
moGetOpt::moAccept::moAccept(const moWCString& what, bool user, bool optional)
{
	const mowc::wc_t	*s, *e, *n;
	mowc::wc_t		c, quote, *next;
	bool			has_default, equal;
	unsigned long		r;

	f_ref = 0;

/* get the start and end of the input string */
	s = what.Data();
	e = s + what.Length();

/* skip spaces */
	while(mowc::isspace(*s)) {
		s++;
	}
	while(e > s && mowc::isspace(e[-1])) {
		e--;
	}

/* get the identifier, if there is one */
	if(*s >= '0' && *s <= '9') {
		errno = 0;
		f_identifier = mowc::strtol(s, &next, 10);
		if((f_identifier < MO_GETOPT_USER_IDENTIFIER && user)
		|| f_identifier == MO_GETOPT_NO_IDENTIFIER
		|| errno != 0) {
std::cerr << "ERROR: moGetOpt::moAccept(): invalid id #" << f_identifier
	<< ", identifiers cannot be null or out of range."
	<< std::endl;
			throw moBug(MO_ERROR_INVALID, "Invalid identifier number");
		}
		s = next;

		/* skip spaces */
		while(mowc::isspace(*s)) {
			s++;
		}
	}
	else {
		f_identifier = MO_GETOPT_NO_IDENTIFIER;
	}

/* get reference */
	if(s[0] == '-' && s[1] == '>') {
		if(f_identifier == MO_GETOPT_NO_IDENTIFIER) {
std::cerr << "ERROR: moGetOpt::moAccept(): references are accepted only when an identifier is given." << std::endl;
			throw moBug(MO_ERROR_INVALID, "References work with identifiers only");
		}
		s += 2;
		errno = 0;
		r = mowc::strtol(s, &next, 10);
		if((r < MO_GETOPT_USER_IDENTIFIER && user)
		|| r == MO_GETOPT_NO_IDENTIFIER
		|| errno != 0) {
std::cerr << "ERROR: moGetOpt::moAccept(): invalid ref #" << r
		<< ", references cannot be null or out of range." << std::endl;
			throw moBug(MO_ERROR_INVALID, "References cannot be zero or our of range.");
		}
		f_reference = r;
		s = next;
		while(mowc::isspace(*s)) {
			s++;
		}
	}
	else {
		f_reference = MO_GETOPT_NO_IDENTIFIER;
	}

	if(e == s) {
std::cerr << "ERROR: moGetOpt::moAccept(): empty options are not acceptable." << std::endl;
		throw moBug(MO_ERROR_INVALID, "Empty options are not acceptable");
	}

/* optional entries are defined between '[' ... ']' */
	f_optional = *s == '[';
	if(f_optional) {
		if(e[-1] != ']') {
			// this error will be fixed by programmers and
			// will never happen to end users
std::cerr << "ERROR: moGetOpt::moAccept(): invalid optional command line option definition (end square brackets (]) not found)." << std::endl;
			throw moBug(MO_ERROR_INVALID, "Optional option improperly defined");
		}
		e--;
		s++;

		/* skip spaces */
		while(mowc::isspace(*s)) {
			s++;
		}
		while(e > s && mowc::isspace(e[-1])) {
			e--;
		}
		if(e == s) {
std::cerr << "ERROR: moGetOpt::moAccept(): empty optional options are not acceptable." << std::endl;
			throw moBug(MO_ERROR_INVALID, "Empty optional options are not acceptable");
		}
	}

	/* parse the introducer */
	n = s;
	c = *n;
	while(n < e && (c == '-' || c == '+' || c == '|' || c == '/')) {
		c = *++n;
	}
	f_introducer = moWCString(s, static_cast<int>(n - s));
	s = n;

	/* skip spaces after the introducer (there shouldn't be any though) */
	while(mowc::isspace(*s)) {
		s++;
	}

	/* run through the name of the option */
	n = s;
	c = *n;
	while(n < e && c != '=' && !mowc::isspace(c)) {
		c = *++n;
	}
	if(n == s) {
std::cerr << "ERROR: moGetOpt::moAccept(): empty names are not acceptable." << std::endl;
		throw moBug(MO_ERROR_INVALID, "Empty names are not acceptable");
	}
	f_name = moWCString(s, static_cast<int>(n - s));
	s = n;

	/* skip spaces */
	while(mowc::isspace(*s)) {
		s++;
	}

	/* has a default value? */
	has_default = equal = *s == '=';
	if(has_default) {
		do {
			s++;
		} while(s < e && mowc::isspace(*s));
	}

		/* not true if =:value or :=value|string */
	if(has_default && s[1] == ':') {
		has_default = false;
	}
	if(has_default && mowc::strcasecmp(s + 1, "value", 5) == 0) {
		has_default = false;
	}
	if(has_default && mowc::strcasecmp(s + 1, "string", 6) == 0) {
		has_default = false;
	}

	if(has_default) {
		equal = false;
		n = s;
		c = *n;
		if(c == '\'' || c == '"' || c == '`') {
			s++;		/* we don't want quote in the default value */
			quote = c;
			do {
				c = *++n;
				if(n < e && c == '\\') {
					/* don't check this as quotation */
					n++;
				}
			} while(n < e && c != quote);
			if(c != quote) {
std::cerr << "ERROR: moGetOpt::moAccept(): invalid default value quotation." << std::endl;
				throw moBug(MO_ERROR_INVALID, "Invalid default value quotation");
			}
		}
		else {
			while(n < e && !mowc::isspace(c)) {
				c = *++n;
			}
			// make sure quote != c
			quote = (char) (c + 1);
		}
		f_default = moWCString(s, static_cast<int>(n - s));
		f_has_default = true;
		s = n;			/* skip the value */
		if(c == quote) {
			s++;		/* and the quote when there is one */
		}
		while(s < e && mowc::isspace(*s)) {
			s++;
		}
	}
	else {
		f_has_default = false;
	}

/* set it now so the special flag 'required' can be used */
	if(optional) {
		f_optional = true;
	}

/* check options now */
	static const char *flags[] = {
		"?", "optional",
		"!", "required",

		"O", "!",
		"O", "once",

		"*", "*",
		"*", "repeat",

		"+", "++",
		"+", "inc",
		"+", "increment",
		"+", "incremental",

		"-", "--",
		"-", "dec",
		"-", "decrement",
		"-", "decremental",

		"T", "on",
		"T", "yes",
		"T", "1",
		"T", "true",
		"T", "use",

		".", "no",	// can be "no" or "no flag"

		"F", "off",
		"F", "0",
		"F", "false",
		"F", "don't",	// "... use"

		":", "equal",
		"=", "set",	// "... to"

		"i", "value",
		"I", "values",
		"i", "integer",
		"I", "integers",
		"i", "int",
		"I", "ints",

		"r", "float",
		"R", "floats",
		"r", "double",
		"R", "doubles",
		"r", "real",
		"R", "reals",

		"s", "string",
		"S", "strings",

		"f", "file",

		"n", "numeric",
		"m", "min",
		"m", "minimum",
		"M", "max",
		"M", "maximum",

		0
	};
	const char	**l;
	unsigned long	follow;

	f_incremental = 0;
	f_minimum = 0;
	f_maximum = 0;
	f_repeat = false;
	f_once = false;
	f_direct = false;
	f_follow = 0;
	while(s < e && *s != '\0') {
		/* get the [:=] option introducer */
		follow = *s == '[' ? s++, (unsigned long) MO_ACCEPT_FOLLOW_OPTIONAL : 0;
		/* check if we got the equal taken away for the default value */
		if(follow == 0 && equal) {
			follow |= MO_ACCEPT_FOLLOW_EQUAL;
		}
		equal = false;
		while(s < e) {
			switch(*s) {
			case ':':
				follow |= MO_ACCEPT_FOLLOW_STICK;
				s++;
				continue;

			case '=':
				follow |= MO_ACCEPT_FOLLOW_EQUAL;
				s++;
				continue;

// put in the list but don't use multiple on the same parameter
			case '!':
				follow |= MO_ACCEPT_FOLLOW_LIST;
				s++;
				continue;

			case ' ':
			case '\t':
			case '\r':
			case '\n':
				continue;

			}
			break;
		}
		if(s < e && *s == ']') {
			if((follow & MO_ACCEPT_FOLLOW_OPTIONAL) == 0) {
std::cerr << "ERROR: moGetOpt::moAccept(): option '" << f_name << "' uses an invalid ']' in its options." << std::endl;
				throw moBug(MO_ERROR_INVALID, "Unexpected ']' in an option definition");
			}
			s++;
		}
		else if((follow & MO_ACCEPT_FOLLOW_OPTIONAL) != 0) {
std::cerr << "ERROR: moGetOpt::moAccept(): cannot find the corresponding ']' in option '" << f_name << "'." << std::endl;
			throw moBug(MO_ERROR_INVALID, "Missing ']' in option");
		}

		/* skip spaces after the ':', '=' or ']' */
		while(s < e && mowc::isspace(*s)) {
			s++;
		}

		n = s;
		while(n < e && !mowc::isspace(*n)) {
			n++;
		}
		if(n == s) {
std::cerr << "ERROR: moGetOpt::moAccept(): option '" << f_name << "' tried to declare an option without a name." << std::endl;
			throw moBug(MO_ERROR_INVALID, "An option without a name is not acceptable");
		}
		l = flags;
		while(*l != 0) {
			r = static_cast<unsigned long>(strlen(l[1]));	/* Flawfinder: ignore */
			if(r == static_cast<size_t>(n - s)
			&& mowc::strcasecmp(s, l[1], r) == 0) {
				switch(l[0][0]) {
				case '?':	/* optional */
					f_optional = true;
					break;

				case '!':	/* required */
					f_optional = false;
					break;

				case '+':	/* ++ */
					f_incremental = 1;
					break;

				case '-':	/* -- */
					f_incremental = -1;
					break;

				case 'T':	/* on */
					f_set = "1";
					break;

				case 'D':	/* don't use */
					while(n < e && mowc::isspace(*n)) {
						n++;
					}
					if(mowc::strcasecmp(n, "use", 3) != 0
					|| (!mowc::isspace(n[3]) && n[3] != '\0')) {
						goto badoption;
					}
				case 'F':	/* off */
					f_set = "0";
					break;

				case '=':	/* set to <value> */
					while(n < e && mowc::isspace(*n)) {
						n++;
					}
					if(mowc::strcasecmp(n, "to", 2) != 0
					|| (!mowc::isspace(n[2]) && n[2] != '\0')) {
						goto badoption;
					}
					n += 2;
				case ':':	/* equal */
					while(n < e && mowc::isspace(*n)) {
						n++;
					}
					s = n;
					while(n < e && !mowc::isspace(*n)) {
						n++;
					}
					f_set = moWCString(s, static_cast<int>(n - s));
					break;

				case 'O':
					f_once = true;
					break;

				case '*':
					f_repeat = true;
					break;

				case 'i':	/* integer */
					f_follow |= MO_ACCEPT_FOLLOW_INTEGER
						| follow;
					break;

				case 'I':	/* integers */
					f_follow |= MO_ACCEPT_FOLLOW_INTEGER
						| MO_ACCEPT_FOLLOW_MULTIPLE
						| follow;
					break;

				case 'r':	/* float */
					f_follow |= MO_ACCEPT_FOLLOW_FLOAT
						| follow;
					break;

				case 'R':	/* floats */
					f_follow |= MO_ACCEPT_FOLLOW_FLOAT
						| MO_ACCEPT_FOLLOW_MULTIPLE
						| follow;
					break;

				case 's':	/* string */
					f_follow |= MO_ACCEPT_FOLLOW_STRING
						| follow;
					break;

				case 'S':	/* strings */
					f_follow |= MO_ACCEPT_FOLLOW_STRING
						| MO_ACCEPT_FOLLOW_MULTIPLE
						| follow;
					break;

				case '.':	/* no | no flag */
					while(n < e && mowc::isspace(*n)) {
						n++;
					}
					if(mowc::strcasecmp(n, "flag", 4) != 0
					|| (!mowc::isspace(n[4]) && n[4] != '\0')) {
						// if not "no flag" it's "no"
						f_set = "0";
						break;
					}
					n += 4;
				case 'f':	/* file */
					f_direct = true;
					break;

				case 'n':	/* numeric */
					f_direct = true;
					f_numeric = true;
					break;

				case 'm':	/* minimum */
					while(n < e && mowc::isspace(*n)) {
						n++;
					}
					s = n;
					while(n < e && !mowc::isspace(*n)) {
						n++;
					}
					f_minimum = moWCString(s, static_cast<int>(n - s)).Integer();
					break;

				case 'M':	/* maximum */
					while(n < e && mowc::isspace(*n)) {
						n++;
					}
					s = n;
					while(n < e && !mowc::isspace(*n)) {
						n++;
					}
					f_maximum = moWCString(s, static_cast<int>(n - s)).Integer();
					break;

				}
				break;
			}
			l += 2;
		}
		if(*l == 0) {
badoption:
std::cerr << "ERROR: moGetOpt::moAccept(): unrecognized option(s) '" << s << "' for '" << f_name << "'." << std::endl;
			throw moBug(MO_ERROR_INVALID, "Unrecognized option(s)");
		}
		s = n;
		while(mowc::isspace(*s)) {
			s++;
		}
	}

#if 0
char tmpbuf[256];	/* Flawfinder: ignore */
printf("moAccept() =\n");
printf("   f_incremental = %d\n", f_incremental);
printf("   f_optional = %d\n", f_optional);
printf("   f_repeat = %d\n", f_repeat);
printf("   f_once = %d\n", f_once);
printf("   f_has_default = %d\n", f_has_default);
printf("   f_follow = %ld\n", f_follow);
printf("   f_identifier = %ld\n", f_identifier);
printf("   f_reference = %ld\n", f_reference);
printf("   f_introducer = %s\n", f_introducer.MBData(tmpbuf, sizeof(tmpbuf)));
printf("   f_name = %s\n", f_name.MBData(tmpbuf, sizeof(tmpbuf)));
printf("   f_default = %s\n", f_default.MBData(tmpbuf, sizeof(tmpbuf)));
printf("   f_set = %s\n", f_set.MBData(tmpbuf, sizeof(tmpbuf)));
#endif
}


moGetOpt::moAccept::moAccept(const moWCString& name)
{
	f_name = name;
	f_identifier = MO_GETOPT_NO_IDENTIFIER;
}

moGetOpt::moAccept::moAccept(unsigned long identifier)
{
	f_name = "unknown";
	f_identifier = identifier;
}





/************************************************************ DOC:

CLASS

	private:
	moAccept

NAME

	Matches - check to know whether an option entry matches

SYNOPSIS

	int Matches(const char *introducer, long length, const char *option) const;

PARAMETERS

	option - the command line option to compare with

DESCRIPTION

	This function will try to match the option to the moAccept
	object entry.

	It first compares the option introducer to the moAccept
	introducers. If one matches, then the option name is
	checked. If both, the option and the moAccept have no
	introducer, the name is directly matched.

	The function returns the number of characters which matched
	in the name. This shall be used to know which option matched
	better (more letters) as -r and -restore are distinct and
	the second one should be used over the first one if it matched.

	Unless the stick bit is set, the returned length is the length
	of the option minus the introducer. This length is always
	equal to the length of the name moAccept object.

RETURN VALUE

	returns the length of the argument name if it matches and
	-1 if it doesn't match

SEE ALSO

	Option

*/
int moGetOpt::moAccept::Matches(const char *introducer, long length, const char *option) const
{
	const mowc::wc_t	*intro, *e;
	bool			stick;
	size_t			min, max, l;

	if(length == 0 && f_direct) {
		// the whole option is the option
		if(option == 0) {
			return 0;
		}
		// TODO: use lstrlen()?
		return static_cast<int>(strlen(option));	/* Flawfinder: ignore */
	}

	// equal to one of this option introducer?
	intro = f_introducer.Data();
	if(*intro != '\0' || length > 0) {
		// one of them is not zero
		e = intro;
		while(*intro != '\0') {
			if(*e == '|' || *e == '/' || *e == '\0') {
				if(e - intro == length
				&& mowc::strcmp(intro, introducer, length) == 0) {
					// we found a match!
					break;
				}
				if(*e != '\0') {
					e++;
				}
				intro = e;
			}
			e++;
		}
		if(*intro == '\0') {
			return -1;
		}
	}

	// match in full?
	stick = (f_follow & MO_ACCEPT_FOLLOW_STICK) != 0
		|| (f_follow & MO_ACCEPT_FOLLOW_EQUAL) != 0;

	// min/max sizes to match
	// TODO: use lstrlen()?
	max = strlen(option);	/* Flawfinder: ignore */
	min = stick ? 1 : max;
	l = f_name.Length();

	if(l < min || l > max) {
		return -1;
	}

	if(f_name.Compare(option, 0, static_cast<int>(l)) != MO_BASE_COMPARE_EQUAL) {
		return -1;
	}

	// we got a match! return its size.
	return static_cast<int>(l);
}



/************************************************************ DOC:

CLASS

	private:
	moAccept

NAME

	Option - create an option entry from the command line option

SYNOPSIS

	moOptionSPtr Option(moOption *option, int argc, const char *argv[],
			long& idx, const char *left, long& max) const;

PARAMETERS

	option - the current option to set
	argc - the number arguments in argv
	argv - the list of command line arguements
	idx - the index within argv
	left - what's left on the current command line option
	max - set to -1 when the remainder of the argv is used by the option
		or the error number when 0 is returned

DESCRIPTION

	This function parses the parameters used for this arguement
	and generates a corresponding moObject.

RETURNED VALUE

	When the function is successful, then an option pointer is returned.
	Otherwise, 0 is returned and the max parameter will be set to the
	error number.

	The idx parameter will be incremented as much as necessary whenever
	additional entries can be used up to satisfy the option (for instance
	an option of type INTEGER and MULTIPLE will get all the integers
	following the option and increase idx accordingly).

	The max parameter will be unchanged if the option was not followed by
	anything. It is set to -1 if the current argv[idx] entry was fully
	used. It is set to an error number if the function returns 0.

SEE ALSO

	Matches

*/
moGetOpt::moOptionSPtr moGetOpt::moAccept::Option(moOptionSPtr opt, int argc, const char *argv[], long& idx, const char *left, long& max) const
{
	unsigned long		id, follow;
	char			buf[256];	/* Flawfinder: ignore */
	mowc::wc_t		*end;
	const mowc::wc_t	*value, *s;
	moWCString		opt_value;
	bool			internal;
	moWCStringSPtr		item;

	if(f_reference != MO_GETOPT_NO_IDENTIFIER) {
		id = f_reference;
	}
	else {
		id = f_identifier;
	}

	// can be used only once?
	if(f_once && opt) {
		max = 1;
		return 0;
	}

	// if we always repeat then forget about the previous option(s)
	if(f_repeat && opt) {
		// NOTE: we need to increment the accept identifier so if
		//	 we have more than 2 items, it still works.
		const_cast<moAccept *>(this)->f_identifier++;
		id = f_identifier;
		opt = 0;
	}

	internal = !opt;
	if(internal) {
		opt = new moOption(f_name, false, id);
	}

	if(f_incremental) {
		// incrementals are assumed to be values
		opt->f_integer++;
		opt->f_float += 1.0;
		sprintf(buf, "%ld", opt->f_integer);	/* Flawfinder: ignore */
		opt->f_result = buf;
	}

	value = 0;
	follow = f_follow;
	if(f_direct) {
		// special case of a 'file' or a numeric
		// here we don't even check the following flags!
		opt_value = argv[idx];
		value = opt_value.Data();
		//max = strlen(argv[idx]);
	}
	else if((follow & (MO_ACCEPT_FOLLOW_STRING | MO_ACCEPT_FOLLOW_INTEGER | MO_ACCEPT_FOLLOW_FLOAT)) != 0) {
		if((follow & MO_ACCEPT_FOLLOW_EQUAL) != 0) {
			if(*left == '=') {
				left++;
				// we expect something stuck after the equal
				follow |= MO_ACCEPT_FOLLOW_STICK;
				follow &= ~MO_ACCEPT_FOLLOW_OPTIONAL;
			}
			else if((follow & MO_ACCEPT_FOLLOW_OPTIONAL) == 0
					&& (follow & MO_ACCEPT_FOLLOW_STICK) == 0) {
				// expected an equal sign!
				max = 2;
				return 0;
			}
		}
		if((follow & MO_ACCEPT_FOLLOW_STICK) != 0) {
			if(*left != '\0') {
				// there is something!
				opt_value = left;
				value = opt_value.Data();
				max = -1;		// used up
			}
			else if((follow & MO_ACCEPT_FOLLOW_OPTIONAL) == 0
					&& f_set.IsEmpty()) {
				// expected a value
				max = 3;
				return 0;
			}
		}
		// argv[idx + 1] should never be a null pointer
		if(value == 0 && idx + 1 < argc
		&& argv[idx + 1] != 0
		&& ((argv[idx + 1][0] != '-' && argv[idx + 1][0] != '+')
				|| f_set.IsEmpty())) {
			// get the next argv[]
			idx++;
			opt_value = argv[idx];
			value = opt_value.Data();
		}
		if(value == 0) {
			if(f_set.IsEmpty()) {
				if((follow & MO_ACCEPT_FOLLOW_OPTIONAL) == 0) {
					// we expected a value from the user!
					max = 4;
					return 0;
				}
				// adding something was optional,
				// so use the default value
				value = f_default.Data();
			}
			else {
				// get the forced value
				value = f_set.Data();
			}
		}
	}
	else if(!f_set.IsEmpty()) {
		value = f_set.Data();
	}

	if(value != 0) {
		// check for an integer if required
		opt->f_integer = mowc::strtol(value, &end, 0);
		if((end == 0 || (*end != '\0' && (*end != ','
			|| (follow & MO_ACCEPT_FOLLOW_MULTIPLE) == 0)))
		&& (follow & MO_ACCEPT_FOLLOW_INTEGER) != 0) {
			// expected a valid integer
			max = 5;
			return 0;
		}
		opt->f_float = mowc::strtod(value, &end);
		if((end == 0 || (*end != '\0' && (*end != ','
			|| (follow & MO_ACCEPT_FOLLOW_MULTIPLE) == 0)))
		&& (follow & MO_ACCEPT_FOLLOW_FLOAT) != 0) {
			// expected a valid float
			max = 6;
			return 0;
		}
		opt->f_result = value;
		// a string is always valid!

		// multiple string, integer or floats?
		if((follow & MO_ACCEPT_FOLLOW_MULTIPLE) != 0) {
			// case 1: does value include any commas?
			s = value;
			while(*s != ',' && *s != '\0') {
				s++;
			}
			if(*s == ',') {
				// each element is comma separated!
				item = new moWCString(value, static_cast<int>(s - value));
				opt->f_list += *item;
				do {
					s++;
				} while(*s == ',');
				value = s;
				while(*value != '\0') {
					while(*s != ',' && *s != '\0') {
						s++;
					}
					item = new moWCString(value, static_cast<int>(s - value));
					// Note: here we should check that
					// the argument is valid
					opt->f_list += *item;
					while(*s == ',') {
						s++;
					}
					value = s;
				}
			}
			else {
				// each element is in a different argv[]
				item = new moWCString(value);
				opt->f_list += *item;
				idx++;
				while(idx < argc) {
					if(argv[idx][0] == '-' || argv[idx][0] == '+') {
						break;
					}
					item = new moWCString(argv[idx]);
					opt->f_list += *item;
					idx++;
				}
				idx--;
			}
		}
		else if(f_direct || (follow & MO_ACCEPT_FOLLOW_LIST) != 0) {
			item = new moWCString(value);
			opt->f_list += *item;
		}
	}

#if 0
printf("value = %ld, %g, %s [%ld]\n",
			opt->f_integer, opt->f_float, opt->f_result.Data(),
			opt->f_list.Count());
#endif

	return opt;
}





/************************************************************ DOC:

CLASS

	private:
	moAccept

NAME

	Default - create an option entry from the defaults

SYNOPSIS

	moOptionSPtr Default(void) const;

DESCRIPTION

	This function creates an option object and initializes it with
	the default values as defined in the Accept() object.

RETURNED VALUE

	An option pointer is returned.

ERRORS

	When the new option can't be allocated, a memory error is thrown.

SEE ALSO

	Option

*/
moGetOpt::moOptionSPtr moGetOpt::moAccept::Default(void) const
{
	unsigned long	id;
	moOptionSPtr	opt;
	moWCStringSPtr	item;

	if(f_reference != MO_GETOPT_NO_IDENTIFIER) {
		id = f_reference;
	}
	else {
		id = f_identifier;
	}

	opt = new moOption(f_name, true, id);
	opt->f_integer = mowc::strtol(f_default.Data(), 0, 0);
	opt->f_float = mowc::strtod(f_default.Data(), 0);
	opt->f_result = f_default;
	if(f_direct || (f_follow & (MO_ACCEPT_FOLLOW_MULTIPLE | MO_ACCEPT_FOLLOW_LIST)) != 0) {
		item = new moWCString(f_default);
		opt->f_list += *item;
	}

	return opt;
}


/** \brief Create a parser of command line options.
 *
 * Create an moGetOpt object used to parse command line options.
 * 
 * This will be created in a main() function or as a global object.
 * 
 * The accept string defines the list of options accepted by this
 * object. The syntax is a set of names separated by semi-colons.
 * The exact syntax is:
 * 
 * [ <id> ] [ '->' <id> [ ',' ... ] [ '[' ] 
 * 	[ <introducer> ] <name> [ '=' <value> ]
 * 	[ <extra> ] [ ']' ] ';' ...
 * 
 * where only the name is required.
 * 
 * The optional <id> is a value used as an identifier to distinguish
 * entries and quickly search for these entries. When no id is given,
 * the default value MO_GETOPT_NO_IDENTIFIER is given. Users can
 * only use values from MO_GETOPT_USER_IDENTIFIER (1000 up).
 * 
 * The identifier can be followed by references. This is used for
 * equivalent options. That is, when an option such as -v as a
 * long equivalent --verbose, only one should be set whichever
 * one is used on the command line. This is done by referencing
 * one in the other as in:
 * 
 * 	"11 -v; 12 -> 11 --verbose"
 * 
 * (note that the spaces are optional). An option can have one and
 * only one reference.
 * 
 * When an option is written between square brackets, it is optional
 * to the end user (i.e. one doesn't need to type it on the command
 * line, non optional entries generate errors whenever a Main()
 * command is called). If all the following commands are optional,
 * one can use the special name asterisk as in "...; [*]; ..."
 * (in this case, spaces are accepted before and after the square
 * brackets, but not within.) The extra 'optional' keyword can
 * also be used.
 * 
 * The introducer can be any number of + and - separated by / or |.
 * To accept one or two dashes (- or --) then write the option as:
 * "-/--name". If the option can receive a plus or a minus: "+/-opt".
 * Note that if the +/- have an effect on the value (i.e. - turns on
 * the option and + turns if off) then you may want to use two
 * entries as in: "1111 -opt set to 1;1112->1111 +opt set to 0". The
 * reference in the second option makes sure only the first one is
 * set to 1 or 0. It is still necessary to use the identifier to
 * search option 1111 to know which of -opt or +opt was used on the
 * command line.
 * 
 * A default value can be given to any of the options.
 * 
 * The <extra> part includes information on the command line option.
 * There needs to be a space between what comes before and an extra
 * entry. These include the following:
 * 
 * 	++, inc, increment, incremental
 * 		incremental flag
 * 
 * 	file, no flag
 * 		accept entries without any option ("-f <file>"
 * 		uses the option '-f' and "<file>" doesn't use any
 * 		option); when such an option is used, the name of
 * 		the option is only for the programmer to reference
 * 		the option; it will not be used by the system
 * 
 * 	numeric
 * 		accept a value as entry; if the entry is not a value,
 * 		test with the following entries; like file otherwise
 * 
 * 	min <value>
 * 	max <value>
 * 		the minimum and maximum number of file entries for
 * 		a given file; once a file entry has reached the
 * 		maximum, the following one is used; at the end,
 * 		the minimum is checked to make sure it was reached,
 * 		if not an error is generated
 * 
 * 	on, 1, true, use, yes
 * 		turn the flag on (true); this is the default when
 * 		nothing else is specified
 * 
 * 	off, 0, false, don't use, no
 * 		turn the flag off (false); usually used to remove
 * 		a default or with a reference to an ON flag
 * 
 * 	equal <value>
 * 	set to <value>
 * 		like on and off except a specific value is given
 * 		(i.e. "set to 1" is equivalent to "on" and
 * 		"set to 0" is equivalent to "off"); the end user
 * 		can't specify a value
 * 
 * 	[ '[' '=' ':' ']' ] [ '[' ] <what> [ 's' ] [ ']' ]
 * 
 * 		Integers:
 * 		<what> = value, values, int, ints, integer, integers
 * 
 * 		Floating points:
 * 		<what> = float, floats, double, doubles, real, reals
 * 
 * 		Strings:
 * 		<what> = string, strings
 * 
 * 		expect one (singular) or several (plurial) integers,
 * 		floats or strings to follow; when an equal sign is
 * 		specified, the option is expected to use the equal
 * 		sign; put the equal sign between square brackets
 * 		if you want to enable both syntaxes; the colon (:)
 * 		can be used to specify that the value(s) must be
 * 		stuck to the option; use the colon between square
 * 		brackets if you want to enable both syntaxes; when
 * 		both the equal and colon are used between square
 * 		brackets then all the possibilities are accepted;
 * 		(note the equal and colon can be given in any order)
 * 		the value(s) and string(s) can be defined between
 * 		square brackets in which case these are mandatory
 * 		and then the option can be used without them; the
 * 		floats will possibly include a decimal point and
 * 		an exponent as defined in C (see the strtod(3)
 * 		function call); values can be signed; when several
 * 		integers, floats or strings are defined as stuck
 * 		or with a non-mandatory equal sign, these will be
 * 		expected to be listed in a single argv entry and
 * 		separated by commas (i.e. --file=a,b,c)
 * 
 * 		Examples:
 * 
 * 		Option definition		Command line option
 * 
 * 		"--debug=0 integer"		--debug 5
 * 
 * 		"--debug=0 =integer"		--debug=5
 * 
 * 		"--debug=0 [=]integer"		--debug 5
 * 		"--debug=0 [=]integer"		--debug=5
 * 
 * 		"--debug=0 :integer"		--debug5
 * 		"--debug=0 [:]integer"		--debug5
 * 		"--debug=0 [:]integer"		--debug 5
 * 
 * 		"--debug=0 [=:]integer"		--debug5
 * 		"--debug=0 [=:]integer"		--debug 5
 * 		"--debug=0 [=:]integer"		--debug=5
 * 
 * 		"--debug=0 :[integer] set to 1"	--debug
 * 		"--debug=0 :[integer] set to 1"	--debug5
 * 
 * 		In the last case, the --debug value is set to
 * 		zero (=0) by default. If the --debug is used
 * 		as is on the command line, then it is "set to 1".
 * 		It can also be followed by a value as in --debug5
 * 		in which case it will be set to 5.
 * 
 * 	'*'
 * 	repeat
 * 		enables the option to be entered several times
 * 		on the command line and each entry is kept in
 * 		the f_list parameter of the option; the f_list
 * 		only includes moWCString objects
 * 
 * 	optional
 * 		make this entry optional (equivalent to square
 * 		brackets around the option)
 * 
 * 	required
 * 		makes this entry required (this can be used after
 * 		a [*] was found to force some entries)
 * 
 * Internally, this object automatically defines the following:
 * 
 * 	"1 -h",
 * 	"2->1 --help",
 * 
 * 	"11 -v ++",
 * 	"12->11 --verbose ++",
 * 
 * 	"21 -V set to -1",
 * 	"22->21 -V0 set to 0",
 * 	"23->21 -V1 set to 1",
 * 	"24->21 --version [=]integer set to -1",
 * 
 * 	"31 --copyright",
 * 
 * 
 * The Main() functions also understand these parameters automatically
 * and therefore there is no need for the user to handle them at
 * all. See Main() for explaination about these options.
 * 
 * The following functions will also be called whenever one of these
 * options is used:
 * 
 * 	OnHelp()		the -h or --help was used
 * 
 * 	OnVersion()		the -V, -V0, -V1 or --version was
 * 				used;
 * 
 * 	OnCopyright()		the --copyright has been used
 * 
 * 	OnError()		an error occured
 *
 * \section examples Examples
 *
 * 1. Options and defaults
 * 
 * In the following accepts -o or --output and the -c flag. Only
 * the options 1001 and 1003 will be defined:
 * 
 * "[*]; 1001 -o=a.out; 1002->1001 --output=a.out; 1003 -c;"
 * 
 * Use the Option() instruction with 1001 and 1003 to get
 * the result. The -o and --output get a.out as the default
 * name.
 * 
 * 2. Filenames
 * 
 * If you need a list of filenames as input and the -q flag,
 * one can use this:
 * 
 * "2000 -f=. file repeat; 1001 -q=1; 1002->1002 -k=0"
 * 
 * Here the option 2000 will have it's f_list defined with a
 * list of moWCString's objects. One string per filename found
 * on the command line. Note that the -f flag itself is ignored.
 * By default, the -q flag will be set to 1 and -k is ignored.
 * If the user specifies -k, then the value of option 1002 will
 * be 0.
 * 
 * WARNING: the repeat flag means that you will get options
 * starting at the specified index (2000 in this example);
 * because you can't tell how many entries you will get, you
 * should use an index larger than any other flag. Also, to
 * retrieve the values, you will need to call the Option()
 * with, in this example, 2000, 2001, 2002, etc. until you get
 * a NULL pointer (if the parameter is optional, then the
 * very first entry can already be a NULL pointer).
 * 
 * 3. Values (integers or strings)
 * 
 * The following offers the user to specify a value following
 * an option:
 * 
 * "1001 --max=0 [:=]integer"
 * 
 * In this case, the max option is set to 0 by default. The
 * end user can set it to any integer using one of:
 * 
 * 	--max3 or --max=3 or --max 3
 * 
 * The ':' in means that the value can be stuck with the option.
 * The '=' means you need to use an equal sign, but the '[' and
 * ']' means it is optional. When the ':' is marked optional,
 * the integer can be written seperated. If no ':' and no '='
 * were specified ("1001 --max=0 integer"), then only a spaced
 * value can be specified.
 * 
 * "1001 --max=0 integer" allows for --max 3 only
 * "1001 --max=0 =integer" allows for --max=3 only
 * "1001 --max=0 :integer" allows for --max3 only
 * 
 * See the --debug for more examples on these options.
 *
 * \param[in] accept    A string of accepted command line options
 *
 * \exception moBug whenever an invalid parameter definition is
 * found in the string of options. A message is also printed.
 * These errors will only occur if the input strings are
 * invalid. Programmers need to fix those before shipping.
 *
 * \sa Main(), Option(), operator [] (), OnError(), OnVersion(),
 * OnCopyright(), OnHelp()
 */
moGetOpt::moGetOpt(const moWCString& accept)
{
	static const char	*defopt[] = {
		"1 -h",
		"2->1 --help",

		"11 -v ++",
		"12->11 --verbose ++",

		"21 -V set to -1",
		"22->21 -V0 set to 0",
		"23->21 -V1 set to 1",
		"24->21 --version [=]integer set to -1",

		"31 --copyright",

		0
	};
	const char		**l;
	const mowc::wc_t	*s, *n;
	mowc::wc_t		quote, c;
	bool			optional;
	long			idx;
	unsigned long		ref;
	moAcceptSPtr		acc, a;
	moList::position_t	p;

	f_errcnt = 0;

	f_options_id.SetCompare(reinterpret_cast<compare_function_t>(&moOption::CompareID));
	f_accept_id.SetCompare(reinterpret_cast<compare_function_t>(&moAccept::CompareID));

	s = accept.Data();
	//e = s + accept.Length();
	optional = false;

	l = defopt;
	while(*l != 0) {
		acc = new moAccept(*l, false, true);
		f_accept += *acc;
		if(acc->HasIdentifier()) {
			f_accept_id += *acc;
		}
		l++;
	}

	while(*s != '\0') {
		/*
		 * skip extraneous ';' in case someone put 2 or more
		 * with empty entries
		 */
		while(*s == ';' || mowc::isspace(*s)) {
			s++;
		}
		/* the following often happens at the end of the string */
		if(*s == '\0') {
			break;
		}
		n = s;
		while(*n != ';' && *n != '\0') {
			/* values may be defined in quotes */
			if(*n == '=') {
				do {
					n++;
				} while(mowc::isspace(*n));
				quote = *n;
				switch(quote) {
				case '\'':
				case '"':
				case '`':
					c = *++n;
					while(c != '\0' && c != quote) {
						c = *++n;
						if(c == '\\' && n[1] != '\0') {
							n++;
						}
					}
					break;

				}
				if(*n != '\0') {
					n++;
				}
			}
			else {
				n++;
			}
		}
		moWCString opt(s, static_cast<int>(n - s));
		opt = opt.Clip();
		if(opt == "[*]") {
			optional = true;
		}
		else {
			acc = new moAccept(opt, true, optional);
			if(acc->HasIdentifier()) {
				f_accept_id += *acc;
			}
			f_accept += *acc;
		}
		s = n;
	}

	/* now we can take care of the referenced accept */
	idx = f_accept.Count();
	while(idx > 0) {
		idx--;
		a = const_cast<moAccept *>(dynamic_cast<const moAccept *>(&f_accept[idx]));
		if(a->HasReference()) {
			ref = a->Reference();
			acc = new moAccept(ref);
			p = f_accept_id.Find(acc);
			if(p == moList::NO_POSITION) {
std::cerr << "ERROR: moGetOpt::moGetOpt(): invalid reference: " << ref << "; reference not found for option " << a->Identifier() << "." << std::endl;
				throw moBug(MO_ERROR_INVALID, "Reference not found");
			}
			if(a == dynamic_cast<const moAccept *>(&f_accept_id[p])) {
std::cerr << "ERROR: moGetOpt::moGetOpt(): invalid reference: " << ref << "; option " << a->Identifier() << " referencing itself." << std::endl;
				throw moBug(MO_ERROR_INVALID, "Option is referencing itself");
			}
			a->Referenced(const_cast<moAccept *>(dynamic_cast<const moAccept *>(&f_accept_id[p])));
		}
	}
}


moGetOpt::~moGetOpt()
{
}


moGetOpt::moGetOpt(const moGetOpt& option)
	: moBase(option)
{
	std::cerr << "ERROR: moGetOpt::moGetOpt(): no copy operator for GetOpt()." << std::endl;
	abort();
}


moGetOpt& moGetOpt::operator = (const moGetOpt& /*option*/)
{
	std::cerr << "ERROR: moGetOpt::operator = (): no assignment operator for GetOpt()." << std::endl;
	abort();
	/*NOTREACHED*/
#if defined(MO_CONFIG_ALWAYS_NEED_RETURN) || defined(_MSC_VER)
	return *this;
#endif
}


/************************************************************ DOC:

CLASS

	moGetOpt

NAME

	Option - get/set options
	operator [] - get options with their identifier

SYNOPSIS

	moOptionSPtr Option(const moWCString& name) const;
	moOptionSPtr Option(int identifier) const;

	moOptionSPtr operator [] (const moWCString& name) const;
	moOptionSPtr operator [] (int identifier) const;

	void AddOption(const moOption& option);

PARAMETERS

	name - name of the option to search
	identifier - identifier of the option to search
	option - the option to insert

DESCRIPTION

	These functions are used to insert new options in a GetOpt()
	object and retrieve existing options.

	Options can be search with their name. If more than one entry
	uses the same name, however, it will be necessary to give them
	different identifier numbers and search them using that
	identifier. (See the Constructor for more information about
	how to define identifiers).

	Options can be search using their identifiers. In this case,
	the array ([]) operator can be used. The identifiers are
	these specified before the option in the list of options
	string.

	Usually, options are set to the defaults specified in the
	string given to the GetOpt() constructor. If some of the
	options depend on dynamic defaults (say $HOME/.vimrc) then
	these can be set before one of the Main() function is
	called. Others can be set after knowing that it may mean
	they will override the options entered by the end user.

EXAMPLES

	moGetOpt	command_line("-/--rcfile='.vimrc'");
	moWCString	home(getenv("HOME"));
	moFilename	name = home + ".vimrc";
	moOption	rcfile("rcfile", name, true);
			command_line.Option(rcfile);
	moOption	*filename = command_line.Option("rcfile");

	// here one can use filename->f_value as the rcfile name

NOTES

	It is possible to use the "set option" function member to
	add options not available on the command line (which could
	be available through other means like a configuration
	file, environment variables, etc.). If you use an identifier
	then create an moOptionIdentifier instead of an moOption
	otherwise it won't be found properly.

SEE ALSO

	Main

*/
moGetOpt::moOptionSPtr moGetOpt::Option(const moWCString& name) const
{
	moOption		option(name);
	moList::position_t	pos;

	pos = f_options.Find(&option);

	return pos == moList::NO_POSITION ?
		0 : dynamic_cast<moGetOpt::moOption *>(f_options.Get(pos));
}


moGetOpt::moOptionSPtr moGetOpt::Option(unsigned long identifier) const
{
	moOption		option("unknown", false, identifier);
	moList::position_t	pos;

	pos = f_options_id.Find(&option);

	return pos == moList::NO_POSITION ?
		0 : dynamic_cast<moGetOpt::moOption *>(f_options_id.Get(pos));
}


moGetOpt::moOptionSPtr moGetOpt::operator [] (const moWCString& name) const
{
	return Option(name);
}


moGetOpt::moOptionSPtr moGetOpt::operator [] (int identifier) const
{
	return Option(identifier);
}


void moGetOpt::AddOption(const moOption& option)
{
	f_options.Insert(&option);
	if(option.f_identifier != MO_GETOPT_NO_IDENTIFIER) {
		f_options_id.Insert(&option);
	}
}





/************************************************************ DOC:

CLASS

	moGetOpt

NAME

	Main - parses command line (like) arguments

SYNOPSIS

	virtual int Main(int argc, const char *argv[]);

PARAMETERS

	argc - number of arguments in argv
	argv - array of arguments

DESCRIPTION

	This function will match the given command line arguments
	to the one defined in the moGetOpt() object.

	It will create options one can retrieve with the Option()
	function members and the array operator ([]) of the moGetOpt
	object.

	The previous set of options will be lost.

	Note that an moApp class could also be created as a derived
	version of moGetOpt() which gives you a chance to (1) overload
	the Main() function and the different events (errors, help,
	version and copyrights).

EXAMPLES

	int main(int argc, char *argv[])
	{
		moGetOpt	getopt("[-c]; [-d]; <filename>");

		command_line.Main(argc, argv);

		moGetOpt::moOption *opt = command_line.Option("c");
		if(opt != 0) {
			// the -c option was given on the command line!
		}

		...
	}

SEE ALSO

	Option, operator []

*/
int moGetOpt::Main(int argc, const char *argv[])
{
	long		idx, acc, found, max, r, length, c;
	moOptionSPtr	option, opt;
	const char	*introducer, *name;
	const moAccept	*accept;
	unsigned long	id;
	char		buf[256];	/* Flawfinder: ignore */

	// empty the lists of options
	f_options.Empty();
	f_options_id.Empty();

	// make sure f_progname is defined
	if(f_progname.IsEmpty()) {
		f_progname = strrchr(argv[0], '/');
		if(f_progname.IsEmpty()) {
			// for Windows
			f_progname = strrchr(argv[0], '\\');
		}
		if(f_progname.IsEmpty()) {
			f_progname = argv[0];
		}
		else {
			f_progname = f_progname.Delete(0, 0);
		}
	}

	// parse each argument
	idx = 1;
	while(idx < argc) {
		if(argv[idx] == 0) {
			// error
std::cerr << "ERROR: moGetOpt::Main() does not like 'argv[] = (null)'." << std::endl;
			throw moBug(MO_ERROR_INVALID, "argv[] array has a null pointer");
		}

		// get the introducer
		introducer = argv[idx];
		name = introducer;
		c = *name;
		while(c == '-' || c == '+') {
			c = *++name;
		}

		// introducer only?
		if(*name == '\0') {
			// this is a plain option! (like -- to avoid options later)
			name = introducer;
			length = 0;
		}
		else {
			length = static_cast<int>(name - introducer);
		}

		do {
			// search the moAccept object matching this option
			found = -1;
			max = 0;
			acc = f_accept.Count();
			while(acc > 0) {
				acc--;
				r = dynamic_cast<const moAccept&>(f_accept[acc]).Matches(introducer, length, name);
				if(r > max) {
					max = r;
					found = acc;
				}
			}
			if(found == -1) {
				// no match for this arguement
				f_errcnt++;
				if(OnError(argv[idx], "unrecognized command line option")) {
					// well, the caller knew better!
					f_errcnt--;
				}
				break;
			}

			// search if we already have that option in stock
			accept = dynamic_cast<const moAccept *>(&f_accept[found]);

			// options with references only get their reference
			// created in the result list of options
			id = accept->Reference();
			if(id == MO_GETOPT_NO_IDENTIFIER) {
				id = accept->Identifier();
			}
			if(id != MO_GETOPT_NO_IDENTIFIER) {
				opt = Option(id);
			}
			else {
				opt = Option(accept->Name());
			}

			option = const_cast<moAccept *>(accept)->Option(opt, argc, argv, idx, name + max, max);
			if(!option) {
				// an error occured, use max to know which one
				f_errcnt++;
				switch(max) {
				case 1:
					r = OnError(argv[idx], "option can't be defined more than once");
					break;

				case 2:
					r = OnError(argv[idx], "expected an equal sign after the option name");
					break;

				case 3:
					r = OnError(argv[idx], "expected a value right after this option");
					break;

				case 4:
					r = OnError(argv[idx], "expected a value following this option");
					break;

				case 5:
					r = OnError(argv[idx], "expected an integer with this option");
					break;

				case 6:
					r = OnError(argv[idx], "expected a floating point number with this option");
					break;

				default:
					r = OnError(argv[idx], "an unexpected error occured");
					break;

				}
				if(r) {
					// well, the caller knew better!
					f_errcnt--;
				}
				break;
			}
			else if(option != opt) {
				AddOption(*option);
			}

			// check for more options within the same line
			name += max;
		} while(*name != '\0' && max > 0);

		// get the next parameter
		idx++;
	}

	acc = f_accept.Count();
	while(acc > 0) {
		acc--;
		accept = dynamic_cast<const moAccept *>(&f_accept[acc]);
		// when there is a reference, the test is done with that
		// referenced entry instead
		if(accept->Reference() == MO_GETOPT_NO_IDENTIFIER) {
			id = accept->Identifier();
			if(id != MO_GETOPT_NO_IDENTIFIER) {
				opt = Option(id);
			}
			else {
				opt = Option(accept->Name());
			}
			if(opt == 0) {
				if(accept->HasDefault()) {
					opt = accept->Default();
					f_options += *opt;
					if(opt->f_identifier != 0) {
						f_options_id += *opt;
					}
				}
				else if(!accept->Optional()) {
					f_errcnt++;
					snprintf(buf, sizeof(buf) - 1, "the option %s is required", accept->Name().MBData());	/* Flawfinder: ignore */
					buf[sizeof(buf) - 1] = '\0';
					r = OnError("???", buf);
					if(r) {
						// well, the caller knew better!
						f_errcnt--;
					}
				}
			}
		}
	}

	if(f_errcnt != 0) {
		// on errors we quit; there is no point in trying to run
		// the program which would most probably crash anyway
		OnHelp();
		std::cerr << f_progname << " [" << getpid() << "] FATAL ERROR: "
			<< f_errcnt
			<< (f_errcnt == 1 ? " error was" : " errors were")
			<< " found on the command line."
			<< std::endl;
		exit(1);
	}

	return 0;
}



/************************************************************ DOC:

CLASS

	moGetOpt

NAME

	SetVersion - defines the software version
	SetProgname - defines the program name
	SetCopyright - defines the copyright (one line)

SYNOPSIS

	void SetVersion(const moWCString& version);
	void SetProgname(const moWCString& progname);
	void SetCopyright(const moWCString& copyright);

PARAMETERS

	version - the software version (i.e. 1.0)
	progname - the software name (i.e. argv[0])
	copyright - the software copyright (one line)

DESCRIPTION

	These functions are expected to be called to setup the moGetOpt
	object before calling the Main() function. These setup the options
	as expected by the different error and similar functions.

	The SetProgname() call is optional since the argv[0] parameter
	can be used in the Main() function to define the name of the
	program.

	The SetVersion() and SetCopyright() can be skipped if you overwrite
	the OnVersion() and OnCopyright() functions. In that case, your
	functions can display whatever you want.

NOTE

	If you use the moApplication class, call that class SetVersion()
	and SetCopyright() functions and then the 

*/
void moGetOpt::SetVersion(const moWCString& version)
{
	f_version = version;
}


void moGetOpt::SetProgname(const moWCString& progname)
{
	f_progname = progname;
}


void moGetOpt::SetCopyright(const moWCString& copyright)
{
	f_copyright = copyright;
}




/************************************************************ DOC:

CLASS

	moGetOpt

NAME

	Messages: OnError, OnHelp, OnVersion, OnCopyright
		- called whenever the corresponding option is selected

SYNOPSIS

	virtual bool OnError(const moWCString& argv, const moWCString& message);
	virtual void OnHelp(void);
	virtual void OnVersion(long count);
	virtual void OnCopyright(void);

PARAMETERS

	argv - the argument which generates the given error
	message - the error message
	count - the number following the -V option

DESCRIPTION

	The OnError() virtual function member will be called whenever
	an unrecognized command line option was used. This function
	should print out a message in some way and return. It is
	possible to exit(3) as well in which case other options won't
	generate errors. Though the error counter will already be
	increased, it is also possible to interpret the argv argument
	in a way not available in the moGetOpt object. In this case,
	the function must return 'true'. In all other cases it needs
	to return 'false'.

	It can be useful to catch errors for options which accept
	values but need to also accept one or two special keywords
	as in the following:

		"-d=0 [=]integer set to 1"

	If you want this option to also accept -d=off to set it to the
	default then the OnError() will have to test whether the following
	option is OFF. If so, it should set the option to zero and make
	sure the OnError() function returns 'true'.

	The OnHelp() virtual function prints out the command line
	options as defined in the moGetOpt() string argument. It also
	prints the version (as it is printed by default with the -V
	option). If extraneous help information is available, once
	can overwrite this function which should still be called to
	print out the options defined in moGetOpt() - unless some
	or many of the options printed that way should remain
	invisible to the end users.

	The OnVersion() virtual function prints out the name of the
	program and its version. The count parameter will be used to
	know which of -V, -V1 or -V0 was used (or the corresponding
	--version). The -V should print a human readable version. The
	-V1 should print out the name of the program and the version
	preceeded by the letter 'V'. The -V2 should only print out the
	version number (no program name and no extraneous letter).

	The version must be set with SetVersion() or the OnVersion()
	function overwritten.

	The OnCopyright() virtual function prints out a copyright
	for the given program. The default function prints out the
	string set with SetCopyright(). If a more complete copyright
	is required, overwrite this function.

RETURN VALUE

	Only OnError() returns a value. It needs to return 'false'
	when the argv argument couldn't be used and a real error
	occured (the default). It will return 'true' if the argv
	argument was used in some special manner by the system.

	Note: the default OnError() function always returns 'false'.

*/
bool moGetOpt::OnError(const moWCString& argv, const moWCString& message)
{
	std::cerr << f_progname << " [" << getpid() << "] ERROR: "
			<< message << " (" << argv << ")" << std::endl;

	return false;
}


void moGetOpt::OnHelp(void)
{
	OnVersion(-1);

	std::cerr << "Usage: " << f_progname << " [-opt] files/names" << std::endl;
}


void moGetOpt::OnVersion(long count)
{
	switch(count) {
	case -1:		/* default version printout */
		std::cerr << f_progname << " V" << f_version << " -- "
				<< f_copyright << std::endl;
		break;

	case 0:
		std::cerr << f_progname << " V" << f_version << std::endl;
		break;

	case 1:
		std::cerr << f_version << std::endl;
		break;

	}
}


void moGetOpt::OnCopyright(void)
{
	std::cerr << "Copyright: " << f_copyright << std::endl;
}


}			// namespace molib;

// vim: ts=8

