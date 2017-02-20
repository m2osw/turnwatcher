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
#pragma implementation "mo/mo_expr.h"
#endif

#include	"mo/mo_expr.h"
#include	"mo/mo_regexpr.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{

#ifdef _MSC_VER
// avoid warnings about the use of ^ without parenthesis (I think I know)
#pragma warning(disable: 4554)
#endif


/************************************************************ DOC:

CLASS

	moExpr

NAME

	Constructor - create an expression string

SYNOPSIS

	moExpr(void);
	moExpr(const moWCString& string);
	moExpr(const moExpr& expr);
	moExpr(const char *string, long length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
	moExpr(const mowc::mc_t *string, long length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UCS2_INTERNAL);
	moExpr(const mowc::wc_t *string, long length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UCS4_INTERNAL);
	moExpr(const wchar_t *string, long length = -1);

DESCRIPTION

	These constructors are similar to the moWCString() constructors.

	Please, refer to the moWCString class for more information.

SEE ALSO

	Result()

*/
moExpr::moExpr(void)
{
}

moExpr::moExpr(const moWCString& string)
	: moWCString(string)
{
}

moExpr::moExpr(const moExpr& expr)
	: moWCString(expr)
{
}

moExpr::moExpr(const char *string, long length, mowc::encoding_t encoding)
	: moWCString(string, length, encoding)
{
}

moExpr::moExpr(const mowc::mc_t *string, long length, mowc::encoding_t encoding)
	: moWCString(string, length, encoding)
{
}

moExpr::moExpr(const mowc::wc_t *string, long length, mowc::encoding_t encoding)
	: moWCString(string, length, encoding)
{
}

moExpr::moExpr(const wchar_t *string, long length)
	: moWCString(string, length)
{
}



/************************************************************ DOC:

CLASS

	moExpr

NAME

	Error - return the last error

SYNOPSIS

	mo_expr_errno_t Error(void) const;

DESCRIPTION

	This function returns the last error the last Result() call
	generated.

RETURN VALUE

	one of MO_EXPR_ERROR_... values; NONE (0) means no error occured.

BUG

	Before a call to the Result() function, the error number is not
	defined.

SEE ALSO

	Result()

*/
moExpr::mo_expr_errno_t moExpr::Error(void) const
{
	return f_errno;
}


/************************************************************ DOC:

CLASS

	moExpr

NAME

	Result - computes a C expression

SYNOPSIS

	moWCString Result(void) const;

DESCRIPTION

	The Result() function transforms the C expression present in the
	moExpr string object into a string with the result of the C
	expression.

	At this time, it supports the following types:

	identifiers	[a-zA-Z_][a-zA-Z_0-9]*
	integers	[0-9]+
	floats		[0-9]+\.[0-9]+([eE][-+][0-9]+)?
	strings		\"([^"]|\\\")\"
	character	\'...\'		C like 8 bit characters

	Identifiers are used to:
	
	1. set variables and read them later (a = 5; b = a * 9),

	2. function names: sin(3.14159),

	3. casts: (long) "134"

	The following are all the expression in their priority order
	that are currently supported in this function:

	Operator	Name			Types supported

	<expr>++	postfix increment	integer, float
	<expr>--	postfix decrement	integer, float

	++<expr>	prefix increment	integer, float
	--<expr>	prefix increment	integer, float

	!<expr>		logical not		integer, float, string
						see String.Reverse()
	~<expr>		bitwise not		integer, string
						see String.Switchcase()
	+<expr>		identity		integer, float, string
						see String.Uppercase()
	-<expr>		negate			integer, float, string
						see String.Lowercase()
	(<expr>)	grouping		any
	(<type>) <expr>	cast			any
						<type> = int, long
							float, double
							string
	<expr>(<expr>)	function call		see the list of supported
						functions below
	<expr>[<expr>]	array			string
		also accepts:	<expr>[<expr>..<expr>]
				<expr>[<expr>..]
				<expr>[..<expr>]
			(negative values are indexes from the end)

	<expr>**<expr>	power			integer, float

	<expr>*<expr>	multiply		integer, float
	<expr>/<expr>	divide			integer, float
	<expr>%<expr>	modulo			integer, float

	<expr>+<expr>	add			integer, float, string
						see String.operator +
	<expr>-<expr>	substract		integer, float

	<expr> << <expr>	shift left	integer
	<expr> >> <expr>	shift right	integer

	<expr> < <expr>		less than	integer, float, string
	<expr> <= <expr>	less or equal	integer, float, string
	<expr> > <expr>		greater than	integer, float, string
	<expr> >= <expr>     greater or equal	integer, float, string

	<expr> == <expr>	equal		integer, float, string
	<expr> != <expr>	not equal	integer, float, string

	<expr>&<expr>	bitwise and		integer

	<expr>^<expr>	bitwise xor		integer

	<expr>|<expr>	bitwise or		integer

	<expr>&&<expr>	bitwise and		integer, float, string

	<expr>^^<expr>	bitwise xor		integer, float, string

	<expr>||<expr>	bitwise or		integer, float, string

	<expr>?<expr>:<expr>	conditional	integer, float, string

	<expr> <assign op> <expr>  assign	identifier = anything
		the assign operators are any of:
			**=, *=, /=, %=, +=, -=, <<=, >>=, &=,
			|=, ^=

	<expr>,<expr>	comma			any

	<expr>;<expr>	end expression		any

	Notes:

	The comma separator can be used within a list of parameter
	for a function call as in regular C/C++.

	The following is a list of the functions supported by this
	object so far:

	atol()		accept a string as parameter; transforms it
			to an integer

	basename()	returns the base name of the first parameter;
			when a second parameter is specified, make
			sure the first doesn't end with it (that is,
			if it does, that part - extension - is also
			removed)

	capitalize_words()
			force the first letter of each word to an
			uppercase and all the others to lowercase

	capitalize()	force the very first letter to an uppercase
			letter and all the others to lowercase

	clip()		returns the string without any spaces at the
			start and at the end (see trim() also)

	included()	accept 2 or more parameters; it returns true
			if the first parameter is equal to any of
			the following parameters

			  Note: at this time only integers are
				supported

	lower()		change all the letters to lowercase letters

	regexpr()	check whether parameter 2 matches the regular
			expression defined in parameter 1; if the regular
			expression does match then this function returns
			true, otherwise it returns false

			the following parameters are strings giving
			variable names which are set to whatever
			parameter fits in as defined in the regular
			expression (see the \(...\))
			(this last feature is not supported yet)

	reg_replace()	parameter 1 is a regular expression to run against
			parameter 3 that is the input string and apply the
			replacements as defined in parameter 2.

	replace()	the first parameter is a string which represents
			a list of pairs separated by semi-colons; each
			pair consists of a string element to replace
			and what to replace it with the string element
			and its replacement have to be separated by an
			equal sign; as many pairs as you need can be
			included; example:

			  "'=<single quote>;\"=<double quote>"

			  This replaces each instance of ' and " by
			  the full name of the quote.

			the second parameter to this function is the
			string on which these replacements are applied;
			the result is the string with the characters
			replaced

	reverse()	reverse the order; swap the first and last letters,
			the second and one before last, etc.

	strf()		format a list of parameters; the first parameter
			is the format (as in printf(3C) under Linux);
			the following parameters are used as specified
			in the format; the result is the formatted
			string

	strlen()	return the length of a string in characters

	switch_case()	change all the lowercase letters in uppercase
			and vice versa

	trim()		trim the specified string from space and newline
			characters from the beginning and the end of the
			string (like clip() but includes the new lines)

	upper()		change all the letters to uppercase letters

RETURN VALUE

	a string with the result
	or "ERROR" to indicate that some error occured

BUGS

	You can't distinguish a real error from an expression which
	itself returns the string ERROR.

SEE ALSO

	Constructors

*/
enum mo_expr_token_t {
	// ! ~ + -
	// ( )
	// ... ( ... )
	// ... [ ... ]
	// * / %
	// + -
	// ^ & |
	// ?:
	// < >
	// =
	// ,
	// ;
	MO_EXPR_TOKEN_UNKNOWN = -3,	// an undefined value
	MO_EXPR_TOKEN_ERROR = -2,	// an invalid token was found
	MO_EXPR_TOKEN_EOS = -1,		// end of string reached
	MO_EXPR_TOKEN_IDENTIFIER = 256,	// [A-Za-z_][A-Za-z_0-9]*
	MO_EXPR_TOKEN_INTEGER,		// [0-9]+
	MO_EXPR_TOKEN_FLOAT,		// [0-9]*\.[0-9]+([eE](+-)?[0-9]+)?
	MO_EXPR_TOKEN_STRING,		// "..."
	MO_EXPR_TOKEN_EQUAL,		// ==
	MO_EXPR_TOKEN_NOT_EQUAL,	// !=
	MO_EXPR_TOKEN_LESS_EQUAL,	// <=
	MO_EXPR_TOKEN_GREATER_EQUAL,	// >=
	MO_EXPR_TOKEN_LOGICAL_AND,	// &&
	MO_EXPR_TOKEN_LOGICAL_OR,	// ||
	MO_EXPR_TOKEN_LOGICAL_XOR,	// ^^
	MO_EXPR_TOKEN_INCREMENT,	// ++
	MO_EXPR_TOKEN_DECREMENT,	// --
	MO_EXPR_TOKEN_POWER,		// **
	MO_EXPR_TOKEN_SHIFT_LEFT,	// <<
	MO_EXPR_TOKEN_SHIFT_RIGHT,	// >>
	MO_EXPR_TOKEN_ASSIGN_POW,	// **=
	MO_EXPR_TOKEN_ASSIGN_MUL,	// *=
	MO_EXPR_TOKEN_ASSIGN_DIV,	// /=
	MO_EXPR_TOKEN_ASSIGN_MOD,	// %=
	MO_EXPR_TOKEN_ASSIGN_ADD,	// +=
	MO_EXPR_TOKEN_ASSIGN_SUB,	// -=
	MO_EXPR_TOKEN_ASSIGN_SHL,	// <<=
	MO_EXPR_TOKEN_ASSIGN_SHR,	// >>=
	MO_EXPR_TOKEN_ASSIGN_AND,	// &=
	MO_EXPR_TOKEN_ASSIGN_OR,	// |=
	MO_EXPR_TOKEN_ASSIGN_XOR,	// ^=
	MO_EXPR_TOKEN_RANGE,		// ..	(for arrays)
	MO_EXPR_TOKEN_max
};


struct mo_expr_value_t {
	moWCString		name;		// a variable name
	mo_expr_token_t		value_type;	// one of MO_EXPR_TOKEN_INTEGER, FLOAT or STRING
	long			value_int;	// defined when value_type is MO_EXPR_TOKEN_INTEGER
	double			value_flt;	// defined when value_type is MO_EXPR_TOKEN_FLOAT
	moWCString		value_str;	// defined when value_type is MO_EXPR_TOKEN_STRING

		mo_expr_value_t(void)
		{
			//name -- auto-init
			value_type = MO_EXPR_TOKEN_UNKNOWN;
			value_int = 0;
			value_flt = 0.0;
			//value_str -- auto-init
		}
};

#define	MO_EXPR_VARIABLE_MAX		50
#define	MO_EXPR_PARAM_MAX		5	// maximum number of parameters for internal functions

struct mo_expr_state_t {
	const mowc::wc_t *	input;		// input stream
	const mowc::wc_t *	start;		// start of current token
	const mowc::wc_t *	end;		// end of current token
	mo_expr_token_t		token;		// current token type
	mo_expr_value_t		value;		// the current value of this state
	long			param_count;	// the number of valid parameters
	mo_expr_value_t		param[MO_EXPR_PARAM_MAX];	// function parameters
	long			count;		// number of variables
	mo_expr_value_t *	variable;	// a list of variables (we use a pointer so sub-states access the same variables)
	mo_expr_state_t *	parent;		// the parent state

		mo_expr_state_t(void)
		{
			// parent structure initialization
			input = 0;
			start = 0;
			end = 0;
			token = MO_EXPR_TOKEN_UNKNOWN;
			param_count = 0;
			count = 0;
			variable = 0;
			parent = 0;
		}

		mo_expr_state_t(const mo_expr_state_t& state)
		{
			// any sub-state initialization
			input = state.input;
			start = state.start;
			end = state.end;
			token = MO_EXPR_TOKEN_UNKNOWN;
			param_count = 0;
			count = state.count;
			variable = state.variable;
			parent = const_cast<mo_expr_state_t *>(&state);
		}

		~mo_expr_state_t()
		{
			if(parent != 0) {
				// a sub-state destruction!
				parent->input = input;
				parent->start = start;
				parent->end = end;
				parent->token = token;
				parent->count = count;
			}
		}
};


static	moExpr::mo_expr_errno_t		expr_comma(mo_expr_state_t& state, bool gettoken);
static	moExpr::mo_expr_errno_t		expr_prefix_inc(mo_expr_state_t& state, bool gettoken);
static	moExpr::mo_expr_errno_t		expr_assign(mo_expr_state_t& state, bool gettoken);



static moExpr::mo_expr_errno_t expr_read_token(mo_expr_state_t& state)
{
	mowc::wc_t	c;
	long		cnt, value;

	// skip spaces
	do {
		c = *state.input++;
	} while(isspace(c));

	if(c == '\0') {
		state.input--;
		state.start = state.input;
		state.token = MO_EXPR_TOKEN_EOS;
		return moExpr::MO_EXPR_ERROR_NONE;
	}

	state.start = state.input - 1;

	switch(c) {
	case '=':
		if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_EQUAL;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '!':
		if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_NOT_EQUAL;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '<':
		if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_LESS_EQUAL;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		if(*state.input == '<') {
			state.input++;
			if(*state.input == '=') {
				state.input++;
				state.token = MO_EXPR_TOKEN_ASSIGN_SHL;
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			state.token = MO_EXPR_TOKEN_SHIFT_LEFT;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '>':
		if(*state.input == '=') {
			state.input++;
			if(*state.input == '=') {
				state.input++;
				state.token = MO_EXPR_TOKEN_ASSIGN_SHR;
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			state.token = MO_EXPR_TOKEN_GREATER_EQUAL;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		if(*state.input == '>') {
			state.input++;
			state.token = MO_EXPR_TOKEN_SHIFT_RIGHT;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '&':
		if(*state.input == '&') {
			state.input++;
			state.token = MO_EXPR_TOKEN_LOGICAL_AND;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		else if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_AND;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '|':
		if(*state.input == '|') {
			state.input++;
			state.token = MO_EXPR_TOKEN_LOGICAL_OR;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		else if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_OR;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '^':
		if(*state.input == '^') {
			state.input++;
			state.token = MO_EXPR_TOKEN_LOGICAL_XOR;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		else if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_XOR;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '+':
		if(*state.input == '+') {
			state.input++;
			state.token = MO_EXPR_TOKEN_INCREMENT;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		else if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_ADD;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '-':
		if(*state.input == '-') {
			state.input++;
			state.token = MO_EXPR_TOKEN_DECREMENT;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		else if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_SUB;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '.':
		if(*state.input == '.') {
			state.input++;
			state.token = MO_EXPR_TOKEN_RANGE;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '*':
		if(*state.input == '*') {
			state.input++;
			if(*state.input == '=') {
				state.input++;
				state.token = MO_EXPR_TOKEN_ASSIGN_POW;
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			state.token = MO_EXPR_TOKEN_POWER;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		else if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_MUL;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '/':
		if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_DIV;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	case '%':
		if(*state.input == '=') {
			state.input++;
			state.token = MO_EXPR_TOKEN_ASSIGN_MOD;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		break;

	}

	if((c >= 'a' && c <= 'z')
	|| (c >= 'A' && c <= 'Z')
	|| c == '_') {
		// identifier
		c = *state.input;
		while((c >= 'a' && c <= 'z')
		   || (c >= 'A' && c <= 'Z')
		   || (c >= '0' && c <= '9')
		   || c == '_') {
			c = *++state.input;
		}
		state.token = MO_EXPR_TOKEN_IDENTIFIER;
		return moExpr::MO_EXPR_ERROR_NONE;
	}

	if(c >= '0' && c <= '9') {
		// integer or float
		c = *state.input;
		while(c >= '0' && c <= '9') {
			c = *++state.input;
		}
		if(c != '.') {	// float?
			state.token = MO_EXPR_TOKEN_INTEGER;
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		if(state.input[1] < '0' || state.input[1] > '9') {
			if(state.input[1] == '.') {
				// this is an integer followed by a range!
				state.token = MO_EXPR_TOKEN_INTEGER;
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			// a float without decimal digits is invalid
			state.token = MO_EXPR_TOKEN_ERROR;
			return moExpr::MO_EXPR_ERROR_INVALID_FLOAT;
		}
		state.input++;		// skip the '.'
	}

	if(c == '.' && (*state.input >= '0' && *state.input <= '9')) {
		// float
		c = *state.input;
		while(c >= '0' && c <= '9') {
			c = *++state.input;
		}
		if(c == 'e' || c == 'E') {
			c = *++state.input;
			if(c == '+' || c == '-') {
				c = *++state.input;
			}
			// here we must have at least one digit
			if(c < '0' || c > '9') {
				state.token = MO_EXPR_TOKEN_ERROR;
				return moExpr::MO_EXPR_ERROR_INVALID_FLOAT;
			}
			while(c >= '0' && c <= '9') {
				c = *++state.input;
			}
		}
		state.token = MO_EXPR_TOKEN_FLOAT;
		return moExpr::MO_EXPR_ERROR_NONE;
	}

	if(c == '"') {
		// string
		c = *state.input++;
		while(c != '"' && c != '\0') {
			if(c == '\\' && state.input[0] != '\0') {
				// ignore this character
				state.input++;
			}
			c = *state.input++;
		}
		if(c != '"') {
			state.token = MO_EXPR_TOKEN_ERROR;
			return moExpr::MO_EXPR_ERROR_INVALID_STRING;
		}
		state.token = MO_EXPR_TOKEN_STRING;
		return moExpr::MO_EXPR_ERROR_NONE;
	}

	if(c == '\'') {
		// character
		c = *state.input++;
		if(c == '\\') {
			c = *state.input++;
			switch(c) {
			case 'a':	// bell
				c = 7;
				break;

			case 'b':	// backspace
				c = '\b';
				break;

			case 'e':	// escape (also \?)
				c = 27;
				break;

			case 'f':	// form feed
				c = '\f';
				break;

			case 'n':	// new line
				c = '\n';
				break;

			case 'r':	// carriage return
				c = '\r';
				break;

			case 't':	// horizontal tab
				c = '\t';
				break;

			case 'v':	// vertical tab
				c = '\v';
				break;

			case '?':	// escape (also \e)
				c = 27;
				break;

			case '0':	// octal or hexadecimal
				c = *state.input;
			case 'x':	// hexadecimal
			case 'X':
				cnt = 0;
				value = 0;
				if(c == 'x' || c == 'X') {
					// expect hexadecimal (1 or 2 digits)
					c = *++state.input;
					while(((c >= '0' && c <= '9')
					    || (c >= 'A' && c <= 'F')
					    || (c >= 'a' && c <= 'f'))
					    && cnt < 2) {
						value <<= 4;
						if(c >= '0' && c <= '9') {
							value |= value - '0';
						}
						else if(c >= 'A' && c <= 'F') {
							value |= value - ('A' + 10);
						}
						else if(c >= 'a' && c <= 'f') {
							value |= value - ('a' + 10);
						}
						c = *++state.input;
						cnt++;
					}
				}
				else {
					// expect octal (1 to 3 digits)
					while(c >= '0' && c <= '7' && cnt < 3) {
						value <<= 3;
						value |= c & 7;
						c = *++state.input;
						cnt++;
					}
				}
				c = static_cast<char>(value);
				break;

			// default: keep other characters as is
			}
		}
		else if(c == '\'') {
			// error, we can't have an empty character
			state.token = MO_EXPR_TOKEN_ERROR;
			return moExpr::MO_EXPR_ERROR_INVALID_CHARACTER;
		}
		if(*state.input != '\'') {
			// error, character isn't closed!
			state.token = MO_EXPR_TOKEN_ERROR;
			return moExpr::MO_EXPR_ERROR_INVALID_CHARACTER;
		}
		state.input++;	// skip the ending quote (')
	}

	state.token = (mo_expr_token_t) c;

	return moExpr::MO_EXPR_ERROR_NONE;
}


static moExpr::mo_expr_errno_t expr_token(mo_expr_state_t& state)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_read_token(state);
	state.end = state.input;

	return r;
}



static moExpr::mo_expr_errno_t expr_unary(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;
	long			l, idx;
	char			buf[256];	/* Flawfinder: ignore */

	if(get_token) {
		r = expr_token(state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
	}

	switch(static_cast<int>(state.token)) {
	case '!':	// logical not
		r = expr_prefix_inc(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			state.value.value_int = !state.value.value_int;
			break;

		case MO_EXPR_TOKEN_FLOAT:
			state.value.value_int = state.value.value_flt == 0.0;
			state.value.value_type = MO_EXPR_TOKEN_INTEGER;
			break;

		case MO_EXPR_TOKEN_STRING:
			state.value.value_str.Reverse();
			break;

		default:;
		}
		break;

	case '~':	// bitwise not
		r = expr_prefix_inc(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			state.value.value_int = ~state.value.value_int;
			break;

		case MO_EXPR_TOKEN_FLOAT:
			return moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;

		case MO_EXPR_TOKEN_STRING:
			state.value.value_str = state.value.value_str.Switchcase();
			break;

		default:;
		}
		break;

	case '+':	// identity (C++ only)
		r = expr_prefix_inc(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		if(state.value.value_type == MO_EXPR_TOKEN_STRING) {
			state.value.value_str = state.value.value_str.Uppercase();
		}
		break;

	case '-':
		r = expr_prefix_inc(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			state.value.value_int = -state.value.value_int;
			break;

		case MO_EXPR_TOKEN_FLOAT:
			state.value.value_flt = -state.value.value_flt;
			break;

		case MO_EXPR_TOKEN_STRING:
			state.value.value_str = state.value.value_str.Lowercase();
			break;

		default:;
		}
		break;

	case '(':
		r = expr_token(state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		if(state.token == MO_EXPR_TOKEN_IDENTIFIER) {
			l = static_cast<long>(state.end - state.start);
			// check for casts
			if((l == 4 && mowc::strcmp(state.start, "long", l) == 0)
			|| (l == 3 && mowc::strcmp(state.start, "int",  l) == 0)) {
				r = expr_token(state);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				if(state.token != ')') {
					return moExpr::MO_EXPR_ERROR_EXPECTED_CLOSE;
				}
				r = expr_prefix_inc(state, true);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (long) state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					break;

				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = state.value.value_str.Integer();
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					break;

				default:;
				}
				//r = expr_token(state);
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			else if((l == 5 && mowc::strcmp(state.start, "float", l) == 0)
			     || (l == 6 && mowc::strcmp(state.start, "double", l) == 0)) {
				r = expr_token(state);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				if(state.token != ')') {
					return moExpr::MO_EXPR_ERROR_EXPECTED_CLOSE;
				}
				r = expr_prefix_inc(state, true);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_flt = (double) state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				case MO_EXPR_TOKEN_STRING:
					state.value.value_flt = state.value.value_str.Float();
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				default:;
				}
				//r = expr_token(state);
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			else if(l == 6 && mowc::strcmp(state.start, "string", l) == 0) {
				r = expr_token(state);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				if(state.token != ')') {
					return moExpr::MO_EXPR_ERROR_EXPECTED_CLOSE;
				}
				r = expr_prefix_inc(state, true);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					sprintf(buf, "%ld", state.value.value_int);	/* Flawfinder: ignore */
					state.value.value_str = buf;
					state.value.value_type = MO_EXPR_TOKEN_STRING;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					sprintf(buf, "%g", state.value.value_flt);	/* Flawfinder: ignore */
					state.value.value_str = buf;
					state.value.value_type = MO_EXPR_TOKEN_STRING;
					break;

				default:;
				}
				return moExpr::MO_EXPR_ERROR_NONE;
			}
			else {
				// an expression with a variable?
				r = expr_comma(state, false);
			}
		}
		else {
			// not a cast, some expression...
			r = expr_comma(state, false);
		}
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		if(state.token != ')') {
			// invalid expression!
			return moExpr::MO_EXPR_ERROR_EXPECTED_CLOSE;
		}
		return expr_token(state);

	case MO_EXPR_TOKEN_IDENTIFIER:
		state.value.value_type = MO_EXPR_TOKEN_IDENTIFIER;
		state.value.value_str = moWCString(state.start, static_cast<int>(state.end - state.start));
		r = expr_token(state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(static_cast<int>(state.token)) {
		case '(':
		case '=':
	   	case MO_EXPR_TOKEN_ASSIGN_POW:
	   	case MO_EXPR_TOKEN_ASSIGN_MUL:
	   	case MO_EXPR_TOKEN_ASSIGN_DIV:
	   	case MO_EXPR_TOKEN_ASSIGN_MOD:
	   	case MO_EXPR_TOKEN_ASSIGN_ADD:
	   	case MO_EXPR_TOKEN_ASSIGN_SUB:
	   	case MO_EXPR_TOKEN_ASSIGN_SHL:
	   	case MO_EXPR_TOKEN_ASSIGN_SHR:
	   	case MO_EXPR_TOKEN_ASSIGN_AND:
	   	case MO_EXPR_TOKEN_ASSIGN_OR:
	   	case MO_EXPR_TOKEN_ASSIGN_XOR:
			// a function or assignment; don't do anything about this identifier!
			return moExpr::MO_EXPR_ERROR_NONE;

		default:
			// in all other cases, that identifier is a variable name
			break;

		}
		idx = state.count;
		while(idx > 0) {
			idx--;
			if(state.value.value_str == state.variable[idx].name) {
				// the variable exists, replace with its content
				state.value.value_type = state.variable[idx].value_type;
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.variable[idx].value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = state.variable[idx].value_flt;
					break;

				case MO_EXPR_TOKEN_STRING:
					state.value.value_str = state.variable[idx].value_str;
					break;

				default:;
				}
				return moExpr::MO_EXPR_ERROR_NONE;
			}
		}
		fprintf(stderr, "ERROR: the variable named '%s' is not defined.\n", state.value.value_str.SavedMBData());
		return moExpr::MO_EXPR_ERROR_VARUNDEFINED;

	case MO_EXPR_TOKEN_INTEGER:
	{
		/*
		l = state.end - state.start;
		if((unsigned long) l >= sizeof(buf)) {
			l = sizeof(buf) - 1;
		}
		strncpy(buf, state.start, l);
		buf[l] = '\0';
		state.value.value_int = strtol(buf, 0, 0);
		*/
		moWCString i(state.start, static_cast<int>(state.end - state.start));
		state.value.value_int = i.Integer();
		state.value.value_type = MO_EXPR_TOKEN_INTEGER;
		return expr_token(state);
	}

	case MO_EXPR_TOKEN_FLOAT:
	{
		/*
		l = state.end - state.start;
		if((unsigned long) l >= sizeof(buf)) {
			l = sizeof(buf) - 1;
		}
		strncpy(buf, state.start, l);
		buf[l] = '\0';
		state.value.value_flt = strtod(buf, 0);
		*/
		moWCString f(state.start, static_cast<int>(state.end - state.start));
		state.value.value_flt = f.Float();
		state.value.value_type = MO_EXPR_TOKEN_FLOAT;
		return expr_token(state);
	}

	case MO_EXPR_TOKEN_STRING:
		// get the string without the quotes
		state.value.value_str = moWCString(state.start + 1, static_cast<int>(state.end - state.start - 2)).FromBackslash();
		state.value.value_type = MO_EXPR_TOKEN_STRING;

		// concatanate all the following strings
		r = expr_token(state);
		while(r == moExpr::MO_EXPR_ERROR_NONE
		   && state.token == MO_EXPR_TOKEN_STRING) {
			state.value.value_str += moWCString(state.start + 1, static_cast<int>(state.end - state.start - 2));
			r = expr_token(state);
		}
		return r;

	default:
		// anything else is an error!
		return moExpr::MO_EXPR_ERROR_SYNTAX;

	}

	return moExpr::MO_EXPR_ERROR_NONE;
}


#if 0
static void expr_substate_init(mo_expr_state_t& state, mo_expr_state_t& sub_state)
{
	sub_state.input = state.input;
	sub_state.value.value_type = MO_EXPR_TOKEN_UNKNOWN;
	sub_state.variable = state.variable;	// keep this pointer!
}


static void expr_substate_done(mo_expr_state_t& state, mo_expr_state_t& sub_state)
{
	state.input = sub_state.input;
	state.token = sub_state.token;
	state.start = sub_state.start;
	state.end = sub_state.end;
	state.value = sub_state.value;
}
#endif


struct expr_format_info {
	long		position;	// position of the parameter
	long		width;		// max. number of characters
	long		width_pos;	// get the width there
	long		precision;	// number of decimals
	long		precision_pos;	// get the precision there
	long		flags;		// a set of flags

			expr_format_info(void)
			{
				position = 1;
			}
};

// flags
static const long	EXPR_FMT_FORMAT		= 0x0001;	// show type format
static const long	EXPR_FMT_ZERO		= 0x0002;	// zero padded
static const long	EXPR_FMT_LEFT		= 0x0004;	// left adjusted
static const long	EXPR_FMT_SPACE		= 0x0008;	// space for + sign
static const long	EXPR_FMT_PLUS		= 0x0010;	// use + or -
static const long	EXPR_FMT_THOUSANDS	= 0x0020;	// show thousands separator
static const long	EXPR_FMT_UPPERCASE	= 0x0040;	// X, G, etc.

// lengths
static const long	EXPR_FMT_NOLENGTH	= 0x0000;
static const long	EXPR_FMT_LENGTH_MASK	= 0xF000;
static const long	EXPR_FMT_BYTE		= 0x1000;	// hh
static const long	EXPR_FMT_SHORT		= 0x2000;	// h
static const long	EXPR_FMT_LONG		= 0x3000;	// l
static const long	EXPR_FMT_LONGLONG	= 0x4000;	// ll (q)
static const long	EXPR_FMT_LONGDOUBLE	= 0x5000;	// L
static const long	EXPR_FMT_INTMAX		= 0x6000;	// j
static const long	EXPR_FMT_SIZE_T		= 0x7000;	// z (Z)
static const long	EXPR_FMT_PTRDIFF_T	= 0x8000;	// t

// types
static const long	EXPR_FMT_NOTYPE		= 0x00000;	// invalid format!
static const long	EXPR_FMT_TYPE_MASK	= 0xF0000;
static const long	EXPR_FMT_INTEGER	= 0x10000;	// i, d
static const long	EXPR_FMT_UNSIGNED	= 0x20000;	// u
static const long	EXPR_FMT_OCTAL		= 0x30000;	// o
static const long	EXPR_FMT_HEXA		= 0x40000;	// x, X (always uppercase)
static const long	EXPR_FMT_EDOUBLE	= 0x50000;	// e, E
static const long	EXPR_FMT_FDOUBLE	= 0x60000;	// f, F
static const long	EXPR_FMT_GDOUBLE	= 0x70000;	// g, G
static const long	EXPR_FMT_ADOUBLE	= 0x80000;	// a, A
static const long	EXPR_FMT_CHARACTER	= 0x90000;	// c (C)
static const long	EXPR_FMT_STRING		= 0xA0000;	// s (S)
static const long	EXPR_FMT_POINTER	= 0xB0000;	// p


// The following parse (in order) the possible parameters available
// in a string format for a printf()
// The order is as follow:
//	the position (<value>$)
//	a flag (#, ,-,...)
//	field width (possibly indirect: *[<value>$])
//	precision (possibly indirect: .*[<value>$])
//	length modifier (hh,h,l,ll,q,...)
//	format (i,d,f,e,g,a...)
// returns -1 when an error occurs and s is unchanged

static int expr_func_strf_format(const mowc::wc_t *& s, expr_format_info& info)
{
	mowc::wc_t		c, *end;
	const mowc::wc_t	*start;
	bool			more;
	long			value;

	start = s;

// reset the info structure
	// info.position unchanged! must be initialized to 1 by caller the very first time
	info.width         = -1;
	info.width_pos     = -1;
	info.precision     = -1;
	info.precision_pos = -1;
	info.flags         = 0;

// position
	if(*s >= '1' && *s <= '9') {
		// position of the parameter to use
		value = mowc::strtol(s, &end, 10);
		// must be followed by a '$' to be a positioning arg.
		// otherwise it's probably a width and is ignored here!
		if(*end == '$') {
			info.position = value;
			s = end + 1;
		}
	}

// flags
	more = true;
	do {
		c = *s++;
		switch(c) {
		case '#':
			info.flags |= EXPR_FMT_FORMAT;
			break;

		case '0':
			info.flags |= EXPR_FMT_ZERO;
			break;

		case '-':
			info.flags |= EXPR_FMT_LEFT;
			break;

		case ' ':
			info.flags |= EXPR_FMT_SPACE;
			break;

		case '+':
			info.flags |= EXPR_FMT_PLUS;
			break;

		case '\'':
			info.flags |= EXPR_FMT_THOUSANDS;
			break;

		default:
			more = false;
			break;

		}
	} while(more);

// width
	if(c == '*') {
		// indirect width
		c = *s;
		if(c >= '1' && c <= '9') {
			// specify a position!
			info.width_pos = mowc::strtol(s, &end, 10);
			if(end == 0 || *end != '$') {
				s = start;
				return -1;
			}
		}
		else {
			info.width_pos = -2;
		}
		s = end + 1;
		c = *s++;
	}
	else if(c >= '1' && c <= '9') {
		// direct width
		info.width = mowc::strtol(s - 1, &end, 10);
		s = end;
		c = *s++;
	}

// precision
	if(c == '.') {
		c = *s++;
		if(c == '*') {
			// indirect precision
			c = *s;
			if(c >= '1' && c <= '9') {
				info.precision_pos = mowc::strtol(s, &end, 10);
				if(end == 0 || *end != '$') {
					s = start;
					return -1;
				}
			}
			else {
				info.precision_pos = -2;
			}
			s = end + 1;
			c = *s++;
		}
		else if(c >= '1' && c <= '9') {
			// direct precision
			info.precision = mowc::strtol(s - 1, &end, 10);
			s = end;
			c = *s++;
		}
	}

// length (only one length can be accepted!)
	switch(c) {
	case 'h':
		if(*s == 'h') {
			info.flags |= EXPR_FMT_BYTE;
			s += 2;
			c = s[-1];
		}
		else {
			info.flags |= EXPR_FMT_SHORT;
			c = *s++;
		}
		break;

	case 'l':
		if(*s == 'l') {
			info.flags |= EXPR_FMT_LONGLONG;
			s += 2;
			c = s[-1];
		}
		else {
			info.flags |= EXPR_FMT_LONG;
			c = *s++;
		}
		break;

	case 'L':
		info.flags |= EXPR_FMT_LONGDOUBLE;
		c = *s++;
		break;

	case 'j':
		info.flags |= EXPR_FMT_INTMAX;
		c = *s++;
		break;

	case 'z':
	case 'Z':
		info.flags |= EXPR_FMT_SIZE_T;
		c = *s++;
		break;

	case 't':
		info.flags |= EXPR_FMT_PTRDIFF_T;
		c = *s++;
		break;

	}

// check the format now (only one format possible!)
	switch(c) {
	case 'i':
	case 'd':
		info.flags |= EXPR_FMT_INTEGER;
		break;

	case 'u':
		info.flags |= EXPR_FMT_UNSIGNED;
		break;

	case 'o':
		info.flags |= EXPR_FMT_OCTAL;
		break;

	case 'x':
		info.flags |= EXPR_FMT_HEXA;
		break;

	case 'X':
		info.flags |= EXPR_FMT_HEXA | EXPR_FMT_UPPERCASE;
		break;

	case 'e':
		info.flags |= EXPR_FMT_EDOUBLE;
		break;

	case 'E':
		info.flags |= EXPR_FMT_EDOUBLE | EXPR_FMT_UPPERCASE;
		break;

	case 'f':
	case 'F':		// that's useless, 'F' is like 'f'
		info.flags |= EXPR_FMT_FDOUBLE;
		break;

	case 'g':
		info.flags |= EXPR_FMT_GDOUBLE;
		break;

	case 'G':
		info.flags |= EXPR_FMT_GDOUBLE | EXPR_FMT_UPPERCASE;
		break;

	case 'a':
		info.flags |= EXPR_FMT_ADOUBLE;
		break;

	case 'A':
		info.flags |= EXPR_FMT_ADOUBLE | EXPR_FMT_UPPERCASE;
		break;

	case 'c':
	case 'C':
		info.flags = (info.flags & ~EXPR_FMT_LENGTH_MASK)
					| EXPR_FMT_LONGLONG | EXPR_FMT_CHARACTER;
		break;

	case 's':
	case 'S':
		info.flags = (info.flags & ~EXPR_FMT_LENGTH_MASK)
					| EXPR_FMT_LONGLONG | EXPR_FMT_STRING;
		break;

	case 'p':
		info.flags |= EXPR_FMT_POINTER;
		break;

	default:
		s = start;
		return -1;

	}

	return 0;
}


static moWCString expr_func_strf_apply(mo_expr_state_t& params, expr_format_info& info)
{
	char		fmt[256];	/* Flawfinder: ignore */
	int		r;

	r = 1;
	fmt[0] = '%';
	fmt[1] = '\0';

// insert flags
	if((info.flags & EXPR_FMT_FORMAT) != 0) {
		fmt[r++] = '#';
	}
	if((info.flags & EXPR_FMT_ZERO) != 0) {
		fmt[r++] = '0';
	}
	if((info.flags & EXPR_FMT_LEFT) != 0) {
		fmt[r++] = '-';
	}
	if((info.flags & EXPR_FMT_SPACE) != 0) {
		fmt[r++] = ' ';
	}
	else if((info.flags & EXPR_FMT_PLUS) != 0) {
		fmt[r++] = '+';
	}
	if((info.flags & EXPR_FMT_THOUSANDS) != 0) {
		fmt[r++] = '\'';
	}

// check for a width
	if(info.width_pos > 0) {
		// get the width from the parameters at the given position
		if(info.width_pos >= params.param_count
		|| info.width_pos >= MO_EXPR_PARAM_MAX) {
			// position out of range
			return "";
		}
		info.position = info.width_pos;
		if(params.param[info.position].value_type != MO_EXPR_TOKEN_INTEGER) {
			// position need to be an integer
			return "";
		}
		info.width = params.param[info.position].value_int;
		info.position++;
	}
	else if(info.width_pos == -2) {
		if(info.position >= params.param_count
		|| info.position >= MO_EXPR_PARAM_MAX) {
			// position out of range
			return "";
		}
		info.width = params.param[info.position].value_int;
		info.position++;
	}
	if(info.width > 0) {
		r += sprintf(fmt + r, "%ld", info.width);	/* Flawfinder: ignore */
	}

// check for a precision
	if(info.precision_pos > 0) {
		// get the width from the parameters at the given position
		if(info.precision_pos >= params.param_count
		|| info.precision_pos >= MO_EXPR_PARAM_MAX) {
			// position out of range
			return "";
		}
		info.position = info.precision_pos;
		if(params.param[info.position].value_type != MO_EXPR_TOKEN_INTEGER) {
			// position need to be an integer
			return "";
		}
		info.precision = params.param[info.position].value_int;
		info.position++;
	}
	else if(info.precision_pos == -2) {
		if(info.position >= params.param_count
		|| info.position >= MO_EXPR_PARAM_MAX) {
			// position out of range
			return "";
		}
		info.precision = params.param[info.position].value_int;
		info.position++;
	}
	if(info.precision >= 0) {
		r += sprintf(fmt + r, ".%ld", info.precision);	/* Flawfinder: ignore */
	}

// insert the length
#if 0
	// we really only offer the default types (long, wc_t, wc_t *, ...)
	switch(info.flags & EXPR_FMT_LENGTH_MASK) {
	case EXPR_FMT_BYTE:
		fmt[r++] = 'h';
		fmt[r++] = 'h';
		break;

	case EXPR_FMT_SHORT:
		fmt[r++] = 'h';
		break;

	case EXPR_FMT_LONG:
		fmt[r++] = 'l';
		break;

	case EXPR_FMT_LONGLONG:
		fmt[r++] = 'l';
		fmt[r++] = 'l';
		break;

	case EXPR_FMT_LONGDOUBLE:
		fmt[r++] = 'L';
		break;

	case EXPR_FMT_INTMAX:
		fmt[r++] = 'j';
		break;

	case EXPR_FMT_SIZE_T:
		fmt[r++] = 'z';
		break;

	case EXPR_FMT_PTRDIFF_T:
		fmt[r++] = 't';
		break;

	// case EXPR_FMT_UNKNOWN - undefined, keep the default
	}
#endif

// insert the type
	switch(info.flags & EXPR_FMT_TYPE_MASK) {
	case EXPR_FMT_INTEGER:
		fmt[r++] = 'l';
		fmt[r++] = 'd';
		break;

	case EXPR_FMT_UNSIGNED:
		fmt[r++] = 'l';
		fmt[r++] = 'u';
		break;

	case EXPR_FMT_OCTAL:
		fmt[r++] = 'l';
		fmt[r++] = 'o';
		break;

	case EXPR_FMT_HEXA:
		fmt[r++] = 'l';
		fmt[r++] = (info.flags & EXPR_FMT_UPPERCASE) ? 'X' : 'x';
		break;

	case EXPR_FMT_EDOUBLE:
		fmt[r++] = (info.flags & EXPR_FMT_UPPERCASE) ? 'E' : 'e';
		break;

	case EXPR_FMT_FDOUBLE:
		fmt[r++] = 'f';
		break;

	case EXPR_FMT_GDOUBLE:
		fmt[r++] = (info.flags & EXPR_FMT_UPPERCASE) ? 'G' : 'g';
		break;

	case EXPR_FMT_ADOUBLE:
		fmt[r++] = (info.flags & EXPR_FMT_UPPERCASE) ? 'A' : 'a';
		break;

	case EXPR_FMT_CHARACTER:
		fmt[r++] = 'l';
		fmt[r++] = 'l';
		fmt[r++] = 'c';
		break;

	case EXPR_FMT_STRING:
		fmt[r++] = 'l';
		fmt[r++] = 'l';
		fmt[r++] = 's';
		break;

	case EXPR_FMT_POINTER:
		// there is no notion of pointer in this expression!
		return "<no ptr>";

	}

	if(info.position >= params.param_count
	|| info.position >= MO_EXPR_PARAM_MAX) {
		// that parameter doesn't exist!
		return "";
	}
	fmt[r] = '\0';

// get the parameter
	switch(info.flags & EXPR_FMT_TYPE_MASK) {
	case EXPR_FMT_INTEGER:
	case EXPR_FMT_UNSIGNED:
	case EXPR_FMT_OCTAL:
	case EXPR_FMT_HEXA:
	case EXPR_FMT_CHARACTER:
		if(params.param[info.position].value_type != MO_EXPR_TOKEN_INTEGER) {
			// format/type mismatch
			return "";
		}
		return moWCString::Format(fmt, mowc::MO_ENCODING_UTF8, params.param[info.position++].value_int);

	case EXPR_FMT_EDOUBLE:
	case EXPR_FMT_FDOUBLE:
	case EXPR_FMT_GDOUBLE:
	case EXPR_FMT_ADOUBLE:
		if(params.param[info.position].value_type != MO_EXPR_TOKEN_FLOAT) {
			// format/type mismatch
			return "";
		}
		return moWCString::Format(fmt, mowc::MO_ENCODING_UTF8, params.param[info.position++].value_flt);

	case EXPR_FMT_STRING:
		if(params.param[info.position].value_type != MO_EXPR_TOKEN_STRING) {
			// format/type mismatch
			return "";
		}
		return moWCString::Format(fmt, mowc::MO_ENCODING_UTF8, params.param[info.position++].value_str.Data());

	}

	return "";
}


static moExpr::mo_expr_errno_t expr_func_strf(mo_expr_state_t& state, mo_expr_state_t& params)
{
	// here we have to parse the format because the parameters
	// are not in standard args...
	// Note that the positioning has to be handled by us also
	// (see the *.*$ and <value>$ notation of printf().

	const mowc::wc_t	*s;
	mowc::wc_t		c;
	int			r;
	moWCString		result;
	expr_format_info	info;

	// get the format
	s = params.param[0].value_str.Data();
	while(*s != '\0') {
		c = *s++;
		switch(c) {
		case '%':
			if(*s != '%') {
				r = expr_func_strf_format(s, info);

#if 0
fprintf(stderr, "Checked format [%s] -> %d\n", s, r);
fprintf(stderr, "  info.position      = %ld\n", info.position);
fprintf(stderr, "  info.width         = %ld\n", info.width);
fprintf(stderr, "  info.width_pos     = %ld\n", info.width_pos);
fprintf(stderr, "  info.precision     = %ld\n", info.precision);
fprintf(stderr, "  info.precision_pos = %ld\n", info.precision_pos);
fprintf(stderr, "  info.flags         = 0x%08lX\n", info.flags);
#endif

				if(r == 0) {	// apply only valid formats
					result += expr_func_strf_apply(params, info);
				}
				break;
			}
			/*FALLTHROUGH*/
		default:
			result += c;
			break;

		case '\\':
			c = static_cast<mowc::wc_t>(mowc::backslash_char(s));
			result += c;
			break;

		}
	}

	state.value.value_type = MO_EXPR_TOKEN_STRING;
	state.value.value_str = result;

	return moExpr::MO_EXPR_ERROR_NONE;
}


static moExpr::mo_expr_errno_t expr_func_regexpr(mo_expr_state_t& state, mo_expr_state_t& params)
{
	moRegularExpression	regexpr(params.param[0].value_str);
	moVariableList		vars("regexpr()");

	state.value.value_type = MO_EXPR_TOKEN_INTEGER;
	state.value.value_int = regexpr.MatchExpression(params.param[1].value_str, &vars);

#if 0
fprintf(stderr, "\n***\n*** Does [%s] match [%s]? (%d)\n***\n",
			params.param[0].value_str.MBData(),
			params.param[1].value_str.MBData(),
			state.value.value_int);
#endif

	if(!state.value.value_int && regexpr.LastError() != moRegularExpression::MO_REGEXPR_ERROR_NONE) {
		return moExpr::MO_EXPR_ERROR_BADREGEXPR;
	}

	// TODO: set the vars in the following parameters (variables)

	return moExpr::MO_EXPR_ERROR_NONE;
}



static moExpr::mo_expr_errno_t expr_func_reg_replace(mo_expr_state_t& state, mo_expr_state_t& params)
{
	moRegularExpression	reg_replace(params.param[0].value_str);

	state.value.value_type = MO_EXPR_TOKEN_STRING;
	state.value.value_str = reg_replace.Replace(params.param[1].value_str, params.param[2].value_str);

	if(state.value.value_str.IsEmpty() && reg_replace.LastError() != moRegularExpression::MO_REGEXPR_ERROR_NONE) {
		return moExpr::MO_EXPR_ERROR_BADREGEXPR;
	}

	return moExpr::MO_EXPR_ERROR_NONE;
}



static moExpr::mo_expr_errno_t expr_func_replace(mo_expr_state_t& state, mo_expr_state_t& params)
{
	state.value.value_type = MO_EXPR_TOKEN_STRING;
	state.value.value_str = params.param[1].value_str.Replace(params.param[0].value_str);

#if 0
fprintf(stderr, "\n***\n*** Replacing [%s] using [%s] is [%s]\n***\n",
			params.param[1].value_str.SavedMBData(),
			params.param[0].value_str.SavedMBData(),
			state.value.value_str.SavedMBData());
#endif

	return moExpr::MO_EXPR_ERROR_NONE;
}



static moExpr::mo_expr_errno_t expr_func_included(mo_expr_state_t& state, mo_expr_state_t& params)
{
	long		idx;

/* by default we find the 1st value in the following list */
	state.value.value_type = MO_EXPR_TOKEN_INTEGER;
	state.value.value_int = true;

	idx = params.param_count;
	while(idx > 1) {
		idx--;
		switch(params.param[0].value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(params.param[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				if(params.param[0].value_int == params.param[idx].value_int) {
					return moExpr::MO_EXPR_ERROR_NONE;
				}
				break;

			default:
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;

			}
			break;

		default:
			return moExpr::MO_EXPR_ERROR_INVALID_PARAM;

		}
	}

/* unfortunatly we didn't find it... */
	state.value.value_int = false;

	return moExpr::MO_EXPR_ERROR_NONE;
}


static moExpr::mo_expr_errno_t expr_array_func(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;
	long			from, to;

	r = expr_unary(state, get_token);
	if(r != moExpr::MO_EXPR_ERROR_NONE) {
		return r;
	}

	if(state.token == '[') {
		// array!
		if(state.value.value_type != MO_EXPR_TOKEN_STRING) {
			return moExpr::MO_EXPR_ERROR_ARRAY;
		}
		mo_expr_state_t sub_state(state);
		r = expr_token(sub_state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		if(sub_state.token == MO_EXPR_TOKEN_RANGE) {
			// from the beginning
			from = 0;
		}
		else {
			r = expr_comma(sub_state, false);
			if(r != moExpr::MO_EXPR_ERROR_NONE) {
				return r;
			}
			if(sub_state.value.value_type != MO_EXPR_TOKEN_INTEGER) {
				// we can only index with an integer at this time
				return moExpr::MO_EXPR_ERROR_INDEX;
			}
			from = sub_state.value.value_int;
		}
		if(sub_state.token == MO_EXPR_TOKEN_RANGE) {
			r = expr_token(sub_state);	// get second value
			if(r != moExpr::MO_EXPR_ERROR_NONE) {
				return r;
			}
			if(sub_state.token == ']') {
				// up to the end
				to = static_cast<long>(state.value.value_str.Length() - 1);
			}
			else {
				r = expr_comma(sub_state, false);
				if(r != moExpr::MO_EXPR_ERROR_NONE) {
					return r;
				}
				if(sub_state.value.value_type != MO_EXPR_TOKEN_INTEGER) {
					// we can only index with an integer at this time
					return moExpr::MO_EXPR_ERROR_INDEX;
				}
				to = sub_state.value.value_int;
			}
		}
		else {
			to = from;
		}

		if(sub_state.token != ']') {
			return moExpr::MO_EXPR_ERROR_EXPECTED_CLOSE;
		}
		// note that from can be large than to in which case
		// characters will be inverted automatically
		if(from < 0) {
			from += static_cast<long>(state.value.value_str.Length());
		}
		if(to < 0) {
			to += static_cast<long>(state.value.value_str.Length() - 1);
		}
		state.value.value_str = state.value.value_str.Get(from, to);
		// go to the next token now
		r = expr_token(sub_state);
	}
	else if(state.token == '(') {
		// a function call
		if(state.value.value_type != MO_EXPR_TOKEN_IDENTIFIER
		&& state.value.value_type != MO_EXPR_TOKEN_STRING) {
			return moExpr::MO_EXPR_ERROR_FUNCTION;
		}
		// get parameters
		mo_expr_state_t sub_state(state);
		r = expr_comma(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		if(sub_state.token != ')') {
			return moExpr::MO_EXPR_ERROR_EXPECTED_CLOSE;
		}
		if(state.value.value_str == "strf") {
			if(sub_state.param_count < 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			r = expr_func_strf(state, sub_state);
		}
		else if(state.value.value_str == "strlen") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_int = static_cast<long>(sub_state.param[0].value_str.Length());
			state.value.value_type = MO_EXPR_TOKEN_INTEGER;
		}
		else if(state.value.value_str == "basename") {
			if(sub_state.param_count < 1
			|| sub_state.param_count > 2) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING
			|| (sub_state.param_count == 2
					&& sub_state.param[1].value_type != MO_EXPR_TOKEN_STRING)) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			if(sub_state.param_count == 2) {
				state.value.value_str = sub_state.param[0].value_str.FilenameBasename(sub_state.param[1].value_str);
			}
			else {
				state.value.value_str = sub_state.param[0].value_str.FilenameBasename("");
			}
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "clip") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Clip();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "trim") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Clip(moWCString::WC_STRING_CLIP_BOTH | moWCString::WC_STRING_CLIP_NEWLINE);
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "upper") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Uppercase();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "lower") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Lowercase();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "switch_case") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Switchcase();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "capitalize_words") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.CapitalizeWords();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "capitalize") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Capitalize();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "reverse") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_str = sub_state.param[0].value_str.Reverse();
			state.value.value_type = MO_EXPR_TOKEN_STRING;
		}
		else if(state.value.value_str == "regexpr") {
			if(sub_state.param_count < 2) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING
			|| sub_state.param[1].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			r = expr_func_regexpr(state, sub_state);
		}
		else if(state.value.value_str == "reg_replace") {
			if(sub_state.param_count < 3) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING
			|| sub_state.param[1].value_type != MO_EXPR_TOKEN_STRING
			|| sub_state.param[2].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			r = expr_func_reg_replace(state, sub_state);
		}
		else if(state.value.value_str == "replace") {
			if(sub_state.param_count < 2) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING
			|| sub_state.param[1].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			r = expr_func_replace(state, sub_state);
		}
		else if(state.value.value_str == "atol") {
			if(sub_state.param_count != 1) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			if(sub_state.param[0].value_type != MO_EXPR_TOKEN_STRING) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM;
			}
			state.value.value_int = sub_state.param[0].value_str.Integer();
			state.value.value_type = MO_EXPR_TOKEN_INTEGER;
		}
		else if(state.value.value_str == "included") {
			if(sub_state.param_count < 2) {
				return moExpr::MO_EXPR_ERROR_INVALID_PARAM_COUNT;
			}
			r = expr_func_included(state, sub_state);
		}
		else {
			return moExpr::MO_EXPR_ERROR_UNKNOWN_FUNCTION;
		}
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		// go to the next token now
		r = expr_token(sub_state);
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_postfix_inc(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_array_func(state, get_token);
	if(r != moExpr::MO_EXPR_ERROR_NONE) {
		return r;
	}

	while(state.token == MO_EXPR_TOKEN_INCREMENT
	   || state.token == MO_EXPR_TOKEN_DECREMENT) {
		// get the right side expression
		switch(state.token) {
		case MO_EXPR_TOKEN_INCREMENT:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int++;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_flt += 1.0;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_DECREMENT:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int--;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_flt -= 1.0;
				break;

			default:;
			}
			break;

		default:;/*NOTREACHED*/
		}
		r = expr_token(state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
	}

	return moExpr::MO_EXPR_ERROR_NONE;
}


static moExpr::mo_expr_errno_t expr_prefix_inc(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;
	long			value;

	if(get_token) {
		r = expr_token(state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
	}

	// the following while represents the L-R of this operator
	// though it's not 100% safe if you want to check overflows
	// on the fly.
	value = 0;
	while(state.token == MO_EXPR_TOKEN_INCREMENT
	   || state.token == MO_EXPR_TOKEN_DECREMENT) {
		// get the right side expression
		switch(state.token) {
		case MO_EXPR_TOKEN_INCREMENT:
			value++;
			break;

		case MO_EXPR_TOKEN_DECREMENT:
			value--;
			break;

		default:;/*NOTREACHED*/
		}
		r = expr_token(state);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
	}

	r = expr_postfix_inc(state, false);
	if(r == moExpr::MO_EXPR_ERROR_NONE && value != 0) {
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			state.value.value_int += value;
			break;

		case MO_EXPR_TOKEN_FLOAT:
			state.value.value_flt += (double) value;
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_power(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;
	long			v;

	r = expr_prefix_inc(state, get_token);
	if(r != moExpr::MO_EXPR_ERROR_NONE) {
		return r;
	}

	if(state.token == MO_EXPR_TOKEN_POWER) {
		// compute the power (it's right justified)
		mo_expr_state_t sub_state(state);
		r = expr_power(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				// no overflow check...
				if(sub_state.value.value_int < 0) {
					state.value.value_int = 0;
				}
				else {
					v = state.value.value_int;
					state.value.value_int = 1;
					while(sub_state.value.value_int > 0) {
						sub_state.value.value_int--;
						state.value.value_int *= v;
					}
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_flt = pow((double) state.value.value_int, state.value.value_flt);
				state.value.value_type = MO_EXPR_TOKEN_FLOAT;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_FLOAT:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_flt = pow(state.value.value_flt, (double) state.value.value_int);
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_flt = pow(state.value.value_flt, state.value.value_flt);
				break;

			default:;
			}
			break;

		default:;
		}
	}

	return r;
}




static moExpr::mo_expr_errno_t expr_multiplicative(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_power(state, get_token);
	if(r != moExpr::MO_EXPR_ERROR_NONE) {
		return r;
	}

	while(state.token == '*' || state.token == '/' || state.token == '%') {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_power(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(static_cast<int>(state.token)) {
		case '*':
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int *= sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = (double) state.value.value_int * sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_flt = state.value.value_flt * (double) sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = state.value.value_flt * sub_state.value.value_flt;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case '/':
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				if(sub_state.value.value_int == 0) {
					return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				if(sub_state.value.value_flt == 0.0) {
					return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
				}
				break;

			default:;
			}
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int /= sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = (double) state.value.value_int / sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_flt = state.value.value_flt / (double) sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = state.value.value_flt / sub_state.value.value_flt;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case '%':
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				if(sub_state.value.value_int == 0) {
					return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				if(sub_state.value.value_flt == 0.0) {
					return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
				}
				break;

			default:;
			}
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int %= sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = fmod((double) state.value.value_int, sub_state.value.value_flt);
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_flt = fmod(state.value.value_flt, (double) sub_state.value.value_int);
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = fmod(state.value.value_flt, sub_state.value.value_flt);
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return moExpr::MO_EXPR_ERROR_NONE;
}



static moExpr::mo_expr_errno_t expr_additive(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_multiplicative(state, get_token);
	if(r != moExpr::MO_EXPR_ERROR_NONE) {
		return r;
	}

	while(state.token == '+' || state.token == '-') {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_multiplicative(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		switch(static_cast<int>(state.token)) {
		case '+':
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int += sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = (double) state.value.value_int + sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_flt = state.value.value_flt + (double) sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = state.value.value_flt + sub_state.value.value_flt;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_str += sub_state.value.value_str;
					break;

				default:;
				}

			default:;
			}
			break;

		case '-':
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int -= sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = (double) state.value.value_int - sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_FLOAT;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_flt = state.value.value_flt - (double) sub_state.value.value_int;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_flt = state.value.value_flt - sub_state.value.value_flt;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return moExpr::MO_EXPR_ERROR_NONE;
}


static moExpr::mo_expr_errno_t expr_shift(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_additive(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE
		&& (state.token == MO_EXPR_TOKEN_SHIFT_LEFT
			|| state.token == MO_EXPR_TOKEN_SHIFT_RIGHT)) {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_additive(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.token) {
		case MO_EXPR_TOKEN_SHIFT_LEFT:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int <<= sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		case MO_EXPR_TOKEN_SHIFT_RIGHT:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int -= sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_relational(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_shift(state, get_token);

	while(r == moExpr::MO_EXPR_ERROR_NONE
	  && (state.token == '<' || state.token == '>'
	   || state.token == MO_EXPR_TOKEN_LESS_EQUAL
	   || state.token == MO_EXPR_TOKEN_GREATER_EQUAL)) {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_shift(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(static_cast<int>(state.token)) {
		case '<':
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_int < sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (double) state.value.value_int < sub_state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_flt < (double) sub_state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = state.value.value_flt < sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = mowc::strcmp(state.value.value_str.Data(), sub_state.value.value_str.Data()) < 0;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		case '>':
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_int > sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (double) state.value.value_int > sub_state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_flt > (double) sub_state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = state.value.value_flt > sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = mowc::strcmp(state.value.value_str.Data(), sub_state.value.value_str.Data()) > 0;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		case MO_EXPR_TOKEN_LESS_EQUAL:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_int <= sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (double) state.value.value_int <= sub_state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_flt <= (double) sub_state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = state.value.value_flt <= sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = mowc::strcmp(state.value.value_str.Data(), sub_state.value.value_str.Data()) <= 0;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		case MO_EXPR_TOKEN_GREATER_EQUAL:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_int >= sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (double) state.value.value_int >= sub_state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_flt >= (double) sub_state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = state.value.value_flt >= sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = mowc::strcmp(state.value.value_str.Data(), sub_state.value.value_str.Data()) >= 0;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_compare(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_relational(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE
	  && (state.token == MO_EXPR_TOKEN_EQUAL
	   || state.token == MO_EXPR_TOKEN_NOT_EQUAL)) {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_relational(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.token) {
		case MO_EXPR_TOKEN_EQUAL:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_int == sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (double) state.value.value_int == sub_state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_flt == (double) sub_state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = state.value.value_flt == sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = mowc::strcmp(state.value.value_str.Data(), sub_state.value.value_str.Data()) == 0;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		case MO_EXPR_TOKEN_NOT_EQUAL:
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_int != sub_state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = (double) state.value.value_int != sub_state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.value.value_int = state.value.value_flt != (double) sub_state.value.value_int;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.value.value_int = state.value.value_flt != sub_state.value.value_flt;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			case MO_EXPR_TOKEN_STRING:
				switch(sub_state.value.value_type) {
				case MO_EXPR_TOKEN_STRING:
					state.value.value_int = mowc::strcmp(state.value.value_str.Data(), sub_state.value.value_str.Data()) != 0;
					state.value.value_type = MO_EXPR_TOKEN_INTEGER;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;/*NOTREACHED*/
				}
				break;

			default:;/*NOTREACHED*/
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_bitwise_and(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_compare(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE && state.token == '&') {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_compare(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int &= sub_state.value.value_int;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;/*NOTREACHED*/
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_bitwise_xor(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_bitwise_and(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE && state.token == '^') {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_bitwise_and(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int ^= sub_state.value.value_int;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_bitwise_or(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_bitwise_xor(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE && state.token == '|') {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_bitwise_xor(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int |= sub_state.value.value_int;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		default:;/*NOTREACHED*/
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_logical_and(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_bitwise_or(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE
	   && state.token == MO_EXPR_TOKEN_LOGICAL_AND) {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_bitwise_or(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = state.value.value_int != 0 && sub_state.value.value_int != 0;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = state.value.value_int != 0 && sub_state.value.value_flt != 0;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = state.value.value_int != 0 && !sub_state.value.value_str.IsEmpty();
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_FLOAT:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = state.value.value_flt != 0 && sub_state.value.value_int != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = state.value.value_flt != 0 && sub_state.value.value_flt != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = state.value.value_flt != 0 && !sub_state.value.value_str.IsEmpty();
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_STRING:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = !state.value.value_str.IsEmpty() && sub_state.value.value_int != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = !state.value.value_str.IsEmpty() && sub_state.value.value_flt != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = !state.value.value_str.IsEmpty() && !sub_state.value.value_str.IsEmpty();
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		default:;
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_logical_xor(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_logical_and(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE
	   && state.token == MO_EXPR_TOKEN_LOGICAL_XOR) {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_logical_and(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = (state.value.value_int != 0) ^ (sub_state.value.value_int != 0);
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = (state.value.value_int != 0) ^ (sub_state.value.value_flt != 0);
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = (state.value.value_int != 0) ^ !sub_state.value.value_str.IsEmpty();
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_FLOAT:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = (state.value.value_flt != 0) ^ (sub_state.value.value_int != 0);
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = (state.value.value_flt != 0) ^ (sub_state.value.value_flt != 0);
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = (state.value.value_flt != 0) ^ !sub_state.value.value_str.IsEmpty();
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_STRING:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = !state.value.value_str.IsEmpty() ^ (sub_state.value.value_int != 0);
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = !state.value.value_str.IsEmpty() ^ (sub_state.value.value_flt != 0);
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = !state.value.value_str.IsEmpty() ^ !sub_state.value.value_str.IsEmpty();
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		default:;
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_logical_or(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_logical_xor(state, get_token);
	while(r == moExpr::MO_EXPR_ERROR_NONE
	   && state.token == MO_EXPR_TOKEN_LOGICAL_OR) {
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_logical_xor(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = state.value.value_int != 0 || sub_state.value.value_int != 0;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = state.value.value_int != 0 || sub_state.value.value_flt != 0;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = state.value.value_int != 0 || !sub_state.value.value_str.IsEmpty();
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_FLOAT:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = state.value.value_flt != 0 || sub_state.value.value_int != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = state.value.value_flt != 0 || sub_state.value.value_flt != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = state.value.value_flt != 0 || !sub_state.value.value_str.IsEmpty();
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_STRING:
			switch(sub_state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = !state.value.value_str.IsEmpty() || sub_state.value.value_int != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_int = !state.value.value_str.IsEmpty() || sub_state.value.value_flt != 0;
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_int = !state.value.value_str.IsEmpty() || !sub_state.value.value_str.IsEmpty();
				state.value.value_type = MO_EXPR_TOKEN_INTEGER;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		default:;
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_conditional(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;
	long			choice;

	r = expr_logical_or(state, get_token);
	// this is a R-L expression
	while(r == moExpr::MO_EXPR_ERROR_NONE
	   && state.token == '?') {
		// make the selection now!
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			choice = state.value.value_int != 0;
			break;

		case MO_EXPR_TOKEN_FLOAT:
			choice = state.value.value_flt != 0;
			break;

		case MO_EXPR_TOKEN_STRING:
			choice = !state.value.value_str.IsEmpty();
			break;

		default:
			/*NOTREACHED*/
			choice = false;
			break;

		}
		// TODO: if 'choice' is false we need to skip
		// the following expression up to the ':'; also
		// if 'choice' is true, we need to skip the
		// expression after the ':'

		// get the left side expression
		r = expr_comma(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			break;
		}
		if(state.token != ':') {
			return moExpr::MO_EXPR_ERROR_NONE;
		}
		// get the right side expression
		mo_expr_state_t sub_state(state);
		r = expr_assign(sub_state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			break;
		}
		if(state.value.value_type != sub_state.value.value_type) {
			return moExpr::MO_EXPR_ERROR_BAD_TYPE;
		}
		if(!choice) {
			// keep the right result!
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.value.value_int = sub_state.value.value_int;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.value.value_flt = sub_state.value.value_flt;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.value.value_str = sub_state.value.value_str;
				break;

			default:;
			}
		}
	}

	return r;
}


static moExpr::mo_expr_errno_t expr_assign(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;
	mo_expr_token_t		token;
	long			v, idx;
	bool			found;

	r = expr_conditional(state, get_token);
	token = state.token;

	// these are a R-L operators
	while(r == moExpr::MO_EXPR_ERROR_NONE
	  && (token == '='
	   || token == MO_EXPR_TOKEN_ASSIGN_POW
	   || token == MO_EXPR_TOKEN_ASSIGN_MUL
	   || token == MO_EXPR_TOKEN_ASSIGN_DIV
	   || token == MO_EXPR_TOKEN_ASSIGN_MOD
	   || token == MO_EXPR_TOKEN_ASSIGN_ADD
	   || token == MO_EXPR_TOKEN_ASSIGN_SUB
	   || token == MO_EXPR_TOKEN_ASSIGN_SHL
	   || token == MO_EXPR_TOKEN_ASSIGN_SHR
	   || token == MO_EXPR_TOKEN_ASSIGN_AND
	   || token == MO_EXPR_TOKEN_ASSIGN_OR
	   || token == MO_EXPR_TOKEN_ASSIGN_XOR)) {
		// only identifiers can be used for variable names
		// we may add the string later if necessary
		if(state.value.value_type != MO_EXPR_TOKEN_IDENTIFIER) {
			return moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		}
		// search for this variable
		found = false;
		idx = state.count;
		while(idx > 0) {
			idx--;
			if(state.value.value_str == state.variable[idx].name) {
				// the variable already exists!
				found = true;
				break;
			}
		}
		if(!found) {
			if(token != '=') {
				// we need the variable to be defined before to do +=, *=, etc.
				return moExpr::MO_EXPR_ERROR_VARUNDEFINED;
			}
			if(state.count >= MO_EXPR_VARIABLE_MAX) {
				return moExpr::MO_EXPR_ERROR_VAROVERFLOW;
			}
			idx = state.count++;
			state.variable[idx].name = state.value.value_str;
		}
		// get the right side expression
		r = expr_assign(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			break;
		}
		r = moExpr::MO_EXPR_ERROR_OPERATOR_TYPE;
		switch(static_cast<int>(token)) {
		case '=':
			state.variable[idx].value_type = state.value.value_type;
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.variable[idx].value_int = state.value.value_int;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.variable[idx].value_flt = state.value.value_flt;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.variable[idx].value_str = state.value.value_str;
				r = moExpr::MO_EXPR_ERROR_NONE;
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_POW:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					if(state.value.value_int < 0) {
						state.variable[idx].value_int = 0;
					}
					else {
						v = state.variable[idx].value_int;
						state.variable[idx].value_int = 1;
						while(state.value.value_int > 0) {
							state.value.value_int--;
							state.variable[idx].value_int *= v;
						}
					}
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt = pow((double) state.variable[idx].value_int, state.value.value_flt);
					state.variable[idx].value_type = MO_EXPR_TOKEN_FLOAT;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_flt = pow(state.variable[idx].value_flt, (double) state.value.value_int);
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt = pow(state.variable[idx].value_flt, state.value.value_flt);
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_MUL:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int *= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt = (double) state.variable[idx].value_int * state.value.value_flt;
					state.variable[idx].value_type = MO_EXPR_TOKEN_FLOAT;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_flt *= (double) state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt *= state.variable[idx].value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_DIV:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int /= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt = (double) state.variable[idx].value_int / state.value.value_flt;
					state.variable[idx].value_type = MO_EXPR_TOKEN_FLOAT;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_flt /= (double) state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt /= state.variable[idx].value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_MOD:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					if(state.value.value_int == 0) {
						return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
					}
					state.variable[idx].value_int %= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					if(state.value.value_flt == 0) {
						return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
					}
					state.variable[idx].value_flt = fmod((double) state.variable[idx].value_int, state.value.value_flt);
					state.variable[idx].value_type = MO_EXPR_TOKEN_FLOAT;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					if(state.value.value_int == 0) {
						return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
					}
					state.variable[idx].value_flt = fmod(state.variable[idx].value_flt, (double) state.value.value_int);
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					if(state.value.value_flt == 0) {
						return moExpr::MO_EXPR_ERROR_DIVIDE_BY_ZERO;
					}
					state.variable[idx].value_flt = fmod(state.variable[idx].value_flt, state.value.value_flt);
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_ADD:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int += state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt = (double) state.variable[idx].value_int + state.value.value_flt;
					state.variable[idx].value_type = MO_EXPR_TOKEN_FLOAT;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_flt += (double) state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt += state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_SUB:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int -= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt = (double) state.variable[idx].value_int - state.value.value_flt;
					state.variable[idx].value_type = MO_EXPR_TOKEN_FLOAT;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			case MO_EXPR_TOKEN_FLOAT:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_flt -= (double) state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				case MO_EXPR_TOKEN_FLOAT:
					state.variable[idx].value_flt -= state.value.value_flt;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_SHL:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int <<= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_SHR:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int >>= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_AND:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int &= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_OR:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int |= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		case MO_EXPR_TOKEN_ASSIGN_XOR:
			switch(state.variable[idx].value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				switch(state.value.value_type) {
				case MO_EXPR_TOKEN_INTEGER:
					state.variable[idx].value_int ^= state.value.value_int;
					r = moExpr::MO_EXPR_ERROR_NONE;
					break;

				default:;
				}
				break;

			default:;
			}
			break;

		default:;/*NOTREACHED*/
		}
		token = state.token;
	}

	return r;
}



static moExpr::mo_expr_errno_t expr_comma(mo_expr_state_t& state, bool get_token)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_assign(state, get_token);
	if(r != moExpr::MO_EXPR_ERROR_NONE) {
		return r;
	}

	state.param[0].value_type = state.value.value_type;
	switch(state.value.value_type) {
	case MO_EXPR_TOKEN_INTEGER:
		state.param[0].value_int = state.value.value_int;
		break;

	case MO_EXPR_TOKEN_FLOAT:
		state.param[0].value_flt = state.value.value_flt;
		break;

	case MO_EXPR_TOKEN_STRING:
		state.param[0].value_str = state.value.value_str;
		break;

	default:;// unused... avoid warnings (yet we could receive an invalid identifier!)
	}
	state.param_count = 1;
	while(state.token == ',') {
		// get the next expression
		r = expr_assign(state, true);
		if(r != moExpr::MO_EXPR_ERROR_NONE) {
			return r;
		}
		// when there are more parameters, we don't generate an error...
		// this will be done in the <identifier>(...) since in all the
		// other cases, it doesn't matter
		if(state.param_count < MO_EXPR_PARAM_MAX) {
			state.param[state.param_count].value_type = state.value.value_type;
			switch(state.value.value_type) {
			case MO_EXPR_TOKEN_INTEGER:
				state.param[state.param_count].value_int = state.value.value_int;
				break;

			case MO_EXPR_TOKEN_FLOAT:
				state.param[state.param_count].value_flt = state.value.value_flt;
				break;

			case MO_EXPR_TOKEN_STRING:
				state.param[state.param_count].value_str = state.value.value_str;
				break;

			default:;/*NOTREACHED*/
			}
		}
		// count the number of parameters - the array still has a
		// maximum of MO_EXPR_PARAM_MAX! - this is to use in another
		// function to know whether too many parameters were defined
		state.param_count++;
	}

	return moExpr::MO_EXPR_ERROR_NONE;
}





static moExpr::mo_expr_errno_t expr(mo_expr_state_t& state)
{
	moExpr::mo_expr_errno_t	r;

	r = expr_token(state);
	while(r == moExpr::MO_EXPR_ERROR_NONE
	   && state.token != MO_EXPR_TOKEN_EOS) {
		if(state.token != ';') {
			r = expr_comma(state, false);
		}
		else {
			// here we skip the ';'
			r = expr_token(state);
		}
	}

	return r;
}


moWCString moExpr::Result(void) const
{
	// by default we assume an error occurs
	moWCString	result("ERROR");
	mo_expr_state_t	state;
	mo_expr_value_t	variables[MO_EXPR_VARIABLE_MAX];
	char		buf[256];	/* Flawfinder: ignore */

	// initialize the parser state
	state.input = Data();
	state.value.value_type = MO_EXPR_TOKEN_UNKNOWN;
	state.variable = variables;

//fprintf(stderr, "Computing [%s]\nReading -> ", SavedMBData());

#ifdef MO_CONFIG_NO_MUTABLE
	const_cast<mo_expr_errno_t&>(f_errno) = expr(state);
#else
	f_errno = expr(state);
#endif
	if(f_errno == MO_EXPR_ERROR_NONE) {
		switch(state.value.value_type) {
		case MO_EXPR_TOKEN_INTEGER:
			sprintf(buf, "%ld", state.value.value_int);	/* Flawfinder: ignore */
			result = buf;
			break;

		case MO_EXPR_TOKEN_FLOAT:
			sprintf(buf, "%g", state.value.value_flt);	/* Flawfinder: ignore */
			result = buf;
			break;

		case MO_EXPR_TOKEN_STRING:
			result = state.value.value_str;
			break;

		default:
#ifdef MO_CONFIG_NO_MUTABLE
			const_cast<mo_expr_errno_t&>(f_errno) = MO_EXPR_ERROR_SYNTAX;
#else
			f_errno = MO_EXPR_ERROR_SYNTAX;
#endif
			break;

		}
	}

#if 0
if(f_errno != 0) {
fprintf(stderr, "\nFrom [%s] remaining [%S] (errno = %d)\n", SavedMBData(), state.input, f_errno);
}
else {
fprintf(stderr, "\nFrom [%s] got result: [%s]\n", SavedMBData(), result.SavedMBData());
}
#endif

	return result;
}






};			// namespace molib;

// vim: ts=8

