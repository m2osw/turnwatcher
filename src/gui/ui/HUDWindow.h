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
//#include <memory>

// Gtkmm
//
#include <gtkmm.h>

// Local includes
//
#include "common.h"
#include "CharacterView.h"
#include "ManagerBase.h"
#include "StatusBox.h"


namespace UI
{

class HUDWindow
	: public Gtk::Window
    , private Application::ManagerBase
{
public:
                        HUDWindow();
	virtual				~HUDWindow();

	void				SetDefaultSort();

	// Actions
	//
	void				OnHUDCloseWindow();
	void				OnHUDChangeFont();

	// Gtk::Window
	//
	virtual void		on_show();
	virtual void		on_hide();
	virtual bool 		on_delete_event( GdkEventAny* event );
	virtual bool 		on_key_press_event( GdkEventKey* event );

private:
	Gtk::VBox				f_mainBox;
	CharacterView			f_charView;
	StatusBox				f_statusBox;
	Gtk::ScrolledWindow		f_scrolledWindow;
	sigc::connection		f_connectChangeFont;

	void PositionWindow();
};


}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

