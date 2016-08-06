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

// GTKMM
//
#include <gtkmm.h>

// Local
//
#include "common.h"
#include "character.h"
#include "effect.h"

#include "ManagerBase.h"

namespace UI
{

class EffectsBook : 
	public Gtk::Notebook,
	public Application::ManagerBase
{
public:
	EffectsBook( Gtk::Window* parent = 0 );
	~EffectsBook() {}

	void SetParent( Gtk::Window* parent )			{ f_parent = parent; }

	void OnSelectionChanged( const Combatant::Character::list_t& selected_list );
	void OnUpdateCharacter( Combatant::Character::pointer_t ch );
	void OnUpdateEffects();
	void Clear();

	void AddEffect()	{ OnAddClick();     }
	void EditEffect()	{ OnEditClick();    }
	void DeleteEffect()	{ OnDismissClick(); }

	void NextTab();
	void PrevTab();

private:
	Combatant::Character::list_t f_selectedChars;
	sigc::connection           f_connection;
	int                        f_currentPage;
	Gtk::Window*               f_parent;

	// Gtk::Notebook
	//
	void OnSwitchPage( GtkNotebookPage*, guint );
	
	// Signals
	//
	void OnAddClick();
	void OnEditClick();
	void OnDismissClick();

	// Pages
	//
	void AddEmptyPage();
	void AddMultiCharPage();
	void AddEffect( Effects::Effect::pointer_t effect );
	void UpdateEffects();

	// Special
	//
	void AddEffect( Combatant::Character::pointer_t ch, Effects::Effect::pointer_t new_effect );
	void AddEffectToMultiple( Effects::Effect::pointer_t new_effect );
	void DismissCurrentEffect( Combatant::Character::pointer_t ch );
	void DismissAllEffects();

	virtual bool on_key_press_event( GdkEventKey* event );
};

}
// namespace UI

// vim: ts=4 sw=4


