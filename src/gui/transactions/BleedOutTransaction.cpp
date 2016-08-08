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




#include "transactions/BleedOutTransaction.h"

namespace Transactions
{


BleedOutTransaction::BleedOutTransaction( Combatant::Character::pointer_t ch )
	: f_character(ch)
{
	const bool bleedOutDying = GetAppSettings().lock()->BleedOutDying();
	const bool justdropped = f_character->justdropped();
	f_prevJustDropped = justdropped;
	f_prevDamage = f_character->damage();
	f_newDamage  = f_character->damage() + (bleedOutDying? 1: 0);
#ifdef DEBUG
	printf( "BleedOutTransaction::BleedOutTransaction(): bleedOutDying=%d, prevDamage=%d, newDamage=%d\n",
			bleedOutDying, f_prevDamage, f_newDamage );
#endif
}

void BleedOutTransaction::doit()
{
	f_character->damage( f_newDamage );
	f_character->justdropped( false );
	f_character->signal_changed().emit();
}

void BleedOutTransaction::undo()
{
	f_character->damage( f_prevDamage );
	f_character->justdropped( f_prevJustDropped );
	f_character->signal_changed().emit();
}	

}
// namespace Transactions

// vim: ts=8 sw=8

