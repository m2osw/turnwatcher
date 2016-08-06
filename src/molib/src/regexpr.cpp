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
#pragma implementation "mo/mo_regexpr.h"
#endif

#include	"mo/mo_regexpr.h"


namespace molib
{



/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	Constructor - create an moRegularExpression object
	Copy operator - copy an moRegularExpression object

SYNOPSIS

	moRegularExpression(void);
	moRegularExpression(const char *str, int length = -1, encoding_t encoding = MO_ENCODING_UTF8);
	moRegularExpression(const mc_t *str, int length = -1, encoding_t encoding = MO_ENCODING_UCS2_INTERNAL);
	moRegularExpression(const wc_t *str, int length = -1, encoding_t encoding = MO_ENCODING_UCS4_INTERNAL);
	moRegularExpression(const wchar_t *str, int length = -1);	// encoding always as defined in the compiler!
	moRegularExpression(const moWCString& string, int length = -1);
	moRegularExpression(const moRegularExpression& regexpr, int length = -1);
	moRegularExpression& operator = (const moWCString& string);
	moRegularExpression& operator = (const moRegularExpression& regexpr);
	moRegularExpression& operator = (const char *str);
	moRegularExpression& operator = (const mc_t *str);
	moRegularExpression& operator = (const wc_t *str);
	moRegularExpression& operator = (const wchar_t *str);

PARAMETERS

	str - a standard null terminated C string
	encoding - the encoding of the corresponding string
	string - an moWCString object to copy
	regexpr - an moRegularExpression object to copy
	length - the maximum number of characters to copy
	         or -1 for all

DESCRIPTION

	These constructors initialize a regular expression object
	from the given parameters. The default operator creates
	an empty expression.

	The copy operators create a new regular expression object
	with data equal to what was defined in the source string
	or regular expression.

ERRORS

	These functions call the Init() function member which can
	generate a memory error long(MO_ERROR_MEMORY).

SEE ALSO

	NoNewLine

*/
moRegularExpression::moRegularExpression(void)
{
	Init();
}


moRegularExpression::moRegularExpression(const char *str, int length, mowc::encoding_t encoding)
	: moWCString(str, length, encoding)
{
	Init();
}


moRegularExpression::moRegularExpression(const mowc::mc_t *str, int length, mowc::encoding_t encoding)
	: moWCString(str, length, encoding)
{
	Init();
}


moRegularExpression::moRegularExpression(const mowc::wc_t *str, int length, mowc::encoding_t encoding)
	: moWCString(str, length, encoding)
{
	Init();
}


moRegularExpression::moRegularExpression(const wchar_t *str, int length)
	: moWCString(str, length)
{
	Init();
}


moRegularExpression::moRegularExpression(const moWCString& str, int length)
	: moWCString(str, length)
{
	Init();
}


moRegularExpression::moRegularExpression(const moRegularExpression& regexpr, int length)
	: moWCString(static_cast<const moWCString&>(regexpr), length)
{
	Init();
}


moRegularExpression& moRegularExpression::operator = (const moWCString& string)
{
	Set(string.Data(), -1);

	return *this;
}


moRegularExpression& moRegularExpression::operator = (const moRegularExpression& regexpr)
{
	// if not copying ourself then regenerate the expression
	if(this != &regexpr) {
		Set(regexpr.Data(), -1);
	}

	return *this;
}


moRegularExpression& moRegularExpression::operator = (const char *str)
{
	Set(str, -1);

	return *this;
}


moRegularExpression& moRegularExpression::operator = (const mowc::mc_t *str)
{
	Set(str, -1);

	return *this;
}


moRegularExpression& moRegularExpression::operator = (const mowc::wc_t *str)
{
	Set(str, -1);

	return *this;
}


moRegularExpression& moRegularExpression::operator = (const wchar_t *str)
{
	Set(str, -1);

	return *this;
}


moRegularExpression::~moRegularExpression(void)
{
	delete f_copy_string;
	delete f_buffer;
}


/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	Node - get a pointer to the specified node

SYNOPSIS

	private:
	mo_regexpr_node_t *Node(int index);

DESCRIPTION

	This function will be used to get the node at
	the specified index.

BUG

	Calling this function with an invalid node number
	can generate an error.

SEE ALSO

	NodeAlloc, NodeReset

*/
moRegularExpression::mo_regexpr_node_t *moRegularExpression::Node(int index)
{
	if(!f_node) {
		return 0;
	}
	return static_cast<mo_regexpr_node_t *>(f_node->Get(index));
}



/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	private:
	NodeReset - forget about all the allocated nodes

SYNOPSIS

	void NodeReset(void);

DESCRIPTION

	This function is used to reset the node allocator.
	None of the nodes allocated before can be reused.

SEE ALSO

	NodeAlloc

*/
void moRegularExpression::NodeReset(void)
{
	f_free = 0;
	// we keep the array allocated, though it could
	// be good to reallocate for space, it saves time
}


/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	LastError - returns the last error number

SYNOPSIS

	mo_regexpr_errno_t LastError(void);

DESCRIPTION

	This function returns the last error encountered while parsing
	the regular expression.

	It will be set to MO_REGEXPR_ERROR_NONE when the current
	expression is valid.

	This function will first ensure that the expression is
	actually analyzed and therefore it can take a little while
	before it returns.

ERRORS

	Since the Analyze() function may generate memory errors,
	this function will eventually throw an std::bad_alloc().

SEE ALSO

	Analyze

*/
moRegularExpression::mo_regexpr_errno_t moRegularExpression::LastError(void)
{
	(void) Analyze();
	return f_errno;
}



/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	GetStartPos - returns the position where the expression first matched
	GetEndPos - returns the position where the expression last matched

SYNOPSIS

	int GetStartPos(void) const;
	int GetEndPos(void) const;

DESCRIPTION

	These functions return the start and ending positions of a matching
	expression. This marks the very beginning and the very end of a match.

RETURN VALUE

	These functions return -1 if the last call to the MatchExpression()
	function was not successful, or that function was never called.

	Otherwise, the values represent a position in the input string from
	the first character that matched to the last character that matched
	plus 1 (i.e. exclusive end.)

SEE ALSO

	MatchExpression

*/
int moRegularExpression::GetStartPos(void) const
{
	return f_start_pos;
}


int moRegularExpression::GetLastPos(void) const
{
	return f_end_pos;
}





/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	Init - initialize a regular expression object

SYNOPSIS

	private:
	void Init(void);

DESCRIPTION

	This function is used internally to initialize a new regular
	expression object.

SEE ALSO

	Analyze

*/
void moRegularExpression::Init(void)
{
	//f_copy_length -- auto-init to zero
	//f_copy_max -- auto-init to zero
	f_copy_string = 0;
	f_buffer = 0;
	//f_invalid -- auto-init to zero

	//f_free -- auto-init to zero
	//f_node -- auto-init to zero
	f_errno = MO_REGEXPR_ERROR_NONE;
}


/** \brief Allocate one node.
 *
 * This function will allocate one node and return it's
 * reference number.
 *
 * \param[in] type      the type of node (see the MO_REGEXPR_...)
 * \param[in] left      the left hand side node
 * \param[in] right     the right hand side node
 * \param[in] pattern   the pattern to match
 * \param[in] length    number of characters to match in pattern
 *
 * \exception std::bad_alloc() if a memory allocation fails.
 *
 * \sa NodeReset()
 */
unsigned long moRegularExpression::NodeAlloc(mo_regexpr_t type, unsigned long left, unsigned long right, const mowc::wc_t *pattern, unsigned long length)
{
	mo_regexpr_node_t	n;

	/* any nodes allocated? */
	if(!f_node) {
		// create a new array of nodes
		f_node = new moArray(sizeof(mo_regexpr_node_t));
	}

	memset(&n, 0, sizeof(mo_regexpr_node_t));
	n.nd_type = type;
	n.nd_left = left;
	n.nd_right = right;
	n.nd_length = length;
	n.nd_pattern = pattern;
	f_node->Set(f_free, &n);

	return f_free++;
}


/** \brief Get the next character from the input string.
 *
 * This function retrieves the next character from the input string.
 *
 * The character is returned as a wide character.
 *
 * \param[in] state   The current regular expression state.
 *
 * \return The next character or '\0' once the end of the string is reached.
 */
mowc::wc_t moRegularExpression::AnalyzeGetC(mo_regexpr_analyze_t& state)
{
	mowc::wc_t	c;

	c = *state.an_input;
	if(c != '\0') {
		state.an_input++;
	}

	state.an_char = c;

//fprintf(stderr, "%c", c);
//fflush(stderr);

	return c;
}


bool moRegularExpression::AnalyzeExprUnary(mo_regexpr_analyze_t& state)
{
	unsigned long		n, length;
	mo_regexpr_node_t	*node;
	mo_regexpr_t		type;
	mowc::wc_t		*pattern, *s, *name, c, quote;

	switch(state.an_char) {
	case '(':
		// group expressions
		AnalyzeGetC(state);		// skip the '('
		state.an_level++;
		if(AnalyzeExprOr(state)) {	// get the sub-expressions
			state.an_level--;
			return true;
		}
		state.an_level--;
		if(state.an_char != ')') {	// valid?
			f_errno = MO_REGEXPR_ERROR_BADGROUP;
			return true;
		}
		AnalyzeGetC(state);	// skip the ')'
		return false;

	case ')':	// end of group!
		AnalyzeGetC(state);		// skip the ')'
		return false;
		// I think we should have an error here?!
		//f_errno = MO_REGEXPR_ERROR_BADGROUP;
		//return true;

	case '[':
		pattern = state.an_input;
		c = AnalyzeGetC(state);
		if(c == '!' || c == '^') {
			pattern = state.an_input;
			c = AnalyzeGetC(state);	// skip the negate
			type = MO_REGEXPR_NONE;
		}
		else {
			type = MO_REGEXPR_ANY;
		}
		do {
			if(c == '[') {
				// check for classes ([:alpha:], etc.)
				// these are defined within the other brackets as in:
				//	[[:alnum:]] === [[:alpha:]0-9]
				if(c == ':') {
					do {
						c = AnalyzeGetC(state);
						if(c == ':') {
							c = AnalyzeGetC(state);
							if(c == ']') {
								goto goodclass;
							}
							break;
						}
					} while((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
					// invalid "class" name -- we most certainly had
					// a standard class!!!
					goto noclassname;
goodclass:
					// make it lowercase only
					name = pattern + 2;
					while(name < state.an_input) {
						if(*name >= 'A' && *name <= 'Z') {
							*name |= 0x20;
						}
						name++;
					}
					name = pattern + 2;
					if(state.an_input - pattern == 9) {
						if(mowc::strcmp(name, "alnum", 5) != 0
						&& mowc::strcmp(name, "alpha", 5) != 0
						&& mowc::strcmp(name, "blank", 5) != 0
						&& mowc::strcmp(name, "cntrl", 5) != 0
						&& mowc::strcmp(name, "digit", 5) != 0
						&& mowc::strcmp(name, "graph", 5) != 0
						&& mowc::strcmp(name, "lower", 5) != 0
						&& mowc::strcmp(name, "print", 5) != 0
						&& mowc::strcmp(name, "punct", 5) != 0
						&& mowc::strcmp(name, "space", 5) != 0
						&& mowc::strcmp(name, "upper", 5) != 0) {
							// invalid class name
							f_errno = MO_REGEXPR_ERROR_BADCLASSNAME;
							return true;
						}
					}
					else if(state.an_input - pattern == 10) {
						if(mowc::strcmp(name, "xdigit", 6) != 0
						&& mowc::strcmp(name, "odigit", 6) != 0) {
							// invalid class name
							f_errno = MO_REGEXPR_ERROR_BADCLASSNAME;
							return true;
						}
					}
					else {
						// invalid class name
						f_errno = MO_REGEXPR_ERROR_BADCLASSNAME;
						return true;
					}
				}
			}
			c = AnalyzeGetC(state);
noclassname:;
		} while(c != '\0' && c != ']');
		if(c != ']') {
			// invalid set of characters
			f_errno = MO_REGEXPR_ERROR_NOENDSET;
			return true;
		}
		length = static_cast<unsigned long>(state.an_input - pattern - 1);
		AnalyzeGetC(state);	// skip the ']'
		break;

	case ']':	// ease debugging
		AnalyzeGetC(state);		// skip the ']'
		f_errno = MO_REGEXPR_ERROR_BADSET;
		return true;

	case '{':
		// is this a variable? if not, make use of the {
		// as if it were a standard character
		// a variable is defined as {<name>=<pattern>}
		// name cannot start with 9-0 or , because r{n,m}
		// could also be used...
		s = state.an_input;
		for(;;) {
			switch(*s) {
			case '\0':
			case '\r':
			case '\n':
				// probably r{n,m}
				return false;

			}
			if(*s == '=') {
				break;
			}
			// a valid name is: [A-Za-z_][0-9A-Za-z_]*
			if((*s < 'a' || *s > 'z')
			&& (*s < 'A' || *s > 'Z')
			&& (*s < '0' || *s > '9' || s == state.an_input)
			&& *s < '_')
			{
				// probably r{n,m}
				return false;
			}
		}
		while(*s != '}') {
			switch(*s) {
			case '\0':
			case '\r':
			case '\n':
				// probably r{n,m}
				return false;

			}
		}
		pattern = state.an_input;
		do {
			c = AnalyzeGetC(state);
		} while(c != '=' && c != '}' && c != '\0');
		if(c == '\0') {
			// invalid set of characters
			f_errno = MO_REGEXPR_ERROR_BADVARIABLE;
			return true;
		}
		if(c == '=') {	// read a variable
			length = static_cast<unsigned long>(state.an_input - pattern);
			AnalyzeGetC(state);		// skip the '='
			state.an_level++;
			if(AnalyzeExprOr(state)) {	// get the sub-expressions
				state.an_level--;
				return true;
			}
			state.an_level--;
			if(state.an_char != '}') {
				f_errno = MO_REGEXPR_ERROR_NOENDVARIABLE;
				return true;
			}
			AnalyzeGetC(state);	// skip the '}'
			node = Node(state.an_end);
			state.an_end = node->nd_left = NodeAlloc(MO_REGEXPR_ENDRECORD);
			state.an_start = NodeAlloc(MO_REGEXPR_STARTRECORD, state.an_start, MO_REGEXPR_NO_NODE, pattern, length);
			return false;
		}
		else {		// insert a variable
			AnalyzeGetC(state);	// skip the '}'
			fprintf(stderr, "ERROR: inserting variables isn't supported yet.\n");
			abort();
		}
		break;

	case '}':	// end of variable
		return false;

	case '.':
		type = MO_REGEXPR_NONE;
		pattern = state.an_input - 1;
		*pattern = '\n';
		length = 1;
		AnalyzeGetC(state);	// skip the '.'
		break;

	case '*':	// ease debugging - otherwise it would be taken literally!
	case '+':	// TODO: these could be taken literally if a flag allows that...
	case '?':
		f_errno = MO_REGEXPR_ERROR_BADREPEAT;
		return true;

	default:
		c = state.an_char;
		// mark the end or literal '$' ?
		if(c == '$' && state.an_level == 0) {
			return false;
		}
		// a trailing context or a literal '/' ?
		if(c == '/' && !state.an_after) {
			return false;
		}
		// a variable/parameter definition ?
		if(c == '\\' && *state.an_input == '(') {
			AnalyzeGetC(state);	// skip the '('
			(void) mowc::strtol(state.an_input, &s, 10);
			if(*s == '$') {
				// keep the \(<value>$... as the
				// positional parameter
				// rename as a variable named "v<value>"
				pattern = state.an_input - 1;
				*pattern = 'v';
				length = static_cast<unsigned long>(s - pattern);
				state.an_input = s + 1;
				AnalyzeGetC(state);	// read next character
			}
			else {
				pattern = state.an_input;
				length = 0;
			}
			state.an_level++;
			if(AnalyzeExprOr(state)) {
				state.an_level--;
				return true;
			}
			state.an_level--;
			if(state.an_char != '\\' || *state.an_input != ')') {
				f_errno = MO_REGEXPR_ERROR_NOENDVARIABLE;
				return false;
			}
			node = Node(state.an_end);
			state.an_end = node->nd_left = NodeAlloc(MO_REGEXPR_ENDRECORD);
			state.an_start = NodeAlloc(MO_REGEXPR_STARTRECORD, state.an_start, MO_REGEXPR_NO_NODE, pattern, length);
			return false;
		}
		// a string of characters
		pattern = state.an_input - 1;
		s = pattern;
		n = 0;
		type = MO_REGEXPR_EXACT;
		// repeat until a special character is encountered
		while(c != '\0' && c != '.' && c != '|' && c != '(' && c != ')'
				&& c != '[' && c != ']' && c != '{' && c != '}'
				&& c != '+' && c != '*' && c != '?'
				&& (c != '$' || state.an_level != 0)
				&& (c != '/' || state.an_after)
		) {
			switch(c) {
			case '"':
			case '\'':
				quote = c;
				c = AnalyzeGetC(state);
				while(c != quote && c != '\0') {
					*s++ = c;
					c = AnalyzeGetC(state);
					while(c == '\\') {
						*s++ = mowc::backslash_char(state.an_input);
						c = AnalyzeGetC(state);
					}
				}
				c = AnalyzeGetC(state);
				continue;

			case '\\':
				if(*state.an_input == '('
				|| *state.an_input == ')') {
					// if \( or \) then it is not a literal '\\'
					state.an_input--;
					c = '\0';	// break the loop now
					continue;
				}
				c = mowc::backslash_char(state.an_input);
				if(c == '\0') {
					f_errno = MO_REGEXPR_ERROR_NOENDSET;
					return true;
				}
				break;

			// default: keep all the other characters as is
			}
			// copy the characters since backslashed characters
			// and characters between quotes are moved
			*s++ = c;
			c = AnalyzeGetC(state);
		}
		length = static_cast<unsigned long>(s - pattern);
		break;

	}

	n = NodeAlloc(type, MO_REGEXPR_NO_NODE, MO_REGEXPR_NO_NODE, pattern, length);
	state.an_start = n;
	state.an_end = n;

	return false;
}

bool moRegularExpression::AnalyzeExprRepeat(mo_regexpr_analyze_t& state)
{
	mo_regexpr_node_t	*node;
	int			c, min, max;
	bool			repeat;
	unsigned long		n, p, q;

	// check the unary entries
	if(AnalyzeExprUnary(state)) {
		return true;
	}

	// is it followed by a repeat?
	min = 0;
	max = 0;
	repeat = true;
	switch(state.an_char) {
	case '*':
		// skip the '*'
		AnalyzeGetC(state);
		break;

	case '+':
		// skip the '+'
		AnalyzeGetC(state);
		min = 1;
		break;

	case '?':
		// skip the '?'
		AnalyzeGetC(state);
		max = 1;
		break;

	case '{':
		c = AnalyzeGetC(state);
		while(c >= '0' && c <= '9') {
			min = min * 10 + c - '0';
			c = AnalyzeGetC(state);
		}
		if(c == ',') {
			c = AnalyzeGetC(state);
			while(c >= '0' && c <= '9') {
				max = max * 10 + c - '0';
				c = AnalyzeGetC(state);
			}
		}
		else {
			max = min;
		}
		if(c != '}') {
			// invalid counter (should be {m,n} !?!)
			f_errno = MO_REGEXPR_ERROR_NOENDCOUNT;
			return true;
		}
		if(min > max && max != 0) {
			// invalid counter (should be {m,n} !?!)
			f_errno = MO_REGEXPR_ERROR_BADCOUNT;
			return true;
		}
		// skip the '}'
		AnalyzeGetC(state);
		break;

	default:
		repeat = false;
		break;

	}

	// any repeatition?
	if(repeat) {
		if(min == 0 && max == 0) {	// r*
			q = NodeAlloc(MO_REGEXPR_LINK);
			p = NodeAlloc(MO_REGEXPR_LINK, q, state.an_start);
			n = NodeAlloc(MO_REGEXPR_LINK, q, state.an_start);
			node = Node(state.an_end);
			node->nd_right = p;
			state.an_start = n;
			state.an_end = q;
		}
		else if(min == 1 && max == 0) {	// r+
			p = NodeAlloc(MO_REGEXPR_LINK, MO_REGEXPR_NO_NODE, state.an_start);
			node = Node(state.an_end);
			node->nd_left = p;
			state.an_end = p;
		}
		else if(min == 0 && max == 1) {	// r?
			q = NodeAlloc(MO_REGEXPR_LINK);
			n = NodeAlloc(MO_REGEXPR_LINK, q, state.an_start);
			node = Node(state.an_end);
			node->nd_left = q;
			state.an_start = n;
			state.an_end = q;
		}
		else if(min == 0) {		// r{,m} or r{0,m}
			q = NodeAlloc(MO_REGEXPR_LINK);
			p = NodeAlloc(MO_REGEXPR_LINK, q, state.an_start);
			n = NodeAlloc(MO_REGEXPR_LINK, q, state.an_start);
			node = Node(n);
			node->nd_reset = p;
			node = Node(state.an_end);
			node->nd_left = p;
			node = Node(p);
			node->nd_max = max;
			state.an_start = n;
			state.an_end = q;
		}
		else {				// r{n,m}
			q = NodeAlloc(MO_REGEXPR_LINK);
			p = NodeAlloc(MO_REGEXPR_LINK, q, state.an_start);
			n = NodeAlloc(MO_REGEXPR_LINK, state.an_start);
			node = Node(n);
			node->nd_reset = p;
			node = Node(state.an_end);
			node->nd_left = p;
			node = Node(p);
			node->nd_min = min;
			node->nd_max = max;
			state.an_start = n;
			state.an_end = q;
		}
	}

	return false;
}

bool moRegularExpression::AnalyzeExprConcat(mo_regexpr_analyze_t& state)
{
	unsigned long		start, end;
	mo_regexpr_node_t	*node;

	// check the repeated entries (*+?)
	if(AnalyzeExprRepeat(state)) {
		return true;
	}

	for(;;) {
		// skip all characters that cannot constitute concatenation
		// repeat characters have already been eaten
		switch(state.an_char) {
		case '\0':
		case '|':
		case '/':
		case '^':
		case '$':
		case ')':
		case '}':
		case ']':		// that should generate an error in the Unary() call
			return false;

		}

		// check the repeated entries (*+?)
		start = state.an_start;
		end = state.an_end;
		if(AnalyzeExprRepeat(state)) {
			return true;
		}
		node = Node(end);
		node->nd_left = state.an_start;
		state.an_start = start;
	}
	/*NOTREACHED*/
#ifdef MO_CONFIG_ALWAYS_NEED_RETURN
	return false;
#endif
}

bool moRegularExpression::AnalyzeExprOr(mo_regexpr_analyze_t& state)
{
	unsigned long		start, end, n, q;
	mo_regexpr_node_t	*node;

	// check the concatenated entries (abc)
	if(AnalyzeExprConcat(state)) {
		return true;
	}

	while(state.an_char == '|') {
		// skip the '|'
		AnalyzeGetC(state);
		start = state.an_start;
		end = state.an_end;
		if(AnalyzeExprConcat(state)) {
			return true;
		}
		n = NodeAlloc(MO_REGEXPR_LINK, start, state.an_start);
		q = NodeAlloc(MO_REGEXPR_LINK);
		node = Node(end);
		node->nd_left = q;
		node = Node(state.an_end);
		node->nd_left = q;
		state.an_start = n;
		state.an_end = q;
	}

	return false;
}

bool moRegularExpression::AnalyzeExprEnd(mo_regexpr_analyze_t& state)
{
	unsigned long		n;
	mo_regexpr_node_t	*node;

	// check the different choices (a|b|c)
	if(AnalyzeExprOr(state)) {
		return true;
	}

	if(state.an_char == '$') {
		// skip the '$'
		AnalyzeGetC(state);

		n = NodeAlloc(MO_REGEXPR_END);
		node = Node(state.an_end);
		node->nd_left = n;
		state.an_end = n;
	}

	return false;
}

bool moRegularExpression::AnalyzeExprStart(mo_regexpr_analyze_t& state)
{
	mo_regexpr_node_t	*node;

	if(state.an_char == '^') {
		// skip the '^' character
		AnalyzeGetC(state);
		if(AnalyzeExprEnd(state)) {
			return true;
		}
		state.an_start = NodeAlloc(MO_REGEXPR_START, state.an_start);
	}
	else {
		if(AnalyzeExprEnd(state)) {
			return true;
		}
	}

	node = Node(state.an_end);
	while(node->nd_type == MO_REGEXPR_END && state.an_char == '^') {
		// skip the '^' character
		AnalyzeGetC(state);
		if(AnalyzeExprEnd(state)) {
			return true;
		}
		state.an_start = NodeAlloc(MO_REGEXPR_START, state.an_start);
		node = Node(state.an_end);
	}

	return false;
}

bool moRegularExpression::AnalyzeExprAfter(mo_regexpr_analyze_t& state)
{
	unsigned long		n, start, end;
	mo_regexpr_node_t	*node;

	if(AnalyzeExprStart(state)) {
		return true;
	}

	if(state.an_char == '/') {
		// we found a "test after pattern" separator
		state.an_after = true;
		AnalyzeGetC(state);		// skip the '/'
		start = state.an_start;
		end = state.an_end;
		if(AnalyzeExprStart(state)) {
			return true;
		}
		n = NodeAlloc(MO_REGEXPR_AFTER, state.an_start);
		node = Node(end);
		node->nd_left = n;
		state.an_start = start;
	}

	return false;
}

bool moRegularExpression::AnalyzeExprAll(mo_regexpr_analyze_t& state)
{
	unsigned long		n;
	mo_regexpr_node_t	*node;

	if(AnalyzeExprAfter(state)) {
		return true;
	}

	n = NodeAlloc(MO_REGEXPR_EXIT);
	node = Node(state.an_end);
	node->nd_left = n;
	state.an_end = n;

	return false;
}


/** \brief Analyze the regular expression.
 *
 * This function is called whenever a match with the regular
 * expression is required.
 *
 * \note
 * There are many other sub-functions used by the Analyze()
 * call which are not listed here.
 *
 * \sa Init()
 */
bool moRegularExpression::Analyze()
{
	mo_regexpr_analyze_t	state;

	// already parsed?
	if(f_copy_string != 0 && static_cast<size_t>(f_copy_length) == Length())
	if(mowc::strcmp(Data(), f_copy_string) == 0) {
		return f_invalid;
	}
	f_copy_length = static_cast<uint32_t>(Length());

	// copy the string in our internal buffer
	if(f_copy_length >= f_copy_max && f_copy_string != 0) {
		delete [] f_copy_string;
		delete [] f_buffer;
		f_copy_string = 0;
		f_buffer = 0;
	}
	if(f_copy_string == 0) {
		f_copy_max = (f_copy_length + 256) & -256;
		f_copy_string = new mowc::wc_t[static_cast<size_t>(f_copy_max)];
		f_buffer = new mowc::wc_t[static_cast<size_t>(f_copy_max)];
	}
	mowc::strcpy(f_copy_string, Data());	/* Flawfinder: ignore */
	mowc::strcpy(f_buffer, f_copy_string);	/* Flawfinder: ignore */

	NodeReset();	// restart with first available node
	memset(&state, 0, sizeof(state));
	state.an_input = f_buffer;

//fprintf(stderr, "Analyzing RegExpr: ");

	AnalyzeGetC(state);
	f_errno = MO_REGEXPR_ERROR_NONE;	// no error so far!

	f_invalid = AnalyzeExprAll(state);

	// the start can be any node (see the different an_start = ... statements)
	f_start = state.an_start;

//fprintf(stderr, "\n");	// terminate the reading of the input string

	return f_invalid;
}


/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	MatchExpression - check to know whether a string matches
		this regular expression

SYNOPSIS

	bool MatchExpression(const moWCString& string,
		moVariableList *parameters = 0);

PARAMETERS

	string - the string to be matched
	parameters - returns a list of variables with the
		parameters contents (the different \(r\)
		and {<name>=r} of the expression)

DESCRIPTION

	This function checks a string against a	regular expression.

	When parameters where defined in the regular expression,
	then it will be set accordingly in the parameters list
	of variables. If the parameters is set to zero, then none
	are generated.

	Positional parameters (i.e. \(r\)) are saved in parameters
	as v# where # is the position of the parameter starting at
	1. It is possible to force the position with the syntax:

		\(<pos>$...\)

	The {<name>=r} syntax can also be used to define positional
	parameters with any name. It is legal to use a standard
	positional name such as v1, v2, etc.

	The currently supported regular expressions are:

		.		(any non-special characters)

		[a-z]		classes, including the named classes:
					alnum alpha blank cntrl digit
					graph lower print punct space
					upper xdigit odigit.
				Use ! or ^ as the very first character
				to invert the mean (i.e. except these
				characters.) Start the class with a
				] to include a ] in the class: [^]]
				is the class with any characters except
				']'. Use the dash - to include a range.
				Use the dash as the first or last
				character in the class to include the
				dash as a character in the class.

		r+		repeat at least once

		r?		once or not

		r*		repeat any number of time

		r{n}		repeat exactly n times

		r{n,m}		repeat at least n and at most m times;
				n and m are optional though at least one
				is needed

		r|r		one or the other

		^r		must start at the beginning

		r$		must stop at the end

		(r)		group expressions

		\(r\)		record r in v#, # increase automatically

		\(<pos>$r\)	record r in v<pos>

		{<name>=r}	record r in <name>

		'<string>'	include <string> as is; remove the '
				use \c inside such strings to include a '

		"<string>"	include <string> as is; remove the "
				use \c inside such strings to include a "

		\c		insert character c as is even if it is a
				special character (unless is is the end of
				the string.)

	Note: if you need to match \(, then use '\\(' or "\\(".

NOTE

	This function should be a constant function, however,
	to gain time, the moWCString Set() function doesn't
	generate a parse of the regular expression. This is
	done only once the expression is used, thus this
	function will create the list of nodes if the string
	of the regular expression has changed.

ERRORS

	This function calls sub-function which allocate buffers
	and may throw an std::bad_alloc exception.

	Also, it uses a stack which has a limited size. If that
	limit is reached, then an MO_ERROR_OVERFLOW error will
	be generated.

	If the analyzer somehow failed to properly parse and
	generated bogus nodes, then this function can also
	return an MO_ERROR_INVALID error.

RETURN VALUE

	true if the string matches this regular expression

SEE ALSO

	Analyze, Replace

*/
bool moRegularExpression::MatchMore(mo_regexpr_match_t& state)
{
	mo_regexpr_node_t	*node, *reset, *start_node;
	const mowc::wc_t	*pattern, *start;
	long			length;
	mowc::wc_t		c, from, to;
	bool			found;

//const mowc::wc_t *string, unsigned long n, unsigned long level

	if(state.ma_level > 200) {
		// too many levels -- generate a stack overflow
		throw moError(MO_ERROR_OVERFLOW, "Stack overflow");
	}

	if(state.ma_level == 0) {
		start = state.ma_string;
	}
	else {
		start = 0;
	}

	do {
		node = Node(state.ma_n);

#if 0
fprintf(stderr, "Node #%lu Level #%lu String: [%s]\n", state.ma_n, state.ma_level, state.ma_string);
fprintf(stderr, "node.nd_type     = %d\n", node->nd_type);
fprintf(stderr, "    .nd_left     = %ld\n", node->nd_left);
fprintf(stderr, "    .nd_right    = %ld\n", node->nd_right);
fprintf(stderr, "    .nd_reset    = %ld\n", node->nd_reset);
fprintf(stderr, "    .nd_length   = %ld\n", node->nd_length);

fprintf(stderr, "    .nd_pattern  = [");
int i;
for(i = 0; i < (int) node->nd_length; i++) {
	switch(node->nd_pattern[i]) {
	case '\n':
		fprintf(stderr, "\\n");
		break;

	case '\r':
		fprintf(stderr, "\\r");
		break;

	case '\f':
		fprintf(stderr, "\\f");
		break;

	case '\t':
		fprintf(stderr, "\\t");
		break;

	case ' ':
		fprintf(stderr, "\\s");
		break;

	case '\033':
		fprintf(stderr, "\\?");
		break;

	default:
		fprintf(stderr, "%c", node->nd_pattern[i]);
		break;

	}
}
fprintf(stderr, "]\n");

fprintf(stderr, "    .nd_count    = %ld\n", node->nd_count);
fprintf(stderr, "    .nd_min      = %ld\n", node->nd_min);
fprintf(stderr, "    .nd_max      = %ld\n", node->nd_max);
fprintf(stderr, "\n");
#endif

		// need to reset a node from here?
		if(node->nd_reset) {
			reset = Node(node->nd_reset);
			reset->nd_count = 0;
		}

		switch(node->nd_type) {
		case MO_REGEXPR_EXIT:
			// we matched if we are at the end of the string
			// note that the test is done later
		case MO_REGEXPR_LINK:
			// no need to match anything...
			break;

		case MO_REGEXPR_EXACT:
			if(mowc::strcmp(state.ma_string, node->nd_pattern, node->nd_length) != 0) {
				return false;
			}
			state.ma_string += node->nd_length;
			break;

		case MO_REGEXPR_ANY:
		case MO_REGEXPR_NONE:
			c = *state.ma_string;
			if(c != '\0') {
				state.ma_string++;
			}
			found = false;
			pattern = node->nd_pattern;
			length = node->nd_length;
			while(length > 0) {
				// do we have a range?
				if(length > 8
				&& pattern[0] == '['
				&& pattern[1] == ':') {
					// a class!
					if(mowc::strcmp(pattern, "[:alnum:]", 9) == 0) {
						if(mowc::isalnum(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:alpha:]", 9) == 0) {
						if(mowc::isalpha(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
#ifdef __USE_GNU
					else if(mowc::strcmp(pattern, "[:blank:]", 9) == 0) {
						if(mowc::isblank(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
#endif
					else if(mowc::strcmp(pattern, "[:cntrl:]", 9) == 0) {
						if(mowc::iscntrl(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:digit:]", 9) == 0) {
						if(mowc::isdigit(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:graph:]", 9) == 0) {
						if(mowc::isgraph(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:lower:]", 9) == 0) {
						if(mowc::islower(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:print:]", 9) == 0) {
						if(mowc::isprint(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:punct:]", 9) == 0) {
						if(mowc::ispunct(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:space:]", 9) == 0) {
						if(mowc::isspace(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:upper:]", 9) == 0) {
						if(mowc::isupper(c)) {
							found = true;
							break;
						}
						pattern += 9;
						length -= 9;
					}
					else if(mowc::strcmp(pattern, "[:xdigit:]", 10) == 0) {
						if(mowc::isxdigit(c)) {
							found = true;
							break;
						}
						pattern += 10;
						length -= 10;
					}
					else if(mowc::strcmp(pattern, "[:odigit:]", 10) == 0) {
						if(mowc::isodigit(c)) {
							found = true;
							break;
						}
						pattern += 10;
						length -= 10;
					}
					else {
						// if we are getting here,
						// there is a mismatch with the analyzer!
						throw moError(MO_ERROR_INVALID, "Invalid pattern name");
					}
				}
				else {
					if(length > 2
					&& pattern[1] == '-') {
						from = pattern[0];
						to = pattern[2];
						pattern += 3;
						length -= 3;
					}
					else {
						from = pattern[0];
						to = from;
						pattern++;
						length--;
					}
					if(c >= from && c <= to) {
						found = true;
						break;
					}
				}
			}

//fprintf(stderr, "Checked ANY (?) and got: %d ^ %d\n",
//			found, node->nd_type == MO_REGEXPR_ANY);

			if(found ^ (node->nd_type == MO_REGEXPR_ANY)) {
				return false;
			}
			break;

		case MO_REGEXPR_START:
			if(state.ma_string != start) {
				return false;
			}
			break;

		case MO_REGEXPR_END:
			// \r\n is equivalent to just \r or just \n in this case
			if(state.ma_string[0] == '\r' && state.ma_string[1] == '\n') {
				state.ma_string++;
			}
			if(*state.ma_string != '\0' && *state.ma_string != '\n' && *state.ma_string != '\r') {
				return false;
			}
			break;

		case MO_REGEXPR_STARTRECORD:
			// start recording a variable (\(...\) or {var=...})
			// TODO: support variable recursivity (i.e. {a=...{b=...}}
			if(state.ma_start_record != 0) {
				throw moError(MO_ERROR_OVERFLOW, "Variable recursivity not supported yet.");
			}
			state.ma_start_node = state.ma_n;
			state.ma_start_record = state.ma_string;
			break;

		case MO_REGEXPR_ENDRECORD:
		{
			moWCString var;

			// end recording a variable
			// f_start_record to string is the content of the variable
			if(state.ma_start_node == 0
			|| state.ma_start_record == 0
			|| state.ma_start_record > state.ma_string) {
				throw moError(MO_ERROR_INVALID, "Variable recording ended but not started.");
			}
			// no need to waste our time of the user did not supply a list of parameters
			if(state.ma_parameters != 0) {
				start_node = Node(state.ma_start_node);
				pattern = start_node->nd_pattern;
				length = start_node->nd_length;

				if(length > 0) {
					// a named parameter (can be a forced positional)
					var.Set(pattern, length);
				}
				else {
					// a positional parameter
					var = moWCString::Format("v%d", state.ma_position);
					++state.ma_position;
				}

				var += "=";
				var.Append(state.ma_start_record, static_cast<int>(state.ma_string - state.ma_start_record));

				// add to the list of parameters
				*state.ma_parameters += var;
			}

			state.ma_start_node = 0;
			state.ma_start_record = 0;
		}
			break;

		default:
			// an invalid node, should never happen
			throw moError(MO_ERROR_INVALID, "Invalid node");

			//MO_REGEXPR_AFTER,		// checks after the pattern, don't eat the input (r/t)
		}

		if(node->nd_left == MO_REGEXPR_NO_NODE) {
			state.ma_n = node->nd_right;
		}
		else if(node->nd_right != MO_REGEXPR_NO_NODE) {
			// check going on the right first
			if(node->nd_min != 0 || node->nd_max != 0) {
				node->nd_count++;
			}
			if(*state.ma_string != '\0'		// can't go on at the end of string
			&& (node->nd_count < node->nd_max || node->nd_max == 0)) {
				// NOTE: we do not need to have the level ++/-- immuned to throws
				++state.ma_level;
				// NOTE: we do not need to save ma_n since it is overwritten
				//	 with nd_left below.
				state.ma_n = node->nd_right;
				// save the string since if MatchMore fails we need to not lose
				// the current position!
				pattern = state.ma_string;
				found = MatchMore(state);
				--state.ma_level;
				if(found) {
					return true;
				}
				state.ma_string = pattern;
			}
			// we couldn't match enough times
			if(node->nd_count < node->nd_min && node->nd_min != 0) {
				return false;
			}
			state.ma_n = node->nd_left;
		}
		else {
			state.ma_n = node->nd_left;
		}

	} while(state.ma_n != MO_REGEXPR_NO_NODE);

	// Argh! I changed that a while back to testing
	// the end of the string... It seems wrong now
	// because then you need to add .* at the start/end
	// to match something in the middle of the input
	// string -- this may generate some problems in
	// unexpected places!
	return true;	//*state.ma_string == '\0';
}


bool moRegularExpression::MatchNow(mo_regexpr_match_t& state)
{
	// the end is at least equal to the start
	f_end_pos = f_start_pos;

	// for variables between {} and \(\)
	state.ma_start_node = 0;
	state.ma_start_record = 0;
	state.ma_position = 1;
	if(state.ma_parameters != 0) {
		// make sure we start from scratch each time
		state.ma_parameters->Empty();
	}

	// reset the parser
	state.ma_n = f_start;
	state.ma_level = 0;
	return MatchMore(state);
}


bool moRegularExpression::Match(mo_regexpr_match_t& state)
{
	mo_regexpr_node_t	*node;
	const mowc::wc_t	*string;

	f_start_pos = 0;
	f_end_pos = 0;
	string = state.ma_string;

	node = Node(f_start);
	if(node->nd_type == MO_REGEXPR_START) {
		// it has to match from the start
		if(!MatchNow(state)) {
			f_start_pos = -1;
			f_end_pos = -1;
			return false;
		}
		f_end_pos = static_cast<int32_t>(state.ma_string - string);
		return true;
	}

	// If it doesn't match at the start, try again
	// from the next character until it either matches
	// or we find the end of string.
	while(string[f_start_pos] != '\0') {
		state.ma_string = string + f_start_pos;
		if(MatchNow(state)) {
			f_end_pos = static_cast<int32_t>(state.ma_string - string);
			return true;
		}
		++f_start_pos;
	}

	f_start_pos = -1;
	f_end_pos = -1;
	return false;
}


bool moRegularExpression::MatchExpression(const moWCString& string, moVariableList *parameters)
{
	if(Analyze()) {
		return false;
	}

//fprintf(stderr, "Match [%s] & [%s]...\n", MBData(), string.MBData());

	mo_regexpr_match_t	state;
	memset(&state, 0, sizeof(state));

	state.ma_parameters = parameters;
	state.ma_string = string.Data();

	return moRegularExpression::Match(state);
}



/************************************************************ DOC:

CLASS

	moRegularExpression

NAME

	MatchExpression - check to know whether a string matches
		this regular expression

SYNOPSIS

	bool MatchExpression(const moWCString& string,
		moVariableList *parameters = 0);

PARAMETERS

	replacement - how to replace the matching area
	subject - the string to be matched

DESCRIPTION

	This function searches the input string, subject, and
	applies replacement to the matching part.

	If string does not match expression, then this function
	returns subject unchanged.

	See MatchExpression() for more information about possible
	errors and supported regular expressions.

RETURN VALUE

	A new string with replacement applied to the part of
	the subject string matching this expression

SEE ALSO

	Analyze, MatchExpression

*/
moWCString moRegularExpression::Replace(const moWCString& replacement, const moWCString& subject)
{
// analyze
	if(Analyze()) {
		throw moError(MO_REGEXPR_ERROR_BADEXPRESSION, "Invalid expression; cannot apply Replace().");
	}

// check for a match
	moVariableList		parameters("moRegularExpression::Replace()");
	mo_regexpr_match_t	state;
	memset(&state, 0, sizeof(state));

	state.ma_parameters = &parameters;
	state.ma_string = subject.Data();

	bool match = moRegularExpression::Match(state);
	if(!match || f_start_pos < 0 || f_end_pos < 0) {
		// did not match, nothing to replace!
		return subject;
	}

// it matched, let's replace!

	// first put the start of subject as is
	moWCString result(subject, f_start_pos);

	// here apply the replacement
	const mowc::wc_t *r = replacement.Data();
	while(*r != '\0') {
		switch(*r) {
		case '\\':	// insert positional
			if(r[1] >= '1' && r[1] <= '9') {
				moWCString name("v");
				name += r[1];
				moVariableSPtr var = parameters.Get(name);
				if(var) {
					result += *var;
				}
				r += 2;
			}
			else if(r[1] == '0') {
				result += subject.Get(f_start_pos, f_end_pos - 1);
				r += 2;
			}
			else if(r[1] == '\0') {
				r += 1;
			}
			else {
				// not a positional, insert r[1] as is
				result += r[1];
				r += 2;
			}
			break;

		case '$':
			if(r[1] == '{') {
				moWCString name;
				// positional? (starts with a digit)
				if(r[2] >= '0' && r[2] <= '9') {
					name += "v";
				}
				// should we forbid spaces in names?
				while(*r != '}' && *r != '\0' && *r != '\r' && *r != '\n') {
					name += *r;
					++r;
				}
				if(*r != '}') {
					throw moError(MO_REGEXPR_ERROR_NOENDVARIABLE, "Invalid back reference in replacement string (unterminated).");
				}
				// skip the '}'
				++r;
				moVariableSPtr var = parameters.Get(name);
				if(var) {
					result += *var;
				}
			}
			else if(r[1] >= '1' && r[1] <= '9') {
				moWCString name("v");
				name += r[1];
				moVariableSPtr var = parameters.Get(name);
				if(var) {
					result += *var;
				}
				r += 2;
			}
			else {
				result += '$';
				++r;
			}
			break;

		case '&':	// insert whole matching area
			result += subject.Get(f_start_pos, f_end_pos - 1);
			++r;
			break;

		default:
			result += *r;
			++r;
			break;

		}
	}

	// last append the end of subject as is
	result += subject.Get(f_end_pos, -1);

	return result;
}




};			// namespace molib;

// vim: ts=8

