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



/** 
 * \brief Header for the C++ library interface to Linden Lab's source code
 *
 * Copyright (c) 2009-2013 by Made to Order Software Corporation
 * 
 * The source code in this file ("Source Code") is provided by Made to Order Software Corporation
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 */

#pragma once

#include <gtkmm.h>
#include <glibmm.h>
#include <webkit/webkitwebview.h>

namespace motk
{

/*! \class	HtmlWidget
 *  \brief	A gtkmm class wrapper for the webkit C interface.
 *
 *  This class wraps around the webkit straight 'C' interface providing
 *  C++ bindings. As the need for functionality increases, more methods
 *  will be added as the API for the webkit library is sizable.
 */
class HtmlWidget
{
public:
	HtmlWidget();
	virtual ~HtmlWidget();

	Gtk::Widget*	GetWidget();

	// C++ bindings
	//
	void			LoadUrl( const Glib::ustring& url );
	void			Refresh();
	void			StopLoad();
	bool			CanGoBack();
	bool			CanGoForward();
	void			GoBack();
	void			GoForward();
	Glib::ustring		GetLocation();
	Glib::ustring		GetTitle();
	Glib::ustring		GetStatusText();
	void			SetSettings( const WebKitWebSettings* settings );
	//void			SetDeviceType( WebKitWebDeviceType device );
	//WebKitWebDeviceType	GetDeviceType();

private:
	Glib::ustring	f_url;
	Gtk::Widget*	f_widget;
	WebKitWebView*	f_webview;
};


}
// namespace motk

// vim: ts=8 sw=8

