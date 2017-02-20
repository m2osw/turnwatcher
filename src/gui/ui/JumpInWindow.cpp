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
#include "JumpInWindow.h"

namespace UI
{

namespace
{
	const molib::moWCString	g_name("NAME");
}


JumpInWindow::JumpInWindow( Glib::ustring title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true )
{
	set_default_size( 320, 300 );
	FillMainBox();
	f_selection->signal_changed().connect( sigc::mem_fun( *this, &JumpInWindow::selection_changed) );
	set_response_sensitive( Gtk::RESPONSE_OK, false );
	set_default_response( Gtk::RESPONSE_OK );
	activate_default();
	//add_events( Gtk::KEY_RELEASE_MASK );
	show_all_children();
}


JumpInWindow::~JumpInWindow()
{
}


void JumpInWindow::selection_changed()
{
	set_response_sensitive( Gtk::RESPONSE_OK, f_selection->count_selected_rows() > 0 );
}


void JumpInWindow::on_show()
{
#ifdef DEBUG
	std::cout << "JumpInWindow::on_show()" << std::endl;
#endif

	Gtk::Dialog::on_show();

	set_focus( f_treeView );
}


#if 0
void JumpInWindow::on_set_focus( Gtk::Widget* focus )
{
	std::cout << "on_set_focus()" << std::endl;
}
#endif


bool JumpInWindow::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Dialog::on_key_press_event(event);

	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
		case GDK_Return:
		if( f_selection->count_selected_rows() > 0 )
		{
			response( Gtk::RESPONSE_OK );
		}
		break;

		case GDK_Escape:
			response( Gtk::RESPONSE_CANCEL );
			break;
	}

	return return_value;
}


void JumpInWindow::FillMainBox()
{
	f_listStore = Gtk::ListStore::create    ( f_columns   );
	f_sortStore = Gtk::TreeModelSort::create( f_listStore );

	f_treeView.set_rules_hint();
	f_treeView.set_headers_visible( false );
	f_treeView.set_model( f_sortStore );

	f_selection = f_treeView.get_selection();

	f_treeView.append_column( gettext("Character Waiting"), f_columns.name );
	f_sortStore->set_sort_column( f_columns.name, Gtk::SORT_ASCENDING );

	Gtk::VBox* vbox = get_vbox();
	vbox->pack_start( f_treeView, true, true );
	
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
}


bool JumpInWindow::FindDuplicates( Combatant::Character::pointer_t ch ) const
{
	assert(ch);
	assert(f_listStore);

	bool dup_found = false;
	//
	for( auto row : f_listStore->children() )
	{
		if( row[f_columns.name] == ch->name().c_str() )
		{
			dup_found = true;
			break;
		}
	}

	return dup_found;
}


Gtk::TreeIter JumpInWindow::FindCharacter( Combatant::Character::pointer_t ch ) const
{
	assert(ch);
	assert(f_listStore);
    return std::find_if( f_listStore->children().begin(), f_listStore->children().end(),
        [&]( const Gtk::TreeRow& row )
        {
            return row[f_columns.name] == ch->name().c_str();
        });
}


void JumpInWindow::AddCharacter( Combatant::Character::pointer_t ch )
{
	assert(ch);
	if( FindDuplicates(ch) ) return;

	Gtk::TreeIter iter ( f_listStore->append() );
	Gtk::TreeRow  row  ( *iter );
	//
	row[f_columns.name] 		= ch->name().c_str();
	row[f_columns.character]	= ch;
}


void JumpInWindow::RemoveCharacter( Combatant::Character::pointer_t ch )
{
	Gtk::TreeIter iter = FindCharacter( ch );
	if( iter != f_listStore->children().end() )
	{
		f_listStore->erase( iter );
	}
}


Combatant::Character::pointer_t JumpInWindow::GetSelected()
{
	Combatant::Character::pointer_t ch;
	if( Size() == 1 )
	{
		Gtk::TreeRow row( f_listStore->children()[0] );
		ch = row[f_columns.character];
	}
	else
	{
		Gtk::TreeIter iter( f_selection->get_selected() );
		Gtk::TreeRow  row( *iter );
		ch = row[f_columns.character];
	}
	return ch;
}
	

}
// namespace UI


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

