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

#include "ui_MainWindow.h"

// local
//
#include "base/ManagerBase.h"
#include "ui/CharacterView.h"
#include "ui/EffectsBook.h"
#include "ui/HUDWindow.h"
#include "ui/RoundsHelper.h"
#include "ui/Splash.h"
#include "ui/StatusBox.h"
#ifdef WANT_NOTES
#include "ui/InfoWindow.h"
#endif

// molib
//
#include "mo/mo_name.h"

// Qt4
//
#include <QtGui>

namespace UI
{

class MainWindow
	: public QMainWindow
	, public Ui::MainWindow
	, private Application::ManagerBase
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *);
    ~MainWindow();

private:
    SplashScreen		    f_splash;
	sigc::connection		f_splash_auto_hide;
	bool					f_isSaving;
	CharacterView			f_charView;
	HUDWindow				f_hudView;
	int						f_lastCharNum;
	int						f_lastEditFocus;
	RoundsHelper::pointer_t	f_roundsHelper;

	// Support methods
	//
	void		Load();
	void		Save();
	void 		CreateEffectsFrame();
	void		UpdateMenus();
	void 		UpdateUI( const bool changeToolbar /*,const bool updateInit*/ );

	// Manager Support
	//
	void		AddTransactCharacter( Combatant::Character::pointer_t ch, Transactions::TransactionGroup::pointer_t TRANSACTION_GROUP = 0 );
	void		EditCharacter( Combatant::Character::pointer_t _char );
	bool		OnTimer();
	void		ShowDocumentation(const char *index);
	void		OnCharListChanged();
	void		OnDoubleClick();
	void		OnSelectionChanged( Combatant::Character::list_t selected_list );
	void		OnSettingsChanged();
	void		OnEditCharacter( Combatant::Character::pointer_t _char );

private slots:
    void on_action_File_Import_triggered();
    void on_action_File_Export_triggered();
    void on_action_File_Clear_triggered();
    void on_action_File_Quit_triggered();
    void on_action_File_Preferences_triggered();
    void on_action_File_StatEditor_triggered();

    void on_action_Edit_Undo_triggered();
    void on_action_Edit_Redo_triggered();
    void on_action_Edit_Add_triggered();
    void on_action_Edit_Edit_triggered();
    void on_action_Edit_Delete_triggered();
    void on_action_Edit_Duplicate_triggered();
    void on_action_Edit_PurgeDeadCharacters_triggered();
    void on_action_Edit_AddEffect_triggered();
    void on_action_Edit_EditEffect_triggered();
    void on_action_Edit_DeleteEffect_triggered();

    void on_action_View_ShowToolbar_triggered();
    void on_action_View_ShowEffectPane_triggered();
    void on_action_View_ShowHUDWindow_triggered();

    void on_action_Roll_Initiative_triggered();
    void on_action_Rounds_Start_triggered();
    void on_action_Rounds_End_triggered();
    void on_action_Rounds_Next_triggered();
    void on_action_Rounds_Delay_triggered();
    void on_action_Rounds_Ready_triggered();
    void on_action_Rounds_Jump_triggered();
    void on_action_Rounds_Damage_triggered();
    void on_action_Rounds_Stabilize_triggered();
    void on_action_Rounds_MoveUp_triggered();
    void on_action_Rounds_MoveDown_triggered();

	void on_action_Help_Documentation_triggered();
	void on_action_Help_Tutorial_triggered();
	void on_action_Help_Contact_triggered();
	void on_action_Help_License_triggered();
	void on_action_Help_HomePage_triggered();
};


}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
