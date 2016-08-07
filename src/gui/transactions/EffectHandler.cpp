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




#include "transactions/EffectHandler.h"
#include "transactions/CharacterEntry.h"

#include <algorithm>
#include <sigc++/functors/mem_fun.h>

#ifdef WANT_EFFECTS

using namespace Effects;

namespace Transactions
{


EffectHandler::EffectHandler( Combatant::Character::Pointer ch ) :
	f_prevChar(ch),
	f_group(gettext("Handle Effects"))
{
	f_newChar.reset( new Combatant::Character );
	f_newChar->Copy( f_prevChar );

	// Now gets a copy automatically, not reference
	//
	Effect::List	effects;
	f_newChar->getEffects( effects );
	//
	std::for_each( effects.begin(), effects.end(), sigc::mem_fun( *this, &EffectHandler::ApplyEffect ) );
	//
	f_newChar->setEffects( effects );

	Transaction::Pointer tr( new EditCharacterTransaction( f_prevChar, f_prevChar, f_newChar ) );
	f_group.addTransaction( tr );
}


void EffectHandler::ApplyEffect( Effect::Pointer effect )
{
	if( effect->startIn() > 0 )
	{
		effect->startIn( effect->startIn() - 1 );
		effect->roundsUsed( 0 );
	}
	else
	{
		const int roundsUsed = effect->roundsUsed() + 1;
		const int roundsLeft = effect->totalRounds() - roundsUsed;
		if( roundsLeft >= 0 )
		{
			effect->roundsUsed( roundsUsed );
		}
	}

	effect->apply( f_newChar );
}


void EffectHandler::doit()
{
	f_group.doit();
}


void EffectHandler::undo()
{
	f_group.undo();
}	


}
// namespace Transactions

#endif // WANT_EFFECTS

// vim: ts=8 sw=8

