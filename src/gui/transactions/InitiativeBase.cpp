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




#include "AppSettings.h"
#include "InitiativeBase.h"

namespace Transactions
{

InitiativeBase::InitiativeBase() :
	f_inRounds	(GetInitMgr().lock()->InRounds()   ),
	f_roundNumber	(GetInitMgr().lock()->RoundNumber()),
	f_prevRoundNum	(GetInitMgr().lock()->RoundNumber()),
	f_currInit	(GetInitMgr().lock()->CurrentInit()),
	f_prevInit	(GetInitMgr().lock()->CurrentInit()),
	f_prevInitData  (GetInitMgr().lock()->GetInitData()),
	f_currInitData  (new Initiative::InitiativeData)
{
	// TODO: This might be expensive--perhaps only subclasses that need it should do this
	//
	f_currInitData->Copy( GetInitMgr().lock()->GetInitData() );
}

void InitiativeBase::persistStatuses()
{
	for( auto ch : getCharacters() )
	{
		assert(ch);
		CharacterStatus status( ch, ch->status(), Combatant::Normal );
		f_statuses.push_back( status );
	}
}


void InitiativeBase::changeStatuses( const bool doit )
{
	for( auto& status : f_statuses )
	{
		status.f_char->status( doit? status.f_newStatus: status.f_prevStatus );
	}
}

void InitiativeBase::startInit()
{
    Combatant::Character::List list( getCharacters() );
	if( list.size() == 0 )
	{
		stopInit();
	}
	else
	{
		GetInitMgr().lock()->Start();
        GetAppSettings().lock()->Modified( true );
	}
}

void InitiativeBase::stopInit()
{
	GetInitMgr().lock()->Stop();
    GetAppSettings().lock()->Modified( true );
}


void InitiativeBase::swapInitData( Initiative::InitiativeData::Pointer data )
{
	if( f_inRounds )
	{
		GetInitMgr().lock()->SetInitData( data );
	}
}


void InitiativeBase::setPositions()
{
	if( f_inRounds )
	{
		GetInitMgr().lock()->SetPositions();
	}
}


void InitiativeBase::updateInitCharacters()
{
	if( f_inRounds )
	{
		stopInit();
        GetInitMgr().lock()->UpdateCharacters( GetCharacterMgr().lock()->GetCharacters() );
		startInit();
	}
}


}
// namespace Transactions

// vim: ts=8 sw=8

