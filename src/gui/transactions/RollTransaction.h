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

#include "transactions/UITransactionBase.h"

namespace Transactions
{


class RollTransaction
	: public Transaction
	, public UITransactionBase
{
public:
	RollTransaction( const molib::mo_name_t id );

	// Transaction::Transaction
	//
	virtual void doit();
	virtual void undo();

private:
	struct Roll
	{
		int								f_prevRoll;
		int								f_currRoll;
		int								f_prevPosition;
		int								f_prevSubPosition;
		int								f_prevManualPos;
		Combatant::Character::pointer_t	f_char;

		Roll( const Combatant::Character::pointer_t ch, const int prev_roll, const int curr_roll );
	};

	typedef std::vector<Roll>		RollList;
	RollList						f_rolls;
	molib::mo_name_t				f_rollId;

	void RollSave();
};

}
// namespace Transactions

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
