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
#pragma implementation "mo/mo_variable.h"
#endif

#include	"mo/mo_variable.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{



/************************************************************ DOC:

CLASS

	moVariable

NAME

	Constructor - create an moVariable object

SYNOPSIS

	moVariable(const moWCString& name);

PARAMETERS

	name - name of the new variable

DESCRIPTION

	A new variable needs a name before to be created. Thus
	only one constructor is available.

	Because there shouldn't be two variables with the same
	name, there is no copy operator.

	The value of the variable is itself. That is, an
	moVariable object is a string and that string represents
	the contents of the variable. Therefore, all the string
	operators and function members applied to an moVariable
	will be applied to the contents of the variable.

	Note that their is one exception: the moVariable redefines
	the Compare() function member to be able to order variables
	in a list. This means, all the comparison operators won't
	work either (doing something like: var1 < var2, will compare
	the variable names, not their contents).

SEE ALSO

	moWCString constructor, Compare, Name

BUGS

	Using the Compare() or either the ordering or equal/not equal
	operators directly on a variable when the intend is to compare
	the contents is a mistake. Casting will usually not solve the
	problem since the Compare() function is a virtual function.

*/
moVariable::moVariable(const moWCString& name)
	: f_name(name)
{
}





/************************************************************ DOC:

CLASS

	moVariable

NAME

	Compare - compare two variables and give their order

SYNOPSIS

	virtual compare_t Compare(const moBase& object) const;

DESCRIPTION

	This function compares the name of two variables and
	returns the corresponding order.

	One can use the Name() function to retrieve the name
	of a variable.

RETURN VALUE

	One of the compare value:
		MO_BASE_COMPARE_SMALLER
		MO_BASE_COMPARE_EQUAL
		MO_BASE_COMPARE_LARGER

SEE ALSO

	Name

*/
moBase::compare_t moVariable::CompareName(const moBase& object) const
{
	return f_name.Compare(dynamic_cast<const moVariable&>(object).Name());
}




/************************************************************ DOC:

CLASS

	moVariable

NAME

	Name - return the name of the variable

SYNOPSIS

	const moWCString& Name(void) const;

DESCRIPTION

	This function returns the name of this variable.

	The name is read-only and can't be modified. This
	is the name used to sort a list of variables.

RETURN VALUE

	A reference on the name of the variable

SEE ALSO

	Compare

*/
const moWCString& moVariable::Name(void) const
{
	return f_name;
}








/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Compare - compare two variable lists

SYNOPSIS

	virtual compare_t Compare(const moBase& object) const;

DESCRIPTION

	This function compares the name of two variable lists.

	This is useful if you need to have a sorted list of variable
	lists.

RETURN VALUE

	One of the compare value:
		MO_BASE_COMPARE_SMALLER
		MO_BASE_COMPARE_EQUAL
		MO_BASE_COMPARE_LARGER

SEE ALSO

	Name

*/
moBase::compare_t moVariableList::Compare(const moBase& object) const
{
	return f_name.Compare(object);
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Name - return the name of the list of variables

SYNOPSIS

	const moWCString& Name(void) const;

DESCRIPTION

	This function returns the name of this list of variables.

	The name is read-only and can't be modified. This
	is the name used to sort lists of lists of variables.

RETURN VALUE

	A reference on the name of the list of variables

SEE ALSO

	Compare

*/
const moWCString& moVariableList::Name(void) const
{
	return f_name;
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Count - return the number of variables

SYNOPSIS

	unsigned long Count(void) const;

DESCRIPTION

	This function returns the number of variables currently
	defined in the variable list.

RETURN VALUE

	a positive value or zero

SEE ALSO

	Get

*/
unsigned long moVariableList::Count(void) const
{
	return moSortedListUnique::Count();
}







/************************************************************ DOC:

CLASS

	moVariable

NAME

	operator = - set a variable value

SYNOPSIS

	moVariable& operator = (const moWCString& value);

PARAMETERS

	value - the new value of the variable

DESCRIPTION

	This operator will be used to set a new value in a variable.

ERRORS

	When invalid parameters are given to the functions, then
	an error will be thrown. Note that these function expect
	a valid variable name that is a name which doesn't include
	an equal sign and which isn't empty.

	These function throw moError() objects.

BUGS

	An Unset() variable is deleted from memory. Keeping pointers
	or references on it will result in bugs.

*/

moVariable& moVariable::operator = (const moWCString& value)
{
	(void) moWCString::operator = (value);

	return *this;
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Constructor - create an moVariableList object

SYNOPSIS

	moVariableList(const moWCString& name);
	moVariableList(const moVariableList& variable);

PARAMETERS

	name - name of the new list of variables
	variable - another list of variables

DESCRIPTION

	A new list of variables is created.

	One can set, get and unset variables using the function
	members of this object.

	Though this object is based on an moSortedListUnique,
	the functions of that object are private. It is done
	this way so the coherency of the list is ensured.

	Note that the Compare() of this object doesn't behave
	like the Compare() of the moListBase object.

SEE ALSO

	moVariable constructor, Compare, Name

*/
moVariableList::moVariableList(const moWCString& name)
	: f_name(name)
{
	SetCompare(reinterpret_cast<moBase::compare_function_t>(&moVariable::CompareName));
}


moVariableList::moVariableList(const moVariableList& variable)
	: moSortedListUniqueVariable(),
	  f_name(variable.f_name)
{
	position_t		p, max;

	SetCompare(reinterpret_cast<moBase::compare_function_t>(&moVariable::CompareName));

	max = variable.Count();
	for(p = 0; p < max; ++p) {
		//Set(*dynamic_cast<const moVariable *>(variable.Get(p)));
		Set(*variable.Get(p));
	}
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Set - set (or create) a variable in a list of variables
	Get - get a variable (its name and content)
	Unset - delete a variable

SYNOPSIS

	virtual void Append(const moVariable& variable);
	virtual void SafeAppend(const moVariable& variable);
	moVariableSPtr Set(const moVariable& variable);
	moVariableSPtr Set(const moWCString& set);
	moVariableSPtr Set(const moWCString& name, const moWCString& value);
	moVariableSPtr Get(const moWCString& name) const;
	moVariableSPtr Get(position_t position) const;
	void Unset(const moWCString& name);

	moVariableList& operator += (const moVariableList& list);
	moVariableList& operator += (const moVariable& variable);
	moVariableList& operator += (const moWCString& set);

PARAMETERS

	variable - copy this variable in this list
	set - a "<variable>=<value>" or just a "<variable name>"
	name - the name of the variable
	value - the initial value of the variable
	position - a specific position at which a variable was defined
	list - another list of variables

DESCRIPTION

	These functions will be used to create, modify, get and
	delete variables in a list.

	The Append() function reads all the variables from a list
	of variables and add them all to the destination list of
	variables. This function overwrites existing variables.
	The SafeAppend() function does the same thing except it
	won't overwrite existing variables.

	The Set() functions can be used to set a variable. If the
	variable doesn't exists, it is created. The first syntax
	is equivalent to the putenv() function, though only a name
	can be given in which case the initial value is the empty
	string. To know if the variable is already defined, use
	the Get() function instead.

	The Get() function can be used to retrieve a variable and
	its contents. The result is a variable which can also be
	seen as a string which is the contents of that variable.
	If the named variable doesn't exist, this function returns
	zero (0).

	The Unset() function will delete the named variable if it
	exists. No error is generated when the variable wasn't
	created first.

	The += operators will append a list of variables or add one
	variable like the Set() function does.

NOTES

	Append, SafeAppend and += of a list on itself is safe. It
	actual does nothing.

RETURN VALUE

	The Set() and Get() functions return a pointer to a variable
	or null (0) when an error occurs (or a variable can't be found)

	The Unset() function doesn't return anything

	The += operators return a reference to this

ERRORS

	When invalid parameters are given to the functions, then
	an error will be thrown. Note that these function expect
	a valid variable name that is a name which doesn't include
	an equal sign and which isn't empty.

	These function throw moError() objects.

BUGS

	An Unset() variable is deleted from memory. Keeping pointers
	or references on it will result in bugs.

	The Set() functions with a variable should not be used
	with a variable from the same list.

*/
moVariableSPtr moVariableList::Set(const moVariable& variable)
{
	return Set(variable.Name(), static_cast<const moWCString&>(variable));
}


moVariableSPtr moVariableList::Set(const moWCString& set)
{
	const mowc::wc_t	*s, *start;

	start = set.Data();
	s = start;
	while(*s != '\0') {
		if(*s == '=') {
			break;
		}
		s++;
	}

	if(*s == '=') {
		return Set(moWCString(start, static_cast<int>(s - start)), moWCString(s + 1));
	}
	else {
		return Set(set, moWCString(""));
	}
}


moVariableSPtr moVariableList::Set(const moWCString& name, const moWCString& value)
{
	moVariableSPtr	var;

	var = Get(name);
	if(var == 0) {
		// the variable doesn't exist yet; create one
		var = new moVariable(name);
		Insert(var);
	}

	// now set the new variable value
	*var = value;

	return var;
}


moVariableSPtr moVariableList::Get(position_t position) const
{
	return dynamic_cast<moVariable *>(moSortedListUnique::Get(position));
}


moVariableSPtr moVariableList::Get(const moWCString& name) const
{
	position_t	p;

	if(name.IsEmpty()) {
		throw moError(MO_ERROR_INVALID, "invalid variable name (empty)");
	}

	// check whether the variable is already defined
	moVariable sample(name);
	p = Find(&sample);

	return p == NO_POSITION ? 0 : moSortedListUniqueVariable::Get(p);
		//dynamic_cast<moVariable *>(moSortedListUnique::Get(p));
}





void moVariableList::Unset(const moWCString& name)
{
	position_t	p;

	if(name.IsEmpty()) {
		throw moError(MO_ERROR_INVALID, "invalid variable name (empty)");
	}

	// check whether the variable is defined
	moVariable sample(name);
	p = Find(&sample);
	if(p != NO_POSITION) {
		// it is defined, get rid of it
		Delete(p);
	}
}



void moVariableList::Append(const moVariableList& list)
{
	position_t	p;

	if(this != &list) {
		p = list.Count();
		while(p > 0UL) {
			p--;
			Set(*list.Get(p));
		}
	}
}



void moVariableList::SafeAppend(const moVariableList& list)
{
	position_t	p;

	if(this != &list) {
		p = list.Count();
		while(p > 0UL) {
			p--;
			// Set() only if we can't Get()
			if(!Get(list.Get(p)->Name())) {
				Set(*list.Get(p));
			}
		}
	}
}


#if 0
moVariableList& moVariableList::operator = (const moVariableList& value)
{
	(void) moWCString::operator = (value);

	return *this;
}
#endif



moVariableList& moVariableList::operator += (const moVariableList& list)
{
	Append(list);
	return *this;
}



moVariableList& moVariableList::operator += (const moVariable& variable)
{
	Set(variable);

	return *this;
}



moVariableList& moVariableList::operator += (const moWCString& set)
{
	Set(set);

	return *this;
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Load - load variables from a file

SYNOPSIS

	bool Load(const moWCString& filename);

PARAMETERS

	filename - the name of the file to be loaded

DESCRIPTION

	This function can be used to load a file of variables.

	The syntax is like in any makefile script:

		<variable name> = <value>

	The name of the variable can't include any spaces or hash
	characters (#). There can be spaces before and after the
	equal sign. The value can be anything (including a comment
	after a hash character).

	A long value can be continued on the following line(s) by
	ending a line with a backslash and a newline ('\\' + '\n')
	sequence.

	Any other line will silently be skipped.

NOTES

	The list of variables is NOT emptied by this function. If
	you want to reload a new list and lose what was defined
	in the original first, then you need to call the Empty()
	function first.

	If an error occurs, nothing happens. Thus, if the named
	file doesn't exists, one can't know about it.

RETURN VALUE

	This function returns true if the file could be opened
	for reading. Note that doesn't mean anything could be
	read from the file.

SEE ALSO

	LoadFromString, Empty, Save

*/
bool moVariableList::Load(const moWCString& filename)
{
	int		c;
	bool		space, more, valid;
	moWCString	input;
	FILE		*f;
	char		name[256];	/* Flawfinder: ignore */

	// TODO: this is fairly incorrect since it doesn't take any
	//	 encoding in account... we will have to redesign
	//	 that a bit later with a proper stream manager
	//	 (for Linux we need to use UTF-8)
	mowc::wcstombs(name, filename.Data(), sizeof(name));
	f = fopen(name, "rb");		/* Flawfinder: ignore */
	if(f == 0) {
		return false;
	}

	space = false;
	more = false;
	valid = false;
	c = fgetc(f);			/* Flawfinder: ignore */
	while(c != EOF) {
		if(c == '\n') {
			// new line
			if(valid) {
				// got a new variable!
				Set(input);
				valid = false;
			}
			input.Empty();
			space = false;
			more = false;
		}
		else if(valid) {
			if(!space || !isspace(c)) {
				input += static_cast<char>(c);
				space = false;
			}
		}
		else if(isspace(c)) {
			space = true;
		}
		else if(c == '=') {
			if(!more && !input.IsEmpty()) {
				// valid name
				input += '=';
				valid = true;
				space = true;	// skip spaces after the '='
			}
			more = true;
		}
		else if(space || c == '#') {
			more = true;
		}
		else {
			input += static_cast<char>(c);
		}
		c = fgetc(f);	/* Flawfinder: ignore */
	}

	fclose(f);

	return true;
}


/************************************************************ DOC:

CLASS

	moVariableList

NAME

	LoadFromString - load variables from a string

SYNOPSIS

	void LoadFromString(const moWCString& variables, moWCString::wc_t separator = '\n', mowc::wc_t append = '\0');

PARAMETERS

	variables - a list of variables
	seperator - the character checked as a separator

DESCRIPTION

	This function is similar to the Load() function in that it
	parses a string with a variable name followed by an equal
	sign and a value. Each new variable is on a new line unless
	you change the separator to another character.

	For instance, to parse a query string from an HTTP request,
	one can use '&' as the separator.

	Note that the special character '#' is viewed as a comment
	introducer. However, it is ignored (taken as part of the
	variable name or content) when the separator is not '\n'.

NOTES

	The list of variables is NOT emptied by this function. If
	you want to reload a new list and lose what was defined
	in the original first, then you need to call the Empty()
	function first.

SEE ALSO

	Load, SaveToString, Empty

*/
void moVariableList::LoadFromString(const moWCString& variables, mowc::wc_t separator, mowc::wc_t append)
{
	moWCString		input;
	moVariableSPtr		var;
	const mowc::wc_t	*s;
	mowc::wc_t		c, str[2];
	bool			valid, space, more;
	long			pos;

	s = variables.Data();
	c = *s++;
	if(c == '\0') {
		return;
	}

	str[0] = append;
	str[1] = L'\0';

	valid = false;
	space = false;
	more = false;
	for(;;) {
		if(c == separator || c == '\0') {
			// new line
			if(valid) {
				// found a variable
				if(append != '\0') {
					pos = input.FindInString("=");
					if(pos < 0) {
						var = Get(input);
					}
					else {
						var = Get(input.Get(0, pos - 1));
					}
					if(var) {
						// append (variable defined multiple times)
						*var = *var + str + input.Get(pos + 1, -1);
					}
					else {
						Set(input);
					}
				}
				else {
					Set(input);
				}
				valid = false;
			}
			if(c == '\0') {
				return;
			}
			input.Empty();
			space = false;
			more = false;
		}
		else if(valid) {
			if(!space || !isspace(c)) {
				input += c;
				space = false;
			}
		}
		else if(isspace(c)) {
			space = true;
		}
		else if(c == '=') {
			if(!more && !input.IsEmpty()) {
				// valid name
				input += '=';
				valid = true;
				space = true;	// skip spaces after the '='
			}
			more = true;
		}
		else if(space || (c == '#' && separator == '\n')) {
			more = true;
		}
		else {
			input += c;
		}
		c = *s++;
	}
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	Save - save variables to a file

SYNOPSIS

	bool Save(const moWCString& filename);

PARAMETERS

	filename - the name of the file to be loaded

DESCRIPTION

	This function can be used to save a list of variables in
	a file. It will create a file which can later be loaded
	with the Load() function.

RETURN VALUE

	The Save() function returns true if it could open the
	output file. It returns false if the output file could
	not be opened (invalid file name, permission denied...)

KNOWN BUGS

	The function doesn't currently test to know whether
	writing to the file works.

SEE ALSO

	SaveToString, Load, LoadFromString

*/
bool moVariableList::Save(const moWCString& filename)
{
	FILE			*f;
	position_t		p, max;
	moVariableSPtr		v;
	char			name[256], value[1024];	/* Flawfinder: ignore */

	// TODO: we need to use a stream instead of an fopen()!
	mowc::wcstombs(name, filename.Data(), sizeof(name));
	f = fopen(name, "wb");		/* Flawfinder: ignore */
	if(f == 0) {
		return false;
	}

	max = Count();
	p = 0;
	while(p < max) {
		//v = dynamic_cast<const moVariable *>(Get(p));
		v = Get(p);
		// TODO: we need a moOStream to do the following properly
		mowc::wcstombs(name, v->Name().Data(), sizeof(name));
		mowc::wcstombs(value, v->Replace("\n=\\\\n;\r=\\\\r;\\=\\\\\\\\").Data(), sizeof(value));
		fprintf(f, "%s=%s\n", name, value);
		p++;
	}

	fclose(f);

	return true;
}



/************************************************************ DOC:

CLASS

	moVariableList

NAME

	SaveToString - save variables to a file

SYNOPSIS

	void SaveToString(moWCString& variables, moWCString::wc_t separator = '\n');

PARAMETERS

	variables - the destination string where the variables are saved
	separator - the character used to seperate each variable

DESCRIPTION

	This function can be used to save a list of variables in
	a string. It will create a string which can later be loaded
	with the LoadFromString() function.

	Note that the input string (variables) is not emptied by
	this function. If you need to have it emptied, then call the
	Empty() function on it before to call this function. The
	enables you to put a header or to call this function
	multiple times with different lists of variables.

BUGS

	At this time this function doesn't check whether a variable contains
	the separator. If it does, then it will be saved as is and therefore
	will cause problems when being reloaded.

*/
void moVariableList::SaveToString(moWCString& variables, mowc::wc_t separator)
{
	position_t		p, max;
	moVariableSPtr		v;

	// NOTE: variables is NOT cleared!

	max = Count();
	p = 0;
	while(p < max) {
		//v = dynamic_cast<const moVariable *>(Get(p));
		v = Get(p);
		p++;
		if(separator != '\n' && p == max) {
			variables += moWCString::Format("%s=%s",
					mowc::MO_ENCODING_ISO8859_1,
						v->Name().Data(), v->Data());
		}
		else {
			variables += moWCString::Format("%s=%s%c",
					mowc::MO_ENCODING_ISO8859_1,
						v->Name().Data(), v->Data(), separator);
		}
	}
}





/************************************************************ DOC:

CLASS

	moVariableList

NAME

	IsEmpty - check whether the list is empty
	Empty - empty the entire list of variables

SYNOPSIS

	bool IsEmpty(void) const;
	virtual void Empty(void);

DESCRIPTION

	The IsEmpty() function checks whether the list of variable
	is currently empty.

	The Empty() function will delete all the variables currently
	defined in the list of variables.

	You can call this function before calling the Load()
	function in order to remove the already existing
	variables.

SEE ALSO

	Count, Load

*/
bool moVariableList::IsEmpty(void) const
{
	return moSortedListUnique::IsEmpty();
}


void moVariableList::Empty(void)
{
	moSortedListUnique::Empty();
}



};			// namespace molib;

// vim: ts=8

