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




#pragma once

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"

namespace UI
{

class InputWindow
    : public Gtk::Dialog
{
	Gtk::Label		f_label;
	Gtk::Entry		f_entry;
	std::string		f_value;

	virtual bool	on_key_press_event( GdkEventKey* event );
	virtual void	on_response( int response_id );

public:
	InputWindow( Glib::ustring title, Gtk::Window& parent );
	~InputWindow();

	void Label( const std::string& val );
	void Value( const std::string& val );
	std::string Value() const 		{ return f_value; }
};

}
// namespace UI


// vim: ts=8

