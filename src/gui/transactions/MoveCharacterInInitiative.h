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

#include "transactions/InitiativeBase.h"

namespace Transactions
{


class MoveCharacterInInitiative:
	public Transaction,
	public InitiativeBase
{
public:
	MoveCharacterInInitiative( const int new_pos, Combatant::Character::pointer_t ch, const bool set_positions = true );

	virtual void doit();
	virtual void undo();

private:
	int					f_newPos;
	bool					f_setPositions;
	Combatant::Character::pointer_t		f_char;
};


}
// namespace Transactions

// vim: ts=8 sw=8

