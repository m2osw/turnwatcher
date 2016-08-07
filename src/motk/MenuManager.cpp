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




#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Local
//
#include "MenuManager.h"

// STL
//
#include <string>
#include <algorithm>

// SIGC++
// 
#include <sigc++/functors/mem_fun.h>

// GTKMM
//
#include <gtkmm/stock.h>

using namespace std;
using namespace molib;

namespace motk
{


/** \class MenuManager
 *
 * \brief Derives from molib::moMenuManager, and provides a Gtk::VBox populated with a menu.
 *
 * Loads up a menu property bag and populates a Gtk::Menu object based on the contents.
 *
 * \todo: Add support for a toolbar
 */

/** \brief Initiatizes a MenuManager object
 *
 */
MenuManager::MenuManager() :
    	molib::moMenuManager(),
	f_menuBar(0),
	f_toolBar(0)
{
	f_currentMenu = "MenuBar";
}


/** \brief Destroys a MenuManager object
 *
 */
MenuManager::~MenuManager()
{
#ifdef DEBUG
    printf( "MenuManager::~MenuManager()\n" );
#endif
}


/** \brief Identify the class type for molib
 *
 */
const char *MenuManager::GetClassName(void) const
{
	return "motk::MenuManager";
}


/** \brief Posts dynamic event when menu item clicked
 *
 * This posts a dynamic menu event when a menu item is selected (clicked).
 */
void MenuManager::OnButtonClicked( Glib::RefPtr<Gtk::Action>& action, const Glib::ustring& event )
{
#ifdef _DEBUG
    cout << "MenuManager::OnButtonClicked(): " << event << endl;
#endif

    MenuBroadcast::PostDynamicEvent( moApplication::Instance(), event.c_str() );
}


/** \brief Posts dynamic event when menu item clicked
 *
 * Clears the menu of all merged menus and menu items.
 */
void MenuManager::ClearAllBars()
{
	if( f_menuBar )
		remove( *f_menuBar );

	if( f_toolBar )
		remove( *f_toolBar );

	UIBase::Clear();
}


/** \brief Adds a new menu item to the menu bar
 *
 * This builds an internal ui XML description of the menu bar, indicating
 * the menu action item has already been added to the GTK uimanager.
 */
void MenuManager::AddMenuItem( const Glib::ustring& event )
{
	Glib::ustring& ui = f_uiInfo;

	ui += f_tabPrefix;
	if( event == "--" )
	{
		ui += "<separator/>\n";
	}
	else
	{
		ui += "<menuitem action='";
		ui += event;
		ui += "'/>\n";
	}
}


/** \brief Populates the menu bar
 *
 * This creates and populates a menu bar based on the property bag.
 */
void MenuManager::PopulateMenuBar()
{
	Glib::ustring& ui = f_uiInfo;
	ui = "<ui>\n";
	ui += "\t<menubar action='MenuBar'>\n";
	f_currentMenu = "MenuBar";
	f_menuStack.push(f_currentMenu);

	molib::moMenuManager::PopulateMenus();

	f_currentMenu = f_menuStack.top();
	f_menuStack.pop();

	ui += "\t</menubar>\n";
	ui += "</ui>\n";

#if defined(_DEBUG) && 0
	cout << "MENU UI:\n" << ui << endl;
#endif

	UIBase::AddUI( ui );
}


/** \brief Add toolbar actions to ui string
 *
 */
void MenuManager::AddToolbarUI( menu_item_map_t::value_type& _item )
{
	molib::moMenuItemSPtr item = _item.second;
	Glib::ustring event = item->Value("Event").c_str();
	if( item->IsSeparator() )
	{
		f_toolbarUiInfo += "\t\t<separator/>\n";
	}
	else
	{
		f_toolbarUiInfo += "\t\t<toolitem action='" + event + "'/>\n";
	}
}


/** \brief Populates the toolbar
 *
 * This creates and populates a toolbar based on the property bag.
 */
void MenuManager::PopulateToolBar()
{
	Glib::ustring& ui = f_toolbarUiInfo;
	ui = "<ui>\n";
	ui += "\t<toolbar action='ToolBar'>\n";

	std::for_each( f_toolbarItems.begin(), f_toolbarItems.end(), sigc::mem_fun(*this, &MenuManager::AddToolbarUI) );

	ui += "\t</toolbar>\n";
	ui += "</ui>\n";

#if defined(MO_DEBUG)
	cout << "TOOLBAR UI:\n" << ui << endl;
#endif

	UIBase::AddUI( ui );
}


/** \brief Create a new menu action
 *
 * Creates a menu action delivered by the derived class.
 */
void MenuManager::Menu( const molib::moWCString& path, molib::moMenuItemSPtr item )
{
	molib::moWCString	name  	  = item->Value("Name");
	molib::moWCString	event 	  = item->Value("Event");
	Glib::ustring		menu_path = UIBase::MakePath( path.c_str(), name.c_str() );

	AddAction( name.c_str(), menu_path );

	f_menuStack.push( f_currentMenu );
	f_currentMenu = menu_path;

	f_tabPrefix = "";
	for( size_t idx = 0; idx <= f_menuStack.size(); idx++ )
	{
		f_tabPrefix += "\t";
	}

	Glib::ustring& ui = f_uiInfo;
	ui += f_tabPrefix;
	ui += "<menu action='";
	ui += menu_path;
	ui += "'>\n";
}


/** \brief End a menu
 *
 * Ends a menu action item delivered by the derived class.
 */
void MenuManager::EndMenu( const molib::moWCString& path, molib::moMenuItemSPtr )
{
	f_currentMenu = f_menuStack.top();
	f_menuStack.pop();

	Glib::ustring& ui = f_uiInfo;
	ui += f_tabPrefix;
	ui += "</menu>\n";
	
	f_tabPrefix = "";
	for( size_t idx = 0; idx <= f_menuStack.size(); idx++ )
	{
		f_tabPrefix += "\t";
	}
}



namespace
{
#define CHECK_STOCK_ID( ID )	if( stock_image == #ID ) return Gtk::Stock::ID;

	Gtk::StockID TranslateBuiltinStockId( const molib::moWCString& stock_image )
	{
		CHECK_STOCK_ID( ABOUT			);
		CHECK_STOCK_ID( ADD  			);
		CHECK_STOCK_ID( APPLY			);
		CHECK_STOCK_ID( BOLD 			);
		CHECK_STOCK_ID( CANCEL			);
		CHECK_STOCK_ID( CDROM			);
		CHECK_STOCK_ID( CLEAR			);
		CHECK_STOCK_ID( CLOSE			);
		CHECK_STOCK_ID( COLOR_PICKER		);
		CHECK_STOCK_ID( CONNECT			);
		CHECK_STOCK_ID( CONVERT			);
		CHECK_STOCK_ID( COPY   			);
		CHECK_STOCK_ID( CUT    			);
		CHECK_STOCK_ID( DELETE 			);
		CHECK_STOCK_ID( DIALOG_AUTHENTICATION	);
		CHECK_STOCK_ID( DIALOG_ERROR		);
		CHECK_STOCK_ID( DIALOG_INFO 		);
		CHECK_STOCK_ID( DIALOG_QUESTION		);
		CHECK_STOCK_ID( DIALOG_WARNING		);
		CHECK_STOCK_ID( DIRECTORY		);
		CHECK_STOCK_ID( DISCONNECT		);
		CHECK_STOCK_ID( DND			);
		CHECK_STOCK_ID( DND_MULTIPLE		);
		CHECK_STOCK_ID( EDIT			);
		CHECK_STOCK_ID( EXECUTE			);
		CHECK_STOCK_ID( FILE			);
		CHECK_STOCK_ID( FIND			);
		CHECK_STOCK_ID( FIND_AND_REPLACE	);
		CHECK_STOCK_ID( FLOPPY			);
		CHECK_STOCK_ID( FULLSCREEN		);
		CHECK_STOCK_ID( GO_BACK			);
		CHECK_STOCK_ID( GO_DOWN			);
		CHECK_STOCK_ID( GO_FORWARD		);
		CHECK_STOCK_ID( GO_UP			);
		CHECK_STOCK_ID( GOTO_BOTTOM		);
		CHECK_STOCK_ID( GOTO_FIRST		);
		CHECK_STOCK_ID( GOTO_LAST		);
		CHECK_STOCK_ID( GOTO_TOP		);
		CHECK_STOCK_ID( HARDDISK		);
		CHECK_STOCK_ID( HELP			);
		CHECK_STOCK_ID( HOME			);
		CHECK_STOCK_ID( INDENT			);
		CHECK_STOCK_ID( INDEX			);
		CHECK_STOCK_ID( INFO			);
		CHECK_STOCK_ID( ITALIC			);
		CHECK_STOCK_ID( JUMP_TO			);
		CHECK_STOCK_ID( JUSTIFY_CENTER		);
		CHECK_STOCK_ID( JUSTIFY_FILL		);
		CHECK_STOCK_ID( JUSTIFY_LEFT		);
		CHECK_STOCK_ID( JUSTIFY_RIGHT		);
		CHECK_STOCK_ID( LEAVE_FULLSCREEN	);
		CHECK_STOCK_ID( MEDIA_FORWARD		);
		CHECK_STOCK_ID( MEDIA_NEXT		);
		CHECK_STOCK_ID( MEDIA_PAUSE		);
		CHECK_STOCK_ID( MEDIA_PLAY		);
		CHECK_STOCK_ID( MEDIA_PREVIOUS		);
		CHECK_STOCK_ID( MEDIA_RECORD		);
		CHECK_STOCK_ID( MEDIA_REWIND		);
		CHECK_STOCK_ID( MEDIA_STOP		);
		CHECK_STOCK_ID( MISSING_IMAGE		);
		CHECK_STOCK_ID( NETWORK			);
		CHECK_STOCK_ID( NEW			);
		CHECK_STOCK_ID( NO			);
		CHECK_STOCK_ID( OK			);
		CHECK_STOCK_ID( OPEN			);
		CHECK_STOCK_ID( PASTE			);
		CHECK_STOCK_ID( PREFERENCES		);
		CHECK_STOCK_ID( PRINT			);
		CHECK_STOCK_ID( PRINT_PREVIEW		);
		CHECK_STOCK_ID( PROPERTIES		);
		CHECK_STOCK_ID( QUIT			);
		CHECK_STOCK_ID( REDO			);
		CHECK_STOCK_ID( REFRESH			);
		CHECK_STOCK_ID( REMOVE			);
		CHECK_STOCK_ID( REVERT_TO_SAVED		);
		CHECK_STOCK_ID( SAVE			);
		CHECK_STOCK_ID( SAVE_AS			);
		CHECK_STOCK_ID( SELECT_COLOR		);
		CHECK_STOCK_ID( SELECT_FONT		);
		CHECK_STOCK_ID( SORT_ASCENDING		);
		CHECK_STOCK_ID( SORT_DESCENDING		);
		CHECK_STOCK_ID( SPELL_CHECK		);
		CHECK_STOCK_ID( STOP			);
		CHECK_STOCK_ID( STRIKETHROUGH		);
		CHECK_STOCK_ID( UNDELETE		);
		CHECK_STOCK_ID( UNDERLINE		);
		CHECK_STOCK_ID( UNDO			);
		CHECK_STOCK_ID( UNINDENT		);
		CHECK_STOCK_ID( YES			);
		CHECK_STOCK_ID( ZOOM_100		);
		CHECK_STOCK_ID( ZOOM_FIT		);
		CHECK_STOCK_ID( ZOOM_IN			);
		CHECK_STOCK_ID( ZOOM_OUT		);
		return Gtk::Stock::FILE;
	}
}


/** \brief Create a new menu item in the current menu
 *
 * Add a menu item along with action.
 */
void MenuManager::Item( const molib::moWCString& path, molib::moMenuItemSPtr item )
{
	molib::moWCString name  	= item->Value("Name");
	molib::moWCString event 	= item->Value("Event");
	molib::moWCString image 	= item->Value("Image");
	molib::moWCString stock_image	= item->Value("StockImage");
	molib::moWCString toolbar	= item->Value("Toolbar");

	if( name.IsEmpty() || event.IsEmpty() )
	{
		std::cerr << "Item " << path.c_str() << " is bad! Important missing elements!" << std::endl;
		exit( 1 );
	}

	if( !image.IsEmpty() )
	{
		ActionManager::ButtonImage _image( image.c_str() );
		AddAction( name.c_str(), event.c_str(), &_image );
	}
	else if( !stock_image.IsEmpty() )
	{
		Gtk::StockID id = TranslateBuiltinStockId( stock_image );
		AddAction( name.c_str(), event.c_str(), id );
	}
	else
	{
		AddAction( name.c_str(), event.c_str() );
	}

	AddMenuItem( event.c_str() );

	if( !toolbar.IsEmpty() )
	{
		const int weight = atoi( toolbar.c_str() );
#ifdef MO_DEBUG
		cout << "toolbar=" << toolbar.c_str() << ", weight=" << weight << endl;
#endif
		f_toolbarItems[weight] = item;
	}
}


/** \brief Create a menu item separator in the current menu
 *
 * Add a separator to the current menu
 */
void MenuManager::Separator( const molib::moWCString& path, molib::moMenuItemSPtr item )
{
	AddMenuItem( "--" );

	molib::moWCString toolbar = item->Value("Toolbar");
	if( !toolbar.IsEmpty() )
	{
		const int weight = atoi( toolbar.c_str() );
#ifdef MO_DEBUG
		cout << "SEPARATOR toolbar=" << toolbar.c_str() << ", weight=" << weight << endl;
#endif
		f_toolbarItems[weight] = item;
	}
}


/** \brief Clear all menus
 *
 * Clear menu bar.
 */
void MenuManager::ClearMenus()
{
	molib::moMenuManager::ClearMenus();
	ClearAllBars();
}


/** \brief Add a menu property bag
 *
 * Add a menu based on property bag.
 */
void MenuManager::AddMenu( molib::moPropBagRef menuPropBag )
{
	molib::moMenuManager::AddMenu( menuPropBag );
}


/** \brief Remove a menu property bag
 *
 * Remove a menu based on property bag.
 */
void MenuManager::RemoveMenu( molib::moPropBagRef menuPropBag )
{
	molib::moMenuManager::RemoveMenu( menuPropBag );
}


/** \brief Populate menus
 *
 * Populate menus based on property bag specification.
 */
void MenuManager::PopulateMenus()
{
	molib::moMenuManager::PopulateMenus();
}


/** \brief Redraw menu
 *
 * Cause the menu to be redrawn. This is done by causing a reload of the xml.
 */
void MenuManager::RedrawMenu()
{
	ClearAllBars();
	UIBase::SetActionGroup( f_refActionGroup );

	f_toolbarItems.clear();

	PopulateMenuBar();
	PopulateToolBar();

	f_menuBar = dynamic_cast<Gtk::MenuBar*>(UIBase::GetWidget( "/ui/MenuBar" ));
	pack_start( *f_menuBar, Gtk::PACK_SHRINK );

	//Gtk::Widget* w = UIBase::GetWidget( "/ui/ToolBar" );
	f_toolBar = dynamic_cast<Gtk::Toolbar*>(UIBase::GetWidget( "/ui/ToolBar" ));
	pack_start( *f_toolBar, Gtk::PACK_SHRINK );

	show_all_children();
}


/** \brief Handle dynamic event
 *
 * Cause the dynamic event to be propagated.
 */
void MenuManager::DynamicEvent( const molib::moName& __name )
{
 	MenuManagerEvent::DynamicEvent( __name );
}


}
//namespace motk

// vim: ts=8 sw=8


