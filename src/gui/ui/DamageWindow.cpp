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




// LOCAL
//
#include "DamageWindow.h"

namespace UI
{

namespace
{
	const molib::moWCString	Name("Name");
	const molib::moWCString	HitPoints("HitPoints");
	const molib::moWCString	TempHP("TempHP");
	const molib::moWCString	DamageHealth("DamageHealth");
}

DamageWindow::DamageWindow( Glib::ustring title, Gtk::Window& parent ) :
	Gtk::Dialog( title, parent, true, true ),
	f_value(0)
{
	Gtk::Label* label = new Gtk::Label(" ", Gtk::ALIGN_LEFT);
	label->set_justify( Gtk::JUSTIFY_LEFT );
	f_table.AddItem( Name, gettext("Character Name"), false, label, NULL );
	//
	label = new Gtk::Label(" ", Gtk::ALIGN_LEFT);
	label->set_justify( Gtk::JUSTIFY_LEFT );
	label->set_use_markup( true );
	f_table.AddItem( HitPoints, gettext("Current Hit Points"), false, label, NULL );
	//
	label = new Gtk::Label(" ", Gtk::ALIGN_LEFT);
	label->set_justify( Gtk::JUSTIFY_LEFT );
	label->set_use_markup( true );
	f_table.AddItem( TempHP, gettext("Temporary Hit Points"), false, label, NULL );
	//
	Gtk::Adjustment* adj = Gtk::manage( new Gtk::Adjustment(0.0, 0.0, 10000.0) );
	f_spinButton  = new Gtk::SpinButton;
	f_spinButton->configure( *adj, 0.0, 0 );
	f_spinButton->set_numeric( true );
	f_table.AddItem( DamageHealth, gettext("Enter new damage/health"), false, f_spinButton, NULL );

	// Pack children into a vbox and add buttons
	//
	Gtk::VBox* vbox = get_vbox();
	vbox->pack_start( f_table );
	//
	// Respect system-dependent alternative button order (defined
	// in gtk+ theme).
	//
	const bool alternative_order = alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) );
	//
	if( !alternative_order )
	{
		add_button( Gtk::Stock::CANCEL,		Gtk::RESPONSE_CANCEL );
	}
	//
	add_button( gettext("Bring to _Full"),	RESPONSE_FULLHEATH   );
	add_button( gettext("Add _Health"),	RESPONSE_HEALTH      );
	add_button( gettext("Add _Damage"),	Gtk::RESPONSE_OK     );
	//
	if( alternative_order )
	{
		add_button( Gtk::Stock::CANCEL,		Gtk::RESPONSE_CANCEL );
	}

	// Dialog stuff
	//
	set_default_response( Gtk::RESPONSE_OK );
	activate_default();
	show_all_children();
}


DamageWindow::~DamageWindow()
{
}


void DamageWindow::OnValueChanged()
{
	Gtk::SpinButton* sp;
	f_table.GetItem( DamageHealth, sp );
	f_value = atoi(sp->get_text().c_str());
}


void DamageWindow::on_show()
{
	Gtk::Dialog::on_show();
	
	f_spinConnection = f_spinButton->signal_value_changed().connect( sigc::mem_fun( *this, &DamageWindow::OnValueChanged ) );
	
	Gtk::SpinButton* sp;
	f_table.GetItem( DamageHealth, sp );
	set_focus( *sp );

	Gtk::Label* charLabel;
	Gtk::Label* hpLabel;
	Gtk::Label* tempHpLabel;
	f_table.GetItem( Name     , charLabel   );
	f_table.GetItem( HitPoints, hpLabel     );
	f_table.GetItem( TempHP   , tempHpLabel );

	if( f_character == 0 )
	{
		charLabel  ->set_text( gettext("Multiple Characters Selected!") );
		hpLabel    ->set_text( " " );
		tempHpLabel->set_text( " " );
	}
	else
	{
		charLabel->set_text( f_character->name().c_str() );

		const int BUFLEN = 128;
		char buf[BUFLEN+1];
		snprintf( buf, BUFLEN, "<b>%d</b>", f_character->hitpoints() );
		hpLabel->set_markup( buf );
		//
		snprintf( buf, BUFLEN, "<b>%d</b>", f_character->tempHP() );
		tempHpLabel->set_markup( buf );
	}
}


void DamageWindow::on_hide()
{
#ifdef DEBUG
	std::cout << "DamageWindow::on_hide()" << std::endl;
#endif

	f_spinConnection.disconnect();
	
	OnValueChanged();

	Gtk::Dialog::on_hide();
}


bool DamageWindow::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Dialog::on_key_press_event(event);

	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
	case GDK_Return:
		response( Gtk::RESPONSE_OK );
		break;

	case GDK_Escape:
		response( Gtk::RESPONSE_CANCEL );
		break;
	}

	return return_value;
}


void DamageWindow::Value( int val )
{
	f_value = val;
	Gtk::SpinButton* sp;
	f_table.GetItem( DamageHealth, sp );
	sp->set_value( f_value );
}

}
// namespace UI

// vim: ts=4 sw=4

