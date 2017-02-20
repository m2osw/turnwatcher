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

// STL
//
#include <map>

// MOTK
//
#include "motk/LabelTableScroller.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "character.h"
#include "CharacterColumns.h"
#include "ManagerBase.h"

namespace UI
{

class InitWindow
	: public Gtk::Dialog
	, private Application::ManagerBase
{
public:
	struct CharacterPair
	{
		Combatant::Character::pointer_t	f_origChar;
		Combatant::Character::pointer_t	f_newChar;
		CharacterPair( Combatant::Character::pointer_t orig ) : f_origChar(orig) {}
	};
	typedef std::vector<CharacterPair> pair_t;

	InitWindow( const Glib::ustring& title, const bool allowCancel );
	~InitWindow();

	void SetCharacters( pair_t& chars );

private:
	typedef std::vector<Combatant::Character::pointer_t>               character_list_t;
	typedef std::map<molib::mo_name_t,Combatant::Character::pointer_t> name_map_t;

	character_list_t         f_chars;
	character_list_t         f_monsters;
	motk::LabelTable         f_charTable;
	motk::LabelTable         f_monsterTable;
	motk::LabelTableScroller f_charTableScroller;
	motk::LabelTableScroller f_monsterTableScroller;
	Gtk::Button*             f_acceptButton;
	Gtk::Button*             f_cancelButton;
	Gtk::Button*             f_resetButton;
	bool                     f_allowCancel;
	name_map_t				 f_charNameMap;
	name_map_t				 f_monsterNameMap;
	
	void 			FillMainBox();
	void			InitTable();

	bool			OnScrollChange( Gtk::ScrollType type, double value, bool is_char );

	// Gtk::Dialog
	//
	virtual void	on_response( int response_id );

	// Gtk::Window
	//
	virtual bool	on_key_press_event( GdkEventKey* event );

	void			OnAcceptButton();
	void			OnResetButton();
};

}
// namespace UI


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

