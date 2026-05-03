//===============================================================================
// Copyright (c) 2011-2017 by Made to Order Software Corporation
// 
// http://turnwatcher.com/
// contact@m2osw.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//===============================================================================
#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow
	: public QMainWindow
	, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *p = 0);
    ~MainWindow();

private:
#if 0
	//
	// Actions for the main menu
	//
	// File Menu
	//
	QAction       f_actionFileImport;
	QAction       f_actionFileExport;
	QAction       f_actionFileClear;
	QAction       f_actionFilePreferences;
	QAction       f_actionFileStatManager;
	QAction       f_actionFileQuit;
	//
	// Edit Menu
	//
	QAction       f_actionEditUndo;
	QAction       f_actionEditRedo;
	QAction       f_actionEditAdd;
	QAction       f_actionEditEdit;
	QAction       f_actionEditDelete;
	QAction       f_actionEditDuplicate;
	QAction       f_actionEditPurgeDead;
	QAction       f_actionEditAddEffect;
	QAction       f_actionEditEditEffect;
	QAction       f_actionEditDeleteEffect;
	//
	// View Menu
	//
	QAction       f_actionViewShowToolbar;
	QAction       f_actionViewToolbarOnBottom;
	QAction       f_actionViewShowEffects;
	QAction       f_actionViewShowInfo;
	QAction       f_actionViewShowHUD;
	//
	QAction       f_actionRollInitiative;
	QAction       f_actionRollGetDC;
	//
	// Rounds Menu
	//
	QAction       f_actionRoundsStart;
	QAction       f_actionRoundsEnd;
	QAction       f_actionRoundsNext;
	QAction       f_actionRoundsDelay;
	QAction       f_actionRoundsReady;
	QAction       f_actionRoundsJumpIn;
	QAction       f_actionRoundsDamage;
	QAction       f_actionRoundsStabilize;
	QAction       f_actionRoundsMoveUp;
	QAction       f_actionRoundsMoveDown;
	//
	// Help Menu
	//
	QAction       f_actionHelpAbout;
	QAction       f_actionHelpDocumentation;
	QAction       f_actionHelpTutorial;
	QAction       f_actionHelpContact;
	QAction       f_actionHelpLicense;
	QAction       f_actionHelpHomePage;
#endif
};

// vim: ts=4 sw=4 et
