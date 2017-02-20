//===============================================================================
// Copyright (c) 2005-2017 by Made to Order Software Corporation
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

#include "effect.h"
#include "character.h"

using namespace molib;

namespace Effects
{

namespace
{
	const moWCString g_name        ("NAME");
	const moWCString g_description ("DESCRIPTION");
	const moWCString g_totalRounds ("TOTALROUNDS");
	const moWCString g_startIn     ("STARTIN");
	const moWCString g_tempHP      ("TEMPHP");
	const moWCString g_hpBoost     ("HPBOOST");
	const moWCString g_roundsUsed  ("ROUNDSUSED");
	const moWCString g_type        ("TYPE");
	const moWCString g_isActive    ("ACTIVE");
}


Effect::Effect() :
	f_totalRounds(10),
	f_roundsUsed(0),
	f_startIn(0),
	f_tempHP(0),
	f_hpBoost(0),
	f_type(Spell),
	f_isActive(false)
{
}


void	Effect::Load( moPropBagRef propBag )
{
	moPropStringRef name        ( g_name        ); // name of effect
	moPropStringRef description ( g_description ); // description of the effect
	moPropIntRef    totalRounds ( g_totalRounds ); // number of rounds the effect lasts
	moPropIntRef    startIn     ( g_startIn     ); // number of rounds until start of effect
	moPropIntRef    tempHP      ( g_tempHP      ); // temp hp to apply
	moPropIntRef    hpBoost     ( g_hpBoost     ); // boost to hp
	moPropIntRef    roundsUsed  ( g_roundsUsed  ); // number of rounds left on the effect
	moPropIntRef    type        ( g_type        ); // true if spell effect
	moPropIntRef    isActive    ( g_isActive    ); // true if effect is running

	name.Link(        propBag ); if( name.HasProp()        ) { f_name        = static_cast<moWCString>(name).c_str(); }
	description.Link( propBag ); if( description.HasProp() ) { f_description = static_cast<moWCString>(description).c_str(); }
	totalRounds.Link( propBag ); if( totalRounds.HasProp() ) { f_totalRounds = totalRounds; }
	startIn.Link(     propBag ); if( startIn.HasProp()     ) { f_startIn     = startIn;     }
	tempHP.Link(      propBag ); if( tempHP.HasProp()      ) { f_tempHP      = tempHP;      }
	hpBoost.Link(     propBag ); if( hpBoost.HasProp()     ) { f_hpBoost     = hpBoost;     }
	roundsUsed.Link(  propBag ); if( roundsUsed.HasProp()  ) { f_roundsUsed  = roundsUsed;  }
	type.Link(        propBag ); if( type.HasProp()        ) { f_type        = static_cast<Type>(static_cast<int>(type)); }
	isActive.Link(    propBag ); if( isActive.HasProp()    ) { f_isActive    = static_cast<bool>(isActive); }
}


void	Effect::Save( moPropBagRef propBag )
{
	moPropStringRef	name   		( g_name		);	// name of effect
	moPropStringRef	description	( g_description	);	// description of the effect
	moPropIntRef	totalRounds	( g_totalRounds	);	// number of rounds the effect lasts
	moPropIntRef	startIn		( g_startIn		);	// number of rounds until the effect starts
	moPropIntRef	tempHP		( g_tempHP		);	// temporary hp to apply
	moPropIntRef	hpBoost		( g_hpBoost		);	// boost to HP
	moPropIntRef	roundsUsed	( g_roundsUsed	);	// number of rounds left on the effect
	moPropIntRef	type		( g_type		);	// true if spell effect
	moPropIntRef	isActive	( g_isActive	);	// true if effect is active

	name.NewProp();        name        = f_name.c_str();         propBag += name;
	description.NewProp(); description = f_description.c_str();  propBag += description;
	totalRounds.NewProp(); totalRounds = f_totalRounds;          propBag += totalRounds;
	startIn.NewProp();     startIn     = f_startIn;              propBag += startIn;
	hpBoost.NewProp();     hpBoost     = f_hpBoost;              propBag += hpBoost;
	roundsUsed.NewProp();  roundsUsed  = f_roundsUsed;           propBag += roundsUsed;
	type.NewProp();        type        = (int) f_type;           propBag += type;
	isActive.NewProp();    isActive    = (int) f_isActive? 1: 0; propBag += isActive;
}


void Effect::Copy( Effect::pointer_t effect )
{
	*this = *effect;
}


void Effect::apply( Combatant::Character::pointer_t ch, const bool force )
{
	if( !force )
	{
		if( f_startIn > 0 )
		{
			// Not ready to apply the effect yet
			return;
		}

		if( f_roundsUsed > 0 )
		{
			if( roundsLeft() == 0 )
			{
				// Unapply the effect
				//
				unapply( ch );
			}

			// Already applied
			//
			return;
		}
	}

	// Apply the effect, since it has just started
	//
	internalApply( ch );
}


void Effect::unapply( Combatant::Character::pointer_t ch )
{
	internalUnapply( ch );
}


void Effect::internalApply( Combatant::Character::pointer_t ch )
{
	if( f_tempHP > 0 )
	{
		const int tempHP = ch->tempHP() + f_tempHP;
		ch->tempHP( tempHP );
	}

	if( f_hpBoost > 0 )
	{
		const int baseHP = ch->baseHP() + f_hpBoost;
		ch->baseHP( baseHP );
	}

	f_isActive = true;
}


void Effect::internalUnapply( Combatant::Character::pointer_t ch )
{
	if( f_tempHP > 0 )
	{
		const int tempHP = ch->tempHP() - f_tempHP;
		ch->tempHP( tempHP > 0? tempHP: 0 );
	}

	if( f_hpBoost > 0 )
	{
		const int baseHP = ch->baseHP() - f_hpBoost;
		ch->baseHP( baseHP );
	}

	f_isActive = false;
}


}
// namespace Effects


// vim: ts=4 sw=4 noet syntax=cpp.doxygen

