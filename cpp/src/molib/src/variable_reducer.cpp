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
#pragma implementation "mo/mo_variable_reducer.h"
#endif

#include	"mo/mo_variable_reducer.h"

#ifndef MO_EXPR_H
#include	"mo/mo_expr.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{



/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Basename

NAME

	basename - removes the path and optionally the extension

SYNOPSIS

	$(basename <param>)

DESCRIPTION

	This system instruction returns the basename part of a
	filename.
	
	This is the filename including the extension but
	without any path information.

	Note that a 2nd parameter can be specified. If it matches
	the extension, then the extension is removed from the
	resulting string.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Basename : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str, ext;

						if(parameters.Count() == 2) {
							ext = *parameters.Get(1);
						}
						str = parameters.Get(0);
						result += str->FilenameBasename(ext);

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "basename";
					}

	virtual	unsigned long		MaximumParameterCount(void) const
					{
						return 2;
					}

} mo_variable_reducer_command_basename;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Capitalize

NAME

	capitalize - transpose all lowercase characters to uppercase

SYNOPSIS

	$(capitalize <string>,<true|false>)

PARAMETERS

	<string> - the string to be capitalized
	<true|false> - whether each word should be capitalized

DESCRIPTION

	This system instruction transform the string so the very first
	letter is an uppercase letter or the first letter of each word
	is an uppercase letter.

	The second parameter can either be true, false, 0 or 1.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Capitalize : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str, words;

						if(parameters.Count() == 2) {
							words = *parameters.Get(1);
						}
						str = parameters.Get(0);
						if(words == "1" || words == "true") {
							result += str->CapitalizeWords();
						}
						else {
							result += str->Capitalize();
						}

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "capitalize";
					}

	virtual	unsigned long		MaximumParameterCount(void) const
					{
						return 2;
					}

} mo_variable_reducer_command_capitalize;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Dirname

NAME

	dirname - returns the path in a filename

SYNOPSIS

	$(dirname <param>)

DESCRIPTION

	This system instruction returns the path defined in a filename.

	Note that all [back]slashes at the end of a filename are removed
	before the command is applied. This the command dirname applied
	to the path a/b/c/ returns /a/b, not /a/b/c.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Dirname : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str;

						str = parameters.Get(0);
						result += str->FilenameDirname();

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "dirname";
					}

} mo_variable_reducer_command_dirname;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Echo

NAME

	echo - print out the parameters

SYNOPSIS

	$(echo <param1>,<param2>,...)

DESCRIPTION

	This system instruction uses printf(3C) to print all of its parameters
	in the standard output.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Echo : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						unsigned long	idx, count;
						moWCString	*msg;

						count = parameters.Count();
						for(idx = 0; idx < count; idx++) {
							msg = parameters.Get(idx);
							printf("%s\n", msg->SavedMBData());
						}

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "echo";
					}

	virtual	unsigned long		MaximumParameterCount(void) const
					{
						return ULONG_MAX;
					}

} mo_variable_reducer_command_echo;






/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Expr

NAME

	expr - compute the following expression

SYNOPSIS

	$(expr <param>)

DESCRIPTION

	This system instruction returns result of the C like expression
	defined in <param>. Please see the moExpr for more information
	of all the supported expressions and types.

SEE ALSO

	moVariableReducerCommand, moExpr

*/
static const class moVariabeReducerCommand_Expr : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moExpr expr = *parameters.Get(0);
						result += expr.Result();
						// TODO: error handling...
						// if(expr.Error() != moExpr::MO_EXPR_ERROR_NONE) {
						//	...
						// }

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "expr";
					}

} mo_variable_reducer_command_expr;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Extension

NAME

	extension - returns the filename extension

SYNOPSIS

	$(extension <param>)

DESCRIPTION

	This system instruction returns the extension part of a
	filename.

	Note that the point is included in the result.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Extension : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str;

						str = parameters.Get(0);
						result += str->FilenameExtension();

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "extension";
					}

} mo_variable_reducer_command_extension;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Lowercase

NAME

	lowercase - transpose all uppercase characters to lowercase

SYNOPSIS

	$(lowercase <param>)

DESCRIPTION

	This system instruction ensures that the parameter string is
	only lowercase characters.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Lowercase : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str;

						str = parameters.Get(0);
						result += str->Lowercase();

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "lowercase";
					}

} mo_variable_reducer_command_lowercase;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Reverse

NAME

	reverse - reverse the string (last character first)

SYNOPSIS

	$(reverse <param>)

DESCRIPTION

	This system instruction returns the input string from the last
	character to the first.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Reverse : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str;

						str = parameters.Get(0);
						result += str->Reverse();

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "reverse";
					}

} mo_variable_reducer_command_reverse;






/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Shell

NAME

	shell - execute a shell command

SYNOPSIS

	$(shell <command params>)

DESCRIPTION

	This is a system implementation of a moVariableReducerCommand
	used to execute shell commands.

	The standard output of the shell commands will be returned as
	the result of the command.

	The command takes exactly one parameter: the shell command
	and its parameters. However, the command and parameter can
	both include commas. These will be used in the command line.

SEE ALSO

	moVariableReducerCommand

TODO

	This function is not yet implemented under VC++. We need to
	have a working moPipe object first.

*/
#ifndef _MSC_VER
static const class moVariabeReducerCommand_Shell : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	shell_command;
						const char	*cmd;
						char		buf[4096 + 1];	/* Flawfinder: ignore */
						int		r, idx, max;
						FILE		*p;

						max = parameters.Count();
						for(idx = 0; idx < max; ++idx) {
							if(idx > 0) {
								shell_command += ",";
							}
							shell_command += *parameters.Get(idx);
						}
						cmd = shell_command.SavedMBData();
						// DO NOT PUT "rb" in a popen(); only "r" and "w"
						// are valid under Unix!
						p = popen(cmd, "r");			/* Flawfinder: ignore */
						if(p != NULL) {
							errno = 0;
							// fgets() reads at most sizeof(buf) - 1
							while(fgets(buf, sizeof(buf), p)) {
								result += buf;
								errno = 0;
							}
							r = errno;
							pclose(p);
						}
						else {
							r = errno;
						}

						return r;
					}

	unsigned long			MaximumParameterCount(void) const
					{
						return ULONG_MAX;
					}

	virtual const moWCString	Name(void) const
					{
						return "shell";
					}

} mo_variable_reducer_command_shell;
#endif





/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Switchcase

NAME

	switchcase - transpose all lowercase characters to uppercase and vice versa

SYNOPSIS

	$(switchcase <param>)

DESCRIPTION

	This system instruction returns the parameter string with all
	of its lowercase transposed to uppercase and all of its uppercase
	transposed to lowercase.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Switchcase : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str;

						str = parameters.Get(0);
						result += str->Switchcase();

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "switchcase";
					}

} mo_variable_reducer_command_switchcase;







/************************************************************ DOC:

CLASS

	moVariableReducerCommand_Uppercase

NAME

	uppercase - transpose all lowercase characters to uppercase

SYNOPSIS

	$(uppercase <param>)

DESCRIPTION

	This system instruction ensures that the parameter string is
	only uppercase characters.

SEE ALSO

	moVariableReducerCommand

*/
static const class moVariabeReducerCommand_Uppercase : public moVariableReducer::moVariableReducerCommand
{
public:
	virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result)
					{
						moWCString	*str;

						str = parameters.Get(0);
						result += str->Uppercase();

						return 0;
					}

	virtual const moWCString	Name(void) const
					{
						return "uppercase";
					}

} mo_variable_reducer_command_uppercase;







/************************************************************ DOC:

CLASS

	moVariableReducer

NAME

	Constructor - initialize an moVariableReducer object
	Destructor - virtual destructor to clean up this object

SYNOPSIS

	moVariableReducer(void);
	virtual ~moVariableReducer();

DESCRIPTION

	An moVariableReducer object can be used to reduce all variable
	references to the actual content of these variables.

	The parser is very basic in its primary functioning. It will
	read a text stream using its GetC() function. A variable is
	introduced by the sequence '$(' (dollar and open parenthesis).
	A variable ends when the corresponding closing parenthesis is
	found. Within a variable you can declare other variables.

	The assumption is that the input file can be of any time and
	thus only the '$(' will make an instance of this object react.
	It still is necessary to be able to skip such a sequence in
	case the file presents examples which require such sequences
	to appear as is. This is done with a backslash: '\$(' will
	be returned as '$(' and no variable parsing is performed.

	Note that a stand alone $ or ( are taken as is also.

	The reduction function is recursive and it can parse a variable
	within another variable with a maximum of MO_VR_MAXDEPTH.

SEE ALSO

	moVariable, moVariableList, moXML, Reduce

*/
moVariableReducer::moVariableReducer(void)
{
	RegisterCommand(mo_variable_reducer_command_basename);
	RegisterCommand(mo_variable_reducer_command_capitalize);
	RegisterCommand(mo_variable_reducer_command_dirname);
	RegisterCommand(mo_variable_reducer_command_echo);
	RegisterCommand(mo_variable_reducer_command_expr);
	RegisterCommand(mo_variable_reducer_command_extension);
	RegisterCommand(mo_variable_reducer_command_lowercase);
	RegisterCommand(mo_variable_reducer_command_reverse);
#ifndef _MSC_VER
	RegisterCommand(mo_variable_reducer_command_shell);
#endif
	RegisterCommand(mo_variable_reducer_command_switchcase);
	RegisterCommand(mo_variable_reducer_command_uppercase);
}


moVariableReducer::~moVariableReducer()
{
}





/************************************************************ DOC:

CLASS

	moVariableReducer

NAME

	Reduce - the function to call to start reducing an input stream
	Reducing - the actual command which parse the input

SYNOPSIS

	virtual int Reduce(moWCString& result);

	private:
	int Reducing(moWCString& result);

DESCRIPTION

	The Reduce() function will be called to reduce the input so no
	variables appear in the input afterward.

	The Reducing() function will be used internally. It manages the
	current depth properly.

BUGS

	At this time there is no way to know whether an error occured
	or the end of the file was found.

	This function is NOT re-entrant within the same object. Watch
	out in case you wanted to have some sort of recursivity.

RETURN VALUE

	If an error occurs, -1 is returned, otherwise 0.

SEE ALSO

	Name

*/
int moVariableReducer::Reduce(moWCString& result)
{
	mowc::wc_t	c;

	f_depth = 0;

	for(;;) {
		c = GetC(f_depth);
		if(c == MO_VR_EOF) {
			return result.IsEmpty() ? -1 : 0;
		}
		if(c == '\\') {
			// check for '\$(' to reduce to '$('
			c = GetC(f_depth);
			if(c == '$') {
				c = GetC(f_depth);
				if(c != '(') {
					result += '\\';
				}
				result += '$';
			}
			else {
				result += '\\';
			}
			result += c;
		}
		else if(c == '$') {
			c = GetC(f_depth);
			// we need to support something like $$$(name) as $$ and variable $(name)
			while(c == '$') {
				result += '$';
				c = GetC(f_depth);
			}
			if(c == '(') {
				// found a variable introducer
				Reducing(result);
			}
			else {
				result += '$';
				result += c;
			}
		}
		else {
			result += c;
		}
	}
}


void moVariableReducer::Reducing(moWCString& result)
{
	mowc::wc_t		c;
	moWCString		name, word, sub_word;
	moListOfWCStrings	params;
	int			count;

	c = SkipSpaces();
	if(c == MO_VR_EOF) {
		return;
	}

	f_depth++;

// first get the variable name
	c = ReadWord(c, name, false);

// skip any space after that name
	if(c == ',' || mowc::isspace(c)) {
		c = SkipSpaces();
	}

// if we don't have a ')' just after, then the name is a command
	if(c != ')' && c != MO_VR_EOF) {
		do {
			// WARNING: we can't do this after the ReadWord() below otherwise
			//	    we could miss the very last parameter! (if that one is
			//	    empty)
			if(c == ',') {		// skip parameter separator
				c = GetC(f_depth);
			}
			if(c != '(') {
				c = ReadWord(c, word, true);
			}
			while(c == '(') {
				count = 1;
				do {
					c = ReadWord(c, sub_word, true);
					word += sub_word;
					if(c == '(') {
						++count;
					}
					else if(c == ')') {
						--count;
					}
				} while(count > 0 && c != MO_VR_EOF);
				if(c != MO_VR_EOF) {
					c = ReadWord(c, sub_word, true);
					word += sub_word;
				}
			}
			params += *new moWCString(word);
		} while(c != ')' && c != MO_VR_EOF);

		// now that we have all the parameters we can execute the command
		/* int r = */ Command(name, params, result);

		// we're done, get rid of the parameters
		params.Empty();
	}
	else {
		result += GetVariable(name);
	}

	f_depth--;
}


mowc::wc_t moVariableReducer::SkipSpaces(void)
{
	mowc::wc_t	c;

	do {
		c = GetC(f_depth);
	} while(mowc::isspace(c) || c == ',');

	return c;
}


mowc::wc_t moVariableReducer::ReadWord(mowc::wc_t c, moWCString& word, bool accept_spaces)
{
	moWCString	sub_result;

	word.Empty();

	// note that the syntax is really not restrictive at
	// this time; it may change later so only alphanumerical
	// characters are taken as variable names.
	do {
		// TODO: should we also have \$( support here?
		while(c == '$') {			// check for $(<name>)
			c = GetC(f_depth);

			while(c == '$') {
				word += '$';
				c = GetC(f_depth);
			}
			if(c == '(') {
				sub_result.Empty();
				Reducing(sub_result);
				word += sub_result;
				goto next;
			}
			else {
				word += '$';
			}
		}
		if(c == '\\') {
			c = GetC(f_depth);
			if(c != MO_VR_EOF) {
				word += c;
			}
		}
		else {
			word += c;
		}
next:
		c = GetC(f_depth);
	} while((accept_spaces || !mowc::isspace(c)) && c != '(' && c != ')' && c != ',' && c != MO_VR_EOF);

	return c;
}




/************************************************************ DOC:

CLASS

	moVariableReducer

NAME

	GetC - get the next character of input

SYNOPSIS

	virtual mowc::wc_t GetC(unsigned long depth) = 0;

DESCRIPTION

	The GetC() function is called by the variable reducer to
	get the character from the input.

	The input can be defined as a file, a string, etc. this
	is user depend.

	The function must be overridden as this is a pure virtual
	function (abstract).

RETURN VALUE

	The function is exepected to return either a Unicode
	character or the special value moVariableReducer::MO_VR_EOF
	when the input is empty.

SEE ALSO

	Name

*/



/************************************************************ DOC:

CLASS

	moVariableReducer

NAME

	Undefined - called whenever a request is made for an undefined variable or command

SYNOPSIS

	virtual void Undefined(const moWCString& name, bool command = false);

PARAMETERS

	name - the name of the undefined variable or command
	command - whether a command (true) or a variable (false) is undefined

DESCRIPTION

	The default (and certainly your) GetVariable() function will
	call this Undefined() function whenever a request for a variable
	can't be satisfied.

	Similarly, the Command() function calls this Undefined() function
	whenever a command can't be found in the list of registered commands.
	You know whether the function is being called for a variable or
	a command by testing the command parameter.

	The result is implementation specific. The default function currently
	does nothing. Your implementation can print out a warning or an
	error message.

SEE ALSO

	GetVariable, Command

*/
void moVariableReducer::Undefined(const moWCString& name, bool command)
{
}



/************************************************************ DOC:

CLASS

	moVariableReducer

NAME

	GetVariable - get the value of a named variable

SYNOPSIS

	virtual moWCString moVariableReducer::GetVariable(const moWCString& name);

DESCRIPTION

	The default GetVariable() function searches for an environment variable and
	return its value. If the variable isn't defined, then the Undefined() function
	is called and an empty string is returned.

	You can override this function to acces your own variables and call this
	one in case none of your variable has the specified name.

RETURN VALUE

	The content of the named variable or the empty string

SEE ALSO

	Undefined

*/
moWCString moVariableReducer::GetVariable(const moWCString& name)
{
	char		*result;

	result = getenv(name.SavedMBData());	/* Flawfinder: ignore */

	if(result == 0) {
		// in case someone wanted to warn about undefined variables
		Undefined(name);
	}

	return result;
}




/************************************************************ DOC:

CLASS

	moVariableReducer

NAME

	RegisterCommand - add a command to the internal list of commands
	UnregisterCommand - remove a command previous registered
	Command - execute the named command with the specified parameters

SYNOPSIS

	bool RegisterCommand(const moVariableReducerCommand& command);
	bool UnregisterCommand(const moVariableReducerCommand& command);
	int Command(const moWCString& command, const moList& parameters, moWCString& result);

DESCRIPTION

	The RegisterCommand() function is used internally to add all the
	default commands (such as the $(shell ...) command).

	The user is welcome to implement his/her own commands. Two commands
	can't have the same name. For this reason, this function returns
	a boolean value which indicates whether the function being registered
	is actually being refused.

	The UnregisterCommand() function remove a command which was previously
	registered with the RegisterCommand() function.

	The Command() function is then used to call one registered command.
	The parameters is a list of strings used by the command. The result
	parameter should be empty on entry, thought, if not, it will represent
	the default result (i.e. it will be unchanged if anything goes wrong
	running the command -- WARNING: some commands may clear the result).

BUGS

	Even thought the result should be modified by the command only if
	a result is properly defined, it is possible that some functions
	will change the result variable even thought they will fail.

RETURN VALUE

	RegisterCommand():

	If the registred command is registered then true
	If the registred command is rejected (a command of that name already exists) then false

	Command():

	0 when nothing goes wrong, the errno otherwise (Warning: the current errno may
	be different than the returned errno value)

SEE ALSO

	?

*/
bool moVariableReducer::RegisterCommand(const moVariableReducerCommand& command)
{
	return f_commands.Insert(&command);
}


bool moVariableReducer::UnregisterCommand(const moVariableReducerCommand& command)
{
	moList::position_t	pos;

	pos = f_commands.Find(&command);
	if(pos != moList::NO_POSITION) {
		// found it, get rid of it.
		f_commands.Delete(pos);
		return true;
	}

	return false;
}




// since the moVariableReducerCommand has several abstract functions
// we need to define a class which defines these functions in order
// to have an object we can use to search for commands
class moVariableReducerCommand_FindCommand : public moVariableReducer::moVariableReducerCommand
{
public:
	moVariableReducerCommand_FindCommand(const moWCString& name)
		: f_name(name)
	{
	}

	virtual int Execute(const moListOfWCStrings& parameters, moWCString& result)
	{
		return 0;
	}

	virtual const moWCString Name(void) const
	{
		return f_name;
	}

private:
	const moWCString	f_name;
};


int moVariableReducer::Command(const moWCString& command, const moListOfWCStrings& parameters, moWCString& result)
{
	unsigned long				count;
	moWCString				*p;
	moList::position_t			pos;
	moVariableReducerCommand		*execute;
	moVariableReducerCommand::mo_vr_param_t	type;

// search for the command
	moVariableReducerCommand_FindCommand cmd(command);
	pos = f_commands.Find(&cmd);
	if(pos == moList::NO_POSITION) {
		// if we can't find a command we can't execute anything
		Undefined(command, true);
		return ENOSYS;
	}
	execute = f_commands.Get(pos);

// verify the number of parameters
	count = parameters.Count();
	if(count < execute->MinimumParameterCount()
	|| count > execute->MaximumParameterCount()) {
#ifdef WIN32
		// under windows we can't distinguish this error from the ones checked out below...
		return EINVAL;
#else
		return EOVERFLOW;
#endif
	}

// verify the type of each parameter
	while(count > 0) {
		count--;
		type = execute->ParameterType(count);
		if(type == moVariableReducerCommand::MO_VR_PARAM_ERROR) {
#ifdef WIN32
			return EINVAL;
#else
			return EOVERFLOW;
#endif
		}
		if(type != moVariableReducerCommand::MO_VR_PARAM_ANY) {
			// we need to test the value...
			p = parameters.Get(count);
			if(p->IsInteger()) {
				if(type != moVariableReducerCommand::MO_VR_PARAM_NUMBER && type != moVariableReducerCommand::MO_VR_PARAM_INTEGER) {
					return EINVAL;
				}
			}
			else if(p->IsFloat()) {
				if(type != moVariableReducerCommand::MO_VR_PARAM_NUMBER && type != moVariableReducerCommand::MO_VR_PARAM_FLOAT) {
					return EINVAL;
				}
			}
			else {
				if(type != moVariableReducerCommand::MO_VR_PARAM_STRING) {
					return EINVAL;
				}
			}
		}
	}

// execute the command now
	return execute->Execute(parameters, result);
}




/************************************************************ DOC:

CLASS

	moVariableReducerCommand

NAME

	Execute - called whenever the command has to be executed
	Name - returns the name of the command
	MinimumParameterCount - the minimum number of parameters necessary
	MaximumParameterCount - the maximum number of parameters
	ParameterType - a function used to ensure the correct type of each parameter

SYNOPSIS

	virtual int Execute(const moListOfWCStrings& parameters, moWCString& result) = 0;
	virtual const moWCString Name(void) const = 0;
	virtual	unsigned long MinimumParameterCount(void) const;
	virtual	unsigned long MaximumParameterCount(void) const;
	virtual mo_vr_param_t ParameterType(unsigned long index) const;

	private:
	virtual compare_t Compare(const moBase& object) const;

PARAMETERS

	parameters - a list of strings representing the paramaters
	result -- the result to grow (i.e. do result += <execution result>)
	index -- the parameter type at that position
	object -- another command which name will be compared

DESCRIPTION

	The moVariableReducerCommand class is defined in order to allow users of
	the moVariableReducer to define additional commands to the system.

	The system defines a certain number of internal functions which don't
	require other objects to work. The following is a small list of these
	internal commands:

		$(echo ...)
		$(first ...)
		$(last ...)
		$(shell ...)
		$(word ...)
		$(count ...)

	Each command needs to overload the Execute() and Name() pure virtual
	functions (abstract functions). The Execute() function is called
	whenever the command needs to be executed. It receives a list with
	all the parameters as expected by the command and a string to be
	filled with the result. The list of parameters will have been tested
	against the minimum, maximum and type as defined below. Thus, the
	Execute() function doesn't need to test all the parameters again
	(unless there is dynamism among the type of parameters).

	The MinimumParameterCount() and MaximumParameterCount() functions
	will return the minimum number of parameters and the maximum number
	of parameters that the command supports. Most commands will accept
	only a specific number of parameters in which case both functions
	return that same number. Functions which accept a variable number
	of parameters can return a minimum and a maximum which are different.
	The maximum always needs to be larger than the minimum.

	The default count for both of these functions is 1 (the minimum
	number of parameters is at least 1 since to be detected as a
	function, a variable name needs to be followed by at least one
	parameter).

	If the number of parameters is not limited, you don't need to
	overload the MinimumParameterCount() function, and you can return
	ULONG_MAX in your MaximumParameterCount() function.

	The ParameterType() function accepts the parameter number (from
	0 to MaximumParameterCount() - 1 or the actual number of parameters
	when less than the maximum is defined). The function needs to return
	one of the following type value:

		enum mo_vr_param_t {
			MO_VR_PARAM_ERROR   = -1,	// index out of bounds or some other error
			MO_VR_PARAM_ANY     = 0,	// anything is fine
			MO_VR_PARAM_INTEGER = 1,	// an integer (vs a float)
			MO_VR_PARAM_FLOAT   = 2,	// a floating point value
			MO_VR_PARAM_NUMBER  = 3,	// either an integer or a float
			MO_VR_PARAM_STRING  = 4		// not a number
		};

	By default, the ParameterType() function returns MO_VR_PARAM_ANY
	which means the parameter can be really anything. If you specify
	MO_VR_PARAM_NUMBER, then an integer or a floating point value are
	accepted for that parameter. At this time, the MO_VR_PARAM_STRING
	is considered as not being neither an integer nor a floating point.

	If there is an error while running the ParameterType() function,
	it needs to return MO_VR_PARAM_ERROR. Usually, this happens when
	the function is called with an index out of bounds.

	The private Compare() function is used to ensure that these objects
	are sorted alphabetically in the list of commands of an
	moVariableReducer object.

BUGS

	Even thought the result should be modified by the command only if
	a result is properly defined, it is possible that some functions
	will change the result variable even thought they will fail.

RETURN VALUE

	Execute():

	Execute is expected to return 0 when no error occurs or errno
	when an error occurs.

	Name():

	Returns the name of the command (such as "echo").

	MinimumParameterCount():

	By default, this function returns 1. It is expected to return
	the minimum number of parameters required by the command so
	the Execute() function works properly.

	MaximumParameterCount():

	By default, this function returns 1. It is expected to return
	the maximum number of parameters supported by the command so
	the Execute() function works properly.

	ParameterType():

	By default, this function returns MO_VR_PARAM_ANY.
	If a specialized parameter is required, then return the type
	of data you expect with one of the mo_vr_param_t values.

	Compare():

	The comparison result. It needs to not be redefined. This is
	an internal function only.

SEE ALSO

	?

*/
unsigned long moVariableReducer::moVariableReducerCommand::MinimumParameterCount(void) const
{
	return 1;
}


unsigned long moVariableReducer::moVariableReducerCommand::MaximumParameterCount(void) const
{
	return 1;
}


moVariableReducer::moVariableReducerCommand::mo_vr_param_t moVariableReducer::moVariableReducerCommand::ParameterType(unsigned long index) const
{
	return MO_VR_PARAM_ANY;
}

moBase::compare_t moVariableReducer::moVariableReducerCommand::Compare(const moBase& object) const
{
	return Name().Compare(dynamic_cast<const moVariableReducerCommand&>(object).Name());
}


}			// namespace molib;

// vim: ts=8

