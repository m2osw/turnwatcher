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

#include "transactions/ResolveInitiativeDuplicates.h"

namespace Transactions
{


ResolveInitiativeDuplicates::ResolveInitiativeDuplicates()
    : InitiativeBase( true /*emit_signals*/ )
    , f_chars(GetCharacterMgr().lock()->GetCharacters())
{
}


void ResolveInitiativeDuplicates::doit()
{
	swapInitData( f_currInitData );
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);
	initMgr->AddCharacters( f_chars );
}


void ResolveInitiativeDuplicates::undo()
{
	swapInitData( f_prevInitData );
	setPositions();
}


}
// namespace Transactions

// vim: ts=8 sw=8

