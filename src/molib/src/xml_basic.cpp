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
#pragma implementation "mo/mo_xml_basic.h"
#endif

#include	"mo/mo_xml_basic.h"

#ifndef MO_FILE_H
#include	"mo/mo_file.h"
#endif
#ifndef MO_MEMFILE_H
#include	"mo/mo_memfile.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{


/************************************************************ DOC:

CLASS

	moXML_Basic

NAME

	Constructor - creates a new XML basic object
	Destructor - cleans up an XML basic object

SYNOPSIS

	moXML_Basic(void);
	virtual ~moXML_Basic();

DESCRIPTION

	The moXML_Basic object implements the basic functions to
	handle variables and other basic functions such as include
	and basic expressions.

SEE ALSO

	moXMLParser

*/
moXML_Basic::moXML_Basic(void)
	: f_elif(f_if),
	  f_elsif(f_if),
	  f_else(f_if)
{
}


moXML_Basic::~moXML_Basic()
{
}



/************************************************************ DOC:

CLASS

	moXML_Basic

NAME

	Attach - attaches basic XML functionality to the specified XML parser

SYNOPSIS

	void Attach(moXMLParser& xml);

DESCRIPTION

	To use the functions that the moXML_Basic object implements one needs
	to call this Attach() function.

BUGS

	At this time there is no way to detach an XML extension.
	If you destroy this object, you need to make sure that the
	ReadNext() from the moXMLParser won't be used anymore.

SEE ALSO

	moXMLParser

*/
void moXML_Basic::Attach(moXMLParser& xml)
{
	// the following is defined in alphabetical order
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_elif);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_else);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_elsif);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_if);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_END,   f_if);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_include);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_insert);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_loop);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_END,   f_loop);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_replace);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_set);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_unset);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_START, f_usedtd);
	xml.RegisterEventHandler(moXMLParser::XML_TYPE_TAG_END,   f_usedtd);
}





/************************************************************ DOC:

CLASS

	moXML_Basic

NAME

	AddIncludePath -- add one include path for the mo:include instruction
	AppendIncludePath -- add a list of paths for the mo:include instruction

SYNOPSIS

	void AddIncludePath(const moWCString& path);
	void AppendIncludePath(const moListBase& paths);

DESCRIPTION

	The AddIncludePath() function adds one path to the list of
	include paths to use to load files via the mo:include instruction.

	The AppendIncludePath() function adds all the paths defined in
	the specified list to the list of include paths to use to load
	files via the mo:include instruction.

	As many paths as you want can be added. Any one path is never added
	more than once in the list (it wouldn't be very useful). The order
	counts since the files are searched in order in the speciied list
	of files.

BUGS

	At this time you can't remove paths from the list.

SEE ALSO

	moXMLParser

*/
void moXML_Basic::AddIncludePath(const moWCString& path)
{
	f_include.AddIncludePath(path);
}


void moXML_Basic::AppendIncludePath(const moListBase& path)
{
	f_include.AppendIncludePath(path);
}





/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Elif

NAME

	Constructor - creates the XML basic Elif object
	Destructor - cleans up the XML basic Elif object
	OnEvent - catch the tags mo:elif

SYNOPSIS

	moXML_Basic_Elif(moXML_Basic_If& basic_if);
	virtual ~moXML_Basic_Elif();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

PARAMETERS

	basic_if - a reference to the basic_if object
	parser - the XML parser which found this tag
	type - the last object read by the XML parser

DESCRIPTION

	The moXML_Basic_Elif object implements the mo:elif function
	if a conditional block. It has to appear inside a conditional
	block and needs to be an empty tag.

	Like the mo:if it needs to include a condition.

	Please, see the moXML_Basic_If for more information.

RETURN VALUE

	The OnEvent() function will return true whenever the set tag
	is valid.

SEE ALSO

	moXML_Basic_If, moXML_Basic_ElsIf, moXML_Basic

*/
moXML_Basic::moXML_Basic_Elif::moXML_Basic_Elif(moXML_Basic_If& basic_if)
	: moXMLEvent(0, "mo:elif"),
	  f_basic_if(&basic_if)
{
}


moXML_Basic::moXML_Basic_Elif::~moXML_Basic_Elif()
{
}


bool moXML_Basic::moXML_Basic_Elif::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
	if(!tag.IsEmpty()) {
		Error(parser, tag, MO_ERROR_INVALID, "an 'mo:else' is expected to be empty");
	}

	f_basic_if->SkipFalse(parser, tag);

	return true;
}


/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Else

NAME

	Constructor - creates the XML basic Else object
	Destructor - cleans up the XML basic Else object
	OnEvent - catch the tags mo:else

SYNOPSIS

	moXML_Basic_Else(moXML_Basic_If& basic_if);
	virtual ~moXML_Basic_Else();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

PARAMETERS

	basic_if - a reference to the basic_if object
	parser - the XML parser which found this tag
	type - the last object read by the XML parser

DESCRIPTION

	The moXML_Basic_Else object implements the mo:else function
	of a conditional block. It has to appear inside a conditional
	block and needs to be an empty tag.

	The else must be used by itself.

	Please, see the moXML_Basic_If for more information.

RETURN VALUE

	The OnEvent() function will return true whenever the set tag
	is valid.

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_Else::moXML_Basic_Else(moXML_Basic_If& basic_if)
	: moXMLEvent(0, "mo:else"),
	  f_basic_if(&basic_if)
{
}


moXML_Basic::moXML_Basic_Else::~moXML_Basic_Else()
{
}


bool moXML_Basic::moXML_Basic_Else::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
	if(!tag.IsEmpty()) {
		Error(parser, tag, MO_ERROR_INVALID, "an 'mo:else' is expected to be empty");
	}

	f_basic_if->SkipFalse(parser, tag);

	return true;
}



/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Elsif

NAME

	Constructor - creates the XML basic Elsif object
	Destructor - cleans up the XML basic Elsif object
	OnEvent - catch the tags mo:elsif

SYNOPSIS

	moXML_Basic_Elsif(moXML_Basic_If& basic_if);
	virtual ~moXML_Basic_Elsif();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

PARAMETERS

	basic_if - a reference to the basic_if object
	parser - the XML parser which found this tag
	type - the last object read by the XML parser

DESCRIPTION

	The moXML_Basic_Elsif object implements the mo:elsif function
	of a conditional block. It has to appear inside a conditional
	block and needs to be an empty tag.

	Like the mo:if it needs to include a condition.

	Please, see the moXML_Basic_If for more information.

RETURN VALUE

	The OnEvent() function will return true whenever the set tag
	is valid.

SEE ALSO

	moXML_Basic_If, moXML_Basic

*/
moXML_Basic::moXML_Basic_Elsif::moXML_Basic_Elsif(moXML_Basic_If& basic_if)
	: moXMLEvent(0, "mo:elsif"),
	  f_basic_if(&basic_if)
{
}


moXML_Basic::moXML_Basic_Elsif::~moXML_Basic_Elsif()
{
}


bool moXML_Basic::moXML_Basic_Elsif::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
	if(!tag.IsEmpty()) {
		Error(parser, tag, MO_ERROR_INVALID, "an 'mo:elsif' is expected to be empty");
	}

	f_basic_if->SkipFalse(parser, tag);

	return true;
}



/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_If

NAME

	Constructor - creates the XML basic If object
	Destructor - cleans up the XML basic If object
	OnEvent - catch the tags mo:if

SYNOPSIS

	moXML_Basic_If(void);
	virtual ~moXML_Basic_If();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

	bool GetCond(moXMLParser& parser, moXMLParser::moXMLType& type, bool& result);
	bool SkipFalse(moXMLParser& parser, moXMLParser::moXMLTag& tag, bool maybe);

PARAMETERS

	parser - the XML parser calling these functions
	type - an XML object
	result - the result of the conditional expression
	maybe - whether an else, elsif or elif can still react (when true it can)

DESCRIPTION

	The moXML_Basic_If object implements the mo:if function
	used to insert some code conditionally.

	It is linked to the mo:else, mo:elsif, mo:elif and mo:endif
	which all are used to change the current state of a conditional
	block.

	The mo:if, mo:elsif and mo:elif use the following parameters:

		cond="<expression>"

		is_empty="<string>"

		is_not_empty="<string>"

	The condition parameter expects an expression which is
	either 'true', 'false', '0' or '1'. Any other value will
	make the parser generate an error.

	An mo:if can be an empty tag in which case it will use the
	data parameter and insert its content if the condition is
	true:

		data="<string>"

	When it isn't empty it must be terminated properly and it
	can include else statements as described below.

	The mo:else will invert the current status from false to
	true if it had never been true. The mo:elsif and mo:elif have
	exactly the same semantic, they are just spelled differently.
	These accept a condition and if that condition is true, it
	has the same effect as the mo:else (i.e. it changes the
	current status to true if the condition was satisified and
	the status has never been true). These three tags must be
	empty tags.

	The </mo:if> tag terminates an if block.

	mo:if blocks can be nested as many times as necessary.

RETURN VALUE

	The OnEvent() function will return true whenever the set tag
	is valid.

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_If::moXML_Basic_If(void)
	: moXMLEvent(0, "mo:if")
{
	f_state_count = 0;
}


moXML_Basic::moXML_Basic_If::~moXML_Basic_If()
{
}


bool moXML_Basic::moXML_Basic_If::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::xml_type_t	t;
	moVariableSPtr		var;
	bool			more, result;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		more = false;

		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		if(GetCond(parser, type, result)) {
			if(tag.IsEmpty()) {
				var = tag.Get("data");
				if(!var) {
					Error(parser, tag, MO_ERROR_INVALID, "an empty 'mo:if' was used without a 'data'");
				}
				else if(result && !var->IsEmpty()) {
					// NOTE: we avoid inserting empty data! that would
					//	 take a lot of time for nothing.
					moMemFileSPtr input = new moMemFile;
					input->Put((mowc::wc_t) 0xFEFF);
					input->Write(var->Data(), var->Length() * sizeof(mowc::wc_t));
					moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
					parser.Include(*stream);
				}
				var = 0;
			}
			else {
				if(result) {
					more = true;
				}
				else {
					// skip anything up to an else or endif
					more = SkipFalse(parser, tag, true);
				}
			}
		}

		// we don't want to change the stack if the tag's empty or we reached the end of the block already
		if(!tag.IsEmpty() && more) {
			f_state_count++;
		}

		return true;
	}
	if(t == moXMLParser::XML_TYPE_TAG_END) {
		if(f_state_count == 0) {
			parser.FormatError(MO_ERROR_INVALID, "a '</mo:if>' found without a corresponding '<mo:if>'; <mo:if> was empty?");
		}
		else {
			f_state_count--;
		}
		return true;
	}
	// if not a TAG_START or TAG_END we do nothing

	return false;
}



bool moXML_Basic::moXML_Basic_If::GetCond(moXMLParser& parser, moXMLParser::moXMLType& type, bool& result)
{
	bool		more;
	moVariableSPtr	var;

	result = false;

	moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);

	var = tag.Get("cond");
	more = true;
	if(var) {
		if(var->IsEmpty()) {
			Error(parser, tag, MO_ERROR_INVALID, "invalid 'cond' parameter for 'mo:if', 'mo:elif' or 'mo:elsif'; it is empty");
		}
		else {
			more = false;
			switch(var->IsTrue(moWCString::WC_STRING_BOOLEAN_TEST_FLOAT | moWCString::WC_STRING_BOOLEAN_TEST_INTEGER_C | moWCString::WC_STRING_BOOLEAN_TEST_NAMED)) {
			case 1:
				result = true;
				break;

			case 0:
				result = false;
				break;

			default:
				Error(parser, tag, MO_ERROR_INVALID, "invalid 'cond' parameter for 'mo:if', 'mo:elif' or 'mo:elsif'; it is none of 'true', 'false', '0' or '1'");
				break;

			}
		}
	}
	if(more) {
		var = tag.Get("is_empty");
		if(var) {
			more = false;
			result = var->IsEmpty();
		}
	}
	if(more) {
		var = tag.Get("is_not_empty");
		if(var) {
			more = false;
			result = !var->IsEmpty();
		}
	}

	if(more) {
		Error(parser, tag, MO_ERROR_UNKNOWN, "an 'mo:if', 'mo:elif' or 'mo:elsif' was used without a 'cond' or 'is_[not_]empty' parameter");
	}

	// return true when valid which is the opposite of more
	return !more;
}


bool moXML_Basic::moXML_Basic_If::SkipFalse(moXMLParser& parser, moXMLParser::moXMLTag& tag, bool maybe)
{
	moXMLParser::xml_type_t		t;
	moXMLParser::moXMLTypeSPtr	skip;
	moWCString			name;
	long				count;
	bool				result;

	count = 1;
	for(;;) {
		if(!parser.ReadNextNoSignal(skip)) {
			Error(parser, tag, MO_ERROR_INVALID, "an '<mo:if>' was used without an '</mo:if>'");
			return false;
		}
		t = skip->GetType();
		if(t == moXMLParser::XML_TYPE_TAG_START) {
			name = skip->GetTagName();
			if(name == "mo:if") {
				count++;
			}
			else {
				if(name == "mo:elif"
				|| name == "mo:elsif") {
					moXMLParser::moXMLTag& sub_tag = dynamic_cast<moXMLParser::moXMLTag&>(*skip);
					if(!sub_tag.IsEmpty()) {
						Error(parser, tag, MO_ERROR_INVALID, "the 'mo:elif' and 'mo:elsif' are expected to be empty tags");
					}
					if(maybe && count == 1) {
						if(GetCond(parser, *skip, result)) {
							if(result) {
								return true;
							}
						}
					}
				}
				else if(name == "mo:else") {
					moXMLParser::moXMLTag& sub_tag = dynamic_cast<moXMLParser::moXMLTag&>(*skip);
					if(!sub_tag.IsEmpty()) {
						Error(parser, tag, MO_ERROR_INVALID, "the 'mo:else' tag is expected to be empty");
					}
					if(maybe && count == 1) {
						return true;
					}
				}
			}
		}
		else if(t == moXMLParser::XML_TYPE_TAG_END) {
			name = skip->GetTagName();
			if(name == "mo:if") {
				count--;
				if(count == 0) {
					return false;
				}
			}
		}
	}
	/*NOTREACHED*/
}




/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Include

NAME

	Constructor -- creates the XML basic Set object
	Destructor -- cleans up the XML basic Set object
	OnEvent -- catch the tags mo:include
	AddIncludePath -- add one path to the list of paths to check for the input file
	AppendIncludePath -- append a list of include paths

SYNOPSIS

	moXML_Basic_Include(void);
	virtual ~moXML_Basic_Include();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);
	void AddIncludePath(const moWCString& path);
	void AppendIncludePath(const moListBase& path);

DESCRIPTION

	The moXML_Basic_Include object implements the mo:include function
	used to include the content of an external file in place of this tag.

	An mo:include tag must be empty.

	The mo:include tag accepts two parameters:

		src="<filename>"	the name of the file to be included
					instead of this tag

		mode="error"		if the file doesn't exist, generate
					an error; this is the default

		mode="warn"		if the file doesn't exist, generate
					a warning only

		mode="quiet"		if the file doesn't exist, just ignore
					this tag

RETURN VALUE

	The OnEvent() function will return true whenever the include tag
	is valid (i.e. either the specified file exists or the mode is
	set to something else than "error").

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_Include::moXML_Basic_Include(void)
	: moXMLEvent(0, "mo:include")
{
}


moXML_Basic::moXML_Basic_Include::~moXML_Basic_Include()
{
}


bool moXML_Basic::moXML_Basic_Include::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::xml_type_t	t;
	moVariableSPtr		name, mode, mime_type;
	moWCString		parent;
	bool			is_open;
	int			m;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		if(tag.IsEmpty()) {
			name = tag.Get("src");
			if(name == 0) {
				Error(parser, tag, MO_ERROR_UNKNOWN, "an 'mo:include' was used without an 'src' parameter");
			}
			else if(name->IsEmpty()) {
				Error(parser, tag, MO_ERROR_UNKNOWN, "an 'mo:include' was used with an empty 'src' parameter");
			}
			else {
				parent = moWCString(parser.InputStream()->Input()->InputFilename());
				moFileSPtr input = new moFile;
				is_open = input->VOpen(f_include_path, parent, *name, ".xml", false);
				if(is_open) {
					mime_type = tag.Get("type");
					moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
					stream->XMLInterpretVariables(parser.InputStream()->XMLGetInterpretVariables());
					parser.Include(*stream, (bool) mime_type ? static_cast<moWCString>(*mime_type) : moWCString("text/xml"));
				}
				else {

{
fprintf(stderr, "%ld paths checked for \"%s\":\n", f_include_path.Count(), name->c_str());
for(unsigned int idx = 0; idx < f_include_path.Count(); ++idx) {
fprintf(stderr, "  [%s]\n", f_include_path.Get(idx)->MBData());
}
}

					mode = tag.Get("mode");
					m = 0;
					if(mode) {
						if(mode->CaseCompare("warn") == MO_BASE_COMPARE_EQUAL) {
							m = 1;
						}
						else if(mode->CaseCompare("quiet") == MO_BASE_COMPARE_EQUAL) {
							m = 2;
						}
					}
					switch(m) {
					case 0:
						Error(parser, tag, MO_ERROR_INVALID, "cannot open file \"%S\" for 'mo:include'", name->Data());
						break;

					case 1:
						Warning(parser, tag, MO_ERROR_INVALID, "cannot open file \"%S\" for 'mo:include'", name->Data());
						break;

					// case 2: ignore!
					}
				}
			}
		}
		else {
			Error(parser, tag, MO_ERROR_NOT_IMPLEMENTED, "at this time, the 'mo:include' tag must be used empty");
		}
		return true;
	}
	// if not a TAG_START we do nothing

	return false;
}



void moXML_Basic::moXML_Basic_Include::AddIncludePath(const moWCString& path)
{
	moWCStringSPtr p = new moWCString(path);
	f_include_path.Insert(static_cast<moWCString *>(p));
}


void moXML_Basic::moXML_Basic_Include::AppendIncludePath(const moListBase& paths)
{
	moList::position_t	idx, max;

	max = paths.Count();
	idx = 0;
	while(idx < max) {
		moWCStringSPtr path = dynamic_cast<moWCString *>(paths.Get(idx));
		if(path) {
			AddIncludePath(*path);
		}
		idx++;
	}
}




/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Insert

NAME

	Constructor - creates the XML basic Set object
	Destructor - cleans up the XML basic Set object
	OnEvent - catch the tags mo:insert

SYNOPSIS

	moXML_Basic_Insert(void);
	virtual ~moXML_Basic_Insert();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

DESCRIPTION

	The moXML_Basic_Insert object implements the mo:insert function
	used to insert the content of a variable in place of this tag.

	An mo:insert tag must be empty.

	This function understand the following parameters:

		name="<variable name>"		the name of the variable
						to be inserted in place

RETURN VALUE

	The OnEvent() function will return true whenever the insert tag
	is valid.

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_Insert::moXML_Basic_Insert(void)
	: moXMLEvent(0, "mo:insert")
{
}


moXML_Basic::moXML_Basic_Insert::~moXML_Basic_Insert()
{
}


bool moXML_Basic::moXML_Basic_Insert::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::xml_type_t	t;
	moVariableSPtr		name, value;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		if(tag.IsEmpty()) {
			name = tag.Get("name");
			if(name == 0) {
				Error(parser, tag, MO_ERROR_UNKNOWN, "an mo:insert was used without a 'name' parameter");
			}
			else {
				// in this case we can set the variable right away
				value = parser.GetVariable(*name);
				if(value == 0) {
					Error(parser, tag, MO_ERROR_UNKNOWN, "undefined variable \"%S\"", name->Data());
				}
				else if(!value->IsEmpty()) {
					// NOTE: we avoid inserting empty variable! that would
					//	 take a lot of time for nothing.
					moMemFileSPtr input = new moMemFile;
					input->Put((mowc::wc_t) 0xFEFF);
					input->Write(value->Data(), value->Length() * sizeof(mowc::wc_t));
					moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
					parser.Include(*stream);
				}
			}
		}
		else {
			Error(parser, tag, MO_ERROR_NOT_IMPLEMENTED, "the 'mo:insert' tag must be empty");
		}
		return true;
	}
	// if not a TAG_START we do nothing

	return false;
}



/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Loop

NAME

	Constructor - creates the XML basic Loop object
	Destructor - cleans up the XML basic Loop object
	OnEvent - catch the tags mo:loop

SYNOPSIS

	moXML_Basic_Loop(void);
	virtual ~moXML_Basic_Loop();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

PARAMETERS

	parser - the XML parser which found this tag
	type - the last object read by the XML parser

DESCRIPTION

	The moXML_Basic_Loop object implements the mo:loop function.
	A loop can be an empty tag in which case it needs to have
	a 'data' parameter with the content to be repeated. All the
	supported parameters are:

		from		start counter value; if 'to' is
				defined, but not 'from', 'from' is
				set to 'to'; this makes the loop
				run only once

		to		end counter value; if not defined,
				the from/to can't break the loop

		step		value used to increment or decrement
				the value 'from' until it reaches 'to'
				this defaults to 1.0 when 'to' is
				larger than 'from' and -1.0 when
				'from' is larger than 'to'

		repeat		maximum number of times to repeat;
				inifinite when undefined

		separators	one or more separator defined in a
				string (default to " "); used with
				'foreach'

		foreach		execute for each word seperated by
				a space or the specified separators

		while		execute as long as this expression
				is true (tested on entry)

		until		execute as long as this expression
				is false (not tested on entry)

		name		the name of the variable to use to
				save the counter, repeat count and
				current word defined as:

					<name>_index (from value)
					<name>_count (repeat count)
					<name>_word (foreach word)

		data		loop on 'data' when the loop tag is
				empty

NOTES

	At this time the while & until can be used with a variable
	which wasn't reduced. This is the only way to use these
	parameters. The followings is an example of how to do this:

	1. set a variable named 'when' to the value '$(test)';
	   value which will stay defined as is.

		<mo:set name="when" keep_vars="true">$(test)</mo:set>

	2. set a startup value in the referenced variable (here 'test')
	   this value could be computed; it is valid to try to start
	   the loop with a result of false (in which case the content
	   of the entire loop will be ignored)

		<mo:set name="test" value="true"/>

	3. start the loop with a counter ('from' defined, but no upper
	   limit or maximum count); the 'while' parameter references
	   the first variable we defined ('when')

		<mo:loop name="idx" from="1" while="$(when)">

	4. within the loop, we change the value of the 'test' variable;
	   whenever it becomes false, the loop breaks; here the loop
	   will be repeated at most 3 times (note that such a simple
	   test can be achieved with the 'repeat' parameter set to
	   3, but for the sake of simplicity of this example...)

		<mo:set name="test" value="$(expr $(idx_count) < 3)"/>

	   Note: the index value $(idx_count) is defined as the name
	   followed by _count (name="idx" from the mo:loop declaration.)

	5. end the loop

		</mo:loop>

	If this gets fixed, the value of the 'while' parameter will
	become the actual expression.

RETURN VALUE

	The OnEvent() function will return true whenever the loop tag
	is valid.

BUGS

	In order to make the loop work with the XML parser as defined
	it needs to add an <mo:loop> tag in a sub-set of the loop.
	This tag is given one attribute named 'ignore' and given the
	value 'true'. If this attribute appears in the user tag, then
	the loop will fail.

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_Loop::Counter::Counter(void)
{
	f_type = COUNTER_UNDEFINED;
	//f_int -- not an int since it's undefined
	//f_float -- not a float since it's undefined
}


void moXML_Basic::moXML_Basic_Loop::Counter::LoadCounter(moXML_Basic::moXML_Basic_Loop *loop, moXMLParser& parser, moXMLParser::moXMLTag& tag, const moWCString& name)
{
	f_type = COUNTER_UNDEFINED;

	moVariableSPtr var = tag.Get(name);
	if(var != 0) {
		if(var->IsInteger()) {
			f_type = COUNTER_INTEGER;
			f_int = var->Integer();
		}
		else if(var->IsFloat()) {
			f_type = COUNTER_FLOAT;
			f_float = var->Float();
		}
		else {
			loop->Error(parser, tag, MO_ERROR_UNKNOWN, "the '%S' parameter of an 'mo:loop' is expected to be a number", name.Data());
		}
	}
}


void moXML_Basic::moXML_Basic_Loop::Counter::ForceInt(void)
{
	if(f_type == COUNTER_FLOAT) {
		f_type = COUNTER_INTEGER;
		f_int = static_cast<long>(f_float);		// round down
	}
}


void moXML_Basic::moXML_Basic_Loop::Counter::ForceFloat(void)
{
	if(f_type == COUNTER_INTEGER) {
		f_type = COUNTER_FLOAT;
		f_float = f_int;
	}
}


void moXML_Basic::moXML_Basic_Loop::Counter::SetToOne(const Counter& counter)
{
	f_type = counter.f_type;
	if(f_type == COUNTER_INTEGER) {
		f_int = 1;
	}
	else if(f_type == COUNTER_FLOAT) {
		f_float = 1.0;
	}
}


void moXML_Basic::moXML_Basic_Loop::Counter::Positive(void)
{
	if(f_type == COUNTER_INTEGER) {
		f_int = labs(f_int);
	}
	else if(f_type == COUNTER_FLOAT) {
		f_float = fabs(f_float);
	}
}


void moXML_Basic::moXML_Basic_Loop::Counter::Negative(void)
{
	if(f_type == COUNTER_INTEGER) {
		f_int = -labs(f_int);
	}
	else if(f_type == COUNTER_FLOAT) {
		f_float = -fabs(f_float);
	}
}


moWCString moXML_Basic::moXML_Basic_Loop::Counter::ToString(void) const
{
	moWCString	result;

	if(f_type == COUNTER_INTEGER) {
		result = moWCString::Format("%d", f_int);
	}
	else if(f_type == COUNTER_FLOAT) {
		result = moWCString::Format("%g", f_float);
	}

	return result;
}


bool moXML_Basic::moXML_Basic_Loop::Counter::operator < (long value) const
{
	if(f_type == COUNTER_INTEGER) {
		return f_int < value;
	}
	if(f_type == COUNTER_FLOAT) {
		return f_float < value;
	}

	// should return an error?
	return false;
}


bool moXML_Basic::moXML_Basic_Loop::Counter::operator < (const Counter& value) const
{
	if(f_type == value.f_type) {
		if(f_type == COUNTER_INTEGER) {
			return f_int < value.f_int;
		}
		if(f_type == COUNTER_FLOAT) {
			return f_float < value.f_float;
		}
	}

	// should return an error?
	return false;
}


bool moXML_Basic::moXML_Basic_Loop::Counter::operator == (long value) const
{
	if(f_type == COUNTER_INTEGER) {
		return f_int < value;
	}
	if(f_type == COUNTER_FLOAT) {
		return f_float < value;
	}

	// should return an error?
	return false;
}


moXML_Basic::moXML_Basic_Loop::Counter& moXML_Basic::moXML_Basic_Loop::Counter::operator -- (int)
{
	if(f_type == COUNTER_INTEGER) {
		f_int--;
	}
	else if(f_type == COUNTER_FLOAT) {
		f_float -= 1.0;
	}

	return *this;
}


moXML_Basic::moXML_Basic_Loop::Counter& moXML_Basic::moXML_Basic_Loop::Counter::operator += (const Counter& value)
{
	if(f_type == value.f_type) {
		if(f_type == COUNTER_INTEGER) {
			f_int += value.f_int;
		}
		else if(f_type == COUNTER_FLOAT) {
			f_float += value.f_float;
		}
	}

	return *this;
}






moXML_Basic::moXML_Basic_Loop::moXML_Basic_Loop(void)
	: moXMLEvent(0, "mo:loop")
{
	f_count = 0;
	// f_data -- if f_count is zero, there is nothing in this array
	// f_stack -- auto-init
}


moXML_Basic::moXML_Basic_Loop::~moXML_Basic_Loop()
{
}




bool moXML_Basic::moXML_Basic_Loop::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::moXMLTypeSPtr	skip;
	moXMLParser::xml_type_t		t;
	moVariableSPtr			var;
	LoopDataSPtr			data;
	moWCString			name;
	long				count, p;
	unsigned long			idx;
	bool				test_until, has_data;
	moList::position_t		from, to;
	moMemFileSPtr			input;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);

		var = tag.Get("ignore");
		if(var != 0 && *var == "true") {
			return true;
		}

		data = new LoopData;

		data->f_from.LoadCounter(this, parser, tag, "from");	// start counter (note that 'from' can be larger than 'to')
		data->f_to.LoadCounter(this,   parser, tag, "to");	// maximum counter (inclusive)
		data->f_step.LoadCounter(this, parser, tag, "step");	// from += step on each loop

		if(data->f_from.IsFloat()
		|| data->f_to.IsFloat()
		|| data->f_step.IsFloat()) {
			data->f_from.ForceFloat();
			data->f_to.ForceFloat();
			data->f_step.ForceFloat();
		}

		if(data->f_from.IsUndefined()
		&& !data->f_to.IsUndefined()) {
			data->f_from = data->f_to;
		}
		/* if to is undefined, then the from/to can't break the loop
		else if(!data->f_from.IsUndefined()
		&& data->f_to.IsUndefined()) {
			data->f_to = data->f_from;
		}
		*/
		if(!data->f_from.IsUndefined()
		&& data->f_step.IsUndefined()) {
			data->f_step.SetToOne(data->f_from);
		}
		if(data->f_step == 0) {
			Error(parser, tag, MO_ERROR_UNKNOWN, "the 'step' parameter of an 'mo:loop' can't be set to zero");
			data->f_step.SetToOne(data->f_from);
		}

		if(!data->f_to.IsUndefined()) {
			if(data->f_from < data->f_to) {
				data->f_step.Positive();
			}
			else {
				data->f_step.Negative();
			}
		}

		data->f_repeat.LoadCounter(this, parser, tag, "repeat");	// repeat that many times (at most)
		if(data->f_repeat.IsFloat()) {
			Error(parser, tag, MO_ERROR_UNKNOWN, "the 'repeat' parameter of an 'mo:loop' is expected to be an integer");
			data->f_repeat.ForceInt();
			data->f_repeat.SetToOne(data->f_repeat);
		}
		else if(!data->f_repeat.IsUndefined() && data->f_repeat < 0) {
			Error(parser, tag, MO_ERROR_UNKNOWN, "the 'repeat' parameter of an 'mo:loop' can't be negative");
			data->f_repeat.SetToOne(data->f_repeat);
		}
		data->f_count = 1;

		var = tag.Get("separators");
		if(var != 0 && !var->IsEmpty()) {
			data->f_for_each.CharSeparators(*var);
		}
		else {
			data->f_for_each.CharSeparators(" ");
		}

		var = tag.Get("foreach");	// repeat loop for each word
		data->f_has_words = var != 0;
		if(data->f_has_words) {
			data->f_for_each.SetWords(*var);
		}

		var = tag.Get("while");		// as long as this expression is true
		if(var != 0) {
			data->f_while = *var;
		}
		else {
			data->f_while.Empty();
		}
		var = tag.Get("until");		// until this expression becomes true (also, not checked on 1st loop)
		if(var != 0) {
			data->f_until = *var;
		}
		else {
			data->f_until.Empty();
		}

		var = tag.Get("name");		// the name of the variable counter
		if(var != 0) {
			data->f_name = *var;
		}
		else {
			data->f_name.Empty();
		}

		// first we need to determine whether we will repeat this loop even once
		// and if not we need to skip everything up to its end (</mo:loop>)
		// otherwise we need to keep a state so the end loop can eventually
		// restart the loop (if all the conditions are met)
		if(tag.IsEmpty()) {
			var = tag.Get("data");
			if(var == 0) {
				Error(parser, tag, MO_ERROR_UNKNOWN, "an empty 'mo:loop' must include a 'data' parameter");
			}
			else if(!var->IsEmpty()) {
				test_until = false;
				while(EvaluateLoopExpr(parser, &tag, data, test_until)) {
					input = new moMemFile;
					input->Put((mowc::wc_t) 0xFEFF);
					input->Write(var->Data(), var->Length() * sizeof(mowc::wc_t));
					moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
					parser.Include(*stream);
					// release the input stream
					input = 0;	//dynamic_cast<moIStream *>(input)->Release();
					EvaluateNextLoop(parser, data);
					test_until = true;
				}
			}
		}
		else if(EvaluateLoopExpr(parser, &tag, data, false)) {
			if(f_count >= STACK_MAX_COUNT) {
				f_stack += *f_data[STACK_MAX_COUNT - 1];
				f_count--;
			}
			idx = f_count;
			while(idx > 0) {
				idx--;
				f_data[idx + 1] = f_data[idx];
			}
			f_data[0] = data;
			f_count++;

			data->f_interpret_variables = parser.InputStream()->XMLGetInterpretVariables();
			parser.InputStream()->XMLInterpretVariables(false);
			has_data = parser.ReadNextBlock(from, to);
			parser.InputStream()->XMLInterpretVariables(data->f_interpret_variables);
			if(has_data) {
				parser.BlockToString(data->f_data, from, to);
				// so we have a matching tag we insert an mo:loop at the
				// start of this buffer
				data->f_data = data->f_data.Insert("<mo:loop ignore=\"true\">\n", 0);

//printf("Loop = <[%s]>\n", data->f_data.MBData());
				input = new moMemFile;
				input->Put((mowc::wc_t) 0xFEFF);
				input->Write(data->f_data.Data(), data->f_data.Length() * sizeof(mowc::wc_t));
				moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
				stream->XMLInterpretVariables(data->f_interpret_variables);
				parser.Include(*stream);
				// Release the input stream
				input = 0;	//dynamic_cast<moIStream *>(input)->Release();
			}
		}
		else {
			// in this case the loop needs to be ignored...
			// (like a mo:if with a false expression)
			data = 0;
			count = 1;
			for(;;) {
				if(!parser.ReadNextNoSignal(skip)) {
					Error(parser, tag, MO_ERROR_INVALID, "an 'mo:loop' was used without an '</mo:loop>'");
					break;
				}
				t = skip->GetType();
				if(t == moXMLParser::XML_TYPE_TAG_START) {
					name = skip->GetTagName();
					if(name == "mo:loop") {
						count++;
					}
				}
				else if(t == moXMLParser::XML_TYPE_TAG_END) {
					name = skip->GetTagName();
					if(name == "mo:loop") {
						count--;
						if(count == 0) {
							break;
						}
					}
				}
			}
		}
		return true;
	}
	if(t == moXMLParser::XML_TYPE_TAG_END) {
		// we found the end, we may want to go back to the start!
		if(f_count == 0) {
			parser.FormatError(MO_ERROR_INVALID, "an '</mo:loop>' was found without an '<mo:loop>'");
		}
		else {
			EvaluateNextLoop(parser, f_data[0]);
			if(EvaluateLoopExpr(parser, 0, f_data[0], true)) {
				input = new moMemFile;
				input->Put((mowc::wc_t) 0xFEFF);
				input->Write(f_data[0]->f_data.Data(), f_data[0]->f_data.Length() * sizeof(mowc::wc_t));
				moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
				stream->XMLInterpretVariables(f_data[0]->f_interpret_variables);
				parser.Include(*stream);
				// Release the stream on our end
				input = 0;	//dynamic_cast<moIStream *>(input)->Release();
			}
			else {
				f_data[0] = 0;
				f_count--;
				if(f_count > 0) {
					for(idx = 0; idx < f_count; ++idx) {
						f_data[idx] = f_data[idx + 1];
					}
					if(!f_stack.IsEmpty()) {
						p = f_stack.Count() - 1;
						f_data[STACK_MAX_COUNT - 1] = dynamic_cast<LoopData *>(f_stack.Get(p));
						f_stack.Delete(p);
						f_count++;
					}
				}
			}
		}
		return true;
	}
	// if not a TAG_START we do nothing

	return false;
}


bool moXML_Basic::moXML_Basic_Loop::EvaluateLoopExpr(moXMLParser& parser, moXMLParser::moXMLTag *tag, LoopData *data, bool test_until)
{
	char		count[64];	/* Flawfinder: ignore */
	moWCString	result;

	if(data->f_repeat.IsInteger()) {
		if(data->f_repeat < data->f_count) {
			return false;
		}
	}

	if(!data->f_from.IsUndefined() && !data->f_to.IsUndefined()) {
		if(data->f_step < 0) {
			if(data->f_from < data->f_to) {
				return false;
			}
		}
		else {
			if(data->f_to < data->f_from) {
				return false;
			}
		}
	}

	if(data->f_has_words) {
		if(data->f_for_each.IsEmpty()) {
			return false;
		}
	}

	if(!data->f_while.IsEmpty()) {
		// we need access to the variable reducer of the
		// parser to do this, also the variables should
		// be kept intact in the string of the 'while'
		// parameter, not too sure how that works yet...
		moXMLParser::moXMLStringReducer reducer(data->f_while, parser.InputStream());
		reducer.Reduce(result);
		switch(result.IsTrue(moWCString::WC_STRING_BOOLEAN_TEST_FLOAT | moWCString::WC_STRING_BOOLEAN_TEST_INTEGER_C | moWCString::WC_STRING_BOOLEAN_TEST_NAMED)) {
		default:
			if(tag == 0) {
				parser.FormatError(MO_ERROR_INVALID, "invalid 'while' parameter for 'mo:loop'; it is none of 'true', 'false', '0' or '1'");
			}
			else {
				Error(parser, *tag, MO_ERROR_INVALID, "invalid 'while' parameter for 'mo:loop'; it is none of 'true', 'false', '0' or '1'");
			}
			/*FLOWTHROUGH*/
		case 0:
			return false;

		case 1:
			break;

		}
	}

	if(test_until && !data->f_until.IsEmpty()) {
		// we need access to the variable reducer of the
		// parser to do this, also the variables should
		// be kept intact in the string of the 'until'
		// parameter, not too sure how that works yet...
		moXMLParser::moXMLStringReducer reducer(data->f_while, parser.InputStream());
		reducer.Reduce(result);
		switch(result.IsFalse(moWCString::WC_STRING_BOOLEAN_TEST_FLOAT | moWCString::WC_STRING_BOOLEAN_TEST_INTEGER_C | moWCString::WC_STRING_BOOLEAN_TEST_NAMED)) {
		default:
			if(tag == 0) {
				parser.FormatError(MO_ERROR_INVALID, "invalid 'until' parameter for 'mo:loop'; it is none of 'true', 'false', '0' or '1'");
			}
			else {
				Error(parser, *tag, MO_ERROR_INVALID, "invalid 'until' parameter for 'mo:loop'; it is none of 'true', 'false', '0' or '1'");
			}
			/*FLOWTHROUGH*/
		case 0:
			return false;

		case 1:
			break;

		}
	}

	if(!data->f_name.IsEmpty()) {
		if(data->f_repeat.IsInteger()) {
			snprintf(count, sizeof(count), "%ld", data->f_count);	/* Flawfinder: ignore */
			parser.SetVariable(data->f_name + "_count", count);
		}
		if(!data->f_from.IsUndefined()) {
			parser.SetVariable(data->f_name + "_index", data->f_from.ToString());
		}
		if(data->f_has_words) {
			parser.SetVariable(data->f_name + "_word", data->f_for_each[0]);
		}
	}

	return true;
}


void moXML_Basic::moXML_Basic_Loop::EvaluateNextLoop(moXMLParser& parser, LoopData *data)
{
	// it is faster to always increment this counter rather than testing
	// whether we should increment it; it has no side effect anyway if
	// it isn't necessary
	data->f_count++;

	if(!data->f_step.IsUndefined()) {
		data->f_from += data->f_step;
	}

	if(data->f_has_words && !data->f_for_each.IsEmpty()) {
		data->f_for_each.Delete(0);
	}
}



/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Set

NAME

	Constructor - creates the XML basic Set object
	Destructor - cleans up the XML basic Set object
	OnEvent - catch the tags mo:set

SYNOPSIS

	moXML_Basic_Set(void);
	virtual ~moXML_Basic_Set();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

DESCRIPTION

	The moXML_Basic_Set object implements the mo:set function
	used to set a variable value.

	Note that the mo:set tag can be used with either an empty tag
	or a block of data between a start and end tag.

	In case of an empty tag, the tag should have a name and
	value.

RETURN VALUE

	The OnEvent() function will return true whenever the set tag
	is valid.

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_Set::moXML_Basic_Set(void)
	: moXMLEvent(0, "mo:set")
{
}


moXML_Basic::moXML_Basic_Set::~moXML_Basic_Set()
{
}


bool moXML_Basic::moXML_Basic_Set::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::xml_type_t	t;
	moVariableSPtr		name, value;
	moList::position_t	from, to;
	moWCString		value_string;
	bool			valid_data, interpret_variables, iv;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		name = tag.Get("name");
		if(name == 0) {
			Error(parser, tag, MO_ERROR_UNKNOWN, "an 'mo:set' was used without a 'name' parameter");
		}
		else if(name->IsEmpty()) {
			Error(parser, tag, MO_ERROR_INVALID, "invalid 'name' parameter for 'mo:set', variable \"%S\" is empty", name->Data());
		}
		else if(tag.IsEmpty()) {
			// in this case we can set the variable right away
			value = tag.Get("value");
			if(value == 0) {
				parser.SetVariable(*name, "");
			}
			else {
				parser.SetVariable(*name, *value);
			}
		}
		else {
			// when the tag isn't empty, we need the data within
			// the start & end as the value of the variable
			iv = true;
			value = tag.Get("keep_variables");
			if(value == 0) {
				value = tag.Get("keep_vars");
			}
			if(value != 0) {
				if(value->CaseCompare("true") == MO_BASE_COMPARE_EQUAL
				|| value->CaseCompare("1") == MO_BASE_COMPARE_EQUAL) {
					iv = false;
				}
			}
			interpret_variables = parser.InputStream()->XMLGetInterpretVariables();
			parser.InputStream()->XMLInterpretVariables(iv);
			valid_data = parser.ReadNextBlock(from, to);
			parser.InputStream()->XMLInterpretVariables(interpret_variables);
			if(valid_data) {
				parser.BlockToString(value_string, from, to - 1);
/*printf("Set variable [%s] to [%s]\n", name->MBData(), value_string.MBData());*/
				parser.SetVariable(*name, value_string);
			}
		}
		return true;
	}
	// if not a TAG_START we do nothing

	return false;
}


/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Unset

NAME

	Constructor - creates the XML basic Set object
	Destructor - cleans up the XML basic Set object
	OnEvent - catch the tags mo:unset

SYNOPSIS

	moXML_Basic_Unset(void);
	virtual ~moXML_Basic_Unset();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

DESCRIPTION

	The moXML_Basic_Unset object implements the mo:unset function
	used to unset (delete, clear) a variable and its value.

	An mo:unset tag must be empty.

RETURN VALUE

	The OnEvent() function will return true whenever the unset tag
	is valid.

SEE ALSO

	moXML_Basic

*/
moXML_Basic::moXML_Basic_Unset::moXML_Basic_Unset(void)
	: moXMLEvent(0, "mo:unset")
{
}


moXML_Basic::moXML_Basic_Unset::~moXML_Basic_Unset()
{
}


bool moXML_Basic::moXML_Basic_Unset::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::xml_type_t	t;
	moVariableSPtr		name;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		name = tag.Get("name");
		if(name == 0) {
			Error(parser, tag, MO_ERROR_UNKNOWN, "an 'mo:unset' was used without a 'name' parameter");
		}
		else if(name->IsEmpty()) {
			Error(parser, tag, MO_ERROR_INVALID, "invalid name for 'mo:unset', 'name' is empty");
		}
		else if(tag.IsEmpty()) {
			// in this case we can set the variable right away
			parser.UnsetVariable(*name);
			// TODO: warn on "name" not defined
		}
		else {
			Error(parser, tag, MO_ERROR_NOT_IMPLEMENTED, "the 'mo:unset' tag must be used empty");
		}
		return true;
	}
	// if not a TAG_START we do nothing

	return false;
}




/************************************************************ DOC:

CLASS

	moXML_Basic::moXML_Basic_Replace

NAME

	Constructor - creates a new XML parser extension object
	Destructor - cleans up an XML parser extension object
	OnEvent - execute an event that this extension understands

SYNOPSIS

	moXML_Basic_Replace(void);
	virtual ~moXML_Basic_Replace();
	bool OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

PARAMETERS

	parser - the XML parser
	type - the type of the tag being passed in

DESCRIPTION

	The <mo:replace ...> tag will ensure that the specified variable
	can be used as a string in a script or as the value of a tag do
	not include character which will break the script or tag.

	The attribute 'name' is used to know which variable is to be acted
	on.

	Since the function could be wrongly used on undefined variables,
	this instruction will generate an error by default. To avoid the
	error, defined the 'undefined' attribute and set it to "no-error".

	If the attribute format is specified, then it is used to know what
	characters the user wants to transform. The string is composed of
	characters as follow:

		=	apply all the previous formats
		a	replace ampersands (&) with &amp;
		b	double blackslashes (\ becomes \\)
		c	remove double spaces (tabs, spaces, etc.)
		C	remove new lines and carriage return characters
		d	replace double quotes (") with &quot;
		g	replace greater than sign (>) with &gt;
		l	replace smaller than sign (<) with &lt;
		n	replace new lines (0x0A and/or 0x0D) with \n
		q	precede single (') and double (") quotes with a backslash (\)
		Q	like q, but put three backslashes instead of just one (i.e. \\\' and \\\")
		s	replace single quotes (') with &#39;
		v	assume that the input is XML (reduce variables and mo tags, remove comments)
		V	assume that the input is XML (like 'v', but keep the comments)

	WARNING: whenever you specify 'v' or 'V', the reduction specified
		 so far applies; otherwise, doubling a character format has
		 absolutly no effect so "Q" and "QQ" have the same effect;
		 if you need to double a format, use an equal sign as in:
		 "Q=Q" (Q is applied twice).

	If the format parameter is not specified, then the default is:

		"adglns"

	Note that the character 'v' is very special in that it will be
	executed at the time it is found in the string. The exact algorithm
	is like this:

		read format character
		if 'v' then
			execute all replacements found so far
			and execute the 'v' command
		repeat until the end of the format string is reached
		execute the remaining replacements found

SEE ALSO

	moXMLParser::moXMLEvent

*/
moXML_Basic::moXML_Basic_Replace::moXML_Basic_Replace(void)
	: moXMLEvent(0, "mo:replace")
{
}


moXML_Basic::moXML_Basic_Replace::~moXML_Basic_Replace()
{
}



bool moXML_Basic::moXML_Basic_Replace::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::moXMLTypeSPtr	ignore;
	moList::position_t		from, to;
	moXMLParser::xml_type_t		t;
	const mowc::wc_t		*str;
	mowc::wc_t			buf[2];
	moVariableSPtr			name, value, format, undefined;
	moWCString			replace;
	const char			*fmt;
	bool				comments_flag;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		if(tag.IsEmpty()) {
			name = tag.Get("name");
			if(!name) {
				Error(parser, tag, MO_ERROR_UNKNOWN, "an mo:replace was used without a 'name' parameter");
			}
			else {
				value = parser.GetVariable(*name);
				if(!value) {
					undefined = tag.Get("undefined");
					if(undefined == 0 || *undefined != "no-error") {
						Error(parser, tag, MO_ERROR_UNKNOWN, "undefined variable \"%S\" in an mo:replace", name->Data());
					}
				}
				else /*if(!value->IsEmpty())*/ {
					format = tag.Get("format");
					if(format) {
						fmt = format->SavedMBData();
					}
					else {
						fmt = "adglns";
					}
					//replace = ""; -- this is already the case
					while(*fmt != '\0') {
						switch(*fmt) {
						case '=':
							*value = value->Replace(replace);
							replace.Empty();
							break;

						case 'a':
							replace += "&=&amp\\;;";
							break;

						case 'b':
							replace += "\\\\=\\\\\\\\;";
							break;

						case 'c':
							*value = value->Replace(replace);
							if(!value->IsEmpty()) {
								replace.Empty();
								buf[1] = L'\0';
								str = value->Data();
								while(*str != '\0') {
									if(mowc::isspace(*str)) {
										while(mowc::isspace(str[1])) {
											str++;
										}
									}
									buf[0] = *str;
									replace += buf;
									str++;
								}
								*value = replace;
							}
							replace.Empty();
							break;

						case 'C':
							*value = value->Replace(replace);
							if(!value->IsEmpty()) {
								replace.Empty();
								buf[1] = L'\0';
								str = value->Data();
								while(*str != '\0') {
									if(*str != '\n' && *str != '\r') {
										buf[0] = *str;
										replace += buf;
									}
									str++;
								}
								*value = replace;
							}
							replace.Empty();
							break;

						case 'd':
							replace += "\"=&quot\\;;";
							break;

						case 'g':
							replace += ">=&gt\\;;";
							break;

						case 'l':
							replace += "<=&lt\\;;";
							break;

						case 'n':
							replace += "\r\n=\\\\n;\n=\\\\n;\r=\\\\n;";
							break;

						case 'q':
							replace += "'=\\\\';\"=\\\\\";";
							break;

						case 'Q':
							replace += "'=\\\\\\\\\\\\';\"=\\\\\\\\\\\\\";";
							break;

						case 's':
							replace += "'=&#39\\;;";
							break;

						case 'v':
						case 'V':
							*value = value->Replace(replace);
							if(!value->IsEmpty()) {
								// NOTE: we avoid inserting empty data! that would
								//	 take a lot of time for nothing.
								moMemFileSPtr input = new moMemFile;
								input->Put((mowc::wc_t) 0xFEFF);
								input->Put((mowc::wc_t) '<');
								input->Put((mowc::wc_t) 'v');
								input->Put((mowc::wc_t) '>');
								input->Write(value->Data(), value->Length() * sizeof(mowc::wc_t));
								input->Put((mowc::wc_t) '<');
								input->Put((mowc::wc_t) '/');
								input->Put((mowc::wc_t) 'v');
								input->Put((mowc::wc_t) '>');
								moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
								stream->XMLInterpretVariables(true);
								comments_flag = parser.ReturnComments(*fmt == 'V');
								parser.Include(*stream);
								// First read the <v> tag
								parser.ReadNextNoSignal(ignore);
//fprintf(stderr, "BEFORE: <?xml ...?><v>%s</v>\n", value->c_str());
								// then read everything up to the </v> tag
								/*has_data =*/ parser.ReadNextBlock(from, to, true, &replace);
								*value = replace;
								/*
								if(has_data) {
									parser.BlockToString(*value, from, to - 1);
								}
								else {
									*value = "";
								}
								*/
//fprintf(stderr, "AFTER (%d): [%s]\n", has_data, value->c_str());
								parser.ReturnComments(comments_flag);
							}
							replace.Empty();
							break;

						}
						fmt++;
					}
#if 0
fprintf(stderr, "REPLACE: Replacing [%s] using [%s] is [%s]\n",
			value->SavedMBData(),
			replace.SavedMBData(),
			value->Replace(replace).SavedMBData());
#endif
					*value = value->Replace(replace);
				}
			}
		}
		else {
			Error(parser, tag, MO_ERROR_NOT_IMPLEMENTED, "the 'mo:replace' tag must be empty");
		}

		return true;
	}
	/* if not XML_TYPE_TAG_START we ignore */

	return false;
}


/** \brief Initializes the UseDTD object.
 *
 * The moXML_Basic_UseDTD object implements the mo:usedtd tags.
 *
 * Whenever a usedtd tag is found, this object calls the XML parser
 * and sets the 'use DTD flag' to the value of the status attribute.
 * If the status attribute is not defined, set the flag to true.
 */
moXML_Basic::moXML_Basic_UseDTD::moXML_Basic_UseDTD(void)
	: moXMLEvent(0, "mo:usedtd"),
	  f_restore_usedtd(sizeof(bool))
{
}


/** \brief Cleans up the UseDTD object.
 *
 * The destructor takes care of releasing the resources.
 */
moXML_Basic::moXML_Basic_UseDTD::~moXML_Basic_UseDTD()
{
}


/** \brief Execute the tag whenever found.
 *
 * The OnEvent() function is called by the system whenever events
 * are allowed.
 *
 * It will call the moXMLParser::UseDTD(bool use) function with
 * true or false. The closing tag will restore the value.
 *
 * This tag cannot be used empty.
 *
 * Use the \p status attribute to the <mo:usedtd> tag to define
 * the new state. By default it is assumed to be "true".
 *
 * \param[in] parser The XML parser generating this event.
 * \param[in] type The type describing the tags.
 *
 * \return true when no error occurs.
 */
bool moXML_Basic::moXML_Basic_UseDTD::OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type)
{
	moXMLParser::xml_type_t		t;
	moVariableSPtr			name, value;
	bool				status;
	moList::position_t		pos;

	t = type.GetType();
	if(t == moXMLParser::XML_TYPE_TAG_START) {
		moXMLParser::moXMLTag& tag = dynamic_cast<moXMLParser::moXMLTag&>(type);
		if(tag.IsEmpty()) {
			Error(parser, tag, MO_ERROR_INVALID, "an 'mo:usedtd' is expected to not be empty");
		}
		else {
			// default to 'true', set to false if user requests so
			status = true;
			name = tag.Get("status");
			if(name) {
				value = parser.GetVariable(*name);
				if(value && (*value == "false" || *value == "off" || *value == "0")) {
					status = false;
				}
			}
			status = parser.UseDTD(status);
			f_restore_usedtd.Insert(&status);
		}
	}
	else {
		if(f_restore_usedtd.IsEmpty()) {
			(void) parser.UseDTD(false);
		}
		else {
			pos = f_restore_usedtd.Count() - 1;
			status = * (bool *) f_restore_usedtd.Get(pos);
			f_restore_usedtd.Delete(pos);
			(void) parser.UseDTD(status);
		}
	}

	return true;
}






};			// namespace molib;

// vim: ts=8

