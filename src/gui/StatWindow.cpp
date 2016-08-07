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
#include "StatWindow.h"

namespace UI
{


StatWindow::StatWindow( Glib::ustring title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true  )
	, f_upButton( Gtk::Stock::GO_UP )
	, f_downButton( Gtk::Stock::GO_DOWN )
	, f_addButton( Gtk::Stock::ADD )
	, f_deleteButton( Gtk::Stock::DELETE )
{
	show_all_children();

	Gtk::VBox* vbox = get_vbox();
	f_scroller.add( f_statEditor );
	vbox->pack_start( f_scroller, true, true );

	//
	// Lower buttons
	//
	// Custom responses
	//
	Gtk::HButtonBox* bbox = static_cast<Gtk::HButtonBox*>(get_action_area());
	bbox->pack_start( f_upButton	 );
	bbox->pack_start( f_downButton	 );
	bbox->pack_start( f_addButton    );
	bbox->pack_start( f_deleteButton );
	//
	f_upButton.set_sensitive( false );
	f_upButton.signal_clicked().connect( sigc::bind( sigc::mem_fun( f_statEditor, &StatEditor::MoveSelected ), true /*up*/ ) );
	//
	f_downButton.set_sensitive( false );
	f_downButton.signal_clicked().connect( sigc::bind( sigc::mem_fun( f_statEditor, &StatEditor::MoveSelected ), false /*up*/ ) );
	//
	f_addButton.set_sensitive( true );
	f_addButton.signal_clicked().connect( sigc::mem_fun( f_statEditor, &StatEditor::AddNewStat ) );
	//
	f_deleteButton.set_sensitive( false );
	f_deleteButton.signal_clicked().connect( sigc::mem_fun( f_statEditor, &StatEditor::DeleteSelected ) );
	//
	f_statEditor.get_selection()->signal_changed().connect( sigc::mem_fun( *this, &StatWindow::OnSelectionChanged ) );
	//
	// Default responses
	//
	if( alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) ) )
	{
		add_button( Gtk::Stock::OK,		Gtk::RESPONSE_OK     );
		add_button( Gtk::Stock::CANCEL,	Gtk::RESPONSE_CANCEL );
	}
	else
	{
		add_button( Gtk::Stock::CANCEL,	Gtk::RESPONSE_CANCEL );
		add_button( Gtk::Stock::OK,		Gtk::RESPONSE_OK     );
	}
	
	f_scroller.set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC );
	//
	set_default_size( 300, 400 );
	
	show_all_children();
}


StatWindow::~StatWindow()
{
}


void StatWindow::on_show()
{
#ifdef DEBUG
	std::cout << "StatWindow::on_show()" << std::endl;
#endif

	Gtk::Dialog::on_show();
}


void StatWindow::on_hide()
{
#ifdef DEBUG
	std::cout << "StatWindow::on_hide()" << std::endl;
#endif

	Gtk::Dialog::on_hide();
}


bool StatWindow::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Dialog::on_key_press_event(event);

	switch( event->keyval )
	{
		case GDK_Return:
			//response( Gtk::RESPONSE_OK );
			break;

		case GDK_Escape:
			response( Gtk::RESPONSE_CANCEL );
			break;
	}

	return return_value;
}


void StatWindow::on_response( int response_id )
{
	switch( response_id )
	{
		case Gtk::RESPONSE_OK:
			f_statEditor.Apply();
            GetStatMgr().lock()->signal_changed().emit();
			break;
	}
	
	Gtk::Dialog::on_response( response_id );
}



void StatWindow::OnSelectionChanged()
{
    Glib::RefPtr<Gtk::TreeSelection>	selection( f_statEditor.get_selection() );
	//
	if( selection->get_selected_rows().size() > 0 )
	{
		f_upButton.set_sensitive( true );
		f_downButton.set_sensitive( true );
		f_deleteButton.set_sensitive( true );
	}
	else
	{
		f_upButton.set_sensitive( false );
		f_downButton.set_sensitive( false );
		f_deleteButton.set_sensitive( false );
	}
}


}
// namespace UI

// vim: ts=4 sw=4 syntax=cpp.doxygen

