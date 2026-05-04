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




#ifndef __SAVEWINDOW_H__
#define __SAVEWINDOW_H__

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"

namespace UI
{

class SaveWindow :
	public Gtk::Dialog
{
	Gtk::Label		f_label;

public:
	SaveWindow( QString title, Gtk::Window& parent ) :
		Gtk::Dialog( title, parent, true, true ),
		f_label( "Please Wait...", Gtk::ALIGN_LEFT )
	{
		Gtk::VBox* box = get_vbox();
		box->pack_start( f_label, Gtk::PACK_EXPAND_WIDGET );
		show_all();
	}
};

}
// namespace UI

#endif //__SAVEWINDOW_H__

// vim: ts=8

