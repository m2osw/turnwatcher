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




#ifndef __LABELTABLESCROLLER_H__
#define __LABELTABLESCROLLER_H__

// STL
//
#include <map>

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

#include "LabelTable.h"

namespace motk
{

class LabelTableScroller :
	public Gtk::ScrolledWindow
{
public:
	LabelTableScroller();
	~LabelTableScroller();

	LabelTable*	GetLabelTable() const			{ return f_table; }
	void 		SetLabelTable( LabelTable* table );

private:
	LabelTable*		f_table;

	void OnTableFocus();
};

}
// namespace motk

#endif //__LABELTABLESCROLLER_H__

// $Log: LabelTable.h,v $
//

// vim: ts=8

