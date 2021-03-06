//===============================================================================
// Copyright (c) 2005-2016 by Made to Order Software Corporation
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

#include <vector>
#include "mo/mo_application.h"
#include "ui/AboutWindow.h"
#include "config/build_version.h"

using namespace molib;

namespace UI
{


AboutWindow::AboutWindow()
{
    typedef std::vector<QString>	UStringList;
    moApplicationSPtr application( moApplication::Instance() );

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	set_name( (application->GetName() + moWCString(" Demo")).c_str() );
#else
	set_name( application->GetName().c_str() );
#endif

	set_version( application->GetVersion().c_str() );
	set_website( WEBSITE_URL );
	set_translator_credits( gettext("translator-credits") );

	const char* comments =																																																										
		"A Dungeon Master's Initiative Manager\n\n"
#if defined(DEMO_VERSION)
		"This version of Turn Watcher is a demonstration version only.\n\n"
#elif defined(BETA_VERSION)
		"This version of Turn Watcher is a beta version for demonstration purposes only, "
		"and is intended solely for registered beta testers. "
		"If you are not a registered beta tester, please uninstall this application and contact "
		"Made to Order Software Corporation to purchase a licensed copy.\n\n"
#endif
		"Uses Gtkmm 2.4, iconv, intl, PNG and zlib.\n"
		"Licensed under the GNU Public License v2 by Made to Order Software Corp.\n"
		"https://github.com/m2osw/turnwatcher/\n"
		;

// TODO: we need to fix the comment for demo versions to work properly
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	set_comments( comments );
#else
	set_comments( gettext(comments) );
#endif

    UStringList	authors;
	authors.push_back( "R. Douglas Barbieri" );
	authors.push_back( "Alexis Wilke" );
	set_authors( authors );

    UStringList artists;
	artists.push_back( "Nikolai Lokteff" );
	set_artists( artists );

	// no gettext() for this one, doesn't need translation
	set_copyright( "Copyright (c) 2005-2016 Made to Order Software Corporation" );

	//set_license( gettext(product_license) );

	moWCString logopath( Common::GetFullPathnameForImage( ABOUT_IMAGE ) );
	if( logopath != "" )
	{
		Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file( logopath.c_str() );
		set_logo( logo );
	}
}


AboutWindow::~AboutWindow()
{
	// Empty
}

}
// namespace UI


// vim: ts=4

