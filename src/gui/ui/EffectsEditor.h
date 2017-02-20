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
#include "EffectsList.h"

namespace UI
{

class EffectsEditor
    : public Gtk::VBox
{

public:
	EffectsEditor();
	~EffectsEditor();

	void AddEffects( Combatant::Character::pointer_t ch );
	void GetEffects( Combatant::Character::pointer_t ch );
	void ClearEffects();

private:
	EffectsList		f_treeView;
	Gtk::ScrolledWindow	f_scroller;

	Gtk::Tooltips		f_tooltips;
	Gtk::HButtonBox		f_buttons;
	Gtk::Button		f_addButton;
	Gtk::Button		f_deleteButton;
	Gtk::Button		f_editButton;
	
	void			AddTooltips();
	void 			FillMainBox();

	void			OnSelected();
	void			OnChanged();
	//
	void			OnEditButton( const bool add );
	void			OnDeleteButton();
};

}
// namespace UI


// vim: ts=8

