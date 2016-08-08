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
#include "CharacterModel.h"

using namespace Combatant;

namespace Transactions
{


//===============================================================================
// CharacterEntryTransaction
//===============================================================================

CharacterEntryTransaction::CharacterEntryTransaction( Character::pointer_t character, const bool emit_signals )
	: InitiativeBase( emit_signals )
	, f_character(character)
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


void CharacterEntryTransaction::insertChar( Character::pointer_t ch )
{
	GetCharacterMgr().lock()->Insert( ch, f_emitSignals );
	GetAppSettings().lock()->Modified( true );
}

void CharacterEntryTransaction::removeChar( Character::pointer_t ch )
{
	GetCharacterMgr().lock()->Remove( ch, f_emitSignals );
	GetAppSettings().lock()->Modified( true );
}


//===============================================================================
// AddCharacterTransaction
//===============================================================================


AddCharacterTransaction::AddCharacterTransaction( Character::pointer_t character, const bool emit_signals )
	: CharacterEntryTransaction( character, emit_signals )
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


RemoveCharacterTransaction::RemoveCharacterTransaction( Character::pointer_t character, const bool emit_signals )
	: CharacterEntryTransaction( character, emit_signals )
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


EditCharacterTransaction::EditCharacterTransaction
        ( Character::pointer_t ch
        , Character::pointer_t prevVal
        , Character::pointer_t newVal
		, const bool emit_signals
		)
	: CharacterEntryTransaction( ch, emit_signals )
	, f_prevVal(0)
	, f_newVal (0)
{
	// Make separate copies
	//
    f_prevVal .reset( new Character );	f_prevVal->Copy( prevVal );
	f_newVal  .reset( new Character );	f_newVal ->Copy( newVal  );
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

RemoveSelectedTransaction::RemoveSelectedTransaction( const Character::list_t& selected_chars, const bool emit_signals )
	: CharacterEntryTransaction( NULL, emit_signals )
	, f_selectedChars( selected_chars )
	, f_group( gettext("Remove Character Group") )
{
	// Empty
}

void RemoveSelectedTransaction::doit()
{
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


ClearCharactersTransaction::ClearCharactersTransaction( const bool emit_signals )
	: f_group(gettext("Clear Out All Characters"))
{
    const Character::list_t& chars( GetCharacterMgr().lock()->GetCharacters() );
    Transaction::pointer_t tr( new RemoveSelectedTransaction( chars, emit_signals ) );
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
}


//===============================================================================
// SignalCharactersTransaction
//===============================================================================
void SignalCharactersTransaction::doit()
{
    CharacterModel::Instance().lock()->Refresh();
}


void SignalCharactersTransaction::undo()
{
    CharacterModel::Instance().lock()->Refresh();
}


}
// namespace Transactions

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
