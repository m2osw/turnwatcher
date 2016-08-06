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
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "character.h"

#include <motk/LabelTable.h>

namespace UI
{

#define RESPONSE_HEALTH		-1
#define RESPONSE_FULLHEATH	-2

class DamageWindow :
	public Gtk::Dialog
{
public:
	DamageWindow( Glib::ustring title, Gtk::Window& parent );
	~DamageWindow();

	void	Char( Combatant::Character::pointer_t ch ) { f_character = ch; }
	void	Value( int val );
	int		Value() const 	{ return f_value; }

private:
	Gtk::SpinButton*				f_spinButton;
	sigc::connection				f_spinConnection;
	motk::LabelTable				f_table;
	int								f_value;
	Combatant::Character::pointer_t	f_character;

	// Gtk::SpinButton signal_value_changed()
	//
	void		OnValueChanged();

	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual void	on_hide();
	virtual bool	on_key_press_event( GdkEventKey* event );	
};

}
// namespace UI

// vim: ts=4 sw=4

