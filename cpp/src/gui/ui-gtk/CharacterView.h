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
#include "ManagerBase.h"

// molib
//
#include "mo/mo_props.h"

namespace UI
{

class CharacterView
	: public Gtk::TreeView
	, private Application::ManagerBase
{
public:
	// Public types
	//
	typedef Combatant::Character::pointer_t		character_pointer_t;
	typedef Combatant::Character::list_t		character_list_t;
	typedef sigc::signal<void>					void_signal_t;
	typedef sigc::signal<void,character_list_t>	character_signal_t;

	// Construction/destruction
	//
	        								 CharacterView();
	virtual 								~CharacterView();

	// Selection maintenance
	//
	character_pointer_t						getFirstSelected();
	character_list_t						getSelected();
	void									setSelected( const Combatant::CharacterModel::iterator_t iter );
	void									setSelected( character_pointer_t ch );
	void									setSelected( character_list_t chars );

	// View column support
	//
	void									AddDefaultColumns();
	void									AddHUDColumns();

	// Selection-releated signals
	//
	void_signal_t							signal_double_click();
	character_signal_t						signal_selection_changed();

private:
	// Private types
	//
	typedef Glib::RefPtr<Gtk::TreeSelection>    tree_selection_t;
	typedef Gtk::TreeSelection::ListHandle_Path selected_paths_t;
	typedef std::map<molib::mo_name_t,int>      soft_columns_map_t;

	// Private data members
	//
	bool									f_isHUD;
	bool									f_initializing;
	bool									f_editingStarted;
	Gtk::TreeViewColumn*					f_currentCol;
	soft_columns_map_t						f_softColumns;
	tree_selection_t						f_selection;
	void_signal_t							f_doubleClickSignal;
	character_signal_t						f_selectionChangedSignal;
	//
	Gtk::TreeView::Column*					f_nameColumn;
	Gtk::TreeView::Column*					f_publicNameColumn;
    Gtk::TreeView::Column*					f_typeColumn;
#ifdef DEBUG
	Gtk::TreeView::Column*					f_positionColumn;
#endif
	Gtk::TreeView::Column*					f_orderColumn;
	Gtk::TreeView::Column*					f_initColumn;
	Gtk::TreeView::Column*					f_hpColumn;
	Gtk::TreeView::Column*					f_statusColumn;
	Gtk::TreeView::Column*					f_effectsColumn;
	Gtk::Tooltips							f_tooltips;

	void									setSelected( Gtk::TreeModel::Path path );
	void									emitSelectionSignal();

	Gtk::TreeView::Column*					addNameColumn( const QString& name );
	Gtk::TreeView::Column*					addInitColumn( const QString& name );
	Gtk::TreeView::Column*					addIconColumn( const QString& name, Gtk::TreeModelColumnBase& column );
	Gtk::TreeView::Column* 					addTextColumn( const QString& name, Gtk::TreeModelColumnBase& column,	
															const bool render_colors = true );
	Gtk::TreeView::Column* 					addTextColumn( const QString& name, Gtk::TreeModelColumnBase& column,
															Gtk::TreeModelColumnBase& sort_col,
															const bool render_colors = true );
	Gtk::TreeView::Column* 					addToggleColumn( const QString& name,
															Gtk::TreeModelColumnBase& column );
	int										addSoftColumn( Attribute::Stat::pointer_t stat );
	void									addSoftColumns( const bool onHud = false );
	void									addHUDSoftColumns();

	// Combatant::CharacterModel
	void									updateNewColumns();
	void 									onModelChange();

	// Attribute::StatManager
	//
	void									onSelectionChanged();
	void									onAppSettingsChanged();
	void									internalClear();
	void									onScrollToPath( Combatant::CharacterModel::path_t path );

	// Handle editing
	//
	void									on_cursor_changed();
	void									on_editing_started( Gtk::CellEditable*, const QString& );
	void									on_editing_canceled();
	void									next_tab_row( const bool forward );
	void									do_tab( const bool forward );

	// Sorting
	//
	void									on_sort_column_changed();
	void									onStartRounds();
	void									onEndRounds();
	void									onTurnChanged( const int index );

	// Gtk::Widget
	//
	virtual bool							on_key_press_event( GdkEventKey* event );
	virtual bool							on_button_press_event( GdkEventButton* event );
};


}
// namespace UI

// vim: ts=4 sw=4

