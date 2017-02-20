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
#pragma implementation "mo/mo_xml.h"
#endif

#include	"mo/mo_xml.h"

#ifndef MO_FILE_H
#include	"mo/mo_file.h"
#endif
#ifndef MO_MEMFILE_H
#include	"mo/mo_memfile.h"
#endif
#ifndef MO_AUTO_RESTORE_H
#include	"mo/mo_auto_restore.h"
#endif


namespace molib
{


#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif



/** \class moXMLParser
 *
 * \brief The XML parser used to read and execute XML files.
 *
 * This XML parser implementation was created in order to read
 * XML file one item at a time. The main function is called
 * ReadNext() and it reads one element such as a tag opening,
 * the content of a tag, a CDATA block, a PI target, etc.
 * Repeat the calls until you get an EOF to read the entire
 * file. This mechanism is very useful to read an XML file in
 * a process used between an input and output pipe.
 *
 * The following is a quick reference to the
 * XML recommendation. Its aim is to help developing this
 * library in order to read valid XML documents following
 * the strict rules set forth by the W3C consortium.
 * 
 * Any data being returned which still includes line separators
 * will only include "\n"/0x0A ($2.11). "\r"/0x0D characters are
 * either removed or converted as required.
 * 
 * See the web site here: http://www.w3.org/TR/REC-xml
 *
 * Note that some entries have been enhanced to support the HTML
 * DTD definitions. These include extraneous information such
 * as whether the open and close tags are optional and comments
 * within elements, entities and attribute lists. The DTD reader
 * also supports multiple element or attribute names at once.
 * 
 * \code
 *	[ 1]	Document	::=	Prolog Element Misc*
 *	[ 2]	Char		::=	#x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
 *	[ 3]	S		::=	(#x20 | #x9 | #xD | #xA)+
 *	[ 4]	NameChar	::=	Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
 *	[ 5]	Name		::=	(Letter | '_' | ':') (NameChar)*
 *	[ 6]	Names		::=	Name (#x20 Name)*
 *	[ 7]	Nmtoken		::=	(NameChar)+
 *	[ 8]	Nmtokens	::=	Nmtoken (#x20 Nmtoken)*
 *	[ 9]    EntityValue	::=	'"' ([^%&"] | PEReference | Reference)* '"' |  "'" ([^%&']
 *								| PEReference | Reference)* "'"
 *	[10]	AttValue	::=	'"' ([^<&"] | Reference)* '"' |  "'" ([^<&'] | Reference)* "'"
 *	[11]	SystemLiteral	::=	('"' [^"]* '"') | ("'" [^']* "'")
 *	[12]	PubidLiteral	::=	'"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
 *	[13]	PubidChar	::=	#x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
 *	[14]	CharData	::=	[^<&]* - ([^<&]* ']]>' [^<&]*)
 *	[15]	Comment		::=	'<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
 *	[16]	PI		::=	'<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
 *	[17]	PITarget	::=	Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
 *	[18]	CDSect		::=	CDStart CData CDEnd
 *	[19]	CDStart		::=	'<![CDATA['
 *	[20]	CData		::=	(Char* - (Char* ']]>' Char*))
 *	[21]	CDEnd		::=	']]>'
 *	[22]	Prolog		::=	XMLDecl? Misc* (DoctypeDecl Misc*)?
 *			Note: the XMLDecl is necessary if the file is a standalone
 *			      since only an external informational file can otherwise
 *			      properly declare the XML version and encoding.
 *	[23]	XMLDecl		::=	'<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
 *	[24]	VersionInfo	::=	S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"')
 *	[25]	Eq		::=	S? '=' S?
 *	[26]	VersionNum	::=	'1.0'
 *			-- was "([a-zA-Z0-9_.:] | '-')+" but these rules are anyway specific
 *			-- to "1.0" and thus the version must be 1.0 in this case
 *	[27]	Misc		::=	Comment | PI | S
 *	[28]	DoctypeDecl	::=	'<!DOCTYPE' S Name (S ExternalID)? S? ('[' IntSubset ']' S?)? '>'
 *	[28a]	DeclSep		::=	PEReference | S
 *	[28b]	IntSubset	::=	(MarkupDecl | DeclSep)*
 *	[29]	MarkupDecl	::=	ElementDecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment
 *	[30]	ExtSubset	::=	TextDecl? ExtSubsetDecl
 *	[31]	ExtSubsetDecl	::=	( MarkupDecl | ConditionalSect | DeclSep)*
 *	[32]	SDDecl		::=	S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
 *	[33-38]	<removed>
 *	[39]	Element		::=	EmptyElemTag | STag Content ETag
 *	[40]	STag		::=	'<' Name (S Attribute)* S? '>'
 *	[41]	Attribute	::=	Name Eq AttValue
 *			-- NOTE: we really accept the SGML attribute syntax:
 *							Name (Eq AttValue)?
 *	[42]	ETag		::=	'</' Name S? '>'
 *	[43]	Content		::=	CharData? ((Element | Reference | CDSect | PI | Comment) CharData?)*
 *	[44]	EmptyElemTag	::=	'<' Name (S Attribute)* S? '/>'
 *	[45]	ElementDecl	::=	'<!ELEMENT' S Name S ContentSpec S? '>'
 *	[46]	ContentSpec	::=	'EMPTY' | 'ANY' | Mixed | Children
 *	[47]	Children	::=	(Choice | Seq) ('?' | '*' | '+')?
 *	[48]	CP		::=	(Name | Choice | Seq) ('?' | '*' | '+')?
 *	[49]	Choice		::=	'(' S? CP ( S? '|' S? CP )+ S? ')'
 *	[50]	Seq		::=	'(' S? CP ( S? ',' S? CP )* S? ')'
 *	[51]	Mixed		::=	'(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')'
 *	[52]	AttlistDecl	::=	'<!ATTLIST' S Name AttDef* S? '>'
 *	[53]	AttDef		::=	S Name S AttType S DefaultDecl
 *	[54]    AttType		::=	StringType | TokenizedType | EnumeratedType
 *	[55]    StringType	::=	'CDATA'
 *	[56]    TokenizedType	::=	'ID' | 'IDREF' | 'IDREFS' | 'ENTITY' | 'ENTITIES'
 *							| 'NMTOKEN' | 'NMTOKENS'
 *	[57]	EnumeratedType	::=	NotationType | Enumeration
 *	[58]    NotationType    ::=	'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
 *	[59]	Enumeration	::=	'(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'
 *	[60]	DefaultDecl	::=	'#REQUIRED' | '#IMPLIED' | (('#FIXED' S)? AttValue)
 *	[61]	ConditionalSect	::=	IncludeSect | IgnoreSect
 *	[62]	IncludeSect	::=	'<![' S? 'INCLUDE' S? '[' ExtSubsetDecl ']]>'
 *	[63]	IgnoreSect	::=	'<![' S? 'IGNORE' S? '[' IgnoreSectContents* ']]>'
 *	[64]	IgnoreSectContents::=	Ignore ('<![' IgnoreSectContents ']]>' Ignore)*
 *	[65]	Ignore		::=	Char* - (Char* ('<![' | ']]>') Char*)
 *	[66]	CharRef		::=	'&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
 *	[67]	Reference	::=	EntityRef | CharRef
 *	[68]	EntityRef	::=	'&' Name ';'
 *	[69]	PEReference	::=	'%' Name ';'
 *	[70]	EntityDecl	::=	GEDecl | PEDecl
 *	[71]	GEDecl		::=	'<!ENTITY' S Name S EntityDef S? '>'
 *	[72]	PEDecl		::=	'<!ENTITY' S '%' S Name S PEDef S? '>'
 *	[73]	EntityDef	::=	EntityValue | (ExternalID NDataDecl?)
 *	[74]	PEDef		::=	EntityValue | ExternalID
 *	[75]    ExternalID	::=	('SYSTEM' | PublicID) S SystemLiteral
 *	[76]	NDataDecl	::=	S 'NDATA' S Name
 *	[77]	TextDecl	::=	'<?xml' VersionInfo? EncodingDecl S? '?>'
 *	[78]	ExtParsedEnt	::=	TextDecl? content
 *	[79]	???
 *	[80]	EncodingDecl	::=	S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" )
 *	[81]	EncName		::=	[A-Za-z] ([A-Za-z0-9._] | '-')*
 *	[82]	NotationDecl	::=	'<!NOTATION' S Name S (ExternalID | PublicID) S? '>'
 *	[83]	PublicID	::=	'PUBLIC' S PubidLiteral
 *	[84]	Letter		::=	BaseChar | Ideographic
 *	[85]	BaseChar	::=	[#x0041-#x005A] | [#x0061-#x007A] | [#x00C0-#x00D6]
 *					| [#x00D8-#x00F6] | [#x00F8-#x00FF] | [#x0100-#x0131]
 *					| [#x0134-#x013E] | [#x0141-#x0148] | [#x014A-#x017E]
 *					| [#x0180-#x01C3] | [#x01CD-#x01F0] | [#x01F4-#x01F5]
 *					| [#x01FA-#x0217] | [#x0250-#x02A8] | [#x02BB-#x02C1]
 *					| #x0386 | [#x0388-#x038A] | #x038C | [#x038E-#x03A1]
 *					| [#x03A3-#x03CE] | [#x03D0-#x03D6] | #x03DA | #x03DC
 *					| #x03DE | #x03E0 | [#x03E2-#x03F3] | [#x0401-#x040C]
 *					| [#x040E-#x044F] | [#x0451-#x045C] | [#x045E-#x0481]
 *					| [#x0490-#x04C4] | [#x04C7-#x04C8] | [#x04CB-#x04CC]
 *					| [#x04D0-#x04EB] | [#x04EE-#x04F5] | [#x04F8-#x04F9]
 *					| [#x0531-#x0556] | #x0559 | [#x0561-#x0586]
 *					| [#x05D0-#x05EA] | [#x05F0-#x05F2] | [#x0621-#x063A]
 *					| [#x0641-#x064A] | [#x0671-#x06B7] | [#x06BA-#x06BE]
 *					| [#x06C0-#x06CE] | [#x06D0-#x06D3] | #x06D5
 *					| [#x06E5-#x06E6] | [#x0905-#x0939] | #x093D
 *					| [#x0958-#x0961] | [#x0985-#x098C] | [#x098F-#x0990]
 *					| [#x0993-#x09A8] | [#x09AA-#x09B0] | #x09B2
 *					| [#x09B6-#x09B9] | [#x09DC-#x09DD] | [#x09DF-#x09E1]
 *					| [#x09F0-#x09F1] | [#x0A05-#x0A0A] | [#x0A0F-#x0A10]
 *					| [#x0A13-#x0A28] | [#x0A2A-#x0A30] | [#x0A32-#x0A33]
 *					| [#x0A35-#x0A36] | [#x0A38-#x0A39] | [#x0A59-#x0A5C]
 *					| #x0A5E | [#x0A72-#x0A74] | [#x0A85-#x0A8B] | #x0A8D
 *					| [#x0A8F-#x0A91] | [#x0A93-#x0AA8] | [#x0AAA-#x0AB0]
 *					| [#x0AB2-#x0AB3] | [#x0AB5-#x0AB9] | #x0ABD | #x0AE0
 *					| [#x0B05-#x0B0C] | [#x0B0F-#x0B10] | [#x0B13-#x0B28]
 *					| [#x0B2A-#x0B30] | [#x0B32-#x0B33] | [#x0B36-#x0B39]
 *					| #x0B3D | [#x0B5C-#x0B5D] | [#x0B5F-#x0B61]
 *					| [#x0B85-#x0B8A] | [#x0B8E-#x0B90] | [#x0B92-#x0B95]
 *					| [#x0B99-#x0B9A] | #x0B9C | [#x0B9E-#x0B9F]
 *					| [#x0BA3-#x0BA4] | [#x0BA8-#x0BAA] | [#x0BAE-#x0BB5]
 *					| [#x0BB7-#x0BB9] | [#x0C05-#x0C0C] | [#x0C0E-#x0C10]
 *					| [#x0C12-#x0C28] | [#x0C2A-#x0C33] | [#x0C35-#x0C39]
 *					| [#x0C60-#x0C61] | [#x0C85-#x0C8C] | [#x0C8E-#x0C90]
 *					| [#x0C92-#x0CA8] | [#x0CAA-#x0CB3] | [#x0CB5-#x0CB9]
 *					| #x0CDE | [#x0CE0-#x0CE1] | [#x0D05-#x0D0C]
 *					| [#x0D0E-#x0D10] | [#x0D12-#x0D28] | [#x0D2A-#x0D39]
 *					| [#x0D60-#x0D61] | [#x0E01-#x0E2E] | #x0E30
 *					| [#x0E32-#x0E33] | [#x0E40-#x0E45] | [#x0E81-#x0E82]
 *					| #x0E84 | [#x0E87-#x0E88] | #x0E8A | #x0E8D
 *					| [#x0E94-#x0E97] | [#x0E99-#x0E9F] | [#x0EA1-#x0EA3]
 *					| #x0EA5 | #x0EA7 | [#x0EAA-#x0EAB] | [#x0EAD-#x0EAE]
 *					| #x0EB0 | [#x0EB2-#x0EB3] | #x0EBD | [#x0EC0-#x0EC4]
 *					| [#x0F40-#x0F47] | [#x0F49-#x0F69] | [#x10A0-#x10C5]
 *					| [#x10D0-#x10F6] | #x1100 | [#x1102-#x1103]
 *					| [#x1105-#x1107] | #x1109 | [#x110B-#x110C]
 *					| [#x110E-#x1112] | #x113C | #x113E | #x1140 | #x114C
 *					| #x114E | #x1150 | [#x1154-#x1155] | #x1159
 *					| [#x115F-#x1161] | #x1163 | #x1165 | #x1167 | #x1169
 *					| [#x116D-#x116E] | [#x1172-#x1173] | #x1175 | #x119E
 *					| #x11A8 | #x11AB | [#x11AE-#x11AF] | [#x11B7-#x11B8]
 *					| #x11BA | [#x11BC-#x11C2] | #x11EB | #x11F0 | #x11F9
 *					| [#x1E00-#x1E9B] | [#x1EA0-#x1EF9] | [#x1F00-#x1F15]
 *					| [#x1F18-#x1F1D] | [#x1F20-#x1F45] | [#x1F48-#x1F4D]
 *					| [#x1F50-#x1F57] | #x1F59 | #x1F5B | #x1F5D
 *					| [#x1F5F-#x1F7D] | [#x1F80-#x1FB4] | [#x1FB6-#x1FBC]
 *					| #x1FBE | [#x1FC2-#x1FC4] | [#x1FC6-#x1FCC]
 *					| [#x1FD0-#x1FD3] | [#x1FD6-#x1FDB] | [#x1FE0-#x1FEC]
 *					| [#x1FF2-#x1FF4] | [#x1FF6-#x1FFC] | #x2126
 *					| [#x212A-#x212B] | #x212E | [#x2180-#x2182]
 *					| [#x3041-#x3094] | [#x30A1-#x30FA] | [#x3105-#x312C]
 *					| [#xAC00-#xD7A3]
 *	[86]	Ideographic	::=	[#x4E00-#x9FA5] | #x3007 | [#x3021-#x3029]
 *	[87]	CombiningChar	::=	[#x0300-#x0345] | [#x0360-#x0361] | [#x0483-#x0486]
 *					| [#x0591-#x05A1] | [#x05A3-#x05B9] | [#x05BB-#x05BD]
 *					| #x05BF | [#x05C1-#x05C2] | #x05C4 | [#x064B-#x0652]
 *					| #x0670 | [#x06D6-#x06DC] | [#x06DD-#x06DF]
 *					| [#x06E0-#x06E4] | [#x06E7-#x06E8] | [#x06EA-#x06ED]
 *					| [#x0901-#x0903] | #x093C | [#x093E-#x094C] | #x094D
 *					| [#x0951-#x0954] | [#x0962-#x0963] | [#x0981-#x0983]
 *					| #x09BC | #x09BE | #x09BF | [#x09C0-#x09C4]
 *					| [#x09C7-#x09C8] | [#x09CB-#x09CD] | #x09D7
 *					| [#x09E2-#x09E3] | #x0A02 | #x0A3C | #x0A3E | #x0A3F
 *					| [#x0A40-#x0A42] | [#x0A47-#x0A48] | [#x0A4B-#x0A4D]
 *					| [#x0A70-#x0A71] | [#x0A81-#x0A83] | #x0ABC
 *					| [#x0ABE-#x0AC5] | [#x0AC7-#x0AC9] | [#x0ACB-#x0ACD]
 *					| [#x0B01-#x0B03] | #x0B3C | [#x0B3E-#x0B43]
 *					| [#x0B47-#x0B48] | [#x0B4B-#x0B4D] | [#x0B56-#x0B57]
 *					| [#x0B82-#x0B83] | [#x0BBE-#x0BC2] | [#x0BC6-#x0BC8]
 *					| [#x0BCA-#x0BCD] | #x0BD7 | [#x0C01-#x0C03]
 *					| [#x0C3E-#x0C44] | [#x0C46-#x0C48] | [#x0C4A-#x0C4D]
 *					| [#x0C55-#x0C56] | [#x0C82-#x0C83] | [#x0CBE-#x0CC4]
 *					| [#x0CC6-#x0CC8] | [#x0CCA-#x0CCD] | [#x0CD5-#x0CD6]
 *					| [#x0D02-#x0D03] | [#x0D3E-#x0D43] | [#x0D46-#x0D48]
 *					| [#x0D4A-#x0D4D] | #x0D57 | #x0E31 | [#x0E34-#x0E3A]
 *					| [#x0E47-#x0E4E] | #x0EB1 | [#x0EB4-#x0EB9]
 *					| [#x0EBB-#x0EBC] | [#x0EC8-#x0ECD] | [#x0F18-#x0F19]
 *					| #x0F35 | #x0F37 | #x0F39 | #x0F3E | #x0F3F
 *					| [#x0F71-#x0F84] | [#x0F86-#x0F8B] | [#x0F90-#x0F95]
 *					| #x0F97 | [#x0F99-#x0FAD] | [#x0FB1-#x0FB7] | #x0FB9
 *					| [#x20D0-#x20DC] | #x20E1 | [#x302A-#x302F] | #x3099
 *					| #x309A
 *	[88]	Digit		::=	[#x0030-#x0039] | [#x0660-#x0669] | [#x06F0-#x06F9]
 *					| [#x0966-#x096F] | [#x09E6-#x09EF] | [#x0A66-#x0A6F]
 *					| [#x0AE6-#x0AEF] | [#x0B66-#x0B6F] | [#x0BE7-#x0BEF]
 *					| [#x0C66-#x0C6F] | [#x0CE6-#x0CEF] | [#x0D66-#x0D6F]
 *					| [#x0E50-#x0E59] | [#x0ED0-#x0ED9] | [#x0F20-#x0F29]
 *	[89]	Extender	::=	#x00B7 | #x02D0 | #x02D1 | #x0387 | #x0640 | #x0E46
 *					| #x0EC6 | #x3005 | [#x3031-#x3035] | [#x309D-#x309E]
 *					| [#x30FC-#x30FE]
 *	
 *	-- added later (2003-03-26)
 *	[XX]	InvalidChar	::=	[#x7F-#x84] | [#x86-#x9F] | [#xFDD0-#xFDDF]
 *					| [#1FFFE-#x1FFFF] | [#2FFFE-#x2FFFF] | [#3FFFE-#x3FFFF]
 *					| [#4FFFE-#x4FFFF] | [#5FFFE-#x5FFFF] | [#6FFFE-#x6FFFF]
 *					| [#7FFFE-#x7FFFF] | [#8FFFE-#x8FFFF] | [#9FFFE-#x9FFFF]
 *					| [#AFFFE-#xAFFFF] | [#BFFFE-#xBFFFF] | [#CFFFE-#xCFFFF]
 *					| [#DFFFE-#xDFFFF] | [#EFFFE-#xEFFFF] | [#FFFFE-#xFFFFF]
 *					| [#10FFFE-#x10FFFF]
 *	
 *	special attributes:
 *		(ATTLIST)	xml:space (default|preserve) 'default'
 *		(ATTLIST)	xml:lang CDATA #IMPLIED
 *		(TAG)		xml:lang="<normalized language encoding>"
 *	
 *	predefined entities:
 *		&amp;
 *		&apos;
 *		&gt;
 *		&lt;
 *		&quot;
 * \endcode
 */




/** \brief The XML parser constructor
 *
 * This function initializes the XML parser with the specified stream
 * as input stream.
 *
 * Once the constructor was called, the parser is ready to be used to
 * read the XML input stream. However, there are a certain number of
 * functions that need to be called before ReadNext() to properly
 * initialize the parser.
 *
 * \param[in] xml_stream The top-most input stream.
 */
moXMLParser::moXMLParser(moXMLStream& xml_stream)
	: f_stack(sizeof(xml_get_func_t))
{
	Init(&xml_stream);
}


/** \brief Initializes the XML parser
 *
 * This function initializes the XML parser. It is then ready for the
 * ReadNext() function to be called.
 *
 * \param[in] xml_stream The top-most XML stream to use as input
 */
void moXMLParser::Init(moXMLStream *xml_stream)
{
	f_name.Empty();		// no syntax name by default--we expect that to be defined in the input file
	//f_stack.Empty(); -- auto-init and initialized in InitDocument() called below

	//f_strict = false; -- auto-init
	//f_return_comments = false; -- auto-init
	//f_keep_entities = false; -- auto-init
	//f_standalone = false; -- auto-init
	//f_running = false; -- auto-init
	//f_read_dtd = false; -- auto-init
	//f_internal_dtd; --auto-init
	//f_version = 0; -- auto-init		// 0 means undefined!
	//f_encoding.Empty(); -- automatic and cleared in Clean()
	//f_current_syntax.Empty(); -- automatic and cleared in Clean()
	//f_current_identification.Empty() -- automatic and cleared in Clean()
	//f_dtd_url.Empty(); -- automatic and cleared in Clean()
	//f_dtd_elements.Empty() -- automatic and cleared in Clean()
	//f_dtd_entities.Empty() -- automatic and cleared in Clean()
	//f_dtd_parsed_entities.Empty() -- automatic and cleared in Clean()
	//f_cond_include -- automatic and cleared in Clean()

	// f_input = 0 -- done in InitStream()
	InitStream(xml_stream);
	//f_input_streams = ... -- automatic and cleared in Clean()
	//f_old_input_streams = ... -- automatic and cleared in Clean()

	//f_data ... -- automatic and cleared in Clean()
	//f_events ... -- auto-init

	//f_cdata_tags.Empty() -- auto-init

	InitDocument();
}


/** \brief Cleans the parser
 *
 * This functions ensures that the parser is clean and then it returns.
 */
moXMLParser::~moXMLParser()
{
	// do nothing! (well... not really, but it's all automatic)
}


/** \brief Clean this parser entirely
 *
 * This function reset the parser to where it can be reused to
 * load another XML file from scratch (i.e. all the data read
 * previously will all be gone.)
 *
 * After a call to this function, functions that have to be
 * called before ReadNext() is called can again be called.
 *
 * \sa moXMLParser::CleanSyntax(void)
 * \sa moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals = false)
 */
void moXMLParser::Clean(void)
{
	// in case we want to be ready to reuse this object
	// (we may one day decide that it's a good idea to
	// have the Clean() function public)
	CleanSyntax();

	f_data.Empty();

	f_input = 0;		// release this input file
	f_input_streams.Empty();
	f_old_input_streams.Empty();

	Init();
}


/** \brief Clean the syntax (DTD)
 *
 * This function cleans all the syntax information (any previously
 * set or read DTD data).
 *
 * This is in effect the opposite of SetSyntax().
 *
 * You can retrieve the current syntax name using the Syntax()
 * function.
 *
 * \sa moXMLParser::Syntax(void) const
 * \sa moXMLParser::SetSyntax(const moWCString& dtd)
 * \sa moXMLParser::CleanSyntax(void)
 */
void moXMLParser::CleanSyntax(void)
{
	// clear the names, source and DTD items
	f_internal_dtd = 0;
	f_current_syntax.Empty();
	f_current_identification.Empty();
	f_dtd_url.Empty();
	f_dtd_elements.Empty();
	f_dtd_entities.Empty();
	f_dtd_parsed_entities.Empty();
	f_cond_include = 0;
}




/** \brief Make the parser strict.
 *
 * By default the moXMLParser is setup to read HTML code.
 *
 * To make the parser much stricter (i.e. more like an XML parser)
 * use this function. Many more errors will occur if the source
 * is not valid XML.
 *
 * \param[in] strict Whether the parser becomes strict
 */
void moXMLParser::Strict(bool strict)
{
	f_strict = strict;
}






/** \brief Set the DTD of the input XML files.
 *
 * This SetSyntax() function expects a string representing the DTD
 * that the parser will use to parse the input XML files.
 *
 * If the XML file being read includes its own DTD, this one will
 * be dropped.
 *
 * The DTD needs to be strictly defined.
 *
 * \bug
 * This function cannot be called after the parsing started (i.e.
 * a call to ReadNext() already happened.
 *
 * \param[in] dtd The whole DTD to use to read the input file
 *
 * \return The function returns true when the DTD was parsed successfully.
 *
 * \sa moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals = false)
 */
bool moXMLParser::SetSyntax(const moWCString& dtd)
{
	xml_get_func_t		func;
	moMemFileSPtr		input;
	unsigned long		p;
	mowc::wc_t		r;

// ReadNext() was already called with success?
	if(!f_data.IsEmpty()) {
		return false;
	}

// insert the DTD

	// put the DTD in an IStream (we use the memory stream for that)
	input = new moMemFile;
	input->InputFilename("*SetSyntax*");
	input->Put((mowc::wc_t) 0xFEFF);
	input->Write(dtd.Data(), dtd.Length() * sizeof(mowc::wc_t));
	// adding ]> so we can be sure to end the stream properly
	//input->Put((mowc::wc_t) ']');
	//input->Put((mowc::wc_t) '>');

	// create an input stream for the XML parser to use
	moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
	stream->SetLine(f_input->Line());
	stream->XMLInterpretVariables(f_input->XMLGetInterpretVariables());
	Include(*stream);

	// hide the existing streams!
	moListOfXMLStreams empty_input_stream_list;
	moAutoRestore<moListOfXMLStreams> auto_restore_input_streams(f_input_streams, empty_input_stream_list);

	// Hide the existing stack
	moArray empty_stack_array(sizeof(xml_get_func_t));
	moAutoRestore<moArray> auto_restore_stack(f_stack, empty_stack_array);

	// setup our stack so the GetAnySubset() function is called
	// (that is the DTD parser function)
	//Push(&moXMLParser::GetIntSubset);	// this catches the ]> we just added
	Push(&moXMLParser::GetAnySubset);

	// make sure we're not already running
	assert(!f_running);

	// set to true, reset to false on exit
	moAutoRestore<zbool_t> auto_restore_running(f_running, true, false);

	// The GetAnySubset() is popped whenever an entry that is not a DTD
	// is encountered.
	do {
		p = f_stack.Count();

		// call the function on the top of the stack
		func = *reinterpret_cast<xml_get_func_t *>(f_stack.Get(p - 1));
		r = (this->*func)();
		if(r == XML_RT_EOF) {
			// WARNING: the EOF may or may not be our moMemFile!
			//	    We check the size of the input streams list
			//	    for that reason. Also, a PopInclude() can
			//	    happen in XMLGetC(), XMLSkipSpaces(), etc.
			r = PopInclude();
		}
	} while(r >= 0 && !f_stack.IsEmpty());

	while(r >= 0) {
		r = XMLGetC();
	}

	// restore the streams now (because we need to pop them)
	auto_restore_input_streams.Restore();
	PopInclude();

	return r == XML_RT_EOF;
}


/** \brief Set the syntax and DTD to HTML.
 *
 * This function can be used to set the syntax of the XML parser
 * to HTML.
 *
 * \param[in] strict Whether the strict HTML DTD is used or the loose one
 *
 * \return true if the syntax is accepted and the parser ReadNext() was never called.
 */
bool moXMLParser::SetHTMLSyntax(bool strict)
{
	return SetSyntax(strict ? g_html_dtd : g_html_loose_dtd);
}


/** \brief Set whether the parser shall use the DTD.
 *
 * Set the flag used to know whether the DTD shall be used in that
 * GetSTag() function to ensure that tags are being closed when
 * invalid in the parent tag.
 *
 * The problem is that when used in files also supporting mo tags
 * it generates problems. Also, the support for the \<script> tag
 * is enhanced in browsers allowing the script tag anywhere whereas
 * the DTD restricts it in many places. Plus, the script may generate
 * tags that then we do not have access to in this parser to know
 * whether the HTML is well formed.
 *
 * This flag can be tweaked within our XML files with the use of the
 * \<mo:usedtd> ... \</mo:usedtd> tags. Note that these can be nested.
 * And the start tag can specify whether the DTD shall be used or not.
 *
 * \param[in] use The new state of the flag.
 *
 * \return The state of the flag before the function is called so you can
 *		restore the flag later if necessary.
 */
bool moXMLParser::UseDTD(bool use)
{
	bool old(f_use_dtd);
	f_use_dtd = use;
	return old;
}


/** \brief Defines the version of the input XML file.
 *
 * This function can be used to initialize the XML parser before
 * the ReadNext() function gets called.
 *
 * The SetVersion() function can be used to determine
 * the version of the input file. This is useful if
 * the file doesn't include an XML header. It is
 * strongly encouraged to also define the encoding
 * if there isn't an XML header in the input file.
 * 
 * At this time, valid versions are 1.0 and 1.1
 * (0x10000 and 0x10001).
 *
 * \param[in] version Defines the XML version
 *
 * \return true when the functions succeed
 *
 * \sa moXMLParser::Encoding(void) const
 * \sa moXMLParser::Syntax(void) const
 * \sa moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals = false)
 */
bool moXMLParser::SetVersion(unsigned long version)
{
	// ReadNext() was already called with success
	if(!f_data.IsEmpty()) {
		return false;
	}

	if(version == 0x10000 || version == 0x10001) {
		f_version = version;
		return true;
	}

	return false;
}


/** \brief Defines the input file encoding.
 *
 * This function can be used to initialize the XML parser before the
 * ReadNext() function is called.
 *
 * The SetInputEncoding() function defines the encoding
 * of the input file. It has to be a valid encoding as
 * understood by iconv(1). Note that a version also
 * has to be specified if the XML file does not include
 * a header. This function fails if the given encoding
 * is not recognized by iconv(3).
 *
 * \param[in] encoding The name of the encoding to use to read the input file
 *
 * \return true when the function succeeds
 *
 * \sa moXMLParser::Encoding(void) const
 * \sa moXMLParser::Syntax(void) const
 * \sa moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals = false)
 */
bool moXMLParser::SetInputEncoding(const moWCString& encoding)
{
	// ReadNext() was already called with success?
	if(!f_data.IsEmpty()) {
		return false;
	}

	if(f_input != 0) {
		if(f_input->XMLSetInputEncoding(encoding) == 0) {
			f_encoding = encoding;
			return true;
		}
	}

	return false;
}



/** \brief Read the next XML element.
 *
 * The ReadNext() function reads one XML element as decribed below
 * and generates a signal to all the handlers registered with this
 * XML parser.
 * 
 * Use the ReadNextNoSignal() to avoid the signals. This is particularly
 * important if you are calling from a handler since these should not
 * recursively generate signals.
 * 
 * The different XML elements are defined as follow:
 * 
 * \li XML_TYPE_TAG_START \n
 * This encompasses a start tag, an empty tag (returned like a start tag marked as empty)
 * and parameters in these tags. The parameters may be standalone in non-strict mode.
 *
 * \code
 *	<name ...>
 *	<name ... />
 *	<... param ...>
 *	<... param=value ...>
 * \endcode
 * 
 * \li XML_TYPE_TAG_END \n
 * A closing tag.
 *
 * \code
 *	</name>
 * \endcode
 * 
 * \li XML_TYPE_TAG_DATA \n
 * Data between tags.
 *
 * \code
 *	data
 * \endcode
 * 
 * \li XML_TYPE_PI \n
 * A language target (usually "xml", could be "php", "perl", etc.)
 *
 * \code
 *	<?pitarget ... ?>
 * \endcode
 * 
 * \li XML_TYPE_COMMENT \n
 * A comment; note that comments are returned to
 * the caller only if specifically requested
 * (and by default this is turned off)
 *
 * \code
 *	<!-- comment ... -->
 * \endcode
 *
 * \li XML_TYPE_CDATA \n
 * data read as is
 *
 * \code
 *	<![CDATA[ ... ]]>
 * \endcode
 * 
 * \li \<no type> \n
 * A definition parsed internally (not actually returned.) These are
 * used to declare the DTD of the XML document.
 *
 * \code
 *	<!... >
 * \endcode
 *
 * \par (XML Parser Message Handling)
 * <small>Note that the following requires Doxygen 1.5.2+ to be transformed
 * in a graph. See mscgen at http://www.mcternan.me.uk/mscgen/ for the
 * parser and http://www.stack.nl/~dimitri/doxygen/commands.html#cmdmsc for
 * the command.</small>
 *
 * \msc
 *	You, ReadNext, Signal, ReadNextNoSignal, ReadNextBlock;
 *
 *	You => ReadNext;
 *	ReadNext => ReadNextNoSignal;
 *	ReadNext << ReadNextNoSignal;
 *	ReadNext =>> Signal [label = "Calling callbacks", ID = 1];
 *	--- [label = "When a signalled tag needs to read a block:"];
 *	Signal => ReadNextBlock;
 *	--- [label = "Some signals can be recursive:"];
 *	ReadNext <= ReadNextBlock;
 *	ReadNext =>> Signal;
 *	...;
 *	ReadNext <<= Signal;
 *	ReadNext >> ReadNextBlock;
 *	--- [label = "When signals are non-recursive:"];
 *	ReadNextNoSignal <= ReadNextBlock;
 *	ReadNextNoSignal >> ReadNextBlock;
 *	--- [label = "Done with the signal handling"];
 *	Signal << ReadNextBlock;
 *	ReadNext <<= Signal [label = "Callback returns"];
 *	You << ReadNext;
 * \endmsc
 *
 * <ol><li>Callbacks are registered using the
 * moXMLParser::RegisterEventHandler(xml_type_t type, moXMLEvent& event)
 * function</li></ol>
 *
 * \param[in,out] data An moXMLType object which has a type and some data
 * \param[in] delete_signals Do not keep signal tags in the output
 *
 * \return true when the function succeeds in reading an XML element;
 *		false if there is an error or the end of the file
 *		was reached and not data was read on this call.
 *
 * \sa moXMLParser::Syntax()
 * \sa moXMLParser::SetSyntax()
 * \sa moXMLParser::Encoding()
 * \sa moXMLParser::Version()
 * \sa moXMLParser::SetVersion()
 * \sa moXMLParser::ReadNextNoSignal()
 * \sa moXMLParser::ReadNextBlock()
 *
 * \sa moXMLParser::moXMLType
 * \sa moXMLParser::moXMLData
 * \sa moXMLParser::moXMLTag
 * \sa moXMLParser::moXMLEvent
 */
bool moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals)
{
	bool		r, repeat;
	int		signal_pos;

	do {
		repeat = false;
		r = ReadNextNoSignal(data);
		if(data != 0) {
			signal_pos = f_data.Count() - 1;
			repeat = Signal(*data);
			if(delete_signals && repeat) {
				f_data.Delete(signal_pos);
			}
		}
	} while(repeat);

	return r;
}


/** \brief Read the next entry and bypass the signal.
 *
 * The ReadNextNoSignal() function is similar to the ReadNext()
 * in that it reads one XML element. However, it will not call the
 * Signal() function so none of the handlers will be called. This
 * is usually called from within a handler to read more raw data.
 *
 * \param[in] data A reference to a smart pointer to be filled with the read data.
 *
 * \return true when some data was read; false if an error occured
 * 	or the end of the file was reached.
 *
 * \sa ReadNext(moXMLTypeSPtr& data, bool delete_signals)
 */
bool moXMLParser::ReadNextNoSignal(moXMLTypeSPtr& data)
{
	xml_get_func_t	func;
	int		r;
	unsigned long	cnt, p;

	assert(!f_running);

	// set to true, reset to false on exit
	moAutoRestore<zbool_t> auto_restore_running(f_running, true, false);

	cnt = f_data.Count();
	do {
		// call the function on the top of the stack
		p = f_stack.Count();
		if(p == 0) {
			// empty stack!?!?
			f_input->FormatWarning(XML_ERRCODE_SYNTAX, "unexpected extraneous characters");
			r = XML_RT_ERROR;
			break;
		}
		func = *reinterpret_cast<xml_get_func_t *>(f_stack.Get(p - 1));
		r = (this->*func)();
		if(r == XML_RT_EOF) {
			if(cnt != f_data.Count()) {
				r = 0;
				break;
			}
			r = PopInclude();
		}
	} while(r >= 0 && cnt == f_data.Count());
	// repeat until we read some data or find an error

	// now we don't want any references to these streams
	f_old_input_streams.Empty();

	if(cnt == f_data.Count()) {
		// we didn't read anything
		data = 0;
		return false;
	}

	data = f_data.Get(cnt);

	return r >= 0;
}



/** \brief Read a block of XML elements between a set of start/end tags.
 *
 * The ReadNextBlock() function was added to ease the reading of a
 * large block of XML elements defined between a start tag and an end
 * tag.
 *
 * Usually one receives a signal on a start tag. The corresponding handler
 * is called and uses this function to read everything up to the end
 * tag. Note that by default the data read will not be signaled since
 * this function calls ReadNextNoSignal().
 *
 * \warning
 * The result of ReadNextBlock() and of GetData() with the 'from' and
 * 'to' values returned by ReadNextBlock() are not equal. The problem
 * comes from the 'v' format of the \<mo:replace> tag. That specific tag
 * wants the signals, but doing so means certain entries should be hidden
 * from the output and calling GetData() would reveal these entries.
 *
 * \exception moBug(MO_ERROR_BAD_PARAMETER)
 * If you do not define an output pointer but expect signals to be
 * emitted (i.e. \p output == 0 && \p emit_signal == true) this exception
 * is thrown.
 *
 * \param[out] from The first data read
 * \param[out] to The last (inclusive) data read
 * \param[in] emit_signal Whether the ReadNextBlock() emits signals when it finds registered tags
 * \param[out] output String where the result is saved (optional, can be set to NULL)
 *
 * \return true when the function read at least one element; false when
 *		the end of the file is reached or an error occurs.
 *
 * \sa ReadNext(moXMLTypeSPtr& data, bool delete_signals)
 */
bool moXMLParser::ReadNextBlock(moList::position_t& from, moList::position_t& to, bool emit_signal, moWCString *output)
{
	moXMLTypeSPtr	data;
	moWCString	type_string;
	size_t		length;
	bool		result;

	if(output != 0) {
		output->Empty();
	}
	else if(emit_signal) {
		throw moBug(MO_ERROR_BAD_PARAMETER, "moXMLParser::ReadNextBlock() used without a result string but the emit_signal set to true");
	}

	from = f_data.Count();
	to = 0;		// invalid by default

#ifdef MO_DEBUG
	if(from == 0UL) {
		throw moBug(MO_ERROR_BAD_PARAMETER, "moXMLParser::ReadNextBlock(): this function can be reached only after a start tag. (1)");
	}
	data = f_data.GetLast();
	if(data->GetType() != XML_TYPE_TAG_START) {
		throw moBug(MO_ERROR_BAD_PARAMETER, "moXMLParser::ReadNextBlock(): this function can be reached only after a start tag. (2)");
	}
#endif

	// we can detect that the end tag was found whenever the
	// name becomes smaller (this only works with well formed
	// XML at this time -- Mar 28, 2004)
	length = f_name.Length();

	for(;;) {
		if(emit_signal) {
			result = ReadNext(data, true);
		}
		else {
			result = ReadNextNoSignal(data);
		}
		if(!result) {
			to = f_data.Count();
			return false;
		}
		// found the end tag?
		if(f_name.Length() < length) {
			// the block was successfully read!
			// 'to' is Count() - 1 because it is inclusive not that
			// it points to the closing tag which will be included if
			// you don't yet substract 1 more before calling BlockToString...
			to = f_data.Count() - 1;
			return true;
		}
		if(output != 0) {
			data->ToString(type_string);
			*output += type_string;
		}
	}
	/*NOTREACHED*/
}


/** \brief Transform a block of tags into a string.
 *
 * This function transforms a block of data to a string.
 *
 * The block is taken from the XML parser current data.
 *
 * The \p from and \p to parameters can be any valid position
 * in the current data, though in general these are the 
 * values returned by the ReadNextBlock() function and
 * thus they correspond to the start and end tags of
 * a block of data.
 *
 * If \p to is smaller than \p from, then this function generates an
 * empty string.
 *
 * \param[in] str The result is saved in this parameter after the string was cleared
 * \param[in] from The first entry of data to transform to string
 * \param[in] to The last entry of data to transform to string, inclusive
 */
void moXMLParser::BlockToString(moWCString& str, moList::position_t from, moList::position_t to)
{
	moXMLTypeSPtr	data;
	moWCString	type_string;

	str.Empty();
	while(from <= to) {
		data = f_data.Get(from);
		data->ToString(type_string);
		str += type_string;
		from++;
	}
}


/** \brief Get one data entry.
 *
 * This function searches for one data entry and returns its pointer.
 *
 * All the data read from an XML file is saved in one large array. The
 * full name represents the DOM of the tag at that location.
 *
 * The position used to query the data must be between 0 and the total
 * number of entries or this function will throw and out of bounds error.
 *
 * \param[in] pos The data entry to return.
 *
 * \return A pointer to an XML type object
 */
moXMLParser::moXMLTypeSPtr moXMLParser::GetData(moList::position_t pos) const
{
	return f_data.Get(pos);
}



/** \brief Push current stream on the stack and make new stream current.
 *
 * The Include() function stacks the current XML input stream and
 * put the new one specified in the function call as the current one.
 *
 * The subsequent calls to the XML parser will read from this new
 * input stream until the end is reached. At that time, the XML
 * parser will release the current XML input stream and switch
 * back to the previous one.
 *
 * Any number of XML input streams can be included in this way, but
 * if these streams are linked to system files, it will certainly
 * be limited to the number of files which can be opened at once
 * on your system.
 *
 * The input file will automatically have its CleanNL() and
 * SetLineSeparators() called with the proper values used by
 * the XML parser. It is a good idea not to modify these values
 * afterward.
 *
 * The type of the data from the input file is assumed to be text/xml.
 * This means it will be parsed for XML data as the root file. If
 * the file being included should not be parsed because it is of
 * some other type (plain text, code, etc.), then you should use
 * a different mime type such as text/plain. This has the effect of
 * switching the reading of the current input stream to
 * 'Read Plain Content' (see the GetPlainContent() function). This
 * means the entire file will be read in a single string and returned
 * in the list of elements read from the input file as a DATA block.
 * Plain files are read as is and can include all the characters
 * you need in there. This is used with the database system whenever
 * the data is not defined as XML data (useful to include tags
 * read from the database which you do not want the XML parser to
 * handle in anyway.)
 *
 * \param[in] xml_stream The new stream to stack
 * \param[in] type The mime type of the data in the XML stream
 *
 * \sa moXMLParser::PopInclude(void)
 * \sa moXMLParser::GetPlainContent(void)
 */
void moXMLParser::Include(moXMLStream& xml_stream, const moWCString& type)
{
	int		r;
	mowc::wc_t	c;

	if(f_input != 0) {
		f_input->XMLSaveName(f_name);
		f_input_streams += *f_input;
	}
	InitStream(&xml_stream);
	xml_stream.XMLSetParser(*this);

	if(f_input != 0) {
		/* just in case, we avoid adding extra FEFF chars in the middle of nowhere */
		c = f_input->GetC();
		if(c != 0xFEFF && c > 0) {
			f_input->UngetC(c);
		}
		if(!type.IsEmpty() && type != "text/xml") {
			Push(&moXMLParser::GetPlainContent);
		}
		else {
			r = f_input->XMLNextLine();
			if(r == XML_RT_NOERROR) {
				if(f_input->XMLTestString("<?xml")) {
					// ha! in case we seem to have a full declaration
					// we need to have a full stack of XML data!
					// TODO: this will destroy the current syntax, etc.
					//	 of the parent file, that needs to be moved
					//	 in the moXMLStream object instead!
					Push(&moXMLParser::GetMisc);
					Push(&moXMLParser::GetElement);
					Push(&moXMLParser::GetMisc);
					Push(&moXMLParser::GetDoctypeDecl);
					Push(&moXMLParser::GetMisc);
					Push(&moXMLParser::GetXMLDecl);
				}
			}
		}
	}
}


/** \brief Called when the end of file of the current stream is reached.
 *
 * Whenever reading a file (an XML stream) the last character returned
 * is the EOF (XML_RT_EOF). If that happens, the system calls this
 * function to drop the current input stream and restore the previous
 * one. This is very similar to returning from an \#include in a C
 * preprocessor.
 *
 * In general, you do not have to worry about calling this function
 * since the other system functions such as XMLSkipSpaces() and
 * XMLGetC() do it automatically.
 *
 * This function generates an error if the file being closed did not
 * properly close all the tags that it opened.
 *
 * \bug
 * The test to know whether the stack is valid does not occur whenever
 * the stack is empty. This is probably okay though.
 *
 * \return XML_RT_NOERROR when a different stream was restored successfully,
 *	XML_RT_ERROR when the tags of the current stream were not all properly closed,
 *	XML_RT_EOF when there is no more streams to restore
 *
 * \sa moXMLParser::Include(moXMLStream& xml_stream, const moWCString& type)
 */
int moXMLParser::PopInclude(void)
{
	moList::position_t	p;

// did someone include something?
	p = f_input_streams.Count();
	if(p == 0UL) {
		return XML_RT_EOF;
	}
	p--;

// we don't need this input anymore, however, this can happen at any
// time and there could be references to objects inside that input
// and thus we keep it in another list
	moWCString name;
	if(f_input != 0) {
		moIStreamSPtr input;
		input = f_input->Input();
		if(input != 0) {
			name = input->InputFilename();
		}
	}

// now we want to unstack the previous input
	f_old_input_streams += *f_input;	// deleted on "exit"
	f_input = f_input_streams.Get(p);
	f_input_streams.Delete(p);

// finally, we have to ensure that the stack is valid
	if(f_input->XMLGetName() != f_name) {
		// TODO: we need to check the tags against the DTD so as to know if some
		//	 of them could be closed automatically; if so then we could in
		//	 some cases avoid this error
		// TODO: the following error won't work if f_name.Length() < f_input.Length()
		//	 and now that can happen!
		f_input->FormatError(XML_ERRCODE_SYNTAX, "the included file \"%S\" did not close the tags: \"%S\" (current path: \"%S\", input full path: \"%S\")",
					name.Data(),
					f_name.Get(static_cast<int>(f_input->XMLGetName().Length()),
							static_cast<int>(f_name.Length() - 1)).Data(),
					f_name.Data(), f_input->XMLGetName().Data());
		return XML_RT_ERROR;
	}

	return XML_RT_NOERROR;
}


/** \brief Set the XML stream.
 *
 * This function defines the pointer to the XML stream to use
 * to read for more input data.
 *
 * This function is used internally by the Include() and PopInclude()
 * functions to determine what stream is the current stream.
 *
 * \param[in] xml_stream The XML stream becoming current.
 */
void moXMLParser::InitStream(moXMLStream *xml_stream)
{
	f_input = xml_stream;
}




/** \brief Write a message in the moXMLStream error stream.
 *
 * This function builds a message from the \p format and following
 * parameters and sends it to the
 * moTextStream::FormatMessage() function.
 *
 * The type is also forwarded as is to the
 * moTextStream::FormatMessage() function.
 *
 * \param[in] type     The type of message.
 * \param[in] format   A format string a la printf(3C).
 * \param[in] ...      The arguments used by the format
 */
void moXMLParser::FormatMessage(const char *type, const char *format, ...)
{
	va_list		args;

	if(f_input != 0) {
		va_start(args, format);
		try {
			f_input->VFormatMessage(type, format, args);
		}
		catch(...) {
			va_end(args);
			throw;
		}
		va_end(args);
	}
}


/** \brief Write a message in the moXMLStream error stream.
 *
 * Please, see the FormatMessage() for more information.
 *
 * \param[in] type     The type of message.
 * \param[in] format   A format string a la printf(3C).
 * \param[in] args     The arguments used by the format
 */
void moXMLParser::VFormatMessage(const char *type, const char *format, va_list args)
{
	if(f_input != 0) {
		f_input->VFormatMessage(type, format, args);
	}
}


/** \brief Write a warning in the moXMLStream error stream.
 *
 * This function builds a warning message from the \p format and following
 * parameters and sends it to the moTextStream::FormatWarning() function.
 *
 * The \p warcode is also forwarded as is to the moTextStream::FormatWarning()
 * function. A tool can then decide to hide this warning or transform
 * it into an error instead.
 *
 * \param[in] warcode   The warning code.
 * \param[in] format    A format string a la printf(3C).
 * \param[in] ...       A set of arguments used by format.
 */
void moXMLParser::FormatWarning(moTextStream::errcode_t warcode, const char *format, ...)
{
	va_list		args;

	if(f_input != 0) {
		va_start(args, format);
		try {
			f_input->VFormatWarning(warcode, format, args);
		}
		catch(...) {
			va_end(args);
			throw;
		}
		va_end(args);
	}
}


/** \brief Write a warning in the moXMLStream error stream.
 *
 * Please, see the FormatWarning() function for more information.
 *
 * \param[in] warcode   The warning code.
 * \param[in] format    A format string a la printf(3C).
 * \param[in] args      A set of arguments used by format.
 */
void moXMLParser::VFormatWarning(moTextStream::errcode_t warcode, const char *format, va_list args)
{
	if(f_input != 0) {
		f_input->FormatWarning(warcode, format, args);
	}
}


/** \brief Write an error in the moXMLStream error stream.
 *
 * This function builds an error message from the \p format and following
 * parameters and sends it to the moTextStream::FormatError() function.
 *
 * The \p errcode is also forwarded as is to the moTextStream::FormaError()
 * function.
 *
 * \param[in] errcode   The error code.
 * \param[in] format    A format string a la printf(3C).
 * \param[in] ...       A set of arguments used by format.
 */
void moXMLParser::FormatError(moTextStream::errcode_t errcode, const char *format, ...)
{
	va_list		args;

	if(f_input != 0) {
		va_start(args, format);
		try {
			f_input->FormatError(errcode, format, args);
		}
		catch(...) {
			va_end(args);
			throw;
		}
		va_end(args);
	}
}


/** \brief Write an error in the moXMLStream error stream.
 *
 * Please, see FormatError() for more information.
 *
 * \param[in] errcode   The error code.
 * \param[in] format    A format string a la printf(3C).
 * \param[in] args      A set of arguments used by format.
 */
void moXMLParser::VFormatError(moTextStream::errcode_t errcode, const char *format, va_list args)
{
	if(f_input != 0) {
		f_input->VFormatError(errcode, format, args);
	}
}


/** \brief Get the current read position of the input stream
 *
 * The GetPosition() function returns what the moIStream::ReadPosition()
 * function returns from the input stream. It can later be used to set
 * the position of the input file back to a place of interest (usualy
 * used to repeat the same data multiple times as used by the \<mo:loop>
 * element.)
 *
 * \return Return the current position of the input stream
 *	or 0 when no input stream is defined or -1 if an error occurs
 *
 * \sa moXMLParser::SetPosition(size_t position)
 */
size_t moXMLParser::GetPosition(void) const
{
	if(f_input == 0) {
		return 0;
	}

	if(f_input->Input() == 0) {
		return 0;
	}
	return f_input->Input()->ReadPosition();
}



/** \brief Set the position of the input stream.
 *
 * The SetPosition() function sets the read position of the input
 * stream to the specified position. The position is always given from
 * the start of the file. A position you obtains using the GetPosition()
 * function can be used with this function.
 *
 * \note
 * If no XML stream is currently defined in the parser, this
 * function has no effect.
 *
 * \bug
 * Since the parser handles multiple streams that it will automatically
 * stack up, this function cannot be used safely if you do not make sure
 * that the current stream has not changed.
 *
 * \bug
 * The corresponding input stream needs to be seekable to support
 * this functions.
 *
 * \param[in] position The new position for the input stream.
 */
void moXMLParser::SetPosition(size_t position)
{
	if(f_input != 0 && f_input->Input() != 0) {
		f_input->Input()->ReadPosition(position);
	}
}



/** \brief Get the name of the syntax.
 *
 * This function returns the name of the document syntax.
 * This is the name defined in the <!DOCTYPE ...> element.
 *
 * \return A reference to the current syntax string.
 */
const moWCString& moXMLParser::Syntax(void) const
{
	return f_current_syntax;
}

/** \brief Get the name of the encoding.
 *
 * This function returns the encoding that was read from
 * the \<?xml ...?> declaration or set with SetEncoding().
 *
 * \return A reference to the current encoding string.
 */
const moWCString& moXMLParser::Encoding(void) const
{
	return f_encoding;
}

/** \brief Get the XML version.
 *
 * This function returns the version of the XML document
 * such as 1.0 or 1.1. The result is a number with the
 * major version number in the upper 16 bits and the
 * minor version in the lower 16 bits.
 *
 * \return The version of the document being read.
 */
unsigned long moXMLParser::Version(void) const
{
	return f_version;
}



/** \brief Attach an event handler to this moXMLParser.
 *
 * The RegisterEventHandler() function adds one event handler
 * to the existing list of handlers.
 * 
 * Handlers are ordered using their name and priority (in that order).
 * 
 * The number of handlers per moXMLParser is not limited.
 *
 * The handlers are used whenever ReadNext() generates a signal.
 * See the Signal() function for more information.
 *
 * \bug
 * If you register the same handler multiple times, then its
 * OnEvent() function will be called multiple times.
 * 
 * \bug
 * At this time there is no way to unregister an event. If you
 * need such a functionality, use a flag in your handler that
 * tells you whether it is active or not.
 *
 * \exception moBug(MO_ERROR_BAD_PARAMETER)
 * This exception is thrown in the debug version of the library
 * whenever the type is out of bounds. Note that this is checked
 * only in debug and considered a bug because the addition
 * event handlers is pretty static and should always happen in
 * your code (it should rarely need to dynamically add handlers.)
 *
 * \param[in] type The type of the event handler to register.
 * \param[in] event The event to register.
 *
 * \sa class moXMLParser::moXMLEvent
 */
void moXMLParser::RegisterEventHandler(xml_type_t type, moXMLEvent& event)
{
#ifdef MO_DEBUG
	if(type < 0 || type >= XML_TYPE_max) {
		throw moBug(MO_ERROR_BAD_PARAMETER, "moXMLParser::RegisterEventHandler(): type index out of bounds");
	}
#endif

	f_events[type] += event;
}



/** \brief Send a signal to the registered handlers.
 *
 * The Signal() function is called whenever the ReadNext() function
 * found some element requiring it. The effect is to call the
 * OnEvent() function of all the handlers or until one handler
 * returns true. This is useful to break the calls and also ensure
 * that the tag which generated this event will not be returned to
 * the caller (it was processed/eaten.)
 *
 * \exception moBug(MO_ERROR_BAD_PARAMETER)
 * This exception is thrown in debug whenever the type is out of bounds.
 * This is an internal bug that needs to be fixed if it ever occurs.
 *
 * \param[in,out] data The data that generated the signal. The handler can modify it.
 *
 * \return false if the handler did not want this data,
 *	true if the data was processed and can be removed from the output.
 */
bool moXMLParser::Signal(moXMLType& data)
{
	moList::position_t	pos, max;
	xml_type_t		type;

	type = data.GetType();

#ifdef MO_DEBUG
	if(type < 0 || type >= XML_TYPE_max) {
		throw moBug(MO_ERROR_INTERNAL, "moXMLParser::Signal(): type index out of bounds");
	}
#endif

	moSortedListOfXMLEvent& list = f_events[type];
	max = list.Count();
	if(max == 0UL) {
		// if this list is empty, go away quickly
		return false;
	}

	switch(type) {
	case XML_TYPE_DATA:
	case XML_TYPE_CDATA:
	case XML_TYPE_COMMENT:
		// these have no name so we call all the handlers
		for(pos = 0; pos < max; pos++) {
			if(list.Get(pos)->OnEvent(*this, data)) {
				return true;
			}
		}
		break;

	default:
		// named data types need to only call the corresponding handlers
		// so we have to search for them by name only (the priority isn't
		// used to search the handlers here)
		moXMLEvent find(moXMLEvent::XML_NO_PRIORITY, data.GetTagName());
		pos = list.Find(&find);
		while(pos != moList::NO_POSITION) {
			if(list.Get(pos)->OnEvent(*this, data)) {
				return true;
			}
			pos = list.FindNext();
		}
		break;

	}

	return false;
}





/** \brief Set a variable in the XML parser.
 *
 * The moXMLParser object supports variables. The variables themselves
 * are saved in the moXMLStream objects. Each stream has its own set
 * of variables making variables within a stream local to that stream
 * and all of its children.
 * 
 * The variable names are defined as a path to support reading variables
 * from the parent moXMLStream objet as well as moXMLParser globals.
 * Thus, a variable name is a string that can include slash (/) characters
 * and supports the double period (..) notation as in a standard Unix
 * filename path.
 * 
 * The variable /global_filename can be used to store a filename in
 * the root (top-most) moXMLStream object. This way, any included
 * file can modify the global variables and thus "return" a value.
 * 
 * The double period can be used to set or get a variable in a parent
 * moXMLStream object. Thus, the variable ../errno could be used to
 * set an error number in a sub-XML file for the parent XML file to
 * know that an error occured. Similarly, if you need to execute a
 * complex expression or a function, you can return the result in
 * a variable using the double period (for instance, one could save
 * the result in the variable ../my_function_result)
 * 
 * The GetVariable() functions will automatically try to find the
 * named variable in this moXMLStream or any parent moXMLStream.
 * Thus, if you write:
 * 
 * \code
 *	<mo:insert name="filename"> or $(filename)
 *	<mo:insert name="../filename">
 *	<mo:insert name="/topmost_filename">
 * \endcode
 * 
 * the variable named "filename" will be searched in the current
 * moXMLStream, and all the parents up to the root (top-most).
 * The first variable found is returned.
 * 
 * When the GetVariable() is used with a name starting with /,
 * then only the root moXMLStream is searched.
 * 
 * When the GetVariable() is used with one or more double period,
 * it starts its searched in the corresponding parent moXMLStream.
 * 
 * The UnsetVariable() unsets a variable only in the specified
 * moXMLStream. It will never try to check a parent or the root
 * whether or not that variable was defined in the specified
 * moXMLStream.
 * 
 * \code
 *	<mo:unset name="background-color">
 *	<mo:unset name="../no-color">
 *	<mo:unset name="/last-errno">
 * \endcode
 * 
 * The SetVariable() function sets the variable in the specified
 * moXMLStream only.
 * 
 * \code
 *	<mo:set name="/global_var" value="this value">
 * \endcode
 * 
 * The examples syntax are valid only with the proper variable XML
 * extension. Other extensions for support of variables can be used.
 *
 * \param[in] variable The variable to be added to the XML parser.
 *
 * \return A pointer to the added variable or NULL
 */
moVariableSPtr moXMLParser::SetVariable(const moVariable& variable)
{
	return SetVariable(variable.Name(), variable);
}


/** \brief Set a variable in the XML parser.
 *
 * This function sets the variable using one string with the
 * name and value separated by an equal sign.
 *
 * See the SetVariable() for more information (click on the link in the See Also below.)
 *
 * \param[in] set A String representing the path, name and variable content separated by an equal sign.
 *
 * \sa moXMLParser::SetVariable(const moVariable& variable)
 */
moVariableSPtr moXMLParser::SetVariable(const moWCString& set)
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
		return SetVariable(moWCString(start, static_cast<int>(s - start)), moWCString(s + 1));
	}
	else {
		return SetVariable(set, moWCString(""));
	}
}


/** \brief Set a variable in the XML parser.
 *
 * This function sets the variable with its name and value specified as
 * two separate strings.
 *
 * See the SetVariable() for more information (click on the link in the See Also below.)
 *
 * \param[in] name The path and name of the variable to set.
 * \param[in] value The value of this variable.
 *
 * \sa moXMLParser::SetVariable(const moVariable& variable)
 */
moVariableSPtr moXMLParser::SetVariable(const moWCString& name, const moWCString& value)
{
	moWCString		var_name;
	moXMLStreamSPtr		input;
	moList::position_t	pos;

	input = FindStream(name, var_name, pos);
	if(!input) {
		return 0;
	}

	return input->XMLSetVariable(var_name, value);
}


/** \brief Search for the named variable.
 *
 * This function searches for a variable starting at the
 * XML stream as specified by the path defined in the
 * variable \p name.
 *
 * The path is defined in the SetVariable() documentation.
 *
 * \param[in] name The path and name of the variable to read.
 *
 * \return A pointer to the variable found or NULL when no variable was found.
 *
 * \sa moXMLParser::SetVariable(const moVariable& variable)
 */
moVariableSPtr moXMLParser::GetVariable(const moWCString& name) const
{
	moWCString		var_name;
	moXMLStreamSPtr		input;
	moVariableSPtr		var;
	moList::position_t	pos;

	input = FindStream(name, var_name, pos);
	if(input) {
		var = input->XMLGetVariable(var_name);
		if(var) {
			return var;
		}
	}

	while(pos > 0UL && !var) {
		pos--;
		input = f_input_streams.Get(pos);
		var = input->XMLGetVariable(var_name);
	}

	return var;
}


/** \brief Delete a variable in the specified XML stream.
 *
 * This function delete the named variable. The name can
 * include a path to a specific XML stream.
 *
 * If the variable was not defined, then nothing happens.
 *
 * \param[in] name The name and path to the variable to delete.
 */
void moXMLParser::UnsetVariable(const moWCString& name)
{
	moWCString		var_name;
	moXMLStreamSPtr		input;
	moList::position_t	pos;

	input = FindStream(name, var_name, pos);
	if(input) {
		input->XMLUnsetVariable(var_name);
	}
}


/** \brief Append a list of variables to this XML parser.
 *
 * The AppendVariableList() function reads the list of variables from
 * the input and adds them to the different parser streams. Note that
 * here the input variable names can include path information, it will
 * be interpreted as expected (see the different SetVariable() functions).
 *
 * \param[in] list A list of variables to append.
 *
 * \sa moXMLParser::SetVariable(const moVariable& variable)
 * \sa moXMLParser::SetVariable(const moWCString& set)
 * \sa moXMLParser::SetVariable(const moWCString& name, const moWCString& value)
 */
void moXMLParser::AppendVariableList(const moVariableList& list)
{
	moList::position_t	p;
	moVariableSPtr		v;

	p = list.Count();
	while(p > 0UL) {
		p--;
		v = list.Get(p);
		SetVariable(v->Name(), v);
	}
}


/** \brief Search for the specified variable name.
 *
 * The FindStream() function can be used to search for an moXMLStream
 * based on a variable name. The input name is the same as you would
 * use with any of the GetVariable(), SetVariable() and UnsetVariable()
 * functions. The var_name is the input name with all the "/" and
 * "../" removed. This function is used internally to find the proper
 * moXMLStream object.
 *
 * \param[in] name The fullname of the variable to find.
 * \param[out] var_name The basename of the variable, computed by this function
 * \param[out] pos The position of the XML stream in the array of input stream
 *
 * \return The XML stream where this variable should be defined
 */
moXMLParser::moXMLStreamSPtr moXMLParser::FindStream(const moWCString& name, moWCString& var_name, moList::position_t& pos) const
{
	const mowc::wc_t	*s;

	pos = f_input_streams.Count();

	s = name.Data();
	while(*s == '/') {
		// a global!
		pos = 0;
		s++;
	}
	while(s[0] == '.' && s[1] == '.' && s[2] == '/') {
		// a parent!
		if(pos > 0UL) {
			pos--;
		}
		s += 3;
	}

	var_name = s;

	if(pos >= f_input_streams.Count()) {
		return f_input;
	}

	return f_input_streams.Get(pos);
}


/** \brief Get the currently active stream.
 *
 * This function returns the stream currently active. Note that
 * this pointer can be NULL.
 *
 * \return A pointer to the currently active XML stream.
 */
moXMLParser::moXMLStreamSPtr moXMLParser::InputStream(void) const
{
	return f_input;
}


/** \brief Check a character to know whether it is valid for an XML name.
 *
 * This function determines the type of the \p c character. Then it
 * checks whether this is a valid character that can be part of an
 * XML name.
 *
 * The \p first parameter must be set to true when checking the
 * first character of an XML name. Note that it defaults to false.
 *
 * A valid character for a name is a letter (upper, lower, graphical)
 * a combined character, a digit or an extender. Only a letter can
 * be used as the very first character.
 *
 * \param[in] c The character to check.
 * \param[in] first Whether this is the first character of the name or a following character.
 *
 * \return true if this character can be part of an XML name.
 */
bool moXMLParser::IsXMLNameChar(mowc::wc_t c, bool first)
{
	const uint32_t flags = XMLCType(c);
	return 0 != (flags & (first ?
		  XML_CTYPE_NAME | XML_CTYPE_LETTER
		: XML_CTYPE_NAME | XML_CTYPE_NAME_CHAR | XML_CTYPE_LETTER
		  | XML_CTYPE_COMBINING | XML_CTYPE_DIGIT | XML_CTYPE_EXTENDER));
}


/** \brief Check whether a character is an XML space.
 *
 * This function is a very fast function checking a character
 * to know whether it is a space or not.
 *
 * \param[in] c The character to check.
 *
 * \return true if \p c is an XML space.
 */
bool moXMLParser::IsXMLSpace(mowc::wc_t c)
{
	// this is faster than calling XMLCType() and it shouldn't change
	return c == 0x09 || c == 0x0A || c == 0x0D || c == 0x20;
}



/** \brief Check characters.
 *
 * This function searches for the specified character in the list
 * of XML character types and it returns that type.
 *
 * The currently known types are listed here:
 *
 * \li moXMLParser::XML_CTYPE_VALID
 * \li moXMLParser::XML_CTYPE_SPACE
 * \li moXMLParser::XML_CTYPE_LETTER
 * \li moXMLParser::XML_CTYPE_BASE
 * \li moXMLParser::XML_CTYPE_IDEOGRAPHIC
 * \li moXMLParser::XML_CTYPE_COMBINING
 * \li moXMLParser::XML_CTYPE_DIGIT
 * \li moXMLParser::XML_CTYPE_EXTENDER
 * \li moXMLParser::XML_CTYPE_ENCODING_START
 * \li moXMLParser::XML_CTYPE_ENCODING
 * \li moXMLParser::XML_CTYPE_PUBLIC_ID
 * \li moXMLParser::XML_CTYPE_NAME
 * \li moXMLParser::XML_CTYPE_NAME_CHAR
 * \li moXMLParser::XML_CTYPE_CHAR_REF
 * \li moXMLParser::XML_CTYPE_XCHAR_REF
 * \li moXMLParser::XML_CTYPE_INVALID
 *
 * \note
 * All characters have a type. Characters considered as non-XML
 * characters are marked as INVALID. In that case, only that one
 * flag is used. For instance, Ctrl-G (the Bell) is invalid in
 * an XML document and is defined as moXMLParser::XML_CTYPE_INVALID.
 *
 * \param[in] c The character which type is searched
 *
 * \return The type of the character
 */
unsigned long moXMLParser::XMLCType(mowc::wc_t c)
{
	struct XMLCharCType {
		mowc::wc_t	from;
		mowc::wc_t	to;
		unsigned long	flags;
	};

	static const XMLCharCType	tbl[] = {
		{ 0x0000, 0x0008, XML_CTYPE_INVALID },
		{ 0x0009, 0x0009, XML_CTYPE_SPACE },
		{ 0x000A, 0x000A, XML_CTYPE_SPACE | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x000B, 0x000C, XML_CTYPE_INVALID },
		{ 0x000D, 0x000D, XML_CTYPE_SPACE | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x000E, 0x001F, XML_CTYPE_INVALID },
		{ 0x0020, 0x0020, XML_CTYPE_SPACE | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x0021, 0x0021, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x0023, 0x0025, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x0027, 0x0027, XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x0028, 0x002C, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x002D, 0x002E, XML_CTYPE_NAME_CHAR | XML_CTYPE_ENCODING | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x002F, 0x002F, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x0030, 0x0039, XML_CTYPE_CHAR_REF | XML_CTYPE_XCHAR_REF | XML_CTYPE_VALID | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ | XML_CTYPE_ENCODING_START | XML_CTYPE_ENCODING | XML_CTYPE_DIGIT },
		{ 0x003A, 0x003A, XML_CTYPE_NAME_CHAR | XML_CTYPE_NAME | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x003B, 0x003B, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x003D, 0x003D, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x003F, 0x0040, XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ },
		{ 0x0041, 0x0046, XML_CTYPE_XCHAR_REF | XML_CTYPE_VALID | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ | XML_CTYPE_ENCODING_START | XML_CTYPE_ENCODING | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0047, 0x005A, XML_CTYPE_VALID | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ | XML_CTYPE_ENCODING_START | XML_CTYPE_ENCODING | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x005F, 0x005F, XML_CTYPE_NAME_CHAR | XML_CTYPE_NAME | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ | XML_CTYPE_ENCODING },
		{ 0x0061, 0x0066, XML_CTYPE_XCHAR_REF | XML_CTYPE_VALID | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ | XML_CTYPE_ENCODING_START | XML_CTYPE_ENCODING | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0067, 0x007A, XML_CTYPE_VALID | XML_CTYPE_PUBLIC_ID | XML_CTYPE_PUBLIC_ID_DQ | XML_CTYPE_ENCODING_START | XML_CTYPE_ENCODING | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x007F, 0x0084, XML_CTYPE_INVALID },
		{ 0x0086, 0x009F, XML_CTYPE_INVALID },
		{ 0x00B7, 0x00B7, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x00C0, 0x00D6, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x00D8, 0x00F6, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x00F8, 0x00FF, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0100, 0x0131, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0134, 0x013E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0141, 0x0148, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x014A, 0x017E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0180, 0x01C3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x01CD, 0x01F0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x01F4, 0x01F5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x01FA, 0x0217, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0250, 0x02A8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x02BB, 0x02C1, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x02D0, 0x02D1, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x0300, 0x0345, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0360, 0x0361, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0386, 0x0386, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0387, 0x0387, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x0388, 0x038A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x038C, 0x038C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x038E, 0x03A1, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03A3, 0x03CE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03D0, 0x03D6, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03DA, 0x03DA, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03DC, 0x03DC, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03DE, 0x03DE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03E0, 0x03E0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x03E2, 0x03F3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0401, 0x040C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x040E, 0x044F, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0451, 0x045C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x045E, 0x0481, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0483, 0x0486, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0490, 0x04C4, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x04C7, 0x04C8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x04CB, 0x04CC, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x04D0, 0x04EB, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x04EE, 0x04F5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x04F8, 0x04F9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0531, 0x0556, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0559, 0x0559, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0561, 0x0586, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0591, 0x05A1, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x05A3, 0x05B9, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x05BB, 0x05BD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x05BF, 0x05BF, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x05C1, 0x05C2, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x05C4, 0x05C4, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x05D0, 0x05EA, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x05F0, 0x05F2, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0621, 0x063A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0640, 0x0640, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x0641, 0x064A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x064B, 0x0652, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0660, 0x0669, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0670, 0x0670, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0671, 0x06B7, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x06BA, 0x06BE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x06C0, 0x06CE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x06D0, 0x06D3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x06D5, 0x06D5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x06D6, 0x06DC, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x06DD, 0x06DF, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x06E0, 0x06E4, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x06E5, 0x06E6, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x06E7, 0x06E8, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x06EA, 0x06ED, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x06F0, 0x06F9, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0901, 0x0903, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0905, 0x0939, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x093C, 0x093C, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x093D, 0x093D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x093E, 0x094C, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x094D, 0x094D, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0951, 0x0954, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0958, 0x0961, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0962, 0x0963, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0966, 0x096F, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0981, 0x0983, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0985, 0x098C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x098F, 0x0990, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0993, 0x09A8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x09AA, 0x09B0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x09B2, 0x09B2, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x09B6, 0x09B9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x09BC, 0x09BC, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09BE, 0x09BE, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09BF, 0x09BF, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09C0, 0x09C4, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09C7, 0x09C8, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09CB, 0x09CD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09D7, 0x09D7, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09DC, 0x09DD, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x09DF, 0x09E1, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x09E2, 0x09E3, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x09E6, 0x09EF, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x09F0, 0x09F1, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A02, 0x0A02, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A05, 0x0A0A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A0F, 0x0A10, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A13, 0x0A28, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A2A, 0x0A30, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A32, 0x0A33, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A35, 0x0A36, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A38, 0x0A39, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A3C, 0x0A3C, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A3E, 0x0A3E, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A3F, 0x0A3F, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A40, 0x0A42, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A47, 0x0A48, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A4B, 0x0A4D, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A59, 0x0A5C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A5E, 0x0A5E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A66, 0x0A6F, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0A70, 0x0A71, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A72, 0x0A74, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A81, 0x0A83, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0A85, 0x0A8B, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A8D, 0x0A8D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A8F, 0x0A91, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0A93, 0x0AA8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0AAA, 0x0AB0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0AB2, 0x0AB3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0AB5, 0x0AB9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0ABC, 0x0ABC, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0ABD, 0x0ABD, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0ABE, 0x0AC5, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0AC7, 0x0AC9, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0ACB, 0x0ACD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0AE0, 0x0AE0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0AE6, 0x0AEF, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0B01, 0x0B03, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B05, 0x0B0C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B0F, 0x0B10, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B13, 0x0B28, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B2A, 0x0B30, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B32, 0x0B33, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B36, 0x0B39, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B3C, 0x0B3C, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B3D, 0x0B3D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B3E, 0x0B43, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B47, 0x0B48, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B4B, 0x0B4D, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B56, 0x0B57, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B5C, 0x0B5D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B5F, 0x0B61, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B66, 0x0B6F, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0B82, 0x0B83, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0B85, 0x0B8A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B8E, 0x0B90, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B92, 0x0B95, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B99, 0x0B9A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B9C, 0x0B9C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0B9E, 0x0B9F, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0BA3, 0x0BA4, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0BA8, 0x0BAA, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0BAE, 0x0BB5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0BB7, 0x0BB9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0BBE, 0x0BC2, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0BC6, 0x0BC8, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0BCA, 0x0BCD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0BD7, 0x0BD7, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0BE7, 0x0BEF, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0C01, 0x0C03, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0C05, 0x0C0C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C0E, 0x0C10, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C12, 0x0C28, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C2A, 0x0C33, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C35, 0x0C39, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C3E, 0x0C44, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0C46, 0x0C48, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0C4A, 0x0C4D, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0C55, 0x0C56, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0C60, 0x0C61, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C66, 0x0C6F, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0C82, 0x0C83, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0C85, 0x0C8C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C8E, 0x0C90, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0C92, 0x0CA8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0CAA, 0x0CB3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0CB5, 0x0CB9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0CBE, 0x0CC4, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0CC6, 0x0CC8, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0CCA, 0x0CCD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0CD5, 0x0CD6, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0CDE, 0x0CDE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0CE0, 0x0CE1, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0CE6, 0x0CEF, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0D02, 0x0D03, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0D05, 0x0D0C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0D0E, 0x0D10, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0D12, 0x0D28, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0D2A, 0x0D39, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0D3E, 0x0D43, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0D46, 0x0D48, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0D4A, 0x0D4D, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0D57, 0x0D57, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0D60, 0x0D61, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0D66, 0x0D6F, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0E01, 0x0E2E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E30, 0x0E30, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E31, 0x0E31, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0E32, 0x0E33, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E34, 0x0E3A, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0E40, 0x0E45, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E46, 0x0E46, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x0E47, 0x0E4E, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0E50, 0x0E59, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0E81, 0x0E82, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E84, 0x0E84, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E87, 0x0E88, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E8A, 0x0E8A, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E8D, 0x0E8D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E94, 0x0E97, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0E99, 0x0E9F, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EA1, 0x0EA3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EA5, 0x0EA5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EA7, 0x0EA7, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EAA, 0x0EAB, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EAD, 0x0EAE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EB0, 0x0EB0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EB1, 0x0EB1, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0EB2, 0x0EB3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EB4, 0x0EB9, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0EBB, 0x0EBC, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0EBD, 0x0EBD, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EC0, 0x0EC4, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0EC6, 0x0EC6, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x0EC8, 0x0ECD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0ED0, 0x0ED9, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0F18, 0x0F19, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F20, 0x0F29, XML_CTYPE_VALID | XML_CTYPE_DIGIT },
		{ 0x0F35, 0x0F35, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F37, 0x0F37, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F39, 0x0F39, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F3E, 0x0F3E, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F3F, 0x0F3F, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F40, 0x0F47, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0F49, 0x0F69, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x0F71, 0x0F84, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F86, 0x0F8B, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F90, 0x0F95, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F97, 0x0F97, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0F99, 0x0FAD, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0FB1, 0x0FB7, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x0FB9, 0x0FB9, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x10A0, 0x10C5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x10D0, 0x10F6, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1100, 0x1100, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1102, 0x1103, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1105, 0x1107, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1109, 0x1109, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x110B, 0x110C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x110E, 0x1112, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x113C, 0x113C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x113E, 0x113E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1140, 0x1140, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x114C, 0x114C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x114E, 0x114E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1150, 0x1150, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1154, 0x1155, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1159, 0x1159, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x115F, 0x1161, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1163, 0x1163, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1165, 0x1165, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1167, 0x1167, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1169, 0x1169, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x116D, 0x116E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1172, 0x1173, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1175, 0x1175, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x119E, 0x119E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11A8, 0x11A8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11AB, 0x11AB, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11AE, 0x11AF, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11B7, 0x11B8, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11BA, 0x11BA, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11BC, 0x11C2, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11EB, 0x11EB, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11F0, 0x11F0, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x11F9, 0x11F9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1E00, 0x1E9B, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1EA0, 0x1EF9, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F00, 0x1F15, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F18, 0x1F1D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F20, 0x1F45, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F48, 0x1F4D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F50, 0x1F57, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F59, 0x1F59, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F5B, 0x1F5B, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F5D, 0x1F5D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F5F, 0x1F7D, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1F80, 0x1FB4, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FB6, 0x1FBC, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FBE, 0x1FBE, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FC2, 0x1FC4, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FC6, 0x1FCC, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FD0, 0x1FD3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FD6, 0x1FDB, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FE0, 0x1FEC, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FF2, 0x1FF4, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x1FF6, 0x1FFC, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x20D0, 0x20DC, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x20E1, 0x20E1, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x2126, 0x2126, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x212A, 0x212B, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x212E, 0x212E, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x2180, 0x2182, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x3005, 0x3005, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x3007, 0x3007, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_IDEOGRAPHIC },
		{ 0x3021, 0x3029, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_IDEOGRAPHIC },
		{ 0x302A, 0x302F, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x3031, 0x3035, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x3041, 0x3094, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x3099, 0x309A, XML_CTYPE_VALID | XML_CTYPE_COMBINING },
		{ 0x309D, 0x309E, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x30A1, 0x30FA, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x30FC, 0x30FE, XML_CTYPE_VALID | XML_CTYPE_EXTENDER },
		{ 0x3105, 0x312C, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0x4E00, 0x9FA5, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_IDEOGRAPHIC },
		{ 0xAC00, 0xD7A3, XML_CTYPE_VALID | XML_CTYPE_LETTER | XML_CTYPE_BASE },
		{ 0xFDD0, 0xFDDF, XML_CTYPE_INVALID },

		{ 0x1FFFE, 0x1FFFF, XML_CTYPE_INVALID },
		{ 0x2FFFE, 0x2FFFF, XML_CTYPE_INVALID },
		{ 0x3FFFE, 0x3FFFF, XML_CTYPE_INVALID },
		{ 0x4FFFE, 0x4FFFF, XML_CTYPE_INVALID },
		{ 0x5FFFE, 0x5FFFF, XML_CTYPE_INVALID },
		{ 0x6FFFE, 0x6FFFF, XML_CTYPE_INVALID },
		{ 0x7FFFE, 0x7FFFF, XML_CTYPE_INVALID },
		{ 0x8FFFE, 0x8FFFF, XML_CTYPE_INVALID },
		{ 0x9FFFE, 0x9FFFF, XML_CTYPE_INVALID },
		{ 0xAFFFE, 0xAFFFF, XML_CTYPE_INVALID },
		{ 0xBFFFE, 0xBFFFF, XML_CTYPE_INVALID },
		{ 0xCFFFE, 0xCFFFF, XML_CTYPE_INVALID },
		{ 0xDFFFE, 0xDFFFF, XML_CTYPE_INVALID },
		{ 0xEFFFE, 0xEFFFF, XML_CTYPE_INVALID },
		{ 0xFFFFE, 0xFFFFF, XML_CTYPE_INVALID },

		{ 0x10FFFE, 0x10FFFF, XML_CTYPE_INVALID },

		{ 0, 0, 0 }
	};

	const XMLCharCType	*l;

#ifdef MO_DEBUG
// ensure that the table is properly ordered
{
	static long done = false;
	if(!done) {
		done = true;
		l = tbl + 1;
		assert(l[-1].from <= l[-1].to);
		do {
			//printf("Testing %d vs %d\n", l->from, l->to);
			assert(l->from <= l->to);
			//printf("Testing %d vs %d\n", l->from, l[-1].to);
			assert(l->from > l[-1].to);
			l++;
		} while(l->from != 0);
	}
}
#endif
	if(c < 0) {		// this happens when an error is used to call XMLCType()
		return XML_CTYPE_INVALID;
	}
	l = tbl;
	do {
		if(c < l->from) {
			break;
		}
		if(c <= l->to) {
			return l->flags;
		}
		l++;
	} while(l->from != 0);

	return XML_CTYPE_VALID;
}




/** \brief Initialize the state to read an XML document
 *
 * The InitDocument() initializes the stack with a
 * set of function pointers that are used to read
 * the XML document given as input.
 *
 * It also ensures that at least one line of data was read.
 *
 * \note
 * This function ignores any older stack setup by first
 * clearing the current stack.
 */
void moXMLParser::InitDocument(void)
{
//	[ 1]	Document	::=	Prolog Element Misc*
//	[22]	Prolog		::=	XMLDecl? Misc* (DoctypeDecl Misc*)?
//
// Note that we included the Prolog in the Document rule since it is only
// composed of sub-declarations. Don't forget we have to stack everything
// backward (like on a regular stack)
	f_stack.Empty();		// make sure our array is empty
	Push(&moXMLParser::GetMisc);
	Push(&moXMLParser::GetElement);
	Push(&moXMLParser::GetMisc);
	Push(&moXMLParser::GetDoctypeDecl);
	Push(&moXMLParser::GetMisc);
	Push(&moXMLParser::GetXMLDecl);

	// We need to read one line for the parser to work properly
	if(f_input != 0) {
		f_input->XMLNextLine();
	}
}


/** \brief Reads one element.
 *
 * Support [39], [40], [41], [42], [43], [44]
 *
 * This function reads one XML element. If it is called without
 * first having found a '<' character, then it fails with a
 * fatal error.
 *
 * \return The character read or an error code.
 */
int moXMLParser::GetElement(void)
{
//	[39]	Element		::=	EmptyElemTag | STag Content ETag
//	[40]	STag		::=	'<' Name (S Attribute)* S? '>'
//	[41]	Attribute	::=	Name Eq AttValue
//	[42]	ETag		::=	'</' Name S? '>'
//	[43]	Content		::=	CharData? ((Element | Reference | CDSect | PI | Comment) CharData?)*
//	[44]	EmptyElemTag	::=	'<' Name (S Attribute)* S? '/>'
	mowc::wc_t		r;

	Pop();

	// the only difference between GetElement() and GetSTag()
	// is the following test which prevents an invalid root
	// element from being read
	// NOTE: when we enter this function all the <! and <?
	// where already skipped
	r = f_input->XMLGetC();
	if(r != '<') {
		f_input->FormatError(XML_ERRCODE_NOROOT, "root tag expected");
		return XML_RT_FATAL;
	}
	f_input->XMLUngetC();

	Push(&moXMLParser::GetSTag);

	return r;
}


/** \brief Reads and convert reference characters.
 *
 * Support [68], [4], [5]
 *
 * The GetReference() reads once character whatsoever and
 * it expects an & as the very first character (though, if
 * it is not then the character read is returned as is and
 * no error is generated).
 *
 * \todo
 * Support entities from the DTD.
 *
 * \return The character read.
 */
mowc::wc_t moXMLParser::GetReference(void)
{
//	[68]	EntityRef	::=	'&' Name ';'
//	[ 4]	NameChar	::=	Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
//	[ 5]	Name		::=	(Letter | '_' | ':') (NameChar)*
	struct internal_ref_t {
		const char	*f_name;
		mowc::wc_t	f_char;
	};
	// are these in any kind of DTD?
	static const internal_ref_t refs[6] = {
		{ "lt",   L'<' },
		{ "amp",  L'&' },
		{ "gt",   L'>' },
		{ "quot", L'"' },
		{ "apos", L'\'' },

		// end of table
		{ (const char *) 0, L'\0' }
	};

	mowc::wc_t		r, c;
	const internal_ref_t	*n;

	r = f_input->XMLGetC();
	if(r != '&' || f_keep_entities) {
		return r;
	}

	r = f_input->XMLGetC();
	if(r == '#') {
		// a value reference; easy!
		r = f_input->XMLGetC();
		c = 0;
		if(r == 'x') {
			// Hexadecimal value
			r = f_input->XMLGetC();
			if((r >= '0' && r <= '9')
			|| (r >= 'A' && r <= 'F')
			|| (r >= 'a' && r <= 'f')) {
				do {
					c = (c << 4)
						+ (r <= '9' ?  r - '0'
							: (r & 0x5F) - 'A');
					r = f_input->XMLGetC();
				} while((r >= '0' && r <= '9')
					|| (r >= 'A' && r <= 'F')
					|| (r >= 'a' && r <= 'f'));
			}
			else {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid character reference, a digit (0-9/A-F) was expected");
				f_input->XMLUngetC();
				f_input->XMLUngetC();
				f_input->XMLUngetC();
				// we return everything to the caller at this time!
				return '&';
			}
		}
		else if(r >= '0' && r <= '9') {
			do {
				c = c * 10 + r - '0';
				r = f_input->XMLGetC();
			} while(r >= '0' && r <= '9');
		}
		else {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid character reference, a digit (0-9) was expected");
			f_input->XMLUngetC();
			f_input->XMLUngetC();
			// we return everything to the caller at this time!
			return '&';
		}
		if(r != ';') {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid character reference, a digit (0-9) was expected");
			f_input->XMLUngetC();
		}
		return c;
	}

	if(!IsXMLNameChar(r, true)) {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid character reference name");
		f_input->XMLUngetC();
		return '&';
	}

	f_input->XMLUngetC();

	r = GetName();
	if(r < 0) {
		return r;
	}

	if(r != ';') {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "character reference not terminated with ';'");
	}
	else {
		f_input->XMLSkipC();
	}

	// first we check the name against the internally known names
	n = refs;
	while(n->f_name != 0) {
		if(f_input->XMLCurrentString() == n->f_name) {
			return n->f_char;
		}
		n++;
	}

	// TODO: if not an XML default entity
	//	 search for that entity in the DTD!
	f_input->FormatError(XML_ERRCODE_UNDEFINED_CHAR, "unknown character \"%S\"",
					f_input->XMLCurrentString().Data());

	return '?';
}


/** \brief Read one attribute from the input stream.
 *
 * Support for [41], [10], [5], [4]
 *
 * This function reads one name followed by an equal
 * sign and a quoted value. When reading the XML file in
 * non-strict mode, the attribute does not need to have
 * a corresponding value and values do not need to be
 * quoted. An error is generated when a < character
 * appears within the value. You can circumvent this
 * using the &lt; entity instead.
 *
 * \param[out] attr The attribute result are saved in this variable or NULL.
 * \param[in] ref The reference are converted if true.
 *
 * \return The last character read, this can be the quote or the character past the value.
 */
mowc::wc_t moXMLParser::GetAttribute(moVariableSPtr& attr, bool ref)
{
//	[41]	Attribute	::=	Name Eq AttValue
//	[10]	AttValue	::=	'"' ([^<&"] | Reference)* '"' |  "'" ([^<&'] | Reference)* "'"
//	[ 5]	Name		::=	(Letter | '_' | ':') (NameChar)*
//	[ 4]	NameChar	::=	Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
	mowc::wc_t		r, t;

	attr = 0;

	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}
	if(!IsXMLNameChar(r, true)) {
		// it is not an error in this function if
		// we can't find a name (maybe we found the >, /> or ?>)
		return r;
	}

	r = GetName();
	if(r < 0) {
		return r;
	}
	attr = new moVariable(f_input->XMLCurrentString());

	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}
	if(r != '=') {
		if(f_strict) {
			// in strict mode we always expect a value after a name
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid attribute specification, '=' was expected");
		}
		// note that in HTML we don't expect to always have ...=<value>
		return XML_RT_NOERROR;
	}

	// skip the equal as is, and then the spaces after it
	f_input->XMLSkipC();

	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}

	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();

	if(r == '"' || r == '\'') {
		f_input->XMLSkipC();		// skip the quote
		t = f_input->XMLGetC();
		while(t >= 0 && t != r) {
			if(t == '&' && ref) {
				// TODO: replace the reference
				f_input->XMLUngetC();
				t = GetReference();
				// NOTE: t can be & or < or ' or "
			}
			else if((t == '<' || t == '&') && f_strict) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid '%c' within a parameter value", t);
			}
			current_string += t;
			t = f_input->XMLGetC();
		}
	}
	else {
		if(f_strict) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid attribute value, expected quotation (\"...\" or '...')");
		}
		// read up to the next space -- this is very much
		// used in HTML which is standard there
		t = f_input->XMLGetC();
		while(t >= 0 && !IsXMLSpace(t)) {
			if(t == '&' && ref) {
				f_input->XMLUngetC();
				t = GetReference();
				// NOTE: t can be & or < or ' or "
			}
			else if((t == '<' || t == '&') && f_strict) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid '%c' within a parameter value", t);
			}
			current_string += t;
			t = f_input->XMLGetC();
		}
		if(t >= 0) {
			f_input->XMLUngetC();
		}
	}

	*attr = current_string;

	return t;
}



/** \brief Read the XML declaration.
 *
 * Support for [23], [24], [25], [26], [32], [77], [80], [81].
 *
 * This function reads the <?xml version=...> header
 * at the beginning of the file. Note that the tag
 * must exist unless some external definition is given
 * to the parser in order to know the version, encoding
 * etc. of the file. This entry is parsed in a very
 * specific way since it is very specific. It follows
 * 100% the specification (only thing at this time...
 * I'm not sure whether the order of the parameters should
 * be enforced!)
 *
 * \return XML_RT_NOERROR or an error code.
 */
int moXMLParser::GetXMLDecl(void)
{
//	[23]	XMLDecl		::=	'<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
//	[24]	VersionInfo	::=	S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"')
//	[25]	Eq		::=	S? '=' S?
//	[26]	VersionNum	::=	'1.0'
//	[32]	SDDecl		::=	S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
//	[77]	TextDecl	::=	'<?xml' VersionInfo? EncodingDecl S? '?>'
//	[80]	EncodingDecl	::=	S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" )
//	[81]	EncName		::=	[A-Za-z] ([A-Za-z0-9._] | '-')*
	mowc::wc_t		r;
	moVariableSPtr		var;
	char			encoding[256];	/* Flawfinder: ignore */

	Pop();

	if(!f_input->XMLTestString("<?xml")) {
		f_input->FormatError(XML_ERRCODE_NOXMLDECL, "no XML declaration found, this is an error when no external declarations is supplied");
		// TODO: need to skip anything?
		return XML_RT_NOERROR;
	}
	f_input->XMLSkipC(5);

// in the XML header we can't have any reference
	r = GetAttribute(var, false);
	if(r < 0) {
		return r;
	}

// VERSION
	if(var == 0 || var->Name() != "version") {
		f_input->FormatWarning(XML_ERRCODE_NOXMLVERSION, "no XML version (version 1.0 assumed)");
		f_version = 0x10000;
	}
	else {
		if(*var == "1.0") {
			f_version = 0x10000;
		}
		else if(*var == "1.1") {
			f_version = 0x10001;
		}
		else {
			f_input->FormatError(XML_ERRCODE_VERSION, "invalid version, only 1.0 and 1.1 are currently supported");
			// we assume that it will fail to read the rest at this time
			return XML_RT_FATAL;
		}

		r = GetAttribute(var, false);
		if(r < 0) {
			return r;
		}
	}

	encoding[0] = '\0';
	if(var != 0 && var->Name() == "encoding") {
		if(f_input->XMLCurrentString().IsEmpty()) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "an empty encoding name is invalid");
		}
		else {
			r = TestWord(*var, XML_CTYPE_ENCODING_START, XML_CTYPE_ENCODING);
			if(r < 0) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid encoding specification, some characters are unacceptable");
			}
			else {
				// we've got a properly named encoding
				var->MBData(encoding, sizeof(encoding));
			}
		}

		r = GetAttribute(var, false);
		if(r < 0) {
			return r;
		}
	}

	if(!var.IsNull() && var->Name() == "standalone") {
		if(*var == "yes") {
			f_standalone = true;
			// we don't want to keep the possible user defined syntax!
			CleanSyntax();
		}
		else if(*var == "no") {
			f_standalone = false;
		}
		else {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid standalone specification, only 'yes' or 'no' is acceptable");
		}

		r = GetAttribute(var, false);
		if(r < 0) {
			return r;
		}
	}

	// skip possible spaces before to test for "?>"
	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}

	if(!f_input->XMLTestString("?>")) {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "\"?>\" was expected at the end of the XML header");
		return XML_RT_ERROR;
	}
	f_input->XMLSkipC(2);

	if(encoding[0] != '\0') {
		if(f_input->XMLSetInputEncoding(encoding)) {
			f_encoding = encoding;
		}
		else {
			f_input->FormatError(XML_ERRCODE_UNKNOWN_ENCODING, "unknown encoding \"%hhs\"", encoding);
		}
	}

	return XML_RT_NOERROR;
}


/** \brief Read the DOCTYPE declaration.
 *
 * Support for [28], [75], [83], [12], [11]
 *
 * This function reads the <!DOCTYPE ...> declaration. This includes the name
 * of the DTD and where to find that DTD.
 *
 * This information is considered internal and it not directly returned to
 * the caller. It can be queried with different other functions such as
 * moXMLParser::Syntax().
 *
 * \return XML_RT_NOERROR or an error code.
 */
int moXMLParser::GetDoctypeDecl(void)
{
//	[28]	DoctypeDecl	::=	'<!DOCTYPE' S Name (S ExternalID)? S? ('[' IntSubset ']' S?)? '>'
//	[75]    ExternalID	::=	('SYSTEM' | PublicID) S SystemLiteral
//	[83]	PublicID	::=	'PUBLIC' S PubidLiteral
//	[12]	PubidLiteral	::=	'"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
//	[11]	SystemLiteral	::=	('"' [^"]* '"') | ("'" [^']* "'")
	mowc::wc_t		r, t;
	bool			has_system_literal;

	Pop();

	// any document declaration?
	if(!f_input->XMLTestString("<!DOCTYPE")) {
		if(f_standalone) {
			f_input->FormatWarning(XML_ERRCODE_NOXMLVERSION, "no <!DOCTYPE ...> definition found in a standalone XML file");
		}
		return XML_RT_NOERROR;
	}
	f_input->XMLSkipC(9);

	// the document name
	r = GetName(SPACES_ONE_OR_MORE);
	if(r < 0) {
		return r;
	}
	f_current_syntax = f_input->XMLCurrentString();

	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}

	has_system_literal = true;
	if(f_input->XMLTestString("PUBLIC")) {
		// read the PubID
		f_input->XMLSkipC(6);
		r = f_input->XMLSkipSpaces();
		if(r < 0) {
			return r;
		}
		if(r == '"') {
			f_input->XMLSkipC();
			t = GetWord(XML_CTYPE_PUBLIC_ID_DQ, XML_CTYPE_PUBLIC_ID_DQ);
			if(t != r) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid PUBLIC identifier, expected '\"' to close the identifier");
			}
			else {
				f_input->XMLSkipC();
			}
		}
		else if(r == '\'') {
			f_input->XMLSkipC();
			t = GetWord(XML_CTYPE_PUBLIC_ID, XML_CTYPE_PUBLIC_ID);
			if(t != r) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid PUBLIC identifier, expected \"'\" to close the identifier");
			}
			else {
				f_input->XMLSkipC();
			}
		}
		else {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid PUBLIC identifier, we expected a quoted identifier");
			return XML_RT_ERROR;
		}
		if(t < 0) {
			return t;
		}
		// we got a public identification string
		// TODO: ...
		f_current_identification = f_input->XMLCurrentString();
	}
	else if(f_input->XMLTestString("SYSTEM")) {
		f_input->XMLSkipC(6);
	}
	else {
		//f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid DOCTYPE, expected PUBLIC or SYSTEM");
		//return XML_RT_FATAL;
		has_system_literal = false;
	}

	if(has_system_literal) {
		r = f_input->XMLSkipSpaces();
		if(r < 0) {
			return r;
		}
		if(r == '\'' || r == '"') {
			f_input->XMLSkipC();
			moWCString& current_string = f_input->XMLCurrentString();
			current_string.Empty();
			t = f_input->XMLGetC();
			while(t >= 0 && t != r) {
				current_string += t;
				t = f_input->XMLGetC();
			}
			f_dtd_url = f_input->XMLCurrentString();
		}
		else {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid DOCTYPE, expected a system literal after PUBLIC or SYSTEM");
		}
	}

	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}
	if(r == '[') {
		f_input->XMLSkipC();
		// Add the necessary functions to read the set of
		// DOCTYPE instructions.
		Push(&moXMLParser::GetIntSubset);
		Push(&moXMLParser::GetAnySubset);
		return XML_RT_NOERROR;
	}

	if(r != '>') {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "DOCTYPE is expected to terminate with '>'");
		return XML_RT_FATAL;
	}

	// skip the '>'
	f_input->XMLSkipC();

	if(!f_dtd_url.IsEmpty() && f_read_dtd) {
		if(f_dtd_url.Compare("file:///", 0, 8) == MO_BASE_COMPARE_EQUAL) {
			// we can read local files!
			moFileSPtr file;
			file = new moFile;
			if(file->Open(f_dtd_url.Get(7, -1))) {
				// insert the DTD
				moXMLParser::moXMLStreamSPtr stream = new moXMLParser::moXMLStream(static_cast<moFile *>(file));
				stream->XMLInterpretVariables(f_input->XMLGetInterpretVariables());
				Include(*stream);
				Push(&moXMLParser::GetAnySubset);
			}
			else {
				// ignore failures?
				f_input->FormatError(XML_ERRCODE_IO, "DTD file \"%S\" cannot be opened", f_dtd_url.Data());
			}
		}
		else {
			// TODO: ... read the system DTD (need network, etc.)
		}
	}

	return XML_RT_NOERROR;
}


/** \brief Read a DTD.
 *
 * Support [28a], [28b], [29], [30], [31], [61], [62], [63], [64], [65]
 *
 * This function is used to read the DTD definitions.
 *
 * It supports \<!ELEMENT ...>, \<!ATTLIST ...> and \<!ENTITY ...>.
 *
 * It does not yet support the \<!NOTATION ...> definitions.
 *
 * The parser understands the conditional \<[IGNORE[...]]> and
 * \<[INCLUDE[...]]> tags.
 *
 * It understands and reads the PITargets, though it does not do anything
 * with them (\<?...?>).
 *
 * It silently skips comments and empty lines.
 *
 * Anything else makes this function call Pop() and return. Especially,
 * it will return whenever it find the "]]>" sequence and it is not within
 * and IGNOREd or INCLUDEd sub-section.
 *
 * \return XML_RT_NOERROR or an error.
 */
int moXMLParser::GetAnySubset(void)
{
//	[28a]	DeclSep		::=	PEReference | S
//	[28b]	IntSubset	::=	(MarkupDecl | DeclSep)*
//	[29]	MarkupDecl	::=	ElementDecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment
//	[30]	ExtSubset	::=	TextDecl? ExtSubsetDecl
//	[31]	ExtSubsetDecl	::=	( MarkupDecl | ConditionalSect | DeclSep)*
//	[61]	ConditionalSect	::=	IncludeSect | IgnoreSect
//	[62]	IncludeSect	::=	'<![' S? 'INCLUDE' S? '[' ExtSubsetDecl ']]>'
//	[63]	IgnoreSect	::=	'<![' S? 'IGNORE' S? '[' IgnoreSectContents* ']]>'
//	[64]	IgnoreSectContents::=	Ignore ('<![' IgnoreSectContents ']]>' Ignore)*
//	[65]	Ignore		::=	Char* - (Char* ('<![' | ']]>') Char*)
	mowc::wc_t		r;

	for(;;) {
		r = GetPEReference(true, true, true);
		if(r < 0) {
			return r;
		}
		if(!f_input->XMLTestString("]]>")) {
			break;
		}
		// ending an INCLUDE conditional
		f_input->XMLSkipC(3);
		if(f_cond_include == 0) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "']]>' not expected, an <![INCLUDE[...]]> conditional was expected");
		}
		else {
			--f_cond_include;
		}
	}

//fprintf(stderr, "TEXT STRING [%S]\n", f_input->XMLTestString("<!ELEMENT"));
	if(f_input->XMLTestString("<!ELEMENT")) {
		Push(&moXMLParser::GetElementDecl);
		return XML_RT_NOERROR;
	}
	if(f_input->XMLTestString("<!ATTLIST")) {
		Push(&moXMLParser::GetAttListDecl);
		return XML_RT_NOERROR;
	}
	if(f_input->XMLTestString("<!ENTITY")) {
		Push(&moXMLParser::GetEntityDecl);
		return XML_RT_NOERROR;
	}
	if(f_input->XMLTestString("<!NOTATION")) {
		// TODO: implement the <!NOTATION ...>
		//Push(&moXMLParser::GetNotationDecl);
fprintf(stderr, "INTERNAL ERROR: <!NOTATION ...> not supported yet.\n");
abort();
		return XML_RT_NOERROR;
	}
	if(f_input->XMLTestString("<![")) {
		// not valid in internal decl. in strict mode
		// but we still want to skip those anyway!
		Push(&moXMLParser::GetConditionalSect);
		return XML_RT_NOERROR;
	}
	if(f_input->XMLTestString("<?")) {
		// what do we do with that in the middle of a DTD?!
		Push(&moXMLParser::GetPI);
		return XML_RT_NOERROR;
	}
	if(f_input->XMLTestString("<!--")) {
		Push(&moXMLParser::GetComment);
		return XML_RT_NOERROR;
	}

	Pop();

	return XML_RT_NOERROR;
}


/** \brief Read the end of an internal DTD definition.
 *
 * This function expects ']]>' that is used to close the DTD
 * declaration. It is used whenever the DOCTYPE declaration
 * says that the DTD follows right there.
 *
 * \return XML_RT_NOERROR or an error code.
 */
int moXMLParser::GetIntSubset(void)
{
	// make sure we have ']' S? '>' to close the DOCTYPE
	mowc::wc_t		r;

	r = f_input->XMLGetC();
	if(r != ']') {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "'] S? >' expected at the end of an internal DTD declaration in a DOCTYPE, ']' missing");
		if(r == '>') {
			f_input->XMLGetC();
		}
	}
	else {
		r = f_input->XMLSkipSpaces();
		if(r < 0) {
			return r;
		}
		if(r != '>') {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "'] S? >' expected at the end of an internal DTD declaration in a DOCTYPE, '>' missing");
		}
		else {
			f_input->XMLSkipC();
		}
	}

	
	Pop();

	return XML_RT_NOERROR;
}


/** \brief Read a list of declaration names.
 *
 * This function is used to support HTML \<!ELEMENT ...>
 * and \<!ATTLIST ...> definitions that can define multiple
 * elements in one go.
 *
 * The function reads one name when it appears as is: Name.
 *
 * It reads as many names as found when defined between
 * parenthesis: '(' Name ( '|' Name ) * ')'.
 *
 * The list of names cannot be empty or an error is issued.
 *
 * \param[out] decl_names The list of names read by this function.
 * \param[in] decl The name of the declaration calling this function to display in error messages.
 *
 * \return XML_RT_NOERROR or an error code.
 */
int moXMLParser::GetDeclNames(moSortedListUniqueOfWCStrings& decl_names, const char *decl)
{
	moWCString				name;
	mowc::wc_t				r;

	// skip the declaration (i.e. "<!ELEMENT")
	f_input->XMLSkipC(static_cast<int>(strlen(decl)));

	// skip spaces; but we want at least one space
	r = XMLGetC();
	if(IsXMLSpace(r)) {
		r = XMLSkipSpaces();
	}
	else if(r != '(') {
		// NOTE: we do not even like '%' right after the declaration keyword
		if(!IsXMLNameChar(r, true)) {
			// not even a valid name?!
			f_input->FormatError(XML_ERRCODE_BADNAME, "invalid character (&#x%lX;) instead of a space before an XML name", r);
			return XML_RT_FATAL;
		}
		f_input->FormatError(XML_ERRCODE_BADNAME, "a space was expected before an XML name");
	}

	// the element name--since it can be an entity,
	// we must read it "by hand" (cannot call GetName())
	while(r == '%') {
		r = GetPEReference(false, false, false);
	}
	if(r == '(') {
		// multiple names... we can find | to separate names
		// and ) to end the list
		f_input->XMLSkipC();
		r = GetPEReference(true, false, true);
		f_input->XMLSkipC();
		for(;;) {
			while(r == ')' || r == '|' || IsXMLSpace(r)) {
				// the name ends here
				if(name.IsEmpty()) {
					f_input->FormatError(XML_ERRCODE_BADNAME, "%s (...) ...> declaration includes an empty name", decl);
				}
				else {
					if(!decl_names.Insert(new moWCString(f_strict ? name : name.Uppercase()))) {
						f_input->FormatError(XML_ERRCODE_BADNAME, "%s (...) ...> declaration defines %S twice", decl, name.Data());
					}
					name.Empty();
				}
				if(IsXMLSpace(r)) {
					r = GetPEReference(true, true, true);
					f_input->XMLSkipC();
				}
				if(r == ')') {
					if(decl_names.IsEmpty()) {
						f_input->FormatError(XML_ERRCODE_BADNAME, "an empty %s () ...> declaration is not valid; at least one name was expected",decl);
					}
					return XML_RT_NOERROR;
				}
				if(r != '|') {
					f_input->FormatError(XML_ERRCODE_BADNAME, "%s (...) ...> declaration expects a | to separate different names", decl);
				}
				else {
					r = GetPEReference(true, true, true);
					f_input->XMLSkipC();
				}
			}
			if(!IsXMLNameChar(r, name.IsEmpty())) {
				// not a valid name...
				f_input->FormatError(XML_ERRCODE_BADNAME, "invalid character (&#x%lX;) for an XML name", r);
				return XML_RT_FATAL;
			}
			name += r;
			r = XMLGetC();
			while(r == '%') {
				r = GetPEReference(false, false, false);
				f_input->XMLSkipC();
			}
		}
	}
	else {
		// well, in this case, let's make it simple!
		// (I do not think that blah%test;foo would be used here so simplify the call!)
		// NOTE: we may want to do something in that regard since we should support
		//	 entities everywhere, really.
		//
		// Space wise, we already checked that one space was present.
		r = GetName(SPACES_NONE);
		if(r < 0) {
			return r;
		}
		const moWCString& name = f_input->XMLCurrentString();
		decl_names.Insert(new moWCString(f_strict ? name : name.Uppercase()));
	}

	return XML_RT_NOERROR;
}


/** \brief Read an element declaration.
 *
 * Support [45], [46], [47], [48], [49], [50], [51]
 *
 * This function reads one \<!ELEMENT ...> definition. Note that since
 * we support the HTML DTD, one element definition can name multiple
 * elements (i.e. (SUB|SUP) are both defined in one go.)
 *
 * The HTML DTD also includes inline comments supported by this function.
 *
 * Most entries within an \<!ELEMENT ...> can be DTD references
 * (i.e. \%name;).
 *
 * \return XML_RT_NOERROR or an error code.
 *
 * \sa moXMLParser::GetDeclNames(moSortedListUniqueOfWCStrings& decl_names, const char *decl)
 */
int moXMLParser::GetElementDecl(void)
{
//	[45]	ElementDecl	::=	'<!ELEMENT' S Name S ContentSpec S? '>'
//	[46]	ContentSpec	::=	'EMPTY' | 'ANY' | Mixed | Children
//	[47]	Children	::=	(Choice | Seq) ('?' | '*' | '+')?
//	[48]	CP		::=	(Name | Choice | Seq) ('?' | '*' | '+')?
//	[49]	Choice		::=	'(' S? CP ( S? '|' S? CP )+ S? ')'
//	[50]	Seq		::=	'(' S? CP ( S? ',' S? CP )* S? ')'
//	[51]	Mixed		::=	'(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')'
	moXMLElement::moXMLEntry::repeat_t	repeat;
	moXMLElement::moXMLEntrySPtr		entry;
	moXMLElementSPtr			element;
	moSortedListUniqueOfXMLElements		elements_list;
	moSortedListUniqueOfWCStrings		element_names;
	mowc::wc_t				r, mode;
	moList::position_t			idx, pos;

	//
	// The HTML DTD allows extra features:
	//
	//	The element name can be an entity (this way the entity can be used in the
	//	element definition and later in other elements references)
	//
	//	The element name can be a list in which case it is defined between
	//	parenthesis and each name is separated by a | operator.
	//
	//	The element name is followed by two characters separated by spaces that specify
	//	whether the start and end tags are required or not. '-' means that it is required
	//	and 'O' (capital letter O) means that the tag is not required. The combinaison
	//	"O -" is not possible. You may however encounter "- -", "- O" and "O O".
	//
	//	Also, an element can end with a comment.
	//
	// '<!ELEMENT' S ElemName (S ('-'|'O') S ('-'|'O'))? S ContentSpec (S '-- ... --' | S?) '>'
	//

	//assert(f_input->XMLTestString("<!ELEMENT"));

	Pop();

	r = GetDeclNames(element_names, "<!ELEMENT");
	if(r < 0) {
		return r;
	}

	for(idx = 0; idx < element_names.Count(); ++idx) {
		element = new moXMLElement(*element_names.Get(idx));
		pos = f_dtd_elements.Find(element);
		if(pos == moList::NO_POSITION) {
			f_dtd_elements.Insert(element);
			elements_list.Insert(element);
		}
		else {
			// watch out! we need to make sure it is unique
			// (i.e. only one entry)
			element = f_dtd_elements.Get(pos);
			entry = element->GetEntry();
			if(entry) {
				// you cannot have two elements with the same name
				f_input->FormatError(XML_ERRCODE_DEFINED_TWICE, "found two <!ELEMENT %S ...> declarations", element->GetName().Data());
			}
			else {
				// its attributes were defined first, now define the entry
				elements_list.Insert(element);
			}
		}
	}
	// if errors occur, the list can be empty here
	if(elements_list.IsEmpty()) {
		// here the name does not matter, the element will be dropped anyway
		elements_list.Insert(new moXMLElement("UNDEFINED"));
	}

	r = GetPEReference(true, true, true);
	if(r < 0) {
		return r;
	}

	// get the first element to setup the entries
	element = elements_list.Get(0);

	// If the HTML DTD is being read, then we have '-' and/or 'O' here.
	// This is easy since you can otherwise get '(', 'EMPTY' or 'ANY'.
	// In otherwise words, 'O' or '-' must be the representation of
	// optional or required or there is an error anyway.
	if(r == 'O') {
		// check the next character, if space, it certainly is the Optional mark
		f_input->XMLSkipC();
		r = XMLGetC();
		if(!IsXMLSpace(r)) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "expected a space after the first 'O' in <!ELEMENT %S O O ...> declaration", element->GetName().Data());
		}
		else {
			r = XMLSkipSpaces();
			f_input->XMLSkipC();
		}
		if(r == '-') {
			f_input->FormatError(XML_ERRCODE_INVALID, "found <!ELEMENT %S O - ...> declaration that is not supported, the start tag only cannot be optional", element->GetName().Data());
			r = XMLGetC();
		}
		else if(r != 'O') {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "the <!ELEMENT %S O ? ...> declaration expected a second 'O' instead of '%C'", element->GetName().Data(), r);
		}
		else {
			r = XMLGetC();
		}
		if(!IsXMLSpace(r)) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "expected a space after the second 'O' in <!ELEMENT %S O O ...> declaration", element->GetName().Data());
			f_input->XMLUngetC();
		}
		else {
			r = XMLSkipSpaces();
		}
		element->SetOptionalTags(true, true);
		if(r == '%') {
			// we actually want to skip the '%' here
			f_input->XMLSkipC();
			r = GetPEReference(false, false, true);
		}
	}
	else if(r == '-') {
		// check the next character, if space, it certainly is the Optional mark
		f_input->XMLSkipC();
		r = XMLGetC();
		if(!IsXMLSpace(r)) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "expected a space after the first 'O' in <!ELEMENT %S O O ...> declaration", element->GetName().Data());
		}
		else {
			r = XMLSkipSpaces();
			f_input->XMLSkipC();
		}
		if(r == '-') {
			element->SetOptionalTags(false, false);
			r = XMLGetC();
		}
		else if(r != 'O') {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "the <!ELEMENT %S - ? ...> declaration expected a second '-' or an 'O' instead of '%C'", element->GetName().Data(), r);
		}
		else {
			element->SetOptionalTags(false, true);
			r = XMLGetC();
		}
		if(!IsXMLSpace(r)) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "expected a space after the second 'O' or '-' in <!ELEMENT %S - - ...> declaration", element->GetName().Data());
			f_input->XMLUngetC();
		}
		else {
			r = XMLSkipSpaces();
		}
		if(r == '%') {
			// we actually want to skip the '%' here
			f_input->XMLSkipC();
			r = GetPEReference(false, false, true);
		}
	}

	entry = new moXMLElement::moXMLEntry;
	if(r == '(') {	// Mixed or Children
		// Inside here we expect a list of:
		//	names
		//	#PCDATA
		//	'(' ')' '*' '?' '+' '|' ','
		//	'>' ends the list (valid if paren = 0)
		f_input->XMLSkipC();
		r = GetChildren(element, entry);
		if(r >= 0) {
			if(r != ')') {
				// invalid '(' ... ')' sequence
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a missing ')' was expected in <!ELEMENT %S ...> declaration", element->GetName().Data());
			}
			// in HTML you can have:  (foor) +(more)
			// thus spaces before + cannot be skipped in this case!
			repeat = GetRepeat(entry, false);
			if(entry->HasPCDATA() && f_strict) {
				// ensure proper Mixed
				if(entry->Count() == 1) {
					if(repeat != moXMLElement::moXMLEntry::REPEAT_ONCE) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "#PCDATA cannot be repeated when used by itself in <!ELEMENT %S ...> declaration", element->GetName().Data());
					}
				}
				else {
					if(repeat != moXMLElement::moXMLEntry::REPEAT_ANY) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "an entry starting with a #PCDATA must be repeated any number of times (*) in <!ELEMENT %S ...> declaration", element->GetName().Data());
					}
				}
			}
		}
	}
	else if(f_input->XMLTestString("EMPTY")) {
		entry->SetType(moXMLElement::moXMLEntry::ENTRY_EMPTY);
		f_input->XMLSkipC(5);
	}
	else if(f_input->XMLTestString("CDATA")) {
		// This is for the HTML DTD--Is CDATA the same as PCDATA in this case?
		// At least, we can see that it is not in a sub-entry so it is different
		// from a proper (#PCDATA)
		entry->SetType(moXMLElement::moXMLEntry::ENTRY_PCDATA);
		f_input->XMLSkipC(5);

		// Whenever this happens, add the tag as a CDATA tag by calling
		// our AddCDataTag() function. (We could also transform the
		// IsCDataTag() call to check the DTD.)
		for(idx = 0; idx < element_names.Count(); ++idx) {
			AddCDataTag(*element_names.Get(idx));
		}
	}
	else if(f_input->XMLTestString("ANY")) {
		entry->SetType(moXMLElement::moXMLEntry::ENTRY_ANY);
		f_input->XMLSkipC(3);
	}
	else {
		// Note: in this error, do not mention CDATA since it is not valid XML
		f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid content specification in <!ELEMENT %S ...> declaration, one of EMPTY, ANY or '(' was expected", element->GetName().Data());
		return XML_RT_FATAL;
	}

	element->SetEntry(entry);

	if(r >= 0) {
		r = XMLSkipSpaces();

		// HTML DTD includes support for +(<names>) and -(<names>)
		// We expect one or the other in any one element
		if(r == '-' || r == '+') {
			mode = r;
			f_input->XMLSkipC();
			r = XMLGetC();
			if(r == '(') {
				// Here only a list of names is permitted (<name1>|<name2>|...<nameN>)
				// GetChildren() will accept sub-lists and repeatition though...
				entry = new moXMLElement::moXMLEntry;
				r = GetChildren(element, entry);
				if(r >= 0) {
					if(r != ')') {
						// invalid '(' ... ')' sequence
						f_input->FormatError(XML_ERRCODE_SYNTAX, "a missing ')' was expected in <!ELEMENT %S ...> declaration", element->GetName().Data());
					}
					else {
						r = XMLSkipSpaces();
					}
				}
				if(mode == '-') {
					element->SetExceptEntry(entry);
				}
				else {
					element->SetAdditionalEntry(entry);
				}
			}
			else {
				f_input->XMLUngetC();
				f_input->XMLUngetC();
				r = mode;
			}
		}

		if(r == '-') {
			// ending comment?
			f_input->XMLSkipC();
			r = XMLGetC();
			if(r == '-') {
				// yes! skip comment up to another --
				do {
					r = XMLGetC();
					if(r == '-') {
						r = XMLGetC();
						if(r == '-') {
							// skip ends here, skip spaces before >
							r = XMLSkipSpaces();
							break;
						}
					}
				} while(r >= 0);
				// the following tests will take care of the errors if any
			}
			else {
				f_input->UngetC(r);
				f_input->UngetC('-');
				r = '-';
			}
		}
		if(r < 0) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "missing '>' to close an <!ELEMENT %S ...> declaration", element->GetName().Data());
			return r;
		}
		if(r != '>') {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "missing '>' to close an <!ELEMENT %S ...> declaration", element->GetName().Data());
			return XML_RT_ERROR;
		}
		f_input->XMLSkipC();
	}

	// now, the first element is set, copy its data to the others
	for(idx = 1; idx < elements_list.Count(); ++idx) {
		elements_list.Get(idx)->CopyEntriesFrom(element);
	}

	return XML_RT_NOERROR;
}


/** \brief Read a list of children names.
 *
 * Support [49], [50]
 *
 * This function is used to support the reading of names in a list
 * with optional sub-lists. Names can be separated by pipes (|)
 * in which case it is a list of choices; commas (,) in which case
 * it is a sequence; ampersand (&) in which case it is a set of
 * items that must all appear in any order.
 *
 * Any item can be followed by a repeatition character (*, + and ?).
 *
 * And sub-lists are defined between parenthesis.
 *
 * \param[in] element The element of which these children are being read
 * \param[in] entry The entry being filled
 *
 * \return The next character when no error occured;
 *	the XML_RT_EOF marker;
 *	or an error code (negative)
 */
int moXMLParser::GetChildren(moXMLElement *element, moXMLElement::moXMLEntry *entry)
{
//	[49]	Choice		::=	'(' S? CP ( S? '|' S? CP )+ S? ')'
//	[50]	Seq		::=	'(' S? CP ( S? ',' S? CP )* S? ')'
//	????	All		::=	'(' S? CP ( S? '&' S? CP )+ S? ')' -- HTML support
	moXMLElement::moXMLEntrySPtr		sub_entry;
	moXMLElement::moXMLEntry::repeat_t	repeat;
	mowc::wc_t				r, sep;
	int					has_sub_entries;

	has_sub_entries = 0;
	sep = '\0';
	for(;;) {
		r = GetPEReference(true, true, true);
		if(r < 0) {
			return r;
		}
		r = XMLGetC();
		if(r < 0) {
			if(r == XML_RT_EOF) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a missing ') S? >' was expected at the end of <!ELEMENT %S ...> declaration", element->GetName().Data());
				return XML_RT_ERROR;
			}
			return r;
		}
		if(r == '>') {
			// this should not occur! only ')'
			f_input->XMLUngetC();
			return r;
		}
		if(r == ')') {
			return r;
		}
		if(r == '(') {
			// NOTE: sub-lists don't have names
			sub_entry = new moXMLElement::moXMLEntry;
			r = GetChildren(element, sub_entry);
			if(r != ')') {
				// invalid '(' ... ')' sequence
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a missing ')' was expected in <!ELEMENT %S ...> declaration", element->GetName().Data());
				return XML_RT_ERROR;
			}
			repeat = GetRepeat(entry);
			// TODO: test #PCDATA?
			if(sub_entry->Count() == 0) {
				// an empty "choice" or "sequence" is not a valid entry
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a choice or sequence name was expected in <!ELEMENT %S ...> declaration", element->GetName().Data());
			}
			if(sub_entry->HasPCDATA() && f_strict) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "you cannot mix sub-choices and the use of #PCDATA in <!ELEMENT %S ...> declaration", element->GetName().Data());
				has_sub_entries = 2;
			}
			else if(has_sub_entries == 0) {
				has_sub_entries = 1;
			}
		}
		else if(r == '#') {
			r = GetName(SPACES_NONE);
			if(r < 0) {
				return r;
			}
			sub_entry = new moXMLElement::moXMLEntry;
			sub_entry->SetType(moXMLElement::moXMLEntry::ENTRY_PCDATA);
			if(f_input->XMLCurrentString() != "PCDATA") {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "only #PCDATA is accepted as an identifier starting with '#' in <!ELEMENT %S ...> declaration", element->GetName().Data());
			}

			if(entry->Count() != 0 && f_strict) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "#PCDATA can only appear as the first element of <!ELEMENT %S ...> declaration", element->GetName().Data());
			}
			if(has_sub_entries == 1 && f_strict) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "you cannot mix sub-choices and the use of #PCDATA in <!ELEMENT %S ...> declaration", element->GetName().Data());
				has_sub_entries = 2;
			}

			repeat = GetRepeat(sub_entry);
			if(repeat != moXMLElement::moXMLEntry::REPEAT_ONCE) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a #PCDATA can only be repeated once in <!ELEMENT %S ...> declaration", element->GetName().Data());
			}
		}
		else {
			// NOTE: names don't have sub-lists
			f_input->XMLUngetC();
			r = GetName();
			if(r < 0) {
				return r;
			}
			sub_entry = new moXMLElement::moXMLEntry;
			sub_entry->SetType(moXMLElement::moXMLEntry::ENTRY_NAME);
			sub_entry->SetName(f_input->XMLCurrentString());

			element->AddEntryName(f_input->XMLCurrentString());

			GetRepeat(sub_entry);
		}

		r = GetPEReference(true, true, true);
		if(r < 0) {
			return r;
		}
		if(r == '|') {
			// we found a choice
			if(sep != '\0' && sep != r) {
				f_input->FormatError(XML_ERRCODE_INVALID, "choices and sequences cannot be mixed in <!ELEMENT %S ...> declaration, expected a ',' or '&' instead of a '|'", element->GetName().Data());
			}
			else {
				entry->SetType(moXMLElement::moXMLEntry::ENTRY_CHOICE);
				sep = r;
			}
			f_input->XMLSkipC();
		}
		else if(r == ',') {
			// we found a sequence
			if(sep != '\0' && sep != r) {
				f_input->FormatError(XML_ERRCODE_INVALID, "choices and sequences cannot be mixed in <!ELEMENT %S ...> declaration, expected a '|' or '&' instead of a ','", element->GetName().Data());
			}
			else {
				if(sub_entry->GetType() == moXMLElement::moXMLEntry::ENTRY_PCDATA && f_strict) {
					f_input->FormatError(XML_ERRCODE_INVALID, "a #PCDATA can only be in a choice list, not a sequence in <!ELEMENT %S ...> declaration, expected a '|' instead of a ','", element->GetName().Data());
				}
				entry->SetType(moXMLElement::moXMLEntry::ENTRY_SEQUENCE);
				sep = r;
			}
			f_input->XMLSkipC();
		}
		else if(r == '&') {
			// HTML extension
			// we found a unordered sequence (all choices required, order not required)
			if(sep != '\0' && sep != r) {
				f_input->FormatError(XML_ERRCODE_INVALID, "choices and sequences cannot be mixed in <!ELEMENT %S ...> declaration, expected a '|' or ',' instead of a '&'", element->GetName().Data());
			}
			else {
				if(sub_entry->GetType() == moXMLElement::moXMLEntry::ENTRY_PCDATA) {
					f_input->FormatError(XML_ERRCODE_INVALID, "a #PCDATA can only be in a choice list, not a sequence in <!ELEMENT %S ...> declaration, expected a '|' instead of a '&'", element->GetName().Data());
				}
				entry->SetType(moXMLElement::moXMLEntry::ENTRY_UNORDERED);
				sep = r;
			}
			f_input->XMLSkipC();
		}
		else {
			if(r != ')' && r != '>') {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "choices in <!ELEMENT %S ...> declaration need to be separated by ',', '|' or '&', got '%C' instead", element->GetName().Data(), r);
			}
			if(sep == '|') {
				entry->SetType(moXMLElement::moXMLEntry::ENTRY_CHOICE);
			}
			else if(sep == '&') {
				entry->SetType(moXMLElement::moXMLEntry::ENTRY_UNORDERED);
			}
			else /*if(sep == ',')*/ {
				// this is the default when there was no '|', '&' or ','
				entry->SetType(moXMLElement::moXMLEntry::ENTRY_SEQUENCE);
			}
		}

		entry->AddSubEntry(sub_entry);
	}
	/*NOTREACHED*/
}


/** \brief Read a repetition character.
 *
 * Support [47 [48]
 *
 * This function reads one character and if it is a repetition
 * character transforms it into the corresponding entry repetition.
 * The result is saved in the specified entry object.
 *
 * If the character is not a repetition character, then it is
 * pushed back in the stream.
 *
 * \param[in] entry The entry object that receives the repetition information.
 * \param[in] accept_spaces Whether the function can accept spaces before the repetition character.
 *
 * \return The repeatition of the item.
 */
moXMLParser::moXMLElement::moXMLEntry::repeat_t moXMLParser::GetRepeat(moXMLElement::moXMLEntry *entry, bool accept_spaces)
{
//	[47]	Children	::=	(Choice | Seq) ('?' | '*' | '+')?
//	[48]	CP		::=	(Name | Choice | Seq) ('?' | '*' | '+')?
	moXMLElement::moXMLEntry::repeat_t	repeat;
	mowc::wc_t				r;

	// in strict mode we don't accept spaces, otherwise we're a bit lax
	if(f_strict || !accept_spaces) {
		r = XMLGetC();
	}
	else {
		r = XMLSkipSpaces();
		f_input->XMLSkipC();
	}
	if(r < 0) {
		// some default on error...
		return moXMLElement::moXMLEntry::REPEAT_ONCE;
	}
	switch(r) {
	case '*':
		repeat = moXMLElement::moXMLEntry::REPEAT_ANY;
		break;

	case '+':
		repeat = moXMLElement::moXMLEntry::REPEAT_ONE_OR_MORE;
		break;

	case '?':
		repeat = moXMLElement::moXMLEntry::REPEAT_ZERO_OR_ONE;
		break;

	default:
		f_input->XMLUngetC();
		repeat = moXMLElement::moXMLEntry::REPEAT_ONCE;
		break;

	}

	entry->SetRepeat(repeat);

	return repeat;
}


/** \brief Read an \<!ATTLIST ...> tag.
 *
 * Support [7], [10], [52], [53], [54], [55], [56], [57], [58], [59], [60]
 *
 * This function reads a list of attributes for the specified elements.
 *
 * Since the adding support for the HTML DTD, this function can define the
 * attributes of multiple elements at the same time.
 *
 * Most entries in an \<!ATTLIST ...> definition can be a DTD entity.
 * (i.e. \%inline;)
 *
 * \bug
 * The type of an attribute must be known by the parser for the parser
 * to accept it. These types are converted to an enumeration. This means
 * DTD using their own special types cannot be read.
 *
 * \return XML_RT_NOERROR or an error code if an error occurs
 */
int moXMLParser::GetAttListDecl(void)
{
//	[ 7]	Nmtoken		::=	(NameChar)+
//	[10]	AttValue	::=	'"' ([^<&"] | Reference)* '"' |  "'" ([^<&'] | Reference)* "'"
//	[52]	AttlistDecl	::=	'<!ATTLIST' S Name AttDef* S? '>'
//	[53]	AttDef		::=	S Name S AttType S DefaultDecl
//	[54]    AttType		::=	StringType | TokenizedType | EnumeratedType
//	[55]    StringType	::=	'CDATA'
//	[56]    TokenizedType	::=	'ID' | 'IDREF' | 'IDREFS' | 'ENTITY' | 'ENTITIES' | 'NMTOKEN' | 'NMTOKENS'
//	[57]	EnumeratedType	::=	NotationType | Enumeration
//	[58]    NotationType    ::=	'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
//	[59]	Enumeration	::=	'(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'
//	[60]	DefaultDecl	::=	'#REQUIRED' | '#IMPLIED' | (('#FIXED' S)? AttValue)
	moSortedListUniqueOfWCStrings		element_names;
	moSortedListUniqueOfXMLElements		elements_list;
	moXMLElementSPtr			element;
	moXMLElement::moXMLAttributeSPtr	attribute;
	moXMLElement::moXMLAttribute::type_t	type;
	moXMLElement::moXMLAttribute::value_t	value_type;
	moWCString				type_name, default_value;
	moList::position_t			pos;
	mowc::wc_t				r, t;
	bool					bad;

	//
	// The HTML DTD adds the following features to the attribute list:
	//
	//	The attribute name can be a reference to an entity.
	//
	//	Multiple names can be defined between '(' and ')' and separated by '|'
	//
	//	Each attribute in a list can be separated by a comment defined between
	//	two pairs of '--'. The closing of the comment does not require the '>'
	//	to follow the '--'.
	//
	// '<!ATTLIST' S (Name | '(' (Name '|')* Name ')') S ('(' AttDef Comment? ')')* S? '>'
	//
	// Comment := '--' (Char* - '--') '--'
	//

	//assert(f_input->XMLTestString("<!ATTLIST"));

	Pop();

	// the element names (these elements attribute list)
	r = GetDeclNames(element_names, "<!ATTLIST");
	if(r < 0) {
		return r;
	}

	// create new elements or use the already existing ones
	for(unsigned long idx = 0; idx < element_names.Count(); ++idx) {
		element = new moXMLElement(*element_names.Get(idx));
		pos = f_dtd_elements.Find(element);
		if(pos == moList::NO_POSITION) {
			f_dtd_elements.Insert(element);
		}
		else {
			element = f_dtd_elements.Get(pos);
		}
		elements_list.Insert(element);
	}
	// if errors occur and the list of names is empty,
	// then the list of elements is empty here
	if(elements_list.IsEmpty()) {
		// here the name does not matter, the element will be dropped anyway
		elements_list.Insert(new moXMLElement("UNDEFINED"));
	}

	for(;;) {
		// get the element each time because it can be changed at the bottom!
		// this is primarily for error consistency
		element = elements_list.Get(0);
		r = GetPEReference(true, true, true);
		if(r < 0) {
			return r;
		}
		// we accept the comments at the start too, just in case
		// but since we loop here, we also accept them at the end!
		// 	<!ATTLIST S Name S -- 1st comment -- ...>
		while(r == '-') {
			r = XMLGetC();
			if(r == '-') {
				// okay, the starting '--' was read, search the ending one now
				do {
					r = XMLGetC();
					if(r == '-') {
						r = XMLGetC();
						if(r == '-') {
							r = GetPEReference(true, true, true);
							break;
						}
					}
				} while(r >= 0);
			}
			else {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid '-' character in <!ATTLIST %S ...> declaration", element->GetName().Data());
				// since r != '-' we'll automatically get out of the while() loop
			}
			if(r < 0) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "unexpected end of input while parsing <!ATTLIST %S ...> declaration", element->GetName().Data());
				return r;
			}
		}
		if(r == '>') {
			f_input->XMLSkipC();
			return XML_RT_NOERROR;
		}
		r = GetName();
		if(r < 0) {
			return r;
		}

		attribute = new moXMLElement::moXMLAttribute(f_input->XMLCurrentString());

		r = GetPEReference(true, true, true);
		if(r < 0) {
			return r;
		}

		if(r == '(') {
			// enumeration
			type = moXMLElement::moXMLAttribute::ATTRIBUTE_ENUMERATION;
			type_name = "ENUMERATION";
		}
		else {
			r = GetName();
			if(r < 0) {
				return r;
			}

			type_name = f_input->XMLCurrentString();
			if(type_name == "CDATA") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_CDATA;
			}
			else if(type_name == "ID") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_ID;
			}
			else if(type_name == "IDREF") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_IDREF;
			}
			else if(type_name == "IDREFS") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_IDREFS;
			}
			else if(type_name == "ENTITY") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_ENTITY;
			}
			else if(type_name == "ENTITIES") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_ENTITIES;
			}
			else if(type_name == "NMTOKEN") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_NMTOKEN;
			}
			else if(type_name == "NMTOKENS") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_NMTOKENS;
			}
			else if(type_name == "NOTATION") {
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_NOTATION;
			}
			else if(type_name == "NAME") {
				// HTML
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_NAME;
			}
			else if(type_name == "NUMBER") {
				// HTML
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_NUMBER;
			}
			else {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "unknown attribute type %S for %S in <!ATTLIST %S ...> declaration", type_name.Data(), attribute->GetName().Data(), element->GetName().Data());
				// some safe default...
				type = moXMLElement::moXMLAttribute::ATTRIBUTE_CDATA;
			}

			r = GetPEReference(true, true, true);
			if(r < 0) {
				return r;
			}
		}
		attribute->SetType(type);
		if(r == '(') {
			f_input->XMLSkipC();
			bad = type != moXMLElement::moXMLAttribute::ATTRIBUTE_NOTATION
			   && type != moXMLElement::moXMLAttribute::ATTRIBUTE_ENUMERATION;
			if(bad) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "attribute %S in %S cannot be followed by a list of names in <!ATTLIST %S ...> declaration", type_name.Data(), attribute->GetName().Data(), element->GetName().Data());
			}

			for(;;) {
				r = GetPEReference(true, true, true);
				if(r < 0) {
					return r;
				}
				if(r == ')') {
					if(!bad && attribute->Count() == 0) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "the %S cannot be empty in <!ATTLIST %S ...> declaration", type_name.Data(), element->GetName().Data());
					}
					f_input->XMLSkipC();
					break;
				}
				if(r == '|') {
					if(!bad && attribute->Count() == 0) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "a '|' was not expected at the beginning of the %S list in <!ATTLIST %S ...> declaration", type_name.Data(), element->GetName().Data());
					}
					f_input->XMLSkipC();
					r = GetPEReference(true, true, true);
					if(r < 0) {
						return r;
					}
				}
				else {
					if(!bad && attribute->Count() != 0) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "a '|' was expected between two entries in the %S list in <!ATTLIST %S ...> declaration", type_name.Data(), element->GetName().Data());
					}
				}
				if(type == moXMLElement::moXMLAttribute::ATTRIBUTE_NOTATION) {
					// a NOTATION name
					r = GetName(SPACES_ANY);
				}
				else {
					// an NMTOKEN
					r = GetWord(
						  XML_CTYPE_NAME
						| XML_CTYPE_NAME_CHAR
						| XML_CTYPE_LETTER
						| XML_CTYPE_COMBINING
						| XML_CTYPE_DIGIT
						| XML_CTYPE_EXTENDER,

						  XML_CTYPE_NAME
						| XML_CTYPE_NAME_CHAR
						| XML_CTYPE_LETTER
						| XML_CTYPE_COMBINING
						| XML_CTYPE_DIGIT
						| XML_CTYPE_EXTENDER
					);
					if(f_input->XMLCurrentString().IsEmpty()) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid NMTOKEN in the %S list in <!ATTLIST %S ...> declaration", type_name.Data(), element->GetName().Data());
						return XML_RT_ERROR;
					}
				}
				if(r < 0) {
					return r;
				}
				if(!bad) {
					if(!attribute->AddUserTypeName(f_input->XMLCurrentString())) {
						f_input->FormatError(XML_ERRCODE_SYNTAX, "the %S name %S is defined twice in <!ATTLIST %S ...> declaration", type_name.Data(), f_input->XMLCurrentString().Data(), element->GetName().Data());
					}
				}
			}

			r = GetPEReference(true, true, true);
			if(r < 0) {
				return r;
			}
		}
		else {
			if(type == moXMLElement::moXMLAttribute::ATTRIBUTE_NOTATION) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a NOTATION attribute must be followed by one or more notation name in <!ATTLIST %S ...> declaration", element->GetName().Data());
			}
		}

		value_type = moXMLElement::moXMLAttribute::VALUE_DEFAULT;
		if(r == '#') {
			f_input->XMLSkipC();
			r = GetName(SPACES_NONE);
			if(r < 0) {
				return r;
			}

			type_name = f_input->XMLCurrentString();
			if(type_name == "REQUIRED") {
				value_type = moXMLElement::moXMLAttribute::VALUE_REQUIRED;
			}
			else if(type_name == "IMPLIED") {
				value_type = moXMLElement::moXMLAttribute::VALUE_IMPLIED;
			}
			else if(type_name == "FIXED") {
				value_type = moXMLElement::moXMLAttribute::VALUE_FIXED;
			}
			else {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "unknown value type %S in <!ATTLIST %S ...> declaration", type_name.Data(), element->GetName().Data());
			}

			r = GetPEReference(true, true, true);
			if(r < 0) {
				return r;
			}
		}
		else if(IsXMLNameChar(r, true)) {
			// a direct name!?
			r = GetName(SPACES_NONE);
			if(r < 0) {
				return r;
			}

			type_name = f_input->XMLCurrentString();
			if(type_name.CaseCompare("RECT")) {
				value_type = moXMLElement::moXMLAttribute::VALUE_RECTANGLE;
			}
			else if(type_name.CaseCompare("TEXT")) {
				value_type = moXMLElement::moXMLAttribute::VALUE_TEXT;
			}
			else if(type_name.CaseCompare("SUBMIT")) {
				value_type = moXMLElement::moXMLAttribute::VALUE_SUBMIT;
			}
			else if(type_name.CaseCompare("GET")) {
				value_type = moXMLElement::moXMLAttribute::VALUE_GET;
			}
			else {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "unknown value type %S in <!ATTLIST %S ...> declaration", type_name.Data(), element->GetName().Data());
			}

			r = GetPEReference(true, true, true);
			if(r < 0) {
				return r;
			}
		}
		if(r == '"' || r == '\'') {
			// a string (default or fixed value)
			if(value_type != moXMLElement::moXMLAttribute::VALUE_DEFAULT
			&& value_type != moXMLElement::moXMLAttribute::VALUE_FIXED) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "#REQUIRED and #IMPLIED cannot be followed by a default value in <!ATTLIST %S ...> declaration", element->GetName().Data());
			}
			default_value.Empty();
			f_input->XMLSkipC();		// skip the quote
			t = XMLGetC();
			while(t >= 0 && t != r) {
				// we will have to make sure that we want to replace these references...
				if(t == '&') {
					// TODO: replace the reference
					f_input->XMLUngetC();
					t = GetReference();
					// NOTE: t can be & or < or ' or "
				}
				else if(t == '<' /*|| t == '&'*/ ) {
					// we assume that a DTD is always strict contrary to lazy HTML and such
					f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid '%c' within a default attribute value", t);
				}
				// note: an AttValue cannot include a %<ref>;
				default_value += t;
				t = XMLGetC();
			}
			attribute->SetDefaultValue(default_value);
		}
		else if(r >= '0' && r <= '9') {
			if(type != moXMLElement::moXMLAttribute::ATTRIBUTE_NUMBER
			&& type != moXMLElement::moXMLAttribute::ATTRIBUTE_ENUMERATION) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a numeric default value must be of type NUMBER in the <!ATTLIST %S ...> declaration", element->GetName().Data());
			}
			// for enumeration, we should check that it is a possible case
			default_value.Empty();
			do {
				r = XMLGetC();
				default_value += r;
			} while(r >= '0' && r <= '9');
			// not a digit, restore!
			f_input->XMLUngetC();
		}
		attribute->SetValueType(value_type);

		// at the attribute to all the mentioned elements
		for(unsigned long idx = 0; idx < element_names.Count(); ++idx) {
			element = elements_list.Get(idx);
			if(!element->AddAttribute(attribute) && f_strict) {
				f_input->FormatWarning(XML_ERRCODE_SYNTAX, "attribute %S is defined twice in <!ATTLIST %S ...> declaration", attribute->GetName().Data(), element->GetName().Data());
			}
		}
	}
}



/** \brief Get a DTD reference converted.
 *
 * Support [69]
 *
 * This function converts a DTD reference from \%\<name>[;] into its
 * content. It can do so recursively. It is called from functions
 * reading an XML DTD whenever they support having a DTD entity in
 * some place (i.e. the names of an \<!ELEMENT ...> definition.)
 *
 * When \p skip_spaces is set to true, the function first skip any
 * spaces then check that the next character is indeed a '\%'. If
 * so, a convertion is applied. Otherwise, the character is returned
 * as is.
 *
 * When \p skip_spaces is false, it is expected that the character '\%'
 * was already found and eaten in the stream. The function then reads
 * the name of the identity.
 *
 * When \p loop is set, it converts starting references recursively.
 * Thus, if the first reference replaced starts with a reference, that
 * reference is converted too. This is applied as many times as
 * required. (\%a; -> \%b; \%c; -> \%d; \%e; \%c;, etc.)
 *
 * The \p add_spaces parameter is used to tell the function to insert
 * a space before and after the reference when being inserted. This
 * ensures that something like: "Test\%a;More" becomes "Test \%a; More"
 * with the contents of \%a; replacing \%a;.
 *
 * In strict mode, all entity references must end with a semi-colon.
 * Otherwise the semi-colon is optional.
 *
 * \note
 * It is an error to find the EOF marker while reading a reference.
 * Thus this function may return XML_RT_ERROR, not XML_RT_EOF.
 *
 * \todo
 * External entities to data other than disk files are not yet supported.
 * (i.e. http://..., ftp://..., etc.)
 *
 * \param[in] skip_spaces Whether the function first skip spaces and
 * 	then check for a \% character or assumes that the \% was already read
 * \param[in] add_spaces Whether to add spaces before and after the replacement text
 * \param[in] loop Whether to apply the references recursively
 *
 * \return The next character (which is the first character of the replace
 *	if a replacement occurs) or an error code (negative)
 */
int moXMLParser::GetPEReference(bool skip_spaces, bool add_spaces, bool loop)
{
//	[69]	PEReference	::=	'%' Name ';'
	moXMLEntitySPtr			entity;
	mowc::wc_t			r;
	moList::position_t		pos;
	moIOStreamSPtr			input;

	if(skip_spaces) {
		r = XMLSkipSpaces();
		if(r < 0) {
			return r;
		}
		if(r != '%') {
			return r;
		}
		f_input->XMLSkipC();
	}
	for(;;) {
		r = GetName(SPACES_NONE);
		if(r < 0) {
			if(r == XML_RT_EOF) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "a '%%<name>;' reference needs to end before the end of the file");
				return XML_RT_ERROR;
			}
			return r;
		}

		if(r != ';') {
			// the ';' is optional in the HTML DTD!!!
			if(f_strict) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "expected ';' at the end of a '%%<name>;' reference");
			}
		}
		else {
			f_input->XMLSkipC();
		}

		moXMLEntity ref(f_input->XMLCurrentString(), moXMLEntity::ENTITY_DTD);
		pos = f_dtd_parsed_entities.Find(&ref);
		if(pos == moList::NO_POSITION) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "cannot find entity %S referenced here", f_input->XMLCurrentString().Data());
		}
		else {
			entity = f_dtd_parsed_entities.Get(pos);

			// insert this entity in the input stack (without spaces)
			const moWCString& value = entity->GetValue();
			moWCString filename;

			if(value.IsEmpty()) {
				const moWCString& system_id = entity->GetSystemID();
				if(!system_id.IsEmpty()) {
					if(system_id.Compare("file:///", 0, 8) == MO_BASE_COMPARE_EQUAL) {
						filename = system_id.Get(7, -1);
					}
					else {
						const mowc::wc_t *s = system_id.Data();
						while(*s != '/' && *s != '\\' && *s != ':' && *s != '\0') {
							s++;
						}
						if(*s != ':' && s > system_id.Data()) {
							if(system_id[0] == '/'		// Unix
							|| system_id[0] == '\\'		// MS-DOS
							|| system_id[0] == ':'		// old Macs
							|| (((system_id[0] >= 'A' && system_id[0] <= 'Z')
								|| (system_id[0] >= 'a' && system_id[0] <= 'z'))
									&& system_id[1] == ':'
									&& (system_id[2] == '/'
										|| system_id[2] == '\\'))) {
								filename = system_id;
							}
							else if(f_dtd_url.Compare("file:///", 0, 8) == MO_BASE_COMPARE_EQUAL) {
								// we can read local files!
								filename = f_dtd_url.Get(7, -1).FilenameDirname().FilenameChild(system_id);
							}
							else {
								// TODO: ... read the system DTD (need network, etc.)
							}
						}
					}
				}
			}

			// try the file first
			if(!filename.IsEmpty()) {
				input = new moFile;
				if(!static_cast<moFile *>(static_cast<moIOStream *>(input))->Open(filename)) {
					// file not present?
					f_input->FormatError(XML_ERRCODE_IO, "file \"%S\" specified in entity %S cannot be opened", filename.Data(), entity->GetName().Data());
					filename.Empty();
				}
			}
			// if not a file, include the value
			if(filename.IsEmpty()) {
				input = new moMemFile;
				input->InputFilename(f_input->GetFilename().SavedMBData());
				input->Put((mowc::wc_t) 0xFEFF);
				if(add_spaces) {
					input->Put((mowc::wc_t) ' ');
				}
				input->Write(value.Data(), value.Length() * sizeof(mowc::wc_t));
				if(add_spaces) {
					input->Put((mowc::wc_t) ' ');
				}
			}

			moXMLStreamSPtr stream = new moXMLParser::moXMLStream(input);
			if(filename.IsEmpty()) {
				stream->SetLine(f_input->Line());
			}
			stream->XMLInterpretVariables(f_input->XMLGetInterpretVariables());
			Include(*stream);
			input = 0;
		}
		if(!loop) {
			// in order to be able to do XMLUngetC()
			// it needs to be from the same file
			r = f_input->XMLGetC();
			f_input->XMLUngetC();
			return r;
		}
		r = XMLSkipSpaces();
		if(r != '%') {
			return r;
		}
		f_input->XMLSkipC();
	}

	return r;
}


/** \brief Read an entity declaration.
 *
 * Support: [9], [70], [71], [72], [73], [74], [75], [76]
 *
 * This function reads an entity declaration \<!ENTITY ...> from a
 * DTD definition.
 *
 * This function supports document and DTD entities.
 *
 * \bug
 * It is an error to read two entities with the same name.
 * Note however that the document and DTD entities are saved in two
 * different lists. This means one document and one DTD entity can
 * share the exact same name. It is not clear whether this is not
 * XML compliant.
 *
 * \return XML_RT_NOERROR or an error code (negative)
 */
int moXMLParser::GetEntityDecl(void)
{
//	[ 9]    EntityValue	::=	'"' ([^%&"] | PEReference | Reference)* '"' |  "'" ([^%&'] | PEReference | Reference)* "'"
//	[70]	EntityDecl	::=	GEDecl | PEDecl
//	[71]	GEDecl		::=	'<!ENTITY' S Name S EntityDef S? '>'
//	[72]	PEDecl		::=	'<!ENTITY' S '%' S Name S PEDef S? '>'
//	[73]	EntityDef	::=	EntityValue | (ExternalID NDataDecl?)
//	[74]	PEDef		::=	EntityValue | ExternalID
//	[75]    ExternalID	::=	('SYSTEM' | PublicID) S SystemLiteral
//	[76]	NDataDecl	::=	S 'NDATA' S Name
	moXMLEntitySPtr			entity;
	moXMLEntity::type_t		type;
	moWCString			value;
	mowc::wc_t			r, t;

	//assert(f_input->XMLTestString("<!ENTITY"));

	Pop();

	f_input->XMLSkipC(8);

	r = XMLGetC();
	if(!IsXMLSpace(r)) {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "the <!ENTITY ...> declaration expects a space between '<!ENTITY' and the name");
	}
	else {
		do {
			r = XMLGetC();
		} while(IsXMLSpace(r));
	}

	if(r == '%') {
		type = moXMLEntity::ENTITY_DTD;
		r = XMLGetC();
		if(!IsXMLSpace(r)) {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "the <!ENTITY %% ...> declaration expect a space after the '%%' character");
			f_input->XMLUngetC();
		}
	}
	else {
		type = moXMLEntity::ENTITY_GENERAL;
	}

	// the element name (that element attribute list)
	// NOTE: spaces could already have been skipped so we cannot have SPACES_ONE_OR_MORE here
	r = GetName(SPACES_ANY);
	if(r < 0) {
		return r;
	}
	// create a new element or use the already existing one
	entity = new moXMLEntity(f_input->XMLCurrentString(), type);

	if(type == moXMLEntity::ENTITY_DTD) {
		// general DTD entities (only visible in DTDs) -- %<name>;
		if(!f_dtd_parsed_entities.Insert(entity) && f_strict) {
			f_input->FormatWarning(XML_ERRCODE_DEFINED_TWICE, "the <!ENTITY %% %S ...> is defined twice", entity->GetName().Data());
		}
	}
	else {
		// general "user" entities (visible in XML files) -- &<name>;
		if(!f_dtd_entities.Insert(entity) && f_strict) {
			f_input->FormatWarning(XML_ERRCODE_DEFINED_TWICE, "the <!ENTITY %S ...> is defined twice", entity->GetName().Data());
		}
	}

	r = XMLSkipSpaces();
	if(r < 0) {
		return r;
	}

	if(r == '"' || r == '\'') {
		// simple string value
		value.Empty();
		f_input->XMLSkipC();		// skip the quote
		moXMLStreamSPtr original = f_input;
		t = XMLGetC();
		while(t >= 0 && (t != r || f_input != original)) {
			if(t == '&') {
				t = XMLGetC();
				if(t == '#') {
					// replace the &#<value>; references
					f_input->XMLUngetC();
					f_input->XMLUngetC();
					t = GetReference();
					// NOTE: t can be & or < or ' or " here
				}
				else {
					// this is a &<name>; which we keep as is
					value += '&';
				}
			}
			else if(t == '%') {
				t = GetPEReference(false, false, true);
				if(t < 0) {
					return t;
				}
				f_input->XMLSkipC();
			}
			value += t;
			t = XMLGetC();
		}
		if(t < 0) {
			return t;
		}
		entity->SetValue(value);
	}
	else {
		r = GetName(SPACES_ANY);
		if(r < 0) {
			return r;
		}
		const moWCString& ext = f_input->XMLCurrentString();
		if(ext == "PUBLIC") {
			// read the PubID
			r = XMLSkipSpaces();
			if(r < 0) {
				return r;
			}
			if(r == '"') {
				f_input->XMLSkipC();
				t = GetWord(XML_CTYPE_PUBLIC_ID_DQ, XML_CTYPE_PUBLIC_ID_DQ);
				if(t != r) {
					f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid PUBLIC identifier, expected '\"' to close the identifier in <!ENTITY %S ...> declaration", entity->GetName().Data());
				}
				else {
					f_input->XMLSkipC();
				}
			}
			else if(r == '\'') {
				f_input->XMLSkipC();
				t = GetWord(XML_CTYPE_PUBLIC_ID, XML_CTYPE_PUBLIC_ID);
				if(t != r) {
					f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid PUBLIC identifier, expected \"'\" to close the identifier in <!ENTITY %S ...> declaration", entity->GetName().Data());
				}
				else {
					f_input->XMLSkipC();
				}
			}
			else {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid PUBLIC identifier, we expected a quoted identifier in <!ENTITY %S ...> declaration", entity->GetName().Data());
				return XML_RT_ERROR;
			}
			if(t < 0) {
				return t;
			}
			// we got a public identification string
			// TODO: ...
			entity->SetPublicID(f_input->XMLCurrentString());
		}
		else if(ext != "SYSTEM") {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "one of PUBLIC or SYSTEM was expected in <!ENTITY %S ...> declaration", entity->GetName().Data());
		}

		r = GetPEReference(true, true, true);
		if(r < 0) {
			return r;
		}
		if(r == '\'' || r == '"') {
			f_input->XMLSkipC();
			moWCString& current_string = f_input->XMLCurrentString();
			current_string.Empty();
			moXMLStreamSPtr original = f_input;
			t = XMLGetC();
			while(t >= 0 && (t != r || f_input != original)) {
				if(t == '%') {
					t = GetPEReference(false, false, true);
				}
				current_string += t;
				t = XMLGetC();
			}
			entity->SetSystemID(current_string);
		}
		else {
			f_input->FormatError(XML_ERRCODE_SYNTAX, "invalid DOCTYPE, expected a system literal after PUBLIC or SYSTEM");
		}
	}

	r = XMLSkipSpaces();
	if(r < 0) {
		return r;
	}

	/* The HTML DTD includes comments within entities defined starting with --
	 *		<!ENTITY % name - - "blah" -- comment -->
	 * Here we catch the -- if present
	 */
	if(r == '-' && !f_strict) {
		f_input->XMLSkipC();
		r = XMLGetC();
		if(r == '-') {
			// we have a comment! skip it
			do {
				r = XMLGetC();
				if(r == '-') {
					r = XMLGetC();
					if(r == '-') {
						// cool, now we could have spaces before the >
						r = XMLSkipSpaces();
						break;
					}
				}
			} while(r >= 0);
			if(r < 0) {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "the --> from the <!ENTITY %S -- ... --> declaration is missing", entity->GetName().Data());
				return r;
			}
		}
		else {
			// the error occurs below--no comment here
			f_input->UngetC(r);
			r = '-';
			f_input->UngetC(r);
		}
	}

	if(r == '>') {
		f_input->XMLSkipC();
	}
	else {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "'>' expected to end the <!ENTITY %S ...> declaration", entity->GetName().Data());
	}

	return XML_RT_NOERROR;
}



/** \brief Read a conditional block.
 *
 * Support [61], [62], [63], [64], [65]
 *
 * In a DTD, blocks of data can be marked as conditional with
 * either <[INCLUDE[...]]> or <[IGNORE[...]]>.
 *
 * This special function manages these two types of block.
 *
 * The blocks marked as IGNORE are dropped. The blocks
 * marked as INCLUDE are kept and parsed as if the
 * <[INCLUDE[ and ]]> characters did not exist.
 *
 * \return An error or XML_RT_NOERROR
 */
int moXMLParser::GetConditionalSect(void)
{
//	[61]	ConditionalSect	::=	IncludeSect | IgnoreSect
//	[62]	IncludeSect	::=	'<![' S? 'INCLUDE' S? '[' ExtSubsetDecl ']]>'
//	[63]	IgnoreSect	::=	'<![' S? 'IGNORE' S? '[' IgnoreSectContents* ']]>'
//	[64]	IgnoreSectContents::=	Ignore ('<![' IgnoreSectContents ']]>' Ignore)*
//	[65]	Ignore		::=	Char* - (Char* ('<![' | ']]>') Char*)
	mowc::wc_t			r, str[3];
	int				depth;

	//assert(f_input->XMLTestString("<!["));

	Pop();

	f_input->XMLSkipC(3);

	r = GetPEReference(true, true, true);
	if(r < 0) {
		return r;
	}
	r = GetName(SPACES_ANY);
	if(r < 0) {
		return r;
	}

	const moWCString& cond = f_input->XMLCurrentString();

	r = XMLSkipSpaces();
	if(r < 0) {
		return r;
	}
	if(r == '[') {
		f_input->XMLSkipC();
	}
	else {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "expected '[' after the conditional word in the <![%S[ ... ]]> declaration", cond.Data());
	}

	if(cond == "INCLUDE") {
		// we continue as if nothing happened!
		++f_cond_include;
		return XML_RT_NOERROR;
	}

// We expect IGNORE, if not we still need to skip the conditional block
	if(cond != "IGNORE") {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "a conditional declaration <![%S[ ... ]]> is not understood, expected INCLUDE or IGNORE, will ignore the entire block", cond.Data());
	}

// we need to skip everything up to the next ]]>, to do it properly we need to
// count the number of <![[ and ]]> since a conditional can include another

	str[0] = f_input->XMLGetC();
	if(str[0] >= 0) {
		str[1] = f_input->XMLGetC();
		if(str[1] >= 0) {
			str[2] = f_input->XMLGetC();
			if(str[2] >= 0) {
				depth = 1;
				for(;;) {
					if(str[0] == '<' && str[1] == '!' && str[2] == '[') {
						++depth;
					}
					else if(str[0] == ']' && str[1] == ']' && str[2] == '>') {
						--depth;
						if(depth == 0) {
							break;
						}
					}
					str[0] = str[1];
					str[1] = str[2];
					str[2] = f_input->XMLGetC();
					if(str[2] < 0) {
						break;
					}
				}
			}
		}
		else {
			str[2] = str[1];
		}
	}
	else {
		str[2] = str[0];
	}


	if(str[2] == XML_RT_EOF) {
		f_input->FormatError(XML_ERRCODE_EOF_BEFORE_END_OF_CONDITION, "the conditional declaration <![%S[ ... ]]> does not terminate before the end of the file", cond.Data());
		return XML_RT_ERROR;
	}
	if(str[2] < 0) {
		return str[2];
	}

	return XML_RT_NOERROR;
}



/** \brief Get a comment, parser target or spaces
 *
 * Support [27]
 *
 * This function is used to skip parts that do no require
 * much work.
 *
 * Note that a comment cannot include -- and a PI cannot include a ?>.
 *
 * \return An error or XML_RT_NOERROR
 */
int moXMLParser::GetMisc(void)
{
//	[27]	Misc		::=	Comment | PI | S
//
// Comments and PI start with '<!--' and '<?' respectively
// Spaces are checked with IsXMLSpace()
	mowc::wc_t	r;

	do {
		// The spaces in a Misc are never saved (these are
		// not in a tag content)
		r = f_input->XMLGetC();
		if(r < 0) {
			return r;
		}
	} while(IsXMLSpace(r));
	f_input->XMLUngetC();

	if(r == '<') {
		// a comment?
		if(f_input->XMLTestString("<!--")) {
			// NOTE: we keep the Misc*
			// on the stack since there
			// can be other Misc's after
			// this comment.
			Push(&moXMLParser::GetComment);
			return XML_RT_NOERROR;
		}
		// a PI?
		if(f_input->XMLTestString("<?")) {
			// NOTE: when we reach here we can't
			// have a valid <?xml ...> since the
			// XMLDecl was already parsed. Thus
			// we can simply call the PI reader
			// We keep the Misc* on the stack since
			// there can be other Misc's after this
			// PI.
			Push(&moXMLParser::GetPI);
			return XML_RT_NOERROR;
		}
	}

	// we're done with this Misc*
	Pop();

	return XML_RT_NOERROR;
}


/** \brief Check a word validity or generate an error.
 *
 * This function checks that the input string is a valid word.
 *
 * The test is the same as the GetWord() function applies to
 * characters being read from the input stream.
 *
 * Note that the whole string is checked. If any character
 * does not fit the mask defined in \p first and \p others
 * then this function returns an error.
 *
 * Note that if \p others is not defined (set to 0) then it
 * is supposed that it has to be set to the same value as
 * \p first.
 *
 * \bug
 * Note that an empty string is viewed as always matching
 * whatever the values of \p first and \p others.
 *
 * \bug
 * The first parameter cannot be set to zero. If it is set to
 * 0 in release mode, and the string is empty, then the function
 * returns no error.
 *
 * \exception moBug(MO_ERROR_INTERNAL)
 * This exception is thrown in debug mode whenever the the parameter
 * first is set to zero which would mean that the function always
 * returns false and thus calling it is pointless.
 *
 * \param[in] str The string to be tested
 * \param[in] first The mask of acceptable characters for the first character of the string
 * \param[in] others The mask of acceptable characters for all the characters other than the first, if set to 0, use \p first
 *
 * \return XML_RT_NOERROR if the word is a match, XML_RT_ERROR otherwise
 */
mowc::wc_t moXMLParser::TestWord(const moWCString& str, unsigned long first, unsigned long others)
{
	mowc::wc_t		r;
	unsigned long		i, max, flags;

#ifdef MO_DEBUG
	if(first == 0) {
		throw moBug(MO_ERROR_INTERNAL, "moXMLParser::TestWord(): the 'first' parameter of GetWord() cannot be zero");
	}
#endif

	if(others == 0) {
		others = first;
	}

	max = static_cast<unsigned long>(str.Length());
	if(max == 0) {
		return XML_RT_NOERROR;
	}

	r = str.First();
	flags = XMLCType(r);
	if((flags & first) == 0) {
		// the entire string needs to be a valid word
		return XML_RT_ERROR;
	}
	i = 1;
	do {
		if(i >= max) {
			return XML_RT_NOERROR;
		}
		r = str.Get(i);
		i++;
		flags = XMLCType(r);
	} while((flags & others) != 0);

	// the entire string needs to be a valid word
	return XML_RT_ERROR;
}


/** \brief Retrieves a word according to the specified character masks.
 *
 * Support <i>variable</i>
 *
 * This function checks for a string of characters built using
 * characters found in the input streams matching the character types
 * having bits common to the bits defined in the \p first and \p others
 * parameters.
 *
 * Each character is defined in one or more groups of characters.
 * For instance, the space character is primarily defined as an
 * XML_CTYPE_SPACE. It can also be found in public identifiers
 * and thus it is also defined as XML_CTYPE_PUBLIC_ID and
 * XML_CTYPE_PUBLIC_ID_IQ.
 *
 * The \p first parameter is used to define the flags acceptable in
 * the first character read. This is often more constrained than the
 * \p others parameter. For instance, for an XML name, the first
 * character cannot be a digit.
 *
 * \note
 * If the EOF is reached from within this function, it is an error since
 * this means we're not accomplished.
 *
 * \note
 * If the first or others masks include the space character, then all
 * the spaces are included in the output. The function does not skip
 * spaces, nor concatenate multiple spaces in a row in a single space.
 *
 * \bug
 * The function returns without errors if the first character does not
 * match. Instead, you have to check and notice that the string is empty.
 *
 * \exception moBug(MO_ERROR_INTERNAL)
 * This exception is thrown in debug mode whenever the the parameter first
 * is set to zero which would mean that the function always return false
 * and thus calling it is pointless.
 *
 * \param[in] first A set of character flags for the word first character.
 * \param[in] others A set of character flags for the word characters except the first.
 *
 * \return The next character or an error code.
 *
 * \sa moXMLParser::XMLCType(mowc::wc_t)
 */
mowc::wc_t moXMLParser::GetWord(unsigned long first, unsigned long others)
{
	mowc::wc_t		r;
	unsigned long		flags;

#ifdef MO_DEBUG
	if(first == 0) {
		throw moBug(MO_ERROR_INTERNAL, "moXMLParser::GetWord(): the 'first' parameter of GetWord() cannot be zero");
	}
#endif

	if(others == 0) {
		others = first;
	}

	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();

	r = f_input->XMLGetC();
	if(r < 0) {
		return r;
	}
	flags = XMLCType(r);
	if((flags & first) == 0) {
		// if an empty result is not good, check for the
		// current_string emptyness.
		return r;
	}
	do {
		current_string += r;
		r = f_input->XMLGetC();
		if(r < 0) {
			return r;
		}
		flags = XMLCType(r);
	} while((flags & others) != 0);
	f_input->XMLUngetC();

	return r;
}


/** \brief Read an XML name.
 *
 * Support [4], [5], [84], [85], [86], [87], [88], [89]
 *
 * This function reads an XML name.
 *
 * Note that the name is saved in the f_current_string
 * variable member. Note that f_current_string is not
 * modified if an error occurs. The function takes care
 * of resetting the string as required.
 *
 * The \p expect spaces has the following meanings:
 *
 * \li SPACES_RELAX accept 0 or more spaces if we are in
 * relaxed mode (i.e. not strict mode.)
 *
 * \li SPACES_ANY accepts any number of spaces.
 *
 * \li SPACES_NONE refuses spaces, if the first character is
 * a space, an error is generated. The parser then attempts to
 * continue by skipping all the spaces.
 *
 * \li SPACES_ONE_OR_MORE forces the existance of at least
 * one space before the XML name. If that white space does
 * not exist then an error is generated.
 *
 * \note
 * When the first character is not a valid character for an
 * XML name, the function returns XML_RT_FATAL.
 *
 * \param[in] expect_spaces one of the SPACES_... enumeration
 *
 * \return The next character or an error (negative value)
 *
 * \sa moXMLParser::IsXMLNameChar
 */
mowc::wc_t moXMLParser::GetName(spaces_t expect_spaces)
{
//	[ 4]	NameChar	::=	Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
//	[ 5]	Name		::=	(Letter | '_' | ':') (NameChar)*
//	[84]	Letter		::=	BaseChar | Ideographic
//	[85]	BaseChar	::=	...
//	[86]	Ideographic	::=	...
//	[87]	CombiningChar	::=	...
//	[88]	Digit		::=	...
//	[89]	Extender	::=	...
	mowc::wc_t		r;

	// we accept spaces here, in strict mode this
	// is not always possible!
	switch(expect_spaces) {
	case SPACES_RELAX:
		do {
			r = f_input->XMLGetC();
		} while(!f_strict && IsXMLSpace(r));
		break;

	case SPACES_ANY:
		do {
			r = f_input->XMLGetC();
		} while(IsXMLSpace(r));
		break;

	case SPACES_NONE:
		r = f_input->XMLGetC();
		if(IsXMLSpace(r)) {
			f_input->FormatError(XML_ERRCODE_BADNAME, "no space is expected before an XML name");
			// so it may work, we skip all the spaces...
			do {
				r = f_input->XMLGetC();
			} while(IsXMLSpace(r));
		}
		break;

	case SPACES_ONE_OR_MORE:
		r = f_input->XMLGetC();
		if(!IsXMLSpace(r)) {
			if(!IsXMLNameChar(r, true)) {
				// not even a valid name?!
				f_input->FormatError(XML_ERRCODE_BADNAME, "invalid character (&#x%lX;) instead of a space before an XML name", r);
				return XML_RT_FATAL;
			}
			f_input->FormatError(XML_ERRCODE_BADNAME, "a space was expected before an XML name");
		}
		else {
			do {
				r = f_input->XMLGetC();
			} while(IsXMLSpace(r));
		}
		break;

	default:
		// not a valid space specification
		f_input->FormatError(XML_ERRCODE_BADNAME, "invalid space specification (%d)", expect_spaces);
		return XML_RT_FATAL;

	}
	if(!IsXMLNameChar(r, true)) {
		// not a valid name...
		f_input->FormatError(XML_ERRCODE_BADNAME, "invalid character (&#x%lX;) to start an XML name", r);
		return XML_RT_FATAL;
	}
	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();
	do {
		current_string += r;
		r = f_input->XMLGetC();
	} while(IsXMLNameChar(r));
	f_input->XMLUngetC();

	return r;
}


/** \brief Read a PITarget tag.
 *
 * Support [16], [17]
 *
 * This function reads the content defined between \<?name ...?>
 * unless the name is "XML" (case insensitive.)
 *
 * The content of the tag is not otherwise modified before being
 * saved in the moXMLData object. The name of the moXMLData is the
 * name of the PI.
 *
 * \return XML_RT_NOERROR if the reading was successful, an error otherwise.
 */
int moXMLParser::GetPI(void)
{
//	[16]	PI		::=	'<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
//	[17]	PITarget	::=	Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
//
// NOTE: we assume that the '<?' was already read when this
// function is called since that's the only way to know
// we have a PI.
	mowc::wc_t	r;
	long		p;

	// we don't want to come back! a PI is read at once
	Pop();

	if(!f_input->XMLTestString("<?")) {
		// no PI it seems...
		// (should that not be possible?)
		return XML_RT_NOERROR;
	}
	f_input->XMLSkipC(2);

	r = GetName();
	if(r < 0) {
		return r;
	}
	moWCString& current_string = f_input->XMLCurrentString();
	if(!current_string.FindCaseString("xml").IsEmpty()) {
		// Ooops! A PI name can't include the word "XML"
		// TODO: add some better error codes for better debug
		//	 of XML documents!
		// TODO: write some recovery code by searching for the
		//	 '?>' and just ignore this PI
		f_input->FormatError(XML_ERRCODE_BADPI, "a PITarget name can't include the word \"XML\" (\"%S\")", current_string.Data());
		return XML_RT_FATAL;
	}
	moXMLData *pi = new moXMLData(XML_TYPE_PI, f_name + "/" + current_string, current_string);
	f_data += *pi;

	p = f_input->XMLFindInCurrentLine("?>");
	if(p > 0) {
		f_input->XMLGetString(current_string, p - f_input->XMLGetPos());
		f_input->XMLSkipC(2);
	}
	else {
		p = static_cast<long>(f_input->XMLCurrentLine().Length());
		current_string = f_input->XMLCurrentLine().Get(f_input->XMLGetPos(), p);
		r = f_input->XMLNextLine();
		while(r >= 0) {
			p = f_input->XMLFindInCurrentLine("?>");
			if(p >= 0) {
				if(p > 0) {
					// if p is zero there's nothing to get here!
					// this gets just the end of the string
					current_string += f_input->XMLCurrentLine().Get(0, p - 1);
				}
				f_input->XMLSetPos(p + 2);
				break;
			}
			// copy the entire line!
			current_string += f_input->XMLCurrentLine();
			r = f_input->XMLNextLine();
		}
	}

	// we save the data we read without the starting & ending
	// spaces -- it isn't clear right now whether the spaces
	// should be kept...
	pi->SetData(current_string.Clip());

	// skip the data and the '?>'
	f_input->XMLSetPos(p + 2);

	return XML_RT_NOERROR;
}



/** Read an XML comment.
 *
 * Support [15]
 *
 * This function reads a comment from \<!-- to -->.
 *
 * Note that by default comments are simply discarded. If you want
 * to keep comments, call the moXMLParser::ReturnComments() function.
 *
 * \note
 * It is an error to use -- within a comment. We do support it in
 * non-strict mode though.
 *
 * \return XML_RT_NOERROR or an error code (negative)
 */
int moXMLParser::GetComment(void)
{
//	[15]	Comment		::=	'<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
	static const char	invalid_cmt[] = "invalid \"--\" within a comment or comment ends with \"--->\"";
	mowc::wc_t	r;
	long		p, q;

	// we don't want to come back! a Comment is read at once
	Pop();

	if(!f_input->XMLTestString("<!--")) {
		return XML_RT_NOERROR;
	}

	//
	// NOTE: to be fully complient, '--' is not valid in a long comment unless
	//	 it is part of the end marker '-->'; also, the end marker can't
	//	 include more than two '--' (i.e. '--->' is invalid).
	//
	// NOTE: a long comment can include ANY character including < and > which
	//	 won't be marking a tag.
	//
	f_input->XMLSkipC(4);		// skip the '<!--'
	if(!f_return_comments) {
		do {
			p = f_input->XMLFindInCurrentLine("-->");
			if(f_strict) {
				q = f_input->XMLFindInCurrentLine("--");
				if(q >= 0 && q < p) {
					f_input->FormatError(XML_ERRCODE_BADCOMMENT, invalid_cmt);
					// we continue because it's more than likely that this
					// was a minor error.
				}
			}
			if(p >= 0) {
				f_input->XMLSetPos(p + 3);
				return XML_RT_NOERROR;
			}
			r = f_input->XMLNextLine();
		} while(r >= 0);
		return r;
	}

	// this case is much more time consuming since it has to
	// record the data to be returned

	// at the start we have a special case since the
	// whole line won't be copied in the current string
	moWCString& current_string = f_input->XMLCurrentString();
	p = f_input->XMLFindInCurrentLine("-->");
	if(p > (long) 0) {
		// "special case" when the comment starts & stops on
		// a single line
		f_input->XMLGetString(current_string, p - f_input->XMLGetPos());
		f_input->XMLSkipC(3);
		r = f_input->XMLGetC();
		f_input->XMLUngetC();
	}
	else {
		p = static_cast<long>(f_input->XMLCurrentLine().Length());
		current_string = f_input->XMLCurrentLine().Get(f_input->XMLGetPos(), p);
		r = f_input->XMLNextLine();
		while(r >= 0) {
			p = f_input->XMLFindInCurrentLine("-->");
			if(p >= 0) {
				if(p > 0) {
					// if p is zero there's nothing to get here!
					// this gets just the end of the string
					current_string += f_input->XMLCurrentLine().Get(0, p - 1);
				}
				f_input->XMLSetPos(p + 3);
				r = f_input->XMLGetC();
				f_input->XMLUngetC();
				break;
			}
			// copy the entire line!
			current_string += f_input->XMLCurrentLine();
			r = f_input->XMLNextLine();
		}
	}

	if(current_string.Length() > 0
	&& f_strict
	&& (current_string.FindInString("--") >= 0 || current_string.Last() == '-')) {
		f_input->FormatError(XML_ERRCODE_BADCOMMENT, invalid_cmt);
		// we continue because it's more than likely that this
		// was a minor error.
	}

	// now we can create a comment object and append it to the
	// list of data object
	moXMLData *comment = new moXMLData(XML_TYPE_COMMENT, f_name);
	f_data += *comment;
	comment->SetData(current_string);

	current_string.Empty();

	return r;
}


/** \brief Read a content data section.
 *
 * Support [2], [18], [19], [20], [21]
 *
 * This function reads a CDATA section defined as \<![CDATA[...]]>.
 *
 * The content of a CDATA section can be any acceptable XML character.
 *
 * \return XML_RT_NOERROR or an error code (negative.)
 *
 * \sa moXMLParser::GetTagCData(void)
 */
int moXMLParser::GetCDSect(void)
{
//	[18]	CDSect		::=	CDStart CData CDEnd
//	[19]	CDStart		::=	'<![CDATA['
//	[20]	CData		::=	(Char* - (Char* ']]>' Char*))
//	[21]	CDEnd		::=	']]>'
//	[ 2]	Char		::=	<all the XML acceptable chars>
	mowc::wc_t		r;

	Pop();

	if(!f_input->XMLTestString("<![CDATA[")) {
		return XML_RT_NOERROR;
	}

	f_input->XMLSkipC(9);

	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();
	for(;;) {
		if(f_input->XMLGetPos() >= f_input->XMLCurrentLine().Length()) {
			r = f_input->XMLNextLine();
			if(r < 0) {
				break;
			}
		}
		if(f_input->XMLTestString("]]>")) {
			break;
		}
		r = f_input->XMLGetC();
		if(r < 0) {
			break;
		}
		current_string += r;
	}

	f_input->XMLSkipC(3);		// skip the ']]>'

	r = f_input->XMLGetC();
	f_input->XMLUngetC();

	moXMLData *data = new moXMLData(XML_TYPE_CDATA, f_name);
	data->SetData(current_string);
	f_data += *data;

	return r;
}


/** \brief Read actual content between tags.
 *
 * Support [14], [43]
 *
 * This function reads data between a regular opening and closing tag.
 *
 * Note that any opening tag, comment, PI, etc. within the content
 * data stops this function (but does not pop it) to take care of
 * that opening tag, comment, PI, etc.
 *
 * \return XML_RT_NOERROR or an error code
 */
int moXMLParser::GetContent(void)
{
//	[14]	CharData	::=	[^<&]* - ([^<&]* ']]>' [^<&]*)
//	[43]	Content		::=	CharData? ((Element | Reference | CDSect | PI | Comment) CharData?)*
	mowc::wc_t		r;

	r = XML_RT_NOERROR;
	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();
	for(;;) {
		if(f_input->XMLGetPos() >= f_input->XMLCurrentLine().Length()) {
			r = f_input->XMLNextLine();
			if(r < 0) {
				break;
			}
		}
		if(f_input->XMLTestString("<")) {
			if(f_input->XMLTestString("<![CDATA[")) {
				Push(&moXMLParser::GetCDSect);
			}
			else if(f_input->XMLTestString("<!--")) {
				Push(&moXMLParser::GetComment);
			}
			else if(f_input->XMLTestString("<?")) {
				Push(&moXMLParser::GetPI);
			}
			else if(f_input->XMLTestString("</")) {
				// in this case we expect the end tag to
				// correspond to the start tag...
				Pop();


				if(f_strict) {
					// in strict mode, the end tag MUST correspond
					Push(&moXMLParser::GetETag);
				}
				else {
					// in non-strict mode (i.e. HTML), we expect some
					// tags to not be closed properly; thus we first
					// check to see whether the current tag is the one
					// being closed; if not we force a close now

					// note that 'last_name' won't include the '/'
					moWCString last_name = f_name.FindRChar(L'/').Delete(0, 0);
					if(f_input->XMLTestName(-2, last_name.c_str(), false)) {
						// it corresponds; assume it is correct and remove
						// that tag cleanly now
						Push(&moXMLParser::GetETag);
					}
					else {
						// invalid! argh! close the current tag "by hand"
						// and continue as if no error happened; note that
						// the currently closing tag remains in the
						// input stream; that's quite important! we'll take
						// care of it a bit later.
						moXMLType *etag = new moXMLType(XML_TYPE_TAG_END, f_name, last_name);
						f_data += *etag;

						size_t l = f_name.Length();
						f_name = f_name.Delete(static_cast<unsigned int>(l - last_name.Length() - 1),
									static_cast<unsigned int>(l));
					}
				}
			}
			else {
				// NOTE: the <name ...> is part of the Element
				// rule, but here we can just stack the start tag.
				//
				// TODO: in non strict we certainly want to
				// accept '<' when not followed by a valid
				// name character (i.e. a "standalone" <)
				//
				// TODO: in HTML like DTD we want to auto-close
				// tags which need to be closed (this should
				// be inside the GetSTag() function since that's
				// the one which generate the new name)
				Push(&moXMLParser::GetSTag);
			}
			break;
		}
		r = f_input->XMLGetC();
		if(r < 0) {
			break;
		}
		if(r == '&') {
			f_input->XMLUngetC();
			// the GetReference() function destroys
			// the current string when it reads a name
			moWCString save = current_string;
			r = GetReference();
			current_string = save;
		}
		// NOTE: Spaces should be managed here! (only one
		// space when no 'xml:space reserve' specified)
		// Also any XML space should be transformed into 0x20
		// However, tags such as <PRE> need to keep the tabs
		// and new lines to be correct
		// We could also manage the spaces after we exited
		// this loop
		current_string += r;
	}

	// TODO: we may want to clip the current string here!
	// (i.e. remove the starting and ending spaces)
	// Since \n should be 0x20 chars, it could be clipped as well!
	// However, tags such as <PRE> need special handling with spaces
	// and here we can't really know yet what handling is required.
	// [fix once we have DTD info available]

	if(!current_string.IsEmpty()) {
		moXMLData *data = new moXMLData(XML_TYPE_DATA, f_name);
		data->SetData(current_string);
		f_data += *data;
	}

	return r;
}


/** \brief Read a tag content as a CDATA block.
 *
 * Support [2] [18] [19] [20] [21]
 *
 * This function reads the data between a opening and closing tag
 * when that tag defines the content as CDATA.
 *
 * In HTML, the \<script> tag is one of the tags defined as a CDATA
 * tag.
 *
 * The one thing interpreted is the corresponding closing tag.
 *
 * \return XML_RT_NOERROR or an error code
 *
 * \sa moXMLParser::GetCDSect(void)
 */
int moXMLParser::GetTagCData(void)
{
//	[18]	TDSect		::=	TDStart TData TDEnd
//	[19]	TDStart		::=	'<tag-name ...>' -- checked and eaten in GetSTag()
//	[20]	TData		::=	(Char* - (Char* '</' Char*))
//	[21]	TDEnd		::=	'</tag-name'
//	[ 2]	Char		::=	<all the XML acceptable chars>
	mowc::wc_t		r;

	Pop();

	// get the name of the closing tag
	moXMLTypeSPtr stag = f_data.GetLast();
	moWCString name = stag->GetTagName();
	name = "</" + name;

	r = XML_RT_NOERROR;
	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();

	for(;;) {
		if(f_input->XMLGetPos() >= f_input->XMLCurrentLine().Length()) {
			r = f_input->XMLNextLine();
			if(r < 0) {
				break;
			}
		}
		if(f_input->XMLTestString(name.c_str())) {
			// we only break on the valid closing tag because
			// in a script you could have a string and some
			// closing tags inside these strings which are
			// perfectly valid in the script!--though a
			// script should really be defined in a
			// <![CDATA[...]]> tag...
			break;
		}
		r = f_input->XMLGetC();
		if(r < 0) {
			break;
		}
		current_string += r;
	}

	if(!current_string.IsEmpty()) {
		moXMLData *data = new moXMLData(XML_TYPE_DATA, f_name);
		data->SetData(current_string);
		f_data += *data;
	}

	if(r >= 0) {
		Push(&moXMLParser::GetETag);
	}

	return r;
}


/** \brief Read the input as is.
 *
 * This function reads the entire input as is and saves it in
 * an moXMLData with the XML_TYPE_DATA type.
 *
 * The data is not processed in any way. It can be any text file
 * including XML or HTML.
 *
 * \note
 * This function is not here to support XML but instead to support
 * the Include() function that can be used to include any file including
 * text files that are not XML or HTML data.
 *
 * \return An error or XML_RT_EOF
 *
 * \sa moXMLParser::Include(moXMLStream& xml_stream, const moWCString& type)
 */
int moXMLParser::GetPlainContent(void)
{
//	[??]	PlainContent	::=	Char*
	mowc::wc_t		r;

	Pop();

	r = XML_RT_NOERROR;
	moWCString& current_string = f_input->XMLCurrentString();
	current_string.Empty();
	for(;;) {
		if(f_input->XMLGetPos() >= f_input->XMLCurrentLine().Length()) {
			r = f_input->XMLNextLine();
			if(r < 0) {
				break;
			}
		}
		r = f_input->XMLGetC();
		if(r < 0) {
			break;
		}
		current_string += r;
	}

	if(!current_string.IsEmpty()) {
		moXMLData *data = new moXMLData(XML_TYPE_DATA, f_name);
		data->SetData(current_string);
		f_data += *data;
	}

	return r;
}


/** \brief Read an opening tag.
 *
 * Support [40], [41], [42], [43], [44]
 *
 * This function processes the opening of a new tag.
 *
 * It is capable of auto-closing empty tags (whether marked empty in the
 * input file or marked empty in the DTD.)
 *
 * It then stacks up the necessary function to read the content of the
 * tag (if not empty.) That other function is responsible for stacking
 * the reading of the closing tag as it could also be missing (in non-strict
 * mode.)
 *
 * This function takes care of reading the attributes.
 *
 * This function adds the tag named to the f_name variable member (unless
 * it is an empty tag.)
 *
 * \return XML_RT_NOERROR or an error code.
 *
 * \sa moXMLParser::GetETag(void)
 */
int moXMLParser::GetSTag(void)
{
//	[40]	STag		::=	'<' Name (S Attribute)* S? '>'
//	[41]	Attribute	::=	Name Eq AttValue
//	[42]	ETag		::=	'</' Name S? '>'
//	[43]	Content		::=	CharData? ((Element | Reference | CDSect | PI | Comment) CharData?)*
//	[44]	EmptyElemTag	::=	'<' Name (S Attribute)* S? '/>'
	mowc::wc_t		r;
	moWCString		name;
	moList::position_t	pos;
	int			idx, max;

	Pop();

	// Before we get the name we want to make sure that this
	// is really the next tag we want to handle. That is, if
	// we need to close a tag instead, then we need to return
	// and not do anything here.

	if(f_use_dtd) {
		// get the parent element definition
		moWCString last_name = f_name.FindRChar(L'/').Delete(0, 0);

		// root tags do not work too well yet here
		if(!last_name.IsEmpty()) {
			moXMLElement parent(f_strict ? last_name : last_name.Uppercase());
			pos = f_dtd_elements.Find(&parent);
			if(pos != moList::NO_POSITION) {
				// we found it, so verify whether this really is a parent
				// (at this time we ignore the tags that we do not find,
				// these could very well be our <mo:db:select ...> & Co.
				moXMLElementSPtr element = f_dtd_elements.Get(pos);

				// Get the name of the tag (+ whatever after it)
				name = f_input->XMLLeftOnCurrentLine();

				// first make sure it is a start tag alright
				r = name.Get(0);
				if(r != '<') {
					return XML_RT_NOERROR;
				}

				// okay, now we expect a valid name
				r = name.Get(1);
				if(!IsXMLNameChar(r, true)) {
					// not a valid name... (in non-strict we may want to be quiet here)
					f_input->FormatError(XML_ERRCODE_BADNAME, "invalid character (&#x%lX;) to start an XML name", r);
					return XML_RT_FATAL;
				}
				max = static_cast<int>(name.Length());
				for(idx = 2; idx < max; ++idx) {
					if(!IsXMLNameChar(name.Get(idx))) {
						break;
					}
				}
				name = name.Delete(idx, max).Delete(0, 0);

				moXMLElement child(f_strict ? name : name.Uppercase());
				pos = f_dtd_elements.Find(&child);
				if(pos != moList::NO_POSITION) {

					if(!IsParentOf(element, f_strict ? name : name.Uppercase())) {
						// invalid! argh! close the current tag "by hand"
						// and continue as if no error happened; note that
						// the currently opening tag remains in the
						// input stream; that's quite important! we'll take
						// care of it a bit later. At this time we need to
						// return to properly manage the stack.
						moXMLType *etag = new moXMLType(XML_TYPE_TAG_END, f_name, last_name);
						f_data += *etag;

						size_t l = f_name.Length();
						f_name = f_name.Delete(static_cast<unsigned int>(l - last_name.Length() - 1),
									static_cast<unsigned int>(l));
						return XML_RT_NOERROR;
					}
				}
			}
		}
	}

// okay! we don't need to close, go ahead and read that start tag
	r = f_input->XMLGetC();
	if(r != '<') {
		f_input->XMLUngetC();
		return XML_RT_NOERROR;
	}

	r = GetName(SPACES_NONE);
	if(r < 0) {
		return r;
	}

	// TODO: in non-strict we should search for this named tag
	// to see if it is defined as empty. If so, we don't want
	// to stack it here.
	//
	// In strict mode, we can search and err in case it is
	// defined as empty in the DTD but isn't in the source.

	// append the name of this tag
	// keep the current name in case this is an empty tag
	moWCString old_name = f_name;
	f_name += "/";
	f_name += f_input->XMLCurrentString();

	moXMLTag *stag = new moXMLTag(f_name, f_input->XMLCurrentString());
	f_data += *stag;

	for(;;) {
		moVariableSPtr var;
		r = GetAttribute(var);
		if(r < 0) {
			// should we restore the name here?
			// I think we need to test the DTD for this
			// tag to know if it is supposed to be empty
			//f_name = old_name;
			return r;
		}
		if(var) {
			if(stag->Get(var->Name()) != 0) {
				f_input->FormatError(XML_ERRCODE_DEFINED_TWICE, "the attribute \"%S\" is defined twice", var->Name().Data());
			}
			else {
				// this makes a copy of the variable
				stag->Set(*var);
			}
		}
		if(r == '/') {
			f_input->XMLSkipC();
			r = f_input->XMLGetC();
			if(r != '>') {
				f_input->FormatError(XML_ERRCODE_SYNTAX, "'>' was expected after the '/' character");
				f_input->XMLUngetC();
			}
			stag->SetPosition(GetPosition());
			stag->MarkAsEmpty();
			f_name = old_name;
			return XML_RT_NOERROR;
		}
		if(r == '>') {
			f_input->XMLGetC();	// skip this '>' char!
			break;
		}
		if(!var) {
			// there is (most certainly) an invalid character here!
			r = f_input->XMLGetC();
			f_input->FormatError(XML_ERRCODE_SYNTAX, "'%c' (%X) wasn't expected here", r, r);
		}
	}

#if 0
{
char b[1024], c[1024];	/* Flawfinder: ignore */
fprintf(stderr, "[STag] Tag name [%s] [%s]\n", f_name.MBData(b, sizeof(b)),
					stag->MBData(c, sizeof(c)));
}
#endif

	stag->SetPosition(GetPosition());

	// The end tag will be pushed on the stack by the
	// GetContent() function whenever necessary.
	// This will allow for unmatched closing tags such
	// as the <td> and <tr> tags in an HTML document
	// which don't need to be closed.
	//Push(&moXMLParser::GetETag);

	if(IsCDataTag(*stag)) {
		Push(&moXMLParser::GetTagCData);
	}
	else {
		Push(&moXMLParser::GetContent);
	}

	return XML_RT_NOERROR;
}



/** \brief Check whether two elements have a parent/child relationship.
 *
 * This function searches an element entries to know whether it supports
 * a specified element name as a child tag.
 *
 * This function is recursive. It uses the moXMLParser::moXMLElement::IsParentOf()
 * function to know whether an element has the specified \p child_name
 * as a direct child definition.
 *
 * \param[in] element The parent element searched for a child named \p child_name.
 * \param[in] child_name The name of the child element to search for.
 *
 * \return true if the child name is defined as one of the entries.
 */
bool moXMLParser::IsParentOf(const moXMLElement *element, const moWCString& child_name) const
{
	moList::position_t	max, idx, pos;

	if(element == 0) {
		return false;
	}

	// direct parent?
	if(element->IsParentOf(child_name)) {
		return true;
	}

	// Note found here, check all the children...
	max = element->EntryNameCount();
	for(idx = 0; idx < max; ++idx) {
		moXMLElement child(element->GetEntryName(idx));
		pos = f_dtd_elements.Find(&child);
		if(pos != moList::NO_POSITION) {
			// we found it, so verify whether this really is a parent
			// (at this time we ignore the tags that we do not find,
			// these could very well be our <mo:db:select ...> & Co.
			moXMLElementSPtr child_element = f_dtd_elements.Get(pos);
			if(child_element->IsStartTagOptional()) {
				if(IsParentOf(child_element, child_name)) {
					return true;
				}
			}
		}
	}

	return false;
}



/** \brief Read and match the closing tag.
 *
 * Support [42]
 *
 * This function reads a closing tag. It is used by the GetContent()
 * function whenever \</ is found in the stream.
 *
 * This function will always pop itself.
 *
 * If the closing tag does not correspond to the opening tag, then
 * this function generates an error. In order to support HTML relax
 * mode, this function needs to be called only when the corresponding
 * tag is being closed. This is accomplished in the GetContent() function.
 *
 * This function removes the tag name from the f_name variable member.
 *
 * \return XML_RT_NOERROR or an error code (negative)
 *
 * \sa moXMLParser::GetSTag(void)
 * \sa moXMLParser::GetContent(void)
 */
int moXMLParser::GetETag(void)
{
//	[42]	ETag		::=	'</' Name S? '>'
	mowc::wc_t		r;
	size_t			l;
	compare_t		cmp;

	Pop();

	r = f_input->XMLGetC();
	if(r != '<') {
		// if this happens then it is not an error
		f_input->XMLUngetC();
		return r < 0 ? r : XML_RT_NOERROR;
	}

#ifdef MO_DEBUG
	// the f_name parameter cannot get empty before the stack
	// get us back out of the list of root tag contents
	// unless there is a bug
	if(f_name.IsEmpty()) {
		fflush(stdout);
		fprintf(stderr, "INTERNAL ERROR: f_name is empty and GetETag() is being called!\n");
		abort();
	}
#endif

	r = f_input->XMLGetC();
	if(r != '/') {
		f_input->FormatError(XML_ERRCODE_SYNTAX, "a '/' after the '<' was expected to close tag: \"%S\".", f_name.Data());
		f_input->XMLUngetC();
		// try to continue the parsing... without that '<'!
		return XML_RT_NOERROR;
	}

	r = GetName();
	if(r < 0) {
		return r;
	}

	r = f_input->XMLSkipSpaces();
	if(r < 0) {
		return r;
	}
	if(r != '>') {
		f_input->FormatError(XML_ERRCODE_BADETAG, "bad closing tag: \"%S\" -- no '>' found", f_input->XMLCurrentString().Data());
	}
	else {
		f_input->XMLSkipC();
	}

	// note that 'last_name' won't include the '/'
	moWCString last_name = f_name.FindRChar(L'/').Delete(0, 0);

//f_input->FormatError(XML_ERRCODE_BADETAG, "closing name = \"%S\"", f_current_string.Data());
//f_input->FormatError(XML_ERRCODE_BADETAG, "expecting name = \"%S\"", f_name.Data());
//f_input->FormatError(XML_ERRCODE_BADETAG, "last name = \"%S\"", last_name.Data());

	if(!f_strict) {
		// in HTML we don't want to compare case sensitive
		cmp = last_name.CaseCompare(f_input->XMLCurrentString());
	}
	else {
		cmp = last_name.Compare(f_input->XMLCurrentString());
	}

	if(cmp == MO_BASE_COMPARE_EQUAL) {
		// we got a match

		// create the end tag before we change the name
		moXMLType *etag = new moXMLType(XML_TYPE_TAG_END, f_name, last_name);
		f_data += *etag;

		l = f_name.Length();
		f_name = f_name.Delete(static_cast<unsigned int>(l - last_name.Length() - 1),
					static_cast<unsigned int>(l));
	}
	else {
		f_input->FormatError(XML_ERRCODE_BADETAG, "closing tag mismatch: \"%S\" -- it doesn't match the last opening tag: \"%S\"", f_input->XMLCurrentString().Data(), last_name.Data());
	}
#if 0
fprintf(stderr, "Last name after Delete() = [%s] / [%s] cmp = %d -> [%s]\n",
	last_name.MBData(), f_current_string.MBData(), cmp,
	f_name.MBData());
#endif

	return XML_RT_NOERROR;
}










/** \brief Add the name of a tag which content is CDATA.
 *
 * The parser needs to know about all the tags that content
 * has to be read as CDATA. This is important since otherwise
 * the parser would parse data within these tags in an improper
 * way (i.e. seeing a tag in a Javascript string for instance!)
 *
 * The content of tags marked as having CDATA are parsed using
 * GetTagCData() instead of GetContent().
 *
 * This is important, especially for entries in HTML between
 * tags such as \<script> and \<style>.
 * 
 * \param[in] name Name of the tag which content is CDATA.
 *
 * \sa moXMLParser::IsCDataTag(const moXMLTag& tag) const
 */
void moXMLParser::AddCDataTag(const moWCString& name)
{
	moWCStringSPtr n = new moWCString(name);
	f_cdata_tags.Insert(n);
}


/** \brief Check whether a tag contents CDATA.
 *
 * This function gets the name of the tag parameter
 * and searches for it in the list of tags which have been
 * registered with the AddCDataTag() function. If a match is
 * found it returns true, otherwise it returns false.
 * 
 * \param[in] tag Tag to check against the list of CDATA tags.
 *
 * \return true if the specified tag is a tag defined as containing CDATA only.
 *
 * \sa moXMLParser::AddCDataTag(const moWCString& name)
 */
bool moXMLParser::IsCDataTag(const moXMLTag& tag) const
{
	const moWCString& name = tag.GetTagName();
	return f_cdata_tags.Find(&name) != moList::NO_POSITION;
}




/** \brief Whether the parser should keep comments.
 *
 * When this function is called with true, it makes the parser
 * return comments on ReadNext() calls. This can be useful in
 * cases where you (1) want to copy a file to, for instance,
 * reformat it or (2) to read a script which was otherwise
 * commented out.
 *
 * \param[in] status Whether to return comments (true) or not (false)
 *
 * \return The previous status of this flag
 *
 * \sa moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals = false)
 */
bool moXMLParser::ReturnComments(bool status)
{
	bool		old_status;

	old_status = f_return_comments;
	f_return_comments = status;

	return old_status;
}



/** \brief Determine whether entities are kept or not
 *
 * When this function is called with true, it makes the parser
 * return entities as is. This means entities will not be
 * transformed to characters. Thus &quot; will remain &quot;
 * instead of becoming ".
 *
 * In general, this is used when the read XML file has to be
 * written back to another XML file and the entities need to
 * be kept as is.
 *
 * This function changes a flag. It should be called before
 * the parsing begins.
 *
 * \note
 * This flag doesn't prevent DTD entities (%&lt;name&gt;;) from being
 * converted.
 *
 * \param[in] status Whether to keep entities (true) or not (false)
 *
 * \return The previous state of this flag
 *
 * \sa moXMLParser::GetReference(void)
 * \sa moXMLParser::GetPEReference(bool skip_spaces, bool add_spaces, bool loop)
 * \sa moXMLParser::ReadNext(moXMLTypeSPtr& data, bool delete_signals = false)
 */
bool moXMLParser::KeepEntities(bool status)
{
	bool		old_status;

	old_status = f_keep_entities;
	f_keep_entities = status;

	return old_status;
}



/** \brief Request the XML parser to read the DTD
 *
 * This function can be used to determine whether the XML parser
 * should read the DTD for the XML being read.
 *
 * This function only sets a flag. The reading of the DTD will
 * happen at another time.
 *
 * \param[in] status Whether the DTD should be read
 *
 * \return The previous state
 *
 * \sa moXMLParser::GetAnySubset(void)
 */
bool moXMLParser::ReadDTD(bool status)
{
	bool		old_status;

	old_status = f_read_dtd;
	f_read_dtd = status;

	return old_status;
}


/** \brief Return the list of XML elements
 *
 * After a DTD was read, this function can be used to retrieve the list
 * of elements acceptable in a corresponding XML document.
 *
 * The elements are defined with the <!ELEMENT ...> declaration.
 *
 * \code
 *	<!ELEMENT BR - O EMPTY       -- forced line break -->
 * \endcode
 *
 * \return A list of XML elements
 *
 * \sa moXMLParser::DTDGetEntities(void) const
 * \sa moXMLParser::DTDGetParsedEntities(void) const
 */
const moXMLParser::moSortedListUniqueOfXMLElements& moXMLParser::DTDGetElements(void) const
{
	return f_dtd_elements;
}


/** \brief Return the XML entities
 *
 * There are two types of entities in a DTD definition. One are called the
 * XML entities, defined without the extra % character in the definition:
 *
 * \code
 *	<!ENTITY nbsp   CDATA "&#160;" -- no-break space = non-breaking space,
 *					  U+00A0 ISOnum -->
 * \endcode
 *
 * These entities are saved in this list.
 *
 * These entities are used to read XML files and convert &amp;&lt;name&gt;; references.
 *
 * \return A list of XML entities
 *
 * \sa moXMLParser::DTDGetElements(void) const
 * \sa moXMLParser::DTDGetParsedEntities(void) const
 */
const moXMLParser::moSortedListUniqueOfXMLEntities& moXMLParser::DTDGetEntities(void) const
{
	return f_dtd_entities;
}


/** \brief Return the DTD entities
 *
 * There are two types of entities in a DTD definition. One are called the
 * DTD entities, defined with an extra % character in the definition:
 *
 * \code
 *	<!ENTITY % i18n 
 *		"lang        %LanguageCode; #IMPLIED  -- language code --
 *		 dir         (ltr|rtl)      #IMPLIED  -- direction for weak/neutral text --"
 *		 >
 * \endcode
 *
 * These entities are saved in this list.
 *
 * Once a DTD was read, these entities are usually of no use.
 *
 * \return A list of XML entities
 *
 * \sa moXMLParser::DTDGetEntities(void) const
 * \sa moXMLParser::DTDGetElements(void) const
 */
const moXMLParser::moSortedListUniqueOfXMLEntities& moXMLParser::DTDGetParsedEntities(void) const
{
	return f_dtd_parsed_entities;
}





/** \brief Stack a new function to be called later.
 *
 * So as to call all the functions in the proper order and
 * to have a recursive effect, the function pointers are
 * stacked in the f_stack array. We need this stack and not
 * the processor stack because the XML parser reads elements
 * one by one instead of reading the entire XML document at
 * once.
 * 
 * The Push() function adds one new function and the Pop()
 * function remove one function from the stack. In general,
 * functions pop themselves once they are done reading their
 * own data (i.e. when the GetContent() function finds a
 * closing tag it pops itself.)
 * 
 * When a specific token is found in a function meaning that
 * another sub-function should be called, then it will stack
 * a new function pointer with Push() and return.
 * 
 * When a function cannot find any valid token and it can
 * return (i.e. it does not expect to find a valid token
 * anymore) then it calls Pop(). This means the next function
 * on the stack will be called.
 * 
 * If the stack becomes empty and the end of the input file
 * was not reached yet, then there is some invalid extra
 * tokens in the input stream. If the end of the stack is not
 * reached but the end of the input file is reached, there are
 * missing tags.
 *
 * In this latter case, we should be able to skip the invalid
 * tags. In the former case, we can either emit a warning
 * or an error (remember that a warning can be turned off).
 * 
 * \param[in] func The function to stack.
 */
void moXMLParser::Push(xml_get_func_t func)
{
	f_stack.Insert(&func);
}


/** \brief Unstack the top most function.
 *
 * This function is called whenever a function reading some
 * data from the XML input stream has nothing more to do.
 * This prevents that function from being called again.
 */
void moXMLParser::Pop(void)
{
	f_stack.SetSize(f_stack.Count() - 1);
}







/** \brief Get the next character.
 *
 * This function calls the corresponding f_input->GetC() function.
 *
 * However, if the f_input function returns EOF, it first
 * tries to pop the current f_input to read more data from the
 * previous file instead of returning XML_RT_EOF.
 *
 * This is repeated as many times as necessary.
 *
 * \return The next character from the input stream.
 *
 * \sa moXMLParser::XMLSkipSpaces(void)
 */
mowc::wc_t moXMLParser::XMLGetC(void)
{
	mowc::wc_t	r;

	for(;;) {
		r = f_input->XMLGetC();
		if(r != XML_RT_EOF) {
			// not the end, return as is
			return r;
		}
		r = PopInclude();
		if(r < 0) {
			// that's really the end or an error occured
			return r;
		}
	}
	/*NOTREACHED*/
}


/** \brief Skip XML spaces from the input stream.
 *
 * This function reads the input files until a character that is not
 * a space is returned.
 *
 * Like the XMLGetC() function, this function will pop streams whenever
 * the current stream returns EOF.
 *
 * \return The next character except spaces.
 *
 * \sa moXMLParser::XMLGetC(void)
 */
mowc::wc_t moXMLParser::XMLSkipSpaces(void)
{
	mowc::wc_t	r;

	for(;;) {
		r = f_input->XMLSkipSpaces();
		if(r != XML_RT_EOF) {
			// not the end, return as is
			return r;
		}
		r = PopInclude();
		if(r < 0) {
			// that's really the end or an error occured
			return r;
		}
	}
	/*NOTREACHED*/
}


/** \class moXMLParser::moXMLStream
 *
 * \brief A class used to specialize the moTextStream
 *
 * This class is used to support an input stream that can transform
 * input data on the fly as required by the moXMLParser. This includes
 * interpreting variables and changing the input from some "random"
 * encoding to USC-4.
 *
 * The encapsulation of the moTextStream also enables the XML stream
 * to have many specialized functions such as a way to skip spaces.
 */


/** \brief Initialize the XML stream object.
 *
 * At least one XML stream object is required for the XML parser to work.
 * The input is mandatory, though it can set to a null pointer, it will
 * then create an empty XML stream. If possible, you should avoid those.
 * 
 * Once you have a working parser, you can create additional XML streams
 * and call the Include() function to add them at the current position.
 * This is often done whenever an input in the original XML files generates
 * data that also need to be parsed by the XML parser.
 * 
 * For example, if you have a tag \<DATE> that you want to change to the
 * current date, you can create a moIOStream to which you write the date in
 * a text format, then you create an moXMLStream using that moIOStream of yours
 * as the input file and finally call Include() with the new XML stream. This
 * will now read from your stream until the EOF is found. At that time, the
 * parser releases the moXMLStream and returns to using the original input file.
 * The number of includes is not limited.
 * 
 * The OnNewStream() function is used so the system can ensure the proper
 * convertor is in place for any new input stream defined on the moXMLStream
 * object. Also, it will clear the state as if the moXMLStream had not be
 * used before.
 *
 * \param[in] input The XML input file.
 * \param[in] output Unused.
 * \param[in] error The error stream.
 */
moXMLParser::moXMLStream::moXMLStream(moIStream *input, moOStream *output, moOStream *error)
	: moTextStream(input, output, error),
	  f_variables("XMLStream"),
	  f_variable_reducer(this)
{
	CleanNL();				// we want to remove \r\n and get only \n
	SetLineSeparators("+\n, +>\n?");	// keep the \n and stop on > so very long lines are broken up

	//f_convertor = ... -- automatic
	// in case Clean() was called we need to restore the encoding
	// also we could have had a user encoding changing this
	f_convertor.SetEncodings("*", "*");
	// NOTE: this should never fail...

	if(input != 0) {
		input->SetInputFilter(&f_convertor);
	}

	f_pos = 0;			// position in the current line chars
	//f_current_line = "" -- automatic
	//f_current_string = "" -- automatic
	//f_name = "" -- automatic

	f_interpret_variables = false;

	f_parser = 0;
}

/** \brief Clean up the XML stream.
 *
 * In particular, this function removes the input filter. This is important to
 * ensure a flush occurs before we destroy everything.
 */
moXMLParser::moXMLStream::~moXMLStream()
{
	moIStreamSPtr	input;

	input = Input();
	if(input) {
		// we need to disconnect because f_convertor is
		// an object part of ourself
		input->SetInputFilter(0);
	}
}


/** \brief Called whenever a new stream is defined.
 *
 * This function is called whenever the stream is changed.
 *
 * The \p which parameter is expected to be set to MO_WHICH_INPUT
 * when the input stream is being changed. The moTextStream calls
 * this function as required.
 *
 * \param[in] which Defines which one of the stream is being modified.
 */
void moXMLParser::moXMLStream::OnNewStream(int which)
{
	moIStreamSPtr	input;

	if(which == MO_WHICH_INPUT) {
		// clear everything about the previous state
		f_pos = 0;
		f_current_line.Empty();
		f_current_string.Empty();
		input = Input();
		if(input) {
			f_convertor.Reset();

			// Even when we didn't yet read the encoding="..." string we
			// use a filter since we may be reading a UTF-16 or UTF-32
			// file!
			input->SetInputFilter(&f_convertor);
		}
	}

	// call the inherited OnNewStream() function
	// just in case something happens there
	moTextStream::OnNewStream(which);
}


/** \brief Forces the encoding of the XML stream.
 *
 * This function is used to change the encoding from the default.
 * The default is to try to detect the encoding of the input file.
 * If that fails, it uses UTF-8, which is the XML default.
 *
 * param[in] encoding The name of the new encoding.
 *
 * \return true when the new encoding is accepted.
 */
bool moXMLParser::moXMLStream::XMLSetInputEncoding(const moWCString& encoding)
{
	char		encname[256];	/* Flawfinder: ignore */

	// names will either not overflow or be wrong
	encoding.MBData(encname, sizeof(encname));

	// note: the default output is UTF-32, so using "*" we avoid
	//	 having an iconv open (which would otherwise be pretty
	//	 much for nothing)
	return f_convertor.SetEncodings(encname, "*") == 0;
}




/** \brief Set the parser pointer in this stream.
 *
 * In order to have the full functionality of the GetVariable()
 * you need to have a pointer to the parser. This function is used
 * for that reason.
 *
 * Individual XML streams can only query their own variables,
 * not their parents (included by).
 *
 * \param[in] parser The parser this stream is being used by.
 */
void moXMLParser::moXMLStream::XMLSetParser(moXMLParser& parser)
{
	f_parser = &parser;
}




/** \brief Get the status of the interpretation of variables.
 *
 * This function returns whether variables are interpreted.
 *
 * If true, then variables such as $(name) are interpreted.
 *
 * \return true if the variables are currently interpreted.
 */
bool moXMLParser::moXMLStream::XMLGetInterpretVariables(void) const
{
	return f_interpret_variables;
}


/** \brief Set the status of the interpretation of variables.
 *
 * By default an moXMLStream parser does not interpret variables. If you
 * call this function with true, then it will understand and convert
 * any variable found in the input stream with its content.
 * 
 * The syntax for a variable is as follow:
 * 
 * \code
 * $(<name>)
 * $(<command> <arguments>)
 * \endcode
 * 
 * That the blanks between a command and any of the arguments can
 * include new lines characters.
 * 
 * The \<name>, \<command> and \<arguments> can themselves include
 * variable references (i.e. it is fully recursive.) Thus, the
 * following returns 'result', which is the current value of the
 * variable named test:
 * 
 * \code
 * <mo:set name="var" value="test">
 * <mo:set name="test" value="result">
 * $($(var))
 * \endcode
 * 
 * First it transforms $(the.name) into the string "test" and then
 * it applies the $() again using that name. This in turns is
 * transformed into the corresponding value, which is "result".
 * 
 * \param[in] interpret_variables Whether the $(\<name> ...)'s have to be interpreted.
 *
 * \sa moXMLParser::moXMLStream::XMLGetInterpretVariables(void) const
 */
void moXMLParser::moXMLStream::XMLInterpretVariables(bool interpret_variables)
{
	f_interpret_variables = interpret_variables;
}





/** \brief Read one line from the input stream.
 *
 * This function is the low level function reading one line
 * of input fromt he input stream attached to the moTextStream
 * of the moXMLStream.
 *
 * This function takes care of removing any 0xFEFF character
 * from the input stream as well as any invalid character.
 *
 * \return Either XML_RT_NOERROR or XML_RT_EOF
 */
int moXMLParser::moXMLStream::XMLNextLineNow(void)
{
	mowc::wc_t	r;
	unsigned long	p, flags;

	f_pos = 0;

	// Below we will replace the \r in case it was the
	// only separator.
	// We will remove it if it was part of the \r\n
	// sequence.
	//
	// KNOWN BUGS: at this time the NextLine() function
	// will not accept \r by itself as a newline character.
	if(!NextLine(f_current_line)) {
		// TODO: we need to check for errors also
		f_current_line.Empty();
		return XML_RT_EOF;
	}

	// make sure all invalid chars are gone
	p = static_cast<unsigned long>(f_current_line.Length());
	while(p > 0) {	// lines can be empty so the while() needs to be here
		p--;
		r = f_current_line.Get(p);
		flags = XMLCType(r);
		if((flags & XML_CTYPE_INVALID) != 0 || r == 0x0FEFF) {
			// TODO: in strict mode, should we have an error?
			//	 (bad chars should be rare anyway)
			f_current_line = f_current_line.Delete(p, p);
		}
	}

#if 0
fflush(stdout);
fprintf(stderr, "Line [%S]\n", (wchar_t *) f_current_line.Data());
#endif

	return XML_RT_NOERROR;
}


/** \brief Read on line of data.
 *
 * This function reads one line of data from the input stream, clean
 * up the line from invalid characters (including 0xFEFF characters)
 * and, if turned on, it interpret variables as defined in
 * XMLInterpretVariables().
 *
 * The interpretation of the variables may call the moXMLParser to
 * get to the parent file or get the content of some special variable.
 *
 * \return XML_RT_NOERROR or XML_RT_EOF
 *
 * \sa moXMLParser::moXMLStream::XMLInterpretVariables(bool interpret_variables)
 */
int moXMLParser::moXMLStream::XMLNextLine(void)
{
	int		r;

	r = XMLNextLineNow();

	// if the user requested variables to be interpreted, we
	// need to do it here!
	if(r == XML_RT_NOERROR && f_interpret_variables) {
		moWCString result;
		f_variable_reducer.Reduce(result);
		f_current_line = result;
		f_pos = 0;
	}

	return r;
}


/** \brief Get the next character.
 *
 * This function reads one character from the input buffer.
 *
 * Whenever the current line is empty, it reads the next line
 * using XMLNextLine().
 *
 * \return The next character or XML_RT_EOF
 *
 * \sa moXMLParser::moXMLStream::XMLNextLine(void)
 */
mowc::wc_t moXMLParser::moXMLStream::XMLGetC(void)
{
	int		r;

	while(f_pos >= f_current_line.Length()) {
		r = XMLNextLine();
		if(r < 0) {
			return r;
		}
	}

	// when we reach here we cleaned the string of invalid chars
	return f_current_line.Get(f_pos++);
}


/** \brief Get one character from the current line.
 *
 * This function reads one character from the current line.
 *
 * If the current line has been read in full, it returns XML_RT_EOF.
 *
 * Use the XMLGetC() instead, if you want to continue reading the
 * input file.
 *
 * This function is particularly useful if you need to ensure that
 * the XMLUngetC() works properly.
 *
 * \return The next character or XML_RT_EOF
 */
mowc::wc_t moXMLParser::moXMLStream::XMLGetCBlock(void)
{
	if(f_pos >= f_current_line.Length()) {
		// this is the end of line
		return XML_RT_EOF;
	}

	// when we reach here we cleaned the string of invalid chars
	return f_current_line.Get(f_pos++);
}


/** \brief Restore the last character to the stream.
 *
 * Whenever you read one more character to check for a token and
 * that character needs to be restored, call this function.
 *
 * The function cannot be called to restore characters between lines.
 * In general, this means you cannot restore more than one character
 * without taking the risk of getting an exception.
 *
 * \exception moBug(MO_ERROR_UNDERFLOW)
 * This exception is thrown whenever you call this function too many
 * times. Do not forget that this call is limited to the current line
 * of text.
 *
 * \sa moXMLParser::moXMLStream::XMLSkipC(int count)
 */
void moXMLParser::moXMLStream::XMLUngetC(void)
{
	if(f_pos == 0) {
		// TODO: This can break whenever we use sub-streams
		//	 Should it be an moError?
		throw moBug(MO_ERROR_UNDERFLOW, "moXMLParser::moXMLStream::XMLUngetC(): called too many times");
	}

	f_pos--;
}



/** \brief Skip \p count characters.
 *
 * This function is used to skip \p count characters in the current line
 * of input. Note that this function will not automatically read the
 * next line of data. If \p count is too large, you get an exception.
 *
 * \exception moBug(MO_ERROR_OVERFLOW)
 * This exception is thrown whenever count is too large. This is an internal
 * error if it happens.
 *
 * \param[in] count The number of characters to skip.
 *
 * \sa moXMLParser::moXMLStream::XMLUngetC(void)
 */
void moXMLParser::moXMLStream::XMLSkipC(int count)
{
	if(f_pos + count > f_current_line.Length()) {
		// NOTE: Should this be an moError?
		throw moBug(MO_ERROR_OVERFLOW, "moXMLParser::moXMLStream::XMLSkipC(): called with too large a count");
	}

	f_pos += count;
}



/** \brief Set the current line position.
 *
 * This function can be used to change the current position
 * within the current line of text.
 *
 * The position can first be retrieved with XMLGetPos().
 *
 * It can be useful to read some data and then restore the
 * pointer in case the input did not look like some expected
 * value.
 *
 * \exception moBug(MO_ERROR_OVERFLOW)
 * This exception is thrown when the \p pos parameter is too large.
 *
 * \param[in] pos The new position in the current line of text.
 */
void moXMLParser::moXMLStream::XMLSetPos(unsigned long pos)
{
	if(pos > f_current_line.Length()) {
		// NOTE: Should this be an moError?
		throw moBug(MO_ERROR_OVERFLOW, "moXMLParser::moXMLStream::XMLSetPos() called with too large a value");
	}

	f_pos = pos;
}



/** \brief Get the current position within the current line.
 *
 * This function returns the current position in the current
 * line. This position is meaningful only until the next line
 * is read.
 *
 * Note that it is not easy to know when a new line of data
 * is read. Whenever the XMLGetC() function reaches the end
 * of the line, it automatically reads the next line with
 * a call to XMLNextLine(). Once that happened, this position
 * loses its meaning.
 *
 * \return The current position in the current line.
 */
unsigned long moXMLParser::moXMLStream::XMLGetPos(void) const
{
	return f_pos;
}



/** \brief Read a string of the specified number of characters.
 *
 * This functions reads \p count characters and saves them in
 * the \p str parameter.
 *
 * It is expected that this function will be used to read a
 * number of characters small enough that all the characters
 * are found in the current line.
 *
 * \param[in] count The number of characters to read
 * \param[out] str A reference to an moWCString where the result is saved
 *
 * \return XML_RT_NOERROR unless an error occurs
 * 		in which case the error code is returned
 */
int moXMLParser::moXMLStream::XMLGetString(moWCString& str, int count)
{
	mowc::wc_t	r;

	// TODO: we should use a direct copy of the f_current_line
	//	 string for speed!
	// TODO: this function expects all the characters to already be in
	//	 f_current_line; XMLGetC() should not return EOF
	str.Empty();
	while(count > 0) {
		r = XMLGetC();
		if(r < 0) {
			return r;
		}
		count--;
		str += r;
	}

	return XML_RT_NOERROR;
}


/** \brief Compare the current line with a parameter string.
 *
 * This function compares the parameter \p str with the current
 * line of the XML input stream.
 *
 * The current line is the last line that was read whenever a
 * call to ReadNext() is made.
 *
 * This function does not change the XML stream pointer. It
 * only compares the string in memory in the current line.
 *
 * \param[in] str The string to compare with.
 * \param[in] case_sensitive If true, compare as is,
 * 	otherwise compare ignoring lower and upper case characters
 *
 * \return true if the current string at the current position is equal to the input string.
 */
bool moXMLParser::moXMLStream::XMLTestString(const char *str, bool case_sensitive) const
{
	compare_t	r;
	int		l;

	l = static_cast<int>(strlen(str));	/* Flawfinder: ignore */

	if(case_sensitive) {
		r = f_current_line.Compare(str, f_pos, l);
	}
	else {
		r = f_current_line.CaseCompare(str, f_pos, l);
	}

#if 0
fprintf(stderr, "Comparing [%s] with {%s} %s on %d chars -> %d\n",
			str,
			f_current_line.SavedMBData() + f_pos,
			case_sensitive ? "Case Sensitive" : "Case Ignored",
			l, r);
#endif

	return r == MO_BASE_COMPARE_EQUAL;
}


/** \brief Compare the current line with a string.
 *
 * Check whether the current line is equal to a given name.
 *
 * The \p skip parameter defines how many characters need to be
 * skipped in the current line before to test is applied.
 * When \p skip is negative, the function also skips spaces.
 *
 * \param[in] skip The number of characters to skip before applying the test
 * \param[in] str The name to test against
 * \param[in] case_sensitive Whether the test is case sensitive (true) or not
 *
 * \return true if the name matches, false otherwise
 */
bool moXMLParser::moXMLStream::XMLTestName(int skip, const char *str, bool case_sensitive) const
{
	compare_t	r;
	int		p, l;

	l = static_cast<int>(strlen(str));	/* Flawfinder: ignore */

	// if skip is too large, the Get() will throw
	p = f_pos + labs(skip);
	if(skip < 0) {
		// in non-strict, allow skipping spaces
		while(moXMLParser::IsXMLSpace(f_current_line.Get(p))) {
			++p;
		}
	}

	if(case_sensitive) {
		r = f_current_line.Compare(str, p, l);
	}
	else {
		r = f_current_line.CaseCompare(str, p, l);
	}

#if 0
fprintf(stderr, "Comparing [%s] with {%s} %s on %d chars -> %d\n",
			str,
			f_current_line.SavedMBData() + p,
			case_sensitive ? "Case Sensitive" : "Case Ignored",
			l, r);
#endif

	return r == MO_BASE_COMPARE_EQUAL;
}


/** \brief Return a copy of what is left on the current line.
 *
 * This function retrieves what is left to be read on the current
 * line. This has no effect on the current position and thus
 * the following XMLGetC() will return the same characters
 * whether this function is called or not.
 *
 * \param[in] skip Some characters to skip before returning
 *
 * \return A copy of the left over in the current line
 */
const moWCString moXMLParser::moXMLStream::XMLLeftOnCurrentLine(int skip) const
{
	return f_current_line.Get(f_pos + skip, -1);
}


/** \brief Skip all spaces.
 *
 * This function reads the current input stream and skips all
 * the spaces. The next character is left in the input stream.
 *
 * A character is known to be a space if the IsXMLSpace()
 * function returns true.
 *
 * \return The next character or an error code
 */
mowc::wc_t moXMLParser::moXMLStream::XMLSkipSpaces(void)
{
	mowc::wc_t	r;

	do {
		r = XMLGetC();
	} while(moXMLParser::IsXMLSpace(r));
	if(r >= 0) {
		XMLUngetC();
	}

	return r;
}


/** \brief Get a copy of the current string.
 *
 * This function returns a copy of the current string that
 * was previously defined by low level functions such as
 * GetName().
 *
 * \return A constant reference to the current string.
 */
const moWCString& moXMLParser::moXMLStream::XMLCurrentString(void) const
{
	return f_current_string;
}



/** \brief Get the current string reference.
 *
 * This function is used by different low level parser functions
 * such as GetName() that read a string and needs to save it in
 * some place.
 *
 * \return a reference to the current string
 */
moWCString& moXMLParser::moXMLStream::XMLCurrentString(void)
{
	return f_current_string;
}



/** \brief Return the current line.
 *
 * This function returns a copy of the line that has been read last.
 * This is viewed as the current line.
 *
 * \return A reference to the current line string.
 */
const moWCString& moXMLParser::moXMLStream::XMLCurrentLine(void) const
{
	return f_current_line;
}



/** \brief Search a sub-string in the current line.
 *
 * This function is used to scan the current line for a given
 * string.
 *
 * \param[in] str The sub-string to search in the current line.
 * \param[in] length The length of str or -1 for the whole string.
 *
 * \return The position where the sub-string is found or -1.
 */
long moXMLParser::moXMLStream::XMLFindInCurrentLine(const moWCString& str, long length) const
{
	return f_current_line.FindInString(str, f_pos, length);
}




/** \brief Save the current name for include functionality test.
 *
 * The XMLSaveName() function is used whenever the
 * moXMLParser::Include() function is called. It saves the current
 * tag name the parser is in. This is used to ensure that when the
 * included file is removed from the stack, we have the correct
 * tag and not an unclosed tag from within the included file.
 * 
 * \param[in] name The current name of tags (i.e. /html/body/table/tbody/tr/td/b)
 */
void moXMLParser::moXMLStream::XMLSaveName(const moWCString& name)
{
	f_name = name;
}


/** \brief Retrieve the last saved name.
 *
 * Return the last name that was saved with XMLSaveName().
 *
 * \return The last name saved.
 */
const moWCString& moXMLParser::moXMLStream::XMLGetName(void) const
{
	return f_name;
}





/** \brief Append a list of variables to this stream.
 *
 * This function adds all the variables from the input
 * list to the moXMLStream object set of variables. Note
 * that the existing variables will be overwritten. Use the
 * XMLSafeAppendVariableList() function instead if you do not
 * want to overwrite the existing variables.
 *
 * \param[in] list A list of variables to append to this stream.
 *
 * \sa moXMLParser::moXMLStream::XMLSafeAppendVariableList(const moVariableList& list)
 */
void moXMLParser::moXMLStream::XMLAppendVariableList(const moVariableList& list)
{
	f_variables.Append(list);
}


/** \brief Append a list of variables to this stream.
 *
 * This function adds all the variables from the input
 * list to the moXMLStream object set of variables. Note
 * that it does not overwrite existing variables, leaving
 * them at their current value. Use the
 * XMLAppendVariableList() function instead if you
 * want to overwrite the existing variables.
 *
 * \param[in] list A list of variables to append to this stream.
 *
 * \sa moXMLParser::moXMLStream::XMLAppendVariableList(const moVariableList& list)
 */
void moXMLParser::moXMLStream::XMLSafeAppendVariableList(const moVariableList& list)
{
	f_variables.SafeAppend(list);
}


/** \brief Set a variable.
 *
 * This function copies the input variable in this XML stream set of variables.
 *
 * \param[in] var The variable to save in this XML stream
 *
 * \return A pointer to the variable saved in the XML stream object.
 */
moVariableSPtr moXMLParser::moXMLStream::XMLSetVariable(const moVariable& var)
{
	return f_variables.Set(var);
}


/** \brief Set a variable from a string.
 *
 * This function parses a string for a variable name, an equal sign and
 * a value. It ignores spaces around the equal sign.
 *
 * Then the value is saved in a variable named after the word defined before
 * the equal sign.
 *
 * \param[in] set A string with a variable name, an equal sign and a value.
 *
 * \return A pointer to the variable saved in the XML stream object.
 */
moVariableSPtr moXMLParser::moXMLStream::XMLSetVariable(const moWCString& set)
{
	return f_variables.Set(set);
}


/** \brief Set a variable from a name and a value.
 *
 * This function sets the named variable to the specified value.
 *
 * \param[in] name The name of the variable to set.
 * \param[in] value The value of the variable to set.
 *
 * \return A pointer to the variable saved in the XML stream object.
 */
moVariableSPtr moXMLParser::moXMLStream::XMLSetVariable(const moWCString& name, const moWCString& value)
{
	return f_variables.Set(name, value);
}


/** \brief Get a variable by name.
 *
 * This function searches for a variable of the specified name and returns
 * a pointer to it or NULL if not found.
 *
 * This function does not have the capability to search in any XML stream
 * currently defined in the corresponding parser.
 *
 * \return A pointer to the variable found in the XML stream object or NULL.
 */
moVariableSPtr moXMLParser::moXMLStream::XMLGetVariable(const moWCString& name) const
{
	return f_variables.Get(name);
}


/** \brief Get a variable by path.
 *
 * This function accepts a path to a variable and a variable name (basename
 * in the given path.)
 *
 * The variable is searched using the moXMLParser::GetVariable() function
 * and thus it supports a full path as defined in that function definition.
 *
 * \return A pointer to the variable found in any one of the XML stream of the parser or NULL.
 */
moVariableSPtr moXMLParser::moXMLStream::XMLGetExtendedVariable(const moWCString& name) const
{
	return f_parser == 0 ? XMLGetVariable(name) : f_parser->GetVariable(name);
}


/** \brief Delete the named variable.
 *
 * This function searches for a variable. If it finds it, it removes it.
 * Otherwise it does nothing.
 *
 * \param[in] name The name of the variable to delete.
 */
void moXMLParser::moXMLStream::XMLUnsetVariable(const moWCString& name)
{
	f_variables.Unset(name);
}



/** \class moXMLParser::moXMLStream::moReducer
 *
 * \brief Reducer to support entities within variables.
 *
 * This class is used by the parser to reduce entities within variables
 * whenever a variable is being reduced.
 *
 * For instance, if you have \$(My\&apos;Var), the \&apos; needs to be changed
 * to an apostrophe (').
 */


/** \brief Initializes the reducer.
 *
 * This function sets up a reducer with a reference back to the XML stream.
 *
 * \exception moBug(MO_ERROR_INTERNAL)
 * This exception should just never occur. If it were to happen, it would mean
 * that the RegisterCommand() call failed and the variables are not properly
 * converted.
 *
 * \param[in] stream The corresponding moXMLStream
 */
moXMLParser::moXMLStream::moReducer::moReducer(moXMLStream *stream)
{
	f_error = false;
	f_stream = stream;
	//f_expand_entities -- auto-init

	// so we can expand entities in variables if need be (useful in tags for attributes)
	if(!RegisterCommand(f_expand_entities)) {
		throw moBug(MO_ERROR_INTERNAL, "moXMLParser::moXMLStream::moReducer::moReducer(): could not register the expand entities event");
	}
}


/** \brief Unregister the reducer from the variable reducer.
 *
 * The destructor ensures that the reducer is not any more
 * registered in the variable reducer.
 */
moXMLParser::moXMLStream::moReducer::~moReducer()
{
	// we must unregister so the ref. count is 1 instead of 2 on destruction
	UnregisterCommand(f_expand_entities);
}


/** \brief Reduce the specified string.
 *
 * This function is called whenever a string needs to be reduced.
 *
 * \param[in] result The string to be reduced.
 */
int moXMLParser::moXMLStream::moReducer::Reduce(moWCString& result)
{
	// When f_error is marked true, we only return EOF and we don't read
	// any more input. This should minimize the errors.
	f_error = false;
	return moVariableReducer::Reduce(result);
}


/** \brief Get a variable by name.
 *
 * This function is called whenever a variable is not resolved by the
 * variable reducer.
 *
 * The XML parser accepts environment variables defined with the
 * "env_..." introducer. For instance, to access your home directory,
 * use "env_HOME".
 *
 * If it is not an environment variable, this function calls
 * XMLGetExtendedVariable().
 *
 * \note
 * If the variable is undefined, then this function calls the
 * Undefined() function that may emit a warning.
 *
 * \param[in] name The name of the variable to search for.
 *
 * \return The content of the variable or an empty string.
 */
moWCString moXMLParser::moXMLStream::moReducer::GetVariable(const moWCString& name)
{
	moVariableSPtr	v;

	if(name.Get(0, 3) == "env_") {
		/* this variable is not always defined, plus it could be set to
		 * a spurious value, we just bypass right here... */
		if(name.Get(4, -1) == "PPID") {
			pid_t id(getpid());
			return moWCString::Format("%d", id);
		}
		return moVariableReducer::GetVariable(name.Get(4, -1));
	}

	v = f_stream->XMLGetExtendedVariable(name);
	if(!v) {
		Undefined(name);
		return "";
	}

	return *v;
}


/** \brief Return the next character.
 *
 * This function reads the next character to form the name of the variable
 * to reduce. Here we read letters using the XMLGetCBlock() function.
 *
 * The depth parameter represents how many parenthesis deep we currently
 * are (i.e. $($($(...))) the ... are 2 deep.)
 *
 * When the depth is not zero and an ampersand is found, it transform the
 * entity if valid. Unknown entities are left as is. (This function only
 * transform basic XML entities; namely: lt, gt, apos, quot, amp.)
 *
 * \param[in] depth The current depth while reading.
 *
 * \return The next character read or MO_VR_EOF
 */
mowc::wc_t moXMLParser::moXMLStream::moReducer::GetC(unsigned long depth)
{
	mowc::wc_t	c, buf[16];
	int		r, idx;

	if(f_error) {
		return MO_VR_EOF;
	}

	for(;;) {
		c = f_stream->XMLGetCBlock();
		if(c == XML_RT_EOF) {
			if(depth == 0) {
				// we're done on that one
				return MO_VR_EOF;
			}
			r = f_stream->XMLNextLineNow();
			if(r < 0) {
				return MO_VR_EOF;
			}
		}
		else {
			// CHANGE Feb 16, 2005
			// we want to change entities only within
			// variables otherwise it won't work correctly
			// since the &lt; would be detected as the
			// character to open a tag which would be
			// wrong in this case
			if(depth != 0) {
				if(c == '&') {
					// ha! this may be a "low level" entity which we need to
					// support (i.e. &quot; &lt; &gt; &apos;)
					idx = 0;
					do {
						buf[idx] = f_stream->XMLGetCBlock();
						idx++;
					} while(idx < 5 && buf[idx - 1] != ';');
					buf[idx] = '\0';
					if(mowc::strcmp(buf, "quot;", LONG_MAX) == 0) {
						return '"';
					}
					if(mowc::strcmp(buf, "lt;", LONG_MAX) == 0) {
						return '<';
					}
					if(mowc::strcmp(buf, "gt;", LONG_MAX) == 0) {
						return '>';
					}
					if(mowc::strcmp(buf, "apos;", LONG_MAX) == 0) {
						return '\'';
					}
					if(mowc::strcmp(buf, "amp;", LONG_MAX) == 0) {
						return '&';
					}
					// TODO: Gee's this is slow! We need to fix it at some point
					while(idx > 0) {
						idx--;
						f_stream->XMLUngetC();
					}
					// not a valid low level entity, ignore it
					return '&';
				}
				if(c == '<' || c == '>') {
					f_stream->XMLUngetC();
					f_stream->FormatError(XML_ERRCODE_SYNTAX, "variables are not allowed to include a '<' or '>' character. This is mainly to protect against missing characters in expressions.");
					f_error = true;
					return MO_VR_EOF;
				}
			}
			return c;
		}
	}
	/*NOTREACHED*/
}




/** \class moXMLParser::moXMLStream::moReducer::moReduceExpandEntities
 *
 * \brief Reducer used to expand XML entities in variable name and data.
 *
 * This object is used to extend the moVariableReducer so it understands
 * how to expand XML entities.
 */


/** \brief Execute the entities expansion.
 *
 * This function is automatically called by the moVariableReducer when it
 * wants to expand the entities in a string.
 *
 * The first string in the parameters array (that cannot be empty) has
 * its entities expanded (i.e. \&lt; becomes \<, etc.) Only the basic
 * entities are expanded (lt, gt, quot, apos, amp). We also expand the
 * apostrophe when defined as \#39 or \#x27.
 *
 * The result of the expansion is appended to the result string.
 *
 * \param[in] parameters A list of parameters, we use the first one only.
 * \param[in,out] result String were we append the expanded result.
 *
 * \return Always return zero.
 */
int moXMLParser::moXMLStream::moReducer::moReduceExpandEntities::Execute(const moListOfWCStrings& parameters, moWCString& result)
{
	//result = parameters.Get(0)->Replace("<=&lt\\;;>=&gt\\;;'=&#27\\;;\"=&quot\\;;&=&amp\\;");
	result += parameters.Get(0)->Replace("&amp\\;=&;&gt\\;=>;&lt\\;=<;&quot\\;=\";&apos\\;=';&#39\\;=';&#x27\\;='");
	return 0;
}


/** \brief Get the name of this reducer object.
 *
 * This function returns the name of this variable reducer.
 *
 * \return The string "expand_entities".
 */
const moWCString moXMLParser::moXMLStream::moReducer::moReduceExpandEntities::Name(void) const
{
	return "expand_entities";
}



/** \class moXMLParser::moXMLStringReducer
 *
 * \brief The string reducer of the XML parser.
 *
 * This object is used to reduce variable with the input data saved
 * in a string (to avoid having to save the data on disk or in an moMemFile
 * object.)
 */


/** \brief Initializes the string reducer.
 *
 * This function initializes the string reducer object with the string that
 * needs to be read by the variable reducer and a stream to have access to
 * the variables.
 *
 * The reducer is read to be used after its constructor was called.
 */
moXMLParser::moXMLStringReducer::moXMLStringReducer(const moWCString& string, moXMLStream *stream)
{
	f_pos = 0;
	f_string = string;
	f_stream = stream;
}


/** \brief Get a variable by name.
 *
 * This function returns the content of a variable as defined by \p name.
 *
 * \param[in] name The variable name that we have to retrieve.
 *
 * \return The content of the variable or an empty string.
 */
moWCString moXMLParser::moXMLStringReducer::GetVariable(const moWCString& name)
{
	moVariableSPtr	v;

	if(name.Get(0, 3) == "env_") {
		return moVariableReducer::GetVariable(name.Get(4, -1));
	}

	v = f_stream->XMLGetExtendedVariable(name);
	if(!v) {
		Undefined(name);
		return "";
	}

	return *v;
}


/** \brief Get the next character.
 *
 * This function gets the next character from the string to reduce.
 *
 * \param[in] depth This parameter is ignored.
 *
 * \return The next character or MO_VR_EOF when no more character is available
 */
mowc::wc_t moXMLParser::moXMLStringReducer::GetC(unsigned long depth)
{
	mowc::wc_t	c;

	// note that the depth is ignored here, we can't go on once the
	// string is fully consumed.
	if(f_pos >= f_string.Length()) {
		return MO_VR_EOF;
	}

	c = f_string[f_pos];
	f_pos++;

	return c;
}


/** \var moXMLParser::XML_TYPE_UNKNOWN
 *
 * \brief The default type when unspecified.
 *
 * Should never be used for an XML object!
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_TAG_START
 *
 * \brief A start tag.
 *
 * The object contains the name of the starting tag.
 *
 * Also, it includes a list of the attributes and their values.
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_TAG_END
 *
 * \brief An end tag.
 *
 * The object contains the name of the ending tag.
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_PI
 *
 * \brief A PI target.
 *
 * A PI, the name is the PI name and the data is what was read
 * between the '<?name' and the '?>' markers.
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_DATA
 *
 * \brief Data between tags.
 *
 * Data read between a start and end tag; it is transformed as required
 * (the &...; that is)
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_CDATA
 *
 * \brief CDATA read inside a CDATA declaration.
 *
 * Data read between a '<![CDATA[' and a ']]>' marker; the data is
 * not transformed in any way. It will include all the spaces, comments,
 * entities, etc. as is.
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_COMMENT
 *
 * \brief The actual comment.
 *
 * A standard XML comment written between '<!--' and '-->'.
 *
 * The markers are not included. The spaces are all kept as is.
 *
 * Note that by default comments are not returned. You need to call
 * the ReturnComments() function to get them.
 *
 * \sa moXMLParser::ReturnComments(bool status)
 * <br>
 * <br>
 */

/** \var moXMLParser::XML_TYPE_max
 *
 * \brief Marks the limit of all the types.
 *
 * This is the upper limit of all the types. The lower limit is zero.
 *
 * An invalid object as a negative value (-1 or moXMLParser::XML_TYPE_UNKNOWN)
 * <br>
 * <br>
 */



/** \class moXMLParser::moXMLType
 *
 * \brief Base type for all the XML elements
 *
 * The moXMLType() base object is used to define the different
 * XML elements. These objects are automatically created by
 * the XML parser as it reads the given items from the XML
 * input file.
 * 
 * An moXMLType is derived from an moWCString and thus is
 * a string. The string of an moXMLType object represents
 * the full name of the tag (for instance, in HTML it could
 * be like this: "/html/head/title"). The tag name parameter
 * is the name of this very tag without the parent names
 * (in our last example, it is: "title")
 * 
 * Note that the other XML object types are all based on
 * this one.
 *
 * As is, the moXMLType is used for the moXMLParser::XML_TYPE_TAG_END.
 */


/** \brief Initializes an XML type object.
 *
 * This function initializes an XML type object.
 *
 * When the tag_name is not specified, it is automatically defined
 * as the basename of name.
 *
 * \param[in] type The type of this XML type object.
 * \param[in] name The name of this XML type object (i.e. path to current tag.)
 * \param[in] tag_name The name of this XML tag (i.e. basename of path.)
 */
moXMLParser::moXMLType::moXMLType(xml_type_t type, const moWCString& name, const moWCString& tag_name)
	: moWCString(name), f_type(type), f_tag_name(tag_name.IsEmpty() ? name.FilenameBasename() : tag_name)
{
}

/** \brief Get the type of this XML element.
 *
 * This function returns the type of this XML element.
 * See the moXMLParser::xml_type_t declaration for a complete list of
 * types.
 *
 * \return One of the XML_TYPE_... enumeration values.
 */
moXMLParser::xml_type_t moXMLParser::moXMLType::GetType(void) const
{
	return f_type;
}

/** \brief Retrieve the name of the tag.
 *
 * This function retrieves the name of the tag.
 *
 * \return The basename of the XML type full name.
 */
const moWCString& moXMLParser::moXMLType::GetTagName(void) const
{
	return f_tag_name;
}

/** \brief Transform this XML type to string.
 *
 * If it is an XML_TYPE_TAG_END, then it generates an end tag.
 * Otherwise, it returns nothing.
 *
 * \param[out] string The empty tag end or an empty string.
 */
void moXMLParser::moXMLType::ToString(moWCString& string) const
{
	// at this time there is only one special case here
	if(f_type == XML_TYPE_TAG_END) {
		string = moWCString("</") + f_tag_name + ">";
	}
	else {
		string.Empty();
	}
}


/** \brief Save a position to repeat or otherwise come back to a certain point.
 *
 * The SetPosition() and GetPosition() functions can be used
 * to save and restore a size_t value. Most of the time, the
 * SetPosition() is used to save the current moXMLStream
 * position (in all cases, when an event occurs, the current
 * position of the moXMLStream object is right after the
 * data which just triggered the event). The GetPosition()
 * will return the last saved value. In case of an
 * OpenTag/CloseTag pair, these functions will be used to
 * repeat the input multiple times (i.e. in case of a for
 * loop, such as \<mo:for ...>data\</mo:for>, it will save
 * the position right after the first closing bracket >).
 *
 * \param[in] position The position to save in this type.
 *
 * \sa moXMLParser::moXMLType::GetPosition(void) const
 */
void moXMLParser::moXMLType::SetPosition(size_t position)
{
	f_position = position;
}

/** \brief Retrieve the position stored by SetPosition().
 *
 * This position defines the position last saved with the
 * SetPosition() function.
 *
 * \return The position last saved with SetPosition()
 *
 * \sa moXMLParser::moXMLType::SetPosition(size_t position)
 */
size_t moXMLParser::moXMLType::GetPosition(void) const
{
	return f_position;
}



/** \class moXMLParser::moXMLData
 *
 * \brief A specialized moXMLType for data.
 *
 * This class is used to create XML type objects holding some sort of
 * raw data.
 *
 * The moXMLData class is used to represent:
 * moXMLParser::XML_TYPE_PI,
 * moXMLParser::XML_TYPE_COMMENT,
 * moXMLParser::XML_TYPE_DATA and
 * moXMLParser::XML_TYPE_CDATA
 */

/** \brief Initializes no moXMLData object.
 *
 * Initializes the moXMLData object with the specified full path in
 * name and the basename in tag_name. You should set tag_name if you
 * have it as is.
 *
 * \param[in] type The type of the XML data object.
 * \param[in] name The name of this XML data object (i.e. path to current tag.)
 * \param[in] tag_name The name of the current XML tag (i.e. basename of path.)
 */
moXMLParser::moXMLData::moXMLData(xml_type_t type, const moWCString& name, const moWCString& tag_name)
	: moXMLType(type, name, tag_name)
{
	//f_data = ... -- automatically empty
}


/** \brief Transform an moXMLData to a string.
 *
 * This function transforms the moXMLData to a string.
 *
 * \param[out] string The string where the result is saved.
 */
void moXMLParser::moXMLData::ToString(moWCString& string) const
{
	// at this time there is only one special case here
	switch(GetType()) {
	case XML_TYPE_PI:
		string = moWCString("<?") + GetTagName() + " " + f_data + "?>";
		break;

	case XML_TYPE_COMMENT:
		string = moWCString("<!-- ") + f_data + " -->";
		break;

	case XML_TYPE_CDATA:
		string = moWCString("<![CDATA[") + f_data + "]]>";
		break;

	default:	// XML_TYPE_DATA
		string = f_data;
		break;

	}
}


/** \brief Save the XML data.
 *
 * This function is used to set the actual data of this moXMLData object.
 *
 * It is expected to be a PI target data, a comment, a CDATA data or
 * data between a start and an end tag.
 *
 * \param[in] data The data to save in this object.
 */
void moXMLParser::moXMLData::SetData(const moWCString& data)
{
	f_data = data;
}


/** \brief Retrieve the XML data.
 *
 * This function returns the data previously set with SetData().
 *
 * \return The data of this moXMLData object.
 */
const moWCString& moXMLParser::moXMLData::GetData(void) const
{
	return f_data;
}



/** \class moXMLParser::moXMLTag
 *
 * \brief A start tag with its attributes and name.
 *
 * This object is used to represent a start tag including
 * its attributes and its name.
 *
 * The type is moXMLParser::XML_TYPE_TAG_START.
 */

/** \brief Initializes an moXMLTag
 *
 * This function initializes the moXMLTag object using the
 * moXMLParser::XML_TYPE_TAG_START type.
 *
 * \p name is the full path to this tag and \p tag_name
 * is the basename from the full path.
 *
 * \param[in] name The name of this XML data object (i.e. path to current tag.)
 * \param[in] tag_name The name of the current XML tag (i.e. basename of path.)
 */
moXMLParser::moXMLTag::moXMLTag(const moWCString& name, const moWCString& tag_name)
	: moXMLType(XML_TYPE_TAG_START, name, tag_name),
	  f_attributes("XML Start Tag")
{
	f_empty = false;
}


/** \brief Transform the tag object to a string.
 *
 * This function transforms the tag to a string including its name
 * and attributes.
 *
 * \param[out] string The resulting string.
 */
void moXMLParser::moXMLTag::ToString(moWCString& string) const
{
	moList::position_t	pos, max;
	moVariableSPtr		var;

	string = moWCString("<") + GetTagName();

	max = f_attributes.Count();
	for(pos = 0; pos < max; pos++) {
		var = f_attributes.Get(pos);
		string += moWCString(" ") + var->Name() + "=\"" + *var + "\"";
	}

	if(f_empty) {
		string += "/";
	}

	string += ">";
}


/** \brief Get one attribute by name.
 *
 * This function searches for the specified attribute by name.
 *
 * \param[in] name The name of the attribute to retrieve.
 *
 * \return A pointer to a variable or a NULL pointer.
 */
moVariableSPtr moXMLParser::moXMLTag::Get(const moWCString& name) const
{
	return f_attributes.Get(name);
}


/** \brief Set one attribute value.
 *
 * This function saves the value of an attribute and its value
 * in the moXMLTag object.
 *
 * \param[in] var A variable with a valid name and value for an attribute.
 *
 * \return A pointer to the newly created variable.
 */
moVariableSPtr moXMLParser::moXMLTag::Set(const moVariable& var)
{
	return f_attributes.Set(var);
}


/** \brief Get the list of all the attributes.
 *
 * If you need to go through all the attributes, you can obtain the
 * entire list and work with it at once.
 *
 * \return A reference to the internal variable list.
 */
moVariableList& moXMLParser::moXMLTag::GetList(void)
{
	return f_attributes;
}


/** \brief Get the list of all the attributes (constant).
 *
 * If you need to go through all the attributes, you can obtain the
 * entire list and work with it at once.
 *
 * \return A reference to the internal variable list.
 */
const moVariableList& moXMLParser::moXMLTag::GetList(void) const
{
	return f_attributes;
}


/** \brief Mark this tag as empty.
 *
 * Tags are considered empty when ending with '/>'. Also, some tags
 * can be marked as empty if it is defined as such in the DTD.
 *
 * \param[in] empty Whether the tag is empty (true) or not (false.)
 */
void moXMLParser::moXMLTag::MarkAsEmpty(bool empty)
{
	f_empty = empty;
}


/** \brief Check whether this tag was marked as empty.
 *
 * This function checks to see whether the tag was marked as an
 * empty tag.
 *
 * \return true if the tag is indeed market as empty.
 */
bool moXMLParser::moXMLTag::IsEmpty(void) const
{
	return f_empty;
}



/** \class moXMLParser::moXMLEvent
 *
 * \brief An event to catch tags as they are found.
 *
 * This event can be used to receive notifications each time a tag is
 * found.
 *
 * Events are named with the name of a tag and notification is sent
 * whenever that tag is found.
 *
 * For instance, if you are parsing an HTML document and are interested
 * in looking at the image tags, you can setup an event with the name
 * "img". The OnEvent() will then be called each time an image is found
 * in the source HTML code.
 */

/** \brief Initializes the moXMLEvent object.
 *
 * This function initializes an moXMLEvent object.
 *
 * The priority and name can be specified to get more events.
 *
 * \param[in] priority The priority of the event in regard to other events.
 * \param[in] name The name of these events.
 */
moXMLParser::moXMLEvent::moXMLEvent(int priority, const moWCString& name)
	: f_priority(priority), f_name(name)
{
}


/** \brief Cleans up an moXMLEvent.
 *
 * At this time, this function does nothing.
 */
moXMLParser::moXMLEvent::~moXMLEvent()
{
}


/** \brief Default OnEvent() function
 *
 * This is the default OnEvent() function call. The idea is simple: by
 * default we do nothing of all the tags. New classes that derive from
 * this class and implement this function.
 *
 * \param[in] parser The can be used to, for instance, retrieve variables, etc.
 * \param[in] type The type of the last tag we read and which is triggering this event.
 */
bool moXMLParser::moXMLEvent::OnEvent(moXMLParser& parser, moXMLType& type)
{
	// by default the event has no effect
	return false;
}


/** \brief Compare two events.
 *
 * This function compares two events between each others.
 *
 * First, it compares them by name. If not equal, then that
 * result is returned.
 *
 * When two events have the same name, then the comparison
 * compares the priority of each event. The event with the
 * smaller priority is considered to go after the event with
 * a larger priority.
 *
 * If one or the other event have an undefined priority
 * (XML_NO_PRIORITY) then both events are considered
 * equal if they have the same name.
 *
 * \return MO_BASE_COMPARE_GREATER, MO_BASE_COMPARE_SMALLER or MO_BASE_COMPARE_EQUAL.
 */
moBase::compare_t moXMLParser::moXMLEvent::Compare(const moBase& object) const
{
	const moXMLEvent& event = dynamic_cast<const moXMLEvent&>(object);

	compare_t r = f_name.Compare(event.f_name);
	if(r != MO_BASE_COMPARE_EQUAL || f_priority == XML_NO_PRIORITY || event.f_priority == XML_NO_PRIORITY) {
		return r;
	}

	// WARNING: we want to have the largest first
	if(f_priority < event.f_priority) {
		return MO_BASE_COMPARE_GREATER;
	}
	if(f_priority > event.f_priority) {
		return MO_BASE_COMPARE_SMALLER;
	}

	return MO_BASE_COMPARE_EQUAL;
}



namespace {
/// List of error modes recognized by ErrorModeValue()
enum {
	ERROR_MODE_FATAL = 1,
	ERROR_MODE_ERROR,
	ERROR_MODE_WARNING,
	ERROR_MODE_MESSAGE,
	ERROR_MODE_QUIET
};

/** \brief Transform an error mode to a number.
 *
 * This function transforms a named error mode into a number.
 * The comparison is done case insensitive.
 *
 * The following describes the possible strings:
 *
 * \li fatal ERROR_MODE_FATAL (1)
 * \li error ERROR_MODE_ERROR (2)
 * \li warning ERROR_MODE_WARNING (3)
 * \li message ERROR_MODE_MESSAGE (4)
 * \li quite ERROR_MODE_QUIET (5)
 *
 * If an unrecognized string is passed in, then ERROR_MODE_ERROR
 * is returned.
 *
 * \param[in] error_mode A string pointer to convert in an error mode
 *
 * \return One of the error mode number
 */
int ErrorModeValue(moWCString *error_mode)
{
	if(error_mode == 0) {
		// default is "error"
		return ERROR_MODE_ERROR;
	}

	if(error_mode->CaseCompare("fatal")) {
		return ERROR_MODE_FATAL;
	}

	if(error_mode->CaseCompare("error")) {
		return ERROR_MODE_ERROR;
	}

	if(error_mode->CaseCompare("warning")) {
		return ERROR_MODE_WARNING;
	}

	if(error_mode->CaseCompare("message")) {
		return ERROR_MODE_MESSAGE;
	}

	if(error_mode->CaseCompare("quiet")) {
		return ERROR_MODE_QUIET;
	}

	// default is "error"
	return ERROR_MODE_ERROR;
}
}		// no name namespace


/** \brief Print out a warning message.
 *
 * This function is used throughout the XML event code to generate
 * a warning to the user.
 *
 * The warning is formatted here and then forwarded to the parser.
 *
 * Note that the warning can be avoided if the tag defines the
 * attribute "error_mode" set to a level higher than ERROR_MODE_WARNING.
 *
 * \param[in] parser The XML parser used to emit this event.
 * \param[in] tag The tag being read when this warning occurs.
 * \param[in] warcode The warning code; this code has no special meaning in the XML parser.
 * \param[in] format The warning message followed by parameters a la printf(3C).
 */
void moXMLParser::moXMLEvent::Warning(moXMLParser& parser, moXMLParser::moXMLTag& tag, moTextStream::errcode_t warcode, const char *format, ...)
{
	const int value = ErrorModeValue(tag.Get("error_mode"));

	if(value <= ERROR_MODE_WARNING)
	{
		// in this case, emit a warning, otherwise be quiet
		//
		va_list		args;

		va_start(args, format);
		try {
			parser.VFormatWarning( warcode, format, args );
		}
		catch(...)
		{
		// unfortunately we cannot have an RAII object for this one
			va_end(args);
			throw;
		}
		va_end(args);
	}
}


/** \brief Print out an error message.
 *
 * This function is used throughout the XML event code to generate
 * an error to the user.
 *
 * The error is formatted here and then forwarded to the parser.
 *
 * Note that the error can be avoided if the tag defines the
 * attribute "error_mode" set to a level higher than ERROR_MODE_ERROR.
 *
 * \param[in] parser The XML parser used to emit this event.
 * \param[in] tag The tag being read when this error occurs.
 * \param[in] errcode The error code; this code has no special meaning in the XML parser.
 * \param[in] format The error message followed by parameters a la printf(3C).
 */
void moXMLParser::moXMLEvent::Error(moXMLParser& parser, moXMLParser::moXMLTag& tag, moTextStream::errcode_t errcode, const char *format, ...)
{
	const int value = ErrorModeValue(tag.Get("error_mode"));

	if(value <= ERROR_MODE_ERROR) {
		// in this case, emit an error, otherwise be quiet
		//
		va_list	args;
	
		va_start(args, format);
		try {
			parser.VFormatError(errcode, format, args);
		}
		catch(...)
		{
			// unfortunately we cannot have an RAII object for this one
			va_end(args);
			throw;
		}
		va_end(args);	
	}
}






/** \class moXMLParser::moXMLElement::moXMLEntry
 *
 * \brief A DTD entry definition.
 *
 * This object is used to define one possible entry (sub-element or
 * child element) of an element defined in a DTD.
 *
 * An entry has a name, possibly sub-entries and a repetition
 * definition.
 */


/** \brief Initializes an entry object.
 *
 * This function is used to initialize the XML entries.
 *
 * By default, the entries have no name, receive a
 * repetition mode of REPEAT_ONCE and are of type
 * ENTRY_PCDATA.
 */
moXMLParser::moXMLElement::moXMLEntry::moXMLEntry(void)
{
	//f_has_pcdata -- auto-init
	f_type = ENTRY_PCDATA;
	f_repeat = REPEAT_ONCE;
	//f_name -- auto-init
	//f_sub_entries -- auto-init
}


/** \brief Clean up the entry object.
 *
 * This destructor ensures that all the resources used by the entry object
 * are released.
 */
moXMLParser::moXMLElement::moXMLEntry::~moXMLEntry()
{
}


/** \brief Set the type of this entry.
 *
 * This function changes the type of this entry.
 *
 * \param[in] type One of ENTRY_EMPTY, ENTRY_ANY, ENTRY_PCDATA, ENTRY_NAME,
 * 		ENTRY_CHOICE, ENTRY_SEQUENCE or ENTRY_UNORDERED
 */
void moXMLParser::moXMLElement::moXMLEntry::SetType(type_t type)
{
	f_type = type;
}


/** \brief Set the repetition of this entry.
 *
 * This function sets the repetition of the entry.
 *
 * \param[in] repeat One of REPEAT_ONCE, REPEAT_ANY, REPEAT_ONE_OR_MORE, REPEAT_ZERO_OR_ONE
 */
void moXMLParser::moXMLElement::moXMLEntry::SetRepeat(repeat_t repeat)
{
	f_repeat = repeat;
}


/** \brief Set the name of this entry.
 *
 * This function changes the name of this entry.
 *
 * \bug
 * If the entry is already saved in a list, the list may get
 * corrupted if the name is changed.
 *
 * \param[in] name The new name of the entry.
 */
void moXMLParser::moXMLElement::moXMLEntry::SetName(const moWCString& name)
{
	if(!f_name) {
		f_name = new moWCString(name);
	}
	else {
		*f_name = name;
	}
}



/** \brief Get the type of this entry object.
 *
 * This function returns the type of this entry.
 *
 * Note that CHOICE and SEQUENCE entries have sub-entries.
 *
 * \return One of ENTRY_EMPTY, ENTRY_ANY, ENTRY_PCDATA, ENTRY_NAME,
 * 		ENTRY_CHOICE, ENTRY_SEQUENCE or ENTRY_UNORDERED
 */
moXMLParser::moXMLElement::moXMLEntry::type_t moXMLParser::moXMLElement::moXMLEntry::GetType(void) const
{
	return f_type;
}


/** \brief Returns the repetition of this entry.
 *
 * This function returns one of the repetition of this entry.
 *
 * \return One of REPEAT_ONCE, REPEAT_ANY, REPEAT_ONE_OR_MORE or REPEAT_ZERO_OR_ONE.
 */
moXMLParser::moXMLElement::moXMLEntry::repeat_t moXMLParser::moXMLElement::moXMLEntry::GetRepeat(void) const
{
	return f_repeat;
}


/** \brief Returns the name of the entry.
 *
 * Each entry has a name that usually correspond to the name of a tag.
 *
 * \return The name of the entry in a string.
 */
const moWCString& moXMLParser::moXMLElement::moXMLEntry::GetName(void) const
{
	if(f_name) {
		return *f_name;
	}
	static const moWCString empty;
	return empty;
}


/** \brief This entry has a \#PCDATA.
 *
 * This function returns true if any one of its child entries
 * or its has a \#PCDATA definition.
 *
 * \bug
 * It looks like this flag is not being duplicated in the parent
 * entries. This may be a problem.
 *
 * \return true if \#PCDATA is defined.
 */
bool moXMLParser::moXMLElement::moXMLEntry::HasPCDATA(void) const
{
	return f_has_pcdata;
}


/** \brief Returns the number of sub-entries.
 *
 * This function is used to know how many sub-entries this entry
 * defines. A set of sub-entries are defined between parenthesis.
 *
 * \return The number of sub-entries.
 */
unsigned long moXMLParser::moXMLElement::moXMLEntry::Count(void) const
{
	return f_sub_entries.Count();
}


/** \brief Get one of the sub-entries.
 *
 * This function is used to retrieve a sub-entry by index.
 *
 * The index parameter can be defined between 0 and Count() - 1.
 *
 * When Count() returns zero, no sub-entries are defined and
 * this function cannot be called.
 *
 * \param[in] idx The index of the sub-entry to return.
 *
 * \return An entry pointer.
 */
moXMLParser::moXMLElement::moXMLEntry *moXMLParser::moXMLElement::moXMLEntry::Get(int idx) const
{
	return f_sub_entries.Get(idx);
}


/** \brief Add a sub-entry to this entry.
 *
 * This function adds a sub-entry to this entry object.
 *
 * If the sub-entry is a \#PCDATA entry, then it sets the
 * corresponding flag to try in this entry.
 *
 * Note that the same sub-entry can be added multiple times
 * as a sub-entry of another entry. However, make sure not
 * to generate loops. (i.e. entry A cannot have B as a
 * direct or indirect sub-entry if A is a sub-entry of B.)
 *
 * \param[in] entry The sub-entry to add to this entry. Can be NULL.
 */
void moXMLParser::moXMLElement::moXMLEntry::AddSubEntry(moXMLEntry *entry)
{
	if(entry != 0) {
		if(entry->GetType() == ENTRY_PCDATA) {
			f_has_pcdata = true;
		}
		f_sub_entries += *entry;
	}
}



/** \brief Compare two entries.
 *
 * This function compares two entries by name.
 */
moBase::compare_t moXMLParser::moXMLElement::moXMLEntry::Compare(const moBase& object) const
{
	// we assume only named elements will be sorted
	return f_name->Compare(static_cast<const moXMLParser::moXMLElement::moXMLEntry&>(object).f_name);
}






/** \class moXMLParser::moXMLElement::moXMLAttribute
 *
 * \brief A DTD attribute definition.
 *
 * This class is used to define one attribute in a start tag.
 *
 * An attribute definition includes its name, type and eventual
 * default value.
 *
 * The type can be an array of XML names representing an enumeration.
 */

/** \brief Initializes an attribute object.
 *
 * This function initializes an attribute object. Specifically it assign
 * its name that cannot be changed later.
 *
 * By default, the attribute type is set to CDATA and the value is
 * set to DEFAULT.
 *
 * No user type name are defined by default.
 *
 * \param[in] name The name of the new attribute.
 */
moXMLParser::moXMLElement::moXMLAttribute::moXMLAttribute(const moWCString& name)
	: f_type(ATTRIBUTE_CDATA),
	  f_value_type(VALUE_DEFAULT),
	  f_name(name)
	  //f_user_types -- auto-init
	  //f_default -- auto-init
{
}


/** \brief Destroys an attribute.
 *
 * This function ensures that the attribute is clean.
 */
moXMLParser::moXMLElement::moXMLAttribute::~moXMLAttribute()
{
}


/** \brief Set the type of the attribute.
 *
 * This function sets the type of the attribute.
 *
 * \param[in] type One of ATTRIBUTE_CDATA, ATTRIBUTE_ID, ATTRIBUTE_IDREF, ATTRIBUTE_IDREFS,
 *		ATTRIBUTE_ENTITY, ATTRIBUTE_ENTITIES, ATTRIBUTE_NMTOKEN, ATTRIBUTE_NMTOKENS,
 *		ATTRIBUTE_NOTATION, ATTRIBUTE_ENUMERATION, ATTRIBUTE_NAME or ATTRIBUTE_NUMBER
 */
void moXMLParser::moXMLElement::moXMLAttribute::SetType(type_t type)
{
	f_type = type;
}


/** \brief Set the type of the attribute value.
 *
 * This function defines the type of the attribute value.
 *
 * \param[in] value_type One of VALUE_REQUIRED, VALUE_IMPLIED, VALUE_FIXED,
 *			VALUE_DEFAULT, VALUE_RECTANGLE, VALUE_TEXT,
 *			VALUE_GET or VALUE_SUBMIT
 */
void moXMLParser::moXMLElement::moXMLAttribute::SetValueType(value_t value_type)
{
	f_value_type = value_type;
}


/** \brief Add a user type name to this attribute.
 *
 * This function tries to insert the specified user type name for this
 * attribute. If the exact same type name already exists, then the new
 * one is not inserted and the function returns false.
 *
 * \param[in] user_name The name to add to the list of acceptable user types.
 *
 * \return true if the type was added.
 */
bool moXMLParser::moXMLElement::moXMLAttribute::AddUserTypeName(const moWCString& user_name)
{
	return f_user_types.Insert(new moWCString(user_name));
}


/** \brief The default default value.
 *
 * This function sets the attribute default value.
 *
 * Until set, the default value is the empty string.
 *
 * \param[in] value The default value of the attribute.
 */
void moXMLParser::moXMLElement::moXMLAttribute::SetDefaultValue(const moWCString& value)
{
	f_default = value;
}


/** \brief Get the attribute name.
 *
 * This is the name of the attribute that can be specified in the
 * corresponding tag.
 *
 * \return A string with the acceptable name.
 */
const moWCString& moXMLParser::moXMLElement::moXMLAttribute::GetName(void) const
{
	return f_name;
}


/** \brief Get the attribute type.
 *
 * This function retrieves the type of the attribute.
 *
 * The type is converted to an enumeration when being read from the DTD.
 *
 * \return One of ATTRIBUTE_CDATA, ATTRIBUTE_ID, ATTRIBUTE_IDREF, ATTRIBUTE_IDREFS,
 *		ATTRIBUTE_ENTITY, ATTRIBUTE_ENTITIES, ATTRIBUTE_NMTOKEN, ATTRIBUTE_NMTOKENS,
 *		ATTRIBUTE_NOTATION, ATTRIBUTE_ENUMERATION, ATTRIBUTE_NAME or ATTRIBUTE_NUMBER
 */
moXMLParser::moXMLElement::moXMLAttribute::type_t moXMLParser::moXMLElement::moXMLAttribute::GetType(void) const
{
	return f_type;
}


/** \brief The the type of the attribute value.
 *
 * This function retrieves the type of the attribute value.
 *
 * The type is converted to an enumeration when being read from the DTD.
 *
 * \return One of VALUE_REQUIRED, VALUE_IMPLIED, VALUE_FIXED,
 *		VALUE_DEFAULT, VALUE_RECTANGLE, VALUE_TEXT,
 *		VALUE_GET or VALUE_SUBMIT
 */
moXMLParser::moXMLElement::moXMLAttribute::value_t moXMLParser::moXMLElement::moXMLAttribute::GetValueType(void) const
{
	return f_value_type;
}


/** \brief Return the number of user types.
 *
 * This function retrieves the number of user types defined in this
 * attribute.
 *
 * \return The number of user types defined.
 */
unsigned long moXMLParser::moXMLElement::moXMLAttribute::Count(void) const
{
	return f_user_types.Count();
}


/** \brief Return the name of a valid user type.
 *
 * This function returns the user type specified by index.
 *
 * The index can be defined between 0 and Count() - 1.
 *
 * If there are no user types, then this function cannot be called.
 *
 * \param[in] idx The index of the user type name to retrieve.
 *
 * \return The user type name in a string.
 */
const moWCString& moXMLParser::moXMLElement::moXMLAttribute::GetUserTypeName(int idx) const
{
	return *f_user_types.Get(idx);
}


/** \brief Get the default value.
 *
 * This function returns the default value of this element attribute.
 *
 * \return The default value in a string.
 */
const moWCString& moXMLParser::moXMLElement::moXMLAttribute::GetDefault(void) const
{
	return f_default;
}


/** \brief Compare two attributes.
 *
 * This function compares two attributes together by name.
 */
moBase::compare_t moXMLParser::moXMLElement::moXMLAttribute::Compare(const moBase& object) const
{
	return f_name.Compare(static_cast<const moXMLParser::moXMLElement::moXMLAttribute&>(object).f_name);
}









/** \class moXMLParser::moXMLElement
 *
 * \brief A DTD element definition.
 *
 * This class represents the definition of an element as defined in
 * a DTD. It includes the name, different possible attributes and
 * a list of child elements if not marked empty.
 */




/** \brief Initialize an element.
 *
 * This function creates an element with the specified name.
 */
moXMLParser::moXMLElement::moXMLElement(const moWCString& name)
	: f_name(name)
{
	//f_entries -- auto-init
	//f_except_entries -- auto-init
	//f_additional_entries -- auto-init
	//f_attlist -- auto-init
	//f_entry_names -- auto-init
	//f_optional_start -- auto-init
	//f_optional_end -- auto-init
}


/** \brief Clean up an element definition.
 *
 * This destructor cleans up an element object.
 */
moXMLParser::moXMLElement::~moXMLElement()
{
}


/** \brief Set an entry that this element supports.
 *
 * Each element is composed of a name and a list of entries.
 * In the element, only one top entry is defined. That top
 * entry can be composed on any number of sub-entries.
 *
 * \param[in] entry The input entry object to attach to this object
 */
void moXMLParser::moXMLElement::SetEntry(moXMLEntry *entry)
{
	f_entries = entry;
}


/** \brief Set an entry that this element does NOT support.
 *
 * At times, it is easier to use the same large list of entries
 * to define many elements and then remove a few entries that
 * just cannot be used within that element.
 *
 * \todo
 * Actually look into removing these entries from the main
 * entry and not save this extra list!
 *
 * \param[in] entry The exception entries
 */
void moXMLParser::moXMLElement::SetExceptEntry(moXMLEntry *entry)
{
	f_except_entries = entry;
}


/** \brief Set the additional entries.
 *
 * It is at times useful to have additional entries that are
 * not otherwised defined in the main entry.
 *
 * This is what is defined as +(A) and is similar to (A)* where
 * each entry in (A)* is separated by an & (unordered sequence).
 *
 * \todo
 * Actually look into adding those to the main entry.
 *
 * \param[in] entry A list of additional entries
 */
void moXMLParser::moXMLElement::SetAdditionalEntry(moXMLEntry *entry)
{
	f_additional_entries = entry;
}


/** \brief Return the entries.
 *
 * This function returns the main entries of an element.
 *
 * These are expected to be defined as is.
 *
 * Any valid element has a set of entries.
 *
 * \return An entry smart pointer or null
 */
moXMLParser::moXMLElement::moXMLEntrySPtr moXMLParser::moXMLElement::GetEntry(void) const
{
	return f_entries;
}


/** \brief Return the entries that cannot appear in this element.
 *
 * This function returns the entries that are not valid in this
 * element.
 *
 * Most elements do not have any exceptions and thus this function
 * will return a NULL smart pointer in most cases.
 *
 * \return An entry smart pointer or null
 */
moXMLParser::moXMLElement::moXMLEntrySPtr moXMLParser::moXMLElement::GetExceptEntry(void) const
{
	return f_except_entries;
}


/** \brief Return the additional entries.
 *
 * This function returns the entries that can also appear in this element.
 *
 * Most elements do not have any additional list of entries and thus this
 * function will return a NULL smart pointer in most cases.
 *
 * \return An entry smart pointer or null
 */
moXMLParser::moXMLElement::moXMLEntrySPtr moXMLParser::moXMLElement::GetAdditionalEntry(void) const
{
	return f_additional_entries;
}


/** \brief Get the element name.
 *
 * This is the name used to create the element. That's the name of an
 * acceptable tag in your definition.
 *
 * \return The name in a constant string
 */
const moWCString& moXMLParser::moXMLElement::GetName(void) const
{
	return f_name;
}


/** \brief Add an attribute to the element.
 *
 * This function is used to assign additional acceptable attribute to
 * an element.
 *
 * Each attribute definition can appear only once. For this reason,
 * this function returns false if the attribute being added cannot
 * be added because it already exists. It is the caller responsibility
 * to generate an error if necessary.
 *
 * \param[in] attr The attribute to add to the list of attributes
 *
 * \return true if the attribute was added.
 */
bool moXMLParser::moXMLElement::AddAttribute(moXMLAttribute *attr)
{
	return f_attlist.Insert(attr);
}


/** \brief Get the number of attributes defined in this element.
 *
 * This function can be used to check out the number of attributes
 * that are acceptable in this element.
 *
 * \return The number of acceptable attributes in this element.
 */
unsigned long moXMLParser::moXMLElement::AttributeCount(void) const
{
	return f_attlist.Count();
}


/** \brief Get one of the element attributes.
 *
 * This function reads the specified attribute from the element
 * attribute list.
 *
 * The index can be any number from 0 to AttributeCount() - 1.
 *
 * If AttributeCount() returns 0, there are no attributes and
 * this function cannot be called.
 *
 * \note
 * As a side effect, since the attributes are sorted by name,
 * they are returned in alphabetical order.
 *
 * \param[in] idx The attribute index
 *
 * \return An attribute pointer.
 */
moXMLParser::moXMLElement::moXMLAttribute *moXMLParser::moXMLElement::GetAttribute(int idx) const
{
	return f_attlist.Get(idx);
}


/** \brief Add an entry name.
 *
 * This function adds the name of an entry in the list of
 * entry names.
 *
 * Two different entries cannot have the same name. If the
 * same name is added a second time, the function returns
 * false.
 *
 * \param[in] name The name of the new entry.
 *
 * \return true if the name is added.
 */
bool moXMLParser::moXMLElement::AddEntryName(const moWCString& name)
{
	return f_entry_names.Insert(new moWCString(name));
}


/** \brief Count the number of entry names.
 *
 * This function defines the number of names this element supports.
 *
 * \return The number of entry names.
 */
unsigned long moXMLParser::moXMLElement::EntryNameCount(void) const
{
	return f_entry_names.Count();
}


/** \brief Get the specified entry name.
 *
 * This function reads the entry name specified by idx.
 *
 * The idx must be defined between 0 and EntryNameCount() - 1.
 *
 * If EntryNameCount() is zero, then there are no names and this
 * function cannot be called.
 *
 * \note
 * As a side effect, the names are in alphabetical order.
 *
 * \param[in] idx The index of the name to read.
 *
 * \return The name at the specified index.
 */
const moWCString& moXMLParser::moXMLElement::GetEntryName(int idx) const
{
	return *f_entry_names.Get(idx);
}


/** \brief Copy the entries defined in another element.
 *
 * Whenever reading the entries of an <!ELEMENT ...> definition, it
 * could include multiple names. In that case, only the first element
 * receives the entries properly. This function is then used to copy
 * the entries to the other named elements.
 *
 * \param[in] element The source element.
 */
void moXMLParser::moXMLElement::CopyEntriesFrom(const moXMLElement& element)
{
	f_entries            = element.f_entries;
	f_except_entries     = element.f_except_entries;
	f_additional_entries = element.f_additional_entries;
	f_entry_names        = element.f_entry_names;
	f_optional_start     = element.f_optional_start;
	f_optional_end       = element.f_optional_end;
}


/** \brief Set the whether the start and end tags are optional.
 *
 * This function defines whether the start and/or end tags are
 * optional. In general, all start tags are not optional. For
 * instance, the HTML and BODY tags in HTML are optional; and
 * the P tag does not need to be closed.
 *
 * \param[in] start Whether the start tag is optional
 * \param[in] end Whether the end tag is optional
 */
void moXMLParser::moXMLElement::SetOptionalTags(bool start, bool end)
{
	// refuse the impossible combinaison (i.e. no start tag, but an end tag)
	if(!start && end) {
		return;
	}

	f_optional_start = start;
	f_optional_end   = end;
}


/** \brief Check whether the start tag is optional.
 *
 * This function defines whether the start tag for this element
 * is optional or not.
 *
 * \return true if the start tag is optional.
 */
bool moXMLParser::moXMLElement::IsStartTagOptional(void) const
{
	return f_optional_start;
}


/** \brief Check whether the end tag is optional.
 *
 * This function defines whether the end tag for this element
 * is optional or not.
 *
 * \return true if the end tag is optional.
 */
bool moXMLParser::moXMLElement::IsEndTagOptional(void) const
{
	return f_optional_end;
}


/** \brief Check whether two elements have a parent/child relationship.
 *
 * This function searches an element entries to know whether it supports
 * a specified element name as a child tag.
 *
 * Note that this function does not check whether the child is valid
 * where defined since it has no concept of that here (i.e. when the
 * order or repetition matters, etc.)
 *
 * A tag C is considered a valid child of another tag P if C is a
 * valid direct child of P or the child of a child of P that has its
 * start tag defined as optional. And this is recursive (which is
 * important for the HTML, HEAD and BODY tags but not for the TBODY
 * tag that is the only other totally optional tag.)
 *
 * Note that the recursivity is not managed in this IsParentOf()
 * function. Instead, it is defined in the moXMLParser::IsParentOf()
 * function.
 *
 * \param[in] child_name The name of the child element to search for.
 *
 * \return true if the child name is defined as one of the entries.
 */
bool moXMLParser::moXMLElement::IsParentOf(const moWCString& child_name) const
{
	moList::position_t	pos;

	// If it is one of the entries defined in this element, then we're good
	pos = f_entry_names.Find(&child_name);
	if(pos != moList::NO_POSITION) {
		return true;
	}

	// could not find it...
	return false;
}


/** \brief Compare two elements by name.
 *
 * The element comparison function compares two elements by name.
 *
 * In one DTD, any two elements have a distinct name.
 */
moBase::compare_t moXMLParser::moXMLElement::Compare(const moBase& object) const
{
	return f_name.Compare(static_cast<const moXMLParser::moXMLElement&>(object).f_name);
}










/** \class moXMLParser::moXMLEntity
 *
 * \brief A DTD definition of an entity.
 *
 * This object is used to declare DTD or XML entities.
 *
 * Entities have a name, a type (DTD or General) and a value.
 * When an entity is found in the input text, it is replaced
 * by its value.
 *
 * DTD entities are referenced using the \%name; syntax.
 *
 * General entities are referenced using the \&name; syntax.
 */


/** \brief Initializes an moXMLEntity object.
 *
 * This object is initialized with a name and a type. The value
 * of the entity can be set later with the SetValue() function.
 * It should never be changed, but it is possible to do so.
 *
 * \param[in] name The name of this entity.
 * \param[in] type One of the entity types: ENTITY_GENERAL or ENTITY_DTD.
 */
moXMLParser::moXMLEntity::moXMLEntity(const moWCString& name, type_t type)
{
	f_type = type;
	f_name = name;
	//f_value -- auto-init
	//f_public_id -- auto-init
	//f_system_id -- auto-init
}


/** \brief Clean up an moXMLEntity object.
 *
 * This function cleans up an moXMLEntity and returns.
 */
moXMLParser::moXMLEntity::~moXMLEntity()
{
}


/** \brief Set the entity value.
 *
 * This function sets the value of an entity. The value represents the
 * text that will be used whenever the entity is encountered somewhere
 * else in the input files.
 *
 * \param[in] value Value representing this entity.
 */
void moXMLParser::moXMLEntity::SetValue(const moWCString& value)
{
	f_value = value;
}


/** \brief Set the public identifier of the entity.
 *
 * This function sets the new public identifier of an entity.
 *
 * \param[in] public_id The new public identifier.
 */
void moXMLParser::moXMLEntity::SetPublicID(const moWCString& public_id)
{
	f_public_id = public_id;
}


/** \brief Set the system identifier of the entity.
 *
 * This function sets the new system identifier of an entity.
 *
 * \param[in] system_id The new system identifier.
 */
void moXMLParser::moXMLEntity::SetSystemID(const moWCString& system_id)
{
	f_system_id = system_id;
}


/** \brief Get the entity type.
 *
 * This function returns the type of the entity.
 *
 * A general entity is referenced using &&lt;name>;.
 *
 * A DTD entity is referenced using %&lt;name>;.
 *
 * \return One of ENTITY_GENERAL or ENTITY_DTD
 */
moXMLParser::moXMLEntity::type_t moXMLParser::moXMLEntity::GetType(void) const
{
	return f_type;
}


/** \brief Get the name of the entity.
 *
 * This function returns the name of the entity.
 *
 * \return A string with the entity name.
 */
const moWCString& moXMLParser::moXMLEntity::GetName(void) const
{
	return f_name;
}


/** \brief Get the entity value.
 *
 * This function returns the value of an entity. This is 
 * saved in a string.
 *
 * \return A string with the entity value.
 */
const moWCString& moXMLParser::moXMLEntity::GetValue(void) const
{
	return f_value;
}


/** \brief Compare two entities by name.
 *
 * This function compares two entities by their name.
 */
moBase::compare_t moXMLParser::moXMLEntity::Compare(const moBase& object) const
{
	return f_name.Compare(static_cast<const moXMLParser::moXMLEntity&>(object).f_name);
}


/** \brief Get the public identifier of an entity.
 *
 * \return The identifier in a string.
 */
const moWCString& moXMLParser::moXMLEntity::GetPublicID(void) const
{
	return f_public_id;
}


/** \brief Get the system identifier of an entity.
 *
 * \return The identifier in a string.
 */
const moWCString& moXMLParser::moXMLEntity::GetSystemID(void) const
{
	return f_system_id;
}






};			// namespace molib;

// vim: ts=8

