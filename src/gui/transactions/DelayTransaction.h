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
#include "transaction.h"
#include "CharacterListUI.h"
#include "transactions/UITransactionBase.h"

namespace Transactions
{


class DelayTransactionBase :
	public Transaction,
	public UITransactionBase
{
public:
	DelayTransactionBase(	Combatant::Character::Pointer character,
				Combatant::Status status,
			    	Combatant::Status new_status );

protected:
	void delay();
	void undelay();

private:
	Combatant::Status		f_status;
	Combatant::Status		f_newStatus;
	Combatant::Character::Pointer	f_character;
};



class DelayTransaction :
	public DelayTransactionBase
{
public:
	DelayTransaction(	Combatant::Character::Pointer character,
				Combatant::Status new_status = Combatant::Delayed ) :
		DelayTransactionBase( character, character->status(), new_status ) {}

	virtual void doit() { delay(); } 
	virtual void undo() { undelay(); }
};


class UndelayTransaction :
		public DelayTransactionBase
{
public:
	UndelayTransaction( Combatant::Character::Pointer character ) :
		DelayTransactionBase( character, Combatant::Normal, character->status() ) {}

	virtual void doit() { undelay(); }
	virtual void undo() { delay(); }
};

}
// namespace Transactions

// vim: ts=8 sw=8

