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




#include "transactions/ForceInitiative.h"

namespace Transactions
{


ForceInitiative::ForceInitiative( Combatant::Character::pointer_t jumpInChar, const int currentInit )
	: f_prevInit( currentInit )
{
	f_nextInit = GetInitMgr().lock()->FindInitiativeOrder( jumpInChar );
}

void ForceInitiative::doit()
{
	GetInitMgr().lock()->CurrentInit( f_nextInit );
	GetInitMgr().lock()->signal_turn_changed().emit( f_nextInit );
}

void ForceInitiative::undo()
{
	GetInitMgr().lock()->CurrentInit( f_prevInit );
	GetInitMgr().lock()->signal_turn_changed().emit( f_prevInit );
}


}
// namespace Transactions

// vim: ts=8 sw=8

