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




#pragma once

#include "common.h"

// STL
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include <stack>

// GTKMM
#include <gtkmm.h>
#include <gdkmm.h>

// Local includes
#include "character.h"
#include "stat.h"
#include "CharacterColumns.h"
#include "SingletonManager.h"

// molib
//
#include "mo/mo_props.h"

namespace UI
{

class CharacterListUI :
	public Gtk::TreeView,
	private Application::ManagerBase
{
public:
	CharacterListUI();
	~CharacterListUI();

	// List maintenance
	//
	void									insertCharacter( Combatant::Character::Pointer ch );
	void 									removeCharacter( Combatant::Character::Pointer ch );
	void									updateCharacter( Combatant::Character::Pointer ch );
	void									clear();

	// Update other features (such as initiative)
	//
	void									update();

	// Selection maintenance
	//
	Combatant::Character::Pointer			getFirstSelected();
	Combatant::Character::List				getSelected();
	void									setSelected( Combatant::Character::Pointer ch );
	void									setSelected( Combatant::Character::List chars );

	void									setDefaultSort();
	int										getCurrentSort( Gtk::SortType& type );
	void									setCurrentSort( const int id, Gtk::SortType type );

	// 
	void									AddDefaultColumns();
	void									AddHUDColumns();

	void									setDC( const int dc );
	int										getDC() const		{ return f_dc; }
	molib::mo_name_t						getSoftSort() const	{ return f_softSortType; }
	void									setSoftSort( const molib::mo_name_t type );

	// Selection-releated signals
	//
	typedef sigc::signal<void>				VoidSignal;
	VoidSignal								signal_double_click();
	//
	typedef sigc::signal<void,Combatant::Character::List> CharactersSignal;
	CharactersSignal						signal_selection_changed();

	typedef Glib::RefPtr<Gtk::TreeStore>	TreeStorePtr;
	TreeStorePtr							GetStore() { return f_store; }

private:
	typedef Glib::RefPtr<Gdk::Pixbuf>			PixbufPtr;
	typedef Glib::RefPtr<Gtk::TreeModelSort>	TreeModelSortPtr;
	typedef Glib::RefPtr<Gtk::TreeSelection>	TreeSelectionPtr;
	typedef Gtk::TreeSelection::ListHandle_Path	SelectedPaths;

	Combatant::CharacterColumns::Pointer	f_columns;	

	bool									f_isHUD;
	bool									f_initializing;
	bool									f_editingStarted;
	Gtk::TreeModel::Path					f_currentPath;
	Gtk::TreeViewColumn*					f_currentCol;

	int										f_dc;
	molib::mo_name_t						f_softSortType;
	typedef std::map<molib::mo_name_t,int>	Name2IntMap;
	Name2IntMap								f_softColumns;

	TreeStorePtr							f_store;
	TreeModelSortPtr						f_initSort;
	TreeSelectionPtr						f_selection;

	PixbufPtr								f_pixbufArrow;
	PixbufPtr								f_pixbufMonster;
	PixbufPtr								f_pixbufCharacter;

	VoidSignal								f_doubleClickSignal;
	CharactersSignal						f_selectionChangedSignal;

	Gtk::TreeView::Column*					f_nameColumn;
	Gtk::TreeView::Column*					f_typeColumn;
#ifdef DEBUG
	Gtk::TreeView::Column*					f_positionColumn;
#endif
	Gtk::TreeView::Column*					f_orderColumn;
	Gtk::TreeView::Column*					f_initColumn;
	Gtk::TreeView::Column*					f_hpColumn;
	Gtk::TreeView::Column*					f_statusColumn;
#if defined(WANT_EFFECTS)
	Gtk::TreeView::Column*					f_effectsColumn;
#endif
	Gtk::Tooltips							f_tooltips;

	typedef Gtk::TreeModel::iterator 		Iterator;

	Iterator								findCharacter( Combatant::Character::Pointer ch );
	void									UpdateSoftColumns( const Gtk::TreeRow& row, const Glib::ustring& default_background_color );
	void									updateRow( const Gtk::TreeRow& iter );
	void									setSelected( Iterator iter );
	void									updateAllCharacters();
	// Cause selection signal to emit (with current selection)
	void									emitSelectionSignal();
	void									connectInitMgrSignals();

	Gtk::TreeView::Column*					addNameColumn( const Glib::ustring& name );
	Gtk::TreeView::Column*					addInitColumn( const Glib::ustring& name );
	Gtk::TreeView::Column*					addIconColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column );
	Gtk::TreeView::Column* 					addTextColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column,	
															const bool render_colors = true );
	Gtk::TreeView::Column* 					addTextColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column,
															Gtk::TreeModelColumnBase& sort_col,
															const bool render_colors = true );
	Gtk::TreeView::Column* 					addToggleColumn( const Glib::ustring& name,
															Gtk::TreeModelColumnBase& column );
	int										addSoftColumn( Attribute::Stat::Pointer stat );
	void									addSoftColumns( const bool onHud = false );
	void									addHUDSoftColumns();

	// Attribute::StatManager
	//
	void									OnStatsChanged();
	void									updateNewColumns();
	void									onSelectionChanged();
	void									onAppSettingsChanged();

	void									unsetArrow();
	void									setArrow( const int index, const bool select );
	void									internalClear();

	// Handle editing
	//
	void									on_cursor_changed();
	void									on_editing_started( Gtk::CellEditable*, const Glib::ustring& );
	void									on_editing_canceled();
	void									next_tab_row( const bool forward );
	void									do_tab( const bool forward );

	// Sorting
	//
	void									on_sort_column_changed();

	// Gtk::Widget
	//
	virtual bool							on_key_press_event( GdkEventKey* event );
	virtual bool							on_button_press_event( GdkEventButton* event );
};


}
// namespace UI

// vim: ts=4 sw=4

