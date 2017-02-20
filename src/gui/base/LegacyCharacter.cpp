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

#include "common.h"
#include "LegacyCharacter.h"

using namespace molib;

namespace Combatant
{

namespace
{
	const moWCString g_name        ("NAME");
	const moWCString g_monster     ("MONSTER");
	const moWCString g_status      ("STATUS");
	const moWCString g_maxHP       ("MAXHP");
	const moWCString g_damage      ("DAMAGE");
	const moWCString g_stabilized  ("STABILIZED");
	const moWCString g_justdropped ("JUSTDROPPED");
	const moWCString g_init        ("INIT");
	const moWCString g_spot        ("SPOT");
	const moWCString g_listen      ("LISTEN");
	const moWCString g_will        ("WILL");
	const moWCString g_position    ("POSITION");
	const moWCString g_initRoll    ("INITROLL");
	const moWCString g_spotRoll    ("SPOTROLL");
	const moWCString g_listenRoll  ("LISTENROLL");
	const moWCString g_willRoll    ("WILLROLL");
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


LegacyCharacter::LegacyCharacter( moPropBagRef propBag )
{
	Load( propBag );
}


void LegacyCharacter::Load( moPropBagRef propBag )
{
	moPropStringRef	name			( g_name		);		// name of character
	moPropIntRef	monster			( g_monster		);		// is this character a monster or pc? (bool value)
	moPropIntRef	status			( g_status		);		// normal, delayed or readied action (Character::Status)
	moPropIntRef	maxHP			( g_maxHP		);		// Maximum hitpoints the character has
	moPropIntRef	damage			( g_damage		);		// Current damage
	moPropIntRef	stabilized		( g_stabilized	);		// Applies to dying character
	moPropIntRef	justdropped		( g_justdropped );		// True if character just dropped this round
	moPropIntRef	init			( g_init		);		// init modifier
	moPropIntRef	spot			( g_spot		);		// spot modifier
	moPropIntRef	listen			( g_listen		);		// listen modifier
	moPropIntRef	will			( g_will		);		// will modifier
	moPropIntRef	position		( g_position	);		// the initiative position
	moPropIntRef	initRoll		( g_initRoll	);		// the actual rolls
	moPropIntRef	spotRoll		( g_spotRoll	);
	moPropIntRef	listenRoll		( g_listenRoll	);
	moPropIntRef	willRoll		( g_willRoll	);

	name        .Link( propBag );
	monster     .Link( propBag );
	status      .Link( propBag );
	maxHP       .Link( propBag );
	damage      .Link( propBag );
	stabilized  .Link( propBag );
	justdropped .Link( propBag );
	init        .Link( propBag );
	spot        .Link( propBag );
	listen      .Link( propBag );
	will        .Link( propBag );
	position    .Link( propBag );
	initRoll    .Link( propBag );
	spotRoll    .Link( propBag );
	listenRoll  .Link( propBag );
	willRoll    .Link( propBag );

	if( name.HasProp()        ) f_name        = static_cast<moWCString>(name).c_str();
	if( status.HasProp()      ) f_status      = static_cast<InternalStatus>( static_cast<int>(status) );
	if( monster.HasProp()     ) f_monster     = monster?        true: false;
	if( maxHP.HasProp()       ) f_maxHP       = maxHP;
	if( damage.HasProp()      ) f_damage      = damage;
	if( stabilized.HasProp()  ) f_stabilized  = stabilized?     true: false;
	if( justdropped.HasProp() ) f_justdropped = justdropped?    true: false;
	if( init.HasProp()        ) f_init        = init;
	if( spot.HasProp()        ) f_spot        = spot;
	if( listen.HasProp()      ) f_listen      = listen;
	if( will.HasProp()        ) f_will        = will;
	if( position.HasProp()    ) f_position    = position;
	if( initRoll.HasProp()    ) f_initRoll    = initRoll;
	if( spotRoll.HasProp()    ) f_spotRoll    = spotRoll;
	if( listenRoll.HasProp()  ) f_listenRoll  = listenRoll;
	if( willRoll.HasProp()    ) f_willRoll    = willRoll;
}

}
// namespace Combatant

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
