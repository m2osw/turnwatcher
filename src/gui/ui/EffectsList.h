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

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "character.h"
#include "effect.h"

namespace UI
{

class EffectsList
    : public Gtk::TreeView
{
public:
	EffectsList();
	~EffectsList();

	void AddEffects  ( Combatant::Character::pointer_t ch );
	void GetEffects  ( Combatant::Character::pointer_t ch );
	void AddEntry    ( const Effects::Effect::pointer_t effect );
	void UpdateEffect( Effects::Effect::pointer_t effect );
	void ClearEffects();

	int                      SelectedCount()     const;
	Effects::Effect::pointer_t GetFirstSelected();
	void                     DeleteSelected();
	bool                     FindDuplicates(     const  Effects::Effect::pointer_t effect );

	typedef sigc::signal<void> VoidSignal;
	VoidSignal	signal_changed() { return f_changedSignal; }

private:
	typedef Glib::RefPtr<Gtk::ListStore>		ListStorePtr;

	struct Columns : public Gtk::TreeModel::ColumnRecord
	{
		Gtk::TreeModelColumn<Glib::ustring>		f_name;
		Gtk::TreeModelColumn<int>			f_totalRounds;
		Gtk::TreeModelColumn<int>			f_roundsLeft;
		Gtk::TreeModelColumn<Glib::ustring>		f_type;
		Gtk::TreeModelColumn<Effects::Effect::pointer_t>	f_effect;

		Columns()
		{
			add( f_name        );
			add( f_totalRounds );
			add( f_roundsLeft  );
			add( f_type        );
			add( f_effect      );
		}
	};

	typedef Glib::RefPtr<Gtk::TreeSelection>	TreeSelectionPtr;
	typedef Gtk::TreeSelection::ListHandle_Path	SelectedPaths;

	Columns			f_columns;
	ListStorePtr		f_store;
	TreeSelectionPtr	f_selection;
	sigc::connection	f_selectionConnection;
	VoidSignal		f_changedSignal;

	Gtk::Tooltips		f_tooltips;

	void			UpdateRow( Gtk::TreeModel::Row& row, const Effects::Effect::pointer_t effect );
	void			AddTooltips();
	void 			FillMainBox();
	void			OnSelected();
};

}
// namespace UI

// vim: ts=8

