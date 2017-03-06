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


// local
//
#include "common.h"
#include "MainWindow.h"
#include "AboutWindow.h"
#include "DuplicateWindow.h"
#include "EditWindow.h"
#include "SettingsWindow.h"
#include "StatWindow.h"
#include "transactions.h"

// molib
//
#include "mo/mo_application.h"
#include "mo/mo_file.h"
#include "mo/mo_props.h"
#include "mo/mo_props_xml.h"

// stl
//
#include <algorithm>
#include <map>
#include <sstream>

#ifdef DARWIN
#include "sync-menu.h"
#endif

#ifdef INCLUDED_PIXMAPS
#	include "pixmaps/Helmet.xpm"
#endif

#define SAVE_TIMEOUT_SECS	10

using namespace Application;
using namespace Attribute;
using namespace Effects;
using namespace molib;
using namespace motk;
using namespace Transactions;

namespace UI
{

#if !defined(DEMO_VERSION)
namespace
{
	const char*	g_app_ext	= ".turnwatcher";
}
#endif


MainWindow::MainWindow()
	: f_isSaving		(false)
	, f_toolbarId		(0)
	, f_menubarId		(0)
	, f_lastCharNum		(0)
	, f_lastDupInput	(1)
#if defined(DEMO_VERSION)
	, f_warnNoSave		(true)
#endif
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	, f_nagWindow		( *this )
#endif
#ifdef WANT_EFFECTS
	, f_effectsFrame	( gettext("Running Effects") )
	, f_effectsBook		( this )
#endif
#ifdef WANT_NOTES
	, f_infoWindow		( gettext("Notes"), *this )
#endif
	, f_mainMenuBar		( 0 )
	, f_mainToolBar		( 0 )
	, f_effectsToolBar	( 0 )
	, f_splash			(this)
	, f_lastEditFocus	(-1)
	, f_roundsHelper    ( new RoundsHelper( this, &f_charView, &f_hudView ) )
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);
	//
	auto actionsMgr(GetActionsMgr().lock());
	assert(actionsMgr);
	//
	auto charMgr(GetCharacterMgr().lock());
	assert(charMgr);
	//
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);
	//
	auto transMgr( GetTransactionMgr().lock() );
	assert(transMgr);

	f_splash.show();
	f_splash_auto_hide = Glib::signal_timeout().connect(
				sigc::mem_fun(*this, &MainWindow::on_splash_auto_hide),
				5 * 1000);	// show for 5 sec. maximum

	
	InitActions();

	// Init UI
	//
	// NOTE: The BETA need to look like a DEMO and therefore
	//	 we want the DEMO Splash screen and title.
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	// WARNING: DO NOT USE gettext() ON THE DEMO VERSION
	set_title( "Turn Watcher (tm) Demo" );
#else
	set_title( gettext("Turn Watcher (tm)") );
#endif
	//
	appSettings->StatusMsg( gettext("Ready") );

	// Add Widgets
	//
	f_charView.AddDefaultColumns();
	f_charView.set_border_width( 1 );
	f_scrolledWindow.add( f_charView );
	f_scrolledWindow.set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	f_scrolledWindow.set_shadow_type( Gtk::SHADOW_IN  );
	f_scrolledWindow.set_border_width( 1 );
#ifdef WANT_EFFECTS
	//
	f_effectsBook.set_border_width(10);
#endif
	//
	FillMainBox();
	add( f_mainBox );

	// Hook up signals from the character manager so other interested objects can be notified when
	// characters are added or deleted
	//
	transMgr->signal_update().connect( sigc::mem_fun( *this, &MainWindow::UpdateMenus ) );
	//
	initMgr->signal_start_rounds() .connect( sigc::bind( sigc::mem_fun( *this, &MainWindow::UpdateUI ), true ) );
	initMgr->signal_end_rounds()   .connect( sigc::bind( sigc::mem_fun( *this, &MainWindow::UpdateUI ), true ) );

	// Load previous state
	//
	Load();
	//
	if( appSettings->X() == -1 )
	{
		set_position( Gtk::WIN_POS_CENTER );
		set_default_size( appSettings->Width(), appSettings->Height() );
	}
	else
	{
		set_default_size( appSettings->Width(), appSettings->Height() );
		move( appSettings->X(), appSettings->Y() );
	}

#ifdef WANT_PANE
	f_pane.set_position( appSettings->PanePosition() );
#endif

	// Set the default application icon
	//
#ifdef INCLUDED_PIXMAPS
	set_icon( Gdk::Pixbuf::create_from_xpm_data( Helmet_xpm ) );
#else
	set_icon( Gdk::Pixbuf::create_from_file( Common::GetFullPathnameForImage( "Helmet.xpm" ).c_str() ) );
#endif

	f_actionViewShowToolbar->set_active( appSettings->ShowToolbar() );
	f_actionViewToolbarOnBottom->set_active( appSettings->ToolBarPos() == AppSettings::TBP_BOTTOM );
#ifdef WANT_EFFECTS
	f_actionViewShowEffects->set_active( appSettings->ShowEffects() );
#endif
#ifdef WANT_NOTES
	f_actionViewShowInfo->set_active( appSettings->ShowInfo() );
#endif
	f_actionViewShowHUD->set_active( appSettings->ShowHUD() );
	ConnectEvents();

	// Setup idle handler for periodic saves
	//
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &MainWindow::OnTimer), SAVE_TIMEOUT_SECS * 1000 );

	// Notify when AppSettings change
	//
	appSettings->signal_changed().connect( sigc::mem_fun(*this, &MainWindow::OnSettingsChanged) );

	// Hook up click/double-click action
	//
	f_charView.signal_double_click().connect( sigc::mem_fun(*this, &MainWindow::OnDoubleClick) );
	f_charView.signal_selection_changed().connect( sigc::mem_fun(*this, &MainWindow::OnSelectionChanged) );
	//
#ifdef WANT_EFFECTS
	f_charView.signal_selection_changed().connect( sigc::mem_fun(f_effectsBook, &EffectsBook::OnSelectionChanged) );
	charMgr->signal_cleared().connect( sigc::mem_fun(f_effectsBook, &EffectsBook::Clear) );
#endif

#ifdef WANT_NOTES
	f_charView.signal_selection_changed().connect( sigc::mem_fun(f_infoWindow, &InfoWindow::OnSelectionChanged) );
	f_infoWindow.signal_edit().connect( sigc::mem_fun( *this, &MainWindow::OnEditCharacter ) );
	charMgr->signal_cleared().connect( sigc::mem_fun(f_infoWindow, &InfoWindow::OnClear) );
#endif
	
	// Show app
	//
	f_statusBox.Update();
	//
	show_all_children();
	f_charView.grab_focus();

#if defined(DEMO_VERSION)
	f_startTime = time(0);
	ConnectNagEvent( true );
#endif
#ifdef DEBUG
	std::cout << "MainWindow initialized.\n";
#endif
	
	motk::AccelGroupPtr accelGroup = actionsMgr->GetAccelGroup();
	add_accel_group( accelGroup );
	//
	// Once we create the actions, we must get the menu bar/toolbars
	//
	//f_mainMenuBar	 = actionsMgr->GetMenubar();
	f_mainToolBar	 = actionsMgr->GetMainToolbar();
	f_effectsToolBar = actionsMgr->GetEffectsToolbar();
	//
	CreateEffectsFrame();
	
	UpdateUI( true );
}


MainWindow::~MainWindow()
{
	if( GetAppSettings().lock()->ShowHUD() )
	{
		f_hudView.hide();
	}
	//
	Save();
	GetTransactionMgr().lock()->clear();
	GetCharacterMgr().lock()->Clear();
}


bool MainWindow::OnTimer()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	if( appSettings->Modified() )
	{
		f_statusBox.PushMessage( "Saving..." );
		Save();
		f_statusBox.PopMessage();
		appSettings->Modified( false );
	}
	return true;
}


void MainWindow::FileImport()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);
	//
	auto charMgr(GetCharacterMgr().lock());
	assert(charMgr);

#if defined(DEMO_VERSION)
	ShowNagWindow( NagWindow::NAG_FEATURE );
#else
	Actions::Desensitizer	ad;
    moApplicationSPtr	application( moApplication::Instance() );
	Gtk::FileChooserDialog filedlg( *this, gettext("Select Turn Watcher file to import"), Gtk::FILE_CHOOSER_ACTION_OPEN );
	filedlg.set_transient_for(*this);
	filedlg.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	filedlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

	filedlg.set_current_folder( appSettings->UserPath().c_str() );

	Gtk::FileFilter filter_text;
	filter_text.set_name(gettext("Turn Watcher v1 files"));
	QString ext( "*" );
	ext += g_app_ext;
	filter_text.add_pattern(ext);
	filedlg.add_filter(filter_text);

	Gtk::FileFilter filter_any;
	filter_any.set_name(gettext("Any files"));
	filter_any.add_pattern("*");
	filedlg.add_filter(filter_any);

	const int result = filedlg.run();
	filedlg.hide();

	if( result == Gtk::RESPONSE_OK )
	{
		const QString filename( filedlg.get_filename() );
		appSettings->UserPath( filedlg.get_current_folder() );

		moPropBagRef charPropBag( "CHARACTERS" );
		const int ret_val = moXMLLoadPropBag( filename.c_str(), charPropBag );
		//
		// 0 means file exists, -1 means not there (or error)
		//
		if( ret_val == 0 )
		{
			QString file_version;
			if( appSettings->GetVersion( charPropBag, file_version ) )
			{
				Transaction::pointer_t signal_tr( new SignalCharactersTransaction );
				TransactionGroupHelper helper( "Import New Characters" );
				helper.AddTransaction( signal_tr );
				charMgr->LoadCharacters( charPropBag, helper.GetGroup(), false /*emit_signals*/ );
				helper.AddTransaction( signal_tr );
			}
			else
			{
				LegacyApp lapp;
				if( lapp.LoadCharacters( charPropBag ) )
				{
                    Combatant::LegacyCharacter::list_t charList;
                    lapp.GetCharList( charList );
					TransactionGroupHelper helper( "Import New Characters" );
                    charMgr->SetLegacyCharacters( charList, helper.GetGroup(), true /*emit_signals*/ );
				}
				else
				{
					QString message( gettext("Cannot import Turn Watcher legacy file! The file must be corrupted.") );
					Gtk::MessageDialog dlg( 
							message,
							false,
							Gtk::MESSAGE_ERROR,
							Gtk::BUTTONS_OK,
							true
							);
					dlg.set_transient_for(*this);
					dlg.run();
				}
			}
		}
	}
#endif
}


void MainWindow::FileExport()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);
	//
	auto charMgr(GetCharacterMgr().lock());
	assert(charMgr);

#if defined(DEMO_VERSION)
	ShowNagWindow( NagWindow::NAG_FEATURE );
#else
	Actions::Desensitizer ad;
	moApplicationSPtr application( moApplication::Instance() );

	Gtk::FileChooserDialog filedlg( *this, gettext("Select file to export to"), Gtk::FILE_CHOOSER_ACTION_SAVE );
	filedlg.set_transient_for(*this);
	filedlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	filedlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

	filedlg.set_current_folder( appSettings->UserPath().c_str() );
	filedlg.set_do_overwrite_confirmation( true );

	Gtk::FileFilter filter_text;
	filter_text.set_name(gettext("Turn Watcher v1 files"));
	QString ext = "*";
	ext += g_app_ext;
	filter_text.add_pattern(ext);
	filedlg.add_filter(filter_text);

	Gtk::FileFilter filter_any;
	filter_any.set_name(gettext("Any files"));
	filter_any.add_pattern("*");
	filedlg.add_filter(filter_any);

	bool done = false;

	while( !done )
	{
		done = true;
		const int result = filedlg.run();
		filedlg.hide();

		if( result == Gtk::RESPONSE_OK )
		{
			QString filename( filedlg.get_filename() );
			const int ext = filename.find( g_app_ext );
			if( ext == QString::npos )
			{
				filename += g_app_ext;
			}

			appSettings->UserPath( filedlg.get_current_folder().c_str() );
			moPropBagRef charPropBag( "CHARACTERS" );
			charPropBag.NewProp();
			charMgr->SaveCharacters( charPropBag );
			appSettings->AddVersion( charPropBag );
			//
			moXMLSavePropBag( filename.c_str(), charPropBag );
		}
	}
#endif
}


void MainWindow::FileClear()
{
	TransactionGroupHelper helper( gettext("Clear out all characters") );
    Transaction::pointer_t tr( new ClearCharactersTransaction );
    helper.AddTransaction( tr );
}


void MainWindow::FilePreferences()
{
	Actions::Desensitizer	ad;
	
	SettingsWindow	dlg( gettext("Preferences"), *this );
	dlg.set_transient_for(*this);
	if( dlg.run() == Gtk::RESPONSE_OK )
	{
		UpdateUI( false );
		Combatant::Character::list_t charList = GetCharacterMgr().lock()->GetCharacters();
		std::for_each( charList.begin(), charList.end(), []( Combatant::Character::pointer_t ch ) { ch->signal_changed().emit(); } );
		//GetCharacterModel().lock()->update();
	}
}


void MainWindow::FileStatManager()
{
	bool changed = false;
	{
		EventDisabler ed( this );
	
		StatWindow dlg( gettext("Manage Stats"), *this );
		dlg.set_transient_for(*this);
		if( dlg.run() == Gtk::RESPONSE_OK )
		{
			GetActionsMgr().lock()->HandleSoftColumnChange();
			changed = true;
		}
	}
	
	if( changed )
	{
		UpdateUI( true );
	}
}


void MainWindow::FileQuit()
{
	hide();
}


void MainWindow::EditUndo()
{
    GetTransactionMgr().lock()->undo();
}


void MainWindow::EditRedo()
{
    GetTransactionMgr().lock()->redo();
}


void MainWindow::EditAdd()
{
	auto charMgr(GetCharacterMgr().lock());
	assert(charMgr);

#ifdef DEMO_VERSION
	const int count = (int) charMgr->GetCharacters().size();
	if( count >= NAG_MAX_CHARACTERS	)
	{
		ShowNagWindow( NagWindow::NAG_MAXCHARS );
		return;
	}
#endif
	Actions::Desensitizer	ad;
	
    Combatant::Character::pointer_t	ch( new Combatant::Character() );
	assert(ch);
	ch->name( gettext("New Character") );
	ch->hitDice(1);
	ch->baseHP(10);

	Combatant::Character::list_t selected = f_charView.getSelected();
	const bool char_selected = selected.size() == 1;
	if( char_selected )
	{
		Combatant::Character::pointer_t sel_ch = selected[0];
		ch->Duplicate( sel_ch );
	}

	bool duplicate = false;
	EditWindow editWindow( gettext("Add Character"), *this );
	editWindow.set_transient_for(*this);
	do
	{
		duplicate = false;
		editWindow.SetData( ch );
		editWindow.SetFocusEntry( f_lastEditFocus );
		const int response_id = editWindow.run();
		f_lastEditFocus = editWindow.GetFocusEntry();
		editWindow.hide();
		editWindow.GetData( ch );
		
		if( response_id == Gtk::RESPONSE_OK )
		{
#if defined(DEMO_VERSION)
			if( f_warnNoSave )
			{
			      ShowNagWindow( NagWindow::NAG_NOSAVE );
			      f_warnNoSave = false;
			}
#endif

			if( charMgr->FindDuplicates( ch ) )
			{
				QString message( gettext("Character \"") );
				message += ch->name();
				message += gettext("\" already exists! Please choose a different name.");
				Gtk::MessageDialog dlg( 
					message,
					false,
					Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_OK,
					true
				);
				dlg.set_transient_for(*this);
				dlg.run();
				duplicate = true;
			}
			else
			{
				AddTransactCharacter( ch );
			}
			f_lastCharNum++;
		}
	}
	while( duplicate );
}


void MainWindow::EditEdit()
{
	EditCharacter( 0 );
}


void MainWindow::EditDelete()
{
	TransactionGroupHelper helper( gettext("Delete Character(s)") );
	Transaction::pointer_t tr( new RemoveSelectedTransaction( f_charView.getSelected() ) );
	helper.AddTransaction( tr );
}


void MainWindow::EditDuplicate()
{
	auto charMgr(GetCharacterMgr().lock());
	assert(charMgr);

#ifdef DEMO_VERSION
	const int count = (int) charMgr->GetCharacters().size();
	if( count >= NAG_MAX_CHARACTERS	)
	{
		ShowNagWindow( NagWindow::NAG_MAXCHARS );
		return;
	}
#endif
	Actions::Desensitizer	ad;

	DuplicateWindow	duplicateWindow( gettext("Enter Number of Duplicates"), *this );
	duplicateWindow.Value( f_lastDupInput );
	const int response_id = duplicateWindow.run();
	duplicateWindow.hide();
	f_lastDupInput = duplicateWindow.Value();

	if( response_id == Gtk::RESPONSE_OK )
	{
		Combatant::Character::pointer_t sel_ch = f_charView.getFirstSelected();
		int char_num = 1;
		QString base_name( sel_ch->name() );
		const int space = base_name.find_last_of(' ');
		if( space != QString::npos )
		{
			const QString last_word( base_name.substr( space ) );
			const int num = atoi( last_word.c_str() );
			if( num != 0 )
			{
				base_name = base_name.substr( 0, space );
				char_num = num;
			}
		}

		// get the count and do a little clamping (just in case!)
		int dup_count = duplicateWindow.Value();
		// this should be clamped by Gtk, but in case it
		// somehow is bogus on that day...
		if(dup_count < 0)
		{
			dup_count = 1;
		}
		else if(dup_count > 10)
		{
			dup_count = 10;
		}
#ifdef DEMO_VERSION
		const int count = (int) charMgr->GetCharacters().size();
		if( count + dup_count > NAG_MAX_CHARACTERS )
		{
			ShowNagWindow( NagWindow::NAG_MAXCHARS );
			return;
		}
#endif

		TransactionGroup::pointer_t group( new TransactionGroup( gettext("Add Duplicate Character(s)") ) );
		for( int idx = 0; idx < dup_count; ++idx )
		{
			Combatant::Character::pointer_t	ch( new Combatant::Character() );
			ch->Duplicate( sel_ch );
			do
			{
				std::stringstream ss;
				ss << base_name << " " << ++char_num;
				ch->name( ss.str() );
			}
			while( charMgr->FindDuplicates( ch ) );

			// Add the character
			//
			AddTransactCharacter( ch, group );
		}

		GetTransactionMgr().lock()->addTransaction( group, false /*doit*/ );
	}
}


void MainWindow::EditPurgeDead()
{
	TransactionGroupHelper helper( gettext("Purge Dead Characters") );
	//
	for( auto ch : GetCharacterMgr().lock()->GetCharacters() )
	{
		assert(ch);
		if( ch->health() == Combatant::Dead )
		{
			helper.AddTransaction( Transaction::pointer_t( new RemoveCharacterTransaction( ch ) ) );
		}
	}
}


void MainWindow::EditAddEffect()
{
#ifdef WANT_EFFECTS
	Actions::Desensitizer	ad;
	f_effectsBook.AddEffect();
#endif
}


void MainWindow::EditEditEffect()
{
#ifdef WANT_EFFECTS
	Actions::Desensitizer	ad;

	f_effectsBook.EditEffect();
#endif
}


void MainWindow::EditDeleteEffect()
{
#ifdef WANT_EFFECTS
	Actions::Desensitizer	ad;

	f_effectsBook.DeleteEffect();
#endif
}


void MainWindow::ViewShowToolbar()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

    appSettings->ShowToolbar( f_actionViewShowToolbar->get_active() );
    UpdateUI( true );
}


void MainWindow::ViewToolbarOnBottom()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

    appSettings->ToolBarPos( f_actionViewToolbarOnBottom->get_active()? AppSettings::TBP_BOTTOM: AppSettings::TBP_TOP );
    UpdateUI( true );
}


#ifdef WANT_EFFECTS
void MainWindow::ViewShowEffects()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	appSettings->ShowEffects( f_actionViewShowEffects->get_active() );
	UpdateUI( true );
}
#endif

#ifdef WANT_NOTES
void MainWindow::ViewShowInfo()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	appSettings->ShowInfo( f_actionViewShowInfo->get_active() );
	UpdateUI( false );
}
#endif


void MainWindow::ViewShowHUD()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	appSettings->ShowHUD( f_actionViewShowHUD->get_active() );
	UpdateUI( false );
}


void MainWindow::RoundsStart()
{
	f_roundsHelper->Start();
}


void MainWindow::RoundsEnd()
{
	f_roundsHelper->End();
}


void MainWindow::RoundsNext()
{
	f_roundsHelper->Next();
}


void MainWindow::RoundsDelay()
{
	f_roundsHelper->Delay();
}


void MainWindow::RoundsReady()
{
	f_roundsHelper->Ready();
}


void MainWindow::RoundsJumpIn()
{
	f_roundsHelper->JumpIn();
}


void MainWindow::RoundsDamage()
{
	f_roundsHelper->Damage();
}


void MainWindow::RoundsStabilize()
{ 
	f_roundsHelper->Stabilize( f_actionRoundsStabilize->get_active() );
}


void MainWindow::RoundsMoveUp()
{
	f_roundsHelper->MoveCharacters( true /* up */ );
}


void MainWindow::RoundsMoveDown()
{
	f_roundsHelper->MoveCharacters( false /* up */ );
}


void MainWindow::RollInitiative()
{
	f_roundsHelper->RollInitiative();
}


void MainWindow::RollSoftColumns( Attribute::Stat::pointer_t stat )
{
	f_roundsHelper->RollOther( stat->name().c_str(), stat->id() );
}


void MainWindow::HelpAbout()
{
	Actions::Desensitizer	ad;
	AboutWindow	dlg;
	dlg.set_transient_for(*this);
	dlg.run();
}

void MainWindow::HelpDocumentation()
{
	ShowDocumentation("index.html");
}

void MainWindow::HelpTutorial()
{
	ShowDocumentation("description.html");
}

void MainWindow::HelpContact()
{
	ShowDocumentation("contact.html");
}

void MainWindow::HelpLicense()
{
	ShowDocumentation("license.html");
}

void MainWindow::HelpHomePage()
{
	ShowDocumentation( WEBSITE_URL );
}


motk::ActionPtr MainWindow::GetAction( Attribute::Stat::pointer_t stat )
{
	const QString	event( "Roll::" + stat->name() );
	//
	return GetActionsMgr().lock()->GetAction( event.c_str() );
}


void MainWindow::Load()
{
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);
	auto statMgr( GetStatMgr().lock() );
	assert(statMgr);

	// Alert the entire app that the StatManager has new values
	//
	statMgr->signal_changed().emit();

	// If we were in rounds before while running the demo, we need to take ourselves
	// out of rounds, otherwise we'll have a crash with no characters loaded.
	//
    const Combatant::Character::list_t& list( initMgr->GetCharacterList() );
	if( list.size() == 0 && initMgr->InRounds() )
	{
        Transaction::pointer_t tr( new StopInitTransaction() );
		tr->doit();
		
		// One more alert to load stuff
		//
		statMgr->signal_changed().emit();
	}
}


void MainWindow::Save()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	f_isSaving = true;

	// Store window position
	//
	int x, y;
	Glib::RefPtr<Gdk::Window> window = get_window();
#ifdef MAC
	if(window->is_visible()) {
#endif
		window->get_root_origin( x, y );
		//
		appSettings->X( x );
		appSettings->Y( y );
#ifdef MAC
	}
#endif

	// Store window dimensions
	//
	int width, height;
	get_size( width, height );
	//
	appSettings->Width( width );
	appSettings->Height( height );

#ifdef WANT_PANE
	appSettings->PanePosition( f_pane.get_position() );
#endif
	//
	appSettings->Save();
	//
	GetCharacterMgr().lock()->Save();
	GetInitMgr().lock()->Save();
	GetStatMgr().lock()->Save();
	//
	f_isSaving = false;
}


/// \brief Create the effects frame widget
//
// Presently it contains the effects toolbar and the effects book. The effects book is going to be deprecated.
//
/// \sa ActionsManager, EffectsBook
//
void MainWindow::CreateEffectsFrame()
{
	f_effectsBox.children().clear();
	f_effectsBox.pack_start( *f_effectsToolBar, Gtk::PACK_SHRINK );
	f_effectsBox.pack_start( f_effectsBook, Gtk::PACK_EXPAND_WIDGET );
	f_effectsFrame.add( f_effectsBox );
	f_effectsFrame.show_all();
}


void MainWindow::FillMainBox()
{
	InstallMenu();
	//
#ifdef WANT_EFFECTS
	f_effectsBook.property_homogeneous() = false;
	f_effectsBook.property_scrollable() = true;
#endif
	//
#ifdef WANT_PANE
	Gtk::ScrolledWindow* scroller = 0;
	f_pane.pack1( f_scrolledWindow, Gtk::EXPAND );
	scroller = Gtk::manage( new Gtk::ScrolledWindow );
	scroller->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	scroller->add( f_bottomBox );
	f_pane.pack2( *scroller, Gtk::SHRINK );

	f_mainBox.pack_start( f_pane, Gtk::PACK_EXPAND_WIDGET );
#else
	f_bottomBox.pack_start( f_scrolledWindow, Gtk::PACK_EXPAND_WIDGET );
	f_bottomBox.reorder_child( f_scrolledWindow, 0 );
	f_mainBox.pack_start( f_bottomBox, Gtk::PACK_EXPAND_WIDGET );
#endif	// !WANT_PANE

	f_mainBox.pack_start( f_statusBox, Gtk::PACK_SHRINK );

	show_all_children();
#ifdef DEBUG
	std::cout << "MainWindow has its main box filled.\n";
#endif
}


void MainWindow::InstallMenu()
{
	// Get the menu bar
	//
	f_mainMenuBar = GetActionsMgr().lock()->GetMenubar();

#ifdef DARWIN
	// Add menus up in Mac bar if we are on MAC OS/X
	//
	sync_menu_takeover_menu (GTK_MENU_SHELL (f_mainMenuBar->gobj()));
#else
	// All other platforms the menu goes at the top
	//
	f_mainBox.pack_start   ( *f_mainMenuBar, Gtk::PACK_SHRINK );
	f_mainBox.reorder_child( *f_mainMenuBar, 0 );
#endif
}


void MainWindow::EditCharacter( Combatant::Character::pointer_t _char )
{
	Actions::Desensitizer	ad;

	bool duplicate;
	EditWindow editWindow( gettext("Edit Character"), *this );
	editWindow.set_transient_for(*this);
	do
	{
		duplicate = false;
		Combatant::Character::pointer_t ch = (_char)? _char: f_charView.getFirstSelected();
		editWindow.SetData( ch );
		editWindow.SetFocusEntry( f_lastEditFocus );
		const int response_id = editWindow.run();
		f_lastEditFocus = editWindow.GetFocusEntry();
		editWindow.hide();

		if( response_id == Gtk::RESPONSE_OK )
		{
			Combatant::Character::pointer_t newCh( new Combatant::Character );
			newCh->Copy( ch );

			editWindow.GetData( newCh );

			Combatant::Character::pointer_t dupCh( GetCharacterMgr().lock()->FindDuplicates( newCh ) );
			if( dupCh && (dupCh != (Combatant::Character::pointer_t) ch) )
			{
				QString message( gettext("Character \"") );
				message += newCh->name();
				message += gettext("\" already exists! Please choose a different name.");
				Gtk::MessageDialog dlg(
					message,
					false,
					Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_OK,
					true
				);
				dlg.run();
				duplicate = true;
			}
			else
			{
				TransactionGroupHelper helper( gettext("Edit Character") );
				Transaction::pointer_t tr( new EditCharacterTransaction( ch, ch, newCh ) );
				helper.AddTransaction( tr );
			}
		}
	}
	while( duplicate );
}


void MainWindow::InitActions()
{
	auto actionsMgr(GetActionsMgr().lock());
	assert(actionsMgr);

	actionsMgr->CreateActions();
	
	// File menu
	//
	f_actionFileImport      = actionsMgr->GetAction( "File::Import"      );
	f_actionFileExport      = actionsMgr->GetAction( "File::Export"      );
	f_actionFileClear       = actionsMgr->GetAction( "File::Clear"       );
	f_actionFilePreferences = actionsMgr->GetAction( "File::Preferences" );
	f_actionFileStatManager = actionsMgr->GetAction( "File::StatManager" );
	f_actionFileQuit        = actionsMgr->GetAction( "File::Quit"        );

	// Edit menu
	//
	f_actionEditUndo			= actionsMgr->GetAction( "Edit::Undo"		);
	f_actionEditRedo			= actionsMgr->GetAction( "Edit::Redo"		);
	f_actionEditAdd				= actionsMgr->GetAction( "Edit::Add"		);
	f_actionEditEdit			= actionsMgr->GetAction( "Edit::Edit"		);
	f_actionEditDelete			= actionsMgr->GetAction( "Edit::Delete"	);
	f_actionEditDuplicate		= actionsMgr->GetAction( "Edit::Duplicate"	);
	f_actionEditPurgeDead		= actionsMgr->GetAction( "Edit::PurgeDead"	);
#ifdef WANT_EFFECTS
	f_actionEditAddEffect		= actionsMgr->GetAction( "Edit::AddEffect"	);
	f_actionEditEditEffect		= actionsMgr->GetAction( "Edit::EditEffect"	);
	f_actionEditDeleteEffect	= actionsMgr->GetAction( "Edit::DeleteEffect"	);
#endif

	// View Menu
	//
	f_actionViewShowToolbar		= motk::ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "View::ShowToolbar"	) );
	f_actionViewToolbarOnBottom	= motk::ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "View::ToolbarOnBottom"	) );
#ifdef WANT_EFFECTS
	f_actionViewShowEffects		= motk::ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "View::ShowEffects"	) );
#endif
#ifdef WANT_NOTES
	f_actionViewShowInfo		= motk::ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "View::ShowInfo"	) );
#endif
	f_actionViewShowHUD			= motk::ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "View::ShowHUD"	) );

	// Rounds Menu
	//
	f_actionRoundsStart 	= actionsMgr->GetAction( "Rounds::Start" 	);
	f_actionRoundsEnd   	= actionsMgr->GetAction( "Rounds::End"   	);
	f_actionRoundsNext  	= actionsMgr->GetAction( "Rounds::Next"  	);
	f_actionRoundsDelay 	= actionsMgr->GetAction( "Rounds::Delay" 	);
	f_actionRoundsReady 	= actionsMgr->GetAction( "Rounds::Ready" 	);
	f_actionRoundsJumpIn	= actionsMgr->GetAction( "Rounds::JumpIn"	);
	f_actionRoundsDamage	= actionsMgr->GetAction( "Rounds::Damage"	);
	f_actionRoundsStabilize	= ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "Rounds::Stabilize"	) );
	f_actionRoundsMoveUp	= actionsMgr->GetAction( "Rounds::MoveUp"	);
	f_actionRoundsMoveDown	= actionsMgr->GetAction( "Rounds::MoveDown"	);

	// Roll menu
	//
	f_actionRollInitiative	= actionsMgr->GetAction( "Roll::Initiative"		);

	// Help menu
	//
	f_actionHelpAbout			= actionsMgr->GetAction( "Help::About"	);
	f_actionHelpDocumentation	= actionsMgr->GetAction( "Help::Documentation");
	f_actionHelpTutorial     	= actionsMgr->GetAction( "Help::Tutorial"	);
	f_actionHelpContact      	= actionsMgr->GetAction( "Help::Contact"	);
	f_actionHelpLicense      	= actionsMgr->GetAction( "Help::License"	);
	f_actionHelpHomePage     	= actionsMgr->GetAction( "Help::HomePage"	);

	//
	// Add to menu
	//
	add_accel_group( actionsMgr->GetAccelGroup() );

	// Turn these off at first.
	//
	f_actionEditUndo->set_sensitive( false );
	f_actionEditRedo->set_sensitive( false );
}


void MainWindow::ConnectEvents()
{
	f_connections.clear();

	f_connections.push_back( f_actionFileImport          ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::FileImport          ) )  );
	f_connections.push_back( f_actionFileExport          ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::FileExport          ) )  );
	f_connections.push_back( f_actionFileClear           ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::FileClear           ) )  );
	f_connections.push_back( f_actionFilePreferences     ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::FilePreferences     ) )  );
	f_connections.push_back( f_actionFileStatManager     ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::FileStatManager     ) )  );
	f_connections.push_back( f_actionFileQuit            ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::FileQuit            ) )  );
	f_connections.push_back( f_actionEditUndo            ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditUndo            ) )  );
	f_connections.push_back( f_actionEditRedo            ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditRedo            ) )  );
	f_connections.push_back( f_actionEditAdd             ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditAdd             ) )  );
	f_connections.push_back( f_actionEditEdit            ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditEdit            ) )  );
	f_connections.push_back( f_actionEditDelete          ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditDelete          ) )  );
	f_connections.push_back( f_actionEditDuplicate       ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditDuplicate       ) )  );
	f_connections.push_back( f_actionEditPurgeDead       ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditPurgeDead       ) )  );
#ifdef WANT_EFFECTS
	f_connections.push_back( f_actionEditAddEffect       ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditAddEffect       ) )  );
	f_connections.push_back( f_actionEditEditEffect      ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditEditEffect      ) )  );
	f_connections.push_back( f_actionEditDeleteEffect    ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::EditDeleteEffect    ) )  );
#endif
	f_connections.push_back( f_actionViewShowToolbar     ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::ViewShowToolbar     ) )  );
	f_connections.push_back( f_actionViewToolbarOnBottom ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::ViewToolbarOnBottom ) )  );
#ifdef WANT_EFFECTS
	f_connections.push_back( f_actionViewShowEffects     ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::ViewShowEffects     ) )  );
#endif
#ifdef WANT_NOTES
	f_connections.push_back( f_actionViewShowInfo        ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::ViewShowInfo        ) )  );
#endif
	f_connections.push_back( f_actionViewShowHUD         ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::ViewShowHUD         ) )  );
	f_connections.push_back( f_actionRoundsStart         ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsStart         ) )  );
	f_connections.push_back( f_actionRoundsEnd           ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsEnd           ) )  );
	f_connections.push_back( f_actionRoundsNext          ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsNext          ) )  );
	f_connections.push_back( f_actionRoundsDelay         ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsDelay         ) )  );
	f_connections.push_back( f_actionRoundsReady         ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsReady         ) )  );
	f_connections.push_back( f_actionRoundsJumpIn        ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsJumpIn        ) )  );
	f_connections.push_back( f_actionRoundsDamage        ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsDamage        ) )  );
	f_connections.push_back( f_actionRoundsStabilize     ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsStabilize     ) )  );
	f_connections.push_back( f_actionRoundsMoveUp        ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsMoveUp        ) )  );
	f_connections.push_back( f_actionRoundsMoveDown      ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RoundsMoveDown      ) )  );
	f_connections.push_back( f_actionRollInitiative      ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::RollInitiative      ) )  );
	//
	// Hook up new stats to UI toolbar
	//
    for( auto pair : GetStatMgr().lock()->GetStats() )
	{
        auto stat( pair.second );
		assert(stat);
		if( !stat->internal() && !stat->deleted() && stat->showOnToolbar() )
		{
			motk::ActionPtr		action = GetAction( stat );
			if( action )
			{
				f_connections.push_back( action->signal_activate().connect( sigc::bind( sigc::mem_fun( 
				*this, &MainWindow::RollSoftColumns ), stat ) ) );
			}
		}
	}
	//
	f_connections.push_back( f_actionHelpAbout         ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::HelpAbout         ) ) );
	f_connections.push_back( f_actionHelpDocumentation ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::HelpDocumentation ) ) );
	f_connections.push_back( f_actionHelpTutorial      ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::HelpTutorial      ) ) );
	f_connections.push_back( f_actionHelpContact       ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::HelpContact       ) ) );
	f_connections.push_back( f_actionHelpLicense       ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::HelpLicense       ) ) );
	f_connections.push_back( f_actionHelpHomePage      ->signal_activate().connect( sigc::mem_fun( *this, &MainWindow::HelpHomePage      ) ) );
}


void MainWindow::DisconnectEvents()
{
	for( auto conn : f_connections )
	{
		conn.disconnect();
	}

	f_connections.clear();
}


void MainWindow::UpdateMenus()
{
	auto transMgr(GetTransactionMgr().lock());
	assert(transMgr);

	if( f_actionEditUndo )
	{
		if( transMgr->isUndoListEmpty() )
		{
			f_actionEditUndo->property_label() = gettext("_Undo");
			f_actionEditUndo->set_sensitive( false );
		}
		else
		{
			f_actionEditUndo->property_label() = gettext("_Undo") + (" " + transMgr->getUndoListTop());
			f_actionEditUndo->set_sensitive( true );
		}
	}

	if( f_actionEditRedo )
	{
		if( transMgr->isRedoListEmpty() )
		{
			f_actionEditRedo->property_label() = gettext("_Redo");
			f_actionEditRedo->set_sensitive( false );
		}
		else
		{
			f_actionEditRedo->property_label() = gettext("_Redo") + (" " + transMgr->getRedoListTop());
			f_actionEditRedo->set_sensitive( true );
		}
	}
}

											  
void MainWindow::SetStatActionsSensitivity( const bool sensitive )
{											  
	for( auto pair : GetStatMgr().lock()->GetStats() )
	{
		Attribute::Stat::pointer_t stat( pair.second );
		assert(stat);
		if( !stat->internal() && !stat->deleted() && stat->showOnToolbar() )
		{
			motk::ActionPtr	action( GetAction( stat ) );
			//
			if( action )
			{
				action->set_sensitive( sensitive );
			}
		}
	}
}


void MainWindow::UpdateUI( const bool changeToolbar /*, const bool updateInit*/ )
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	if( changeToolbar )
	{
		// Regenerate the XML
		//
		GetActionsMgr().lock()->AlterUI();
		//
		f_mainBox.remove( *f_mainToolBar );
		//
		if( appSettings->ShowToolbar() )
		{
			// Put the toolbar back if switched on
			//
			f_mainBox.pack_start   ( *f_mainToolBar, Gtk::PACK_SHRINK );
			f_mainBox.reorder_child( *f_mainToolBar, appSettings->ToolBarPos() );
		}
	}

	Combatant::Character::list_t selected = f_charView.getSelected();
	const bool char_selected = selected.size() == 1;
	const bool chars_selected = selected.size() > 0;
	//
	f_actionEditAdd      ->set_sensitive( true           );
	f_actionEditEdit     ->set_sensitive( char_selected  );
	f_actionEditDelete   ->set_sensitive( chars_selected );
	f_actionEditDuplicate->set_sensitive( char_selected  );

#ifdef WANT_EFFECTS
	if( char_selected )
	{
		Combatant::Character::pointer_t ch( selected[0] );
		Effect::list_t effects;
		ch->getEffects( effects );
		//
		f_actionEditAddEffect   ->set_sensitive( true		    );
		f_actionEditEditEffect  ->set_sensitive( effects.size() > 0 );
		f_actionEditDeleteEffect->set_sensitive( effects.size() > 0 );
	}
	else if( chars_selected )
	{
		f_actionEditAddEffect   ->set_sensitive( true  );
		f_actionEditEditEffect  ->set_sensitive( false );
		f_actionEditDeleteEffect->set_sensitive( true  );
	}
	else
	{
		f_actionEditAddEffect   ->set_sensitive( false );
		f_actionEditEditEffect  ->set_sensitive( false );
		f_actionEditDeleteEffect->set_sensitive( false );
	}

	if( appSettings->ShowEffects() )
	{
		Gtk::Box::BoxList& list = f_bottomBox.children();
		if( list.find( f_effectsFrame ) == list.end() )
		{
			f_bottomBox.pack_start( f_effectsFrame, false, false );
			f_effectsFrame.show_all();
		}
		//
		f_effectsBook.OnSelectionChanged( f_charView.getSelected() );
	}
	else
	{
		f_effectsFrame.hide_all();
		//
		Gtk::Box::BoxList& list = f_bottomBox.children();
		if( list.find( f_effectsFrame ) != list.end() )
		{
			list.remove( f_effectsFrame );
		}
	}
#endif

#ifdef WANT_NOTES
	if( appSettings->ShowInfo() )
	{
		f_infoWindow.show_all();
		f_infoWindow.OnSelectionChanged( f_charView.getSelected() );
	}
	else
	{
		f_infoWindow.hide_all();
	}
#endif

	if( appSettings->ShowHUD() )
	{
		f_hudView.show_all();
	}
	else
	{
		f_hudView.hide_all();
	}

	const Combatant::Character::list_t& charList( GetCharacterMgr().lock()->GetCharacters() );
	const bool empty = charList.empty();
	f_actionRollInitiative->set_sensitive( !empty );
	//
	SetStatActionsSensitivity( !empty );

	// Allow purging of dead charList only if there are some
	//
	bool jumpins = false;
	int dead_char_count = 0;
	for( auto ch : charList )
	{
		assert(ch);
		//
		if( ch->health() == Combatant::Dead )
		{
			dead_char_count++;
		}
		else
		{
			switch( ch->status() )
			{
				case Combatant::Delayed:
				case Combatant::Readied:
					jumpins = true;
					break;
				default:
					break;
			}
		}
	}

	f_actionEditPurgeDead->set_sensitive( dead_char_count > 0 );

	f_actionRoundsDamage->set_sensitive( chars_selected );

	if( GetInitMgr().lock()->InRounds() )
	{
		f_actionFileImport->set_sensitive( true );
		f_actionFileExport->set_sensitive( true );
		f_actionFileClear->set_sensitive( false );
		f_actionRoundsStart->set_sensitive( false );
		//f_actionRoundsStart->set_visible( false );
		f_actionRoundsEnd->set_sensitive( true );
		//f_actionRoundsEnd->set_visible( true );
		f_actionRoundsNext->set_sensitive( true );
		f_actionRoundsDelay->set_sensitive( true );
		f_actionRoundsReady->set_sensitive( true );
		f_actionRoundsJumpIn->set_sensitive( jumpins );
		f_actionRoundsMoveUp->set_sensitive( chars_selected );
		f_actionRoundsMoveDown->set_sensitive( chars_selected );

		// Enable/disable, check (or un-check) the "Stabilize" menu entry
		//
		int  dying_chars      = 0;
		int  stabilize_checks = 0;
		const int count = selected.size();
		//
		for( int idx = 0; idx < count; ++idx )
		{
			Combatant::Character::pointer_t ch = selected[idx];
			switch( ch->health() )
			{
				case Combatant::Stabilized:
					++stabilize_checks;
					++dying_chars;
					break;

				case Combatant::Dying:
					++dying_chars;
					break;

				default:
					break;
			}
		}
		//
		if( dying_chars == count )
		{
			EventDisabler ed( this );
			f_actionRoundsStabilize->set_active( stabilize_checks == count );
			f_actionRoundsStabilize->set_sensitive( true );
		}
		else
		{
			f_actionRoundsStabilize->set_sensitive( false );
		}
	}
	else
	{
		f_actionFileImport->set_sensitive( true );
		f_actionFileExport->set_sensitive( true );
		f_actionFileClear->set_sensitive( true );
		f_actionRoundsStart->set_sensitive( !charList.empty() );
		//f_actionRoundsStart->set_visible( true );
		f_actionRoundsEnd->set_sensitive( false );
		//f_actionRoundsEnd->set_visible( false );
		f_actionRoundsNext->set_sensitive( false );
		f_actionRoundsDelay->set_sensitive( false );
		f_actionRoundsReady->set_sensitive( false );
		f_actionRoundsJumpIn->set_sensitive( false );
		f_actionRoundsStabilize->set_sensitive( false );
		f_actionRoundsMoveUp->set_sensitive( false );
		f_actionRoundsMoveDown->set_sensitive( false );
	}

	// Allow the toolbar_on_bottom menu if we are showing the toolbar itself
	//
	f_actionViewToolbarOnBottom->set_sensitive( appSettings->ShowToolbar() );

	show_all_children();
	f_charView.grab_focus();
}


void MainWindow::AddTransactCharacter( Combatant::Character::pointer_t ch, TransactionGroup::pointer_t group )
{
	bool end_transaction;
	//
	if((end_transaction = !group))
	{
		group.reset( new TransactionGroup( gettext("Add Character") ) );
	}
	//
	Transaction::pointer_t	tr( new AddCharacterTransaction( ch ) );
	tr->doit();
	group->addTransaction( tr );

	if( end_transaction )
	{
		GetTransactionMgr().lock()->addTransaction( group, false /*doit*/ );
	}
}


void MainWindow::ShowDocumentation(QString const & index)
{
	Common::ShowDocumentation( index );
}


void MainWindow::OnCharListChanged()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

#ifdef DEBUG
	std::cout  << "OnCharListChanged() " << std::endl;
#endif
	UpdateUI( false );
	appSettings->Modified( true );
}


void MainWindow::OnDoubleClick()
{
#ifdef DEBUG
	std::cout  << "OnDoubleClick() " << std::endl;
#endif
	f_actionEditEdit->activate();
}


void MainWindow::OnSelectionChanged( Combatant::Character::list_t selection_list )
{
	UpdateUI( false );
}


void MainWindow::OnSettingsChanged()
{
	UpdateUI( false );
}


void MainWindow::OnEditCharacter( Combatant::Character::pointer_t _char )
{
	if( _char )
	{
		EditCharacter( _char );
	}
}

#if defined(DEMO_VERSION)
void MainWindow::ConnectNagEvent( const bool connect_it )
{
	if( connect_it )
	{
		f_nagEvent = Glib::signal_timeout().connect(
				sigc::mem_fun(*this, &MainWindow::OnNagEvent),
				NAG_TIMEOUT_SECS * 1000);
	}
	else
	{
		f_nagEvent.disconnect();
	}
}


bool MainWindow::OnNagEvent()
{
	const time_t mins_since_start = (time(0) - f_startTime) / 60;
	
	if( mins_since_start % NAG_INTERVAL_MINS == 0 )
	{
#ifdef DEBUG
		std::cout << "NAG MINS: mins_since_start: " << mins_since_start << std::endl;
#endif
		ShowNagWindow( NagWindow::NAG_TIMEOUT );
	}

	if( mins_since_start >= NAG_QUIT_MINS )
	{
#ifdef DEBUG
		std::cout << "NAG_QUIT_MINS: mins_since_start: " << mins_since_start << std::endl;
#endif
		hide();
	}

	return true;
}
#endif

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
void MainWindow::ShowNagWindow( const NagWindow::NagType type )
{
	if( f_nagWindow.is_visible() )
	{
		return;
	}

	Actions::Desensitizer	ad;

	f_nagWindow.Update( type );

	f_nagWindow.show();
	f_nagWindow.present();
	const int response = f_nagWindow.run();
	f_nagWindow.hide();

	if( response == NAG_WINDOW_GOTO_SITE )
	{
		ShowDocumentation( WEBSITE_URL );
	}
}
#endif


bool MainWindow::on_event( GdkEvent* event )
{
	if( f_isSaving )
		return true;
	else
		return Gtk::Window::on_event( event );
}


bool MainWindow::on_key_press_event( GdkEventKey* event )
{
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	// Emit the OK response on ENTER
	//
	switch( event->keyval )
	{
		case GDK_Return:
			{
				f_actionEditEdit->activate();
				return true;
			}
			break;

		case GDK_Up:
			if( event->state & GDK_CONTROL_MASK && initMgr->InRounds() )
			{
				f_actionRoundsMoveUp->activate();
				return true;
			}
			else
			{
			    // Otherwise, make sure charList gets focus...
			    //
			    f_charView.grab_focus();
			}
			break;

		case GDK_Down:
			if( event->state & GDK_CONTROL_MASK && initMgr->InRounds() )
			{
				f_actionRoundsMoveDown->activate();
				return true;
			}
			else
			{
			    // Otherwise, make sure charList gets focus...
			    //
			    f_charView.grab_focus();
			}
			break;

#ifdef WANT_EFFECTS
		case GDK_Tab:
			f_effectsBook.NextTab();
			return true;

		case GDK_ISO_Left_Tab:
			if( event->state & GDK_SHIFT_MASK )
			{
				f_effectsBook.PrevTab();
			}
			return true;
#endif
	}

	return Window::on_key_press_event(event);
}

void MainWindow::on_hide()
{
#ifdef DEBUG
	std::cout << "MainWindow::on_hide()" << std::endl;
#endif

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	ShowNagWindow( NagWindow::NAG_EXIT );
#endif

#if defined(DEMO_VERSION)
	ConnectNagEvent( false );
#endif
	Gtk::Window::on_hide();
	Gtk::Main::quit();
}


// Cause the application to shutdown after the main window is hidden
//
bool MainWindow::on_delete_event(GdkEventAny* event)
{
	const bool return_value = Gtk::Window::on_delete_event( event );
	hide();
	return return_value;
}


/// \brief Hide the splash screen after an interval
//
bool MainWindow::on_splash_auto_hide(void)
{
	grab_focus();
	f_splash.hide();
	return true;
}


}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

