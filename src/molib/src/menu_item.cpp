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
#pragma implementation "mo/mo_menu_item.h"
#endif


// MOLIB
//
#include "mo/mo_menu_item.h"



namespace molib
{


/** \class moMenuItem
 *
 * \brief Wraps an instance of a menu item
 *
 * You may extract various attribute values from the item, for example "Name" or "Event".
 *
 * \code
 *	molib::moMenuItem	item( somePropBag );
 *	molib::moWCString	name  = item["Name"];
 *	molib::moWCString	event = item["Event"];
 * \endcode
 */


/* \brief Initiatizes a moMenuItem using a propBag instance
 *
 */
moMenuItem::moMenuItem( moPropBagRef propBag ) :
	f_propBag(propBag),
	f_isSeparator(false)
{
}


/* \brief Allows look-up of a name for a value
 */
const moWCString& moMenuItem::Value( const moWCString& name )
{
	if( f_values[name] == "" )
	{
		try
		{
			moPropStringRef	valueProp = f_propBag[moIndexOrName(name.c_str())];
			moWCString value = valueProp;
			f_values[name] = value.c_str();
		}
		catch( const moException& )
		{
		}
	}

	return f_values[name];
}


}
// namespace molib



// vim: ts=8 sw=8

