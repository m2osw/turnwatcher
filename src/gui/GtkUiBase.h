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

// GTKMM
//
#include <gtkmm.h>

#define MENU_PREFIX	"MENU:"

namespace UI
{

typedef Glib::RefPtr<Gtk::ActionGroup>        ActionGroupPtr;
typedef Glib::RefPtr<Gtk::UIManager>          UIManagerPtr;
typedef Glib::RefPtr<Gtk::AccelGroup>         AccelGroupPtr;
typedef std::map<Glib::ustring,Glib::ustring> String2String;
typedef Gtk::UIManager::ui_merge_id           MergeId;
typedef std::vector<MergeId>                  MergeIdVector;

class GtkUiBase
{
protected:
	GtkUiBase();
	~GtkUiBase();

	void						ui( const Glib::ustring& name, const Glib::ustring& value );
	Glib::ustring&				ui( const Glib::ustring& name );
	String2String::iterator		begin_ui();
	String2String::iterator		end_ui();

	AccelGroupPtr 				GetAccelGroup();
	Gtk::Widget*				GetWidget( const Glib::ustring& name );
	void						SetActionGroup( ActionGroupPtr val );
	MergeId						AddUI( const Glib::ustring& ui );
	void						RemoveUI( MergeId id );
	void						Clear();

	static Glib::ustring		MakePath( const Glib::ustring& path, const Glib::ustring& name );

	void						CreateUIManager();
	UIManagerPtr				GetUIManager() const { return f_refUIManager; }

private:
	ActionGroupPtr				f_actionGroup;
	UIManagerPtr				f_refUIManager;
	String2String				f_uiInfoMap;
	MergeIdVector				f_mergeIds;
};

}
// namespace UI

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

