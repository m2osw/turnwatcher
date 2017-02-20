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




#include "transactions/StabilizeTransaction.h"

namespace Transactions
{


StabilizeTransaction::StabilizeTransaction( const Combatant::Character::list_t& charList, const bool stabilize )
	: f_stabilize(stabilize)
{
	for( auto ch : charList )
	{
		assert(ch);
		f_prevStates.push_back( PrevState( ch ) );
	}
}

void StabilizeTransaction::doit()
{
	for( auto& st : f_prevStates )
	{
		assert(st.f_character);
		st.f_character->stabilized ( f_stabilize );
		st.f_character->justdropped( false       );
		st.f_character->signal_changed().emit();
	}
}

void StabilizeTransaction::undo()
{
	for( auto& st : f_prevStates )
	{
		assert(st.f_character);
		st.f_character->stabilized ( st.f_stabilized  );
		st.f_character->justdropped( st.f_justDropped );
		st.f_character->signal_changed().emit();
	}
}


}
// namespace Transactions

// vim: ts=8 sw=8

