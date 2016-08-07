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




#include "transactions/DamageTransaction.h"

namespace Transactions
{


DamageTransaction::DamageTransaction( Combatant::Character::Pointer character, const int damage ) :
	f_character(character)
{
	f_damageAmount = damage;
	f_newDamage  = f_character->damage() + damage;
	f_oldDamage  = f_character->damage();
	f_tempHP     = f_character->tempHP();
	f_stabilized = f_character->stabilized();
	f_healing    = (damage <= 0);
}


void DamageTransaction::doit()
{
	int damage = f_newDamage;

	if( !f_healing && f_tempHP > 0 )
	{
		if( f_tempHP > f_damageAmount )
		{
			f_character->tempHP( f_tempHP - f_damageAmount );
			damage = f_oldDamage;	// Leave damage alone, only take away from temp hitpoints
		}
		else
		{
			damage -= f_tempHP;	// Take tempHP out of damage, then zero it out
			f_character->tempHP( 0 );
		}
	}

	const bool not_dying_before = f_character->health() != Combatant::Dying;
	f_character->justdropped( false );
	//
	f_character->damage( damage );
	//
	switch( f_character->health() )
	{
		case Combatant::Dying:
			f_character->justdropped( not_dying_before );
			break;

		case Combatant::Stabilized:
			f_character->stabilized( f_healing );
			f_character->justdropped( !f_healing );
			break;
		default:
			break;
	}
	//
	//f_charList->updateCharacter( f_charList->characterToIter( f_character ), f_character );
	f_character->signal_changed().emit();
}

void DamageTransaction::undo()
{
	f_character->stabilized( f_stabilized );
	f_character->damage( f_oldDamage );
	f_character->tempHP( f_tempHP );
	f_character->justdropped( false /*f_oldJustDropped*/ );
	//f_charList->updateCharacter( f_charList->characterToIter( f_character ), f_character );
	f_character->signal_changed().emit();
}

}
// namespace Transactions

// vim: ts=8 sw=8

