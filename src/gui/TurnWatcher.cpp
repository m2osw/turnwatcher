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




#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// STL
//
#include <iostream>
#include <cstdlib>

// GTKMM
// 
#include <gtkmm.h>

// MOLIB
//
#include "mo/mo_base.h"
#include "mo/mo_name.h"
#include "mo/mo_application.h"
#include "mo/mo_image.h"

// LOCAL
//
#include "common.h"
#include "legacy_app.h"
#include "legacy_character.h"
#include "MainWindow.h"
#include "transaction.h"
#include "splash.h"
#include "SingletonManager.h"
#include "system.h"
#include "version.h"

namespace 
{
	molib::moWCString g_progname  ( PACKAGE_NAME                                           );
	molib::moWCString g_version   ( PACKAGE_VERSION                                        );
	molib::moWCString g_copyright ( "Copyright (c) Made to Order Software Corp. 2005-2008" );
};


void ConvertLegacyFiles()
{
	auto appSettings(Application::Manager::Instance().lock()->GetAppSettings().lock());
	assert(appSettings);

	Application::LegacyApp lapp;
	if( lapp.Load() )
	{
        auto initMgr( Application::Manager::Instance().lock()->GetInitMgr().lock() );
		assert(initMgr);
		//
		initMgr ->InRounds    ( lapp.inRounds    () );
		initMgr ->RoundNumber ( lapp.roundNumber () );
		initMgr ->CurrentInit ( lapp.currentInit () );

		appSettings->ToolBarPos  ( lapp.toolBarPos    () );
		appSettings->Width       ( lapp.width         () );
		appSettings->Height      ( lapp.height        () );
		appSettings->UserPath    ( lapp.currentFolder () );
		appSettings->UltraInit   ( lapp.ultraInit     () );
		appSettings->ShowToolbar ( lapp.showToolbar   () );
		appSettings->SkipDead    ( lapp.skipDead      () );
		//
        Combatant::LegacyCharacter::List charList;
		lapp.GetCharList( charList );
		//
		appSettings->SetLegacyCharacters( charList, NULL );
	}
	else
	{
		// Open up version 1.1 files to copy into main conf directory, since we are moving
		// away from a versionized conf directory. In other words, instead of
		// ~/.turnwatcher-1.X we have ~/.turnwatcher. The same under Win32.
		//
		auto application( molib::moApplication::Instance() );
		application->ResetPrivateUserPath();
		application->SetVersion( "1.1" );

		molib::moWCString	confDir	 = application->GetPrivateUserPath();
		molib::moWCString	confFile = confDir.FilenameChild( DEFAULT_FILENAME );
		std::cout << "1.1 ConfFile: " << confFile.c_str() << std::endl;
		appSettings->Load( confFile );

        application->SetVersion( g_version );

		// Reset path
		//
		application->ResetPrivateUserPath();
	}
}


int real_main(int argc, char *argv[])
{
	static const char opts[] =
			"[*];"
			;

	auto application( molib::moApplication::Instance() );
	application->SetName		( "turnwatcher"	);
	application->SetVersion		( g_version	);
	application->SetCopyright	( g_copyright	);
	//
#ifndef WIN32
	molib::moWCString appPath = molib::moWCString(DATADIR) + "/turnwatcher";
	application->SetApplicationPath( appPath );
#endif
	application->SetOptions(opts, argc, const_cast<const char **>(argv));

	// This must be done before we create the singleton manager as Actions::Manager needs
	// GTK things to be initialized--otherwise, CRASH!
	//
	Gtk::Main kit( argc, argv );

	// Get application settings
	//
	auto appSettings(Application::Manager::Instance().lock()->GetAppSettings().lock());
	assert(appSettings);
	appSettings->UserPath( application->GetUserPath() );

	// Load the configuration file
	//
	molib::moWCString confFile( application->GetPrivateUserPath( false /*append_version*/ ).FilenameChild( DEFAULT_FILENAME ) );
#if defined(DEMO_VERSION)
	appSettings->Load( confFile );
#else
#ifdef DEBUG
	std::cout << "Loading confFile: " << confFile.c_str() << std::endl;
#endif
	if( !appSettings->Load( confFile ) )
	{
		ConvertLegacyFiles();
	}
#endif

	// Show the UI
	//
	UI::MainWindow	mainWindow( confFile );
	Gtk::Main::run( mainWindow );

	return 0;
}
  

int main(int argc, char *argv[])
{
#ifdef DEBUG
	std::cout << "Welcome to turnwatcher v" << VERSION << " DEBUG BUILD!" << std::endl;
#endif

	const int r = real_main( argc, argv );

#ifdef DEBUG
	// we need to tell all the singletons to go away
	Application::Manager::LocalRelease();

	molib::moNamePool::Done();
	molib::moImageFileFactory::Done();
	molib::moApplication::Done();

	// print out the objects (moBase derived objects)
	molib::moBase::ShowAllocatedObjects();

	// check all the malloc, realloc, free
	molib::mo_show_allocated_buffers();

#ifdef DEBUG
	std::cout << "Successful application termination" << std::endl;
#endif
#endif

	return r;
}

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen


