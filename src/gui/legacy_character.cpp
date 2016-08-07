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





#include "common.h"
#include "legacy_character.h"

using namespace std;
using namespace molib;

namespace Combatant
{

namespace
{
	const molib::moWCString g_name		("NAME");
	const molib::moWCString g_monster	("MONSTER");
	const molib::moWCString g_status	("STATUS");
	const molib::moWCString g_maxHP		("MAXHP");
	const molib::moWCString g_damage	("DAMAGE");
	const molib::moWCString g_stabilized	("STABILIZED");
	const molib::moWCString g_justdropped	("JUSTDROPPED");
	const molib::moWCString g_init		("INIT");
	const molib::moWCString g_spot		("SPOT");
	const molib::moWCString g_listen	("LISTEN");
	const molib::moWCString g_will		("WILL");
	const molib::moWCString g_position	("POSITION");
	const molib::moWCString g_initRoll	("INITROLL");
	const molib::moWCString g_spotRoll	("SPOTROLL");
	const molib::moWCString g_listenRoll	("LISTENROLL");
	const molib::moWCString g_willRoll	("WILLROLL");
}


LegacyCharacter::LegacyCharacter() :
	f_status(InternalNormal),
	f_monster(true),
	f_maxHP(0),
	f_damage(0),
	f_init(0),
	f_spot(0),
	f_listen(0),	
	f_will(0),
	f_position(0),
	f_initRoll(0),
	f_spotRoll(0),
	f_listenRoll(0),
	f_willRoll(0),
	f_stabilized(false),
	f_justdropped(false)
{
}


LegacyCharacter::LegacyCharacter( molib::moPropBagRef propBag )
{
	Load( propBag );
}


void LegacyCharacter::Load( molib::moPropBagRef propBag )
{
	molib::moPropStringRef	_name   	( g_name	);	// name of character
	molib::moPropIntRef	_monster	( g_monster	);	// is this character a monster or pc? (bool value)
	molib::moPropIntRef	_status 	( g_status	);	// normal, delayed or readied action (Character::Status)
	molib::moPropIntRef	_maxHP  	( g_maxHP	);	// Maximum hitpoints the character has
	molib::moPropIntRef	_damage 	( g_damage	);	// Current damage
	molib::moPropIntRef	_stabilized	( g_stabilized	);	// Applies to dying character
	molib::moPropIntRef	_justdropped	( g_justdropped );	// True if character just dropped this round
	molib::moPropIntRef	_init		( g_init	);	// init modifier
	molib::moPropIntRef	_spot		( g_spot	);	// spot modifier
	molib::moPropIntRef	_listen		( g_listen	);	// listen modifier
	molib::moPropIntRef	_will		( g_will	);	// will modifier
	molib::moPropIntRef	_position	( g_position	);	// the initiative position
	molib::moPropIntRef	_initRoll	( g_initRoll	);	// the actual rolls
	molib::moPropIntRef	_spotRoll	( g_spotRoll	);
	molib::moPropIntRef	_listenRoll	( g_listenRoll	);
	molib::moPropIntRef	_willRoll	( g_willRoll	);

	_name		.Link( propBag );
	_monster	.Link( propBag );
	_status		.Link( propBag );
	_maxHP		.Link( propBag );
	_damage		.Link( propBag );
	_stabilized	.Link( propBag );
	_justdropped	.Link( propBag );
	_init		.Link( propBag );
	_spot		.Link( propBag );
	_listen		.Link( propBag );
	_will		.Link( propBag );
	_position	.Link( propBag );
	_initRoll	.Link( propBag );
	_spotRoll	.Link( propBag );
	_listenRoll	.Link( propBag );
	_willRoll	.Link( propBag );

	if( _name.HasProp() )		f_name		= _name;
	if( _status.HasProp() )		f_status 	= (InternalStatus) (int) _status;
	if( _monster.HasProp() )	f_monster	= _monster? true: false;
	if( _maxHP.HasProp() )		f_maxHP		= _maxHP;
	if( _damage.HasProp() )		f_damage	= _damage;
	if( _stabilized.HasProp() )	f_stabilized	= _stabilized? true: false;
	if( _justdropped.HasProp() )	f_justdropped	= _justdropped? true: false;
	if( _init.HasProp() )		f_init		= _init;
	if( _spot.HasProp() )		f_spot		= _spot;
	if( _listen.HasProp() )		f_listen	= _listen;
	if( _will.HasProp() )		f_will		= _will;
	if( _position.HasProp() )	f_position	= _position;
	if( _initRoll.HasProp() )	f_initRoll	= _initRoll;
	if( _spotRoll.HasProp() )	f_spotRoll	= _spotRoll;
	if( _listenRoll.HasProp() )	f_listenRoll	= _listenRoll;
	if( _willRoll.HasProp() )	f_willRoll	= _willRoll;
}

}
// namespace Combatant

// vim: ts=8 sw=8

