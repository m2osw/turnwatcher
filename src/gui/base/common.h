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

#pragma once

#include <libintl.h>

#include "mo/mo_props.h"

#include <QString>

#if defined(BETA_VERSION)
#   define WEBSITE_URL "http://www.turnwatcher.com/en/beta.html"
#else
#	define WEBSITE_URL "http://www.turnwatcher.com/"
#endif

// Configuration base filename
//
#define DEFAULT_FILENAME	"turnwatcher.conf"

#if defined(DEMO_VERSION)
#	define NAG_TIMEOUT_SECS		60
#	define NAG_INTERVAL_MINS	05
#	define NAG_QUIT_MINS		30
#	define NAG_MAX_CHARACTERS	 5
#endif

namespace Common
{
    QString				GetFullPathnameForImage( const QString& basename );
    QString				GetFullPathnameForDoc( const QString& basename );
	int					StatToMod( const int stat );
	int					CalculateHP( const int con, const int hit_dice, const int base_hp, const int temp_hp );
    QString 			WrapString( const QString& string, const int break_chars = 60 );
    bool				StringToken( QString& base, QString& element, const QString& token );

    QString				DiceFacesModifierToStr( int dice, int faces, int modifier );
	void				StrToDiceFacesModifier( const char *str, int& dice, int& faces, int& modifier );

    void				ShowDocumentation( QString const & index );

    bool				LoadBagFromFile ( const QString& conf_file_basename, molib::moPropBagRef& propBag );
    bool				SaveBagToFile   ( const QString& conf_file_basename, molib::moPropBagRef& propBag );

    bool				FileExists( QString const & path );
}

// TODO: We will want to not turn this option on at some point so that images can be
//       dynamically loaded. This could open up theming the icons in the application.
#define INCLUDED_PIXMAPS

// This deals with turning on/off the appropriate
// switches for the UI
//
#if defined(OLD_UI)
#   undef WANT_PANE
#  	define WANT_EFFECTS	1
#  	undef WANT_NOTES
#else
#   undef	WANT_PANE
#	define WANT_EFFECTS	1
//# undef WANT_EFFECTS
#	define WANT_NOTES	1
//# undef WANT_NOTES
#endif

// vim: ts=4 sw=4 syntax=cpp.doxygen

