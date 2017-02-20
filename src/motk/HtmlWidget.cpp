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




#include "HtmlWidget.h"

namespace motk
{

/*!	\brief	HtmlWidget default constructor
 *
 * Constructs a instance of the HtmlWidget class. Initializes the f_webview and f_widget members
 * by creating an f_webview instance and wrapping it into a gtkmm widget. It is also added
 * to the gtk memory manager, which will handle its destruction
 */
HtmlWidget::HtmlWidget() : f_webview(0) 
{
	GtkWidget* widget = webkit_web_view_new();
	f_webview	  = WEBKIT_WEB_VIEW(widget);
	f_widget 	  = Gtk::manage( Glib::wrap( widget ) );
}


/*!	\brief	HtmlWidget destructor
 *
 * The f_widget and f_webview members are destroyed when the gtkmm Main object is destroyed.
 */
HtmlWidget::~HtmlWidget()
{
	// f_webview is destroyed by gtkmm
}


/*!	\brief	Returns the Gtk::Widget pointer
 *
 * This returns the pointer to a fully constructed Gtk::Widget object. This is created by
 * wrapping it into gtkmm.
 *
 * \return The Gtk widget used by this HtmlWidget.
 */
Gtk::Widget* HtmlWidget::GetWidget()
{
	return f_widget;
} 


/*! \brief	Load an URL into the web widget
 *
 * This loads a URL into the webi widget.
 *
 * \param[in] url  URL to load in the Web Widget
 */
void HtmlWidget::LoadUrl( const Glib::ustring& url )
{
	webkit_web_view_load_uri( f_webview, url.c_str() );
}


/*! \brief	Refresh current page
 *
 * Causes the current url to reload.
 */
void HtmlWidget::Refresh()
{
	webkit_web_view_reload( f_webview );
}


/*! \brief	Stop the current load
 *
 * Cancels the load currently in progress, if any.
 */
void HtmlWidget::StopLoad()
{
	webkit_web_view_stop_loading( f_webview );
}


/*! \brief	Check to go back in history
 *
 * Returns a boolean status indicating whether the browser has history.
 *
 * \return true if it is possible to go back.
 *
 * \sa GoForward
 * \sa GoBack
 * \sa CanGoForward
 */
bool HtmlWidget::CanGoBack()
{
	return (bool) webkit_web_view_can_go_back( f_webview );
}


/*! \brief	Check to go forward in history
 *
 * Returns a boolean status true indicating there is forward history.
 *
 * \return true if it is possible to go forward.
 *
 * \sa GoForward
 * \sa GoBack
 * \sa CanGoBack
 */
bool HtmlWidget::CanGoForward()
{
	return (bool) webkit_web_view_can_go_forward( f_webview );
}


/*! \brief	Go back in history
 *
 * Go back one page in history.
 *
 * \sa CanGoBack
 * \sa GoForward
 * \sa CanGoForward
 */
void HtmlWidget::GoBack()
{
	webkit_web_view_go_back( f_webview );
}


/*! \brief	Go forward in history
 *
 * Go forward one page in history.
 *
 * \sa CanGoBack
 * \sa GoBack
 * \sa CanGoForward
 */
void HtmlWidget::GoForward()
{
	webkit_web_view_go_forward( f_webview );
}


/*! \brief	Get current browser location
 *
 * \return The current URL the browser is pointing to.
 */
Glib::ustring HtmlWidget::GetLocation()
{
	return Glib::ustring( webkit_web_view_get_uri( f_webview ) );
}


/*! \brief	Get the page title
 *
 * \return A string with the title of the current page.
 */
Glib::ustring HtmlWidget::GetTitle()
{
	return Glib::ustring( webkit_web_view_get_title( f_webview ) );
}


/*! \brief	Get status bar text
 *
 * \return Text that can be displayed in the status bar.
 */
Glib::ustring HtmlWidget::GetStatusText()
{
#if 0
	return Glib::ustring( webkit_web_view_get_status_text( f_webview ) );
#else
	// Not yet implemented by the webi API!
	//
	return Glib::ustring("");
#endif
}


/*! \brief	Change the browser settings
 *
 * Alter the browser settings.
 *
 * \param[in] settings  The new settings.
 */
void HtmlWidget::SetSettings( const WebKitWebSettings* settings )
{
	webkit_web_view_set_settings( f_webview, const_cast<WebKitWebSettings*>(settings) );
}


#if 0
/*! \brief	Change the device type
 *
 * Change the type of device currently being written to (for example,
 * screen, printer, etc.).
 *
 * \param[in] device   The new device type
 */
void HtmlWidget::SetDeviceType( WebKitWebDeviceType device )
{
	webkit_web_view_set_device_type( f_webview, device );
}


/*! \brief	Return the device type
 *
 * \return the current device.
 */
WebKitWebDeviceType HtmlWidget::GetDeviceType()
{
	return webkit_web_view_get_device_type( f_webview );
}
#endif

}
// namespace motk

// vim: ts=8 sw=8

