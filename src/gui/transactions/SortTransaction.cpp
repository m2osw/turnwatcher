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




#include "transactions/SortTransaction.h"
	

namespace Transactions
{


SortTransaction::SortTransaction( UI::CharacterListUI* ui, const molib::mo_name_t id, const int dc )
	: f_charListUI(ui)
	, f_sortId(id)
	, f_dc(dc)
{
	assert(f_charListUI);
	f_prevDC	 = f_charListUI->getDC();
	f_prevSortId = f_charListUI->getSoftSort();
}


void SortTransaction::doit()
{
	assert(f_charListUI);
	f_charListUI->setSoftSort( f_sortId );
	f_charListUI->setDC      ( f_dc     );
}


void SortTransaction::undo()
{
	assert(f_charListUI);
	f_charListUI->setSoftSort( f_prevSortId );
	f_charListUI->setDC      ( f_prevDC     );
}



ClearSortTransaction::ClearSortTransaction( UI::CharacterListUI* ui ) :
	f_charListUI( ui )
{
	f_sortId = f_charListUI->getSoftSort();
	f_prevDC = f_charListUI->getDC();
}


void ClearSortTransaction::doit()
{
	f_charListUI->setSoftSort( molib::moName("NONE") );
	f_charListUI->setDC( -1 );
}


void ClearSortTransaction::undo()
{
	f_charListUI->setSoftSort( f_sortId );
	f_charListUI->setDC( f_prevDC );
}


}
// namespace Transactions

// vim: ts=4 sw=4 syntax=cpp.doxygen

