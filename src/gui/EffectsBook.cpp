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




#include "EffectsBook.h"
#include "EffectEditor.h"
#include "transaction.h"
#include "transactions.h"
#include "ActionsManager.h"

using namespace Transactions;
using namespace Effects;

namespace UI
{

EffectsBook::EffectsBook( Gtk::Window* parent ) :
	f_parent(parent),
	f_currentPage(0)
{
	set_border_width(10);
	signal_switch_page().connect( sigc::mem_fun( *this, &EffectsBook::OnSwitchPage ) );
}


void EffectsBook::AddEffect( Combatant::Character::Pointer ch, Effect::Pointer new_effect )
{
	assert(ch);

    Combatant::Character::Pointer newChar( new Combatant::Character );
	newChar->Copy( ch );
	//
    Effect::List effects_list;
	newChar->getEffects( effects_list );
	new_effect->apply( newChar );
	effects_list.push_back( new_effect );
	newChar->setEffects( effects_list );
	//
	TransactionGroupHelper group( gettext("Add New Effect") );
    Transactions::Transaction::Pointer tr( new EditCharacterTransaction( ch, ch, newChar ) );
	group.AddTransaction( tr );
}


void EffectsBook::AddEffectToMultiple( Effect::Pointer new_effect )
{
	TransactionGroupHelper group( gettext("Add New Effect to Multiple Characters") );
	//
	for( auto ch : f_selectedChars )
	{
		assert(ch);

		Combatant::Character::Pointer newChar( new Combatant::Character );
		newChar->Copy( ch );
		//
		Effect::Pointer effect_copy( new Effect );
		effect_copy->Copy( new_effect );
		//
        Effect::List effects_list;
		newChar->getEffects( effects_list );
		effect_copy->apply( newChar );
		effects_list.push_back( effect_copy );
		newChar->setEffects( effects_list );
		//
		Transactions::Transaction::Pointer tr( new EditCharacterTransaction( ch, ch, newChar ) );
		group.AddTransaction( tr );
	}
}

	
void EffectsBook::OnAddClick()
{
	Actions::Desensitizer	ad;

	Effect::Pointer	new_effect( new Effect );
	EffectEditor	editor( gettext("Add New Effect") );
	//
	editor.set_transient_for( *f_parent );
	editor.SetEffect( new_effect );
	//
	if( editor.run() == Gtk::RESPONSE_OK )
	{
		if( f_selectedChars.size() == 1 )
		{
			AddEffect( f_selectedChars[0], new_effect );
		}
		else
		{
			AddEffectToMultiple( new_effect );
		}
	}
}


void EffectsBook::OnEditClick()
{
	Actions::Desensitizer	ad;

	const int pagenum = get_current_page();
	assert( pagenum != -1 );

	Combatant::Character::Pointer ch = f_selectedChars[0];
	assert(ch);
	//
	Combatant::Character::Pointer newChar( new Combatant::Character );
	newChar->Copy( ch );
	assert(newChar);

	Effect::List	effects_list;
	ch->getEffects( effects_list );
	//
	const Glib::ustring	name = gettext("Edit Effect");
	EffectEditor		editor( name );
	Effect::Pointer	effect = effects_list[pagenum];
	//
	effect->unapply( newChar );
	//
	editor.set_transient_for( *f_parent );
	editor.SetEffect( effect );
	//
	if( editor.run() == Gtk::RESPONSE_OK )
	{
#ifdef DEBUG
		printf( "EffectBooks::OnEditClick(): effect %s, totalRounds=%d, roundsLeft=%d\n",
				effect->name().c_str(),
				effect->totalRounds(),
				effect->roundsLeft() );
#endif
		effect->apply( newChar, true /*force*/ );
		newChar->setEffects( effects_list );
		//
		TransactionGroupHelper group( name.c_str() );
		Transactions::Transaction::Pointer tr( new EditCharacterTransaction( ch, ch, newChar ) );
		group.AddTransaction( tr );
	}
}


void EffectsBook::DismissCurrentEffect( Combatant::Character::Pointer ch )
{
	const int pagenum = get_current_page();
	assert( pagenum != -1 );
	assert( ch );

	TransactionGroupHelper group( gettext("Delete Effect" ) );

	Effect::List	effects_list;
	ch->getEffects( effects_list );
	//
	Combatant::Character::Pointer newChar( new Combatant::Character );
	newChar->Copy( ch );
	//
	const auto iter = effects_list.begin() + pagenum;
	assert(iter == effects_list.end());
	//
	Effect::Pointer effect = *iter;
	assert(effect);
	effect->unapply( newChar );
	//
	effects_list.erase( iter );
	//
	newChar->setEffects( effects_list );
	//
	Transactions::Transaction::Pointer tr( new EditCharacterTransaction( ch, ch, newChar ) );
	group.AddTransaction( tr );
}


void EffectsBook::DismissAllEffects()
{
	TransactionGroupHelper group( gettext("Delete All Effects") );
	//
	//Effect::List	empty_effects;
	//
    for( auto ch : f_selectedChars )
	{
		assert(ch);

		Combatant::Character::Pointer newChar( new Combatant::Character );
		newChar->Copy( ch );

		Effect::List	effect_list;
		ch->getEffects( effect_list );
		//
		for( auto effect : effect_list )
		{
			assert(effect);
			effect->unapply( newChar );
		}
		//
		effect_list.clear();
		//
		newChar->setEffects( effect_list );
		//
		Transactions::Transaction::Pointer tr( new EditCharacterTransaction( ch, ch, newChar ) );
		group.AddTransaction( tr );
	}
}


void EffectsBook::OnDismissClick()
{
	if( f_selectedChars.size() == 1 )
	{
		DismissCurrentEffect( f_selectedChars[0] );
	}
	else
	{
		DismissAllEffects();
	}
}


void EffectsBook::AddEmptyPage()
{
	Gtk::HBox* box = Gtk::manage(new Gtk::HBox);
	assert(box);
	//
	Gtk::Label* label = Gtk::manage(new Gtk::Label( " ", Gtk::ALIGN_LEFT ));
	assert(label);
	box->pack_start( *label, Gtk::PACK_EXPAND_WIDGET );
 	//
	append_page( *box, "No Effects" );
}


void EffectsBook::AddMultiCharPage()
{
	Gtk::HBox* box = Gtk::manage(new Gtk::HBox);
	assert(box);
	//
	Gtk::Label* label = Gtk::manage(new Gtk::Label( " ", Gtk::ALIGN_LEFT ));
	assert(label);
	box->pack_start( *label, Gtk::PACK_EXPAND_WIDGET );
	//
	append_page( *box, "Multiple Combatants Selected!" );
}


void EffectsBook::AddEffect( Effect::Pointer effect )
{
	assert(effect);
#ifdef DEBUG
	printf( "EffectsBooks::AddEffect(): adding effect %s\n", effect->name().c_str() );
#endif

	Glib::ustring type;
	switch( effect->type() )
	{
		case Effect::Spell  	: type = gettext("Spell");   break;
		case Effect::Ability	: type = gettext("Ability"); break;
		default             	: type = gettext("Other");   break;
	}

	const int BUFLEN = 512;
	char BUFFER[BUFLEN+1];
	const int startIn    = effect->startIn();
	const int roundsLeft = effect->roundsLeft();
	if( startIn > 0 )
	{
		snprintf( BUFFER, BUFLEN, gettext(" Effect starts in %d %s, Total rounds: %d, Type: %s, Description: %s"),
				startIn,
				startIn > 1? "rounds": "round",
				effect->totalRounds(),
				type.c_str(),
				effect->description().c_str() );
	}
	else
	{
		snprintf( BUFFER, BUFLEN, gettext(" %sRounds left: %d, Type: %s, Description: %s"),
				roundsLeft? "": gettext("**** Expired ****"),
				roundsLeft,
				type.c_str(),
				effect->description().c_str() );
	}

	Glib::ustring name = effect->name().c_str();
	if( !roundsLeft )
	{
		name = "(" + name + ")";
	}

	Gtk::HBox* box = Gtk::manage(new Gtk::HBox);
	assert(box);
	//
	Gtk::Label* label = Gtk::manage(new Gtk::Label( BUFFER, Gtk::ALIGN_LEFT ));
	assert(label);
	box->pack_start( *label, Gtk::PACK_EXPAND_WIDGET );
 	//
	append_page( *box, name );
}


void EffectsBook::UpdateEffects()
{
	Clear();

	if( f_selectedChars.size() == 1 )
	{
		Combatant::Character::Pointer ch( f_selectedChars[0] );
		assert( ch );
#ifdef DEBUG
		printf( "EffectsBooks::UpdateEffects(): updating character %s\n", ch->name().c_str() );
#endif

		Effect::List effect_list;
		ch->getEffects( effect_list );
	
		if( effect_list.empty() )
		{
			AddEmptyPage();
		}
		else
		{
			for( auto effect : effect_list )
			{
				assert(effect);
				AddEffect( effect );
			}
		}
	}

	show_all_children();
}


void EffectsBook::OnUpdateEffects()
{
	if( !is_visible() )	return;

	if( f_selectedChars.size() == 0 )
	{
		Clear();
	}
	else if( f_selectedChars.size() == 1 )
	{
		UpdateEffects();
	}
	else
	{
		Clear();
		AddMultiCharPage();
		show_all_children();
	}
}


void EffectsBook::OnUpdateCharacter( Combatant::Character::Pointer ch )
{
	f_connection.disconnect();
	f_selectedChars.clear();

	if( !is_visible() )	return;

	assert( ch );
	f_connection = ch->signal_changed().connect( sigc::mem_fun( *this, &EffectsBook::OnUpdateEffects ) );
	f_selectedChars.push_back( ch );

	OnUpdateEffects();
}


void EffectsBook::OnSelectionChanged( const Combatant::Character::List& selected_list )
{
	if( !f_selectedChars.empty() && (f_selectedChars == selected_list) )
	{
		// Already connected
		return;
	}

	// Disconnect in preparation for attachment below
	//
	f_selectedChars.clear();
	f_connection.disconnect();

	if( is_visible() )
	{
		f_selectedChars = selected_list;

		if( f_selectedChars.size() == 1 )
		{
			Combatant::Character::Pointer ch = f_selectedChars[0];
			assert( ch );
			f_connection = ch->signal_changed().connect( sigc::mem_fun( *this, &EffectsBook::OnUpdateEffects ) );
		}

		OnUpdateEffects();
	}
}


void EffectsBook::Clear()
{
	pages().clear();
}


void EffectsBook::OnSwitchPage( GtkNotebookPage*, guint )
{
	f_currentPage = get_current_page();
}


void EffectsBook::PrevTab()
{
	f_currentPage = f_currentPage? f_currentPage - 1: pages().size() - 1;
	set_current_page( f_currentPage );
}


void EffectsBook::NextTab()
{
	f_currentPage = f_currentPage < pages().size() - 1? f_currentPage + 1: 0;
	set_current_page( f_currentPage );
}


bool EffectsBook::on_key_press_event( GdkEventKey* event )
{
#ifdef DEBUG
	printf( "*** EffectsBook::on_key_press_event(): event->keyval=%x, event->state=%x\n", event->keyval, event->state );
#endif
	
	switch( event->keyval )
	{
	case GDK_A:
		if( event->state & GDK_SHIFT_MASK & GDK_CONTROL_MASK )
		{
			OnAddClick();
			return true;
		}
		break;

	case GDK_E:
		if( event->state & GDK_SHIFT_MASK & GDK_CONTROL_MASK )
		{
			OnEditClick();
			return true;
		}
		break;

	case GDK_X:
		if( event->state & GDK_SHIFT_MASK & GDK_CONTROL_MASK )
		{
			OnDismissClick();
			return true;
		}
		break;
	}
	
	return Gtk::Notebook::on_key_press_event(event);	
}
	
}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

