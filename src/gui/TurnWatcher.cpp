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

// STL
//
#include <cstdlib>
#include <fstream>
#include <iostream>

// GTKMM
// 
#include <gtkmm.h>

// MOLIB
//
#include "mo/mo_name.h"
#include "mo/mo_application.h"

// LOCAL
//
#include "base/common.h"
#include "base/LegacyApp.h"
#include "base/LegacyCharacter.h"
#include "ui/MainWindow.h"
#include "ui/Splash.h"
#include "version.h"

using namespace molib;

namespace
{
#ifdef DEBUG
	// Gtk/gdk noise output file, in DEBUG builds only.
	//
    const char* OUTPUT_LOG = "turnwatcher_gtk_debug.log";
#endif

	// molib uses these to figure out where to open control files.
	//
	moWCString g_progname  ( PACKAGE_NAME                                           );
	moWCString g_version   ( PACKAGE_VERSION                                        );
	moWCString g_copyright ( "Copyright (c) Made to Order Software Corp. 2005-2016" );
};


/// \brief Turn off all gtk warnings and errors.
/// \note Logs gtk/gdk noise to an output file, but only in a DEBUG build.
//
void my_gtk_error_handler
        ( const gchar *		log_domain
        , GLogLevelFlags	log_level
        , const gchar*		message
        , gpointer 			user_data
        )
{
#ifdef DEBUG
    std::ofstream log;
    log.open( OUTPUT_LOG, std::ios_base::app );
    log << message << std::endl;
    log.close();
#endif
}


/// \brief "Real" main method, which converts legacy files and runs the application.
//
int real_main(int argc, char *argv[])
{
	static const char opts[] =
			"[*];"
			"--gtk-module string;"
			"--g-fatal-warnings;"
			"--gtk-debug string;"
			"--gtk-no-debug string;"
			"--class string;"
			"--name string;"
			"--gdk-debug string;"
			"--gdk-no-debug string;"
			;

	auto application( moApplication::Instance() );
	application->SetName		( "turnwatcher"	);
	application->SetVersion		( g_version	    );
	application->SetCopyright	( g_copyright	);
	//
#ifndef WIN32
	const moWCString appPath( moWCString(DATADIR) + "/turnwatcher" );
	application->SetApplicationPath( appPath );
#endif
	application->SetOptions(opts, argc, const_cast<const char **>(argv));

	// Convert any legacy files if they exist.
	//
    Application::LegacyApp::ConvertLegacyConfig();

    // This must be done before we create the singleton manager as Actions::Manager needs
	// GTK things to be initialized--otherwise, CRASH!
	//
    Gtk::Main kit( argc, argv );

	// Get application settings, which creates the singleton manager.
	// Then update the user path.
	//
	auto appSettings(Application::AppSettings::Instance().lock());
	assert(appSettings);
	appSettings->UserPath( application->GetUserPath().c_str() );

	// Show the UI
	//
	UI::MainWindow	mainWindow;
	Gtk::Main::run( mainWindow );

	return 0;
}
  

/// \brief Low-level main function.
//
int main(int argc, char *argv[])
{
#ifdef DEBUG
    unlink( OUTPUT_LOG );
	std::cerr << "Welcome to turnwatcher v" << VERSION << " DEBUG BUILD!" << std::endl;
#endif

    // Set this error handler to quiet all of the gtk noise.
    //
    g_log_set_default_handler( my_gtk_error_handler, 0 );

    // Call the "real" main method to start and run the app
    //
	const int r = real_main( argc, argv );

    // Release everything before shutdown
    //
    Actions::Manager::Instance                 ().lock()->Release();
    Application::AppSettings::Instance         ().lock()->Release();
    Combatant::CharacterManager::Instance      ().lock()->Release();
    Combatant::CharacterModel::Instance        ().lock()->Release();
    Initiative::InitiativeManager::Instance    ().lock()->Release();
    Attribute::StatManager::Instance           ().lock()->Release();
    Transactions::TransactionManager::Instance ().lock()->Release();

	moNamePool::Done();
	moImageFileFactory::Done();
	moApplication::Done();

#ifdef DEBUG
    // print out the objects (moBase derived objects)
    //
	moBase::ShowAllocatedObjects();

	// check all the malloc, realloc, free
    //
    mo_show_allocated_buffers();

	std::cerr << "Successful application termination" << std::endl;
#endif

	return r;
}

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen


