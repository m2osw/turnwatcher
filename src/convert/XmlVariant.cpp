//===============================================================================
// Copyright:	Copyright (c) 2005-2010 Made to Order Software Corp.
//
//		All Rights Reserved.
//
//		This software and its associated documentation contains
//		proprietary, confidential and trade secret information
//		of Made to Order Software, Corp. and except as provided by
//		written agreement with Made to Order Software, Corp.
//
//		a) no part may be disclosed, distributed, reproduced,
//		   transmitted, transcribed, stored in a retrieval system,
//		   adapted or translated in any form or by any means
//		   electronic, mechanical, magnetic, optical, chemical,
//		   manual or otherwise,
//
//		and
//
//		b) the recipient is not entitled to discover through reverse
//		   engineering or reverse compiling or other such techniques
//		   or processes the trade secrets contained therein or in the
//		   documentation.
//
//===============================================================================

// Stl
//
#include <exception>
#include <iostream>

// Qt4
//
#include <QFile>

// Local
//
#include "LibCommon.h"
#include "QStringUtils.h"
#include "XmlVariant.h"

/*! \class XmlVariant
 *
 * This class derives from the Qt4 QVariant class, allowing one to embed a map of
 * QString to XmlVariant objects. On construction, pass in a pointer to a QXmlStreamReader
 * which contains an XML description of a moPropBag output. This is useful for importing
 * a Turnwatcher settings file.
 */


XmlVariant::XmlVariant( const QString& fileName, const QString& section_name )
	: f_reader( new QXmlStreamReader )
{
	f_sectionId = section_name;
	//
	QFile file( fileName );
	if( !file.open( QFile::ReadOnly | QFile::Text ) )
	{
		throw XmlException( QString("Cannot open \"%1\" for reading!").arg(fileName) );
	}
	//
	f_reader->setDevice( &file );
	//
	ReadSection();
}


XmlVariant::XmlVariant( ReaderPtr reader, const QString& section_name )
	: f_reader( reader )
{
	f_sectionId = section_name;
	//
	ReadSection();
}


/*! \brief Read a section from the XML stream and update the internal map.
 */
void XmlVariant::ReadSection()
{
	bool    at_end = false;		// Keep going until we hit the close.
	QString text_name;			// Has to be here since the text_name gets set on a "node" above.

	while( !f_reader->atEnd() && !at_end )
	{
		QXmlStreamReader::TokenType tt = f_reader->readNext();
		switch( tt )
		{
			case QXmlStreamReader::StartElement:
				{
					QString attr_name = f_reader->attributes().value("name").toString();
					//
					if( f_reader->name() == "int" )
					{
						f_map[attr_name] = f_reader->attributes().value("value").toString();
					}
					//
					if( f_reader->name() == "string" )
					{
						text_name = attr_name;
					}
					//
					if( f_reader->name() == "array" )
					{
						f_map[attr_name] = XmlVariant( f_reader, "array" );
					}
					//
					if( f_reader->name() == "propbag" )
					{
						f_map[attr_name] = XmlVariant( f_reader, "propbag" );
					}
					//
					if( f_reader->name() == "propbag_item" )
					{
						const QString item_num		 = f_reader->attributes().value("item").toString();
						const QString item_attr_name = QString("%1_%2").arg(attr_name).arg(item_num, 3, '0');
						LogTrace( QString("item_attr_name = '%1'").arg(item_attr_name) );
						f_map[item_attr_name] = XmlVariant( f_reader, "propbag_item" );
					}
				}
				break;

			case QXmlStreamReader::Characters:
				if( !text_name.isEmpty() )
				{
					f_map[text_name] = f_reader->text().toString();
					text_name.clear();
				}
				break;

			case QXmlStreamReader::EndElement:
				if( f_reader->name() == f_sectionId )
				{
					at_end = true;
				}
				break;

			default:
				break;
		}
	}
}

// vim: ts=4 sw=4 syntax=cpp.doxygen

