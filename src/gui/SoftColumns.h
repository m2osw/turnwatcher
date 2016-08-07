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

//#include "common.h"

// STL
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include <stack>
#include <memory>

// MOLIB
#include "mo/mo_base.h"
#include "mo/mo_smartptr.h"

// GTKMM
#include <gtkmm.h>
#include <gdkmm.h>
#include <glibmm.h>

// Local includes
#include "character.h"
//#include "stat.h"


namespace Combatant
{

class SoftColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	typedef std::shared_ptr<SoftColumns> Pointer;

protected:
			SoftColumns();
	virtual ~SoftColumns();

	typedef Glib::RefPtr<Gdk::Pixbuf>	PixbufPtr;

	template <class T>
	class ColumnAdd
	{
		Gtk::TreeModelColumn<T>* f_ptr;
	public:
		ColumnAdd()
		{
			f_ptr = new Gtk::TreeModelColumn<T>;
		}

		operator Gtk::TreeModelColumn<T>* (void)
		{
			return f_ptr;
		}
	};

	typedef ColumnAdd<bool>                          BoolColumn;
	typedef ColumnAdd<int>                           IntColumn;
	typedef ColumnAdd<Pango::Weight>                 WeightColumn;
	typedef ColumnAdd<Pango::Style>                  StyleColumn;
	typedef ColumnAdd<PixbufPtr>                     PixbufColumn;
	typedef ColumnAdd<Combatant::Character::Pointer> CharacterColumn;
	typedef ColumnAdd<sigc::connection>              ConnectionColumn;
	typedef ColumnAdd<Glib::ustring>                 StringColumn;

	template <class T>
	class ColumnGet
	{
		Gtk::TreeModelColumn<T>* f_ptr;
	public:
		ColumnGet( Gtk::TreeModelColumnBase* base )
		{
			f_ptr = static_cast<Gtk::TreeModelColumn<T>*>(base);
		}

		operator Gtk::TreeModelColumn<T>* (void)
		{
			return f_ptr;
		}
	};

	typedef ColumnGet<bool>                          GetBoolColumn;
	typedef ColumnGet<int>                           GetIntColumn;
	typedef ColumnGet<Pango::Weight>                 GetWeightColumn;
	typedef ColumnGet<Pango::Style>                  GetStyleColumn;
	typedef ColumnGet<PixbufPtr>                     GetPixbufColumn;
	typedef ColumnGet<Combatant::Character::Pointer> GetCharacterColumn;
	typedef ColumnGet<sigc::connection>              GetConnectionColumn;
	typedef ColumnGet<Glib::ustring>                 GetStringColumn;

	int							AddColumn( Gtk::TreeModelColumnBase* col );
	Gtk::TreeModelColumnBase*	GetColumn( const int id ) const;
	
private:
	class Column
	{
		typedef std::shared_ptr<Column>		Pointer;
		typedef std::vector<Pointer>		List;

		friend class SoftColumns;
		Gtk::TreeModelColumnBase* f_column;
		public:
			Column() 								: f_column(0) {}
			Column(Gtk::TreeModelColumnBase* c )	: f_column(c) {}
			~Column() { delete f_column; }
	};

	Column::List	f_columns;

	void add( Gtk::TreeModelColumnBase& );	// Forbidden		
};

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

