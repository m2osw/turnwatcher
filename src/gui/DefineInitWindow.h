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

// MOLIB
//
#include "mo/mo_base.h"

// MOTK
//
#include "motk/LabelTable.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "character.h"
#include "SingletonManager.h"
//#include "TableConnections.h"

namespace UI
{

class DefineInitWindow
	: public Gtk::Dialog
	, private Application::ManagerBase
{
public:
	DefineInitWindow( Glib::ustring title, Gtk::Window& parent);
	virtual ~DefineInitWindow();

#if !defined(DEMO_VERSION)
	void	GetData();
#endif

private:
	motk::LabelTable		f_baseTable;
	Attribute::Stat::Pointer	f_initStat;

	void		ClearMainBox();
	void 		FillMainBox();
	void		UpdateDialog();
	void		UpdateDlgFromInit();
#if !defined(DEMO_VERSION)
	void		UpdateInitFromDlg();
#endif

	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual void	on_hide();
	virtual bool	on_key_press_event( GdkEventKey* event );
};

}
// namespace UI

// vim: ts=8

