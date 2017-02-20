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





#pragma once

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// motk
//
#include <motk/LabelTable.h>

// LOCAL
//
#include "common.h"

namespace UI
{

class DuplicateWindow
	: public Gtk::Dialog
{
	motk::LabelTable	f_table;
	int			f_value;

	// Gtk::SpinButton signal_value_changed()
	//
	void		OnValueChanged();

	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual bool	on_key_press_event( GdkEventKey* event );

public:
	DuplicateWindow( Glib::ustring title, Gtk::Window& parent );
	~DuplicateWindow();

	void Value( int val );
	int Value() const 	{ return f_value; }
};

}
// namespace UI


// vim: ts=8

