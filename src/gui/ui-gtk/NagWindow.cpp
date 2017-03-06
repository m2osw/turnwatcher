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

#include "NagWindow.h"
#include "build_version.h"
#include "common.h"
#include "splash.h"

#include "mo/mo_application.h"
#include "mo/mo_image.h"

#include <iostream>

namespace UI
{

namespace
{
	typedef std::vector<Glib::ustring>	Strings;
}

NagWindow::NagWindow(Gtk::Window& parent)
{
#if !defined(DEMO_VERSION) && !defined(BETA_VERSION)
	std::cout << "This should only be called for the demo / beta versions! Please fix this!" << std::endl;
	assert( 0 );
#endif
	set_transient_for( parent );
	add_button( gettext("Get Turn Watcher Now!") , NAG_WINDOW_GOTO_SITE );
}

NagWindow::~NagWindow()
{
	// Empty
}

void NagWindow::Update( const NagType nagType )
{
	molib::moApplicationSPtr	application = molib::moApplication::Instance();
	molib::moWCString			comments;
	molib::moWCString			name;
	//molib::moWCString	version;

	switch( nagType )
	{
	case NAG_EXIT:
#if defined(DEMO_VERSION)
		name = 
			"Thank You for Trying Turn Watcher!";
		comments = 
			"We hope you enjoyed using Turn Watcher and find it a useful tool. "
			"Order your copy of Turn Watcher today and help us "
			"deliver more quality software like this to people like you!\n";
#else
		name =
			application->GetName() + " Beta";
		comments = 
			"Thanks for testing the Beta version of Turn Watcher! You can buy the full version "
			"at our website, where we will offer you a discount!\n";
#endif
		break;
		
	case NAG_FEATURE:
		name =
			"Full Version Only!";
		comments =
			"This feature is available in the full version of Turn Watcher. Go to our website "
			"to order your copy today!\n";
		break;

	case NAG_TIMEOUT:
		name =
			"Register today!";
		comments =
			"This DEMO version of Turn Watcher will display this window every 5 minutes of use. "
			"After 30 minutes of use, Turn Watcher will terminate automatically. If you want to make this "
			"annoying window go away, go to our website and buy a registered version today!\n";
		break;
		
	case NAG_MAXCHARS:
		name = 
			"Maximum Combatants Reached!";
		comments =
			"You can only add up to 5 characters/monsters with the DEMO version of Turn Watcher."
			"If you want to add more, get the full version today from our website today!";
		break;

	case NAG_NOSAVE:
		name =
			"No Save for You!";
		comments =
			"This demonstration version of Turn Watcher will let you add combatants, but will not save your combatants "
			"between sessions. If you would like to save, export and import combatants, please get the full "
			"version from our website today!";
		break;
	}

	// Never use gettext for demo/beta stuff!
	//
	set_name( name.c_str() );
	set_comments( comments.c_str() );
	set_website( WEBSITE_URL );
	//set_version( (application->GetVersion() + "." BUILD_VERSION_STRING).c_str() );

	// no gettext() for this one, doesn't need translation
	set_copyright( "Copyright (c) 2005-2008 Made to Order Software Corporation" );

	molib::moWCString logopath = Common::GetFullPathnameForImage( SPLASH_IMAGE );
	if( logopath != "" )
	{
		Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file( logopath.c_str() );
		set_logo( logo );
	}
}

}
// namespace UI

// vim: ts=4 sw=4


