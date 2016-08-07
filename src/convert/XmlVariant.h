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
#pragma once

//===============================================================================
/// \brief Main Function
//===============================================================================

#include "Variant.h"

// Qt4
//
#include <QXmlStreamReader>
#include <QSharedPointer>

class XmlException : public std::exception
{
	public:
		XmlException( const QString& msg ) throw() : f_msg(msg) {}
		virtual ~XmlException() throw() {}

		QString GetMessage() const { return f_msg; }

		const char* what() const throw()
		{
			return f_msg.toAscii().data();
		}

	private:
		QString	f_msg;
};


class XmlVariant : public Variant
{
	public:
		typedef QSharedPointer<QXmlStreamReader> ReaderPtr;

		XmlVariant() {}
		//
		XmlVariant( const bool      val ) : Variant( val ) {}
		XmlVariant( const int       val ) : Variant( val ) {}
		XmlVariant( const QString&  val ) : Variant( val ) {}
		XmlVariant( const Variant&  val ) : Variant( val ) {}
		XmlVariant( const QVariant& val ) : Variant( val ) {}
		//
		XmlVariant( const QString& fileName, const QString& section_name );

	private:
		ReaderPtr			f_reader;

		XmlVariant( ReaderPtr reader, const QString& section_name );
		//
		void 				ReadSection();
};

// vim: ts=4 sw=4 syntax=cpp.doxygen

