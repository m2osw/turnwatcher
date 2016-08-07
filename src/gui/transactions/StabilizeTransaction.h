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




#pragma once

#include "transactions/UITransactionBase.h"

namespace Transactions
{


class StabilizeTransaction
	: public Transaction
	, public UITransactionBase
{
public:
	StabilizeTransaction( const Combatant::Character::List& chars, const bool stabilize );
	virtual void doit();
	virtual void undo();

private:
	bool	f_stabilize;

	struct PrevState
	{
		Combatant::Character::Pointer	f_character;
		bool				f_stabilized;
		bool				f_justDropped;

		PrevState() : f_stabilized(false), f_justDropped(false) {}
		PrevState( Combatant::Character::Pointer ch )
		{
			assert(ch);
            f_character = ch;
			f_stabilized  = f_character->stabilized();
			f_justDropped = f_character->justdropped();
		}
	};

	typedef std::vector<PrevState> PrevStateList;
	PrevStateList	f_prevStates;
};


}
// namespace Transactions

// vim: ts=8 sw=8

