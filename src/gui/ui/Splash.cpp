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

#include "common.h"

// mo
//
#include "mo/mo_application.h"
#include "mo/mo_image.h"
#include "mo/mo_template.h"

#include "ui/Splash.h"

// We need to specify the main application icon here too, otherwise the main window
// loses it's own (an X windows bug).
//
#include "pixmaps/Helmet.xpm"

//////////////////////////////////////////////////////////////////////
//
// Splash Screen object
//
// This is the window used for the Splash Screen
//
//////////////////////////////////////////////////////////////////////

namespace UI
{


SplashScreen::SplashScreen(Gtk::Window *parent) :
	Gtk::Window(Gtk::WINDOW_POPUP),
	f_parent( parent )
{
    molib::moWCString splash_path( Common::GetFullPathnameForImage( SPLASH_IMAGE ) );

	if( splash_path != "" )
	{
		splash_path = molib::moFile::FullPath(splash_path);
		f_splash.set( splash_path.c_str() );
	}

#if 0
	// When debugging you usually don't have a valid install and thus
	// we want to get the image in the current directory or some
	// local share folder. When that fails, we try with a tweaked
	// application data (read only) directory
	if(f_splash.get_storage_type() == Gtk::IMAGE_EMPTY
	|| f_splash.get_storage_type() == Gtk::IMAGE_STOCK) {
		f_splash.set("../share/turnwatcher/TurnWatcher-Splash.png");

		if(f_splash.get_storage_type() == Gtk::IMAGE_EMPTY
		|| f_splash.get_storage_type() == Gtk::IMAGE_STOCK) {
			molib::moApplicationSPtr app = molib::moApplication::Instance();
			molib::moWCString path = app->GetApplicationPath();
			path = path.FilenameChild("TurnWatcher-Splash.png");
			f_splash.set(path.c_str());
			std::cout << "path: " << path.c_str() << std::endl;
		}
	}
#endif
	set_type_hint(Gdk::WINDOW_TYPE_HINT_SPLASHSCREEN);
	set_title(gettext("Turn Watcher Splashscreen"));
	set_resizable(false);
	set_decorated(false);
	set_has_frame(false);
	set_position(Gtk::WIN_POS_CENTER);
	set_transient_for(*f_parent);	// avoid having an icon in toolbar
	set_modal(true);		// the first click closes the splash no matter where
	set_keep_above();		// keep splashscreen on top of everything
	set_icon( Gdk::Pixbuf::create_from_xpm_data( Helmet_xpm ) );

	add(f_box);

	f_box.add(f_splash);

	f_box.set_events(Gdk::BUTTON_PRESS_MASK);
	f_box.signal_button_press_event().connect(
			sigc::mem_fun(*this, &SplashScreen::on_button_press_event));

	show_all_children();
}



SplashScreen::~SplashScreen()
{
}


const char *SplashScreen::GetClassName(void) const
{
	return "sswfgui::SplashScreen";
}


bool SplashScreen::on_button_press_event(GdkEventButton *event)
{
	hide();
	return true;
}


bool SplashScreen::on_key_press_event(GdkEventKey* event)
{
	hide();
	return true;
}


void SplashScreen::on_show()
{
	Gtk::Window::on_show();
	//
	// Does nothing
}


void SplashScreen::on_hide()
{
	Gtk::Window::on_hide();
	//
	// Try to keep the main window in focus as we dismiss
	//
	f_parent->grab_focus();
}


}
// namespace UI




// vim: ts=8

