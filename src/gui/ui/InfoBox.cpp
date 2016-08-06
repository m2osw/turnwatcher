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
#include "ui/InfoBox.h"

#include <math.h>

using namespace Attribute;


namespace UI
{


InfoBox::InfoBox()
{
	auto statMgr(GetStatMgr().lock());
	assert(statMgr);
	//f_stats = statMgr->GetStats();
	
	// Misc stuff about the label
	//
	set_wrap_mode( Gtk::WRAP_WORD );
	f_infoBuffer = Gtk::TextBuffer::create();
	set_buffer( f_infoBuffer );
	set_editable( false );

	// StatManager
	//
	statMgr->signal_changed().connect( sigc::mem_fun( *this, &InfoBox::OnReloadTables ) );
}


InfoBox::~InfoBox()
{
}


void InfoBox::AddCR()
{
	f_infoBuffer->insert_at_cursor( "\n" );
}


void InfoBox::AddComma()
{
	f_infoBuffer->insert_at_cursor( ", " );
}


void InfoBox::AddAttribute( const Glib::ustring& name, const Glib::ustring& value )
{
	Glib::RefPtr<Gtk::TextTag> attr_tag  = f_infoBuffer->create_tag();
	attr_tag->property_weight()  = Pango::STYLE_ITALIC;

	Glib::RefPtr<Gtk::TextTag> value_tag = f_infoBuffer->create_tag();
	value_tag->property_weight() = Pango::WEIGHT_BOLD;

	f_infoBuffer->insert_with_tag( f_infoBuffer->end(), name + ": ", attr_tag );
	f_infoBuffer->insert_with_tag( f_infoBuffer->end(), value, value_tag );
}


void InfoBox::AddAttribute( const Glib::ustring& name, const int value )
{
	const int BUFLEN = 16;
	char buffer[BUFLEN+1];
	snprintf( buffer, BUFLEN, "%d", value );
	AddAttribute( name, buffer );
}


void InfoBox::AddModifier( const Glib::ustring& value )
{
	f_infoBuffer->insert_at_cursor( " (+" + value + ")" );
}

void InfoBox::AddModifier( const int value )
{
	const int BUFLEN = 16;
	char buffer[BUFLEN+1];
	snprintf( buffer, BUFLEN, "%d", value );
	AddModifier( buffer );
}


void InfoBox::AddPercentage( const Glib::ustring& value )
{
	Glib::RefPtr<Gtk::TextTag> value_tag = f_infoBuffer->create_tag();
	value_tag->property_weight() = Pango::WEIGHT_BOLD;

	const Glib::ustring text( " (" + value + "%) " );
	f_infoBuffer->insert_with_tag( f_infoBuffer->end(), text, value_tag );
}

void InfoBox::AddPercentage( const int value )
{
	const int BUFLEN = 16;
	char buffer[BUFLEN+1];
	snprintf( buffer, BUFLEN, "%d", value );
	AddPercentage( buffer );
}


void InfoBox::OnEditCharacter()
{
	if( !is_visible() ) return;
	f_signalEdit.emit( f_char );
}


void InfoBox::GenerateAbilityString()
{
	f_infoBuffer->insert_at_cursor( "Name: " );
	f_infoBuffer->insert_at_cursor( f_char->name() );
	f_infoBuffer->insert_at_cursor( " " );
	Glib::RefPtr<Gtk::TextChildAnchor> refAnchor = f_infoBuffer->create_child_anchor(f_infoBuffer->end());
	Gtk::Button* editBtn = Gtk::manage( new Gtk::Button("Edit") );
	editBtn->signal_clicked().connect( sigc::mem_fun( *this, &InfoBox::OnEditCharacter ) );
	//editBtn->set_sensitive( true );
	add_child_at_anchor( *editBtn, refAnchor );
	f_infoBuffer->insert_at_cursor( "\n" );
	editBtn->show_all();

	auto statMgr( GetStatMgr().lock() );
	assert(statMgr);

	auto       iter (statMgr->GetStats().begin());
	const auto end  (statMgr->GetStats().end()  );

	while( iter != end )
	{
		Stat::pointer_t stat ( iter->second );
		Value::pointer_t value ( f_char->getStat( stat->id() ) );

		const bool last = (++iter == end);

		if( !stat->ability() ) continue;
#if 0
		if( stat->enabled() )
		{
#endif
			AddAttribute( stat->name(), value->total() );
			AddModifier( Common::StatToMod( value->total() ) );
#if 0
		}
		else
		{
			AddAttribute( stat->name(), "--" );
		}
#endif

		if( last )
		{
			AddCR();
		}
		else
		{
			AddComma();
		}
	}
}


void InfoBox::GenerateBuiltinString()
{
	const int	init    = f_char->getMod( GetStatMgr().lock()->initId(), true /*with_ability*/ );
	const int	hp      = f_char->maxHP();
	const int	damage  = f_char->damage();
	const int	hp_left = f_char->hitpoints();
	const int	pct     = hp? (int)(((float)hp_left / (float) hp) * 100.0): 0;

	AddAttribute( "Init",              init    ); AddComma();
	AddAttribute( "Max Hitpoints",     hp      ); AddComma();
	AddAttribute( "Current Damage",    damage  ); AddComma();
	AddAttribute( "Current Hitpoints", hp_left ); AddPercentage( pct>=0? pct: 0 );
	AddCR();
}


void InfoBox::GenerateStatString()
{
	auto statMgr( GetStatMgr().lock() );
	assert(statMgr);
	auto		iter( statMgr->GetStats().begin() );
	const auto	end ( statMgr->GetStats().end()   );

	while( iter != end )
	{
		Stat::pointer_t	stat ( iter->second );

		const int mod   = f_char->getMod( stat->id(), true /*with_ability*/ );
		const bool last = (++iter == end);

		if( stat->ability() || stat->internal() ) continue;

		AddAttribute( stat->name(), mod );

		if( last )
		{
			AddCR();
		}
		else
		{
			AddComma();
		}
	}
}


void InfoBox::AddNotesString()
{
	f_infoBuffer->insert_at_cursor( f_char->notes() );
}


#if 0
void InfoBox::MakeReadOnly()
{
	Glib::RefPtr<Gtk::TextTag> read_only_tag = f_infoBuffer->create_tag();
	read_only_tag->property_editable() = false;
	//
	f_infoBuffer->apply_tag( read_only_tag, f_infoBuffer->begin(), f_infoBuffer->end() );
}
#endif


void InfoBox::UpdateDialog()
{
	if( !is_visible() ) return;
#if 0
	if( !f_editCharAction && f_refActionGroup )
	{
		f_editCharAction = f_refActionGroup->get_action( "Edit::Edit" );
	}
#endif

	f_infoBuffer->erase( f_infoBuffer->begin(), f_infoBuffer->end() );
	//
	if( f_char )
	{
		GenerateAbilityString();
		GenerateBuiltinString();
		GenerateStatString();
		//
		AddCR();
		AddNotesString();
		//
		//MakeReadOnly();
	}
	show_all();
}

void InfoBox::OnReload()
{
	UpdateDialog();
}


void InfoBox::OnSelectionChanged( Combatant::Character::pointer_t ch )
{
	if( f_char == ch )
	{
		return;
	}

	if( ch )
	{
		f_char			= ch;
		f_connection	= f_char->signal_changed().connect( sigc::mem_fun( *this, &InfoBox::OnReload ) );
	}
	else
	{
		f_char = 0;
		f_connection.disconnect();
	}
	//
	UpdateDialog();
}


void InfoBox::OnSelectionChanged( const Combatant::Character::list_t& selected_list )
{
	Combatant::Character::pointer_t ch;
	if( selected_list.size() == 1 )
	{
		ch = selected_list[0];
	}
	OnSelectionChanged( ch );
}
	

void InfoBox::OnReloadTables()
{
	if( !is_visible() ) return;
	//f_stats = GetStatMgr().lock()->GetStats();
	UpdateDialog();
}


}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
