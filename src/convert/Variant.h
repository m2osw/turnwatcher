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

// Stl
//
#include <exception>
#include <map>

// Qt4
//
#include <QString>
#include <QVariant>
#include <QMap>
#include <QList>

class Variant : public QVariant
{
	public:
        typedef QMap<QString, Variant>		Map;
        typedef std::map<QString, Variant>	StdMap;

		Variant() : QVariant( QVariant::UserType ) {}
		//
		Variant( const bool      val ) : QVariant( val ) {}
		Variant( const int       val ) : QVariant( val ) {}
		Variant( const quint64   val ) : QVariant( val ) {}
		Variant( const QString&  val ) : QVariant( val ) {}
		Variant( const QVariant& val ) : QVariant( val ) {}

		QString	SectionId() const 					{ return f_sectionId; }
		void	SectionId( const QString& val )		{ f_sectionId = val; }

		const size_t	size() const 				{ return f_map.size(); }
		Variant&		operator[]( const QString& id )
		{
			return f_map[id]; 
		}

		Variant&		operator=( const char* value ) { *this = QString(value); return *this; }

        const StdMap& 	ToStdMap() const	{ return f_map; }
        StdMap& 		ToStdMap() 			{ return f_map; }

        Map 			ToQMap() 			{ return Map(f_map); }

		void DebugOutput( const int level = 0 );
		void DebugDump();

	protected:
		QString		f_sectionId;
		//
        StdMap		f_map;
};


typedef QList<Variant> VariantList;

// vim: ts=4 sw=4 syntax=cpp.doxygen

