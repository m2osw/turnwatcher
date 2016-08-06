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

// STL
//
#include <map>

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// MOTK
//
#include "motk/LabelTable.h"

// LOCAL
//
#include "common.h"
#include "character.h"
#include "ManagerBase.h"
#include "StatEditor.h"

#define RESPONSE_EFFECTS 100

namespace UI
{

class StatWindow
	: public Gtk::Dialog
	, private Application::ManagerBase
{
public:
	StatWindow( Glib::ustring title, Gtk::Window& parent);
	~StatWindow();

	void	Apply();
	void	Revert();

private:
	StatEditor			f_statEditor;
	Gtk::ScrolledWindow	f_scroller;
	Gtk::Button			f_upButton;
	Gtk::Button			f_downButton;
	Gtk::Button			f_addButton;
	Gtk::Button			f_deleteButton;

	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual void	on_hide();
	virtual bool	on_key_press_event( GdkEventKey* event );
	virtual void	on_response( int response_id );
		
	void	OnSelectionChanged();
};

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

