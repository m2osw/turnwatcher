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



#ifndef MO_REGEXPR_H
#define	MO_REGEXPR_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_VARIABLE_H
#include	"mo_variable.h"
#endif
#ifndef MO_ARRAY_H
#include	"mo_array.h"
#endif

namespace molib
{


class MO_DLL_EXPORT moRegularExpression : public moWCString
{
public:
	enum mo_regexpr_errno_t {
		MO_REGEXPR_ERROR_NONE = 0,
		MO_REGEXPR_ERROR_BADCLASSNAME,
		MO_REGEXPR_ERROR_BADCOUNT,
		MO_REGEXPR_ERROR_BADEXPRESSION,
		MO_REGEXPR_ERROR_BADGROUP,
		MO_REGEXPR_ERROR_BADREPEAT,
		MO_REGEXPR_ERROR_BADSET,
		MO_REGEXPR_ERROR_BADVARIABLE,
		MO_REGEXPR_ERROR_NOENDCLASS,
		MO_REGEXPR_ERROR_NOENDCOUNT,
		MO_REGEXPR_ERROR_NOENDSET,
		MO_REGEXPR_ERROR_NOENDVARIABLE,
		MO_REGEXPR_ERROR_max
	};

				moRegularExpression(void);
				moRegularExpression(const char *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF8);
				moRegularExpression(const mowc::mc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF16_INTERNAL);
				moRegularExpression(const mowc::wc_t *str, int length = -1, mowc::encoding_t encoding = mowc::MO_ENCODING_UTF32_INTERNAL);
				moRegularExpression(const wchar_t *str, int length = -1);	// encoding defined by running system!
				moRegularExpression(const moWCString& str, int length = -1);
				moRegularExpression(const moRegularExpression& regexpr, int length = -1);
	virtual			~moRegularExpression();

	moRegularExpression&	operator = (const moWCString& str);
	moRegularExpression&	operator = (const moRegularExpression& str);
	moRegularExpression&	operator = (const char *str);
	moRegularExpression&	operator = (const mowc::mc_t *str);
	moRegularExpression&	operator = (const mowc::wc_t *str);
	moRegularExpression&	operator = (const wchar_t *str);

	bool			MatchExpression(const moWCString& string, moVariableList *parameters = 0);
	moWCString		Replace(const moWCString& replacement, const moWCString& subject);
	mo_regexpr_errno_t	LastError(void);
	int			GetStartPos(void) const;
	int			GetLastPos(void) const;

private:
	enum mo_regexpr_t {
		MO_REGEXPR_FREE = 0,	// a free node (usually unused)
		MO_REGEXPR_LINK,	// just a node to link to other nodes
		MO_REGEXPR_EXACT,	// match pattern exactly (x)
		MO_REGEXPR_ANY,		// match one in pattern [...]
		MO_REGEXPR_NONE,	// don't match any in pattern [!...]
		MO_REGEXPR_AFTER,	// checks after the pattern, don't eat the input (r/t)
		MO_REGEXPR_STARTRECORD,	// start recording a variable
		MO_REGEXPR_ENDRECORD,	// end recording a variable
		MO_REGEXPR_START,	// the ^ operator
		MO_REGEXPR_END,		// the $ operator
		MO_REGEXPR_EXIT,	// the end of the regular expression
		MO_REGEXPR_max
	};

	static const unsigned long	MO_REGEXPR_NO_NODE = (unsigned long) -1;

	struct mo_regexpr_node_t {
		mo_regexpr_t		nd_type;
		unsigned long		nd_left;	// this are indexes within the f_node array
		unsigned long		nd_right;
		unsigned long		nd_reset;	// a node where f_count must be reset
		unsigned long		nd_length;	// number of characters in nd_pattern
		const mowc::wc_t *	nd_pattern;	// a pattern to match (or not)
		unsigned long		nd_count;
		unsigned long		nd_min;		// counter limits
		unsigned long		nd_max;
	};

	struct mo_regexpr_analyze_t {
		unsigned long		an_start;
		unsigned long		an_end;
		unsigned long		an_level;
		bool			an_after;
		mowc::wc_t *		an_input;
		mowc::wc_t		an_char;
	};

	struct mo_regexpr_match_t {
		const mowc::wc_t *	ma_string;		// the string being analized
		unsigned long		ma_n;			// the current node by number
		unsigned long		ma_level;		// to avoid deep recursivity
		moVariableList *	ma_parameters;		// the list of variables passed to the MatchExpression() function
		unsigned long		ma_start_node;		// the STARTRECORD node
		const mowc::wc_t *	ma_start_record;	// when matching, the start of a variable to be recorded
		unsigned long		ma_position;		// positioning for variable defined between \( and \)
	};

	void			Init(void);

	mo_regexpr_node_t *	Node(int index);
	unsigned long		NodeAlloc(mo_regexpr_t type = MO_REGEXPR_FREE, unsigned long left = MO_REGEXPR_NO_NODE, unsigned long right = MO_REGEXPR_NO_NODE, const mowc::wc_t *pattern = 0, unsigned long length = 0);
	void			NodeReset(void);

	bool			Analyze(void);
	mowc::wc_t		AnalyzeGetC(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprUnary(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprRepeat(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprConcat(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprOr(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprStart(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprEnd(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprAfter(mo_regexpr_analyze_t& state);
	bool			AnalyzeExprAll(mo_regexpr_analyze_t& state);

	bool			Match(mo_regexpr_match_t& state);
	bool			MatchNow(mo_regexpr_match_t& state);
	bool			MatchMore(mo_regexpr_match_t& state);

	// keep a copy of the string in order to know when it
	// was modified
	zsize_t			f_copy_length;	// the length of the string copy
	zsize_t			f_copy_max;	// the size of the f_copy_string and f_buffer buffers
	mowc::wc_t *		f_copy_string;	// exact copy of the string
	mowc::wc_t *		f_buffer;	// copy of the string parsed
	zbool_t			f_invalid;	// this f_copy_string has an invalid expression

	zuint32_t		f_free;		// next free node
	zuint32_t		f_start;	// the node to start with (may not be 0)
	moArraySPtr		f_node;		// an array of nodes (needs to be allocated!)
	mo_regexpr_errno_t	f_errno;	// last error (set by Analyze...() functions)

	zint32_t		f_start_pos;	// the position where the expression matched first
	zint32_t		f_end_pos;	// the position where the expression matched last
};






};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_REGEXPR_H

