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

// STL
//
#include <iostream>

// STDC
//
#include <stdarg.h>

// MOLIB
//
#include <mo/mo_error.h>

// LOCAL
//
#include "LabelTable.h"
#include "Common.h"

namespace motk
{

namespace
{
	const int x_padding = 5;
	const int y_padding = 0;
}


LabelTable::LabelTable() :
	Gtk::Table( 9, 2, false /*not homogenous*/ ),
	f_lastFocus(0),
	f_columnCount(0),
	f_lastRow(0),
	f_rowPos(0)
{
}


LabelTable::~LabelTable()
{
}


void LabelTable::Clear()
{
	Gtk::Table::TableList& items = children();
	items.erase( items.begin(), items.end() );
	f_entryControls.clear();
	f_columnCount = 0;
	f_rowPos = 0;
	f_lastRow = 0;
}


#if 0
LabelTable::Item::Item( const molib::moName& id, const QString& label, const bool checkbox ) :
	f_id(id),
	f_checkbox(checkbox),
	f_label(0)
{
	if( f_checkbox )
	{
		f_label = Gtk::manage( new Gtk::CheckButton(label) );
	}
	else
	{
		f_label = Gtk::manage( new Gtk::Label(label, Gtk::ALIGN_LEFT  ) );
	}
}


void LabelTable::Item::AddTextColumn( const molib::moName& id, const QString& default_value )
{
 	Gtk::Entry* ent = new Gtk::Entry;
 	ent->get_text( default_value );
 	f_columns[id] = ent;
}


void LabelTable::Item::AddComboColumn( const molib::moName& id, const std::vector<QString>& strings, const QString& default_value )
{
	Gtk::ComboBoxText* cb = new Gtk::ComboBoxText;
	std::for_each( strings.begin(), strings.end(), sigc::mem_fun( cb, &Gtk::ComboBoxText::append_text ) );
	cb->set_active_text(default_value);
	f_columns[id] = cb;
}

void LabelTable::Item::AddValueColumn( const molib::moName& id, double default_value, double lower, double upper )
{
	Gtk::Adjustment* adj = Gtk::manage( new Gtk::Adjustment( default_value, lower, upper ) );
	Gtk::SpinButton* sp  = new Gtk::SpinButton (*adj );
	sp->set_numeric( true );
	f_columns[id] = sp;
}


QString LabelTable::Item::GetTextColumn( const molib::moName& id )
{
	Gtk::Widget* widget = f_columns[id];
	Gtk::Entry*  ent    = dynamic_cast<Gtk::Entry*>(widget);
	if( !ent )
	{
		throw molib::moError("Column id %s does not represent a text entry widget!", static_cast<molib::moWCstring>(id).c_str() );
	}
	return ent->get_text();
}


QString LabelTable::Item::GetComboColumn( const molib::moName& id )
{
	Gtk::Widget*		widget	= f_columns[id];
	Gtk::ComboBoxText*	cb	= dynamic_cast<Gtk::ComboBoxText*>(widget);
	if( !cb )
	{
		throw molib::moError("Column id %s does not represent a combobox entry widget!", static_cast<molib::moWCstring>(id).c_str() );
	}
	return cb->get_active_text();
}


double LabelTable::Item::GetValueColumn( const molib::moName& id )
{
	Gtk::Widget*		widget	= f_columns[id];
	Gtk::SpinButton*	sp	= dynamic_cast<Gtk::SpinButton*>(widget);
	if( !sp )
	{
		throw molib::moError("Column id %s does not represent a value entry widget!", static_cast<molib::moWCstring>(id).c_str() );
	}
	return sp->get_value();
}
#endif


void LabelTable::AddColumnHeader( const char* column_name, const int left, const int right )
{
	assert(column_name);
	Gtk::Label*  label_column  = Gtk::manage( new Gtk::Label(column_name, Gtk::ALIGN_LEFT ) );
	label_column->set_use_markup( true );
    label_column->set_markup( QString("<b>%1</b>").arg(column_name).toUtf8().data() );

	attach( *(label_column), left, right, f_lastRow, f_lastRow+1, Gtk::FILL|Gtk::SHRINK, Gtk::SHRINK, x_padding, y_padding );
}


void LabelTable::AddHeader( const char* label, ... )
{
	if( f_lastRow != 0 )
	{
		throw molib::moError( "Cannot add a column header in the middle of a table! Call the Clear() method first." );
	}

	int left  = 0;
	int right = 1;

	// Add first column, which is always there
	//
	AddColumnHeader( label, left++, right++ );

	va_list ap;
	va_start( ap, label );
	char* column_label = va_arg( ap, char* );

	if( column_label == NULL )
	{
		throw molib::moError( "Must have at least one column defined!" );
	}

	f_columnCount = 0;

	while( column_label )
	{
		AddColumnHeader( column_label, left++, right++ );
		column_label = va_arg( ap, char* );
		f_columnCount++;
	}

	++f_lastRow;
}


void LabelTable::AddColumnItem( Gtk::Widget* widget, const int left, const int right )
{
	assert(widget);

	attach( *(widget), left, right, f_lastRow, f_lastRow+1, Gtk::FILL|Gtk::SHRINK, Gtk::SHRINK, x_padding, y_padding );
	widget->signal_grab_focus().connect( sigc::bind( sigc::mem_fun(*this, &LabelTable::OnChildSetFocus), widget, f_lastRow ) );

	Gtk::Editable* entry = dynamic_cast<Gtk::Editable*>(widget);
	//
	if( entry )
	{
		entry->signal_changed().connect( sigc::mem_fun( *this, &LabelTable::OnChanged ) );
	}
}


void LabelTable::AddItem( const molib::moName& id, const QString& label, const bool checkbox, ... )
{
	Gtk::Widget*  label_control  = 0;

	if( checkbox )
	{
        label_control = Gtk::manage( new Gtk::CheckButton(label.toUtf8().data()) );
	}
	else
	{
        label_control = Gtk::manage( new Gtk::Label(label.toUtf8().data(), Gtk::ALIGN_LEFT  ) );
	}

	int left  = 0;
	int right = 1;

	attach( *(label_control  ), left++, right++, f_lastRow, f_lastRow+1, Gtk::FILL|Gtk::EXPAND, Gtk::SHRINK, x_padding, y_padding );

	TableEntry ent( label_control );

	va_list ap;
	va_start( ap, checkbox );
	Gtk::Widget* widget_control = (Gtk::Widget*) va_arg( ap, void* );
	//
	if( widget_control == NULL )
	{
		throw molib::moError( "Must add at least one column!" );
	}
	//
	ent.AddWidget( widget_control );

	int columnCount = 0;

	while( widget_control )
	{
		AddColumnItem( widget_control, left++, right++ );
		widget_control = (Gtk::Widget*) va_arg( ap, char* );
		ent.AddWidget( widget_control );
		columnCount++;
	}

#if 0
	// TODO: this needs to be better thought out!
	//
	if( columnCount != f_columnCount )
	{
		throw molib::moError( "Expected column count is %d, got %d. You must add exactly %d columns!",
				f_columnCount, columnCount, f_columnCount );
	}
#endif

	++f_lastRow;

	if( checkbox )
	{
		Gtk::CheckButton* btn = dynamic_cast<Gtk::CheckButton*>(label_control);
		btn->signal_toggled().connect( sigc::bind( sigc::mem_fun( *this, &LabelTable::OnToggle ), ent ) );
	}
	//
	f_entryControls[id] = ent;
}


void LabelTable::SetLabel( const molib::moName& id, const QString& label )
{
	Gtk::Label* lbl = dynamic_cast<Gtk::Label*>(f_entryControls[id].f_label);
    lbl->set_label( label.toUtf8().data() );
}


bool LabelTable::GetCheck( const molib::moName& id )
{
	bool active = false;
	Gtk::CheckButton* btn = dynamic_cast<Gtk::CheckButton*>(f_entryControls[id].f_label);
	if( btn )
	{
		active = btn->get_active();
	}

	return active;
}


void LabelTable::SetCheck( const molib::moName& id, const bool check )
{
	Gtk::CheckButton* btn = dynamic_cast<Gtk::CheckButton*>(f_entryControls[id].f_label);
	if( btn )
	{
		btn->set_active( check );
		OnToggle( f_entryControls[id] );
	}
}


void LabelTable::OnChildSetFocus( Gtk::Widget* focus, int rowPos )
{
	f_lastFocus	= focus;
	f_rowPos	= rowPos;
	f_setFocusSignal.emit();

#ifdef DEBUG
	const Gtk::WidgetFlags flags = focus->get_flags();
	std::cout << "=======================" << std::endl;
	printf( "focus=%p, flags=%x\n", focus, flags );
	if( flags & Gtk::TOPLEVEL 		) { std::cout << "TOPLEVEL 		" << std::endl; }
	if( flags & Gtk::NO_WINDOW 		) { std::cout << "NO_WINDOW 		" << std::endl; }
	if( flags & Gtk::REALIZED 		) { std::cout << "REALIZED 		" << std::endl; }
	if( flags & Gtk::MAPPED 		) { std::cout << "MAPPED 		" << std::endl; }
	if( flags & Gtk::VISIBLE 		) { std::cout << "VISIBLE 		" << std::endl; }
	if( flags & Gtk::SENSITIVE 		) { std::cout << "SENSITIVE 		" << std::endl; }
	if( flags & Gtk::PARENT_SENSITIVE 	) { std::cout << "PARENT_SENSITIVE 	" << std::endl; }
	if( flags & Gtk::CAN_FOCUS 		) { std::cout << "CAN_FOCUS 		" << std::endl; }
	if( flags & Gtk::HAS_FOCUS 		) { std::cout << "HAS_FOCUS 		" << std::endl; }
	if( flags & Gtk::CAN_DEFAULT 		) { std::cout << "CAN_DEFAULT 		" << std::endl; }
	if( flags & Gtk::HAS_DEFAULT 		) { std::cout << "HAS_DEFAULT 		" << std::endl; }
	if( flags & Gtk::HAS_GRAB 		) { std::cout << "HAS_GRAB 		" << std::endl; }
	if( flags & Gtk::RC_STYLE 		) { std::cout << "RC_STYLE 		" << std::endl; }
	if( flags & Gtk::COMPOSITE_CHILD	) { std::cout << "COMPOSITE_CHILD	" << std::endl; }
	if( flags & Gtk::NO_REPARENT 		) { std::cout << "NO_REPARENT 		" << std::endl; }
	if( flags & Gtk::APP_PAINTABLE 		) { std::cout << "APP_PAINTABLE 	" << std::endl; }
	if( flags & Gtk::RECEIVES_DEFAULT 	) { std::cout << "RECEIVES_DEFAULT 	" << std::endl; }
	if( flags & Gtk::DOUBLE_BUFFERED 	) { std::cout << "DOUBLE_BUFFERED 	" << std::endl; }
	if( flags & Gtk::NO_SHOW_ALL		) { std::cout << "NO_SHOW_ALL		" << std::endl; }
	std::cout << "=======================" << std::endl << std::endl;
#endif
}


namespace
{
	void SetSensitive( Gtk::Widget* widget, const bool active )
	{
		widget->set_sensitive( active );
	}
}

void LabelTable::OnToggle( TableEntry& entry )
{
	Gtk::CheckButton* btn = dynamic_cast<Gtk::CheckButton*>(entry.f_label);

	if( btn )
	{
		const bool active = btn->get_active();
		std::for_each( entry.f_widgets.begin(), entry.f_widgets.end(), sigc::bind( &SetSensitive, active ) );
	}

	f_toggledSignal.emit();
}


void LabelTable::OnChanged()
{
	f_changedSignal.emit();
}


}
// namespace motk


// vim: ts=8 sw=8


