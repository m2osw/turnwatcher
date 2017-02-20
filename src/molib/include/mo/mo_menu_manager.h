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




#ifndef MO_MENU_MANAGER_H
#define MO_MENU_MANAGER_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif


#ifndef MO_MENU_ITEM_H
#include	"mo_menu_item.h"
#endif



namespace molib
{


class MO_DLL_EXPORT moMenuManager : public moBase
{
protected:
	moMenuManager();
	virtual ~moMenuManager();

	// moBase
	//
	virtual const char *	moGetClassName(void) const;

	// Events
	//
	virtual void Menu     ( const moWCString& path, moMenuItemSPtr ) = 0;
	virtual void EndMenu  ( const moWCString& path, moMenuItemSPtr ) = 0;
	virtual void Item     ( const moWCString& path, moMenuItemSPtr ) = 0;
	virtual void Separator( const moWCString& path, moMenuItemSPtr ) = 0;

	// Methods (with local implementations, but enforced overloading )
	//
	virtual void ClearMenus() 				= 0;
	virtual void AddMenu( moPropBagRef menuPropBag ) 	= 0;
	virtual void RemoveMenu( moPropBagRef menuPropBag ) 	= 0;
	virtual void PopulateMenus() 				= 0;

private:
	moPropBagRef		f_mainPropBag;	// Propbag containing the merged set of properties which
						// correspond to each menu entry
	moWCString		f_appPath;

	void MergeMenu( moPropArrayRef dest, moPropArrayRef source );
	void MergeMenu( moPropBagRef   dest, moPropBagRef   source );

	void PopulateMenu( const moWCString& path, moPropBagRef propBag );
	void PopulateMenu( const moWCString& path, moPropArrayRef array );
};

typedef moSmartPtr<moMenuManager>	moMenuManagerSPtr;

}
// namespace molib

// vim: ts=8 sw=8
#endif //MO_MENU_MANAGER_H

