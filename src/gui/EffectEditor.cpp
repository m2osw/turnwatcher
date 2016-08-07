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




// LOCAL
//
#include "EffectEditor.h"

#include <math.h>

using namespace motk;
using namespace molib;
using namespace Effects;

namespace UI
{

namespace
{
	const moWCString	Name		("Name");
	const moWCString	Description	("Description");
	const moWCString	TotalRounds	("TotalRounds");
	const moWCString	StartIn		("StartIn");
	const moWCString	RoundsUsed	("RoundsUsed");
	const moWCString	TempHP		("TempHP");
	const moWCString	HPBoost		("HPBoost");
	const moWCString	Type		("Type");

	const double g_lower_range	= -100000.0;
	const double g_upper_range	= 100000.0;

	moWCString GetText( LabelTable& tbl, const moName& id )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		return e ? e->get_text().c_str() : gettext("Error!");
	}

	void SetText( LabelTable& tbl, const moName& id, const moWCString& text )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		if( e ) e->set_text( text.c_str() );
	}

	Effect::Type GetChoice( LabelTable& tbl, const moName& id )
	{
		Gtk::ComboBoxText* c;
		tbl.GetItem( id, c );
		return c? (Effect::Type) c->get_active_row_number(): Effect::Other;
	}

	void SetChoice( LabelTable& tbl, const moName& id, const Effect::Type value )
	{
		Gtk::ComboBoxText* c;
		tbl.GetItem( id, c );
		if( c ) c->set_active( (int) value );
	}

	int GetValue( LabelTable& tbl, const moName& id )
	{
		Gtk::SpinButton* b;
		tbl.GetItem( id, b );
		return b? atoi(b->get_text().c_str()): -1;
	}

	void SetValue( LabelTable& tbl, const moName& id, int value )
	{
		Gtk::SpinButton* b;
		tbl.GetItem( id, b );
		if( b ) b->set_value( value );
	}
}


EffectEditor::EffectEditor( const Glib::ustring& title ) :
	Gtk::Dialog( title, true, true ),
	f_cancelButton (0),
	f_acceptButton (0),
	f_modified     (false)
{
	FillMainBox();

	show_all_children();

	f_tableConnections.push_back( TableConnection( &f_table ) );

	ConnectAdjustments();
	ConnectTables();
}


EffectEditor::~EffectEditor()
{
	DisconnectAdjustments();
	DisconnectTables();

	for( auto ac : f_adjConnections )
	{
		delete ac.f_adj;
	}

	f_adjConnections.clear();
}


void EffectEditor::FillMainBox()
{
	Gtk::VBox* vbox = get_vbox();

	InitTable();

	// Add table editor
	//
	vbox->pack_start( f_table, false, false );

	if( alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) ) )
	{
		f_acceptButton = add_button( Gtk::Stock::APPLY , Gtk::RESPONSE_OK     );
		f_cancelButton = add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	}
	else
	{
		f_cancelButton = add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
		f_acceptButton = add_button( Gtk::Stock::APPLY , Gtk::RESPONSE_OK     );
	}

	f_cancelButton->set_sensitive( true  );
	f_acceptButton->set_sensitive( false );

	f_tooltips.set_tip( *f_acceptButton, gettext("Accept changes to the effect") );
	f_tooltips.set_tip( *f_cancelButton, gettext("Cancel changes to the effect") );
	f_tooltips.enable();

	show_all_children();
}


void EffectEditor::InitTable()
{
	Gtk::Adjustment* adj;
	Gtk::SpinButton* sp;
	Gtk::Entry*	 ent;

	f_table.AddItem( Name, gettext("Effect Name"), false, ent = new Gtk::Entry, NULL );
	f_tooltips.set_tip( *ent, gettext("A unique name for the effect") );
	f_table.AddItem( Description, gettext("Effect Description"), false, ent = new Gtk::Entry, NULL );
	f_tooltips.set_tip( *ent, gettext("A more detailed description of the effect. Shows in the Effects Pane.") );
	//
	adj = new Gtk::Adjustment(1.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_table.AddItem( TotalRounds, gettext("Total Rounds"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric(true);
	f_tooltips.set_tip( *sp, gettext("The total number of rounds the effect will last.") );
	//
	adj = new Gtk::Adjustment(1.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_table.AddItem( StartIn, gettext("Start In"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric(true);
	f_tooltips.set_tip( *sp, gettext("The number of rounds before the effect will start. Note: Rounds Used below will be reset to 0.") );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_table.AddItem( RoundsUsed, gettext("Rounds Used"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric(true);
	f_tooltips.set_tip( *sp, gettext("The number of rounds the effect has been active.") );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_table.AddItem( TempHP, gettext("Temporary Hit Points"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric(true);
	f_tooltips.set_tip( *sp, gettext("Hit points to add to the character's temporary hit point pool. These points are used first, then hit points come off of the regular pool.") );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_table.AddItem( HPBoost, gettext("Hit Point Boost"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric(true);
	f_tooltips.set_tip( *sp, gettext("Hit points to add to the character's main hit point pool. When the effect expires, however, the character's real hit points will drop by this value as well. This works well for a constitution boost.") );
	//
	Gtk::ComboBoxText* cb = new Gtk::ComboBoxText;
	cb->append_text( gettext("Spell")   );
	cb->append_text( gettext("Ability") );
	cb->append_text( gettext("Other")   );
	cb->set_active(0);
	f_table.AddItem( Type, gettext("Type"), false, cb, NULL );
	f_tooltips.set_tip( *cb, gettext("The type of effect--either Spell, Ability or Other (user-defined).") );
}


void EffectEditor::OnAcceptButton()
{
	f_effect->name       ( GetText  ( f_table,	Name		) );
	f_effect->description( GetText  ( f_table,	Description	) );
	f_effect->totalRounds( GetValue ( f_table,	TotalRounds	) );
	f_effect->startIn    ( GetValue ( f_table,	StartIn		) );
	f_effect->roundsUsed ( GetValue ( f_table,	RoundsUsed	) );
	f_effect->tempHP     ( GetValue ( f_table,	TempHP		) );
	f_effect->hpBoost    ( GetValue ( f_table,	HPBoost		) );
	f_effect->type	     ( GetChoice( f_table,	Type		) );
	//
#if 0
	if( effect->name() == "" )
	{
		Glib::ustring message = gettext("You must type a name for the effect!");
		Gtk::MessageDialog dlg(	message,
					false,
					Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_OK,
					true );
		dlg.run();
	}
#endif
}


void EffectEditor::on_response( int response_id )
{
	if( response_id == Gtk::RESPONSE_OK )
	{
		OnAcceptButton();
	}
}


bool EffectEditor::on_key_press_event( GdkEventKey* event )
{
	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
		case GDK_Return:
		{
			if( f_modified )
			{
				response( Gtk::RESPONSE_OK );
				return true;
			}
		}
		break;
	}

	return Window::on_key_press_event(event);
}


void EffectEditor::SetEffect( Effect::Pointer effect )
{
	f_effect = effect;
	SetText  ( f_table,	Name		, f_effect->name       () );
	SetText  ( f_table,	Description	, f_effect->description() );
	SetValue ( f_table,	TotalRounds	, f_effect->totalRounds() );
	SetValue ( f_table,	StartIn		, f_effect->startIn    () );
	SetValue ( f_table,	RoundsUsed	, f_effect->roundsUsed () );
	SetValue ( f_table,	TempHP		, f_effect->tempHP     () );
	SetValue ( f_table,	HPBoost		, f_effect->hpBoost    () );
	SetChoice( f_table,	Type		, f_effect->type       () );
}


void EffectEditor::OnChanged()
{
	const moWCString	name		= GetText  ( f_table, Name );
	const moWCString	desc		= GetText  ( f_table, Description );
	const int			totalRounds	= GetValue ( f_table, TotalRounds );
	const int			startIn 	= GetValue ( f_table, StartIn	  );
	const int			roundsUsed 	= GetValue ( f_table, RoundsUsed  );
	const int			tempHP	 	= GetValue ( f_table, TempHP      );
	const int			hpBoost	 	= GetValue ( f_table, HPBoost     );
	const Effect::Type	type       	= GetChoice( f_table, Type );

	if( name	    != ""
		&& name	    != f_effect->name()	       )	f_modified = true;
	if( desc        != f_effect->description() )	f_modified = true;
	if( totalRounds != f_effect->totalRounds() )	f_modified = true;
	if( startIn	    != f_effect->startIn()	   )	f_modified = true;
	if( roundsUsed  != f_effect->roundsUsed()  )	f_modified = true;
	if( tempHP  	!= f_effect->tempHP()  	   )	f_modified = true;
	if( hpBoost  	!= f_effect->hpBoost()     )	f_modified = true;
	if( type        != f_effect->type()        )	f_modified = true;

	f_acceptButton->set_sensitive( f_modified );
}

void EffectEditor::ConnectAdjustments()
{
	for( auto& ac : f_adjConnections )
	{
		ac.f_connect = ac.f_adj->signal_value_changed().connect( sigc::mem_fun( *this, &EffectEditor::OnChanged ) );
	}
}

void EffectEditor::ConnectTables()
{
	for( auto& tc : f_tableConnections )
	{
		motk::LabelTable* table = tc.f_table;
		assert(table);
		tc.f_toggleConn = table->signal_toggled() .connect( sigc::mem_fun( *this, &EffectEditor::OnChanged ) );
		tc.f_tableConn  = table->signal_changed()  .connect( sigc::mem_fun( *this, &EffectEditor::OnChanged ) );
	}
}


void EffectEditor::DisconnectAdjustments()
{
	for( auto& ac : f_adjConnections )
	{
		ac.f_connect.disconnect();
	}
}

void EffectEditor::DisconnectTables()
{
	for( auto& tc : f_tableConnections )
	{
		tc.f_focusConn  .disconnect();
		tc.f_toggleConn .disconnect();
		tc.f_tableConn  .disconnect();
	}
}

}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

