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
#include "mo/mo_name.h"

// GTKMM
#include <gtkmm.h>
#include <gdkmm.h>
#include <glibmm.h>

// Local includes
#include "base/character.h"


namespace Combatant
{

class SoftColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	typedef std::shared_ptr<SoftColumns> pointer_t;

protected:
			SoftColumns();
	virtual ~SoftColumns();

	typedef Combatant::Character::pointer_t         character_t;
	typedef Glib::RefPtr<Gdk::Pixbuf>               pixbuf_t;
	typedef Gtk::TreeModelColumn<int>              	col_int_t;
	typedef Gtk::TreeModelColumn<bool>             	col_bool_t;
	typedef Gtk::TreeModelColumn<QString>    	col_ustring_t;
	typedef Gtk::TreeModelColumn<pixbuf_t>         	col_pixbuf_t;
	typedef Gtk::TreeModelColumn<Pango::Weight>    	col_weight_t;
	typedef Gtk::TreeModelColumn<Pango::Style>     	col_style_t;
	typedef Gtk::TreeModelColumn<character_t>      	col_character_t;
	typedef Gtk::TreeModelColumn<sigc::connection> 	col_connection_t;
	//
	typedef Gtk::TreeModelColumnBase                column_t;
	typedef std::shared_ptr<column_t>               column_ptr_t;

	void			AddColumn( const molib::mo_name_t& id, column_ptr_t col );
	column_ptr_t	GetColumn( const molib::mo_name_t& id );
	
private:
	typedef std::map<molib::mo_name_t, column_ptr_t> column_map_t;
	column_map_t f_columnMap;

	void add( Gtk::TreeModelColumnBase& );	// Forbidden		
};

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

