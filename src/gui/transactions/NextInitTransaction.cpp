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




#include "transactions/NextInitTransaction.h"
#include "transactions/BleedOutTransaction.h"
#include "transactions/DelayTransaction.h"
#include "transactions/EffectHandler.h"

namespace Transactions
{


NextInitTransaction::NextInitTransaction() :
	f_group( gettext("Undelay Transaction") ),
	f_firstTime(true)
{
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);
	f_initIter  = initMgr->CurrentInit();
	f_nextRound = f_prevRound = initMgr->RoundNumber();
	f_nextIter  = initMgr->PeekNext( &f_nextRound );

    Combatant::Character::pointer_t ch( initMgr->GetInitiativeCharacter( f_nextIter ) );
#ifdef DEBUG
	printf( "NextInitTransaction(): f_initIter=%d, f_nextIter=%d, f_nextRound=%d, ch=%s\n",
			f_initIter, f_nextIter, f_nextRound, ch->name().c_str() );
#endif
	//
	if( ch->health() == Combatant::Dying )
	{
        Transaction::pointer_t tr( new BleedOutTransaction( ch ) );
		tr->doit();
		f_group.addTransaction( tr );
	}
	//
	switch( ch->status() )
	{
	case Combatant::Delayed:
	case Combatant::Readied:
		{
			// Character's delayed/readied action has expired
			//
            Transaction::pointer_t tr( new UndelayTransaction( ch ) );
			tr->doit();
			f_group.addTransaction( tr );
		}
		break;
	default:
		break;
	}

#ifdef WANT_EFFECTS
    Effects::Effect::list_t effects;
	ch = initMgr->GetInitiativeCharacter( f_nextIter );
	ch->getEffects( effects );
	if( effects.size() > 0 )
	{
        Transaction::pointer_t tr( new EffectHandler( ch ) );
		tr->doit();
		f_group.addTransaction( tr );
	}
#endif
}


void NextInitTransaction::doit()
{
	auto appSettings( GetAppSettings().lock() );
	assert(appSettings);
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	if( !f_group.empty() && !f_firstTime ) f_group.doit();
	f_firstTime = false;
	appSettings->Modified( true );
	initMgr->Next( f_nextIter, f_nextRound );
}


void NextInitTransaction::undo()
{
	auto appSettings( GetAppSettings().lock() );
	assert(appSettings);
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	if( !f_group.empty() ) f_group.undo();
	appSettings->Modified( true );
	initMgr->Next( f_initIter, f_prevRound );
}


}
// namespace Transactions

// vim: ts=8 sw=8

