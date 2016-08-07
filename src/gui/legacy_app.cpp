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




#include "common.h"
#include "legacy_app.h"

#include "mo/mo_application.h"
#include "mo/mo_props_xml.h"
#include "mo/mo_file.h"

namespace Application
{

namespace
{
	const molib::moWCString g_mainPropBag	("TURN_WATCHER Main Property Bag");
	const molib::moWCString g_charBag	("CHARACTERS");
	const molib::moWCString g_inRounds	("IN_ROUNDS");
	const molib::moWCString g_roundNumber	("ROUND_NUMBER");
	const molib::moWCString g_currentInit	("CURRENT_INIT");
	const molib::moWCString g_toolbarPos	("TOOLBAR_POS");
	const molib::moWCString g_windowLeft	("WINDOW_LEFT");
	const molib::moWCString g_windowTop	("WINDOW_TOP");
	const molib::moWCString g_windowHeight	("WINDOW_HEIGHT");
	const molib::moWCString g_windowWidth	("WINDOW_WIDTH");
	const molib::moWCString g_currentFolder	("CURRENT_FOLDER");
	const molib::moWCString g_ultraInit	("ULTRA_INITIATIVE");
	const molib::moWCString g_showToolbar	("SHOW_TOOLBAR");
	const molib::moWCString g_skipDead	("SKIP_DEAD");
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


bool LegacyApp::LoadCharacters( molib::moPropBagRef charPropBag )
{
	const int count = charPropBag.Count();

	for( int idx = 0; idx < count; ++idx )
	{
		if( charPropBag[idx].HasProp() )
		{
			Combatant::LegacyCharacter::Pointer ch( new Combatant::LegacyCharacter( charPropBag[idx] ) );
			f_charList.push_back( ch );
		}
	}

	return count > 0;
}


void LegacyApp::Load( molib::moPropBagRef mainPropBag )
{
	// Load application state
	//
	molib::moPropIntRef inRounds( g_inRounds   );
	inRounds.Link( mainPropBag );
	if( inRounds.HasProp() ) f_inRounds= inRounds;

	molib::moPropIntRef roundNumber( g_roundNumber );
	roundNumber.Link( mainPropBag );
	if( roundNumber.HasProp() ) f_roundNumber = roundNumber;

	molib::moPropIntRef currentInit( g_currentInit );
	currentInit.Link( mainPropBag );
	if( currentInit.HasProp() ) f_currentInit = currentInit;

	molib::moPropIntRef toolbarPos( g_toolbarPos );
	toolbarPos.Link( mainPropBag );
	if( toolbarPos.HasProp() ) f_toolBarPos = toolbarPos;

	molib::moPropIntRef windowHeight( g_windowHeight );
	windowHeight.Link( mainPropBag );
	if( windowHeight.HasProp() ) f_windowHeight = windowHeight;

	molib::moPropIntRef windowWidth( g_windowWidth );
	windowWidth.Link( mainPropBag );
	if( windowWidth.HasProp() ) f_windowWidth = windowWidth;

	molib::moPropStringRef currentFolder( g_currentFolder );
	currentFolder.Link( mainPropBag );
	if( currentFolder.HasProp() ) f_currentFolder = currentFolder;

	molib::moPropIntRef ultraInit( g_ultraInit );
	ultraInit.Link( mainPropBag );
	if( ultraInit.HasProp() ) f_ultraInit = ultraInit;

	molib::moPropIntRef showToolbar( g_showToolbar );
	showToolbar.Link( mainPropBag );
	if( showToolbar.HasProp() ) f_showToolbar = showToolbar;

	molib::moPropIntRef skipDead( g_skipDead );
	skipDead.Link( mainPropBag );
	if( skipDead.HasProp() ) f_skipDead = skipDead;

	// Load characters
	//
	molib::moPropBagRef charPropBag( g_charBag );
	charPropBag.Link( mainPropBag );
	LoadCharacters( charPropBag );
}


bool LegacyApp::Load()
{
	molib::moApplicationSPtr application( molib::moApplication::Instance() );
	assert(application);

	// Save version for after we temporarily alter the version for the legacy load below.
	//
	const molib::moWCString currentVersion( application->GetVersion() );

	// Set the old version so we can reload
	//
	application->ResetPrivateUserPath();
	application->SetVersion( "1.0" );
	//
	const molib::moWCString confDir	 ( application->GetPrivateUserPath()         );
	const molib::moWCString confFile ( confDir.FilenameChild( DEFAULT_FILENAME ) );

	// Load previous state
	//
	molib::moPropBagRef mainPropBag( g_mainPropBag );
	std::cout << "Old ConfFile: " << confFile.c_str() << std::endl;
	const int ret_val = molib::moXMLLoadPropBag( confFile.c_str(), mainPropBag );

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


void LegacyApp::GetCharList( Combatant::LegacyCharacter::List& charlist )
{
	charlist = f_charList;
}


}
// namespace Application

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

