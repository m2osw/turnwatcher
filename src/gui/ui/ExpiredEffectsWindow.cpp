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





// LOCAL
//
#include "ExpiredEffectsWindow.h"
#include "transactions/CharacterEntry.h"

using namespace Effects;

namespace UI
{

ExpiredEffectsWindow::ExpiredEffectsWindow( Glib::ustring title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true )
	, f_label( "Do you want to delete the expired effect_list?", Gtk::ALIGN_LEFT )
{
	f_label.set_justify( Gtk::JUSTIFY_LEFT );

	Gtk::VBox* box = get_vbox();
	assert(box);
	box->pack_start( f_effectsList, Gtk::PACK_EXPAND_WIDGET );
	box->pack_start( f_label,       Gtk::PACK_SHRINK        );

	add_button( Gtk::Stock::YES, Gtk::RESPONSE_YES );
	add_button( Gtk::Stock::NO,  Gtk::RESPONSE_NO  );

	set_default_response( Gtk::RESPONSE_YES );
	activate_default();
	show_all_children();
}


ExpiredEffectsWindow::~ExpiredEffectsWindow()
{
}


bool ExpiredEffectsWindow::AddExpiredEffects( Combatant::Character::pointer_t ch )
{
	f_effectsList.ClearEffects();

	bool expired_effect_list = false;
	Effect::list_t	effect_list;
	ch->getEffects(effect_list);
	for( auto effect : effect_list )
	{
		assert(effect);
		if( effect->roundsLeft() <= 0 )
		{
			// Only add expired rounds
			f_effectsList.AddEntry( effect );
			expired_effect_list = true;
		}
	}

	return expired_effect_list;
}


void ExpiredEffectsWindow::on_response( int response_id )
{
	// Empty
}


bool ExpiredEffectsWindow::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Dialog::on_key_press_event(event);

	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
	case GDK_Return:
	case GDK_y:
	case GDK_Y:
		response( Gtk::RESPONSE_YES );
		break;

	case GDK_Escape:
	case GDK_n:
	case GDK_N:
		response( Gtk::RESPONSE_NO );
		break;
	}

	return return_value;
}

}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

