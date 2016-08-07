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




#include "SoftColumns.h"

namespace Combatant
{

SoftColumns::SoftColumns()
{
}


SoftColumns::~SoftColumns()
{
}


int SoftColumns::AddColumn( Gtk::TreeModelColumnBase* col )
{
	f_columns.push_back( Column::Pointer(new Column( col )) );
	Gtk::TreeModel::ColumnRecord::add( *col );
	return f_columns.size() - 1;
}


Gtk::TreeModelColumnBase* SoftColumns::GetColumn( const int id ) const
{
	assert( (id > -1) && (id < f_columns.size()) );
	return f_columns[id]->f_column;
}

}
// namespace UI


// vim: ts=8 sw=8

