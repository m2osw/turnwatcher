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




#include "Chooser.h"
#include "Common.h"

#include <libintl.h>

namespace motk
{

Chooser::Chooser()
{
	f_store		= Gtk::ListStore::create    ( f_columns );
	f_sortStore	= Gtk::TreeModelSort::create( f_store   );

	set_model( f_sortStore );
	set_rules_hint();
	set_headers_visible( false );
	set_headers_clickable( false );
	set_enable_search( false );

	// Create the first visible column (icon + name)
	//
	f_nameColumn	= addTextColumn( gettext("Name"), f_columns.name );

	f_sortStore->set_sort_column( f_columns.name, Gtk::SORT_ASCENDING );

	f_selection = get_selection();
	f_selection->set_mode( Gtk::SELECTION_SINGLE );
	f_selection->signal_changed().connect( sigc::mem_fun( *this, &Chooser::onSelectionChanged ) );
}


Chooser::~Chooser()
{
}


Chooser::VoidSignal Chooser::signal_click()
{
	return f_clickSignal;
}


void Chooser::onSelectionChanged()
{
	f_clickSignal.emit();
}


Gtk::TreeView::Column* Chooser::addTextColumn( const QString& name, Gtk::TreeModelColumnBase& column )
{
	Gtk::CellRendererText* renderer = Gtk::manage( new Gtk::CellRendererText );
    Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name.toUtf8().data(), *renderer ) );
	col->add_attribute( renderer->property_text(), column );
	/*col->add_attribute( renderer->property_cell_background(), f_columns.background_color );
	col->add_attribute( renderer->property_foreground(), f_columns.foreground_color );
	col->add_attribute( renderer->property_style(), f_columns.style );
	col->add_attribute( renderer->property_sensitive(), f_columns.sensitive );*/
	col->set_resizable( true );
	col->set_alignment( 0.5 );
	col->set_sort_column( column );
	append_column( *col );
	return col;
}


void Chooser::AddEntry( const char* name, molib::moBase* payload )
{
	Iterator	iter = f_store->append();
	Gtk::TreeRow	row  = *iter;

	row[f_columns.name]		= name;
	row[f_columns.payload]		= payload;

	show_all_children();
}


molib::moBaseSPtr Chooser::GetSelected()
{
	Iterator			sort_model_iter = f_selection->get_selected();
	Iterator			list_model_iter	= f_sortStore->convert_iter_to_child_iter( sort_model_iter);
	const Gtk::TreeModel::Row&	row 		= *list_model_iter;
	return row.get_value( f_columns.payload );
}


void Chooser::Clear()
{
	f_store->clear();
}


#if 0
bool Chooser::on_button_press_event( GdkEventButton* event )
{
	const bool retval = Gtk::TreeView::on_button_press_event( event );

	if( event->button == 1 )
	{
		if( event->type == GDK_2BUTTON_PRESS )
		{
			f_doubleClickSignal.emit();
		}
	}

	return retval;
}
#endif

}
// namespace motk

// $Log: Chooser.cpp,v $
// Revision 1.2  2006/08/26 23:57:07  doug
// Compiles correctly under Win32. Also added new features for TW 1.5--can added two items to label with one method.
//
// Revision 1.1  2006/04/23 18:45:47  doug
// Added chooser class. Fixed debian dependencies. Added ability to change
// icon size on construction.
//
//

// vim: ts=8

