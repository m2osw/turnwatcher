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
#pragma implementation "mo/mo_simple_editor.h"
#endif

#include	"mo/mo_simple_editor.h"

#ifndef MO_MEMFILE_H
#include	"mo/mo_memfile.h"
#endif



namespace molib
{



/** \class moSimpleEditor
 *
 * \brief Simple editor in a library.
 *
 * This object is a replacement for the famous Unix tool called
 * sed(1).
 *
 * You need to call AddInstruction() with all the instructions you
 * want to apply to your source files. For instance, if you want to
 * replace \@PATH@ with a path that is known at install time or at
 * run time, use the moSedInstruction::ACTION_REPLACE_ALL_MATCHES
 * with \@PATH@ in the moSedInstruction::Match() and the actual
 * path in Text().
 *
 * \code
 *	moSimpleEditor sed;
 *	// add the replace instruction
 *	moSimpleEditor::moSedInstruction inst_replace(moSimpleEditor::moSedInstruction::ACTION_REPLACE_ALL_MATCHES);
 *	inst_replace.Match("@PATH@");
 *	inst_replace.Text(path);
 *	sed.AddInstruction(inst_replace);
 *	// print the result
 *	moSimpleEditor::moSedInstruction inst_print(moSimpleEditor::moSedInstruction::ACTION_PRINT);
 *	sed.AddInstruction(inst_print);
 *	// parse files (you can run the sed against multiple files in a row)
 *	sed.Parse(in1, out1);
 *	sed.Parse(in2, out2);
 *	sed.Parse(in3, out3);
 * \endcode
 *
 * Note that address matching can be applied to all instructions, though
 * the address matching of labels is always ignored.
 *
 * According to sed, the append, insert, quit and read file should
 * have only the start address matching, no end line or pattern.
 */


/** \typedef molib::moSimpleEditor::moSedInstructionSPtr
 *
 * \brief Defines a smart pointer for the Sed instruction
 *
 * This typedef can be used when dynamically allocating
 * Sed instruction objects. The life time of the instruction
 * will then be automatically managed.
 */


/** \typedef molib::moSimpleEditor::moListOfSedInstructions
 *
 * \brief Defines a list of Sed instructions
 *
 * This typedef simplifies the management of lists of Sed instructions.
 * This is used internally for the list of instructions used by the
 * simple editor class.
 */


/** \var moSimpleEditor::f_instructions
 *
 * \brief List of Sed instructions
 */

/** \var moSimpleEditor::f_input_convertor
 *
 * \brief The input stream character convertor
 */

/** \var moSimpleEditor::f_output_convertor
 *
 * \brief The output stream character convertor
 */


/** \class moSimpleEditor::moSedInstruction
 *
 * \brief One simple editor instruction
 *
 * This class is used to create instructions to run the simple text
 * editor against some input file.
 *
 * The instructions are very similar to those found in the Unix tool
 * named sed(1). Whenever there is an sed equivalent to an instruction,
 * this one is noted. For instance, the
 * moSedInstruction::ACTION_REPLACE_ALL_MATCHES is equivalent to:
 *
 * \code
 * sed -e 's/<pattern>/<replace>/g' ...
 * \endcode
 */


/** \var moSimpleEditor::moSedInstruction::f_matched_line
 *
 * \brief The line number on which we got the first match; or 0
 *
 * This variable is set to the line number which was the first
 * to match one of the start addresses.
 */

/** \var moSimpleEditor::moSedInstruction::f_start_line
 *
 * \brief The line number that represents a match.
 *
 * This defines the line number that the parser needs to reach
 * before the address is considered to match.
 *
 * By default this is set to -1 meaning that the line number is
 * ignored.
 *
 * This variable can be set to 0 meaning that the instruction is
 * considered to match from before the parsing started. This means
 * the end address can turn off the match on line 1.
 */

/** \var moSimpleEditor::moSedInstruction::f_end_line
 *
 * \brief The line number that represents the end of a match.
 *
 * This defines the line number that the parser needs to reach
 * before the address is considered to not match anymore.
 *
 * This is similar to the f_start_line variable. Note that if
 * end and start are equal, then the line is not a match.
 *
 * The end cannot be set to zero. It is set to -1 by default.
 */

/** \var moSimpleEditor::moSedInstruction::f_step
 *
 * \brief Match every time that many lines are read.
 *
 * After a line matched the start address, f_step - 1 lines
 * are skipped, before another line is considered a match.
 */

/** \var moSimpleEditor::moSedInstruction::f_start_pattern
 *
 * \brief A pattern starting a matching area
 *
 * Each line is checked against the start pattern. If the string
 * is empty, it is considered to always match (i.e. '.*').
 *
 * The match will last until the end pattern match if there is
 * one. Otherwise, it lasts only for this one line.
 */

/** \var moSimpleEditor::moSedInstruction::f_end_pattern;
 *
 * \brief A pattern ending a matching area
 *
 * One an instruction start pattern (or start line) matched, then
 * the end pattern is checked. If the end pattern matches, the
 * line is skipped (unless that very line matched the start pattern
 * right here.)
 */

/** \var moSimpleEditor::moSedInstruction::f_action
 *
 * \brief The code to execute when this instruction is reached.
 *
 * This variable represents what action to take when executing this
 * instruction. This should actually disappear and a virtual Execute()
 * function be created. This would make it a lot more object oriented.
 */

/** \var moSimpleEditor::moSedInstruction::f_match
 *
 * \brief The matching pattern of a match and replace or transpose.
 *
 * This is the pattern used to match the input before replacing it.
 *
 * The match by itself also uses it. The transpose uses it as the source.
 */

/** \var moSimpleEditor::moSedInstruction::f_text
 *
 * \brief replace, text, filename or label
 *
 * The text used to replace, as text to insert, append, etc. as a
 * filename or as the label of a branchment.
 */

/** \var moSimpleEditor::moSedInstruction::f_file
 *
 * \brief Input or output file for R/W instructions
 *
 * This file object is used by the R/W instructions to make sure that
 * the same file is not opened and closed all the time.
 */


/** \enum moSimpleEditor::moSedInstruction::action_t
 *
 * \brief Enumeration of all the possible instructions
 *
 * This enumeration lists all the possible instructions.
 * Note that at some point I would like to transform these
 * in seperate objects instead. Yet that means 36 objects!
 * (the callback functionality would automatically be
 * available without having to create such an extra
 * instruction.)
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_MATCH
 *
 * \brief Match with a regular expression (Group: Matching)
 *
 * Match the pattern. Useful with the conditional branches and without having to
 * transform the matching pattern.
 *
 * This is equivalent, but faster than this:
 *
 * \code
 *	s/<pattern>/&/
 * \endcode
 *
 * \sa moSedInstruction::Match()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_NO_CASE_MATCH
 *
 * \brief Match with a regular expression (Group: Matching)
 *
 * Match the pattern. Useful with the conditional branches and without having to
 * transform the matching pattern. The matching is case insensitive.
 *
 * This is equivalent, but faster than this:
 *
 * \code
 *	s/<pattern>/&/i
 * \endcode
 *
 * \sa moSedInstruction::Match()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_REPLACE_MATCH
 *
 * \brief Replace the first matching pattern (Group: Replacing)
 *
 * This action searches for the first match and replaces it with
 * the replace text.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/<replace>/
 * \endcode
 *
 * \sa moSedInstruction::Match()
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_NO_CASE_REPLACE_MATCH
 *
 * \brief Replace the first matching pattern (Group: Replacing)
 *
 * This action searches for the first match and replaces it with
 * the replace text. The matching is case insensitive.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/<replace>/i
 * \endcode
 *
 * \sa moSedInstruction::Match()
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_REPLACE_ALL_MATCHES
 *
 * \brief Replace all the matching patterns (Group: Replacing)
 *
 * This action searches for all the matches and replaces them with
 * the replace text.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/<replace>/g
 * \endcode
 *
 * \sa moSedInstruction::Match()
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_NO_CASE_REPLACE_ALL_MATCHES
 *
 * \brief Replace all the matching patterns (Group: Replacing)
 *
 * This action searches for all the matches and replaces them with
 * the replace text. The matching is case insensitive.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/<replace>/gi
 * \endcode
 *
 * \sa moSedInstruction::Match()
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_TRANSPOSE
 *
 * \brief Transpose all the matching characters (Group: Replacing)
 *
 * This action transposes the characters from the pattern that
 * match a character in source with the corresponding character
 * in destination. The transposition is applied once only. For
 * instance, if you want to change all the X in Y and Y in X,
 * you can use y/XY/YX/.
 *
 * This is equivalent to:
 *
 * \code
 *	y/<source>/<destination>/
 * \endcode
 *
 * \sa moSedInstruction::Match()
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_NO_CASE_TRANSPOSE,
 *
 * \brief Transpose all the matching characters (Group: Replacing)
 *
 * This action transposes the characters from the pattern that
 * match a character in source with the corresponding character
 * in destination. The transposition is applied once only.
 *
 * The matching between the pattern and the source is done ignoring
 * case.
 *
 * For instance, if you want to change all the X in Y and Y in X,
 * you can use y/XY/YX/.
 *
 * This is equivalent to:
 *
 * \code
 *	y/<source>/<destination>/i
 * \endcode
 *
 * \sa moSedInstruction::Match()
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_NEXT
 *
 * \brief Read the next line of input (Group: Action)
 *
 * This instruction clears the current pattern buffer and
 * read one line from the input.
 *
 * This is equivalent to:
 *
 * \code
 *	n
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_APPEND_NEXT
 *
 * \brief Read the next line of input (Group: Action)
 *
 * This instruction read one line from the input and adds it
 * to the current list of patterns.
 *
 * One such instruction is automatically inserted at the start
 * of the list. You currently cannot avoid running it once at
 * the very beginning. However, you can avoid running it again
 * using branches with labels properly placed.
 *
 * This is equivalent to:
 *
 * \code
 *	N
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_DELETE
 *
 * \brief Clear the pattern space (Group: Action)
 *
 * This instruction release the pattern space entirely. This
 * means a print will have no effect after a delete (at least,
 * it shouldn't have any effect.)
 *
 * This is equivalent to:
 *
 * \code
 *	d
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_DELETE_FIRST_LINE
 *
 * \brief Delete the first line from the pattern space (Group: Action)
 *
 * This instruction releases the first line from the pattern space.
 *
 * This is equivalent to:
 *
 * \code
 *	D
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_DELETE_LAST_LINE
 *
 * \brief Delete the last line from the pattern space (Group: Action)
 *
 * This instruction releases the last line from the pattern space.
 * The line before the last becomes the current line.
 *
 * There is no simple equivalent in sed.
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_APPEND
 *
 * \brief Append text to the output (Group: Action)
 *
 * This instruction adds the current line of pattern space to
 * the pattern list, then add the text to the pattern space.
 *
 * This is equivalent to:
 *
 * \code
 *	a text
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_INSERT
 *
 * \brief Insert text to the output (Group: Action)
 *
 * This instruction adds the text to the pattern space. The
 * current pattern line remains the same.
 *
 * This is equivalent to:
 *
 * \code
 *	i text
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_REPLACE
 *
 * \brief Replace text (Group: Action)
 *
 * This instruction replaces the text in the pattern space. The
 * current pattern line is lost.
 *
 * This is equivalent to:
 *
 * \code
 *	c text
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_PRINT
 *
 * \brief Print text in output (Group: Action)
 *
 * This instruction prints the pattern space in the output
 * and then clears the pattern space.
 *
 * This is equivalent to:
 *
 * \code
 *	p
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_PRINT_FIRST_LINE
 *
 * \brief Print first line of pattern space in output (Group: Action)
 *
 * This instruction prints the first line of the pattern space in the output
 * and clears it.
 *
 * This is equivalent to:
 *
 * \code
 *	P
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_PRINT_LAST_LINE
 *
 * \brief Print first line of pattern space in output (Group: Action)
 *
 * This instruction prints the first line of the pattern space in the output
 * and clears it.
 *
 * There is no simple equivalent in sed.
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_READ_FROM_FILE
 *
 * \brief Copy a file in the output (Group: File)
 *
 * This instruction reads a file and writes it unchanged in the output.
 *
 * This is equivalent to:
 *
 * \code
 *	r filename
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_READ_ONE_LINE_FROM_FILE
 *
 * \brief Copy one line from a file in the output (Group: File)
 *
 * This instruction reads the next line from a file and writes it unchanged
 * in the output. The first time this instruction is hit, the file is opened.
 * The next time, the next line is read.
 *
 * This is equivalent to:
 *
 * \code
 *	R filename
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_WRITE_TO_FILE
 *
 * \brief Copy the entire pattern space in the specified file (Group: File)
 *
 * This instruction writes the entire pattern space (like
 * moSedInstruction::ACTION_PRINT) to the specified file.
 *
 * This is equivalent to:
 *
 * \code
 *	w filename
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_WRITE_FIRST_LINE_TO_FILE
 *
 * \brief Copy the first line of the pattern space to the specified file (Group: File)
 *
 * This instruction writes the first line of the pattern space (like
 * moSedInstruction::ACTION_PRINT_FIRST_LINE) to the specified file.
 *
 * This is equivalent to:
 *
 * \code
 *	W filename
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_WRITE_LAST_LINE_TO_FILE
 *
 * \brief Copy the current pattern line to the specified file (Group: File)
 *
 * This instruction writes the current line of the pattern space (like
 * moSedInstruction::ACTION_PRINT_LAST_LINE) to the specified file.
 *
 * There is no simple equivalent in sed.
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_SET_VARIABLE
 *
 * \brief Save the current pattern space in a variable (Group: Variable)
 *
 * This instruction saves the current pattern space in the variable named
 * by Text().
 *
 * This is equivalent to:
 *
 * \code
 *	h
 * \endcode
 *
 * except that our version supports any variable name. This lets you use
 * any number of hold spaces.
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_APPEND_TO_VARIABLE
 *
 * \brief Append the current pattern space to a variable (Group: Variable)
 *
 * This instruction appends the current pattern space to a variable named
 * by Text(). If the variable does not exist yet, it is created.
 *
 * This is equivalent to:
 *
 * \code
 *	H
 * \endcode
 *
 * except that our version supports any variable name. This lets you use
 * any number of hold spaces.
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_GET_VARIABLE
 *
 * \brief Read the content of a variable (Group: Variable)
 *
 * This instruction reads the content of a variable and replaces the current
 * pattern space. The variable name is specified with Text().
 *
 * This is equivalent to:
 *
 * \code
 *	g
 * \endcode
 *
 * except that our version supports any variable name. This lets you use
 * any number of hold spaces.
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_APPEND_FROM_VARIABLE
 *
 * \brief Read and append the content of a variable (Group: Variable)
 *
 * This instruction reads the content of a variable and appends it to the
 * current pattern space. The variable name is specified with Text().
 *
 * This is equivalent to:
 *
 * \code
 *	G
 * \endcode
 *
 * except that our version supports any variable name. This lets you use
 * any number of hold spaces.
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_EXCHANGE
 *
 * \brief Exchange the content of a variable and the pattern space (Group: Variable)
 *
 * This instruction exchanges the content of a variable and the current
 * pattern space. The variable name is specified with Text().
 *
 * This is equivalent to:
 *
 * \code
 *	x
 * \endcode
 *
 * except that our version supports any variable name. This lets you use
 * any number of hold spaces.
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_QUIT
 *
 * \brief Quit processing right away (Group: Branch)
 *
 * This instruction stops the processing at once. No print happens before
 * quitting. If you need to print, add an moSedInstruction::ACTION_PRINT
 * before the moSedInstruction::ACTION_QUIT.
 *
 * This is equivalent to:
 *
 * \code
 *	q
 * \endcode
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_BRANCH
 *
 * \brief Always go to the specified label (Group: Branch)
 *
 * This instruction changes the instruction pointer to the location of
 * the specified label. If no label is specified, the location changes
 * to the end of the script (which in effect makes it loop back at the
 * start and execute the default moSedInstruction::ACTION_APPEND_NEXT
 * instruction.)
 *
 * This is equivalent to:
 *
 * \code
 *	b label
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_BRANCH_IF_MATCHED
 *
 * \brief Go to the specified label if a match occured (Group: Branch)
 *
 * This instruction jumps to the specified label like
 * moSedInstruction::ACTION_BRANCH if the last
 * moSedInstruction::ACTION_MATCH and equivalents did match.
 *
 * The branch clears the match flag and thus a second branch without an
 * intermediate match will not be taken.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/&/
 *	...
 *	t label
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_BRANCH_IF_NOT_MATCHED
 *
 * \brief Go to the specified label when no match occured (Group: Branch)
 *
 * This instruction jumps to the specified label like
 * moSedInstruction::ACTION_BRANCH if
 * the last moSedInstruction::ACTION_MATCH and equivalents did not match.
 *
 * The branch clears the match flag and thus a second branch without an
 * intermediate match will always be taken.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/&/
 *	...
 *	T label
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_BRANCH_IF_REPLACED
 *
 * \brief Go to the specified label if a replacement occured (Group: Branch)
 *
 * This instruction jumps to the specified label like
 * moSedInstruction::ACTION_BRANCH if
 * the last moSedInstruction::ACTION_REPLACE and equivalents did apply
 * a replacement.
 *
 * The branch clears the replace flag and thus a second branch without an
 * intermediate replace will not be taken.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/<replace>/
 *	...
 *	t label
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_BRANCH_IF_NOT_REPLACED
 *
 * \brief Go to the specified label when no replacement occured (Group: Branch)
 *
 * This instruction jumps to the specified label like
 * moSedInstruction::ACTION_BRANCH if
 * the last moSedInstruction::ACTION_REPLACE and equivalents did not apply
 * a replacement.
 *
 * The branch clears the replace flag and thus a second branch without an
 * intermediate replace will always be taken.
 *
 * This is equivalent to:
 *
 * \code
 *	s/<pattern>/<replace>/
 *	...
 *	T label
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_LABEL
 *
 * \brief Defines a label (Group: Branch)
 *
 * This instruction has no effect. It is used to define the place where a
 * branch will continue when the corresponding name is specified.
 *
 * This is equivalent to:
 *
 * \code
 *	: label
 * \endcode
 *
 * \sa moSedInstruction::Text()
 *
 * <br>
 * <br>
 */

/** \var moSimpleEditor::moSedInstruction::ACTION_CALLBACK
 *
 * \brief User defined callback (Group: Callback)
 *
 * Internally, this instruction has no effect. It will simply call the used
 * defined callback. At this time, it is not possible to change the pointer
 * (i.e. do the equivalent of a branch) or test the current state. If need
 * be, we can add this functionality to the editor.
 *
 * There is no equivalent in sed.
 *
 * <br>
 * <br>
 */



/** \brief Initializes the Sed instruction with defaults
 *
 * This constructor marks this instruction as not having any
 * address matching, no match and no text.
 *
 * These parameters need to be set later with the other functions
 * of the moSedInstruction object.
 *
 * \sa StartLine()
 * \sa EndLine()
 * \sa Step()
 * \sa StartPattern()
 * \sa EndPattern()
 * \sa Match()
 * \sa Text()
 */
moSimpleEditor::moSedInstruction::moSedInstruction(action_t action)
	: f_start_line(-1),
	  f_action(action)
{
	if(action < 0 || action > ACTION_max) {
		throw moError(MO_ERROR_INVALID, "Invalid action for a Simple Editor Instruction");
	}
}


/** \brief Copy an Sed instruction
 *
 * This copy operator duplicates an Sed instruction in another.
 *
 * Note that the file is not copied. So if a file was already opened
 * in the source, the destination simple gets a closed file handle.
 * (note that needs to be fixed since different r/R/w/W need to load
 * or save in the exact same file if they reference the exact same
 * file.)
 *
 * \param[in] source The source to copy in this instruction
 */
moSimpleEditor::moSedInstruction::moSedInstruction(const moSedInstruction& source)
	: moBase(source),
	  f_matched_line(source.f_matched_line),
	  f_start_line(source.f_start_line),
	  f_end_line(source.f_end_line),
	  f_step(source.f_step),
	  f_start_pattern(source.f_start_pattern),
	  f_end_pattern(source.f_end_pattern),
	  f_action(source.f_action),
	  f_match(source.f_match),
	  f_text(source.f_text)
{
}



/** \brief Cleans up an Sed instruction
 *
 * This destructor is defined mainly because the class is
 * virtual.
 */
moSimpleEditor::moSedInstruction::~moSedInstruction()
{
}



/** \brief Copy one instruction in another
 *
 * This copy operator duplicates an Sed instruction in another.
 *
 * Note that the file is not copied. So if a file was already opened
 * in the source, the destination simple gets a closed file handle.
 * (note that needs to be fixed since different r/R/w/W need to load
 * or save in the exact same file if they reference the exact same
 * file.)
 *
 * \param[in] rhs The source to copy in this instruction
 */
moSimpleEditor::moSedInstruction& moSimpleEditor::moSedInstruction::operator = (const moSedInstruction& rhs)
{
	if(this != &rhs) {
		f_matched_line = rhs.f_matched_line;
		f_start_line = rhs.f_start_line;
		f_end_line = rhs.f_end_line;
		f_step = rhs.f_step;
		f_start_pattern = rhs.f_start_pattern;
		f_end_pattern = rhs.f_end_pattern;
		f_action = rhs.f_action;
		f_match = rhs.f_match;
		f_text = rhs.f_text;
	}

	return *this;
}



/** \brief Line number when this starts matching
 *
 * This function is used to indicate the line number where the
 * instruction becomes active.
 *
 * The special line number of zero (0) means that the instruction
 * is active before the editor starts looping. This means if the
 * end line or pattern match the very first line, it will turn off
 * the instruction on the very first line. This is different from
 * using 1 since the very first line cannot be skipped when using
 * 1.
 *
 * \param[in] line The first line number matching
 */
void moSimpleEditor::moSedInstruction::StartLine(int line)
{
	f_start_line = line;
}


/** \brief Line number when this stops matching
 *
 * This function is used to indicate the line number where the
 * instruction becomes inactive.
 *
 * This ends the matching except of the current line if the
 * current line also matched a start line or pattern.
 *
 * Note that if the end line is larger than the start line,
 * the start line will match this once anyway. If you need to
 * not execute that instruction, don't insert it in the editor
 * or set the start line to a very large number
 * (i.e. 2 ** 31 - 1).
 *
 * \param[in] line The last line number matching
 */
void moSimpleEditor::moSedInstruction::EndLine(int line)
{
	f_end_line = line;
}


/** \brief Number of lines between each match
 *
 * This function defines the lines that match within a block.
 *
 * You start a matching block with StartLine() or StartPattern().
 * The line that matched first is accepted as a match. Then one
 * new line is accepted every 'step' lines. Thus, by default, you
 * can consider that step is set to 1. If you use 2, you will get
 * every other lines.
 *
 * Use StartLine(1) and Step(2) to get all the odd lines. Use
 * StartLine(2) and Step(2) to get all the even lines.
 *
 * The math goes like this:
 *
 * if ( (current_line - first_matched_line) % step == 0 ) is true
 * it is a match.
 *
 * \param[in] step The line stepping amount
 */
void moSimpleEditor::moSedInstruction::Step(int step)
{
	f_step = step;
}


/** \brief Match pattern is a match for this entry
 *
 * This function sets the start pattern used to know whether the
 * instruction shall be applied.
 *
 * When no end pattern is defined, the instruction is applied to
 * only the lines matching this pattern. Otherwise, it is applied
 * to all the lines between the matching start and the matching
 * end patterns.
 *
 * \param[in] pattern The pattern to match
 */
void moSimpleEditor::moSedInstruction::StartPattern(const moWCString& pattern)
{
	f_start_pattern = pattern;
}


/** \brief Match pattern stops the match for this entries
 *
 * This function sets the pattern used to stop the matching started
 * with a StartLine() or a StartPattern().
 *
 * If the line matches this pattern, do not apply the instruction to the
 * line unless the start line or pattern matched in this line.
 *
 * \param[in] pattern The pattern to match
 */
void moSimpleEditor::moSedInstruction::EndPattern(const moWCString& pattern)
{
	f_end_pattern = pattern;
}


/** \brief Match pattern of the match, replace and transpose instructions
 *
 * This function sets the pattern used by the match, replace and
 * transpose instructions.
 *
 * In case of the match and replace, it is the \<pattern> regular expression
 * without the starting and ending / characters.
 *
 * \code
 *	s/<pattern>/<replace>/
 * \endcode
 *
 * In case of the transpose, it is the set of characters to transpose.
 * At this time the transpose does not support ranges.
 *
 * \param[in] match The matching pattern or transpose source characters
 */
void moSimpleEditor::moSedInstruction::Match(const moWCString& match)
{
	f_match = match;
}


/** \brief Text used by many instructions
 *
 * This function sets the text used by many instructions as follow:
 *
 * \li moSedInstruction::ACTION_REPLACE_MATCH and related
 *
 * \par
 * In case of the replace instruction, it is the \<replace> string
 * without the starting and ending / characters.
 *
 * \par
 * \code
 *	s/<pattern>/<replace>/
 * \endcode
 *
 * \par
 * The replace string can include simple back references \\1 to \\9
 * large back references ${\<number>} and named back references ${\<name>}.
 * For PHP equivalent support, we also accept $1 to $9. These references
 * include what was defined between \( and \) in the source. In the case
 * of the \( and \), the positional is defined starting at 1. It can
 * also be specified as in: \(\<pos>$...\). The pattern can also include
 * a named variable defined between curly brackets: {\<name>=...}. In
 * that case, you need to use ${\<name>} to reference such variables.
 * Note that positional variables are saved with the name "v\<pos>". So
 * you can access them with ${\<name>} as well.
 *
 * \li moSedInstruction::ACTION_TRANSPOSE, moSedInstruction::ACTION_NO_CASE_TRANSPOSE
 *
 * \par
 * This is the \<destination> string of the transpose.
 *
 * \par
 * \code
 *	y/<source>/<destination>/
 * \endcode
 *
 * \li moSedInstruction::ACTION_APPEND, moSedInstruction::ACTION_INSERT,
 * moSedInstruction::ACTION_REPLACE
 *
 * \par
 * The text to append after the pattern, insert before the pattern or
 * used to replace the pattern.
 *
 * \li moSedInstruction::ACTION_READ_FROM_FILE,
 * moSedInstruction::ACTION_WRITE_TO_FILE and related
 *
 * \par
 * The text defines the filename.
 *
 * \li moSedInstruction::ACTION_SET_VARIABLE and related
 *
 * \par
 * The text defines the variable name.
 *
 * \li moSedInstruction::ACTION_LABEL,
 * moSedInstruction::ACTION_BRANCH and related
 *
 * \par
 * The text defines the label.
 *
 * \param[in] text The new text string
 */
void moSimpleEditor::moSedInstruction::Text(const moWCString& text)
{
	f_text = text;
}


/** \brief User function called whenever this insruction is reached.
 *
 * This function shall be overwritten by the user to defines what the
 * instruction shall do. At this time, the state of the simple editor
 * is not directly accessible by the callback and thus it makes the
 * callback somewhat not useful. Once we have a clearer idea of the
 * usefulness of this function, we can implement the necessary
 * state.
 *
 * The function is expected to return true if processing needs to
 * stop. In this case, the instruction behaves like
 * moSedInstruction::ACTION_QUIT.
 *
 * \param[in] sed A reference to the simple editor
 *
 * \return false to continue; true to quit the simple editor at once
 */
bool moSimpleEditor::moSedInstruction::Callback(moSimpleEditor& sed)
{
	// ignore this instruction by default
	return false;
}




/** \brief Initializes the simple editor.
 *
 * This function initializes the simple editor by initializes the
 * input and output text convertors and adding the default
 * moSedInstruction::ACTION_APPEND_NEXT in the list of instructions.
 */
moSimpleEditor::moSimpleEditor()
{
	f_input_convertor.SetEncodings("UTF-8", mowc::g_ucs4_internal);
	f_output_convertor.SetEncodings(mowc::g_ucs4_internal, "UTF-8");

	// we need to start with an Append Next (i.e. read a line of input)
	moSedInstruction next_line(moSedInstruction::ACTION_APPEND_NEXT);
	AddInstruction(next_line);
}


/** \brief Change the input encoding
 *
 * This function lets you change the input encoding to what it really
 * is. By default, the simple editor assumes that the input is in
 * UTF-8. If another format is used, you must specify it here.
 *
 * It is legal to use "*" as the encoding in which case the system
 * will try to infer the encoding.
 *
 * \param[in] encoding The name of the input encoding
 *
 * \return 0 when the function succeeds, -1 otherwise
 */
int moSimpleEditor::SetInputEncoding(const char *encoding)
{
	return f_input_convertor.SetEncodings(encoding, "UTF-32");
}


/** \brief Change the output encoding
 *
 * This function lets you change the output encoding to what it really
 * is. By default, the simple editor assumes that the output is in
 * UTF-8. If another format is necessary, you must specify it here.
 *
 * \param[in] encoding The name of the output encoding
 *
 * \return 0 when the function succeeds, -1 otherwise
 */
int moSimpleEditor::SetOutputEncoding(const char *encoding)
{
	return f_output_convertor.SetEncodings("UTF-32", encoding);
}


/** \brief Add an instruction to the simple editor
 *
 * This function adds one instruction to the simple editor.
 *
 * The function duplicates your instruction and thus you are free
 * to change it and call this function again if you need to. Also
 * you can define you instructions on the stack.
 *
 * Any number of instructions can be added. Note that the simple
 * editor constructor adds the moSedInstruction::ACTION_APPEND_NEXT
 * instruction automatically.
 *
 * \param[in] inst The instruction to add.
 */
void moSimpleEditor::AddInstruction(const moSedInstruction& inst)
{
	f_instructions += *new moSedInstruction(inst);
}


/** \brief Clear all the instructions
 *
 * This function clears all the instructions so you can re-assign
 * new ones and reuse this simple editor object.
 *
 * Note that the moSedInstruction::ACTION_APPEND_NEXT instruction
 * automatically added by the constructor is kept in the list.
 */
void moSimpleEditor::ClearInstructions()
{
	// note that we never delete the very first instruction
	f_instructions.SetSize(1);
}


/** \brief Transform a list of string into one string
 *
 * This function transforms an array of strings into one long string
 * with each string separated by a newline.
 *
 * The line is appended at the very end of the string without a newline.
 *
 * \param[in] list The list to convert
 * \param[in] line The last string to append to the result
 *
 * \return All the strings concatenated with '\n' in between
 */
moWCString moSimpleEditor::ListToString(const moListOfWCStrings& list, const moWCString& line)
{
	moWCString	result;
	int		idx, max;

	max = list.Count();
	for(idx = 0; idx < max; ++idx) {
		if(idx > 0) {
			result += "\n";
		}
		result += *list.Get(idx);
	}
	if(max > 0) {
		result += "\n";
	}
	result += line;

	return result;
}


/** \brief Transform string in an array of strings
 *
 * This function transforms a strings into an array of strings.
 * The string is cut at the newline characters.
 *
 * The very last string is saved in line instead of the list.
 *
 * \param[in] str The string to break up in strings
 * \param[in] pattern The array where the strings are saved
 * \param[in] line Save the last string in this variable
 */
void moSimpleEditor::StringToList(const moWCString& str, moListOfWCStrings& pattern, moWCString& line)
{
	const mowc::wc_t	*s, *start;
	bool			first;

	line.Empty();
	first = true;
	s = str.Data();
	while(*s != '\0') {
		if(!first) {
			pattern += *new moWCString(line);
		}
		first = false;

		start = s;
		while(*s != '\n' && *s != '\0') {
			++s;
		}
		line.Set(start, static_cast<int>(s - start));
		if(*s == '\n') {
			++s;
		}
	}
}


/** \brief Find the specified label.
 *
 * This function searches the list of instructions for a given
 * label. If the label is found, the function returns its position
 * plus 1. Otherwise, the function returns the total number of
 * instructions (i.e. f_instructions.Count()).
 *
 * \param[in] name Name of the label to search
 *
 * \return The label position of f_instructions.Count()
 */
int moSimpleEditor::FindLabel(const moWCString& name)
{
	int		ip, max;
	moSedInstruction	*inst;

	max = f_instructions.Count();
	for(ip = 0; ip < max; ++ip) {
		inst = f_instructions.Get(ip);
		if(inst->f_action == moSedInstruction::ACTION_LABEL && inst->f_text == name) {
			// no need to just on the label itself, so add 1
			return ip + 1;
		}
	}

	return max;
}


/** \brief Parse a text file
 *
 * This function is the actual simple editor. It takes a
 * text stream as input. The text stream has to include an
 * input and output stream to work properly.
 *
 * The function reads lines from the text_stream one by
 * one and apply the instructions to them. The result
 * (i.e. moSedInstruction::ACTION_PRINT) is printed in
 * the text_stream output stream.
 *
 * The function expects the text streams to be properly
 * initialized (i.e. have the proper convertors assigned
 * to them.)
 *
 * The function returns when an moSedInstruction::ACTION_QUIT
 * or equivalent is found or the input text was exhausted.
 *
 * \bug
 * This function cannot handle the input and output streams
 * to be the same or pointing to the same file.
 *
 * \param[in] text_stream The stream used to read and write text
 */
void moSimpleEditor::Parse(moTextStream& text_stream)
{
	moWCString		line, uppercase, match_uppercase, str, extra;
	moListOfWCStrings	pattern;
	moVariableList		variables("moSimpleEditor");
	moSedInstruction	*inst;
	int			ip, idx, max, line_no;
	bool			eof, match, replace, new_address_match, consumed;

	ip = 0;		// instruction pointer
	match = false;
	replace = false;
	eof = false;
	consumed = true;

	// reset address matching
	max = f_instructions.Count();
	for(ip = 0; ip < max; ++ip) {
		inst = f_instructions.Get(ip);
		if(inst->f_start_line <= 0
		|| inst->f_start_pattern.IsEmpty()) {
			inst->f_matched_line = 1;
		}
		else {
			inst->f_matched_line = 0;
		}
	}

	for(;;) {
		// automatically loop around
		if(ip >= static_cast<int>(f_instructions.Count())) {
			ip = 0;
		}
		inst = f_instructions.Get(ip);

		// if there is an address, we need a match
		line_no = text_stream.Line();
		new_address_match = inst->f_matched_line != 0;
		if(!new_address_match) {
			// check the start
			if(inst->f_start_line > -1
			&& inst->f_start_line == line_no) {
				inst->f_matched_line = line_no;
				new_address_match = true;
			}
			if(!inst->f_start_pattern.IsEmpty()
			&& inst->f_start_pattern.MatchExpression(line)) {
				inst->f_matched_line = line_no;
				new_address_match = true;
			}
		}
		if(inst->f_matched_line != 0) {
			// check the end
			if(inst->f_end_line > 0
			&& inst->f_end_line < line_no) {
				inst->f_matched_line = 0;
				new_address_match = false;
			}
			if(!inst->f_end_pattern.IsEmpty()
			&& inst->f_end_pattern.MatchExpression(line)) {
				inst->f_matched_line = 0;
			}
			// if there is no end, only this one line is a match
			// so we need to clear the f_matched_line; but
			// new_address_match remains true if it is.
			if(inst->f_end_line <= 1
			&& inst->f_end_pattern.IsEmpty()
			&& (inst->f_start_line > -1 || !inst->f_start_pattern.IsEmpty())) {
				inst->f_matched_line = 0;
			}
		}

		// compute the current flag and then tweak with step if defined
		new_address_match = new_address_match || inst->f_matched_line != 0;
		if(new_address_match
		&& inst->f_step >= 2
		&& (line_no - inst->f_matched_line) % inst->f_step != 0) {
			new_address_match = false;
		}

		// execute action if address matches
//fprintf(stderr, "%3d. %s %d...\n", ip, new_address_match ? "Execute" : "Skip", inst->f_action);
		if(new_address_match) switch(inst->f_action) {
		case moSedInstruction::ACTION_MATCH:
			match = match || inst->f_match.MatchExpression(line);
			break;

		case moSedInstruction::ACTION_NO_CASE_MATCH:
			// not supported yet, really
			match = match || inst->f_match.MatchExpression(line);
			break;

		case moSedInstruction::ACTION_REPLACE_MATCH:
			str = inst->f_match.Replace(inst->f_text, line);
			replace = replace || str != line;
			line = str;
			break;

		case moSedInstruction::ACTION_NO_CASE_REPLACE_MATCH:
			// not supported yet, really
			str = inst->f_match.Replace(inst->f_text, line);
			replace = replace || str != line;
			line = str;
			break;

		case moSedInstruction::ACTION_REPLACE_ALL_MATCHES:
			for(;;) {
				str = inst->f_match.Replace(inst->f_text, line);
				if(str == line) {
					break;
				}
				replace = true;
				line = str;
			}
			break;

		case moSedInstruction::ACTION_NO_CASE_REPLACE_ALL_MATCHES:
			// not supported yet, really
			for(;;) {
				str = inst->f_match.Replace(inst->f_text, line);
				if(str == line) {
					break;
				}
				replace = true;
				line = str;
			}
			break;

		case moSedInstruction::ACTION_TRANSPOSE:
			line = line.Transpose(inst->f_match, inst->f_text);
			break;

		case moSedInstruction::ACTION_NO_CASE_TRANSPOSE:
			uppercase = line.Uppercase();
			match_uppercase = inst->f_match.Uppercase();
			line = uppercase.Transpose(match_uppercase, inst->f_text);
			break;

		case moSedInstruction::ACTION_NEXT:
			// we already reached the end, so quit (this is certainly wrong, TBD)
			if(eof) {
				return;
			}
			// lose pattern and read next line
			pattern.Empty();
			eof = !text_stream.NextLine(line);
			// TODO: what shall we do at the end of file?
			match = false;
			replace = false;
			break;

		case moSedInstruction::ACTION_APPEND_NEXT:
			// we already reached the end, so quit (this is certainly wrong, TBD)
			if(eof) {
				return;
			}
			if(!consumed) {
				pattern += *new moWCString(line);
			}
			eof = !text_stream.NextLine(line);
			// TODO: what shall we do at the end of file?
			match = false;
			replace = false;
			consumed = false;
			break;

		case moSedInstruction::ACTION_DELETE:
			pattern.Empty();
			line.Empty();
			break;

		case moSedInstruction::ACTION_DELETE_FIRST_LINE:
			if(!pattern.IsEmpty()) {
				pattern.Delete(0);
			}
			else {
				line.Empty();
			}
			break;

		case moSedInstruction::ACTION_DELETE_LAST_LINE:
			line.Empty();
			break;

		case moSedInstruction::ACTION_APPEND:
			pattern += *new moWCString(line);
			line = inst->f_text;
			break;

		case moSedInstruction::ACTION_INSERT:
			pattern += *new moWCString(inst->f_text);
			break;

		case moSedInstruction::ACTION_REPLACE:
			line = inst->f_text;
			break;

		case moSedInstruction::ACTION_PRINT:
			max = pattern.Count();
			for(idx = 0; idx < max; ++idx) {
				text_stream.Print("%S\n", pattern.Get(idx)->Data());
			}
			text_stream.Print("%S\n", line.Data());
			pattern.Empty();
			consumed = true;
			break;

		case moSedInstruction::ACTION_PRINT_FIRST_LINE:
			if(!pattern.IsEmpty()) {
				text_stream.Print("%S\n", pattern.Get(0)->Data());
				pattern.Delete(0);
			}
			else {
				text_stream.Print("%S\n", line.Data());
				consumed = true;
			}
			break;

		case moSedInstruction::ACTION_PRINT_LAST_LINE:
			text_stream.Print("%S\n", line.Data());
			consumed = true;
			break;

		case moSedInstruction::ACTION_READ_FROM_FILE:
			if(inst->f_file.Open(inst->f_text)) {
				mowc::moIConv conv(f_input_convertor);
				conv.Reset();
				moIStreamScopeFilter input_convertor(inst->f_file, &conv);
				moTextStream input(&inst->f_file, 0, 0);
				while(input.NextLine(str)) {
					text_stream.Print("%S\n", str.Data());
				}
				// avoid leaking resources
				inst->f_file.Close();
			}
			break;

		case moSedInstruction::ACTION_READ_ONE_LINE_FROM_FILE:
			if(!inst->f_file.IsOpen()) {
				inst->f_file.Open(inst->f_text);
			}
			if(inst->f_file.IsOpen()) {
				// we create a text stream so we can read one line of
				// text... imagine that! maybe we should have that in
				// the instruction?
				mowc::moIConv conv(f_input_convertor);
				conv.Reset();
				moIStreamScopeFilter input_convertor(inst->f_file, &conv);
				moTextStream input(&inst->f_file, 0, 0);
				if(input.NextLine(str)) {
					text_stream.Print("%S\n", str.Data());
				}
			}
			break;

		case moSedInstruction::ACTION_WRITE_TO_FILE:
			if(!inst->f_file.IsOpen()) {
				inst->f_file.Open(inst->f_text, moFile::MO_FILE_MODE_WRITE | moFile::MO_FILE_MODE_CREATE);
			}
			if(inst->f_file.IsOpen()) {
				mowc::moIConv conv(f_output_convertor);
				conv.Reset();
				moIStreamScopeFilter output_convertor(inst->f_file, &conv);
				moTextStream output(0, &inst->f_file, 0);
				max = pattern.Count();
				for(idx = 0; idx < max; ++idx) {
					output.Print("%S\n", pattern.Get(idx)->Data());
				}
				output.Print("%S\n", line.Data());
				pattern.Empty();
				consumed = true;
			}
			break;

		case moSedInstruction::ACTION_WRITE_FIRST_LINE_TO_FILE:
			if(!inst->f_file.IsOpen()) {
				inst->f_file.Open(inst->f_text, moFile::MO_FILE_MODE_WRITE | moFile::MO_FILE_MODE_CREATE);
			}
			if(inst->f_file.IsOpen()) {
				mowc::moIConv conv(f_output_convertor);
				conv.Reset();
				moIStreamScopeFilter output_convertor(inst->f_file, &conv);
				moTextStream output(0, &inst->f_file, 0);
				if(!pattern.IsEmpty()) {
					output.Print("%S\n", pattern.Get(0)->Data());
					pattern.Delete(0);
				}
				else {
					output.Print("%S\n", line.Data());
				}
			}
			break;

		case moSedInstruction::ACTION_WRITE_LAST_LINE_TO_FILE:
			if(!inst->f_file.IsOpen()) {
				inst->f_file.Open(inst->f_text, moFile::MO_FILE_MODE_WRITE | moFile::MO_FILE_MODE_CREATE);
			}
			if(inst->f_file.IsOpen()) {
				mowc::moIConv conv(f_output_convertor);
				conv.Reset();
				moIStreamScopeFilter output_convertor(inst->f_file, &conv);
				moTextStream output(0, &inst->f_file, 0);
				output.Print("%S\n", line.Data());
			}
			break;

		case moSedInstruction::ACTION_SET_VARIABLE:
			// TODO: we need to save the whole pattern
			str = ListToString(pattern, line);
			variables.Set(inst->f_text, str);
			break;

		case moSedInstruction::ACTION_APPEND_TO_VARIABLE:
			str = ListToString(pattern, line);
			str = variables.Get(inst->f_text) + "\n" + str;
			variables.Set(inst->f_text, str);
			break;

		case moSedInstruction::ACTION_GET_VARIABLE:
			pattern.Empty();
			str = variables.Get(inst->f_text);
			StringToList(str, pattern, line);
			break;

		case moSedInstruction::ACTION_APPEND_FROM_VARIABLE:
			pattern += *new moWCString(line);
			str = variables.Get(inst->f_text);
			StringToList(str, pattern, line);
			break;

		case moSedInstruction::ACTION_EXCHANGE:
			extra = variables.Get(inst->f_text);
			str = ListToString(pattern, line);
			variables.Set(inst->f_text, str);
			pattern.Empty();
			StringToList(str, pattern, line);
			break;

		case moSedInstruction::ACTION_QUIT:
			// TODO: really just return?
			return;

		case moSedInstruction::ACTION_BRANCH:
			ip = FindLabel(inst->f_text) - 1;
			break;

		case moSedInstruction::ACTION_BRANCH_IF_MATCHED:
			if(match) {
				match = false;
				ip = FindLabel(inst->f_text) - 1;
			}
			break;

		case moSedInstruction::ACTION_BRANCH_IF_NOT_MATCHED:
			if(!match) {
				ip = FindLabel(inst->f_text) - 1;
			}
			else {
				match = false;
			}
			break;

		case moSedInstruction::ACTION_BRANCH_IF_REPLACED:
			if(replace) {
				replace = false;
				ip = FindLabel(inst->f_text) - 1;
			}
			break;

		case moSedInstruction::ACTION_BRANCH_IF_NOT_REPLACED:
			if(!replace) {
				ip = FindLabel(inst->f_text) - 1;
			}
			else {
				replace = false;
			}
			break;

		case moSedInstruction::ACTION_LABEL:
			// here this is a do nothing
			break;

		case moSedInstruction::ACTION_CALLBACK:
			// user does something...
			if(inst->Callback(*this)) {
				return;
			}
			break;

		case moSedInstruction::ACTION_max:
			throw moError(MO_ERROR_INTERNAL, "An moSedInstruction is set to moSedInstruction::ACTION_max");

		}
		++ip;
	}
}


/** \brief Parse the input to the output.
 *
 * This function initializes the input and output streams
 * with the correct convertors as defined internally.
 * Then it calls the Parse(moTextStream& text_stream)
 * function.
 *
 * The input and output streams are expected to be text
 * files.
 *
 * \bug
 * This function cannot handle the input and output streams
 * to be the same or pointing to the same file.
 *
 * \param[in] input The input stream
 * \param[in] output The output stream
 */
void moSimpleEditor::Parse(moIStream& input, moOStream& output)
{
	moIStreamScopeFilter input_convertor(input, &f_input_convertor);
	moOStreamScopeFilter output_convertor(output, &f_output_convertor);

	moTextStream text_stream(&input, &output, 0);

	Parse(text_stream);
}


/** \brief Parse the input to the output.
 *
 * This function is a helper function that will create
 * the necessary input and output streams for you.
 *
 * Also, this function properly handles the case where
 * the input and output filenames are the same (i.e.
 * exactly equal). Thus, if input_filename == output_filename
 * works.
 *
 * \bug
 * Though the input and output filenames can be one and the
 * same, it makes use of an moMemFile. This means you need to
 * have enough memory to memorize the converted files before
 * saving it back in the input file.
 *
 * \param[in] input_filename The input filename
 * \param[in] output_filename The output filename
 */
void moSimpleEditor::Parse(const moWCString& input_filename, const moWCString& output_filename)
{
	bool		same_file;

	// the input file is create the same way whether the output is the same or not
	moFile input(input_filename);

#if WIN32
	same_file = input_filename.CaseCompare(output_filename) == MO_BASE_COMPARE_EQUAL;
#else
	same_file = input_filename.Compare(output_filename) == MO_BASE_COMPARE_EQUAL;
#endif
	if(same_file) {
		// here we've got a problem!
		// both files are the same, so we bufferize the result in an moMemFile
		// then we overwrite the input file with that result.
		moMemFile mem;

		// okay, convert to memory
		Parse(input, mem);

		// now copy the result in the output file (that happens to the the same as the input)
		input.Open(output_filename, moFile::MO_FILE_MODE_WRITE | moFile::MO_FILE_MODE_CREATE);
		char buf[1024];
		for(;;) {
			int sz = mem.Read(buf, sizeof(buf));
			if(sz <= 0) {
				break;
			}
			input.Write(buf, sz);
		}
	}
	else {
		moFile output(output_filename, moFile::MO_FILE_MODE_WRITE | moFile::MO_FILE_MODE_CREATE);

		// convert directly from file A to file B
		Parse(input, output);
	}
}


};			// namespace molib

// vim: ts=8

