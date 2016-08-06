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

#include "common.h"
#include "mo/mo_application.h"
#include "mo/mo_file.h"
#include "mo/mo_props_xml.h"
#include <glibmm.h>
#include <math.h>

using namespace molib;

namespace Common
{

moWCString GetFullPathnameForImage( const moWCString& basename )
{
	moWCString filename;

	// When debugging you usually don't have a valid install and thus
	// we want to get a file in the current directory or some
	// local share folder. When that fails, we try with a tweaked
	// application data (read only) directory

	// "../share/turnwatcher" is for Win32
	// "../../../../share/turnwatcher" is used in most cases under Linux
	filename = moFile::FindFile(
        ".:src/pixmaps:pixmaps:images:../images"
			":../share/turnwatcher/images:/usr/share/turnwatcher/images",
		basename, moFile::MO_ACCESS_READ);
	if(!filename.IsEmpty()) {
		return filename;
	}

	// test from the application folder...
	moApplicationSPtr application = moApplication::Instance();
	moWCString path = application->GetApplicationPath();
	path = path.FilenameChild("images").FilenameChild(basename);

	if( moFile::Access( path, moFile::MO_ACCESS_READ ) )
	{
		return path;
	}

	// we could not find it
	return "";
}


moWCString GetFullPathnameForDoc( const moWCString& basename )
{
	// TODO: what we really need here is to change the language properly!

	// When debugging you usually don't have a valid install and thus
	// we want to get a file from the current directory or some
	// local share folder. When that fails, we try with a tweaked
	// application data (read only) directory
	//
	// Here is if you start from the src sub-directory
	moWCString filename = "../doc/en";
	filename = filename.FilenameChild( basename );
	if( Glib::file_test( filename.c_str(), Glib::FILE_TEST_EXISTS ) )
	{
		return filename;
	}
	// in case you're like me and start the software form the main directory
	filename = "doc/en";
	filename = filename.FilenameChild( basename );
	if( Glib::file_test( filename.c_str(), Glib::FILE_TEST_EXISTS ) )
	{
		return filename;
	}

	// this is in case you install a tarball; we cd inside the
	// library directory and need to get out 4 times
	// this should actually work with the full install!
	filename = "../../../../share/doc/turnwatcher/html/en";
	filename = filename.FilenameChild( basename );

	if( Glib::file_test( filename.c_str(), Glib::FILE_TEST_EXISTS ) )
	{
		return filename;
	}

	// the full install will always work with this one (we hope...)
	moApplicationSPtr application = moApplication::Instance();
	moWCString path = application->GetApplicationPath(true);
	path = path.FilenameChild("doc/html/en").FilenameChild(basename);

	if( Glib::file_test( path.c_str(), Glib::FILE_TEST_EXISTS ) )
	{
		return path;
	}

	// we could not find it
	return "";
}


int StatToMod( const int stat )
{
	if( stat >= 10 )
		return (stat - 10) / 2;
	else
		return (stat - 11) / 2;
}


int CalculateHP( const int con, const int hit_dice, const int base_hp, const int temp_hp )
{
	const int con_mod = StatToMod( con );
	return (con>0)
		? (int) fmax( (double) (base_hp + hit_dice*con_mod), (double) hit_dice ) + temp_hp
		: 0;
}


Glib::ustring WrapString( const Glib::ustring& string, const int break_chars )
{
	bool need_break = false;
	Glib::ustring wrapped_string;
	const int size = string.size();
	for( int c = 0; c < size; ++c )
	{
		char ch = string[c];
		wrapped_string += ch;
		if( c>=break_chars && (c % break_chars == 0) )
		{
			need_break = true;
		}
		if( need_break )
		{
			if( (ch == ',') || (ch == ' ') )
			{
				wrapped_string += "\n";
				need_break = false;
			}
		}
	}

	return wrapped_string;
}


bool StringToken( moWCString& base, moWCString& element, const moWCString& token )
{
	bool more = false;
	const long tokloc = base.FindInString( token );
	element = "";

	if( tokloc > 0 )
	{
		// Get string, parse to next token
		//
		element = base.Get( 0, tokloc - 1 );
		base = base.Get( tokloc + 1, -1 );
		more = true;
	}
	else if( tokloc == 0 )
	{
		// One token, but an empty string
		//
		element = "";
		base = base.Get( 1, -1 );
		more = true;
	}
	else if( tokloc == -1 )
	{
		// Grab the whole string if no token found
		//
		element = base.Get( 0, -1 );
		base = "";
	}

	element.Strip();

	return more;
}


/** \brief Convert dice count and faces to string in the form of XdYY
 *
 * \param dice		die count
 * \param faces		die faces (e.g. 20)
 * \param modifier	amount to add to each roll (e.g. 2d6+10)
 */
moWCString DiceFacesModifierToStr(int dice, int faces, int modifier)
{
    if(dice <= 0) {
	    dice = 1;
    }
    else if(dice > 10) {
	    dice = 10;
    }
    if(faces < 2) {
	    faces = 2;
    }
    else if(faces > 100) {
	    faces = 100;
    }
    if(modifier < -1000) {
	    modifier = -1000;
    }
    else if(modifier > 1000) {
	    modifier = 1000;
    }
    if(modifier == 0) {
	    return moWCString::Format("%dd%d", dice, faces);
    }
    else {
	    return moWCString::Format("%dd%d%+d", dice, faces, modifier);
    }
}


/** \brief Extract die values from die spec string
 *
 * Spec string has the form: XdY+M where X is the number of dice, Y is the face count
 * and M is the modifier to add to each roll
 *
 * \param str		Spec str containing format above
 * \param dice		Place to store dice count
 * \param faces		Place to store face count
 * \param modifier	Place to store modifier
 */
void StrToDiceFacesModifier(const char *str, int& dice, int& faces, int& modifier)
{
    // NOTE: we completely ignore garbage! it's up to the user to
    //       write a proper die reference
    int	sign;

    // reset values
    dice = 0;
    faces = 0;
    modifier = 0;

    while(isspace(*str)) {
	    ++str;
    }

    // parse 'dice'
    while(*str >= '0' && *str <= '9') {
	    dice = dice * 10 + *str - '0';
	    ++str;
    }
    if(dice <= 0) {
	    dice = 1;
    }
    else if(dice > 10) {
	    dice = 10;
    }

    while(isspace(*str) || *str == 'd') {
	    ++str;
    }

    // parse 'faces'
    while(*str >= '0' && *str <= '9') {
	    faces = faces * 10 + *str - '0';
	    ++str;
    }
    if(faces == 0) {
	    // correct default is improperly defined!
	    faces = 20;
    }
    else if(faces < 2) {
	    faces = 2;
    }
    else if(faces > 100) {
	    faces = 100;
    }

    while(isspace(*str)) {
	    ++str;
    }

    // parse 'modifier'
    if(*str == '+') {
	    sign = 1;
    }
    else if(*str == '-') {
	    sign = -1;
    }
    else {
	    // no + or - thus no modifier
	    return;
    }

    ++str;	// skip the sign
    while(isspace(*str)) {
	    ++str;
    }

    while(*str >= '0' && *str <= '9') {
	    modifier = modifier * 10 + *str - '0';
	    ++str;
    }
    if(modifier < 0) {
	    modifier = 0;
    }
    else if(modifier > 1000) {
	    modifier = 1000;
    }

    modifier *= sign;
}

#ifdef WIN32
#	include <windows.h>
#endif

#ifdef MO_LINUX
#	include <sys/types.h>
#	include <unistd.h>
#endif


#if defined(MO_LINUX)
namespace
{
	// Search for an application under GNU/Linux only.
	//
	moWCString FindExec( const moWCString& path, const char **software )
	{
		moWCString	fullname;

		while(*software != NULL)
		{
			fullname = moFile::FindFile(path, *software, moFile::MO_ACCESS_EXECUTE);
			if(!fullname.IsEmpty())
			{
				break;
			}
			++software;
		}

		return fullname;
	}
}
#endif


void ShowDocumentation( const char *index )
{
#ifdef WIN32
	if(strncmp(index, "http:", 5) == 0)
	{
		ShellExecute(GetDesktopWindow(), "open", index, NULL, "C:\\", SW_SHOWNORMAL);
	}
	else
	{
		moApplication::pointer_t app = moApplication::Instance();
		moWCString path = app->GetApplicationPath();
		moWCString html = path.FilenameChild("doc/en").FilenameChild(index);
		ShellExecute(GetDesktopWindow(), "open", html.c_str(), NULL, path.c_str(), SW_SHOWNORMAL);
	}
#elif defined(MO_LINUX)
	// NOTE: The linux/unix installer should make a soft-link to the current browser
	// for the system
	//
	if( fork() == 0 )
	{
		// possible browser names
        const char *software[] =
			{ "xdg-open"		// Newer method which works with mime
			, "x-www-browser"	// expected name under Debian; user favorite
            , "firefox"			// other browsers, in case the above didn't work
			, "google-chrome"
			, "konqueror"
			, "mozilla"
            , "seamonkey"
			, "opera"
			, "netscape"
			, NULL
			};

		// should we only use the PATH variable?!
		moWCString path("/usr/bin:/usr/local/bin:../../../../bin:../../../../..:");
		path += getenv("PATH");

		// TODO: should we report to the user that we could not find
		// any browser to start?!
		moWCString browser = FindExec( path, software );
#ifdef DEBUG
		std::cerr << "browser: " << browser.c_str() << std::endl;
#endif
		if(!browser.IsEmpty()) {
			if(strncmp(index, "http:", 5) == 0)
			{
				execl( browser.c_str(), browser.c_str(), index, NULL );
			}
			else {
				moWCString html = Common::GetFullPathnameForDoc(index);
				if(!html.IsEmpty()) {
					if(html[0] != '/') {
						html = moFile::FullPath(html);
					}
					html = "file://" + html;
#ifdef DEBUG
					std::cerr << "documentation: " << html.c_str() << std::endl;
#endif
					execl( browser.c_str(), browser.c_str(), html.c_str(), NULL );
				}
			}
		}
		exit(1);
	}
#endif
}


bool LoadBagFromFile( const moWCString& conf_file_basename, moPropBagRef& propBag )
{
	const moWCString fullpath( moApplication::Instance()->GetPrivateUserPath( false /*append_version*/ ).FilenameChild( conf_file_basename ) );
	return moXMLLoadPropBag( fullpath, propBag ) > -1;
}


bool SaveBagToFile( const moWCString& conf_file_basename, moPropBagRef& propBag )
{
	const moWCString fullpath( moApplication::Instance()->GetPrivateUserPath( false /*append_version*/ ).FilenameChild( conf_file_basename ) );
	return moXMLSavePropBag( fullpath, propBag ) > -1;
}


};		// namespace Common

// vim: ts=4 sw=4 syntax=cpp.doxygen

