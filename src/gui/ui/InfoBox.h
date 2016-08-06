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

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "base/common.h"
#include "base/character.h"
#include "base/ManagerBase.h"
#include "base/stat.h"
#include "ui/CharacterView.h"

namespace UI
{


class InfoBox
	: public Gtk::TextView
	, private Application::ManagerBase
{
public:
	InfoBox();
	~InfoBox();
	
	typedef sigc::signal1<void,Combatant::Character::pointer_t> CharSignal;
	CharSignal signal_edit() { return f_signalEdit; }

	// CharacterView
	//
	void OnReload();
	void OnSelectionChanged( Combatant::Character::pointer_t ch );
	void OnSelectionChanged( const Combatant::Character::list_t& selected_list );

private:
	CharSignal				f_signalEdit;
	Combatant::Character::pointer_t		f_char;
	Glib::RefPtr<Gtk::TextBuffer>		f_infoBuffer;
	sigc::connection			f_connection;
	//Attribute::StatManager::StatMap		f_stats;

	void		OnEditCharacter();

	void 		AddCR();
	void		AddComma();
	void 		AddAttribute( const Glib::ustring& name, const Glib::ustring& value );
	void 		AddAttribute( const Glib::ustring& name, const int value );
	void		AddModifier( const Glib::ustring& value );
	void		AddModifier( const int value );
	void		AddPercentage( const Glib::ustring& value );
	void		AddPercentage( const int value );
	void		AddNotesString();
	void		GenerateAbilityString();
	void		GenerateBuiltinString();
	void		GenerateStatString();
	void		UpdateDialog();

	// StatManager
	//
	void OnReloadTables();
};

}
// namespace UI


// vim: ts=8

