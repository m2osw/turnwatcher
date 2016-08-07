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

#include "transactions/InitiativeBase.h"

namespace Transactions
{

class CharacterEntryTransaction :
	public InitiativeBase
{
protected:
	Combatant::Character::Pointer		f_character;
	TransactionGroup::Pointer		f_group;

	CharacterEntryTransaction( Combatant::Character::Pointer character );

	void localStartInit();
	void localStopInit();

	void insertChar( Combatant::Character::Pointer ch );
	void removeChar( Combatant::Character::Pointer ch );
};


class AddCharacterTransaction :
	public Transaction,
	public CharacterEntryTransaction
{
public:
	AddCharacterTransaction( Combatant::Character::Pointer character );
	virtual void doit();
	virtual void undo();
};


class RemoveCharacterTransaction :
	public Transaction,
	public CharacterEntryTransaction
{
public:
	RemoveCharacterTransaction( Combatant::Character::Pointer character );

	virtual void doit();
	virtual void undo();
};


class EditCharacterTransaction :
	public Transaction,
	public CharacterEntryTransaction
{
	Combatant::Character::Pointer	f_prevVal;
	Combatant::Character::Pointer	f_newVal;

public:
	EditCharacterTransaction(
			Combatant::Character::Pointer ch,
			Combatant::Character::Pointer prevVal,
			Combatant::Character::Pointer newVal );

	virtual void doit();
	virtual void undo();
};



class RemoveSelectedTransaction :
	public Transaction,
	public CharacterEntryTransaction
{
public:
	RemoveSelectedTransaction( const Combatant::Character::List& selected_chars );

	virtual void doit();
	virtual void undo();

private:
	Combatant::Character::List	f_selectedChars;
	TransactionGroup		f_group;
};


class ClearCharactersTransaction :
	public Transaction,
	public UITransactionBase
{
public:
	ClearCharactersTransaction();

	virtual void doit();
	virtual void undo();

private:
	TransactionGroup	f_group;
};


}
// namespace Transactions

// vim: ts=8 sw=8

