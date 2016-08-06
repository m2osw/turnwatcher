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

// STL
//
#include <map>

// MOLIB
//
#include "mo/mo_base.h"

// MOTK
//
#include "motk/LabelTable.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "base/common.h"
#include "base/ManagerBase.h"
#include "ui/EffectsBook.h"
#include "ui/InfoBox.h"
#include "ui/CharacterView.h"

namespace UI
{

#define MAX_PANES 2

class InfoWindow :
	public Gtk::Dialog,
	private Application::ManagerBase
{
public:
	InfoWindow( Glib::ustring title, Gtk::Window& parent );
	~InfoWindow();

	typedef sigc::signal1<void,Combatant::Character::pointer_t> CharSignal;
	CharSignal	signal_edit() { return f_signalEdit; }

	void	OnReload();
	void	OnSelectionChanged( const Combatant::Character::list_t& selected_list );
	void	OnEditCharacter( Combatant::Character::pointer_t _char );
	void	OnClear();

private:
	CharSignal		f_signalEdit;
	Gtk::VPaned		f_pane;
	Gtk::ScrolledWindow	f_scroller[MAX_PANES];
	EffectsBook		f_effectsBook[MAX_PANES];
	InfoBox			f_infoBox[MAX_PANES];
	motk::ToggleActionPtr	f_showInfoAction;
	sigc::connection	f_connection;

	Gtk::ScrolledWindow*	CreateScroller( const int id );
	void			FillMainBox();

	virtual	void		on_show();
	virtual void		on_hide();
	virtual bool		on_key_press_event( GdkEventKey* event );
	virtual bool		on_delete_event(GdkEventAny* event);
};

}
// namespace UI


// vim: ts=8 sw=8

