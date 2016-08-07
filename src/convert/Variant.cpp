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
#include "Variant.h"

/*! \class Variant
 *
 * This class derives from the Qt4 QVariant class, allowing one to embed a map of
 * QString to Variant objects. 
 */


/*! \brief Output the contents of the internal map.
 */
void Variant::DebugOutput( const int level )
{
#ifdef DEBUG
	QString tabs;
	for( int idx = 0; idx < level; ++idx ) { tabs.push_back("\t"); }

	if( type() == QVariant::UserType )
	{
		std::cout << QString("%1***** Begin section [%2]").arg(tabs).arg(f_sectionId) << std::endl;

		Q_FOREACH( StdMap::value_type section, f_map )
		{
			std::cout << QString("%1Section [%2]").arg(tabs).arg(section.first) << std::endl;

			section.second.DebugOutput( level + 1 );
		}

		std::cout << QString("%1***** End section [%2]").arg(tabs).arg(f_sectionId) << std::endl;
	}
	else
	{
		std::cout << QString("%1Value [%2]").arg(tabs).arg(toString()) << std::endl;
	}
#endif //DEBUG
}


void Variant::DebugDump()
{
	QString values;
	Q_FOREACH( StdMap::value_type section, f_map )
	{
		values += QString("%1 = [%2]\n").arg(section.first).arg(section.second.toString());
	}

	//LogTrace( QString("%1").arg(values) );
}

// vim: ts=4 sw=4 syntax=cpp.doxygen

