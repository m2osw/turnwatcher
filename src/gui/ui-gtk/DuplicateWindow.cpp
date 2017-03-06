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
#include "DuplicateWindow.h"

namespace
{
	const QString	g_id( "Duplicates" );
}

namespace UI
{

DuplicateWindow::DuplicateWindow( QString title, Gtk::Window& parent )
    : Gtk::Dialog( title, parent, true, true )
    , f_value(0)
{
	Gtk::Adjustment* adj = Gtk::manage( new Gtk::Adjustment(0.0, 1.0, 10.0) );
	Gtk::SpinButton* sp  = new Gtk::SpinButton;
	sp->configure( *adj, 0.0, 0 );
	sp->set_numeric( true );
	sp->signal_value_changed().connect( sigc::mem_fun( *this, &DuplicateWindow::OnValueChanged ) );
	f_table.AddItem( g_id, gettext("Enter number of duplicates:"), false, sp, NULL );

	// Pack children into a vbox and add buttons
	//
	Gtk::VBox* vbox = get_vbox();
	vbox->pack_start( f_table );
	
	if( alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) ) )
	{
		add_button( Gtk::Stock::OK,		Gtk::RESPONSE_OK     );
		add_button( Gtk::Stock::CANCEL,		Gtk::RESPONSE_CANCEL );
	}
	else
	{
		add_button( Gtk::Stock::CANCEL,		Gtk::RESPONSE_CANCEL );
		add_button( Gtk::Stock::OK,		Gtk::RESPONSE_OK     );
	}

	// Dialog stuff
	//
	set_default_response( Gtk::RESPONSE_OK );
	activate_default();
	show_all_children();
}


DuplicateWindow::~DuplicateWindow()
{
}


void DuplicateWindow::OnValueChanged()
{
	Gtk::SpinButton* sp;
	f_table.GetItem( g_id, sp );
	f_value = atoi(sp->get_text().c_str());
}


void DuplicateWindow::on_show()
{
	Gtk::Dialog::on_show();

	Gtk::SpinButton* sp;
	f_table.GetItem( g_id, sp );
	set_focus( *sp );
}


bool DuplicateWindow::on_key_press_event( GdkEventKey* event )
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


void DuplicateWindow::Value( int val )
{
	f_value = val;
	Gtk::SpinButton* sp;
	f_table.GetItem( g_id, sp );
	sp->set_value( f_value );
}

}
// namespace UI

// vim: ts=8

