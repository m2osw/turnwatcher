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

// GTKMM
//
#include <gtkmm.h>

// MOLIB
//
#include "mo/mo_base.h"

#include <QString>

#define MENU_PREFIX	"MENU:"

namespace motk
{

typedef Glib::RefPtr<Gtk::ActionGroup>			ActionGroupPtr;
typedef Glib::RefPtr<Gtk::UIManager>			UIManagerPtr;
typedef Glib::RefPtr<Gtk::AccelGroup>			AccelGroupPtr;
typedef std::map<QString,QString>		String2String;
typedef Gtk::UIManager::ui_merge_id			MergeId;
typedef std::vector<MergeId>				MergeIdVector;

class UIBase
{
protected:
	UIBase();
	~UIBase();

	//UIManagerPtr	GetUIManager();
    void				ui( const QString& name, const QString& value );
    QString&			ui( const QString& name );
	String2String::iterator		begin_ui();
	String2String::iterator		end_ui();

	AccelGroupPtr 			GetAccelGroup();
    Gtk::Widget*			GetWidget( const QString& name );
	void				SetActionGroup( ActionGroupPtr val );
    MergeId				AddUI( const QString& ui );
	void				RemoveUI( MergeId id );
	void				Clear();

    static QString		MakePath( const QString& path, const QString& name );

	void					CreateUIManager();
	UIManagerPtr			GetUIManager() const { return f_refUIManager; }

private:
	ActionGroupPtr			f_actionGroup;
	UIManagerPtr			f_refUIManager;
	String2String			f_uiInfoMap;
	MergeIdVector			f_mergeIds;
};

}
// namespace motk

// $Log: uiBase.h,v $
// Revision 1.2  2006/03/10 05:40:43  doug
// Added signal for focus changing. Put classes in a namespace.
// Added ability for two widgets in a row. Added header ability.
//
// Revision 1.1  2006/02/17 17:36:56  doug
// Now builds and copies to the correct place.
//
// Revision 1.3  2005/12/23 08:24:38  alexis
// Fixed the copyright to Corp. and removed the locker/author
//
// Revision 1.2  2005/12/17 01:37:01  doug
// About box now displays version from molib::application.
// Import/Export dialogs now point to UserPath() from application, but
// now persist which folder the user selected to import from/export to.
//
// Revision 1.1  2005/09/11 18:13:02  doug
// Correctly working mockup-ui in GTKMM.
//
// Revision 1.2  2005/08/31 19:29:04  doug
// Menus now work correctly using the new GTK action/ui scheme.
//
// Revision 1.1  2005/08/27 18:24:05  doug
// Now using uiBase.
//
//
// vim: ts=8

