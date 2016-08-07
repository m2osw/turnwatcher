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

#include "common.h"
#include "effect.h"
#include "character.h"
#include "transaction.h"
#include "CharacterListUI.h"
#include "transactions/UITransactionBase.h"

#ifdef WANT_EFFECTS

namespace Transactions
{


class EffectHandler :
	public Transaction,
	public UITransactionBase
{
public:
	EffectHandler( Combatant::Character::Pointer ch );
	virtual void doit();
	virtual void undo();

private:
	Combatant::Character::Pointer	f_prevChar;
	Combatant::Character::Pointer	f_newChar;
	TransactionGroup		f_group;

	void ApplyEffect( Effects::Effect::Pointer effect );
};


}
// namespace Transactions

#endif // WANT_EFFECTS

// vim: ts=8 sw=8

