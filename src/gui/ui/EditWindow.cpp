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
#include "EditWindow.h"

#include <math.h>

using namespace molib;
using namespace motk;
using namespace Attribute;

namespace UI
{

namespace
{
	const moWCString	Name		("Name");
	const moWCString	PublicName	("PublicName");
	const moWCString	Type		("Type");
	const moWCString	Initiative	("Initiative");
	const moWCString	HitPoints	("HitPoints");
	const moWCString	TempHP		("TempHP");
	const moWCString	Damage		("Damage");
	const moWCString	Level		("Level");

	const double g_lower_range	= -1000.0;
	const double g_upper_range	= 1000.0;
}


EditWindow::EditWindow( const Glib::ustring& title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true  )
	, f_showing(false)
	, f_focusEntry(-1)
{
	// Update the lable below
	//
	f_baseTable  .set_row_spacings( 1 );
	f_combatTable.set_row_spacings( 1 );
	f_statTable  .set_row_spacings( 1 );
	//
	f_tableConnections.push_back( TableConnection( &f_baseTable        ) );
	f_tableConnections.push_back( TableConnection( &f_combatTable      ) );
	f_tableConnections.push_back( TableConnection( &f_statTable        ) );

	// Listen to page changes
	//
	//f_notebook.signal_switch_page().connect( sigc::mem_fun( *this, &EditWindow::OnSwitchPage ) );

	//
	// Lower buttons
	//
	if( alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) ) )
	{
		add_button( Gtk::Stock::OK      , Gtk::RESPONSE_OK     	);
		add_button( Gtk::Stock::CANCEL  , Gtk::RESPONSE_CANCEL 	);
	}
	else
	{
		add_button( Gtk::Stock::CANCEL  , Gtk::RESPONSE_CANCEL 	);
		add_button( Gtk::Stock::OK      , Gtk::RESPONSE_OK     	);
	}

	// StatManager
	//
	auto statMgr( GetStatMgr().lock() );
	assert(statMgr);
	statMgr->signal_changed().connect( sigc::mem_fun( *this, &EditWindow::OnReloadTables ) );

	FillMainBox();
	show_all_children();
}


EditWindow::~EditWindow()
{
	ClearMainBox();
}


void EditWindow::GetStats()
{
	typedef std::vector<Attribute::Stat::pointer_t> StatList;
	StatList statList;
	//
	for( auto statPair : GetStatMgr().lock()->GetStats() )
	{
		auto stat( statPair.second );
		if( !stat->internal() && !stat->deleted() )
		{
			statList.push_back( stat );
		}
	}
	//
	for( auto stat : statList )
	{
		f_statMap[stat->order()] = stat;
	}
}


void EditWindow::InitBaseTable()
{
	Gtk::Entry*	 ent;

	f_baseTable.AddItem( Name, gettext("Combatant Name"), false, ent = new Gtk::Entry, NULL );
	f_tooltips.set_tip( *ent, gettext("A unique name for the character or monster.") );
	f_entryWidgets.push_back( ent );
	//
	f_baseTable.AddItem( PublicName, gettext("Public Name"), false, ent = new Gtk::Entry, NULL );
	f_tooltips.set_tip( *ent, gettext("This is the name shown on the HUD display. Be careful what you put here as your players can see it!") );
	f_entryWidgets.push_back( ent );
	//
	Gtk::ComboBoxText* cb = new Gtk::ComboBoxText;
	cb->append_text( gettext("Character") );
	cb->append_text( gettext("Monster")   );
	cb->set_active(0);
	f_baseTable.AddItem( Type, gettext("Character Type"), false, cb, NULL );
 	f_entryWidgets.push_back( cb );
}


void EditWindow::InitCombatTable()
{
	Gtk::Adjustment* adj;
	Gtk::SpinButton* sp;

	adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_combatTable.AddItem( Initiative, gettext("Initiative"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The initiative modifier for the character.") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(10.0, 1.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_combatTable.AddItem( HitPoints, gettext("Hit Points"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The character's total hit points.") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_combatTable.AddItem( TempHP, gettext("Temporary Hit Points"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("Temporary hit points. These are used first, then damage is added to the damage column above.") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_combatTable.AddItem( Damage, gettext("Current Damage"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The current damage sustained in melee (or other ways).") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_combatTable.AddItem( Level, gettext("Level"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The level of the combatant. "
		"This can be used optionally with the Alternate Death Rule where the combatant "
		"is not considered dead until he is at -(10+level) hitpoints.") );
	f_entryWidgets.push_back( sp );
}


void EditWindow::AddStatEntry( const char* name, const mo_name_t id )
{
	Gtk::Adjustment* stat_adj;
	Gtk::SpinButton* sp1;

	stat_adj = new Gtk::Adjustment(10.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( stat_adj ) );
	f_statTable.AddItem( id, gettext(name), false /*checkbox*/,	sp1 = new Gtk::SpinButton(*stat_adj), NULL );
	sp1->set_numeric( true );
	f_tooltips.set_tip( *sp1, gettext("Add a modifier to boost (or lower) the result.") ); 
	f_entryWidgets.push_back( sp1 );
}


void EditWindow::AddStatNotes( const char* name, const mo_name_t id )
{
	Gtk::Entry* entry;
	f_statTable.AddItem( id, gettext(name), false /*checkbox*/,	entry = new Gtk::Entry,	NULL );
	f_tooltips.set_tip( *entry, gettext("Add a note to appear on the grid.") ); 
	f_entryWidgets.push_back( entry );
}


void EditWindow::InitStatTable()
{
	f_statTable.AddHeader( gettext("Name"), gettext("Mod"), NULL );

	for( auto statPair : f_statMap )
	{
		auto stat( statPair.second );
		if( stat->showOnToolbar() )
		{
			AddStatEntry( stat->name().c_str(), stat->id() );
		}
		else
		{
			AddStatNotes( stat->name().c_str(), stat->id() );
		}
	}
}


void EditWindow::FillMainBox()
{
	GetStats();

	InitBaseTable();
	InitCombatTable();
	InitStatTable();

	Gtk::VBox* vbox = get_vbox();
	Gtk::Frame* frame;
	Gtk::VBox*	frame_vbox;
	Gtk::VBox*	base_vbox = new Gtk::VBox;
	//
	frame = Gtk::manage(new Gtk::Frame(gettext("Base")));
	frame->add( f_baseTable );
	frame_vbox = Gtk::manage(new Gtk::VBox);
	frame_vbox->pack_start( *frame, true, true );
	base_vbox->pack_start( *frame_vbox, true, true );
	//
	frame = Gtk::manage(new Gtk::Frame(gettext("Combat")));
	frame->add( f_combatTable );
	frame_vbox = Gtk::manage(new Gtk::VBox);
	frame_vbox->pack_start( *frame, true, true );
	base_vbox->pack_start( *frame_vbox, true, true );
	//
	Gtk::ScrolledWindow* scroller = Gtk::manage(new Gtk::ScrolledWindow);
	scroller->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC );
	scroller->set_shadow_type( Gtk::SHADOW_NONE );
	scroller->set_border_width( 0 );
	scroller->add( f_statTable );
	frame = Gtk::manage(new Gtk::Frame(gettext("Stats")));
	frame->add( *scroller );
	Gtk::HBox* hbox = Gtk::manage( new Gtk::HBox );
	hbox->pack_start( *base_vbox );
	hbox->pack_start( *frame );
	//
	vbox->pack_start( *hbox, true, true );

	show_all_children();
}


void EditWindow::ClearMainBox()
{
	f_baseTable		.Clear();
	f_combatTable	.Clear();
	f_statTable		.Clear();

	for( auto adj : f_adjConnections )
	{
		delete adj.f_adj;
	}
	//
	f_adjConnections.clear();
}


namespace
{
	Glib::ustring GetText( LabelTable& tbl, const moName& id )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		return e? e->get_text(): "error!";
	}

	void SetText( LabelTable& tbl, const moName& id, const Glib::ustring& text )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		if( e ) e->set_text( text );
#ifdef DEBUG
		moWCString name( id );
		std::cerr << "CharacterListUI: setting " << name.c_str() << " to " << text.c_str() << std::endl;
#endif
	}

	int GetChoice( LabelTable& tbl, const moName& id )
	{
		Gtk::ComboBoxText* c;
		tbl.GetItem( id, c );
		return c? c->get_active_row_number(): -1;
	}

	void SetChoice( LabelTable& tbl, const moName& id, const bool value )
	{
		Gtk::ComboBoxText* c;
		tbl.GetItem( id, c );
		if( c ) c->set_active( value );
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

	void GetNotes( LabelTable& tbl, const moName& id, Combatant::Character& ch )
	{
		Gtk::Entry* e = (Gtk::Entry*) tbl.GetItem( id, 0 );
		ch.setNotes( id, e? e->get_text().c_str(): 0 );
	}

	void SetNotes( LabelTable& tbl, const moName& id, Combatant::Character& ch )
	{
		Gtk::Entry* e = (Gtk::Entry*) tbl.GetItem( id, 0 );
		if( e ) e->set_text( ch.getNotes( id ).c_str() );
	}

	void GetStat( LabelTable& tbl, const moName& id, Combatant::Character& ch )
	{
		Gtk::SpinButton* s = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		ch.setMod( id, s? atoi(s->get_text().c_str()): 0 );
	}

	void SetStat( LabelTable& tbl, const moName& id, Combatant::Character& ch )
	{
		Gtk::SpinButton* s = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		if( s ) s->set_value( ch.getMod( id ) );
	}
}


void EditWindow::UpdateDlgFromChar()
{
    	if( !f_char )
	{
	    return;
	}

	auto statMgr( GetStatMgr().lock() );

	SetText   ( f_baseTable,   Name,       f_char->name() );
	SetText   ( f_baseTable,   PublicName, f_char->public_name()  );
	SetChoice ( f_baseTable,   Type,       f_char->monster() );
	SetValue  ( f_combatTable, Initiative, f_char->getMod( statMgr->initId() ) );
	SetValue  ( f_combatTable, HitPoints,  f_char->baseHP() );
	SetValue  ( f_combatTable, TempHP,     f_char->tempHP() );
	SetValue  ( f_combatTable, Damage,     f_char->damage() );
	SetValue  ( f_combatTable, Level,      f_char->getMod( statMgr->levelId() ) );

	for( auto statPair : f_statMap )
	{
		const auto stat( statPair.second );
		if( stat->showOnToolbar() )
		{
			SetStat( f_statTable, stat->id(), *f_char );
		}
		else
		{
			SetNotes( f_statTable, stat->id(), *f_char );
		}
	}
}


void EditWindow::UpdateCharFromDlg()
{
	if( !f_char )
	{
	    return;
	}

	auto statMgr( GetStatMgr().lock() );

	f_char->name       ( GetText  ( f_baseTable, Name ) );
	f_char->public_name( GetText  ( f_baseTable, PublicName ) );
	f_char->monster    ( GetChoice( f_baseTable, Type ) );
	//
	f_char->setMod ( statMgr->initId(), GetValue( f_combatTable, Initiative ) );
	f_char->baseHP ( GetValue ( f_combatTable, HitPoints ) );
	f_char->tempHP ( GetValue( f_combatTable, TempHP     ) );
	f_char->damage ( GetValue( f_combatTable, Damage     ) );

	f_char->setMod ( statMgr->levelId()   , GetValue( f_combatTable, Level ) );

	for( auto pair : f_statMap )
	{
		const auto stat( pair.second );
		if( stat->showOnToolbar() )
		{
			GetStat( f_statTable, stat->id(), *f_char );
		}
		else
		{
			GetNotes( f_statTable, stat->id(), *f_char );
		}
	}

	//f_charConnection = f_char->signal_changed().connect( sigc::mem_fun( *this, &EditWindow::UpdateDlgFromChar ) );

	f_changedSignal.emit();
}


void EditWindow::UpdateDialog()
{
	UpdateCharFromDlg();
}


void EditWindow::OnSetFocus( LabelTable* table )
{
	if( f_showing )
	{
		Gtk::Widget* focusWidget = table->GetLastFocus();
		//
		int idx = 0;
		for( auto widget : f_entryWidgets )
		{
			if( widget == focusWidget )
			{
				f_focusEntry = idx;
				break;
			}
			++idx;
		}
		//
		UpdateDialog();
	}
}


void EditWindow::OnToggled()
{
	UpdateDialog();
}


void EditWindow::on_show()
{
#ifdef DEBUG
	std::cerr << "EditWindow::on_show()" << std::endl;
#endif

	for( auto& adj : f_adjConnections )
	{
		adj.f_connect = adj.f_adj->signal_value_changed().connect( sigc::mem_fun( *this, &EditWindow::UpdateDialog ) );
	}

	for( auto& table : f_tableConnections )
	{
		motk::LabelTable* ltable( table.f_table );
		assert(ltable);
		table.f_focusConn  = ltable->signal_set_focus() .connect( sigc::bind    ( sigc::mem_fun( *this, &EditWindow::OnSetFocus ), ltable ) );
		table.f_toggleConn = ltable->signal_toggled()   .connect( sigc::mem_fun ( *this, &EditWindow::OnToggled    ) );
		table.f_tableConn  = ltable->signal_set_focus() .connect( sigc::mem_fun ( *this, &EditWindow::UpdateDialog ) );
	}

	UpdateDialog();

	Gtk::Dialog::on_show();
	
	if( (f_focusEntry > -1) && (f_focusEntry < f_entryWidgets.size()) )
	{
		set_focus( *f_entryWidgets[f_focusEntry] );
	}
	
	f_showing = true;
}


void EditWindow::on_hide()
{
#ifdef DEBUG
	std::cerr << "EditWindow::on_hide()" << std::endl;
#endif
	f_showing = false;

	for( auto& adj : f_adjConnections )
	{
		adj.f_connect.disconnect();
	}

	for( auto& table : f_tableConnections )
	{
		table.f_focusConn .disconnect();
		table.f_toggleConn.disconnect();
		table.f_tableConn .disconnect();
	}

	//f_charConnection.disconnect();

	Gtk::Dialog::on_hide();
}


bool EditWindow::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Dialog::on_key_press_event(event);

	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
		case GDK_Return:
			response( Gtk::RESPONSE_OK );
			break;

		case GDK_Escape:
			response( Gtk::RESPONSE_CANCEL );
			break;
	}

	return return_value;
}


void EditWindow::OnReloadTables()
{
	f_tooltips.disable();
	UpdateCharFromDlg();
	ClearMainBox();
	//
	InitBaseTable();
	InitCombatTable();
	InitStatTable();
	show_all_children();
	//
	f_tooltips.enable();
	UpdateDialog();
}


void EditWindow::GetData( Combatant::Character::pointer_t ch )
{
	if( !ch ) return;
	UpdateCharFromDlg();
	ch->Copy( f_char );
}


void EditWindow::SetData( Combatant::Character::pointer_t ch )
{
	if( !ch ) return;
	f_char.reset( new Combatant::Character );
	f_char->Copy( ch );
	UpdateDlgFromChar();
}

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

