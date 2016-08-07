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




#include <vector>
#include "mo/mo_application.h"

#include "AboutWindow.h"
#include "build_version.h"
//#include "license.cpp"

namespace UI
{

namespace
{
	typedef std::vector<Glib::ustring>	Strings;
}

AboutWindow::AboutWindow()
{
	molib::moApplicationSPtr application( molib::moApplication::Instance() );

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	set_name( (application->GetName() + molib::moWCString(" Demo")).c_str() );
#else
	set_name( application->GetName().c_str() );
#endif

	set_version( (application->GetVersion() + "." BUILD_VERSION_STRING).c_str() );
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
		"Uses Gtk+ 2.15, Gtkmm 2.12, iconv, intl, PNG and zlib.\n"
		"Licensed under the GNU Public License v2.\n";

// TODO: we need to fix the comment for demo versions to work properly
#if defined(DEMO_VERSION) || defined(BETA_VERSION)
	set_comments( comments );
#else
	set_comments( gettext(comments) );
#endif

	Strings	authors;
	authors.push_back( "R. Douglas Barbieri" );
	authors.push_back( "Alexis Wilke" );
	set_authors( authors );

	Strings artists;
	artists.push_back( "Nikolai Lokteff" );
	set_artists( artists );

	// no gettext() for this one, doesn't need translation
	set_copyright( "Copyright (c) 2005-2013 Made to Order Software Corporation" );

	//set_license( gettext(product_license) );

	molib::moWCString logopath = Common::GetFullPathnameForImage( ABOUT_IMAGE );
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

