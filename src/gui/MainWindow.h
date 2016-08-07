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

// CLIB
//
#include <time.h>

// STL
//
#include <map>

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// MOLIB
//
#include "mo/mo_name.h"
#include "mo/mo_string.h"

// LOCAL
//
#include "AppSettings.h"
#include "CharacterListUI.h"
#include "transaction.h"
#include "EditWindow.h"
#include "JumpInWindow.h"
#include "DamageWindow.h"
#include "DuplicateWindow.h"
#include "EffectsBook.h"
#include "HUDWindow.h"
#include "SaveWindow.h"
#include "SingletonManager.h"
#include "splash.h"
#include "StatusBox.h"


#if defined(DEMO_VERSION) || defined(BETA_VERSION)
#include "NagWindow.h"
#endif

namespace UI
{

class MainWindow :
	public Gtk::Window,
	private Application::ManagerBase
{

public:
	MainWindow( const molib::moWCString& confFile );
	~MainWindow();


private:
	UI::SplashScreen		f_splash;
	sigc::connection		f_splash_auto_hide;
	
	bool					f_isSaving;

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	NagWindow				f_nagWindow;
#endif
	molib::moWCString		f_confFile;

	Gtk::VBox				f_mainBox;
	CharacterListUI			f_charListUI;
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
	HUDWindow				f_hudWindow;
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

	typedef std::map<molib::moWCString, Attribute::Stat::Pointer>	StringToStatMap;
	StringToStatMap			f_eventMap;

	typedef std::vector<sigc::connection>	Connections;
	Connections				f_connections;

#if defined(DEMO_VERSION)
	sigc::connection		f_nagEvent;
	time_t					f_startTime;
	bool                    f_warnNoSave;
#endif


	// FILE MENU
	//
	motk::ActionPtr	f_actionFileImport;
	motk::ActionPtr	f_actionFileExport;
	motk::ActionPtr	f_actionFileClear;
	motk::ActionPtr	f_actionFilePreferences;
	motk::ActionPtr	f_actionFileStatManager;
	motk::ActionPtr	f_actionFileQuit;
	//
	void	FileImport	();
	void	FileExport	();
	void	FileClear	();
	void	FilePreferences	();
	void	FileStatManager	();
	void	FileQuit	();

	motk::ActionPtr	f_actionEditUndo	;
	motk::ActionPtr	f_actionEditRedo	;
	motk::ActionPtr	f_actionEditAdd		;
	motk::ActionPtr	f_actionEditEdit	;
	motk::ActionPtr	f_actionEditDelete	;
	motk::ActionPtr	f_actionEditDuplicate	;
	motk::ActionPtr	f_actionEditPurgeDead	;
	motk::ActionPtr	f_actionEditAddEffect	;
	motk::ActionPtr	f_actionEditEditEffect	;
	motk::ActionPtr	f_actionEditDeleteEffect;
	//
	void	EditUndo	();
	void	EditRedo	();
	void	EditAdd		();
	void	EditEdit	();
	void	EditDelete	();
	void	EditDuplicate	();
	void	EditPurgeDead	();
	void	EditAddEffect	();
	void	EditEditEffect	();
	void	EditDeleteEffect();

	// View Menu
	//
	motk::ToggleActionPtr	f_actionViewShowToolbar	;
	motk::ToggleActionPtr	f_actionViewToolbarOnBottom;
#ifdef WANT_EFFECTS
	motk::ToggleActionPtr	f_actionViewShowEffects	;
#endif
#ifdef WANT_NOTES
	motk::ToggleActionPtr	f_actionViewShowInfo	;
#endif
	motk::ToggleActionPtr	f_actionViewShowHUD	;
	//
	void	ViewShowToolbar();
	void	ViewToolbarOnBottom();
#ifdef WANT_EFFECTS
	void	ViewShowEffects();
#endif
#ifdef WANT_NOTES
	void	ViewShowInfo();
#endif
	void	ViewShowHUD();

	// Roll Menu
	//
	motk::ActionPtr			f_actionRollInitiative          ;
	//
	motk::ToggleActionPtr	f_actionRollGetDC               ;
	//
	void					RollDefineInitiativeDice();
	void					RollInitiative          ();
	void					RollInitOnStart         ();
	void					RollManualInit          ();
	void					RollGetDC               ();
	motk::ActionPtr			GetAction( Attribute::Stat::Pointer stat );
	void					RollSoftColumns( Attribute::Stat::Pointer stat );

	// Rounds Menu
	//
	motk::ActionPtr			f_actionRoundsStart 	;
	motk::ActionPtr			f_actionRoundsEnd   	;
	motk::ActionPtr			f_actionRoundsNext  	;
	motk::ActionPtr			f_actionRoundsDelay 	;
	motk::ActionPtr			f_actionRoundsReady 	;
	motk::ActionPtr			f_actionRoundsJumpIn	;
	motk::ActionPtr			f_actionRoundsDamage	;
	motk::ToggleActionPtr	f_actionRoundsStabilize	;
	motk::ActionPtr			f_actionRoundsMoveUp	;
	motk::ActionPtr			f_actionRoundsMoveDown	;
	//
	void					RoundsStart				();
	void					RoundsEnd				();
	void					RoundsNext				();
	void					RoundsDelay				();
	void					RoundsReady				();
	void					RoundsJumpIn			();
	void					RoundsDamage			();
	void					RoundsStabilize			();
	void					RoundsMoveUp			();
	void					RoundsMoveDown			();

	// Help Menu
	//
	motk::ActionPtr	f_actionHelpAbout		;
	motk::ActionPtr	f_actionHelpDocumentation	;
	motk::ActionPtr	f_actionHelpTutorial     	;
	motk::ActionPtr	f_actionHelpContact      	;
	motk::ActionPtr	f_actionHelpLicense      	;
	motk::ActionPtr	f_actionHelpHomePage     	;
	//
	void HelpAbout()	;
	void HelpDocumentation();
	void HelpTutorial()	;
	void HelpContact() 	;
	void HelpLicense() 	;
	void HelpHomePage()	;

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

	void		ConnectEvents();
	void		DisconnectEvents();
	
	void		AddTransactCharacter( Combatant::Character::Pointer ch, Transactions::TransactionGroup::Pointer TRANSACTION_GROUP = 0 );

	void		EditCharacter( Combatant::Character::Pointer _char );
	void		JumpIn();
	void		Damage();
	void		Stabilize();
	void		MoveCharacters( const bool up );

	void		DisplayPendingEffects();
	bool		CreateNextInIntiativeTransaction( Transactions::TransactionGroup::Pointer group );
	bool		DoRollInitiative( Transactions::TransactionGroup::Pointer group, bool allowCancel = true );
	void		RollOther( const std::string& tranact_desc, const molib::mo_name_t statId );

	bool		OnTimer();
	void		ShowDocumentation(const char *index);

	void		OnCharListChanged();
	void		OnDoubleClick();
	void		OnSelectionChanged( Combatant::Character::List selected_list );
	void		OnSettingsChanged();
	void		OnEditCharacter( Combatant::Character::Pointer _char );
	
#if defined(DEMO_VERSION)
	void		ConnectNagEvent( const bool connect_it = true );
	bool		OnNagEvent();
#endif
	
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	void		ShowNagWindow( const NagWindow::NagType type );
#endif

	// Gtk::Widget
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

