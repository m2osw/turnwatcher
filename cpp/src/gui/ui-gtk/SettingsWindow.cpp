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
#include "SettingsWindow.h"
#include "common.h"
#include "stat.h"

// stdc
//
#include <math.h>

using namespace molib;

namespace UI
{

namespace
{
	// Label Table
	//
	const moWCString	InitDie					("InitDie");
	const moWCString	DeathThreshold			("DeathThreshold");
	const moWCString	CharListFont			("CharListFont");
	const moWCString	AltCharListFont			("AltCharListFont");
	
	// Tree view
	//
	const moWCString	UltraInit				("UltraInit");
	const moWCString	BleedOut				("BleedOut");
	const moWCString	SkipDead				("SkipDead");
	const moWCString	AltDeathRule			("AltDeathRule");
#ifdef WANT_EFFECTS
	const moWCString	NotifyExpiredEffects	("NotifyExpiredEffects");
#endif
	const moWCString	InitOnStart				("InitOnStart");
	const moWCString	ManualInit				("ManualInit");
	const moWCString	GetDC					("GetDC");

	const double g_lower_range	= -1000.0;
	const double g_upper_range	= 1000.0;
}


SettingsWindow::SettingsWindow( QString title, Gtk::Window& parent ) :
	Gtk::Dialog( title, parent, true, true  )
{
	// Update the lable below
	//
	f_table.set_row_spacings( 1 );

	f_initStat = GetStatMgr().lock()->GetStat( GetStatMgr().lock()->initId() );

	FillMainBox();
	UpdateDialog();

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
	
	show_all_children();
}


SettingsWindow::~SettingsWindow()
{
}


void SettingsWindow::InitTable()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	Gtk::ComboBoxEntryText* cb = new Gtk::ComboBoxEntryText;
	f_table.AddItem( InitDie, gettext("Initiative Dice (default: 1d20)"), false, cb, NULL );
	f_tooltips.set_tip( *cb, gettext("Die value used to roll Initiative internally. 1d20 is the default, but you may enter other values such as 2d10.") );

	Gtk::Adjustment* adj = new Gtk::Adjustment(0.0, g_lower_range, g_upper_range);
	Gtk::SpinButton* sp	 = new Gtk::SpinButton( *adj );

	f_table.AddItem( DeathThreshold, gettext("Death Threshold"), false, sp, NULL );
	sp->set_numeric( true );
	f_tooltips.set_tip( *sp, gettext("Hitpoint threshold value below 0 that indicates combatant death (default is -10).") );
	
	Gtk::FontButton* fb = new Gtk::FontButton( appSettings->CombatantListFont() );
	f_table.AddItem( CharListFont, gettext("Main Combatant List Font"), false, fb, NULL );
	f_tooltips.set_tip( *fb, gettext("Select the font to use for the main combatant list. Requires application restart.") );

	fb = new Gtk::FontButton( appSettings->AltCombatantListFont() );
	f_table.AddItem( AltCharListFont, gettext("Alternate Combatant List Font (for players)"), false, fb, NULL );
	f_tooltips.set_tip( *fb, gettext("Select the font to use for the player view combatant list. Requires application restart.") );
}


void SettingsWindow::InitTreeView()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	f_model = Gtk::ListStore::create( f_columns );
	f_treeView.set_model( f_model );

	f_treeView.append_column( gettext("Name"),   f_columns.f_name   );
	f_treeView.append_column_editable( gettext("Active"), f_columns.f_active );

	Gtk::TreeModel::Row row;

	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(UltraInit);
	row[f_columns.f_name]	= gettext("Use Ulta Initiative (roll initiative each round)");
	row[f_columns.f_active]	= appSettings->UltraInit();
	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(BleedOut);
	row[f_columns.f_name]	= gettext("Bleed Out Dying Combatants");
	row[f_columns.f_active]	= appSettings->BleedOutDying();
	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(SkipDead);
	row[f_columns.f_name]	= gettext("Skip Dead Combatants");
	row[f_columns.f_active]	= appSettings->SkipDead();
	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(AltDeathRule);
	row[f_columns.f_name]	= gettext("Use Alternate Death Rule (death at threshold minus character level)");
	row[f_columns.f_active]	= appSettings->AlternateDeathRule();
	//
#ifdef WANT_EFFECTS
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(NotifyExpiredEffects);
	row[f_columns.f_name]	= gettext("Notify when effects expire on combatant's turn");
	row[f_columns.f_active]	= appSettings->NotifyExpiredEffects();
#endif
	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(InitOnStart);
	row[f_columns.f_name]	= gettext("Roll Initiative on Start");
	row[f_columns.f_active]	= appSettings->InitOnStart();
	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(ManualInit);
	row[f_columns.f_name]	= gettext("Manual Initiative Entry");
	row[f_columns.f_active]	= appSettings->ManualInit();
	//
	row = *(f_model->append());
	row[f_columns.f_id]	= moName(GetDC);
	row[f_columns.f_name]	= gettext("Get DC on Rolls");
	row[f_columns.f_active]	= appSettings->GetDC();
}


void SettingsWindow::FillMainBox()
{
	InitTable();
	InitTreeView();

	Gtk::VBox* vbox = get_vbox();
	Gtk::Frame* frame;
	Gtk::VBox* frame_vbox;

	frame = Gtk::manage(new Gtk::Frame(gettext("Misc Options")));
	frame->add( f_table );
	frame_vbox = Gtk::manage(new Gtk::VBox);
	frame_vbox->pack_start( *frame, true, true );
	vbox->pack_start( *frame_vbox, true, true );
	//
	frame = Gtk::manage(new Gtk::Frame(gettext("Settings")));
	frame->add( f_treeView );
	frame_vbox = Gtk::manage(new Gtk::VBox);
	frame_vbox->pack_start( *frame, true, true );
	vbox->pack_start( *frame_vbox, true, true );

	show_all_children();
}


namespace
{
	int GetValue( motk::LabelTable& tbl, const moName& id )
	{
		Gtk::SpinButton* b;
		tbl.GetItem( id, b );
		return b? atoi(b->get_text().c_str()): -1;
	}

	void SetValue( motk::LabelTable& tbl, const moName& id, int value )
	{
		Gtk::SpinButton* b;
		tbl.GetItem( id, b );
		if( b ) b->set_value( value );
	}

#if 0
	QString GetText( motk::LabelTable& tbl, const moName& id )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		return e? e->get_text(): "1d20+0";
	}

	void SetText( motk::LabelTable& tbl, const moName& id, const QString& text )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		if( e ) e->set_text( text );
	}
#endif
	
	QString GetFontName( motk::LabelTable& tbl, const moName& id )
	{
		Gtk::FontButton* fb;
		tbl.GetItem( id, fb );
		return fb? fb->get_font_name(): "Unknown font";
	}
	
	void SetFontName( motk::LabelTable& tbl, const moName& id, const QString& text )
	{
		Gtk::FontButton* fb;
		tbl.GetItem( id, fb );
		if( fb ) fb->set_font_name( text );
	}
}


void SettingsWindow::SetDieFace()
{
	const QString dice_info(
			Common::DiceFacesModifierToStr
				( f_initStat->dice()
				, f_initStat->faces()
				, f_initStat->modifier()
				).c_str()
			);
	Gtk::ComboBoxEntryText* cb = 0;
	f_table.GetItem( InitDie, cb );
	assert(cb);
	cb->remove_text( dice_info );
	cb->append_text( dice_info );
	cb->set_active_text( dice_info );
}

void SettingsWindow::GetDieFace()
{
	int dice, faces, modifier;

	Gtk::ComboBoxEntryText* cb = 0;
	f_table.GetItem( InitDie, cb );
	assert(cb);
	const QString dice_info( cb->get_entry()->get_text() );
	Common::StrToDiceFacesModifier( dice_info.c_str(), dice, faces, modifier );
	f_initStat->dice     ( dice     );
	f_initStat->faces    ( faces    );
	f_initStat->modifier ( modifier );

	GetStatMgr().lock()->signal_changed().emit();

	auto appSettings(GetAppSettings().lock());
	assert(appSettings);
	//
    auto die_list( appSettings->InitDieStrings() );
    const auto match_iter = std::find_if( die_list.begin(), die_list.end(),
        [&]( const QString& s ) -> bool
        {
            return s == dice_info;
        });
    //
    if( match_iter != die_list.end() )
	{
		// Save strings with new entry
		//
        die_list.push_back( dice_info );
        appSettings->InitDieStrings( die_list );
	}
}


void SettingsWindow::UpdateDialog()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	Gtk::ComboBoxEntryText* cb = 0;
	f_table.GetItem( InitDie, cb );
	assert(cb);
	cb->clear_items();

	for( auto str : appSettings->InitDieStrings() )
	{
		cb->append_text( str );
	}

	SetDieFace();

	SetValue( f_table, DeathThreshold, appSettings->DeathThreshold() );
	
	SetFontName( f_table, CharListFont   , appSettings->CombatantListFont()    );
	SetFontName( f_table, AltCharListFont, appSettings->AltCombatantListFont() );
}


void SettingsWindow::SaveValues()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	GetDieFace();

	appSettings->DeathThreshold      ( GetValue   ( f_table, DeathThreshold  ) );
	appSettings->CombatantListFont   ( GetFontName( f_table, CharListFont    ) );
	appSettings->AltCombatantListFont( GetFontName( f_table, AltCharListFont ) );

	Gtk::TreeModel::iterator	iter = f_model->children().begin();
	const Gtk::TreeModel::iterator	end  = f_model->children().end();
	//
	for( ; iter != end; ++iter )
	{
		Gtk::TreeModel::Row row = *iter;
		const mo_name_t	id     = row[f_columns.f_id];
		const bool		active = row[f_columns.f_active];

		if( id == (mo_name_t) moName(UltraInit)	) { appSettings->UltraInit( active );		}
		if( id == (mo_name_t) moName(BleedOut)	) { appSettings->BleedOutDying( active );	}
		if( id == (mo_name_t) moName(SkipDead)	) { appSettings->SkipDead( active );		}
		if( id == (mo_name_t) moName(AltDeathRule)) { appSettings->AlternateDeathRule( active );	}
#ifdef WANT_EFFECTS
		if( id == (mo_name_t) moName(NotifyExpiredEffects)) { appSettings->NotifyExpiredEffects( active );}
#endif
		if( id == (mo_name_t) moName(InitOnStart)		) { appSettings->InitOnStart( active );		}
		if( id == (mo_name_t) moName(ManualInit)			) { appSettings->ManualInit( active );		}
		if( id == (mo_name_t) moName(GetDC)			) { appSettings->GetDC( active );		}
	}

	appSettings->signal_changed().emit();
}


void SettingsWindow::on_show()
{
#ifdef DEBUG
	std::cout << "SettingsWindow::on_show()" << std::endl;
#endif

#if 0
	Gtk::Widget* lastFocus = f_table.GetLastFocus();
	if( lastFocus )
		set_focus( *lastFocus );

	UpdateDialog();
#endif

	Gtk::Dialog::on_show();
}


void SettingsWindow::on_hide()
{
#ifdef DEBUG
	std::cout << "SettingsWindow::on_hide()" << std::endl;
#endif

	Gtk::Dialog::on_hide();
}


void SettingsWindow::on_response( int response_id )
{
	if( response_id == Gtk::RESPONSE_OK )
	{
		SaveValues();
	}
}


bool SettingsWindow::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Dialog::on_key_press_event(event);

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


}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

