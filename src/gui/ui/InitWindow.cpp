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
#include "InitWindow.h"

#include <math.h>
#include <algorithm>
#include <sstream>

using namespace motk;
using namespace Attribute;
using namespace Combatant;

namespace UI
{

namespace
{
	const double g_lower_range	= -100000.0;
	const double g_upper_range	= 100000.0;

	int GetValue( LabelTable& tbl, const molib::moName& id )
	{
		Gtk::SpinButton* b;
		tbl.GetItem( id, b );
		return b? atoi(b->get_text().c_str()): -1;
	}

	void SetValue( LabelTable& tbl, const molib::moName& id, int value )
	{
		Gtk::SpinButton* b;
		tbl.GetItem( id, b );
		if( b ) b->set_value( value );
	}
}


InitWindow::InitWindow( const Glib::ustring& title, const bool allowCancel ) :
	Gtk::Dialog( title, true, true ),
	f_cancelButton (0),
	f_acceptButton (0),
	f_allowCancel  (allowCancel)
{
    assert(GetStatMgr().lock());
	FillMainBox();

	show_all_children();
}


InitWindow::~InitWindow()
{
	// Empty
}


void InitWindow::FillMainBox()
{
	Gtk::VBox* 		vbox = get_vbox();
	Gtk::HBox*		hbox = Gtk::manage( new Gtk::HBox );

	// Add table editor
	//
	Gtk::Frame* charFrame = Gtk::manage( new Gtk::Frame( gettext("Characters") ) );
	//
	charFrame->add( f_charTable );
	//
	hbox->pack_start( *charFrame );

	Gtk::Frame* monsterFrame = Gtk::manage( new Gtk::Frame( gettext("Monsters") ) );
	//
	monsterFrame->add( f_monsterTable );
	//
	hbox->pack_start( *monsterFrame );
	vbox->pack_start( *hbox );

	const bool alternative_order = alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) );
	//
	if( alternative_order )
	{
		// Windows order, OK on the left, Cancel on the right
		f_acceptButton = add_button( Gtk::Stock::APPLY , Gtk::RESPONSE_OK );
	}
	//
	if( f_allowCancel )
	{
		f_cancelButton = add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	}
	else
	{
		f_resetButton = add_button( Gtk::Stock::REVERT_TO_SAVED, Gtk::RESPONSE_REJECT );
	}
	//
	if( !alternative_order )
	{
		// Other order like Gnome2 or MacOS
		//
		f_acceptButton = add_button( Gtk::Stock::APPLY , Gtk::RESPONSE_OK );
	}

	show_all_children();
}


void InitWindow::InitTable()
{
	f_charNameMap.clear();
	f_monsterNameMap.clear();
	//
    const int initId = GetStatMgr().lock()->initId();
	//
	{
		f_charTable.Clear();
		int id = 1;

		for( auto ch : f_chars )
		{
			assert(ch);
			Gtk::Adjustment* adj;
			adj = new Gtk::Adjustment(1.0, g_lower_range, g_upper_range);
			//
			std::stringstream ss;
			ss << "Character " << id << std::ends;
			molib::moName name( ss.str().c_str() );
			//
			f_charNameMap[name] = ch;
			//
			Gtk::SpinButton* sp;
			f_charTable.AddItem( name, ch->name().c_str(), false, sp = new Gtk::SpinButton(*adj), NULL );
			sp->set_numeric( true );
			const int init = ch->getRoll( initId );
			//
			SetValue( f_charTable, name, init );
		}
	}
	//
	{
		f_monsterTable.Clear();
		int id = 1;

        for( auto ch : f_monsters )
		{
			assert(ch);

			Gtk::Adjustment* adj;
			adj = new Gtk::Adjustment(1.0, g_lower_range, g_upper_range);
			//
			std::stringstream ss;
			ss << "Monster " << id << std::ends;
			molib::moName name( ss.str().c_str() );
			//
			f_monsterNameMap[name] = ch;
			//
			f_monsterTable.AddItem( name, ch->name().c_str(), false, new Gtk::SpinButton(*adj), NULL );
			const int init = ch->getRoll( initId );
			//
			SetValue( f_monsterTable, name, init );
		}
	}

	show_all_children();
}


void InitWindow::on_response( int response_id )
{
	switch( response_id )
	{
		case Gtk::RESPONSE_OK:
			OnAcceptButton();
			break;

		case Gtk::RESPONSE_REJECT:
			OnResetButton();
			break;
	}
}


bool InitWindow::on_key_press_event( GdkEventKey* event )
{
	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
		case GDK_Return:
		{
			//if( f_modified )
			//{
				response( Gtk::RESPONSE_OK );
				return true;
			//}
		}
		break;
	}

	return Window::on_key_press_event(event);
}


bool InitWindow::OnScrollChange( Gtk::ScrollType type, double value, bool is_char )
{
#ifdef DEBUG
	const char* from = is_char? "character": "monster";
	std::cout << "OnScrollChange: " << value
		  << " from " << from
		  << " scroller." << std::endl;
#endif
	return true;
}


void InitWindow::SetCharacters( pair_t& charPairList )
{
	f_chars.clear();
	f_monsters.clear();

	for( auto& pair : charPairList )
	{
		Character::pointer_t ch( new Character );
		assert(ch);
		ch->Copy( pair.f_origChar );
		//
		if( ch->monster() )
		{
			f_monsters.push_back( ch );
		}
		else
		{
			f_chars.push_back( ch );
		}
		//
		pair.f_newChar = ch;
	}

	auto lt_charcmp = []( Character::pointer_t c1, Character::pointer_t c2 ) -> bool
	{
		return c1->name() < c2->name();
	};

	// Sort the characters/monsters by name
	//
	std::stable_sort( f_chars.begin()   , f_chars.end()   , lt_charcmp );
	std::stable_sort( f_monsters.begin(), f_monsters.end(), lt_charcmp );

	// Add chars/monsters to the table
	//
	InitTable();
}


void InitWindow::OnAcceptButton()
{
	const int initId = GetStatMgr().lock()->initId();
	//
	for( auto pair : f_charNameMap )
	{
		const int init = GetValue( f_charTable, pair.first );
		assert( pair.second );
		pair.second->setRoll( initId, init );
	}
	//
	for( auto pair : f_monsterNameMap )
	{
		const int init = GetValue( f_monsterTable, pair.first );
		assert( pair.second );
		pair.second->setRoll( initId, init );
	}
}


void InitWindow::OnResetButton()
{
	// Reset values to original rolls from last transaction
	//
	InitTable();
}


}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
