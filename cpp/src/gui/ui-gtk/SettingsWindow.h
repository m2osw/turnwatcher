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

// STL
//
#include <map>

// MOLIB
//
#include <mo/mo_base.h>

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// MOTK
//
#include <motk/LabelTable.h>

// LOCAL
//
#include "common.h"
#include "ManagerBase.h"

namespace UI
{

class SettingsWindow
	: public Gtk::Dialog
	, private Application::ManagerBase
{
public:
	SettingsWindow( QString title, Gtk::Window& parent);
	~SettingsWindow();

private:
	class ListColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ListColumns()
		{ add(f_id), add(f_name); add(f_active); }

		Gtk::TreeModelColumn<molib::mo_name_t>	f_id;
		Gtk::TreeModelColumn<QString>	f_name;
		Gtk::TreeModelColumn<bool>		f_active;
	};

	ListColumns			f_columns;
	Glib::RefPtr<Gtk::ListStore>	f_model;
	Gtk::TreeView			f_treeView;
	Gtk::ComboBoxEntryText		f_comboBox;
	motk::LabelTable		f_table;
	Gtk::Tooltips			f_tooltips;
	Attribute::Stat::pointer_t	f_initStat;

	void		InitTable();
	void		InitTreeView();
	void		GetDieFace();
	void		SetDieFace();
	void 		FillMainBox();
	void		UpdateDialog();
	void		SaveValues();

	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual void	on_hide();
	virtual void	on_response( int response_id );
	virtual bool	on_key_press_event( GdkEventKey* event );
};

}
// namespace UI


// vim: ts=8 sw=8

