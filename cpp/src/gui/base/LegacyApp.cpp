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

#include "base/LegacyApp.h"
#include "base/AppSettings.h"
#include "base/CharacterManager.h"
#include "base/InitiativeManager.h"
#include "base/StatManager.h"
#include "base/transaction.h"
#include "base/common.h"

#include "mo/mo_application.h"
#include "mo/mo_props_xml.h"
#include "mo/mo_file.h"

using namespace molib;
using namespace Combatant;
using namespace Transactions;

namespace Application
{

namespace
{
	// 1.0
	//
    const QString g_mainPropBag   ("TURN_WATCHER Main Property Bag");
    const QString g_charBag       ("CHARACTERS");
    const QString g_inRounds      ("IN_ROUNDS");
    const QString g_roundNumber   ("ROUND_NUMBER");
    const QString g_currentInit   ("CURRENT_INIT");
    const QString g_toolbarPos    ("TOOLBAR_POS");
    const QString g_windowLeft    ("WINDOW_LEFT");
    const QString g_windowTop     ("WINDOW_TOP");
    const QString g_windowHeight  ("WINDOW_HEIGHT");
    const QString g_windowWidth   ("WINDOW_WIDTH");
    const QString g_currentFolder ("CURRENT_FOLDER");
    const QString g_ultraInit     ("ULTRA_INITIATIVE");
    const QString g_showToolbar   ("SHOW_TOOLBAR");
    const QString g_skipDead      ("SKIP_DEAD");
	//
	// 1.1+
	//
    const QString g_version  ("VERSION");
    const QString g_initBag  ("INITIATIVE");
    const QString g_statsBag ("STATS");

    bool GetVersion( moPropBagRef& bag, QString& version )
	{
		moPropStringRef	versionProp( g_version );
		versionProp.Link( bag );

		bool has_version = false;
		//
		if( versionProp.HasProp() )
		{
            version = static_cast<moWCString>(versionProp).c_str();
			has_version = true;
		}

		return has_version;
	}
}

LegacyApp::LegacyApp()
	: f_inRounds	 (false)
	, f_roundNumber	 (0)
	, f_currentInit	 (0)
	, f_toolBarPos	 (0)
	, f_windowLeft	 (0)
	, f_windowTop	 (0)
	, f_windowWidth	 (0)
	, f_windowHeight (0)
	, f_ultraInit	 (false)
	, f_showToolbar	 (false)
	, f_skipDead	 (false)
{
	// empty
}


bool LegacyApp::LoadCharacters( moPropBagRef charPropBag )
{
	const int count = charPropBag.Count();

	for( int idx = 0; idx < count; ++idx )
	{
		if( charPropBag[idx].HasProp() )
		{
			LegacyCharacter::pointer_t ch( new LegacyCharacter( charPropBag[idx] ) );
			f_charList.push_back( ch );
		}
	}

	return count > 0;
}


void LegacyApp::Load( moPropBagRef mainPropBag )
{
	// Load application state
	//
	moPropIntRef    inRounds      ( g_inRounds      ); 
	moPropIntRef    roundNumber   ( g_roundNumber   ); 
	moPropIntRef    currentInit   ( g_currentInit   ); 
	moPropIntRef    toolbarPos    ( g_toolbarPos    ); 
	moPropIntRef    windowHeight  ( g_windowHeight  ); 
	moPropIntRef    windowWidth   ( g_windowWidth   ); 
	moPropStringRef currentFolder ( g_currentFolder ); 
	moPropIntRef    ultraInit     ( g_ultraInit     ); 
	moPropIntRef    showToolbar   ( g_showToolbar   ); 
	moPropIntRef    skipDead      ( g_skipDead      ); 
	//
	inRounds.Link      ( mainPropBag ); if( inRounds.HasProp()      ) f_inRounds      = inRounds;
	roundNumber.Link   ( mainPropBag ); if( roundNumber.HasProp()   ) f_roundNumber   = roundNumber;
	currentInit.Link   ( mainPropBag ); if( currentInit.HasProp()   ) f_currentInit   = currentInit;
	toolbarPos.Link    ( mainPropBag ); if( toolbarPos.HasProp()    ) f_toolBarPos    = toolbarPos;
	windowHeight.Link  ( mainPropBag ); if( windowHeight.HasProp()  ) f_windowHeight  = windowHeight;
	windowWidth.Link   ( mainPropBag ); if( windowWidth.HasProp()   ) f_windowWidth   = windowWidth;
	currentFolder.Link ( mainPropBag ); if( currentFolder.HasProp() ) f_currentFolder = static_cast<moWCString>(currentFolder).c_str();
	ultraInit.Link     ( mainPropBag ); if( ultraInit.HasProp()     ) f_ultraInit     = ultraInit;
	showToolbar.Link   ( mainPropBag ); if( showToolbar.HasProp()   ) f_showToolbar   = showToolbar;
	skipDead.Link      ( mainPropBag ); if( skipDead.HasProp()      ) f_skipDead      = skipDead;

	// Load characters
	//
	moPropBagRef charPropBag( g_charBag );
	charPropBag.Link( mainPropBag );
	LoadCharacters( charPropBag );
}


bool LegacyApp::Load()
{
	moApplicationSPtr application( moApplication::Instance() );
	assert(application);

	// Save version for after we temporarily alter the version for the legacy load below.
	//
    const QString currentVersion( application->GetVersion().c_str() );

	// Set the old version so we can reload
	//
	application->ResetPrivateUserPath();
	application->SetVersion( "1.0" );
	//
    const QString confDir  ( application->GetPrivateUserPath().c_str() );
    const QString confFile ( QString("%1/%2").arg(confDir).arg(DEFAULT_FILENAME) );

	// Load previous state
	//
	moPropBagRef mainPropBag( g_mainPropBag );
#ifdef DEBUG
	std::cerr << "Old ConfFile: " << confFile.c_str() << std::endl;
#endif
    const int ret_val = moXMLLoadPropBag( confFile, mainPropBag );

	// Load the old data
	//
	bool loaded = false;
	if( ret_val == 0 )
	{
		Load( mainPropBag );
		loaded = true;
	}

	// Put the current version back
	//
	application->ResetPrivateUserPath();
	application->SetVersion( currentVersion );

	return loaded;
}


void LegacyApp::GetCharList( LegacyCharacter::list_t& charlist )
{
	charlist = f_charList;
}


/// \brief Convert tw legacy files, so we may import them.
//
void LegacyApp::ConvertLegacyFiles( const QString& lversion )
{
	auto appSettings(Application::AppSettings::Instance().lock());
	assert(appSettings);
	auto charMgr(CharacterManager::Instance().lock());
	assert(charMgr);
	auto initMgr(Initiative::InitiativeManager::Instance().lock());
	assert(initMgr);
	auto statMgr(Attribute::StatManager::Instance().lock());
	assert(statMgr);

	// Try 1.0 files first
	//
	LegacyApp lapp;
	if( lapp.Load() )
	{
		initMgr->InRounds        ( lapp.inRounds    () );
		initMgr->RoundNumber     ( lapp.roundNumber () );
		initMgr->CurrentInit     ( lapp.currentInit () );
		//
		appSettings->ToolBarPos  ( lapp.toolBarPos    () );
		appSettings->Width       ( lapp.width         () );
		appSettings->Height      ( lapp.height        () );
		appSettings->UserPath    ( lapp.currentFolder () );
		appSettings->UltraInit   ( lapp.ultraInit     () );
		appSettings->ShowToolbar ( lapp.showToolbar   () );
		appSettings->SkipDead    ( lapp.skipDead      () );
		//
        LegacyCharacter::list_t charList;
		lapp.GetCharList( charList );
		//
        TransactionGroup::pointer_t group( new TransactionGroup("Add Legacy Chars") );
        charMgr->SetLegacyCharacters( charList, group, false /*emit_signals*/ );
		group->doit();
	}
	else
	{
		// Open up version 1.1 files to copy into main conf directory, since we are moving
		// away from a versionized conf directory. In other words, instead of
		// ~/.turnwatcher-1.X we have ~/.turnwatcher. The same under Win32.
		//
		auto application( moApplication::Instance() );
		application->ResetPrivateUserPath();
		application->SetVersion( lversion );

		moPropBagRef mainPropBag( "TURNWATCHER"	);
		if( Common::LoadBagFromFile( DEFAULT_FILENAME, mainPropBag ) )
		{
			moPropBagRef charPropBag( g_charBag );
			charPropBag.Link( mainPropBag );
			if( charPropBag.HasProp() )
			{
                charMgr->PermanentClear();
				charMgr->LoadCharacters( charPropBag );
			}

			moPropBagRef statsBag( g_statsBag );
			statsBag.Link( mainPropBag );
			if( statsBag.HasProp() )
			{
				statMgr->LoadStats( statsBag );
			}

			moPropBagRef initBag( g_initBag );
			initBag.Link( mainPropBag );
			if( initBag.HasProp() )
			{
				initMgr->LoadInitData( initBag );
			}
		}

		// Reset path back to normal
		//
		application->ResetPrivateUserPath();
        application->SetVersion( PACKAGE_VERSION );
	}

	// Now write out the new config files
	//
	appSettings->Save();
	charMgr->Save();
	initMgr->Save();
	statMgr->Save();

	// Reload all of the managers
	//
	appSettings->Release();
	initMgr->Release();
    statMgr->Release();
	charMgr->Release();
}


void LegacyApp::ConvertLegacyConfig()
{
	// Load the configuration file
	//
	moPropBagRef mainPropBag( "TURNWATCHER"	);
	if( Common::LoadBagFromFile( DEFAULT_FILENAME, mainPropBag ) )
	{
        QString version;
		if( !GetVersion( mainPropBag, version ) )
		{
            ConvertLegacyFiles( "1.0" );
		}
		else if( version < PACKAGE_VERSION )
		{
			ConvertLegacyFiles( version );
		}
	}
}


}
// namespace Application

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

