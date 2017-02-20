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



#ifndef MO_SIMPLE_EDITOR_H
#define	MO_SIMPLE_EDITOR_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_REGEXPR_H
#include	"mo_regexpr.h"
#endif
#ifndef MO_TEXT_STREAM_H
#include	"mo_text_stream.h"
#endif
#ifndef MO_FILE_H
#include	"mo_file.h"
#endif



namespace molib
{

class MO_DLL_EXPORT moSimpleEditor : public moBase
{
public:
	class moSedInstruction : public moBase
	{
	public:
		enum action_t {
			// matching
			ACTION_MATCH = 0,		// check for a match
			ACTION_NO_CASE_MATCH,		// same ignoring case
			ACTION_REPLACE_MATCH,		// replace matching strings
			ACTION_NO_CASE_REPLACE_MATCH,
			ACTION_REPLACE_ALL_MATCHES,	// replace all matching strings
			ACTION_NO_CASE_REPLACE_ALL_MATCHES,
			ACTION_TRANSPOSE,		// transpose letters from a set to another
			ACTION_NO_CASE_TRANSPOSE,

			// read/write/delete input/output
			ACTION_NEXT,			// read next line of input immediately, lose pattern
			ACTION_APPEND_NEXT,		// append next line of input to pattern
			ACTION_DELETE,			// delete pattern space
			ACTION_DELETE_FIRST_LINE,	// delete first line of pattern space
			ACTION_DELETE_LAST_LINE,	// delete last (current) line of pattern space
			ACTION_APPEND,			// append text to pattern space (after current line)
			ACTION_INSERT,			// insert text to pattern space (before current line)
			ACTION_REPLACE,			// replace current line with this text in pattern space
			ACTION_PRINT,			// print pattern space in output
			ACTION_PRINT_FIRST_LINE,	// print first line of pattern space in output
			ACTION_PRINT_LAST_LINE,		// print last line of pattern space in output

			// access other files
			ACTION_READ_FROM_FILE,		// read text from specified file
			ACTION_READ_ONE_LINE_FROM_FILE,	// read one line of text from specified file
			ACTION_WRITE_TO_FILE,		// write pattern and current line to specified file
			ACTION_WRITE_FIRST_LINE_TO_FILE,// write first line in pattern to specified file
			ACTION_WRITE_LAST_LINE_TO_FILE,	// write current line to specified file

			// temporary storage
			ACTION_SET_VARIABLE,		// store in a variable
			ACTION_APPEND_TO_VARIABLE,	// append to variable
			ACTION_GET_VARIABLE,		// replace pattern with variable content
			ACTION_APPEND_FROM_VARIABLE,	// append variable to pattern
			ACTION_EXCHANGE,		// exchange pattern and variable contents

			// branching
			ACTION_QUIT,			// stop processing and printing at once
			ACTION_BRANCH,			// branch always
			ACTION_BRANCH_IF_MATCHED,	// branch if we had a match (//)
			ACTION_BRANCH_IF_NOT_MATCHED,	// branch if we did not have a match (//)
			ACTION_BRANCH_IF_REPLACED,	// branch if we had a substitution (s///)
			ACTION_BRANCH_IF_NOT_REPLACED,	// branch if we did not have a substitution (s///)
			ACTION_LABEL,			// a label for branches to go somewhere

			// user action
			ACTION_CALLBACK,		// call the Instruction callback (need to be overloaded)

			ACTION_max
		};

					moSedInstruction(action_t action);
					moSedInstruction(const moSedInstruction& source);
		virtual			~moSedInstruction();

		moSedInstruction&	operator = (const moSedInstruction& rhs);

		// <start>,<end>
		void			StartLine(int line);
		void			EndLine(int line);
		// <start>~<step>
		void			Step(int step);

		// /<start>/,/<end>/
		void			StartPattern(const moWCString& pattern);
		void			EndPattern(const moWCString& pattern);

		// s/<match>/<replace>/ or y/<source>/<destination>/
		void			Match(const moWCString& match);

		// text, filename or label
		void			Text(const moWCString& text);

		// if address matches, call this if ACTION_CALLBACK
		// return true means QUIT now
		virtual bool		Callback(moSimpleEditor& sed);

	private:
		/// Made the simple editor my friend to simplify my life, but that should be changed...
		friend class moSimpleEditor;

		// Address
		zint32_t		f_matched_line;		// line we got the 1st match; or 0
		mint32_t		f_start_line;		// <start>,<end>
		zint32_t		f_end_line;
		zint32_t		f_step;			// <start>,<step>

		moRegularExpression	f_start_pattern;	// /<start>/,/<end>/
		moRegularExpression	f_end_pattern;

		// Action
		action_t		f_action;		// what shall we do when we get a match?
		moRegularExpression	f_match;		// s/<match>/<replace>/
		moWCString		f_text;			// replace, text, filename or label

		// For file actions
		moFile			f_file;			// input or output file for R/W instructions
	};
	typedef moSmartPtr<moSedInstruction>		moSedInstructionSPtr;
	typedef moTmplList<moSedInstruction, moList>	moListOfSedInstructions;

				moSimpleEditor();

	int			SetInputEncoding(const char *encoding);
	int			SetOutputEncoding(const char *encoding);

	void			AddInstruction(const moSedInstruction& inst);
	void			ClearInstructions();

	void			Parse(const moWCString& input_filename, const moWCString& output_filename);
	void			Parse(moIStream& input, moOStream& output);
	void			Parse(moTextStream& text_stream);

private:
	void			StringToList(const moWCString& str, moListOfWCStrings& pattern, moWCString& line);
	moWCString		ListToString(const moListOfWCStrings& list, const moWCString& line);
	int			FindLabel(const moWCString& name);

	moListOfSedInstructions	f_instructions;
	mowc::moIConv		f_input_convertor;
	mowc::moIConv		f_output_convertor;
};

typedef moSmartPtr<moSimpleEditor>		moSimpleEditorSPtr;
typedef moTmplList<moSimpleEditor, moList>	moListOfSimpleEditors;

};			// namespace molib

// vim: ts=8 sw=8
#endif		// #ifndef MO_SIMPLE_EDITOR_H

