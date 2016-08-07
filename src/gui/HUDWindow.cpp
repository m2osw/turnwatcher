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




// mo
//
#include "mo/mo_application.h"
#include "mo/mo_template.h"

#include "HUDWindow.h"

// We need to specify the main application icon here too, otherwise the main window
// loses it's own (an X windows bug).
//
#include "pixmaps/Helmet.xpm"

/** \class CharacterListUI
 *
 * The HUDWindow functions as a Heads Up Display Window for players to view
 * the current state of combatants, without giving too much away.
 *
 * \sa CharacterListUI
 */

namespace UI
{


/** \brief
 *
 * Constructor. Sets up a modeless window and adds the HUD component to a
 * ScrolledWindow component.
 *
 */
HUDWindow::HUDWindow()
{
	set_type_hint(Gdk::WINDOW_TYPE_HINT_NORMAL);
	set_title(gettext("Player HUD Window"));
	set_resizable(true);
	set_decorated(true);
	set_position(Gtk::WIN_POS_CENTER);
	set_modal(false);
	set_icon( Gdk::Pixbuf::create_from_xpm_data( Helmet_xpm ) );

	PositionWindow();

    f_charListUI.AddHUDColumns();
    f_charListUI.set_border_width( 1 );
    f_charListUI.update();
	//
    f_scrolledWindow.add( f_charListUI );
	f_scrolledWindow.set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	f_scrolledWindow.set_shadow_type( Gtk::SHADOW_IN  );
	f_scrolledWindow.set_border_width( 1 );
	//
	f_mainBox.pack_start( f_scrolledWindow, Gtk::PACK_EXPAND_WIDGET );
	f_mainBox.pack_start( f_statusBox, Gtk::PACK_SHRINK );
	//
	add( f_mainBox );

	// Hook up signals so list may react correctly
	//
	auto charMgr(GetCharacterMgr().lock());
	assert(charMgr);
    charMgr->signal_character_added()  .connect( sigc::mem_fun( f_charListUI, &UI::CharacterListUI::insertCharacter   ) );
    charMgr->signal_character_removed().connect( sigc::mem_fun( f_charListUI, &UI::CharacterListUI::removeCharacter   ) );
    charMgr->signal_cleared()          .connect( sigc::mem_fun( f_charListUI, &UI::CharacterListUI::clear             ) );

	f_statusBox.Update();
	
	show_all_children();
}



/** \brief
 *
 * Destructor. Empty.
 *
 */
HUDWindow::~HUDWindow()
{
}


void HUDWindow::SetDefaultSort()
{
    f_charListUI.setDefaultSort();
}


/** \brief
 *
 * Position window to stored size
 */
void HUDWindow::PositionWindow()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	const int x = appSettings->HUDX();
	const int y = appSettings->HUDY();
	const int w = appSettings->HUDWidth();
	const int h = appSettings->HUDHeight();

	Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
	//
	const int screen_height = screen->get_height();
	const int screen_width  = screen->get_width();
	
	bool use_defaults = (x == -1);
	
	if( (x > screen_width)
	|| 	(y > screen_height)
	||	(x+w > screen_width)
	||	(y+h > screen_height) )
	{
		use_defaults = true;
	}

	if( use_defaults )
	{
		set_position( Gtk::WIN_POS_CENTER );
		set_default_size( 640, 480 );
	}
	else
	{
		set_default_size( appSettings->HUDWidth(), appSettings->HUDHeight() );
		move( appSettings->HUDX(), appSettings->HUDY() );
	}
}


/** \brief
 *
 * Called when the window is shown.
 *
 * \todo Add functionality as needed to handle CharacterListUI object showing.
 */
void HUDWindow::on_show()
{
	Gtk::Window::on_show();
	//
	PositionWindow();

	// Listen to this now
	//
	motk::ActionPtr changeHUDFont = motk::ActionPtr::cast_dynamic( GetActionsMgr().lock()->GetAction( "HUD::ChangeFont" ) );
	f_connectChangeFont = changeHUDFont->signal_activate().connect( sigc::mem_fun( *this, &HUDWindow::OnHUDChangeFont ) );
}


/** \brief
 *
 * Called when the window is hidden.
 *
 * \todo Add functionality as needed to handle CharacterListUI object hiding.
 */
void HUDWindow::on_hide()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	// Store window position
	//
	int x, y;
	Glib::RefPtr<Gdk::Window> window = get_window();
	window->get_root_origin( x, y );
	//
	appSettings->HUDX( x );
	appSettings->HUDY( y );

	// Store window dimensions
	//
	int width, height;
	get_size( width, height );
	//
    appSettings->HUDWidth( width );
	appSettings->HUDHeight( height );

	// Stop listening to this change now that we are hidden
	//
	f_connectChangeFont.disconnect();

	// Do the rest
	//
	Gtk::Window::on_hide();
}


void HUDWindow::OnHUDChangeFont()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	Gtk::FontSelectionDialog dlg( "Select Player HUD Font" );
	dlg.set_font_name( appSettings->AltCombatantListFont().c_str() );
	if( dlg.run() == Gtk::RESPONSE_OK )
	{
		appSettings->AltCombatantListFont( dlg.get_font_name().c_str() );
		appSettings->signal_changed().emit();
	}
}


bool HUDWindow::on_key_press_event( GdkEventKey* event )
{
	bool return_value = false;

	auto actionsMgr(GetActionsMgr().lock());
	assert(actionsMgr);

	switch( event->keyval )
	{
		case GDK_h:
			if( event->state & GDK_CONTROL_MASK )
			{
				motk::ToggleActionPtr showHUDAction = motk::ToggleActionPtr::cast_dynamic( actionsMgr->GetAction( "View::ShowHUD" ) );
				showHUDAction->set_active( false );
				return_value = true;
			}
			break;

		case GDK_F:
			if( event->state & GDK_CONTROL_MASK )
			{
				motk::ActionPtr changeHUDFont = motk::ActionPtr::cast_dynamic( actionsMgr->GetAction( "HUD::ChangeFont" ) );
				changeHUDFont->activate();
			}
			break;
	}

	// Call base class, to allow normal handling,
	// such as allowing the row to be selected by the right-click:
	//
	if( !return_value )
	{
		return_value = Gtk::Window::on_key_press_event(event);
	}

	return return_value;
}


bool HUDWindow::on_delete_event( GdkEventAny* event )
{
	motk::ToggleActionPtr showHUDAction( motk::ToggleActionPtr::cast_dynamic( GetActionsMgr().lock()->GetAction( "View::ShowHUD" ) ) );
	showHUDAction->set_active( false );
	return true;
}


}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

