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


#include "SoftColumns.h"

namespace Combatant
{

SoftColumns::SoftColumns()
{
}


SoftColumns::~SoftColumns()
{
}


void SoftColumns::AddColumn( const molib::mo_name_t& id, column_ptr_t col )
{
    f_columnMap[id] = col;
	Gtk::TreeModel::ColumnRecord::add( *col );
}


SoftColumns::column_ptr_t SoftColumns::GetColumn( const molib::mo_name_t& id )
{
	return f_columnMap[id];
}


}
// namespace UI


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
