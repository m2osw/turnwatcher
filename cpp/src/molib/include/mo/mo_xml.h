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



#ifndef MO_XML_H
#define	MO_XML_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_TEXT_STREAM_H
#include	"mo_text_stream.h"
#endif
#ifndef MO_VARIABLE_REDUCER_H
#include	"mo_variable_reducer.h"
#endif
#ifndef MO_VARIABLE_H
#include	"mo_variable.h"
#endif
#ifndef MO_ARRAY_H
#include	"mo_array.h"
#endif



namespace molib
{

class MO_DLL_EXPORT moXMLParser : public moBase
{
public:
	//static const unsigned long	XML_BUFSIZ = 4096;
	enum xml_type_t {
		XML_TYPE_UNKNOWN	= -1,
		XML_TYPE_TAG_START	=  0,		// opening tag <name ...>
		XML_TYPE_TAG_END	=  1,		// closing tag </name>
		XML_TYPE_PI		=  2,		// <?name ... ?> -- external script
		XML_TYPE_DATA		=  3,		// data between start/end tag
		XML_TYPE_CDATA		=  4,		// CDATA
		XML_TYPE_COMMENT	=  5,		// <!-- ... --> (conditionaly returned)

		XML_TYPE_max
	};

	enum xml_errcode_t {
		XML_ERRCODE_NONE = 0,

		// please, sort in alphabetical order
		XML_ERRCODE_BADNAME,
		XML_ERRCODE_BADPI,
		XML_ERRCODE_BADCOMMENT,
		XML_ERRCODE_BADETAG,
		XML_ERRCODE_DEFINED_TWICE,
		XML_ERRCODE_EOF_BEFORE_END_OF_CONDITION,
		XML_ERRCODE_INVALID,
		XML_ERRCODE_IO,
		XML_ERRCODE_NOROOT,
		XML_ERRCODE_NOXMLDECL,
		XML_ERRCODE_NOXMLVERSION,
		XML_ERRCODE_SYNTAX,
		XML_ERRCODE_UNDEFINED_CHAR,
		XML_ERRCODE_UNKNOWN_ENCODING,
		XML_ERRCODE_VERSION,

		XML_ERRCODE_max
	};

	// the following is to test complete sets of Unicode chars
	// don't use LETTER for the [a-zA-Z] range only
	// don't use DIGIT for the [0-9] range only
	// characters not listed in any of the tables are valid [P]CDATA
	// characters (opposed to the INVALID characters listed below)
	static const uint32_t	XML_CTYPE_VALID		= 0x00000001;
	static const uint32_t	XML_CTYPE_SPACE		= 0x00000002;
	static const uint32_t	XML_CTYPE_LETTER	= 0x00000004;
	static const uint32_t	XML_CTYPE_BASE		= 0x00000008;
	static const uint32_t	XML_CTYPE_IDEOGRAPHIC	= 0x00000010;
	static const uint32_t	XML_CTYPE_COMBINING	= 0x00000020;
	static const uint32_t	XML_CTYPE_DIGIT		= 0x00000040;
	static const uint32_t	XML_CTYPE_EXTENDER	= 0x00000080;
	static const uint32_t	XML_CTYPE_ENCODING_START= 0x00000100;
	static const uint32_t	XML_CTYPE_ENCODING	= 0x00000200;
	static const uint32_t	XML_CTYPE_PUBLIC_ID	= 0x00000400;
	static const uint32_t	XML_CTYPE_PUBLIC_ID_DQ	= 0x00000800;
	static const uint32_t	XML_CTYPE_NAME		= 0x00001000;	// | 0x0004
	static const uint32_t	XML_CTYPE_NAME_CHAR	= 0x00002000;	// | 0x08E4
	static const uint32_t	XML_CTYPE_CHAR_REF	= 0x00004000;
	static const uint32_t	XML_CTYPE_XCHAR_REF	= 0x00008000;
	static const uint32_t	XML_CTYPE_INVALID	= 0x80000000;

	// The stream object used to read the files
	// Note that this is seperate so we can have a stack of
	// streams to allow for an include like mechanism (because we
	// need to keep the exact state of the current stream in order
	// to continue reading it later)
	class MO_DLL_EXPORT moXMLStream : public moTextStream
	{
	public:
					moXMLStream(moIStream *input = 0, moOStream *output = 0, moOStream *error = 0);
		virtual			~moXMLStream();

		bool			XMLSetInputEncoding(const moWCString& encoding);
		void			XMLSetParser(moXMLParser& parser);

		int			XMLNextLine(void);
		mowc::wc_t		XMLGetC(void);
		void			XMLUngetC(void);
		int			XMLGetString(moWCString& str, int count);
		bool			XMLTestString(const char *str, bool case_sensitive = true) const;
		bool			XMLTestName(int skip, const char *str, bool case_sensitive = true) const;
		const moWCString	XMLLeftOnCurrentLine(int skip = 0) const;
		mowc::wc_t		XMLSkipSpaces(void);
		void			XMLSkipC(int count = 1);
		void			XMLSetPos(unsigned long pos);
		unsigned long		XMLGetPos(void) const;
		const moWCString&	XMLCurrentLine(void) const;
		const moWCString&	XMLCurrentString(void) const;
		moWCString&		XMLCurrentString(void);
		long			XMLFindInCurrentLine(const moWCString& str, long length = -1) const;

		void			XMLSaveName(const moWCString& name);
		const moWCString&	XMLGetName(void) const;

		bool			XMLGetInterpretVariables(void) const;
		void			XMLInterpretVariables(bool interpret_variables = true);
		void			XMLAppendVariableList(const moVariableList& list);
		void			XMLSafeAppendVariableList(const moVariableList& list);
		moVariableSPtr		XMLSetVariable(const moVariable& var);
		moVariableSPtr		XMLSetVariable(const moWCString& set);
		moVariableSPtr		XMLSetVariable(const moWCString& name, const moWCString& value);
		moVariableSPtr		XMLGetVariable(const moWCString& name) const;
		moVariableSPtr		XMLGetExtendedVariable(const moWCString& name) const;
		void			XMLUnsetVariable(const moWCString& name);

	private:
		friend class moReducer;
		class MO_DLL_EXPORT moReducer : public moVariableReducer
		{
		public:
						moReducer(moXMLStream *stream);
			virtual			~moReducer();

			virtual int		Reduce(moWCString& result);
			virtual moWCString	GetVariable(const moWCString& name);
			virtual mowc::wc_t	GetC(unsigned long depth);

		private:
			class MO_DLL_EXPORT moReduceExpandEntities : public moVariableReducerCommand
			{
			public:
				virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result);
				virtual const moWCString	Name(void) const;
			};

			bool			f_error;
			moXMLStream *		f_stream;
			moReduceExpandEntities	f_expand_entities;
		};

		virtual void		OnNewStream(int which);
		int			XMLNextLineNow(void);
		mowc::wc_t		XMLGetCBlock(void);

		unsigned long		f_pos;			// position in the current line chars
		moWCString		f_current_line;		// the current line of data from the input
		moWCString		f_current_string;	// what was read last (like a Name, Comment, etc.)
		moWCString		f_name;			// the name at the time an include is found
		moVariableList		f_variables;		// local variables
		mowc::moIConv		f_convertor;		// byte stream convertor to UTF-32 characters
		bool			f_interpret_variables;	// whether we understand $(<name> ...)
		moReducer		f_variable_reducer;	// to transform $(<name> ...) to the variable(s) value
		moXMLParser *		f_parser;		// the parser this stream is being used in at this time
	};
	typedef moSmartPtr<moXMLStream>		moXMLStreamSPtr;
	typedef moTmplList<moXMLStream, moList>	moListOfXMLStreams;

	class MO_DLL_EXPORT moXMLStringReducer : public moVariableReducer
	{
	public:
					moXMLStringReducer(const moWCString& string, moXMLStream *stream);

		virtual moWCString	GetVariable(const moWCString& name);
		virtual mowc::wc_t	GetC(unsigned long depth);

	private:
		unsigned long		f_pos;
		moWCString		f_string;
		moXMLStreamSPtr		f_stream;
	};

	// XML is composed of all the following data types
	class MO_DLL_EXPORT moXMLType : public moWCString
	{
	// used for:
	//	Closing Tags
	//	Empty (once in a while it is easier to return such an item)
	public:
		moXMLType(xml_type_t type, const moWCString& name, const moWCString& tag_name = "");

		xml_type_t		GetType(void) const;
		const moWCString&	GetTagName(void) const;	// this tag name vs. this tag "directory like name"
		virtual void		ToString(moWCString& string) const;
		void			SetPosition(size_t position);
		size_t			GetPosition(void) const;
	private:
		const xml_type_t	f_type;
		const moWCString	f_tag_name;
		size_t			f_position;	// input file offset (save only when required)
	};
	typedef moSmartPtr<moXMLType>		moXMLTypeSPtr;
	typedef moTmplList<moXMLType, moList>	moListOfXMLType;

	class MO_DLL_EXPORT moXMLData : public moXMLType
	{
	// used for:
	//	Direct Data (between tags)
	//	Comments (only when requested)
	//	PI targets
	//	CDATA (read from a <![CDATA[...]]>)
	public:
		moXMLData(xml_type_t type, const moWCString& name, const moWCString& tag_name = "");

		virtual void		ToString(moWCString& string) const;
		void			SetData(const moWCString& data);
		const moWCString&	GetData(void) const;
	private:
		moWCString		f_data;
	};
	class MO_DLL_EXPORT moXMLTag : public moXMLType
	{
	// used for:
	//	Opening Tags
	//	Empty Tags
	public:
		moXMLTag(const moWCString& name, const moWCString& tag_name = "");

		virtual void		ToString(moWCString& string) const;
		moVariableSPtr		Get(const moWCString& name) const;
		moVariableSPtr		Set(const moVariable& var);
		const moVariableList&	GetList(void) const;
		moVariableList&		GetList(void);
		void			MarkAsEmpty(bool empty = true);
		bool			IsEmpty(void) const;
	private:
		moVariableList		f_attributes;
		bool			f_empty;
	};

	class MO_DLL_EXPORT moXMLEvent : public moBase
	{
	public:
		static const int	XML_NO_PRIORITY = INT_MIN;

					moXMLEvent(int priority = 0, const moWCString& name = "");
		virtual			~moXMLEvent();

		virtual bool		OnEvent(moXMLParser& parser, moXMLType& type);
		virtual compare_t	Compare(const moBase& object) const;

		void			Warning(moXMLParser& parser, moXMLParser::moXMLTag& type, moTextStream::errcode_t warcode, const char *format, ...);
		void			Error(moXMLParser& parser, moXMLParser::moXMLTag& type, moTextStream::errcode_t errcode, const char *format, ...);

	private:
		const int		f_priority;
		const moWCString	f_name;
	};
	typedef moTmplList<moXMLEvent, moSortedList> moSortedListOfXMLEvent;

	// NOTE: you cannot define an element more than once
	class MO_DLL_EXPORT moXMLElement;
	typedef moSmartPtr<moXMLElement>	moXMLElementSPtr;
	typedef moTmplList<moXMLElement, moSortedListUnique> moSortedListUniqueOfXMLElements;
	class MO_DLL_EXPORT moXMLElement : public moBase
	{
	public:
		class moXMLEntry;
		typedef moTmplList<moXMLEntry, moList> moListOfXMLEntries;
		typedef moTmplList<moXMLEntry, moSortedListUnique> moSortedListUniqueOfXMLEntries;
		class moXMLEntry : public moBase
		{
		public:
			enum type_t {
				ENTRY_EMPTY,
				ENTRY_ANY,
				ENTRY_PCDATA,
				ENTRY_NAME,
				ENTRY_CHOICE,
				ENTRY_SEQUENCE,
				ENTRY_UNORDERED
			};
			enum repeat_t {
				REPEAT_ONCE,		// nothing
				REPEAT_ANY,		// '*'
				REPEAT_ONE_OR_MORE,	// '+'
				REPEAT_ZERO_OR_ONE	// '?'
			};

						moXMLEntry(void);
			virtual			~moXMLEntry();

			void			SetType(type_t type);
			void			SetRepeat(repeat_t repeat);
			void			SetName(const moWCString& name);

			type_t			GetType(void) const;
			repeat_t		GetRepeat(void) const;
			const moWCString&	GetName(void) const;
			bool			HasPCDATA(void) const;

			unsigned long		Count(void) const;
			moXMLEntry *		Get(int idx) const;
			void			AddSubEntry(moXMLEntry *entry);

			// sorted by names when testing for uniqueness (in choices)
			virtual compare_t	Compare(const moBase& object) const;

		private:
			zbool_t			f_has_pcdata;
			type_t			f_type;
			repeat_t		f_repeat;
			moWCStringSPtr		f_name;
			moListOfXMLEntries	f_sub_entries;
		};
		typedef moSmartPtr<moXMLEntry>		moXMLEntrySPtr;

		// NOTE: the same attribute can be defined multiple times,
		//	 but only the first definition will be kept
		//	 (we can warn about it but it isn't an error)
		class moXMLAttribute : public moBase
		{
		public:
			enum type_t {
				ATTRIBUTE_CDATA,
				ATTRIBUTE_ID,
				ATTRIBUTE_IDREF,
				ATTRIBUTE_IDREFS,
				ATTRIBUTE_ENTITY,
				ATTRIBUTE_ENTITIES,
				ATTRIBUTE_NMTOKEN,
				ATTRIBUTE_NMTOKENS,
				ATTRIBUTE_NOTATION,
				ATTRIBUTE_ENUMERATION,
				ATTRIBUTE_NAME,		// for HTML
				ATTRIBUTE_NUMBER	// for HTML
			};
			enum value_t {
				VALUE_REQUIRED,		// user must specify this attribute
				VALUE_IMPLIED,		// some internal default
				VALUE_FIXED,		// a string which cannot be changed
				VALUE_DEFAULT,		// a default string
				VALUE_RECTANGLE,	// for HTML
				VALUE_TEXT,		// for HTML
				VALUE_SUBMIT,		// for HTML
				VALUE_GET		// for HTML
			};

						moXMLAttribute(const moWCString& name);
			virtual			~moXMLAttribute();

			void			SetType(type_t type);
			void			SetValueType(value_t value_type);
			bool			AddUserTypeName(const moWCString& name);
			void			SetDefaultValue(const moWCString& value);

			const moWCString&	GetName(void) const;
			type_t			GetType(void) const;
			value_t			GetValueType(void) const;
			unsigned long		Count(void) const;
			const moWCString&	GetUserTypeName(int idx) const;
			const moWCString&	GetDefault(void) const;

			// sorted by names
			virtual compare_t	Compare(const moBase& object) const;

		private:
			type_t				f_type;
			value_t				f_value_type;
			const moWCString		f_name;
			moSortedListUniqueOfWCStrings	f_user_types;	// enumerations and notations
			moWCString			f_default;
		};
		typedef moSmartPtr<moXMLAttribute>	moXMLAttributeSPtr;
		typedef moTmplList<moXMLAttribute, moSortedListUnique> moSortedListUniqueOfXMLAttributes;

					moXMLElement(const moWCString& name);
		virtual			~moXMLElement();

		void			SetEntry(moXMLEntry *entry);
		void			SetExceptEntry(moXMLEntry *entry);
		void			SetAdditionalEntry(moXMLEntry *entry);
		moXMLEntrySPtr		GetEntry(void) const;
		moXMLEntrySPtr		GetExceptEntry(void) const;
		moXMLEntrySPtr		GetAdditionalEntry(void) const;
		const moWCString&	GetName(void) const;
		bool			AddAttribute(moXMLAttribute *attr);
		unsigned long		AttributeCount(void) const;
		moXMLAttribute *	GetAttribute(int idx) const;
		bool			AddEntryName(const moWCString& name);
		unsigned long		EntryNameCount(void) const;
		const moWCString&	GetEntryName(int idx) const;
		void			CopyEntriesFrom(const moXMLElement& element);
		void			SetOptionalTags(bool start, bool end);
		bool			IsStartTagOptional(void) const;
		bool			IsEndTagOptional(void) const;
		bool			IsParentOf(const moWCString& child_name) const;

		// sorted by names
		virtual compare_t	Compare(const moBase& object) const;

	private:
		const moWCString		f_name;
		moXMLEntrySPtr			f_entries;		// (A)<repeat>
		moXMLEntrySPtr			f_except_entries;	// -(A) [html]
		moXMLEntrySPtr			f_additional_entries;	// +(A) [html]
		moSortedListUniqueOfXMLAttributes f_attlist;
		moSortedListUniqueOfWCStrings	f_entry_names;	// list of names (unique)
		zbool_t				f_optional_start;
		zbool_t				f_optional_end;
	};

	class MO_DLL_EXPORT moXMLEntity : public moBase
	{
	public:
		enum type_t {
			ENTITY_GENERAL,
			ENTITY_DTD
		};

					moXMLEntity(const moWCString& name, type_t type = ENTITY_GENERAL);
		virtual			~moXMLEntity();

		void			SetValue(const moWCString& value);
		void			SetPublicID(const moWCString& public_id);
		void			SetSystemID(const moWCString& system_id);

		type_t			GetType(void) const;
		const moWCString&	GetName(void) const;
		const moWCString&	GetValue(void) const;
		const moWCString&	GetPublicID(void) const;
		const moWCString&	GetSystemID(void) const;

		// sorted by names
		virtual compare_t	Compare(const moBase& object) const;

	private:
		type_t			f_type;
		moWCString		f_name;
		moWCString		f_value;
		moWCString		f_public_id;
		moWCString		f_system_id;
	};
	typedef moSmartPtr<moXMLEntity>		moXMLEntitySPtr;
	typedef moTmplList<moXMLEntity, moSortedListUnique> moSortedListUniqueOfXMLEntities;


				// WARNING: the XML stream objects are AddRef()'d and Release()'d
				//	    by the XML object
				moXMLParser(moXMLStream& xml_stream);
	virtual			~moXMLParser();

	// user supplied syntax -- ignored when the input file includes
	// a syntax (DTD) of its own
	bool			SetSyntax(const moWCString& dtd);
	bool			SetHTMLSyntax(bool strict = false);
	bool			SetVersion(unsigned long version);
	bool			SetInputEncoding(const moWCString& encoding);

	const moWCString&	Encoding(void) const;
	const moWCString&	Syntax(void) const;
	unsigned long		Version(void) const;
	void			RegisterEventHandler(xml_type_t type, moXMLEvent& event);

	// read the next entry (tag + parameter, data, end tag, comment...)
	bool			ReadNext(moXMLTypeSPtr& data, bool delete_signals = false);
	bool			ReadNextNoSignal(moXMLTypeSPtr& data);
	bool			ReadNextBlock(moList::position_t& from, moList::position_t& to, bool emit_signals = false, moWCString *str = 0);
	void			BlockToString(moWCString& str, moList::position_t from, moList::position_t to);
	moXMLTypeSPtr		GetData(moList::position_t pos) const;
	//void			PushSTag(void);
	//void			PopSTag(void);

	size_t			GetPosition(void) const;
	void			SetPosition(size_t position);

	// Include() changes the current input stream to the new one specified.
	//
	// When the end of a stream is reached, the parser comes back to the
	// previous XML stream.
	//
	// WARNING: the XML stream objects are AddRef()'d and Release()'d
	//	    by the XML object
	void			Include(moXMLStream& input, const moWCString& type = "text/xml");

	static unsigned long	XMLCType(mowc::wc_t c);
	static bool		IsXMLNameChar(mowc::wc_t c, bool first = false);
	static bool		IsXMLSpace(mowc::wc_t c);

	void			AddCDataTag(const moWCString& name);
	bool			ReturnComments(bool status = true);
	bool			KeepEntities(bool status = true);
	bool			ReadDTD(bool status = true);

	bool			UseDTD(bool use);
	const moSortedListUniqueOfXMLElements&	DTDGetElements(void) const;
	const moSortedListUniqueOfXMLEntities&	DTDGetEntities(void) const;
	const moSortedListUniqueOfXMLEntities&	DTDGetParsedEntities(void) const;	// these are rather useless once the DTD was parsed...
	bool			IsParentOf(const moXMLElement *element, const moWCString& name) const;

	void			AppendVariableList(const moVariableList& varlist);
	moVariableSPtr		SetVariable(const moVariable& var);
	moVariableSPtr		SetVariable(const moWCString& set);
	moVariableSPtr		SetVariable(const moWCString& name, const moWCString& value);
	moVariableSPtr		GetVariable(const moWCString& name) const;
	void			UnsetVariable(const moWCString& name);
	moXMLStreamSPtr		FindStream(const moWCString& name, moWCString& var_name, moList::position_t &pos) const;
	moXMLStreamSPtr		InputStream(void) const;

	void			FormatMessage(const char *type, const char *format, ...);
	void			VFormatMessage(const char *type, const char *format, va_list args);
	void			FormatWarning(moTextStream::errcode_t warcode, const char *format, ...);
	void			VFormatWarning(moTextStream::errcode_t warcode, const char *format, va_list args);
	void			FormatError(moTextStream::errcode_t errcode, const char *format, ...);
	void			VFormatError(moTextStream::errcode_t errcode, const char *format, va_list args);

private:
	// the xml_get_func type is used to defined the
	// state of the parser (i.e. what do we do next
	// on a ReadNext() call); these functions must
	// return on of the XML_RT_... values and change
	// the state appropriatly
	typedef int (moXMLParser::*xml_get_func_t)(void);
#if 0
	struct xml_tag_pos_t {
		moXMLTag *		f_tag;
		moList::position_t	f_pos;
	};
#endif

	// spaces rules for GetName() [and maybe others later?]
	enum spaces_t {
		SPACES_RELAX,		// can have spaces unless in strict mode
		SPACES_NONE,		// never accept spaces
		SPACES_ANY,		// any number of spaces is fine
		SPACES_ONE_OR_MORE	// we need to have at least one space
	};

	static const int	XML_RT_EOF		= -1;
	static const int	XML_RT_FATAL		= -2;
	static const int	XML_RT_ERROR		= -3;	// we can recover from this one
	static const int	XML_RT_NOERROR		=  0;

	void			Strict(bool strict = true);	// false by default (HTML like)
	void			Init(moXMLStream *xml_stream = 0);
	void			InitStream(moXMLStream *xml_stream);
	int			PopInclude(void);
	//virtual void		OnNewStream(int which);
	void			Clean(void);
	void			CleanSyntax(void);

	void			InitDocument(void);	// call this to start the parser
	void			Push(xml_get_func_t func);
	void			Pop(void);
	bool			Signal(moXMLType& data);

	int			InputNextLine(void);
	mowc::wc_t		InputGetC(void);

	int			GetAnySubset(void);
	int			GetAttListDecl(void);
	int			GetCDSect(void);
	int			GetChildren(moXMLElement *element, moXMLElement::moXMLEntry *entry);
	int			GetComment(void);
	int			GetConditionalSect(void);
	int			GetContent(void);
	int			GetDeclNames(moSortedListUniqueOfWCStrings& decl_names, const char *decl);
	int			GetDoctypeDecl(void);
	int			GetElement(void);
	int			GetElementDecl(void);
	int			GetEntityDecl(void);
	int			GetETag(void);
	int			GetIntSubset(void);
	int			GetMisc(void);
	int			GetNotationDecl(void);
	int			GetPEReference(bool skip_spaces, bool add_spaces, bool loop);
	int			GetPI(void);
	int			GetPlainContent(void);
	moXMLElement::moXMLEntry::repeat_t GetRepeat(moXMLElement::moXMLEntry *entry, bool accept_spaces = true);
	int			GetSTag(void);
	int			GetTagCData(void);
	int			GetXMLDecl(void);

	int			GetAttribute(moVariableSPtr& attr, bool ref = true);
	mowc::wc_t		GetName(spaces_t expect_spaces = SPACES_RELAX);
	mowc::wc_t		GetReference(void);
	mowc::wc_t		GetWord(unsigned long first, unsigned long others = 0);
	mowc::wc_t		TestWord(const moWCString& str, unsigned long first, unsigned long others);
	bool			IsCDataTag(const moXMLTag& tag) const;

	mowc::wc_t		XMLGetC(void);
	mowc::wc_t		XMLSkipSpaces(void);

	static const char *	g_html_dtd;
	static const char *	g_html_loose_dtd;

	moWCString		f_name;			// the tags full path (i.e. HTML/BODY/TABLE/TR/TD/BR)
	moArray			f_stack;		// an array of functions to call (a bit like a processor stack)

	zbool_t			f_strict;		// whether any mistake is an error
	zbool_t			f_return_comments;	// useful to return commented scripts
	zbool_t			f_keep_entities;	// whether &<name>; is converted or not
	zbool_t			f_standalone;		// whether there won't be any external DTD specification
	zbool_t			f_running;		// when within ReadNext() this is true
	zbool_t			f_read_dtd;		// if possible, read the DTD
	zbool_t			f_use_dtd;		// whether the DTD is checked for validity
	zint32_t		f_internal_dtd;		// in strict mode, an internal DTD can't include conditionals
	zuint32_t		f_version;		// 1.0 or 1.1 in 16.16 fixed (Default to 0 -- undefined)
	moWCString		f_encoding;		// character set encoding
	moWCString		f_current_syntax;	// <[DOCTYPE 'name' [ ...]]>
	moWCString		f_current_identification; // the public identifier
	moWCString		f_dtd_url;		// a URL to this document DTD
	moSortedListUniqueOfXMLElements	f_dtd_elements;		// the <!ELEMENT ...> entries and their corresponding <!ATTLIST ...>
	moSortedListUniqueOfXMLEntities	f_dtd_entities;		// the <!ENTITY name ...> entries
	moSortedListUniqueOfXMLEntities	f_dtd_parsed_entities;	// the <!ENTITY % name ...> entries
	zint32_t		f_cond_include;		// in a DTD, we are in this many <![INCLUDE[ ... so we have to find that many ]]>

	moXMLStreamSPtr		f_input;		// the current input stream
	moListOfXMLStreams	f_input_streams;	// input streams stacked when an include is found
	moListOfXMLStreams	f_old_input_streams;	// old input streams to delete on exit (so we can have references to them while working)

	moListOfXMLType		f_data;			// all of the data read in the XML file
	moSortedListOfXMLEvent	f_events[XML_TYPE_max];	// array of moXMLEvent depending on the type

	moSortedListUniqueOfWCStrings f_cdata_tags;	// names of tags which content is defined as CDATA (such as <script> and <style> for HTML)
};

typedef moSmartPtr<moXMLParser>		moXMLParserSPtr;


};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_XML_H

