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



#ifndef MO_PROPS_XML_H
#define	MO_PROPS_XML_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_PROPS_H
#include	"mo_props.h"
#endif
#ifndef MO_XML_H
#include	"mo_xml.h"
#endif


namespace molib
{




class MO_DLL_EXPORT moPropIO_XML : public moPropIO
{
public:
	enum binary_mode_t {
		MO_XML_BINARY_MODE_UUENCODE = 0,	// this is the default (better compressed compatible with uuencode(1))
		MO_XML_BINARY_MODE_HEX,			// 2 hex digits per character (humain readable, kinda)
		MO_XML_BINARY_MODE_COMPACT,		// our own compression mode using all acceptable 8 bits XML characters (best compression, not compatible with anything)
		MO_XML_BINARY_MODE_max
	};

				moPropIO_XML(void);

	void			BinaryMode(binary_mode_t mode);

private:
	virtual const char *	moGetClassName(void) const;

	virtual int		InternalLoad(moPropBagRef& prop_bag);
	virtual int		InternalSave(const moPropBagRef& prop_bag);

	int			LoadBag(moXMLParser& parser, moPropBagRef& prop_bag);

	binary_mode_t		f_binary_mode;
	const char *		f_binary_mode_name;
	mowc::moIConv		f_convertor;	// output file need to be UTF-8
};


// helper functions to load & save property bags in XML
MO_DLL_EXPORT_FUNC extern	int	moXMLLoadPropBag(const moWCString& filename, moPropBagRef& prop_bag);
MO_DLL_EXPORT_FUNC extern	int	moXMLSavePropBag(const moWCString& filename, moPropBagRef& prop_bag, moPropIO_XML::binary_mode_t binary_mode = moPropIO_XML::MO_XML_BINARY_MODE_UUENCODE);



};			// namespace molib

// vim: ts=8 sw=8
#endif		// #ifndef MO_PROPS_XML_H

