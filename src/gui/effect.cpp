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




#include "effect.h"
#include "character.h"

namespace Effects
{

namespace
{
	const molib::moWCString g_name        ("NAME");
	const molib::moWCString g_description ("DESCRIPTION");
	const molib::moWCString g_totalRounds ("TOTALROUNDS");
	const molib::moWCString g_startIn     ("STARTIN");
	const molib::moWCString g_tempHP      ("TEMPHP");
	const molib::moWCString g_hpBoost     ("HPBOOST");
	const molib::moWCString g_roundsUsed  ("ROUNDSUSED");
	const molib::moWCString g_type        ("TYPE");
	const molib::moWCString g_isActive    ("ACTIVE");
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


void	Effect::Load( molib::moPropBagRef propBag )
{
	molib::moPropStringRef _name        ( g_name        ); // name of effect
	molib::moPropStringRef _description ( g_description ); // description of the effect
	molib::moPropIntRef    _totalRounds ( g_totalRounds ); // number of rounds the effect lasts
	molib::moPropIntRef    _startIn     ( g_startIn     ); // number of rounds until start of effect
	molib::moPropIntRef    _tempHP      ( g_tempHP      ); // temp hp to apply
	molib::moPropIntRef    _hpBoost     ( g_hpBoost     ); // boost to hp
	molib::moPropIntRef    _roundsUsed  ( g_roundsUsed  ); // number of rounds left on the effect
	molib::moPropIntRef    _type        ( g_type        ); // true if spell effect
	molib::moPropIntRef    _isActive    ( g_isActive    ); // true if effect is running

	_name.Link( propBag );
	if( _name.HasProp() )		{ molib::moWCString str = _name; f_name = str.c_str(); }

	_description.Link( propBag );
	if( _description.HasProp() )	{ molib::moWCString str = _description; f_description = str.c_str(); }

	_totalRounds.Link( propBag );
	if( _totalRounds.HasProp() )	{ f_totalRounds = _totalRounds; }

	_startIn.Link( propBag );
	if( _startIn.HasProp() )	{ f_startIn = _startIn; }

	_tempHP.Link( propBag );
	if( _tempHP.HasProp() )		{ f_tempHP = _tempHP; }

	_hpBoost.Link( propBag );
	if( _hpBoost.HasProp() )	{ f_hpBoost = _hpBoost; }

	_roundsUsed.Link( propBag );
	if( _roundsUsed.HasProp() )	{ f_roundsUsed = _roundsUsed; }

	_type.Link( propBag );
	if( _type.HasProp() )		{ f_type = (Type) (int) _type; }

	_isActive.Link( propBag );
	if( _isActive.HasProp() )	{ f_isActive = (bool) _isActive? true: false; }
}


void	Effect::Save( molib::moPropBagRef propBag )
{
	molib::moPropStringRef	_name   		( g_name	);	// name of effect
	molib::moPropStringRef	_description	( g_description	);	// description of the effect
	molib::moPropIntRef		_totalRounds	( g_totalRounds	);	// number of rounds the effect lasts
	molib::moPropIntRef		_startIn		( g_startIn	);	// number of rounds until the effect starts
	molib::moPropIntRef		_tempHP			( g_tempHP	);	// temporary hp to apply
	molib::moPropIntRef		_hpBoost		( g_hpBoost	);	// boost to HP
	molib::moPropIntRef		_roundsUsed		( g_roundsUsed	);	// number of rounds left on the effect
	molib::moPropIntRef		_type			( g_type	);	// true if spell effect
	molib::moPropIntRef		_isActive		( g_isActive	);	// true if effect is active

	_name.NewProp();        _name        = f_name.c_str();         propBag += _name;
	_description.NewProp(); _description = f_description.c_str();  propBag += _description;
	_totalRounds.NewProp(); _totalRounds = f_totalRounds;          propBag += _totalRounds;
	_startIn.NewProp();     _startIn     = f_startIn;              propBag += _startIn;
	_hpBoost.NewProp();     _hpBoost     = f_hpBoost;              propBag += _hpBoost;
	_roundsUsed.NewProp();  _roundsUsed  = f_roundsUsed;           propBag += _roundsUsed;
	_type.NewProp();        _type        = (int) f_type;           propBag += _type;
	_isActive.NewProp();    _isActive    = (int) f_isActive? 1: 0; propBag += _isActive;
}


void Effect::Copy( Effect::Pointer effect )
{
	*this = *effect;
}


void Effect::apply( Combatant::Character::Pointer ch, const bool force )
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


void Effect::unapply( Combatant::Character::Pointer ch )
{
	internalUnapply( ch );
}


void Effect::internalApply( Combatant::Character::Pointer ch )
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


void Effect::internalUnapply( Combatant::Character::Pointer ch )
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

