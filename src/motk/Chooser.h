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




#ifndef __CHOOSER_H__
#define __CHOOSER_H__

// STL
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include <stack>

// MOLIB
#include <mo_base.h>

// GTKMM
#include <gtkmm.h>
#include <gdkmm.h>

namespace motk
{

class Chooser : public Gtk::TreeView
{
public:
	typedef Gtk::TreeModel::iterator 	Iterator;
	typedef std::vector<Iterator>	 	Iterators;
	typedef sigc::signal<void>		VoidSignal;

private:
	typedef Glib::RefPtr<Gdk::Pixbuf>		PixbufPtr;
	typedef Glib::RefPtr<Gtk::ListStore>		ListStorePtr;
	typedef Glib::RefPtr<Gtk::TreeModelSort>	TreeModelSortPtr;
	typedef Glib::RefPtr<Gtk::TreeSelection>	TreeSelectionPtr;
	typedef Gtk::TreeSelection::ListHandle_Path	SelectedPaths;

	class Columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<Glib::ustring>	name;
		Gtk::TreeModelColumn<molib::moBaseSPtr>	payload;

		Columns()
		{
			add(name);
			add(payload);
		}
	};
	Columns			f_columns;

	ListStorePtr		f_store;
	TreeModelSortPtr	f_sortStore;
	TreeSelectionPtr	f_selection;

	VoidSignal		f_clickSignal;

	Gtk::TreeView::Column*	f_nameColumn;
	Gtk::TreeView::Column*	f_payloadColumn;

	Gtk::TreeView::Column* 	addTextColumn( const Glib::ustring& name  , Gtk::TreeModelColumnBase& column );

	void			onSelectionChanged();

	// Gtk::Widget
	//
	//virtual bool		on_button_press_event( GdkEventButton* event );

public:
	Chooser();
	virtual ~Chooser();

	void			AddEntry( const char* name, molib::moBase* payload );
	molib::moBaseSPtr	GetSelected();	
	void			Clear();

	VoidSignal		signal_click();
};

}
// namespace motk

#endif //__CHOOSER_H__


// $Log: Chooser.h,v $
// Revision 1.1  2006/04/23 18:45:47  doug
// Added chooser class. Fixed debian dependencies. Added ability to change
// icon size on construction.
//
//
// vim: ts=8

