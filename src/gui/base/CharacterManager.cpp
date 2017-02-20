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


#include "base/CharacterManager.h"
#include "base/AppSettings.h"
#include "base/StatManager.h"
#include "transactions/CharacterEntry.h"

using namespace molib;
using namespace Attribute;

namespace Combatant
{

CharacterManager::private_pointer_t CharacterManager::f_instance;


CharacterManager::CharacterManager()
	: f_propBagName ( "CHARACTERS" )
	, f_arrayBagName( "CHARACTERS" )
{
	if( !Load() )
	{
		LoadDemoCharacters();
	}
}


CharacterManager::~CharacterManager()
{
}


void CharacterManager::Release()
{
	f_instance.reset();
}


CharacterManager::pointer_t CharacterManager::Instance()
{
	if( !f_instance )
	{
        f_instance.reset( new CharacterManager );
	}
	return f_instance;
}


bool CharacterManager::Load()
{
	f_chars.clear();
	moPropBagRef propBag( f_propBagName );
	if( !Common::LoadBagFromFile( "characters.conf", propBag ) ) return false;
	LoadCharacters( propBag );
	return true;
}


bool CharacterManager::Save()
{
	moPropBagRef propBag( f_propBagName );
	propBag.NewProp();
	SaveCharacters( propBag );
	return Common::SaveBagToFile( "characters.conf", propBag );
}


void CharacterManager::LoadDemoCharacters()
{
	auto statMgr( StatManager::Instance().lock() );
	assert(statMgr);

	const int	initId  = statMgr->initId();
	const int	spotId	= statMgr->spotId();
	const int	listenId= statMgr->listenId();
	const int	willId	= statMgr->willId();
	const int	levelId	= statMgr->levelId();

	Combatant::Character::pointer_t ch( new Combatant::Character() );
	ch->name( "Errol" );
	ch->monster( false );
	ch->baseHP( 36 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 2 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 4 );
	ch->setMod( levelId, 5 );
	f_chars.push_back( ch );
	//
	ch.reset( new Combatant::Character() );
	ch->name( "Stubs" );
	ch->monster( false );
	ch->baseHP( 45 );
	ch->setMod( initId, 1 );
	ch->setMod( spotId, -1 );
	ch->setMod( listenId, -1 );
	ch->setMod( willId, 5 );
	ch->setMod( levelId, 5 );
	Effects::Effect::pointer_t effect( new Effects::Effect );
	effect->name( "Bull Strength" );
	effect->description( "Gives a +4 to strength" );
	effect->totalRounds( 110 );
	ch->addEffect( effect );
	f_chars.push_back( ch );
	//
	ch.reset( new Combatant::Character() );
	ch->name( "Ariel" );
	ch->monster( false );
	ch->baseHP( 24 );
	ch->setMod( initId, 1 );
	ch->setMod( spotId, 1 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 4 );
	ch->setMod( levelId, 5 );
	f_chars.push_back( ch );
	//
	ch.reset( new Combatant::Character() );
	ch->name( "K the Knife" );
	ch->monster( false );
	ch->baseHP( 37 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 4 );
	ch->setMod( listenId, 4 );
	ch->setMod( willId, 6 );
	ch->setMod( levelId, 5 );
	f_chars.push_back( ch );
	//
	ch.reset( new Combatant::Character() );
	ch->name( "Sinclair" );
	ch->monster( true );
	ch->baseHP( 35 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 2 );
	ch->setMod( listenId, 2 );
	ch->setMod( willId, 8 );
	ch->setMod( levelId, 5 );
	f_chars.push_back( ch );
	//
	ch.reset( new Combatant::Character() );
	ch->name( "Dench" );
	ch->monster( true );
	ch->baseHP( 90 );
	ch->setMod( initId, 3 );
	ch->setMod( spotId, 1 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 6 );
	ch->setMod( levelId, 5 );
	f_chars.push_back( ch );
}


void CharacterManager::LoadCharacters( moPropBagRef& charBag, Transactions::TransactionGroup::pointer_t group, const bool emit_signals )
{
	moPropArrayRef	array(f_arrayBagName);
	array.Link( charBag );
    assert(array.HasProp());

	const int count = array.CountIndexes();
	for( int idx = 0; idx < count; ++idx )
	{
		const int 		item_no  ( array.ItemNoAtIndex( idx ) );
		moPropSPtr		prop_ptr ( array.Get( item_no ) );
		moName			name	 ( prop_ptr->GetName()  );
		moPropBagRef	propBag(name);
		propBag.NewProp();
		propBag.GetProperty()->Copy( *prop_ptr );

		Combatant::Character::pointer_t ch( new Combatant::Character() );
		ch->Load( propBag );

		// Add "(copy)" in case of duplicates
		//
		Combatant::Character::pointer_t dup = FindDuplicates( ch );
		//
		while( dup )
		{
			ch->name( ch->name() + gettext(" (copy)") );
			dup = FindDuplicates( ch );
		}

		if( group )
		{
			// Add character via transaction.
			//
			Transactions::Transaction::pointer_t tr( new Transactions::AddCharacterTransaction( ch, emit_signals ) );
			tr->doit();
			group->addTransaction( tr );
		}
		else
		{
			// Add character directly
			//
			f_chars.push_back( ch );
		}
	}
}


void CharacterManager::SaveCharacters( moPropBagRef& charBag )
{
	moPropArrayRef	array("CHARACTERS");
	array.NewProp();

	int array_index = 0;
	for( auto ch : f_chars )
	{
		moPropBagRef propBag( "CHARACTER" );
		propBag.NewProp();
		ch->Save( propBag );
		array.Set( array_index++, propBag );
	}

	charBag += array;
}


void CharacterManager::Insert( Combatant::Character::pointer_t ch, const bool signal )
{
	ch->deleted( false );
	//
	if( FindDuplicates( ch ) == 0 )
	{
		f_chars.push_back( ch );
		//
		if( signal )
		{
			f_changedSignal.emit();
			f_characterAddedSignal.emit( ch );
		}
	}
}


void CharacterManager::Remove( Combatant::Character::pointer_t ch, const bool signal )
{
	ch->deleted( true );
	//
	const auto found_iter = FindCharacterIter( ch->name() );
	//
	if( found_iter != f_chars.end() )
	{
		Remove( found_iter, ch, signal );
	}
}


void CharacterManager::Remove( Character::list_t::iterator iter, Combatant::Character::pointer_t ch, const bool signal )
{
	f_chars.erase( iter );
	//
	if( signal )
	{
		f_changedSignal.emit();
		f_characterRemovedSignal.emit( ch );
	}
}


void CharacterManager::Clear( const bool signal )
{
	if( signal )
	{
		for( auto ch : f_chars )
		{
			if( ch )
			{
				// Emit signal to show that the character has been removed from the list
				//
				f_characterRemovedSignal.emit( ch );
			}
		}
	}

	// Alexis, you're right; that loop was waaaay to complicated. The point was to
	// emit signals for each character removed from the manager so other UI objects
	// could react properly. :-)
	//
	PermanentClear();

	if( signal )
	{
		f_changedSignal.emit();
		f_clearedSignal.emit();
	}
}


void CharacterManager::PermanentClear()
{
	f_chars.clear();
}


Combatant::Character::pointer_t CharacterManager::FindDuplicates( Combatant::Character::pointer_t newChar )
{
	Combatant::Character::pointer_t dupChar;
	//
	for( auto ch : f_chars )
	{
		if( ch->name() == newChar->name() )
		{
			dupChar = ch;
			break;
		}
	}

	return dupChar;
}


Combatant::Character::pointer_t CharacterManager::FindCharacter( const Glib::ustring& name )
{
	Combatant::Character::pointer_t found_char;
	const auto found_iter = FindCharacterIter( name );
	if( f_chars.end() != found_iter )
	{
		found_char = *found_iter;
	}

	return found_char;
}


Combatant::Character::list_t::iterator CharacterManager::FindCharacterIter( const Glib::ustring& name )
{
	auto		iter       = f_chars.begin();
	const auto	end        = f_chars.end();
	auto		found_iter = end;
	for( ; iter != end; ++iter )
	{
		auto ch( *iter );
		if( ch->name() == name )
		{
			found_iter = iter;
			break;
		}
	}

	return found_iter;
}


void CharacterManager::SetLegacyCharacters( const LegacyCharacter::list_t& lchars, Transactions::TransactionGroup::pointer_t group, const bool emit_signals )
{
    Transactions::Transaction::pointer_t tr;

    if( emit_signals )
    {
        tr.reset( new Transactions::SignalCharactersTransaction );
        group->addTransaction( tr );
    }

    tr.reset( new Transactions::ClearCharactersTransaction( false /*emit_signals*/ ) );
	group->addTransaction( tr );

	for( auto lch : lchars )
	{
		Combatant::Character::pointer_t ch( new Combatant::Character );
		ch->Copy( lch );
		//
        tr.reset( new Transactions::AddCharacterTransaction( ch, false /*emit_signals*/ ) );
		group->addTransaction( tr );
	}

    if( emit_signals )
    {
        tr.reset( new Transactions::SignalCharactersTransaction );
        group->addTransaction( tr );
    }
}


}
// namespace UI


// vim: ts=4 sw=4 noet syntax=cpp.doxygen

