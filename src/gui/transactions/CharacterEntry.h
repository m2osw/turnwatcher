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

#pragma once

#include "character.h"
#include "transactions/InitiativeBase.h"

namespace Transactions
{

class CharacterEntryTransaction
		: public InitiativeBase
{
protected:
		Combatant::Character::pointer_t			f_character;
		TransactionGroup::pointer_t				f_group;

		CharacterEntryTransaction( Combatant::Character::pointer_t character, const bool emit_signals = true );

		void localStartInit();
		void localStopInit();

		void insertChar( Combatant::Character::pointer_t ch );
		void removeChar( Combatant::Character::pointer_t ch );
};


class AddCharacterTransaction
		: public Transaction
		, public CharacterEntryTransaction
{
public:
		AddCharacterTransaction( Combatant::Character::pointer_t character, const bool emit_signals = true );
		virtual void doit();
		virtual void undo();
};


class RemoveCharacterTransaction
		: public Transaction
		, public CharacterEntryTransaction
{
public:
		RemoveCharacterTransaction( Combatant::Character::pointer_t character, const bool emit_signals = true );

		virtual void doit();
		virtual void undo();
};


class EditCharacterTransaction
		: public Transaction
		, public CharacterEntryTransaction
{
		Combatant::Character::pointer_t	f_prevVal;
		Combatant::Character::pointer_t	f_newVal;

public:
		EditCharacterTransaction( Combatant::Character::pointer_t ch,
								  Combatant::Character::pointer_t prevVal,
								  Combatant::Character::pointer_t newVal,
								  const bool emit_signals = true );

		virtual void doit();
		virtual void undo();
};



class RemoveSelectedTransaction
        : public Transaction
		, public CharacterEntryTransaction
{
public:
		RemoveSelectedTransaction( const Combatant::Character::list_t& selected_chars, const bool emit_signals = true );

		virtual void doit();
		virtual void undo();

private:
		Combatant::Character::list_t		f_selectedChars;
		TransactionGroup				f_group;
};


class ClearCharactersTransaction
		: public Transaction
		, public UITransactionBase
{
public:
		ClearCharactersTransaction( const bool emit_signals = true );

		virtual void doit();
		virtual void undo();

private:
		TransactionGroup		f_group;
};


class SignalCharactersTransaction
	: public Transaction
{
public:
	SignalCharactersTransaction() {}

	virtual void doit();
	virtual void undo();
};


}
// namespace Transactions

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
