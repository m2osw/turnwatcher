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

// molib
//
#include "mo/mo_name.h"

// local
//
#include "base/ManagerBase.h"
#include "ui/CharacterView.h"
#include "ui/EffectsBook.h"
#include "ui/HUDWindow.h"
#include "ui/RoundsHelper.h"
#include "ui/Splash.h"
#include "ui/StatusBox.h"
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
#   include "ui/NagWindow.h"
#endif

// gtkmm
//
#include <gtkmm.h>
#include <glibmm.h>

namespace UI
{

class MainWindow :
	public Gtk::Window,
	private Application::ManagerBase
{

public:
	MainWindow();
	~MainWindow();


private:
	class EventDisabler
	{
	public:
	    EventDisabler( MainWindow* wnd) : f_mainWindow(wnd)
		{ f_mainWindow->DisconnectEvents(); }
	    ~EventDisabler()
		{ f_mainWindow->ConnectEvents(); }

	private:
	    MainWindow* f_mainWindow;
	};
	friend class EventDisabler;
	typedef std::map<Glib::ustring, Attribute::Stat::pointer_t>	stat_map_t;
	typedef std::vector<sigc::connection>	connection_list_t;

    SplashScreen		    f_splash;
	sigc::connection		f_splash_auto_hide;
	bool					f_isSaving;
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	NagWindow				f_nagWindow;
#endif
	Gtk::VBox				f_mainBox;
	CharacterView			f_charView;
	Gtk::ScrolledWindow		f_scrolledWindow;
	Gtk::VBox				f_bottomBox;
#ifdef WANT_PANE
	Gtk::VPaned				f_pane;
#endif
#ifdef WANT_EFFECTS
	Gtk::Frame				f_effectsFrame;
	Gtk::HBox				f_effectsBox;
	EffectsBook				f_effectsBook;
#endif
#ifdef WANT_NOTES
	InfoWindow				f_infoWindow;
#endif
	HUDWindow				f_hudView;
	//
	StatusBox				f_statusBox;
	//
	Gtk::Widget*			f_mainMenuBar;
	Gtk::Widget*			f_mainToolBar;
	Gtk::Widget*			f_effectsToolBar;
	int						f_lastDupInput;
	motk::MergeId			f_menubarId;
	motk::MergeId			f_toolbarId;
	int						f_lastCharNum;
	int						f_lastEditFocus;
	stat_map_t				f_eventMap;
	connection_list_t		f_connections;
#if defined(DEMO_VERSION)
	sigc::connection		f_nagEvent;
	time_t					f_startTime;
	bool                    f_warnNoSave;
#endif
	RoundsHelper::pointer_t	f_roundsHelper;
	//
	// Actions for the main menu
	//
	// File Menu
	//
	motk::ActionPtr       f_actionFileImport;
	motk::ActionPtr       f_actionFileExport;
	motk::ActionPtr       f_actionFileClear;
	motk::ActionPtr       f_actionFilePreferences;
	motk::ActionPtr       f_actionFileStatManager;
	motk::ActionPtr       f_actionFileQuit;
	//
	// Edit Menu
	//
	motk::ActionPtr       f_actionEditUndo;
	motk::ActionPtr       f_actionEditRedo;
	motk::ActionPtr       f_actionEditAdd;
	motk::ActionPtr       f_actionEditEdit;
	motk::ActionPtr       f_actionEditDelete;
	motk::ActionPtr       f_actionEditDuplicate;
	motk::ActionPtr       f_actionEditPurgeDead;
	motk::ActionPtr       f_actionEditAddEffect;
	motk::ActionPtr       f_actionEditEditEffect;
	motk::ActionPtr       f_actionEditDeleteEffect;
	//
	// View Menu
	//
	motk::ToggleActionPtr f_actionViewShowToolbar;
	motk::ToggleActionPtr f_actionViewToolbarOnBottom;
#ifdef WANT_EFFECTS
	motk::ToggleActionPtr f_actionViewShowEffects;
#endif
#ifdef WANT_NOTES
	motk::ToggleActionPtr f_actionViewShowInfo;
#endif
	motk::ToggleActionPtr f_actionViewShowHUD;
	//
	motk::ActionPtr       f_actionRollInitiative;
	motk::ToggleActionPtr f_actionRollGetDC;
	//
	// Rounds Menu
	//
	motk::ActionPtr       f_actionRoundsStart;
	motk::ActionPtr       f_actionRoundsEnd;
	motk::ActionPtr       f_actionRoundsNext;
	motk::ActionPtr       f_actionRoundsDelay;
	motk::ActionPtr       f_actionRoundsReady;
	motk::ActionPtr       f_actionRoundsJumpIn;
	motk::ActionPtr       f_actionRoundsDamage;
	motk::ToggleActionPtr f_actionRoundsStabilize;
	motk::ActionPtr       f_actionRoundsMoveUp;
	motk::ActionPtr       f_actionRoundsMoveDown;
	//
	// Help Menu
	//
	motk::ActionPtr       f_actionHelpAbout;
	motk::ActionPtr       f_actionHelpDocumentation;
	motk::ActionPtr       f_actionHelpTutorial;
	motk::ActionPtr       f_actionHelpContact;
	motk::ActionPtr       f_actionHelpLicense;
	motk::ActionPtr       f_actionHelpHomePage;

	//
	// Menu methods
	//
	// File Menu
	//
	void            FileImport();
	void            FileExport();
	void            FileClear();
	void            FilePreferences();
	void            FileStatManager();
	void            FileQuit();
	//
	// Edit Menu
	//
	void            EditUndo();
	void            EditRedo();
	void            EditAdd();
	void            EditEdit();
	void            EditDelete();
	void            EditDuplicate();
	void            EditPurgeDead();
	void            EditAddEffect();
	void            EditEditEffect();
	void            EditDeleteEffect();
	//
	// View Menu
	//
	void            ViewShowToolbar();
	void            ViewToolbarOnBottom();
#ifdef WANT_EFFECTS
	void            ViewShowEffects();
#endif
#ifdef WANT_NOTES
	void            ViewShowInfo();
#endif
	void            ViewShowHUD();
	//
	// Roll Menu
	//
	void            RollDefineInitiativeDice();
	void            RollInitiative();
	void            RollInitOnStart();
	void            RollManualInit();
	void            RollGetDC();
	motk::ActionPtr GetAction( Attribute::Stat::pointer_t stat );
	void            RollSoftColumns( Attribute::Stat::pointer_t stat );
	//
	// Rounds Menu
	//
	void            RoundsStart();
	void            RoundsEnd();
	void            RoundsNext();
	void            RoundsDelay();
	void            RoundsReady();
	void            RoundsJumpIn();
	void            RoundsDamage();
	void            RoundsStabilize();
	void            RoundsMoveUp();
	void            RoundsMoveDown();
	//
	// Help Menu
	//
	void            HelpAbout();
	void            HelpDocumentation();
	void            HelpTutorial();
	void            HelpContact();
	void            HelpLicense();
	void            HelpHomePage();

	// Support methods
	//
	void		Load();
	void		Save();
	void 		CreateEffectsFrame();
	void 		FillMainBox();
	void		InstallMenu();
	void		UpdateMenus();
	void 		UpdateUI( const bool changeToolbar /*,const bool updateInit*/ );
	
	// Gtk UI Manager support
	//
	void 		InitActions();
	void 		SetStatActionsSensitivity( const bool sensitive );
	void		ConnectEvents();
	void		DisconnectEvents();
	void		AddTransactCharacter( Combatant::Character::pointer_t ch, Transactions::TransactionGroup::pointer_t TRANSACTION_GROUP = 0 );
	void		EditCharacter( Combatant::Character::pointer_t _char );
	bool		OnTimer();
	void		ShowDocumentation(const char *index);
	void		OnCharListChanged();
	void		OnDoubleClick();
	void		OnSelectionChanged( Combatant::Character::list_t selected_list );
	void		OnSettingsChanged();
	void		OnEditCharacter( Combatant::Character::pointer_t _char );
#if defined(DEMO_VERSION)
	void		ConnectNagEvent( const bool connect_it = true );
	bool		OnNagEvent();
#endif
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	void		ShowNagWindow( const NagWindow::NagType type );
#endif
	//
	// Gtk::Widget virtual methods and slots
	//
	virtual void	on_hide();
	virtual bool	on_event( GdkEvent* event );
	virtual bool	on_key_press_event( GdkEventKey* event );
 	virtual bool	on_delete_event   ( GdkEventAny* event );
	virtual bool	on_splash_auto_hide(void);
};

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

