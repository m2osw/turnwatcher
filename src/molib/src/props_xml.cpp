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
#pragma implementation "mo/mo_props_xml.h"
#endif

#include	"mo/mo_props_xml.h"

#ifndef MO_UUENCODE_H
#include	"mo/mo_uuencode.h"
#endif
#ifndef MO_FILE_H
#include	"mo/mo_file.h"
#endif


namespace molib
{


/************************************************************ DOC:

CLASS

	moPropIO_XML

NAME

	Contructor - initialize the moPropIO_XML object
	moGetClassName - get the name of this class as a string

SYNOPSIS

	moPropIO_XML(void);
	virtual const char *moGetClassName(void) const;

DESCRIPTION

	The constructor initialize an moPropIO_XML.

	It will force the binary mode to UUENCODE by default. You
	can change this by calling the BinaryMode() function.

	The moGetClassName() function returns the name of this class.

RETURN VALUE

	The moGetClassName() function returns a string with the
	name of this class.

SEE ALSO

	InternalLoad, InternalSave, BinaryMode

*/
moPropIO_XML::moPropIO_XML(void)
{
	BinaryMode(MO_XML_BINARY_MODE_UUENCODE);
	if(f_convertor.SetEncodings(mowc::g_ucs4_internal, "UTF-8") != 0) {
		fprintf(stderr, "error: moPropIO_XML::moPropIO_XML(): SetEncodings() failed\n");
	}
}

const char *moPropIO_XML::moGetClassName(void) const
{
	return "molib::moPropIO::moPropIO_XML";
}



/************************************************************ DOC:

CLASS

	moPropIO_XML

NAME

	BinaryMode - set the mode to use to save binary data

SYNOPSIS

	void BinaryMode(binary_mode_t mode);

PARAMETERS

	mode - the binary mode to be used

		MO_XML_BINARY_MODE_UUENCODE
		MO_XML_BINARY_MODE_HEX
		MO_XML_BINARY_MODE_COMPACT

DESCRIPTION

	This function sets the mode to be used to save the binary
	data. This is saved in the binary tags as mode="<name>".

	The valid modes are "uuencode" (compatible with the tool
	of the same name), "hex" (pairs of hexadecimal digits for
	each byte), "compact" (using characters 21 to 255 except
	38, 60, 62 and 127 to 160 inclusive).

BUGS

	The compact mode is not implemented yet. uuencoding will
	be used instead for now.

SEE ALSO

	InternalLoad, InternalSave

*/
void moPropIO_XML::BinaryMode(binary_mode_t mode)
{
	f_binary_mode = mode;

	// NOTE:
	// Though we are likely not to have any binary buffer in an
	// moPropBag we define the name of the mode here since this
	// way it's done only once
	switch(f_binary_mode) {
	case MO_XML_BINARY_MODE_HEX:
		f_binary_mode_name = "hex";
		break;

	default:	// case MO_XML_BINARY_MODE_UUENCODE:
		// we only support base 64 uuencoding, no others
		// (more or less, UTF-7)
		f_binary_mode_name = "base64";
		f_binary_mode = MO_XML_BINARY_MODE_UUENCODE;
		break;

	}
}



/************************************************************ DOC:

CLASS

	moPropIO_XML

NAME

	private:
	InternalLoad - loads an XML file and put it in a property set

SYNOPSIS

	virtual int InternalLoad(moPropBagRef& prop_bag);

PARAMETERS

	prop_bag - the bag of properties to fill with input data

DESCRIPTION

	The InternalLoad() function of the XML version of the
	property bag I/O will load an XML file and transform it
	in properties saved in the bag.

	The DTD is described below in a string that the InternalSave()
	defines. The format namespace is "propbag".

SEE ALSO

	InternalSave

*/
int moPropIO_XML::InternalLoad(moPropBagRef& prop_bag)
{
	// TODO: we most certainly want to have a log file for
	//	 errors which may be printed by the XML parser
	moXMLParser::moXMLStream input_stream(f_input);

	moXMLParser parser(input_stream);

	moXMLParser::moXMLTypeSPtr ln;
	for(;;) {
		if(!parser.ReadNext(ln)) {
			// empty or invalid format (i.e. not a propbag XML file)
			SetError(MO_ERROR_EMPTY);
			return -1;
		}
		if(ln->GetType() == moXMLParser::XML_TYPE_TAG_START) {
			if(ln->GetTagName() == "propbag") {
				break;
			}
		}
	}

	// We can't change the name of a property so the user
	// prop_bag's name will remain unchanged. Period.

	int r = LoadBag(parser, prop_bag);

	return r;
}

enum what_t {
	WHAT_WHAT = 0,		// what's that?!
	WHAT_STRING,
	WHAT_BINARY,
	WHAT_ARRAY
};

int moPropIO_XML::LoadBag(moXMLParser& parser, moPropBagRef& prop_bag)
{
	what_t				what;
	moWCString			name, value, item_no, mode, filename;
	int				r, filemode;
	mo_uudecode_error_t		err;
	moPropArrayRef			array("Array");
	moList				array_stack;
	moXMLParser::moXMLTypeSPtr	ln;

	what = WHAT_WHAT;
	while(parser.ReadNext(ln)) {
		switch(ln->GetType()) {
		case moXMLParser::XML_TYPE_TAG_START:
		{
			const moXMLParser::moXMLTag *tag = dynamic_cast<const moXMLParser::moXMLTag *>(static_cast<const moXMLParser::moXMLType *>(ln));
			const moWCString& tag_name = ln->GetTagName();
			moVariableSPtr attr;

			attr = tag->Get("name");
			if(!attr) {
				SetError(MO_ERROR_INVALID);
				name = "*** missing name ***";
			}
			else {
				name = attr;
			}
			attr = tag->Get("value");
			if(!attr) {
				value.Empty();
			}
			else {
				value = attr;
			}
			if(!array.IsNull()) {
				attr = tag->Get("item");
				if(!attr) {
					SetError(MO_ERROR_INVALID);
					item_no = "0";
				}
				else {
					item_no = attr;
				}
			}
			if(tag_name == "propbag"
			|| tag_name == "propbag_item") {
				//name = tag->Get("name");
				moPropBagRef sub_bag(name);
				sub_bag.NewProp();
				r = LoadBag(parser, sub_bag);
				if(r != 0) {
					return -1;
				}
				if(array.IsNull()) {
					prop_bag.Set(name, sub_bag);
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), sub_bag);
				}
			}
			else if(tag_name == "int"
			     || tag_name == "int_item") {
				//name = tag->Get("name");
				//value = tag->Get("value");
				moPropIntRef p_int(name);
				p_int.NewProp();
				p_int = value.Integer();
				if(array.IsNull()) {
					prop_bag += p_int;
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), p_int);
				}
			}
			else if(tag_name == "longlong"
			     || tag_name == "longlong_item") {
				//name = tag->Get("name");
				//value = tag->Get("value");
				moPropLongLongRef p_ll(name);
				p_ll.NewProp();
				p_ll = value.LargeInteger();
				if(array.IsNull()) {
					prop_bag += p_ll;
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), p_ll);
				}
			}
			else if(tag_name == "float"
			     || tag_name == "float_item") {
				//name = tag->Get("name");
				//value = tag->Get("value");
				moPropFloatRef p_float(name);
				p_float.NewProp();
				p_float = value.Float();
				if(array.IsNull()) {
					prop_bag += p_float;
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), p_float);
				}
			}
			else if(tag_name == "double"
			     || tag_name == "double_item") {
				//name = tag->Get("name");
				//value = tag->Get("value");
				moPropDoubleRef p_double(name);
				p_double.NewProp();
				p_double = value.Float();
				if(array.IsNull()) {
					prop_bag += p_double;
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), p_double);
				}
			}
			else if(tag_name == "pointer"
			     || tag_name == "pointer_item") {
				//name = tag->Get("name");
				//value = tag->Get("value");
				moPropPointerRef p_pointer(name);
				p_pointer.NewProp();
				p_pointer = reinterpret_cast<moBase *>(value.LargeInteger());
				if(array.IsNull()) {
					prop_bag += p_pointer;
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), p_pointer);
				}
			}
			else if(tag_name == "string"
			     || tag_name == "string_item") {
				what = WHAT_STRING;
				//name = tag->Get("name");
				//if(!array.IsNull()) {
					//item_no = tag->Get("item");
				//}
			}
			else if(tag_name == "binary"
			     || tag_name == "binary_item") {
				what = WHAT_BINARY;
				//name = tag->Get("name");
				attr = tag->Get("mode");
				if(attr) {
					mode = *attr;
				}
				else {
					mode = "?";
				}
				//if(!array.IsNull()) {
					//item_no = tag->Get("item");
				//}
			}
			else if(tag_name == "external"
			     || tag_name == "external_item") {
#ifdef MO_DEBUG
				// not supported yet
				throw moError("PropBag external not supported yet");
#endif
			}
			else if(tag_name == "array"
			     || tag_name == "array_item") {
				moProp::prop_type_t type;
				//name = tag->Get("name");
				attr = tag->Get("type");
				if(!attr) {
					type = moProp::MO_PROP_TYPE_UNKNOWN;
				}
				else if(attr->CaseCompare("propbag") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_PROP_BAG;
				}
				else if(attr->CaseCompare("int") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_INT;
				}
				else if(attr->CaseCompare("longlong") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_LONG_LONG;
				}
				else if(attr->CaseCompare("float") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_FLOAT;
				}
				else if(attr->CaseCompare("double") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_DOUBLE;
				}
				else if(attr->CaseCompare("pointer") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_POINTER;
				}
				else if(attr->CaseCompare("string") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_STRING;
				}
				else if(attr->CaseCompare("binary") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_BINARY;
				}
				else if(attr->CaseCompare("array") == MO_BASE_COMPARE_EQUAL) {
					type = moProp::MO_PROP_TYPE_ARRAY;
				}
				else /*if(attr->CaseCompare("any") == MO_BASE_COMPARE_EQUAL)*/ {
					type = moProp::MO_PROP_TYPE_UNKNOWN;
				}

				moPropArrayRef p_array(name);
				p_array.NewProp(type);
				if(array.IsNull()) {
					prop_bag += p_array;
					// the += creates a copy so we need
					// to get the new pointer
					p_array = prop_bag.Get(name);
				}
				else {
					//item_no = tag->Get("item");
					array.Set(item_no.Integer(), p_array);
					array_stack += *array.GetProperty();
				}
				array = p_array;
			}
			// else we ignore for now...
			// (that's forward compatible, right?)
		}
			break;

		case moXMLParser::XML_TYPE_DATA:
			switch(what) {
			case WHAT_STRING:
			{
				moPropStringRef p_string(name);
				p_string.NewProp();
				p_string = dynamic_cast<const moXMLParser::moXMLData *>(static_cast<const moXMLParser::moXMLType *>(ln))->GetData();
				if(array.IsNull()) {
					prop_bag += p_string;
				}
				else {
					array.Set(item_no.Integer(), p_string);
				}
			}
				break;

			case WHAT_BINARY:
			{
				moPropBinaryRef p_binary(name);
				p_binary.NewProp();
				err = moUUDecode(dynamic_cast<const moXMLParser::moXMLData *>(static_cast<const moXMLParser::moXMLType *>(ln))->GetData(),
						const_cast<moBuffer&>(p_binary.Get()), filemode, filename);
 				if(err != 0) {
					SetError(MO_ERROR_INVALID);
					return -1;
				}
				if(array.IsNull()) {
					prop_bag += p_binary;
				}
				else {
					array.Set(item_no.Integer(), p_binary);
				}
			}
				break;

			// we ignore WHAT_WHAT for now
			default:
				break;

			}
			break;

		case moXMLParser::XML_TYPE_TAG_END:
		{
			const moWCString& tag_name = ln->GetTagName();
			if(tag_name == "propbag"
			|| tag_name == "propbag_item") {
				return 0;
			}
			if(tag_name == "array"
			|| tag_name == "array_item") {
				if(array_stack.IsEmpty()) {
					array.ClearRef();
				}
				else {
					// NOTE: we need an intermediate reference to avoid a copy
					moPropArrayRef ref(*dynamic_cast<moPropArray *>(array_stack.GetLast()));
					array = ref;
					array_stack.Delete(array_stack.Count() - 1);
				}
			}
			what = WHAT_WHAT;
		}
			break;

		// avoid warnings
		default:
			break;

		}
	}

	SetError(MO_ERROR_END_NOT_EXPECTED);

	return -1;
}




/************************************************************ DOC:

CLASS

	moPropXML_IO

NAME

	InternalSave - save a property bag to an XML file

SYNOPSIS

	private:
	virtual int InternalSave(const moPropBagRef& prop_bag);

	local:
	int save_info_t::SaveBag(const moPropBagRef& prop_bag);
	int save_info_t::SaveProp(const moPropRef& p);

PARAMETERS

	prop_bag - the property bag to save in an XML file
	out - the output stream
	indent - the current indentation level (# of spaces to write)
	p - the property to save in the output stream

DESCRIPTION

	The InternalSave() function writes all the data of a
	property bag to an XML file. It uses the SaveBag()
	function to save bags of properties and SaveProp()
	to save each individual property.

	The SaveProp() will be called recursively to save all
	the items of an array.

	The SaveBag() is called recursively by the SaveProp()
	function whenever it needs to save a property bag.

NOTES

	At this time, there is no safe guard agains looping
	property sets (i.e. a counter to know how many times
	you have called SaveProp() recursively). It may be
	added at a later time.

RETURN VALUE

	This functions will return 0 when nothing goes wrong.
	Otherwise they return -1. The error code can be retrieved
	using the GetLastError() function.

BUGS

	While this function is running the stream filter will be
	replaced with an internal filter to force the text to be
	written in UTF-8 in the output file (which is what the DTD
	defines). The original filter will be restore before the
	function returns, however, the filter in effect when
	calling the function will be ignored.

SEE ALSO

	InternalLoad

*/
const char propbag_dtd[] =
	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n"
	"<!DOCTYPE propbag [\n"
	"	<!ELEMENT int EMPTY>\n"
	"	<!ATTLIST int"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED>\n"
	"	<!ELEMENT longlong EMPTY>\n"
	"	<!ATTLIST longlong"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED>\n"
	"	<!ELEMENT float EMPTY>\n"
	"	<!ATTLIST float"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED>\n"
	"	<!ELEMENT double EMPTY>\n"
	"	<!ATTLIST double"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED>\n"
	"	<!ELEMENT pointer EMPTY>\n"
	"	<!ATTLIST pointer"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED>\n"
	"	<!ELEMENT string (#PCDATA)>\n"
	"	<!ATTLIST string"
			" name CDATA #REQUIRED"
			" xml:space (preserve) 'preserve'>\n"
	"	<!ELEMENT binary (#PCDATA)>\n"
	"	<!ATTLIST binary"
			" name CDATA #REQUIRED"
			" mode (hex|base64) #REQUIRED>\n"
	"	<!ELEMENT external EMPTY>\n"
	"	<!ATTLIST external"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED>\n"

	"	<!ELEMENT int_item EMPTY>\n"
	"	<!ATTLIST int_item"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT longlong_item EMPTY>\n"
	"	<!ATTLIST longlong_item"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT float_item EMPTY>\n"
	"	<!ATTLIST float_item"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT double_item EMPTY>\n"
	"	<!ATTLIST double_item"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT pointer_item EMPTY>\n"
	"	<!ATTLIST pointer_item"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT string_item (#PCDATA)>\n"
	"	<!ATTLIST string_item"
			" name CDATA #REQUIRED"
			" xml:space (preserve) 'preserve'"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT binary_item (#PCDATA)>\n"
	"	<!ATTLIST binary_item"
			" name CDATA #REQUIRED"
			" mode (hex|base64) #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT external_item EMPTY>\n"
	"	<!ATTLIST external_item"
			" name CDATA #REQUIRED"
			" value CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"
	"	<!ELEMENT array_item (array_item|int_item|longlong_item"
				"|float_item|double_item|pointer_item"
				"|string_item|binary_item|external_item"
				"|propbag_item)*>\n"
	"	<!ATTLIST array_item"
			" name CDATA #REQUIRED"
			" item CDATA #REQUIRED"
			" type (any|array|int|longlong|float|double|pointer"
				"|string|binary|propbag) \"any\">\n"
	"	<!ELEMENT propbag_item (array|int|longlong|float|double"
				"|pointer|string|binary|external|propbag)*>\n"
	"	<!ATTLIST propbag_item"
			" name CDATA #REQUIRED"
			" item CDATA #REQUIRED>\n"

	"	<!ELEMENT array (array_item|int_item|longlong_item"
				"|float_item|double_item|pointer_item"
				"|string_item|binary_item|external_item"
				"|propbag_item)*>\n"
	"	<!ATTLIST array"
			" name CDATA #REQUIRED"
			" type (any|array|int|longlong|float|double|pointer"
				"|string|binary|propbag) \"any\">\n"
	"	<!ELEMENT propbag (array|int|longlong|float|double"
				"|pointer|string|binary|external|propbag)*>\n"
	"	<!ATTLIST propbag"
			" name CDATA #REQUIRED>\n"

	"]>\n";


class save_info_t
{
public:
	save_info_t(const moPropBagRef& prop_bag, moOStreamSPtr output,
				moPropIO_XML::binary_mode_t binary_mode,
				const moWCString& binary_mode_name,
				bool save_pointers)
		: f_prop_bag(prop_bag),
		  f_np(&moNamePool::GetNamePool()),
		  f_out(0, output),
		  f_binary_mode(binary_mode),
		  f_binary_mode_name(binary_mode_name),
		  f_save_pointers(save_pointers)
	{
		f_out.Print("%s", propbag_dtd);
		f_out.Print("<propbag name=\"%S\">\n", f_np->Get(f_prop_bag.GetName()).Data());
	}

	~save_info_t()
	{
		f_out.Print("</propbag>\n");
	}

	int SaveBag(const moPropBagRef& prop_bag);
	int SaveProp(const moPropRef& p);

	const moPropBagRef	f_prop_bag;
	const moNamePoolSPtr	f_np;
	moTextStream		f_out;
	zint32_t		f_indent;
	zbool_t			f_array_item;
	int			f_item;	// not used until f_array_item is true
	moPropIO_XML::binary_mode_t f_binary_mode;
	const moWCString&	f_binary_mode_name;
	zbool_t			f_save_pointers;
};



int save_info_t::SaveBag(const moPropBagRef& prop_bag)
{
	zint32_t	rc;
	int		r, idx, max;

	f_indent += 2;

	max = prop_bag.Count();
	for(idx = 0; idx < max; ++idx) {
		r = SaveProp(prop_bag.Get(idx));
		if(r != 0) {
			rc = -1;
		}
	}

	f_indent -= 2;

	return rc;
}


int save_info_t::SaveProp(const moPropRef& p)
{
	// make the name a valid XML name
	// (actually, the Replace() needs a lot of work to really be
	// 100% compatible -- we may want to have a function in the
	// moXML class to do that work)
	moWCString name = f_np->Get(p.GetName()).Replace("<=&lt\\;;>=&gt\\;;&=&amp\\;");

	moWCString item, closing;
	if(f_array_item) {
		item = moWCString::Format("_item item=\"%d\"", f_item);
		closing = "_item";
	}

	f_out.Repeat(f_indent, " ");
	switch(p.GetType()) {
	case moProp::MO_PROP_TYPE_PROP_BAG:
	{
		f_out.Print("<propbag%S name=\"%S\">\n",
				item.Data(), name.Data());
		const moPropBagRef sub_prop(p);
		if(sub_prop) {
			// we are not in an array while saving
			// a sub-property bag
			bool has_array_item = f_array_item;
			f_array_item = false;
			SaveBag(sub_prop);
			f_array_item = has_array_item;
		}
		f_out.Repeat(f_indent, " ");
		f_out.Print("</propbag%S>\n", closing.Data());
	}
		break;

	case moProp::MO_PROP_TYPE_INT:
	{
		moPropIntRef p_int(p);
		f_out.Print("<int%S name=\"%S\" value=\"%d\"/>\n",
				item.Data(), name.Data(),
				static_cast<int>(p_int));
	}
		break;

	case moProp::MO_PROP_TYPE_LONG_LONG:
	{
		moPropLongLongRef p_ll(p);
		f_out.Print("<longlong%S name=\"%S\" value=\"%lld\"/>\n",
				item.Data(), name.Data(),
				static_cast<int64_t>(p_ll));
	}
		break;

	case moProp::MO_PROP_TYPE_FLOAT:
	{
		moPropFloatRef p_float(p);
		f_out.Print("<float%S name=\"%S\" value=\"%e\"/>\n",
				item.Data(), name.Data(),
				static_cast<float>(p_float));
	}
		break;

	case moProp::MO_PROP_TYPE_DOUBLE:
	{
		moPropDoubleRef p_double(p);
		f_out.Print("<double%S name=\"%S\" value=\"%e\"/>\n",
				item.Data(), name.Data(),
				static_cast<double>(p_double));
	}
		break;

	case moProp::MO_PROP_TYPE_POINTER:
		if(f_save_pointers) {
			moPropPointerRef p_pointer(p);
			f_out.Print("<pointer%S name=\"%S\" value=\"%p\"/>\n",
					item.Data(), name.Data(),
					static_cast<moBase *>(static_cast<moBaseSPtr>(p_pointer)));
		}
		break;

	case moProp::MO_PROP_TYPE_STRING:
	{
		const moPropStringRef p_string(p);
		f_out.Print("<string%S name=\"%S\">%S</string%S>\n",
			item.Data(), name.Data(),
			static_cast<const moWCString&>(p_string).Replace("<=&lt\\;;>=&gt\\;;&=&amp\\;").Data(),
			closing.Data());
	}
		break;

	case moProp::MO_PROP_TYPE_BINARY:
	{
		const moPropBinaryRef p_binary(p);
		f_out.Print("<binary%S name=\"%S\" mode=\"%S\">",
				item.Data(), name.Data(),
				f_binary_mode_name.Data());
		void *buffer;
		unsigned long size;
		static_cast<const moBuffer&>(p_binary).Get(buffer, size);
		if(f_binary_mode == moPropIO_XML::MO_XML_BINARY_MODE_HEX) {
			unsigned char *data = static_cast<unsigned char *>(buffer);
			if(size > 16) {
				while(size > 0) {
					f_out.Print("\n ");
					f_out.Repeat(f_indent, " ");
					unsigned long idx;
					for(idx = 0; idx < 16 && size > 0; ++idx, --size, ++data) {
						f_out.Print("%02X", *data);
					}
				}
				f_out.Print("\n");
				f_out.Repeat(f_indent, " ");
			}
			else {
				while(size > 0) {
					--size;
					f_out.Print("%02X", *data);
					data++;
				}
			}
		}
		//else if(f_binary_mode == moPropIO_XML::MO_XML_BINARY_MODE_COMPACT) {
		//	NOT IMPLEMENTED YET
		//}
		else /* if(f_binary_mode == moPropIO_XML::MO_XML_BINARY_MODE_UUENCODE)*/ {
			// NOTE: UUENCODE is the default mode
			moWCString uuencode;
			moUUEncode(uuencode, p_binary, 0600, name);
			// uuencode data can include the <, > and & characters
			f_out.Print("\n%S", uuencode.Replace("<=&lt\\;;>=&gt\\;;&=&amp\\;").Data());
		}
		f_out.Repeat(f_indent, " ");
		f_out.Print("</binary%S>\n", closing.Data());
	}
		break;

	case moProp::MO_PROP_TYPE_ARRAY:
	{
		bool save_array_item = f_array_item;
		int save_item = f_item;
		moPropArrayRef array(p);
		const char *type;

		switch(array.GetElementsType()) {
		case moProp::MO_PROP_TYPE_UNKNOWN:
			type = "";
			break;

		case moProp::MO_PROP_TYPE_PROP_BAG:
			type = " type=\"propbag\"";
			break;

		case moProp::MO_PROP_TYPE_INT:
			type = " type=\"int\"";
			break;

		case moProp::MO_PROP_TYPE_LONG_LONG:
			type = " type=\"longlong\"";
			break;

		case moProp::MO_PROP_TYPE_FLOAT:
			type = " type=\"float\"";
			break;

		case moProp::MO_PROP_TYPE_DOUBLE:
			type = " type=\"double\"";
			break;

		case moProp::MO_PROP_TYPE_POINTER:
			type = " type=\"pointer\"";
			break;

		case moProp::MO_PROP_TYPE_STRING:
			type = " type=\"string\"";
			break;

		case moProp::MO_PROP_TYPE_BINARY:
			type = " type=\"binary\"";
			break;

		case moProp::MO_PROP_TYPE_ARRAY:
			type = " type=\"array\"";
			break;

		// invalid types:
		default:
		case moProp::MO_PROP_TYPE_max:
			throw moError("invalid type used for elements in an array");

		}
		f_out.Print("<array%S name=\"%S\"%s>\n",
				item.Data(), name.Data(), type);

		f_indent += 2;
		f_array_item = true;
		int max = array.CountIndexes();
		for(int idx = 0; idx < max; ++idx) {
			f_item = array.ItemNoAtIndex(idx);
			moPropRef array_prop(0, array.GetAtIndex(idx));
			SaveProp(array_prop);
		}
		f_indent -= 2;

		f_out.Repeat(f_indent, " ");
		f_out.Print("</array%S>\n", closing.Data());

		f_array_item = save_array_item;
		f_item = save_item;
	}
		break;

	default:	// avoid warnings & inform user of missing type
		f_out.Print("<!-- skipping unknown property type %d -->\n", p.GetType());
		break;

	}

	return 0;
}



int moPropIO_XML::InternalSave(const moPropBagRef& prop_bag)
{
	// in case we use the same moPropIO_XML multiple times,
	// we need to reset the convertor FIFO in case something
	// went wrong the last time it was used.
	f_convertor.Reset();
	moFIFOSPtr old_filter = f_output->SetOutputFilter(&f_convertor);

	int r;
	{
		save_info_t info(prop_bag, f_output,
				f_binary_mode, f_binary_mode_name,
				f_save_pointers);

		r = info.SaveBag(prop_bag);
	}

	// restore the user filter
	f_output->SetOutputFilter(old_filter);

	return r;
}







/************************************************************ DOC:

CLASS

	moPropIO_XML

NAME

	Helper functions:

	moXMLLoadPropBag - loads an XML file and put it in a property bag
	moXMLSavePropBag - saves a property bag to an XML file

SYNOPSIS

	extern int moXMLLoadPropBag(const moWCString& filename, moPropBagRef& prop_bag);
	extern int moXMLSavePropBag(const moWCString& filename, moPropBagRef& prop_bag,
				binary_mode_t binary_mode = MO_XML_BINARY_MODE_UUENCODE);

PARAMETERS

	filename - the name of the XML file to use to load/save the property bag
	prop_bag - the property bag used to load or save
	binary_mode - the mode to use to save the binary content

DESCRIPTION

	The moXMLLoadPropBag() loads a property bag from an XML file which name
	you specify in the call. This function can be used to avoid having to
	yourself create an moFile, attach it to an XML file and then call the
	Load() function on the property bag I/O handler. The XML file is expected
	to be a disk file at this time (until the moFile supports other protocols).

	The moXMLSavePropBag() saves a property bag to an XML file which name
	you specify in the call. This function can be used to avoid having to
	yourself create an moFile, attach it to an XML file and then call the
	Save() function on the property bag I/O handler. The XML file is expected
	to end-up being a disk file at this time.

RETURN VALUE

	All of these functions return 0 if no error occured while loading/saving
	the property bag. They return -1 if an error occurs.

BUGS

	At this time you don't have access to the error code if the functions
	fail. This may be fixed at a later date by adding a parameter to these
	function calls.

SEE ALSO

	moPropIO, moPropIO_XML

*/
int moXMLLoadPropBag(const moWCString& filename, moPropBagRef& prop_bag)
{
	// create the input file
	moFile input;

	if(!input.Open(filename)) {
		return -1;
	}

	// create the prop I/O and attach the input file
	moPropIO_XML prop_io_xml;
	prop_io_xml.SetInput(&input);

	// load the XML file
	return prop_io_xml.Load(prop_bag);
}


int moXMLSavePropBag(const moWCString& filename, moPropBagRef& prop_bag, moPropIO_XML::binary_mode_t binary_mode)
{
	// create the output file
	moFile output;

	if(!output.Open(filename, moFile::MO_FILE_MODE_WRITE | moFile::MO_FILE_MODE_CREATE)) {
		return -1;
	}

	// create the prop I/O and attach the output file
	moPropIO_XML prop_io_xml;
	prop_io_xml.SetOutput(&output);
	prop_io_xml.BinaryMode(binary_mode);

	return prop_io_xml.Save(prop_bag);
}


// vim: ts=8
}		// namespace molib

