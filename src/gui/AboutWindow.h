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

#include <gdkmm.h>
#include <gtkmm.h>

#include "common.h"

#if defined(DEMO_VERSION) || defined(BETA_VERSION)
#define ABOUT_IMAGE "turnwatcher-about-demo.png"
#else
#define ABOUT_IMAGE "turnwatcher-about.png"
#endif


namespace UI
{

class AboutWindow : public Gtk::AboutDialog
{
public:
	AboutWindow();
	~AboutWindow();
};

}
// namespace UI


// vim: ts=4

