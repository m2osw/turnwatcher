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

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// MOLIB
//
#include <mo/mo_name.h>

namespace motk
{


class LabelTable :
	public Gtk::Table
{
public:
	LabelTable();
	~LabelTable();

#if 0
	class Item : public molib::moBase
	{
	public:
        Item( const molib::moName& id, const QString& label, const bool checkbox = false );
		
        void	AddTextColumn( const molib::moName& id, const QString& default_value );
        void	AddComboColumn( const molib::moName& id, const std::vector<QString>& strings, const int default_value = 0 );
		void	AddValueColumn( const molib::moName& id, double default_value, double lower, double upper );
		
        QString	GetTextColumn( const molib::moName& id );
        int		GetComboColumn( const molib::moName& id /*std::vector<QString>& strings*/ );
		double		GetValueColumn( const molib::moName& id );
		
	private:
		molib::moName	f_id;
		Gtk::Widget*	f_label;
		bool		f_checkout;
		//
		typedef std::map<molib::mo_name_t, Gtk::Widget*> WidgetMap;
 		WidgetMap	f_columns;
	};
	//
	typedef molib::moSmartPtr<Item> ItemSPtr;
	void		AddItem( ItemSPtr item );
	ItemSPtr	GetItem( const molib::moName& id );
#else
    void		AddItem( const molib::moName& id, const QString& label, const bool checkbox, ... );
	Gtk::Widget* 	GetItem( const molib::moName& id, const int idx = 0 ) { return f_entryControls[id].f_widgets[idx]; }
#endif
	void		AddHeader( const char* label, ... );

    void		SetLabel( const molib::moName& id, const QString& label );

	bool		GetCheck( const molib::moName& id );
	void		SetCheck( const molib::moName& id, const bool check = true );

	template <class T> void GetItem( const molib::moName& id, T*& item ) { item = dynamic_cast<T*>(f_entryControls[id].f_widgets[0]); }
	template <class T> void GetItem( const molib::moName& id, const int idx, T*& item ) { item = dynamic_cast<T*>(f_entryControls[id].f_widgets[idx]); }

	Gtk::Widget*	GetLastFocus() const	{ return f_lastFocus;   }
	int		GetRowPos() const	{ return f_rowPos;      }
	int		GetColumnCount() const	{ return f_columnCount; }

	void	Clear();

	typedef sigc::signal<void> VoidSignal;

	VoidSignal	signal_set_focus()	{ return f_setFocusSignal; }
	VoidSignal	signal_toggled()	{ return f_toggledSignal; }
	VoidSignal	signal_changed()	{ return f_changedSignal; }

private:
#if 0
	typedef std::vector<ItemSPtr>	f_items;
#else
	struct TableEntry
	{
		Gtk::Widget*	f_label;

		typedef std::vector<Gtk::Widget*> WidgetList;
 		WidgetList	f_widgets;

		TableEntry() : f_label(0) {}
		TableEntry( Gtk::Widget* label ) :
			f_label(label) {}

		void AddWidget( Gtk::Widget* widget )	{ f_widgets.push_back( widget ); }
	};

	typedef std::map<molib::mo_name_t,TableEntry> TableEntryControls;
	TableEntryControls	f_entryControls;
#endif

	Gtk::Widget*		f_lastFocus;
	int			f_columnCount;
	int			f_lastRow;
	int			f_rowPos;
	VoidSignal		f_setFocusSignal;
	VoidSignal		f_toggledSignal;
	VoidSignal		f_changedSignal;

	// Utility methods for facilitating column adding
	//
	void AddColumnHeader( const char* column_name, const int left, const int right );
	void AddColumnItem( Gtk::Widget* widget, const int left, const int right );

	// last focus support
	//
	void OnChildSetFocus( Gtk::Widget* focus, int rowPos );
	void OnToggle( TableEntry& button );
	void OnChanged();
};

}
// namespace motk

// vim: ts=8 sw=8

