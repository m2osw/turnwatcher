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




#include "transactions/CharacterEntry.h"
#include "transactions/MoveCharacterInInitiative.h"

namespace Transactions
{


//===============================================================================
// CharacterEntryTransaction
//===============================================================================

CharacterEntryTransaction::CharacterEntryTransaction( Combatant::Character::Pointer character ) :
	f_character(character)
{
}


void CharacterEntryTransaction::localStartInit()
{
	if( f_inRounds )
	{
		startInit();
	}
}


void CharacterEntryTransaction::localStopInit()
{
	if( f_inRounds )
	{
		stopInit();
	}
}


void CharacterEntryTransaction::insertChar( Combatant::Character::Pointer ch )
{
	GetCharacterMgr().lock()->Insert( ch );
	GetAppSettings().lock()->Modified( true );
}

void CharacterEntryTransaction::removeChar( Combatant::Character::Pointer ch )
{
	GetCharacterMgr().lock()->Remove( ch );
	GetAppSettings().lock()->Modified( true );
}


//===============================================================================
// AddCharacterTransaction
//===============================================================================


AddCharacterTransaction::AddCharacterTransaction( Combatant::Character::Pointer character ) :
	CharacterEntryTransaction( character )//,
	//f_firstTime(true)
{
	f_character->makeAllRolls();
}

void AddCharacterTransaction::doit()
{
	localStopInit();
	swapInitData( f_currInitData );
	insertChar( f_character );
	updateInitCharacters();
	localStartInit();
}

void AddCharacterTransaction::undo()
{
	localStopInit();
	swapInitData( f_prevInitData );
	removeChar( f_character );
	updateInitCharacters();
	localStartInit();
}


//===============================================================================
// RemoveCharacterTransaction
//===============================================================================


RemoveCharacterTransaction::RemoveCharacterTransaction( Combatant::Character::Pointer character ) :
	CharacterEntryTransaction( character )//,
	//f_firstTime(true)
{
}


void RemoveCharacterTransaction::doit()
{
	localStopInit();
	swapInitData( f_currInitData );
	removeChar( f_character );
	updateInitCharacters();
	localStartInit();
}

void RemoveCharacterTransaction::undo()
{
	localStopInit();
	swapInitData( f_prevInitData );
	insertChar( f_character );
	updateInitCharacters();
	localStartInit();
}


//===============================================================================
// EditCharacterTransaction
//===============================================================================


EditCharacterTransaction::EditCharacterTransaction(
		Combatant::Character::Pointer ch, Combatant::Character::Pointer prevVal, Combatant::Character::Pointer newVal )
	: CharacterEntryTransaction( ch )
	, f_prevVal(0)
	, f_newVal (0)
{
	// Make separate copies
	//
	f_prevVal .reset( new Combatant::Character );	f_prevVal->Copy( prevVal );
	f_newVal  .reset( new Combatant::Character );	f_newVal ->Copy( newVal  );
}

void EditCharacterTransaction::doit()
{
	f_character->Copy( f_newVal );
	f_character->signal_changed().emit();
	GetAppSettings().lock()->Modified( true );
}

void EditCharacterTransaction::undo()
{
	f_character->Copy( f_prevVal );
	f_character->signal_changed().emit();
	GetAppSettings().lock()->Modified( true );
}


//===============================================================================
// RemoveSelectedTransaction
//===============================================================================

RemoveSelectedTransaction::RemoveSelectedTransaction( const Combatant::Character::List& selected_chars )
	: CharacterEntryTransaction( NULL )
	, f_selectedChars( selected_chars )
	, f_group( gettext("Remove Character Group") )
{
	// Empty
}

void RemoveSelectedTransaction::doit()
{
	//f_group.doit();

	localStopInit();
	swapInitData( f_currInitData );

	for( auto ch : f_selectedChars )
	{
		removeChar( ch );
	}

	updateInitCharacters();
	localStartInit();
}

void RemoveSelectedTransaction::undo()
{
	//f_group.undo();

	localStopInit();
	swapInitData( f_prevInitData );

	for( auto ch : f_selectedChars )
	{
		insertChar( ch );
	}

	updateInitCharacters();
	localStartInit();
}


//===============================================================================
// ClearCharactersTransaction
//===============================================================================


ClearCharactersTransaction::ClearCharactersTransaction()
	: f_group(gettext("Clear Out All Characters"))
{
	const Combatant::Character::List& chars( GetCharacterMgr().lock()->GetCharacters() );
	Transaction::Pointer tr( new RemoveSelectedTransaction( chars ) );
	f_group.addTransaction( tr );
}

void ClearCharactersTransaction::doit()
{
	f_group.doit();
	GetCharacterMgr().lock()->signal_cleared().emit();
}

void ClearCharactersTransaction::undo()
{
	f_group.undo();
	//GetCharacterMgr().lock()->signal_cleared().emit();
}



}
// namespace Transactions

// vim: ts=8 sw=8

