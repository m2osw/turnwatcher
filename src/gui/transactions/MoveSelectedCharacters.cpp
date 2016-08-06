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




#include "transactions/MoveSelectedCharacters.h"
#include "transactions/MoveCharacterInInitiative.h"

namespace Transactions
{

MoveSelectedCharacters::MoveSelectedCharacters( const Combatant::Character::list_t& selected_chars, const bool up )
	: f_up(up)
	, f_selectedChars(selected_chars)
	, f_group(gettext("Move Character in Rounds"))
{
	AddPositions();
}


void MoveSelectedCharacters::AddPositions()
{
	for( auto ch : f_selectedChars )
	{
        assert(ch);
		const int newPos = f_up? ch->getPosition() + 1: ch->getPosition() - 1;
        Transaction::pointer_t tr( new MoveCharacterInInitiative( newPos, ch, false ) );
		f_group.addTransaction( tr );
	}
}


void MoveSelectedCharacters::doit()
{
	f_group.doit();
}


void MoveSelectedCharacters::undo()
{
	f_group.undo();
}


}
// namespace Transactions

// vim: ts=8 sw=8

