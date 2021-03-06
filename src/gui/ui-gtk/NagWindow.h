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

#define NAG_WINDOW_GOTO_SITE	1000

namespace UI
{

class NagWindow : public Gtk::AboutDialog
{
public:
	NagWindow( Gtk::Window& parent );
	~NagWindow();

	typedef enum { NAG_EXIT, NAG_FEATURE, NAG_TIMEOUT, NAG_MAXCHARS, NAG_NOSAVE } NagType;
	void Update( const NagType nagType = NAG_EXIT );
};

}
// namespace UI


// vim: ts=4 sw=4


