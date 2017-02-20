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




#ifndef MO_MENU_ITEM_H
#define MO_MENU_ITEM_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_PROPS_H
#include	"mo_props.h"
#endif


// STL
//
#include	<map>


namespace molib
{


class MO_DLL_EXPORT moMenuItem : public moBase
{
public:
	moMenuItem( moPropBagRef );

	const moWCString& Value( const moWCString& name );
	
	bool IsSeparator() const	{ return f_isSeparator; }
	void IsSeparator( bool val )	{ f_isSeparator = val;  }

private:
	moPropBagRef			f_propBag;
	std::map<moWCString,moWCString>	f_values;
	bool				f_isSeparator;
};

typedef moSmartPtr<moMenuItem>	moMenuItemSPtr;

}
// namespace molib


// vim: ts=8 sw=8
#endif //MO_MENU_ITEM_H

