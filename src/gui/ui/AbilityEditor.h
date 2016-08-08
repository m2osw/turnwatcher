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
#include "ManagerBase.h"

namespace UI
{

class AbilityEditor
	: public Gtk::TreeView
	, private Application::ManagerBase
{
public:
	AbilityEditor();
	virtual ~AbilityEditor();

	void Apply();
	void Revert();

private:
	typedef Glib::RefPtr<Gtk::ListStore>	ListStorePtr;

	class Columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<Glib::ustring>				f_name;
		Gtk::TreeModelColumn<Glib::ustring>				f_displayName;
		Gtk::TreeModelColumn<Attribute::Stat::pointer_t>	f_ability;

		Columns()
		{
			add( f_name		    );
			add( f_displayName	);
			add( f_ability		);
		}
	};

	class ComboColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<int>	f_dieFace;

		ComboColumns()
		{
			add( f_dieFace );
		}
	};

	Columns			f_columns;
	ComboColumns	f_dieFacesColumns;
	ListStorePtr	f_store;
	ListStorePtr	f_dieFacesStore;;

	typedef std::vector<Attribute::Stat::pointer_t>	Stats;
	Stats		f_deletedStats;

	// Tab column support
	//
	bool                                    m_editingStarted;
	Gtk::TreeModel::Path                    m_currentPath;
	Gtk::TreeViewColumn*                    m_currentCol;

	void		AddAbility( const Glib::ustring& name, Attribute::Stat::pointer_t stat );
	void 		InitDialog();
	void		ClearDialog();
};

}
// namespace UI

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

