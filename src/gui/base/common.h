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

#include "mo/mo_string.h"
#include "mo/mo_props.h"

#ifndef HAVE_LIBINTL_H
#   error libintl.h is required!
#endif
#include <libintl.h>
#include <glibmm.h>

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
	molib::moWCString	GetFullPathnameForImage( const molib::moWCString& basename );
	molib::moWCString	GetFullPathnameForDoc( const molib::moWCString& basename );
	int					StatToMod( const int stat );
	int					CalculateHP( const int con, const int hit_dice, const int base_hp, const int temp_hp );
	Glib::ustring 		WrapString( const Glib::ustring& string, const int break_chars = 60 );
	bool				StringToken( molib::moWCString& base, molib::moWCString& element, const molib::moWCString& token );

	molib::moWCString	DiceFacesModifierToStr( int dice, int faces, int modifier );
	void				StrToDiceFacesModifier( const char *str, int& dice, int& faces, int& modifier );

	void				ShowDocumentation( const char *index );

	bool				LoadBagFromFile ( const molib::moWCString& conf_file_basename, molib::moPropBagRef& propBag );
	bool				SaveBagToFile   ( const molib::moWCString& conf_file_basename, molib::moPropBagRef& propBag );
}

// TODO: We will want to not turn this option on at some point so that images can be
//       dynamically loaded. This could open up theming the icons in the application.
#define INCLUDED_PIXMAPS

// vim: ts=4 sw=4 syntax=cpp.doxygen

