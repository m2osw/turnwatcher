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
#if defined(OLD_UI)
	const moWCString	Name		("Name");
	const moWCString	PublicName	("PublicName");
	const moWCString	Type		("Type");
	const moWCString	Initiative	("Initiative");
	const moWCString	HitPoints	("HitPoints");
	const moWCString	TempHP		("TempHP");
	const moWCString	Damage		("Damage");
	const moWCString	Level		("Level");
#else
	const moWCString	Name		("Name");
	const moWCString	Type		("Type");
	const moWCString	HitDice		("HitDice");
	const moWCString	HitPoints	("HitPoints");
	const moWCString	CharEffects	("CharEffects");
	const moWCString	Initiative	("Initiative");
	const moWCString	TempHP		("TempHP");
	const moWCString	Damage		("Damage");
#endif

	const double g_lower_range	= -1000.0;
	const double g_upper_range	= 1000.0;
}


EditWindow::EditWindow( const QString& title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true  )
	, f_showing(false)
	, f_focusEntry(-1)
#if !defined(OLD_UI)
	, f_currentPage(0)
#endif
{
#if !defined(OLD_UI)
 	GetStats( f_abilityList, Stat::Abilities );
 	GetStats( f_skillList  , Stat::Skills    );
 	GetStats( f_saveList   , Stat::Saves     );

	f_textBuffer = Gtk::TextBuffer::create();
#endif

	// Update the lable below
	//
#if defined(OLD_UI)
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
#else
	f_baseTable     .set_row_spacings( 1 );
	f_statTable     .set_row_spacings( 1 );
	f_savesModTable .set_row_spacings( 1 );
	f_skillsModTable.set_row_spacings( 1 );
	//
	f_tableConnections.push_back( TableConnection( &f_baseTable        ) );
	f_tableConnections.push_back( TableConnection( &f_statTable        ) );
	f_tableConnections.push_back( TableConnection( &f_savesModTable    ) );
	f_tableConnections.push_back( TableConnection( &f_skillsModTable   ) );

	// Listen to page changes
	//
	f_notebook.signal_switch_page().connect( sigc::mem_fun( *this, &EditWindow::OnSwitchPage ) );
#endif

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


#if defined(OLD_UI)

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


#if 0
void EditWindow::InitStatTable()
{
	Gtk::Adjustment* adj;
	Gtk::SpinButton* sp;
	Gtk::Entry*	 ent;

	adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_statTable.AddItem( Spot, gettext(f_spotStat->name().c_str()), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The character's Spot skill modifier. Used in secret Spot checks.") );
	//
	adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_statTable.AddItem( Listen, gettext(f_listenStat->name().c_str()), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The character's Listen skill modifier. Used in secret Listen checks.") );
	//
	adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_statTable.AddItem( Will, gettext(f_willStat->name().c_str()), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The character's Will save modifier. Used in secret Will saves.") );
}
#endif


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


#else //defined(OLD_UI)


void EditWindow::InitBaseTable()
{
	Gtk::Adjustment* adj;
	Gtk::SpinButton* sp;
	Gtk::Entry*	 ent;

	f_baseTable.AddItem( Name, gettext("Character Name"), false, ent = new Gtk::Entry, NULL );
	f_tooltips.set_tip( *ent, gettext("A unique name for the character or monster.") );
	f_entryWidgets.push_back( ent );
	//
	Gtk::ComboBoxText* cb = new Gtk::ComboBoxText;
	cb->append_text( gettext("Character") );
	cb->append_text( gettext("Monster")   );
	cb->set_active(0);
	f_baseTable.AddItem( Type, gettext("Character Type"), false, cb, NULL );
	f_entryWidgets.push_back( cb );
	//
	adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_baseTable.AddItem( Initiative, gettext("Initiative"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The initiative modifier for the character.") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(10.0, 1.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_baseTable.AddItem( HitPoints, gettext("Base Hit Points"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The character's base hit points (without Con boost).") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_baseTable.AddItem( TempHP, gettext("Temporary Hit Points"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("Temporary hit points. These are used first, then damage is added to the damage column above.") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_baseTable.AddItem( Damage, gettext("Current Damage"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The current damage sustained in melee (or other ways).") );
	f_entryWidgets.push_back( sp );
	//
	adj = new Gtk::Adjustment(1.0, 1.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( adj ) );
	f_baseTable.AddItem( HitDice, gettext("Hit Dice"), false, sp = new Gtk::SpinButton(*adj), NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("The character's hit dice.") );
	f_entryWidgets.push_back( sp );
}


void EditWindow::AddStatEntry( const char* name, const int id )
{
	Gtk::Adjustment* stat_adj;
	Gtk::Adjustment* mod_adj;
	Gtk::SpinButton* sp1;
	Gtk::SpinButton* sp2;

	stat_adj = new Gtk::Adjustment(10.0, 0.0, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( stat_adj ) );
	mod_adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	f_adjConnections.push_back( AdjustmentConnection( mod_adj ) );
	f_statTable.AddItem( id, gettext(name), true /*checkbox*/,
			sp1 = new Gtk::SpinButton(*stat_adj),
			sp2 = new Gtk::SpinButton(*mod_adj),
			NULL );
	sp1->set_numeric( true );
	f_tooltips.set_tip( *sp1, gettext("The base stat.") ); 
	sp2->set_numeric( true );
	f_tooltips.set_tip( *sp2, gettext("A modifier added to the stat.") );
	f_entryWidgets.push_back( sp1 );
	f_entryWidgets.push_back( sp2 );	
}

void EditWindow::InitStatTable()
{
	f_statTable.AddHeader( gettext("Name"), gettext("Stat"), gettext("Mod"), NULL );

	for( auto stat : f_abilityList )
	{
		assert(stat);
		if( !stat->deleted() )
		{
			AddStatEntry( stat->name().c_str(), stat->id() );
		}
	}
}


void EditWindow::AddSavesTable()
{
	Gtk::SpinButton* sp1;
	Gtk::SpinButton* sp2;

	f_savesModTable.AddHeader( gettext("Name"), gettext("Base"), gettext("Mod"), NULL );
	//
	for( auto stat : f_saveList )
	{
		assert( stat );
		if( stat->deleted() )
			continue;
		Gtk::Adjustment* base_adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
		f_adjConnections.push_back( AdjustmentConnection( base_adj ) );
		Gtk::Adjustment* mod_adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
		f_adjConnections.push_back( AdjustmentConnection( mod_adj ) );
		f_savesModTable.AddItem( stat->id(), stat->name().c_str(), false,
				sp1 = new Gtk::SpinButton(*base_adj),
				sp2 = new Gtk::SpinButton(*mod_adj),
				NULL );
		sp1->set_numeric( true );
		f_tooltips.set_tip( *sp1, gettext("The base save.") );
		sp2->set_numeric( true );
		f_tooltips.set_tip( *sp2, gettext("An optional modifier to the save.") );
		f_entryWidgets.push_back( sp1 );
		f_entryWidgets.push_back( sp2 );	
	}
}


void EditWindow::AddSkillsTable()
{
	Gtk::SpinButton* sp1;
	Gtk::SpinButton* sp2;

	f_skillsModTable.AddHeader( gettext("Name"), gettext("Ranks"), gettext("Mod"), NULL );
	//
	for( auto stat : f_skillList )
	{
		if( stat->deleted() )
			continue;
		Gtk::Adjustment* ranks_adj = new Gtk::Adjustment(0.0, 0.0, g_upper_range);
		f_adjConnections.push_back( AdjustmentConnection( ranks_adj ) );
		Gtk::Adjustment* mod_adj   = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
		f_adjConnections.push_back( AdjustmentConnection( mod_adj ) );
		f_skillsModTable.AddItem( stat->id(), stat->name().c_str(), false,
				sp1 = new Gtk::SpinButton(*ranks_adj),
				sp2 = new Gtk::SpinButton(*mod_adj),
				NULL );
		sp1->set_numeric( true );
		f_tooltips.set_tip( *sp1, gettext("The base skill.") );
		sp2->set_numeric( true );
		f_tooltips.set_tip( *sp2, gettext("An optional modifier to the skill.") );
		f_entryWidgets.push_back( sp1 );
		f_entryWidgets.push_back( sp2 );	
	}
}

void EditWindow::GetStats( StatList& list, Stat::Type type )
{
	const StatManager::StatMap&	statMap = GetStatMgr()->GetStats( type );
	//
	list.clear();
	//
	for( auto statPair : statMap )
	{
		list.push_back( statPair.second );
	}
}


void EditWindow::InitModifierTable()
{
	Gtk::Adjustment* adj;

	AddSavesTable ();
	AddSkillsTable();
}
#endif


void EditWindow::FillMainBox()
{
#if defined(OLD_UI)
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
#if 0
	//
	f_notebook.append_page( *base_vbox,     	gettext("Base") );
	f_notebook.append_page( *frame, 	 		gettext("Columns") );
	//
	vbox->pack_start( f_notebook, true, true );
#else
	Gtk::HBox* hbox = Gtk::manage( new Gtk::HBox );
	hbox->pack_start( *base_vbox );
	hbox->pack_start( *frame );
	//
	vbox->pack_start( *hbox, true, true );
#endif

#else //defined(OLD_UI)

	GetStats( f_abilityList, Stat::Abilities );
	GetStats( f_skillList  , Stat::Skills  );
	GetStats( f_saveList   , Stat::Saves   );

	InitBaseTable();
	InitStatTable();
	InitModifierTable();

	Gtk::VBox* vbox = get_vbox();
	Gtk::VBox* mods_vbox = Gtk::manage(new Gtk::VBox);
	//
	Gtk::Frame* frame = Gtk::manage(new Gtk::Frame(gettext("Saves")));
	frame->add( f_savesModTable );
	mods_vbox->pack_start( *frame, true, true );
	//
	frame = Gtk::manage(new Gtk::Frame(gettext("Skills")));
	frame->add( f_skillsModTable );
	mods_vbox->pack_start( *frame, true, true );
	//
	Gtk::ScrolledWindow* scroller = Gtk::manage(new Gtk::ScrolledWindow);
	scroller->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC );
	scroller->set_shadow_type( Gtk::SHADOW_NONE );
	scroller->set_border_width( 0 );
	scroller->add( *mods_vbox );
	//
	f_notesEntry.set_wrap_mode( Gtk::WRAP_WORD );
	f_notesEntry.set_buffer( f_textBuffer );
	Gtk::ScrolledWindow* scroller2 = Gtk::manage(new Gtk::ScrolledWindow);
	scroller2->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	scroller2->set_shadow_type( Gtk::SHADOW_NONE );
	scroller2->set_border_width( 0 );
	scroller2->add( f_notesEntry );
	//
	f_notebook.append_page( f_baseTable,     gettext("Base") );
	f_notebook.append_page( f_statTable,     gettext("Abilities") );
	f_notebook.append_page( *scroller, 	 gettext("Modifiers") );
	f_notebook.append_page( *scroller2,      gettext("Notes") );
	f_notebook.append_page( f_effectsEditor, gettext("Effects") );
	//
	vbox->pack_start( f_notebook, true, true );
	//
	//vbox->pack_start( *Gtk::manage(new Gtk::HSeparator), false, false );
	//vbox->pack_start( f_infoBox, false, false );
	//vbox->pack_start( *Gtk::manage(new Gtk::HSeparator), false, false );
#endif

	show_all_children();
}


void EditWindow::ClearMainBox()
{
#if defined(OLD_UI)
	f_baseTable		.Clear();
	f_combatTable	.Clear();
	f_statTable		.Clear();
#else
	f_baseTable		.Clear();
	f_statTable		.Clear();
	f_savesModTable	.Clear();
	f_skillsModTable.Clear();
#endif

	for( auto adj : f_adjConnections )
	{
		delete adj.f_adj;
	}
	//
	f_adjConnections.clear();

#if 0
	Gtk::Notebook::PageList& pages = f_notebook.pages();
	pages.erase( pages.begin(), pages.end() );
#endif
}


namespace
{
	QString GetText( LabelTable& tbl, const moName& id )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		return e? e->get_text(): "error!";
	}

	void SetText( LabelTable& tbl, const moName& id, const QString& text )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		if( e ) e->set_text( text );
#ifdef DEBUG
		moWCString name( id );
		std::cout << "CharacterListUI: setting " << name.c_str() << " to " << text.c_str() << std::endl;
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

#if !defined(OLD_UI)
	void GetSave( LabelTable& tbl, const moName& id, SaveStat* save )
	{
		assert(save);
		Gtk::SpinButton* b = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		Gtk::SpinButton* m = (Gtk::SpinButton*) tbl.GetItem( id, 1 );
		save->base( b? atoi(b->get_text().c_str()): 0 );
		save->mod ( m? atoi(m->get_text().c_str()): 0 );
	}

	void SetSave( LabelTable& tbl, const moName& id, SaveStat* save )
	{
		assert(save);
		Gtk::SpinButton* b = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		Gtk::SpinButton* m = (Gtk::SpinButton*) tbl.GetItem( id, 1 );
		if( b ) b->set_value( save->base() );
		if( m ) m->set_value( save->mod()   );
	}

	void GetSkill( LabelTable& tbl, const moName& id, Skill* skill )
	{
		assert(skill);
		Gtk::SpinButton* r = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		Gtk::SpinButton* m = (Gtk::SpinButton*) tbl.GetItem( id, 1 );
		skill->ranks( r? atoi(r->get_text().c_str()): 0 );
		skill->mod  ( m? atoi(m->get_text().c_str()): 0 );
	}

	void SetSkill( LabelTable& tbl, const moName& id, Skill* skill )
	{
		Gtk::SpinButton* r = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		Gtk::SpinButton* m = (Gtk::SpinButton*) tbl.GetItem( id, 1 );
		if( r ) r->set_value( skill->ranks() );
		if( m ) m->set_value( skill->mod()   );
	}

	void GetStat( LabelTable& tbl, const moName& id, Combatant::Character& ch )
	{
		Gtk::SpinButton* s = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		Gtk::SpinButton* m = (Gtk::SpinButton*) tbl.GetItem( id, 1 );
		Ability* ability = dynamic_cast<Ability*>(static_cast<StatBase*>(ch.getStat( Stat::Abilities, id )));
		ability->enabled( tbl.GetCheck(id)? true: false );
		ability->score( s? atoi(s->get_text().c_str()): 0 );
	       	ability->mod  ( m? atoi(m->get_text().c_str()): 0 );
	}

	void SetStat( LabelTable& tbl, const moName& id, Combatant::Character& ch )
	{
		Gtk::SpinButton* s = (Gtk::SpinButton*) tbl.GetItem( id, 0 );
		Gtk::SpinButton* m = (Gtk::SpinButton*) tbl.GetItem( id, 1 );
		Ability* ability = dynamic_cast<Ability*>(static_cast<StatBase*>(ch.getStat( Stat::Abilities, id )));
		if( ability->enabled() ) tbl.SetCheck( id );
		if( s ) s->set_value( ability->score() );
		if( m ) m->set_value( ability->mod()   );
	}
#endif
}


#if !defined(OLD_UI)
void EditWindow::GetTypeValues( LabelTable& table, StatList& list )
{
	for( auto stat : list )
	{
		assert(stat);
		const Stat::Type type	= stat->type();
		const int id		= stat->id();

		StatBase* base = static_cast<StatBase*>(f_char->getStat( type, id ));
		assert(base);

		if( Stat::Skills == type )
		{
			Skill* skill = dynamic_cast<Skill*>(base);
			assert(skill);
			GetSkill( table, id, skill );
		}
		else
		{
			SaveStat* save = dynamic_cast<SaveStat*>(base);
			assert(save);
			GetSave( table, id, save );
		}
	}
}


void EditWindow::SetTypeValues( LabelTable& table, StatList& list )
{
	for( auto stat : list )
	{
		assert(stat);
		const Stat::Type type	= stat->type();
		const int id		= stat->id();

		StatBase* base = static_cast<StatBase*>(f_char->getStat( type, id ));
		assert(base);

		if( Stat::Skills == type )
		{
			Skill* skill = dynamic_cast<Skill*>(base);
			assert(skill);
			SetSkill( table, id, skill );
		}
		else
		{
			SaveStat* save = dynamic_cast<SaveStat*>(base);
			assert(save);
			SetSave( table, id, save );
		}
	}
}
#endif


void EditWindow::UpdateDlgFromChar()
{
    	if( !f_char )
	{
	    return;
	}

	auto statMgr( GetStatMgr().lock() );

#if defined(OLD_UI)
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

#else
	SetText  ( f_baseTable, Name,       f_char->name() );
	SetChoice( f_baseTable, Type,       f_char->monster() );
	SetValue ( f_baseTable,	Initiative, f_char->getMod( Stat::Misc, statMgr->initId() ) );
	SetValue ( f_baseTable, HitPoints,  f_char->baseHP() );
	SetValue ( f_baseTable,	TempHP,     f_char->tempHP() );
	SetValue ( f_baseTable,	Damage,     f_char->damage() );
	SetValue ( f_baseTable, HitDice,    f_char->hitDice() );

	for( auto stat : f_abilityList )
	{
		assert(stat);
		SetStat( f_statTable, stat->id(), *f_char );
	}

	SetTypeValues( f_savesModTable , f_saveList  );
	SetTypeValues( f_skillsModTable, f_skillList );

	f_textBuffer->set_text( f_char->notes().c_str() );

	// Set Effects
	//
	f_effectsEditor.AddEffects( f_char );
#endif
}


void EditWindow::UpdateCharFromDlg()
{
	if( !f_char )
	{
	    return;
	}

	auto statMgr( GetStatMgr().lock() );

#if defined(OLD_UI)
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
#else
	// by default, (char *) strings are taken as ISO-8859-1, here we need a UTF-8
	f_char->name    ( GetText   ( f_baseTable, Name      ) );
	f_char->monster ( GetChoice ( f_baseTable, Type      ) );
	f_char->baseHP  ( GetValue  ( f_baseTable, HitPoints ) );
	f_char->tempHP  ( GetValue  ( f_baseTable, TempHP    ) );
	f_char->damage  ( GetValue  ( f_baseTable, Damage    ) );
	f_char->hitDice ( GetValue  ( f_baseTable, HitDice   ) );
	//
	f_char->setMod( Stat::Misc, statMgr->initId(), GetValue( f_baseTable, Initiative ) );
	//
	for( auto stat : f_abilityList )
	{
		assert(stat);
		GetStat( f_statTable, stat->id(), *f_char );
	}
	//
	GetTypeValues( f_savesModTable , f_saveList  );
	GetTypeValues( f_skillsModTable, f_skillList );
	//
	f_char->notes( f_textBuffer->get_text() );

	// Get Effects
	//
	f_effectsEditor.GetEffects( f_char );
#endif

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
	std::cout << "EditWindow::on_show()" << std::endl;
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
	std::cout << "EditWindow::on_hide()" << std::endl;
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
#if !defined(OLD_UI)
	const bool on_notes_page = (f_currentPage == 3);
#endif

	switch( event->keyval )
	{
		case GDK_Return:
#if !defined(OLD_UI)
			if( !on_notes_page )
			{
#endif
				response( Gtk::RESPONSE_OK );
#if !defined(OLD_UI)
			}
#endif
			break;

		case GDK_Escape:
			response( Gtk::RESPONSE_CANCEL );
			break;

#if 0
		case GDK_Page_Up:
			f_currentPage = f_currentPage? f_currentPage - 1: f_notebook.pages().size() - 1;
			f_notebook.set_current_page( f_currentPage );
			//
#if !defined(OLD_UI)
			if( on_notes_page )
			{
				f_notesEntry.grab_focus();
			}
#endif
			break;

		case GDK_Page_Down:
			f_currentPage = f_currentPage < f_notebook.pages().size() - 1? f_currentPage + 1: 0;
			f_notebook.set_current_page( f_currentPage );
			//
#if !defined(OLD_UI)
			if( on_notes_page )
			{
				f_notesEntry.grab_focus();
			}
#endif
			break;
#endif
	}

	return return_value;
}


#if 0
void EditWindow::OnSwitchPage( GtkNotebookPage*, guint )
{
	f_currentPage = f_notebook.get_current_page();

#if !defined(OLD_UI)
	if( f_currentPage == 3 )
	{
		f_notesEntry.grab_focus();
	}
#endif
}
#endif


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

