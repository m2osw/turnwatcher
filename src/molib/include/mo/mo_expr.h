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



#ifndef MO_EXPR_H
#define	MO_EXPR_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STRING_H
#include	"mo_string.h"
#endif


namespace molib
{


class MO_DLL_EXPORT moExpr : public moWCString
{
public:
	enum mo_expr_errno_t {
		MO_EXPR_ERROR_NONE = 0,		// no error

		MO_EXPR_ERROR_OPERATOR_TYPE = -1,	// this operator can't be used with this type
		MO_EXPR_ERROR_INVALID_FLOAT = -2,	// a float is defined with a period and no digits or an exponent and not digit
		MO_EXPR_ERROR_INVALID_STRING = -3,	// a float is defined with a period and no digits or an exponent and not digit
		MO_EXPR_ERROR_INVALID_CHARACTER = -4,	// a float is defined with a period and no digits or an exponent and not digit
		MO_EXPR_ERROR_SYNTAX = -5,		// a syntax error was found
		MO_EXPR_ERROR_INDEX = -6,		// an array index was not an integer
		MO_EXPR_ERROR_ARRAY = -7,		// only strings can be used with []
		MO_EXPR_ERROR_FUNCTION = -8,		// a string or identifier was expected for the function name
		MO_EXPR_ERROR_UNKNOWN_FUNCTION = -9,	// the name of the function doesn't remind us anything
		MO_EXPR_ERROR_DIVIDE_BY_ZERO = -10,	// attempting to divide by zero...
		MO_EXPR_ERROR_EXPECTED_COLON = -11,	// colon in ?: not found
		MO_EXPR_ERROR_BAD_TYPE = -12,		// in a ?: the two results don't have the same type
		MO_EXPR_ERROR_VAROVERFLOW = -13,	// too many variables defined
		MO_EXPR_ERROR_VARUNDEFINED = -14,	// variable not defined yet
		MO_EXPR_ERROR_INVALID_PARAM_COUNT = -15,// too few or too many parameters for a function call
		MO_EXPR_ERROR_INVALID_PARAM = -16,	// function parameter type doesn't match
		MO_EXPR_ERROR_EXPECTED_CLOSE = -17,	// expected ) or ]
		MO_EXPR_ERROR_BADREGEXPR = -18,		// bad regular expression

		MO_EXPR_ERROR_max
	};

				moExpr(void);
				moExpr(const moWCString& string);
				moExpr(const moExpr& string);
				moExpr(const char *string, long length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
				moExpr(const mowc::mc_t *string, long length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
				moExpr(const mowc::wc_t *string, long length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
				moExpr(const wchar_t *string, long length = -1);

	// compute the expression and return the result
	moWCString		Result(void) const;
	mo_expr_errno_t		Error(void) const;

private:
	mutable mo_expr_errno_t	f_errno;
};






};			// namespace molib;

// vim: ts=8
#endif		// #ifndef MO_EXPR_H

