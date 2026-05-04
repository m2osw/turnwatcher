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

// STL
//
#include <map>

// MOLIB
//
#include "mo/mo_base.h"
#include "mo/mo_props.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "character.h"

namespace UI
{

class JumpInWindow
	: public Gtk::Dialog
{

public:
	JumpInWindow( QString title, Gtk::Window& parent );
	~JumpInWindow();

	void AddCharacter( Combatant::Character::pointer_t ch );
	void RemoveCharacter( Combatant::Character::pointer_t ch );
	void Clear()           { f_listStore->clear(); }
	const int Size() const { return f_listStore->children().size(); }
	Combatant::Character::pointer_t GetSelected();

private:
	class Columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<QString>			name;
		Gtk::TreeModelColumn<Combatant::Character::pointer_t>	character;
		//
		Columns()
		{
			add( name );
			add( character );
		}
	};

	Glib::RefPtr<Gtk::ListStore>		f_listStore;
	Glib::RefPtr<Gtk::TreeModelSort>	f_sortStore;
	Glib::RefPtr<Gtk::TreeSelection>	f_selection;
	Gtk::TreeView				f_treeView;
	Columns					f_columns;

	void 		FillMainBox();
	bool 		FindDuplicates( Combatant::Character::pointer_t ch ) const;
	Gtk::TreeIter	FindCharacter( Combatant::Character::pointer_t ch ) const;
	void 		selection_changed();

	// Gtk::Window
	//
	//virtual void	on_set_focus( Gtk::Widget* focus );
	
	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual bool	on_key_press_event( GdkEventKey* event );
};

}
// namespace UI


// vim: ts=8 sw=8

