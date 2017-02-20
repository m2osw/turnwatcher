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

#pragma once

#include "transaction.h"
#include "character.h"
#include "transactions/UITransactionBase.h"

namespace Transactions
{

//===============================================================================
// Character managment transactions
//===============================================================================


class InitiativeBase
    : public UITransactionBase
{
protected:	
	InitiativeBase( const bool emit_signals );

	void persistStatuses();
	void changeStatuses( const bool doit );
	void startInit();
	void stopInit();

protected:
	bool	f_inRounds;
	int		f_roundNumber;
	int		f_prevRoundNum;
	int		f_currInit;
	int		f_prevInit;
	bool	f_emitSignals;

	Initiative::InitiativeData::pointer_t f_prevInitData;
	Initiative::InitiativeData::pointer_t f_currInitData;

	void swapInitData( Initiative::InitiativeData::pointer_t data );
	void setPositions();
	void updateInitCharacters();

private:
	struct CharacterStatus
	{
		Combatant::Character::pointer_t	f_char;
        Combatant::Status		        f_prevStatus;
        Combatant::Status				f_newStatus;

        CharacterStatus(
                Combatant::Character::pointer_t ch,
                Combatant::Status prevStatus,
                Combatant::Status newStatus
                )
            : f_char(ch), f_prevStatus(prevStatus), f_newStatus(newStatus)
		{}

		CharacterStatus() : f_prevStatus(Combatant::Normal), f_newStatus(Combatant::Normal)
		{}
	};

	typedef std::vector<CharacterStatus> list_t;
	list_t	f_statuses;
};


class StartInitTransaction
    : public Transaction
    , public InitiativeBase
{
public:
    StartInitTransaction() : InitiativeBase( true /*emit_signals*/ )
	{
		f_roundNumber = 1;
		f_currInit    = 0;
	}

	virtual void doit()
	{
		GetInitMgr().lock()->RoundNumber( f_roundNumber );
		GetInitMgr().lock()->CurrentInit( f_currInit    );
		startInit();
	}

	virtual void undo()
	{
		GetInitMgr().lock()->RoundNumber( f_prevRoundNum );
		GetInitMgr().lock()->CurrentInit( f_prevInit     );
		stopInit();
	}
};


class StopInitTransaction
    : public Transaction
    , public InitiativeBase
{
public:
    StopInitTransaction() : InitiativeBase( true /*emit_signals*/ )
	{
		persistStatuses();
	}

	virtual void doit()
	{
		GetInitMgr().lock()->RoundNumber( f_roundNumber );
		GetInitMgr().lock()->CurrentInit( f_currInit    );
		changeStatuses( true /*doit*/ );
		stopInit();
	}

	virtual void undo()
	{
		GetInitMgr().lock()->RoundNumber( f_prevRoundNum );
		GetInitMgr().lock()->CurrentInit( f_prevInit     );
		changeStatuses( false /*undo-it*/ );
		startInit();
	}
};


}
// namespace Transactions

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

