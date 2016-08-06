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

// Gtkmm
//
#include <gtkmm.h>

// molib
//
#include "mo/mo_image.h"

// Local includes
//
//#include "splash_event.h"

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
#   define SPLASH_IMAGE "turnwatcher-splash-demo.png"
#else
#	define SPLASH_IMAGE "turnwatcher-splash.png"
#endif

namespace UI
{

class SplashScreen : public Gtk::Window
{
public:
							SplashScreen(Gtk::Window *parent);
	virtual					~SplashScreen();

	// moBase
	//
	virtual const char *	GetClassName(void) const;

	virtual bool			on_button_press_event(GdkEventButton *event);
	virtual bool			on_key_press_event(GdkEventKey* event);

	// Gtk::Window
	//
	virtual void			on_show();
	virtual void			on_hide();

private:
	Gtk::EventBox			f_box;
	Gtk::Image				f_splash;
	molib::zuint32_t		f_logo_height;
	molib::moImage			f_logo;
	Gtk::Window*			f_parent;
};

}
// namespace UI


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

