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
#include "stat.h"
#include "ManagerBase.h"

namespace UI
{

class StatEditor :
	public Application::ManagerBase,
	public Gtk::TreeView
{
public:
	StatEditor();
	~StatEditor();

	void	Apply();
	void	AddNewStat();
	void	MoveSelected( const bool up );
	void	DeleteSelected();

private:
	typedef Glib::RefPtr<Gtk::TreeModelSort>	TreeModelSortPtr;
	typedef Glib::RefPtr<Gtk::ListStore>		ListStorePtr;
	typedef Glib::RefPtr<Gtk::TreeStore>		TreeStorePtr;
	typedef Glib::RefPtr<Gtk::TreeModel>		TreeModelPtr;

	class Columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<int>					f_order;
		Gtk::TreeModelColumn<Glib::ustring>			f_name;
		Gtk::TreeModelColumn<Glib::ustring>			f_dieFaces;
		Gtk::TreeModelColumn<Glib::ustring>			f_accelKey;
		Gtk::TreeModelColumn<bool>					f_showOnToolbar;
		Gtk::TreeModelColumn<bool>					f_showOnHUD;
		Gtk::TreeModelColumn<bool>					f_showMonsterOnHUD;
		Gtk::TreeModelColumn<Attribute::Stat::pointer_t>	f_stat;

		Columns()
		{
			add( f_order				);
			add( f_name					);
			add( f_dieFaces				);
			add( f_accelKey				);
			add( f_showOnToolbar		);
			add( f_showOnHUD			);
			add( f_showMonsterOnHUD		);
			add( f_stat					);
		}
	};


	class ComboColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<Glib::ustring>	f_name;

		ComboColumns()
		{
			add( f_name );
		}
	};

	Columns						f_columns;
	ComboColumns				f_accelKeyColumns;
	ComboColumns				f_dieFacesColumns;
	//
	ListStorePtr				f_store;
	ListStorePtr				f_accelKeyStore;
	ListStorePtr				f_dieFacesStore;
	
	typedef std::vector<Attribute::Stat::pointer_t>	StatList;
	StatList					f_statsCopy;
	
	Gtk::TreeView::Column*		f_nameColumn;

	void	Clear();
	void	FixOrderings();
	void	CopyStats();
	void	AddStats();
	void	AddDieFace( const Glib::ustring& value );
	void	AddAccelKey( const Glib::ustring& value );
	void	AddDieFacesColumn();
	void	AddAccelKeyColumn();
	void	AddAccelKeyFromStats();
	Attribute::Stat::pointer_t GetOrCreateStat( const Gtk::TreeModel::Row& row );
	void	AddStat( Attribute::Stat::pointer_t stat );
	void	AddAccelKeys();
	//
	void	OnOrderEdited( const Glib::ustring& path, const Glib::ustring& new_text );
	void	OnNameEdited( const Glib::ustring& path, const Glib::ustring& new_text );
	void	OnDieFacesEdited( const Glib::ustring& path, const Glib::ustring& new_text );
	void	OnAccelKeyEdited( const Glib::ustring& path, const Glib::ustring& new_text );
	void	OnShowMonsterOnHUDToggled( const Glib::ustring& path );
	void	OnShowOnToolbarToggled( const Glib::ustring& path );
	void	OnShowOnHUDToggled( const Glib::ustring& path );
	//
	bool	PopulateRow( const Gtk::TreeModel::iterator& iter );

	// Gtk::Widget
	//
	virtual bool	on_key_press_event( GdkEventKey* event );
};

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

