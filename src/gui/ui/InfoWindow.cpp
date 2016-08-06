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
#include "ui/InfoWindow.h"
#include "base/character.h"

using namespace Combatant;

namespace UI
{


InfoWindow::InfoWindow( Glib::ustring title, Gtk::Window& parent ) :
	Gtk::Dialog( title, parent )
{
	f_showInfoAction = motk::ToggleActionPtr::cast_dynamic( GetActionsMgr().lock()->GetAction( "View::ShowInfo" ) );

	set_default_size( 600, 480 );
	for( int idx = 0; idx < MAX_PANES; ++idx )
	{
		f_effectsBook[idx].SetParent(&parent);
	}
	FillMainBox();
	show_all_children();
	
	// Hook up signals
	//
	for( int idx = 0; idx < MAX_PANES; ++idx )
	{
		f_infoBox[idx].signal_edit().connect( sigc::mem_fun( *this, &InfoWindow::OnEditCharacter ) );
	}

	add_accel_group( GetActionsMgr().lock()->GetAccelGroup() );
}

InfoWindow::~InfoWindow()
{
}


void InfoWindow::OnEditCharacter( Character::pointer_t ch )
{
	if( !is_visible() )	return;
	f_signalEdit.emit( ch );
}


Gtk::ScrolledWindow* InfoWindow::CreateScroller( const int id )
{
	Gtk::VBox*		box;
	Gtk::ScrolledWindow*	scroller = &f_scroller[id];

	assert( id >=0 && id <MAX_PANES );

	box = Gtk::manage( new Gtk::VBox );
	box->pack_start( f_effectsBook[id], Gtk::PACK_SHRINK );
	box->pack_start( f_infoBox[id], Gtk::PACK_EXPAND_WIDGET );

	scroller->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	scroller->add( *box );

	return scroller;
}


void InfoWindow::FillMainBox()
{
	//Gtk::VBox*		box;
	Gtk::ScrolledWindow*	scroller;

	scroller = CreateScroller( 0 );
	f_pane.pack1( *scroller, Gtk::EXPAND );
	
	scroller = CreateScroller( 1 );
	f_pane.pack2( *scroller, Gtk::EXPAND );

	Gtk::VBox* vbox = get_vbox();
	vbox->pack_start( f_pane, true, true );
}


void InfoWindow::OnReload()
{
	for( int idx = 0; idx < MAX_PANES; ++idx )
	{
		f_effectsBook[idx].OnUpdateEffects();
		f_infoBox[idx].OnReload();
	}
}


void InfoWindow::OnClear()
{
	Character::list_t empty_list;
	OnSelectionChanged( empty_list );
}


void InfoWindow::OnSelectionChanged( const Character::list_t& selected_list )
{
	if( !is_visible() )	return;

	f_pane.remove( f_scroller[1] );

	if( selected_list.size() == 1 )
	{
		Character::pointer_t char_1( selected_list[0] );
		f_effectsBook[0].OnUpdateCharacter( char_1 );
		f_infoBox[0].OnSelectionChanged( char_1 );
	}
	else if( selected_list.size() == 2 )
	{
		Character::pointer_t char_1( selected_list[0] );
		f_effectsBook[0].OnUpdateCharacter( char_1 );
		f_infoBox[0].OnSelectionChanged( char_1 );

		f_pane.pack2( *CreateScroller( 1 ), Gtk::EXPAND );
		Character::pointer_t char_2( selected_list[1] );
		f_effectsBook[1].OnUpdateCharacter( char_2 );
		f_infoBox[1].OnSelectionChanged( char_2 );
	}
	else
	{
		for( int idx = 0; idx < MAX_PANES; ++idx )
		{
			f_effectsBook[idx].Clear();
			f_infoBox[idx].OnSelectionChanged( 0 );
		}
	}
}


void InfoWindow::on_show()
{
	Gtk::Dialog::on_show();
}


void InfoWindow::on_hide()
{
	Gtk::Dialog::on_hide();
}


bool InfoWindow::on_key_press_event( GdkEventKey* event )
{
#if 0
	bool return_value = false;

	switch( event->keyval )
	{
		case GDK_Return:
			break;

		case GDK_N:
			if( event->state & GDK_CONTROL_MASK )
			{
				f_showInfoAction->set_active( false );
				return_value = true;
			}
			break;

		case GDK_Escape:
			f_showInfoAction->set_active( false );
			return_value = true;
			break;
	}

	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	if( !return_value )
	{
		return_value = Dialog::on_key_press_event(event);
	}

	return return_value;
#else
	return Dialog::on_key_press_event(event);
#endif
}

bool InfoWindow::on_delete_event(GdkEventAny* event)
{
#if 0
	const bool return_value = Gtk::Window::on_delete_event( event );
	return return_value;
#else
	f_showInfoAction->set_active(false);
	return true;
#endif
}

}
// namespace UI


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
