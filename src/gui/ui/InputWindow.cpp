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
#include "InputWindow.h"

namespace UI
{

InputWindow::InputWindow( Glib::ustring title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true )
	, f_label( "Value goes here", Gtk::ALIGN_LEFT )
{
	f_label.set_justify( Gtk::JUSTIFY_LEFT );

	Gtk::VBox* box = get_vbox();
	box->pack_start( f_label, Gtk::PACK_SHRINK );
	box->pack_start( f_entry, Gtk::PACK_EXPAND_WIDGET );

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

	set_default_response( Gtk::RESPONSE_OK );
	activate_default();
	show_all_children();
}


InputWindow::~InputWindow()
{
}


void InputWindow::Label( const std::string& val )
{
	f_label.set_label( val );
}


void InputWindow::Value( const std::string& val )
{
	f_value = val;
	f_entry.set_text( f_value );
}


void InputWindow::on_response( int response_id )
{
	if( response_id == Gtk::RESPONSE_OK )
	{
		f_value = f_entry.get_text();
	}
}


bool InputWindow::on_key_press_event( GdkEventKey* event )
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

}
// namespace UI

// vim: ts=8

