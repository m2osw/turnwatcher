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
#include "EffectsList.h"

namespace UI
{

class ExpiredEffectsWindow :
	public Gtk::Dialog
{
public:
	ExpiredEffectsWindow( Glib::ustring title, Gtk::Window& parent );
	~ExpiredEffectsWindow();

	bool AddExpiredEffects( Combatant::Character::Pointer ch );

private:
	Gtk::Label	f_label;
	EffectsList	f_effectsList;

	virtual bool	on_key_press_event( GdkEventKey* event );
	virtual void	on_response( int response_id );
};

}
// namespace UI


// vim: ts=8 sw=8

