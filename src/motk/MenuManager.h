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




#ifndef __MENUMANAGER_H__
#define __MENUMANAGER_H__

// STL
//
#include <memory>
#include <vector>
#include <string>

// MOLIB
//
#include <mo/mo_base.h>
#include <mo/mo_name.h>
#include <mo/mo_event.h>
#include <mo/mo_props.h>
#include <mo/mo_application.h>
#include <mo/mo_menu_manager.h>

// GTKMM
//
#include <glibmm.h>
#include <gtkmm.h>

// MOTK
//
#include <ActionManager.h>
#include <UiBase.h>

// Local includes
//
#include "Menu_event.h"

namespace motk
{

class MenuManager :
	public Gtk::VBox,
	public molib::moMenuManager,
	public molib::moEventReceiver,
	public MenuManagerEvent,
	public MenuBroadcast,
	public motk::UIBase,
	public motk::ActionManager
{
public:
	MenuManager();
	virtual ~MenuManager();

	// moBase
	//
	virtual const char *	GetClassName(void) const;

	// moMenuManager
	//
	virtual void Menu     ( const molib::moWCString& path, molib::moMenuItemSPtr );
	virtual void EndMenu  ( const molib::moWCString& path, molib::moMenuItemSPtr );
	virtual void Item     ( const molib::moWCString& path, molib::moMenuItemSPtr );
	virtual void Separator( const molib::moWCString& path, molib::moMenuItemSPtr );

	// MenuManagerEvent
	//
	virtual void ClearMenus();
	virtual void AddMenu( molib::moPropBagRef menuPropBag );
	virtual void RemoveMenu( molib::moPropBagRef menuPropBag );
	virtual void PopulateMenus();

	virtual void RedrawMenu();

	// MenuBroadcast
	//
	virtual void DynamicEvent( const molib::moName& __name );

private:
	Gtk::MenuBar*		f_menuBar;	// Main menu control we are attaching to
	Gtk::Toolbar*		f_toolBar;

	typedef std::map<int,molib::moMenuItemSPtr> menu_item_map_t;
	menu_item_map_t		f_toolbarItems;

	motk::StringStack	f_menuStack;
	Glib::ustring		f_currentMenu;
	Glib::ustring		f_uiInfo;
	Glib::ustring		f_toolbarUiInfo;
	Glib::ustring		f_tabPrefix;

	void			ClearAllBars();
	void			ClearToolBar();
	void 			AddMenuItem( const Glib::ustring& event );
	void			AddToolbarUI( menu_item_map_t::value_type& item );
	void			PopulateMenuBar();
	void			PopulateToolBar();

	// ActionManager
	//
	virtual void 		OnButtonClicked( Glib::RefPtr<Gtk::Action>& action, const Glib::ustring& event  );
};

typedef molib::moSmartPtr<MenuManager>	MenuManagerSPtr;


}
//namespace motk

#endif //__MENUMANAGER_H__

// vim: ts=8 sw=8

