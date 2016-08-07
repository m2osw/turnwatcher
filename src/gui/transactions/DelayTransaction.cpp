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




#include "transactions/DelayTransaction.h"

namespace Transactions
{

DelayTransactionBase::DelayTransactionBase(
			Combatant::Character::Pointer character,
			Combatant::Status status,
			Combatant::Status new_status ) :
	f_character(character),
	f_status(status),
	f_newStatus(new_status)
{
}

void DelayTransactionBase::delay()
{
	f_character->status( f_newStatus );
	f_character->signal_changed().emit();
}

void DelayTransactionBase::undelay()
{
	f_character->status( f_status );
	f_character->signal_changed().emit();
}

}
// namespace Transactions

// vim: ts=8 sw=8

