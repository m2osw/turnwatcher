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




#include "StatusBox.h"


namespace UI
{


StatusBox::StatusBox()
{
	f_statusBar.set_has_resize_grip( false );
	f_statusInit.set_has_resize_grip( false );
	f_statusRounds.set_has_resize_grip( true );

	pack_start( f_statusBar );
	pack_start( f_statusInit );
	pack_start( f_statusRounds );

	auto initMgr(GetInitMgr().lock());
	assert(initMgr);
	//
	initMgr->signal_start_rounds().connect( sigc::mem_fun( *this, &StatusBox::Update ) );
	initMgr->signal_end_rounds()  .connect( sigc::mem_fun( *this, &StatusBox::Update ) );
	//
	GetAppSettings().lock()->signal_changed().connect( sigc::mem_fun( *this, &StatusBox::Update ) );
}


void StatusBox::PushMessage( const char* msg )
{
	f_statusBar.push( msg );
	show_all_children();
}


void StatusBox::PopMessage()
{
	f_statusBar.pop();
	show_all_children();
}


void StatusBox::Update()
{
	f_statusBar.pop();
	f_statusRounds.pop();
	f_statusInit.pop();

	auto initMgr(GetInitMgr().lock());
	assert(initMgr);
	//
	if( initMgr->InRounds() )
	{
		std::ostringstream	str;
		str << gettext("Round: ") << initMgr->RoundNumber();
		f_statusRounds.push( Glib::locale_to_utf8( str.str() ) );
	}
	else
	{
		f_statusRounds.push( gettext( "Ready" ) );
	}

	auto appSettings(GetAppSettings().lock());
	assert(appSettings);
	//
	f_statusInit.push( appSettings->UltraInit()? gettext( "Ultra" ) : gettext( "Normal" ) );
	f_statusBar .push( appSettings->StatusMsg().c_str() );
}

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

