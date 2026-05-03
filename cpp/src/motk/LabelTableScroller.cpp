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




// STL
//
#include <iostream>

// LOCAL
//
#include "LabelTableScroller.h"
#include "Common.h"

namespace motk
{

LabelTableScroller::LabelTableScroller()
{
}


LabelTableScroller::~LabelTableScroller()
{
}


void LabelTableScroller::SetLabelTable( LabelTable* table )
{
	remove();
	f_table = table;
	assert(f_table);
	add( *f_table );
	f_table->signal_set_focus().connect( sigc::mem_fun(*this, &LabelTableScroller::OnTableFocus) );
	show_all_children();
}


void LabelTableScroller::OnTableFocus()
{
	assert(f_table);
	const int rowPos 	= f_table->GetRowPos();
	Gtk::Widget* widget	= f_table->GetLastFocus();
	const int spacing	= f_table->get_default_row_spacing();
	const int row_height	= widget->get_height() + spacing;
	const int pos_of_widget	= row_height * rowPos;

	Gtk::Adjustment* vadj	= get_vadjustment();
	const double value	= vadj->get_value();
	const double position	= value / vadj->get_page_size();
	const double end_pos	= position + vadj->get_page_size();
	const double lower	= vadj->get_lower();
	const double upper	= (vadj->get_upper() - vadj->get_page_size());
	//
	if( pos_of_widget >= position && pos_of_widget < end_pos )
	{
		// do nothing, is visible
#ifdef DEBUG
		std::cout << "Visible" << std::endl;
#endif
	}
	else
	{
		if( pos_of_widget >= lower && pos_of_widget <= upper )
		{
			vadj->set_value( pos_of_widget );
#ifdef DEBUG
			std::cout << "Move it" << std::endl;
#endif
		}
	}

#ifdef DEBUG
	printf( "pos_of_widget=%d, lower=%f, upper=%f, value=%f, position=%f, end_pos=%f, page_size=%f\n",
			pos_of_widget,
			lower, upper,
			value,
			position, end_pos,
			vadj->get_page_size() );
#endif
}


}
// namespace motk


// $Log: LabelTable.cpp,v $
//

// vim: ts=8

