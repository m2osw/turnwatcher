//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
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
#pragma implementation "mo/mo_menu_manager.h"
#endif


// MOLIB
//
#include "mo/mo_menu_manager.h"

#ifndef MO_APPLICATION_H
#include "mo/mo_application.h"
#endif



namespace molib
{


/** \class moMenuManager
 *
 * \brief Provides a base class that processes a menu propBag and parses it into menu entries
 *
 * Derive from moMenuManager, and make sure you overload the pure virtual methods.
 * Once you load a "*.menu" file into a propBag instance, pass it into "AddMenu()".
 * For each menu entry, Menu() will be called. Likewise for the other methods (EndMenu,
 * Item, and Separator.)
 *
 */


/** \brief Initiatizes a moMenuManager object
 *
 */
moMenuManager::moMenuManager() :
	f_mainPropBag( "MainMenu" )
{
	f_mainPropBag.NewProp();
	f_appPath = moApplication::Instance()->GetApplicationPath();
}


/** \brief Destroys a moMenuManager object
 *
 */
moMenuManager::~moMenuManager()
{
}


// documented in base.c++
const char *moMenuManager::moGetClassName() const
{
	return "molib::moMenuManager";
}


/** \brief Merge a prop array containing menu information into a destination array
 *
 * This recursive method takes a property array and peruses it, adding its
 * contents to the source array. This allows you to add a menu to another menu.
 * Menu weights are respected.
 *
 * \param[in] dest Merge the \p source array of menu items to this property array
 * \param[in] source The source of menu items to merge
 *
 * \sa MergeMenu
 */
void moMenuManager::MergeMenu( moPropArrayRef dest, moPropArrayRef source )
{
	unsigned long idx = 0;
	const unsigned long end = source.CountIndexes();

	for( ; idx < end; idx++ )
	{
		const int		item_no		= source.ItemNoAtIndex( idx );
		moPropSPtr		sourceProp	= source.Get( item_no );
		moProp::prop_type_t	type		= sourceProp->GetType();
		moName			name		= sourceProp->GetName();

		switch( type )
		{
		case moProp::MO_PROP_TYPE_PROP_BAG:
		    {
			    moPropBagRef	sourceRef(name);
			    sourceRef.NewProp();
			    sourceRef.GetProperty()->Copy(*sourceProp);

			    moPropBagRef	destRef(name);
			    destRef.NewProp();
			    moProp*	prop = dest.Get( item_no );
			    if( prop )
			    {
			    	destRef = *((moPropBag*)prop);
			    }

			    MergeMenu( destRef, sourceRef );
			    dest.Set( item_no, destRef );
		    }
		    break;
		
		case moProp::MO_PROP_TYPE_STRING:
		    {
			moPropStringRef	strProp(name);
			strProp.NewProp();
			strProp.GetProperty()->Copy( *sourceProp );
			dest.Set( item_no, strProp );
		    }
		    break;

		default:
			// TODO: Message needed here
			// to indicate we have an unsupported type for this structure
			std::cerr << "Unsupported property type for this structure!" << std::endl;
		}
	}
}


/** \brief Merge a property set containing menu information into a destination property set
 *
 * This recursive method takes a property bag and peruses it, adding its
 * contents to the source property bag. This allows you to add a menu to another menu.
 * Menu weights are respected.
 *
 * \param[in] dest The destination property bag where \p source is merged
 * \param[in] source The source property bag to add to the destination
 *
 * \sa MergeMenu
 */
void moMenuManager::MergeMenu( moPropBagRef dest, moPropBagRef source )
{
	unsigned long idx = 0;
	const unsigned long end = source.Count();

	for( ; idx < end; idx++ )
	{
		moPropRef		sourceProp	= source[idx];
		moProp::prop_type_t	type		= sourceProp.GetType();
		moName			name		= sourceProp.GetName();
		moIndexOrName		mo_index( (moWCString) name );

		switch( type )
		{
		case moProp::MO_PROP_TYPE_ARRAY:
			{
				moPropArrayRef	array = sourceProp;
				try
				{
					moPropArrayRef	p = dest[mo_index];
					MergeMenu( p, array );
				}
				catch( const moError& )
				{
					moPropArrayRef	destArrayProp( name );
					destArrayProp.NewProp();
					MergeMenu( destArrayProp, array );
					dest.Set( mo_index, destArrayProp );
				}
	
			}
			break;

		case moProp::MO_PROP_TYPE_STRING:
		case moProp::MO_PROP_TYPE_INT:
			{
				dest.Set( mo_index, sourceProp );
			}
			break;

		default:
			// TODO: Message needed here
			// to indicate we have an unsupported type for this structure
			std::cerr << "Unsupported property type for this structure!" << std::endl;
		}
	}
}


/** \brief Populate a displayed menu
 *
 * This method peruses an array of menu items, calling the pure virtual methods
 * which the derived class should build a displayed menu.
 *
 * \param[in] path Path to this menu
 * \param[in] array An array of menu items
 *
 * \sa PopulateMenu
 */
void moMenuManager::PopulateMenu( const moWCString& path, moPropArrayRef array )
{
	const moNamePool& pool   = moNamePool::GetNamePool();
	const mo_name_t	menuName = pool.Get("Menu");
	const mo_name_t	itemName = pool.Get("Item");
	const mo_name_t sepName  = pool.Get("Separator");
	//const mo_name_t tbName   = pool.Get("Toolbar");

	unsigned long idx = 0;
	const unsigned long end = array.CountIndexes();

	for( ; idx < end; idx++ )
	{
		moPropSPtr		itemProp	= array.GetAtIndex( idx );
		moProp::prop_type_t	type		= itemProp->GetType();
		moName			name		= itemProp->GetName();
		mo_name_t		name_t		= (mo_name_t) name;

		switch( type )
		{
		case moProp::MO_PROP_TYPE_PROP_BAG:
		    {
			moPropBagRef	itemRef(name);
			itemRef.NewProp();
			itemRef.GetProperty()->Copy(*itemProp);

			moMenuItemSPtr	menu_item = new moMenuItem( itemRef );

			if( name_t == menuName )
			{
				// Signal menu start
				//
				Menu( path, menu_item );

				// Populate the menu
				//
				moWCString	item_name( menu_item->Value("Name") );
				moWCString	p = path + "/" + item_name;
				PopulateMenu( p, itemRef );

				// Signal menu finish
				//
				EndMenu( path, menu_item );
			}
			else if( name_t == itemName )
			{
				Item( path, menu_item );
			}
			else if( name_t == sepName )
			{
				menu_item->IsSeparator( true );
				Separator( path, menu_item );
			}
		    }
		    break;

		default:
		    std::cerr << "PopulateMenu(): Unsupported property type for this structure!" << std::endl;
		}
	}

}


/** \brief Populate a displayed menu
 *
 * This method peruses a property bag of arrays of menu items. It calls
 * PopulateMenu() to cause a displayed menu to be populated.
 *
 * \param[in] path Path to this menu
 * \param[in] propBag A set of property bag arrays to add to the menus
 *
 * \sa PopulateMenu
 */
void moMenuManager::PopulateMenu( const moWCString& path, moPropBagRef propBag )
{
	unsigned long idx = 0;
	const unsigned long end = propBag.Count();

	for( ; idx < end; idx++ )
	{
		moPropRef		prop		= propBag[idx];
		moProp::prop_type_t	type		= prop.GetType();

		if( type == moProp::MO_PROP_TYPE_ARRAY )
		{
			moPropArrayRef	array = prop;
			PopulateMenu( path, array );
		}
	}
}


/** \brief Clear out the menu property bag
 *
 * This method dumps the main property bag menu items.  This is required to be
 * overridden, then call moMenuManager::ClearMenus() to make sure the bag is
 * emptied.
 *
 */
void moMenuManager::ClearMenus()
{
	f_mainPropBag.Empty();
}


/** \brief Add new menus and menu entries
 *
 * Adds a new bag of menus and menu entries to the current menu set. This can
 * throw molib::moError on failure.
 *
 * \exception moError
 *
 * \param[in] newPropBag The property bag to add to the current menu set.
 */
void moMenuManager::AddMenu( moPropBagRef newPropBag )
{
	MergeMenu( f_mainPropBag, newPropBag );
}


/** \brief Remove menu property bag
 *
 * Removes a menu bag from the current menu.
 *
 * \todo: This method is currently non-functional--code is required!
 *
 */
void moMenuManager::RemoveMenu( moPropBagRef menuPropBag )
{
	// TODO: Add code
}


/** \brief Populate the top-level menu
 *
 * This method populates the top-level menu
 *
 */
void moMenuManager::PopulateMenus()
{
	PopulateMenu( "$/", f_mainPropBag );
}


}
// namespace molib



// vim: ts=8 sw=8

