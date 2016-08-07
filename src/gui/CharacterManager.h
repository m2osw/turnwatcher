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

#include "common.h"

// STL
#include <map>
#include <stack>
#include <memory>

// GTKMM
#include <gtkmm.h>

// molib
//
#include "mo/mo_string.h"

// Local includes
#include "character.h"

namespace Combatant
{

class CharacterManager
{
public:
	typedef std::shared_ptr<CharacterManager> Pointer;

	CharacterManager();
	~CharacterManager();

	// Public typedefs
	//
	typedef sigc::signal<void>						VoidSignal;
	typedef sigc::signal1<void,Character::Pointer>	CharSignal;

	// Public methods
	//
	const Character::List &	GetCharacters() const	{ return f_chars; }
	//
	void					Insert( Character::Pointer ch, const bool signal = true );
	void 					Remove( Character::Pointer ch, const bool signal = true );
	void					Clear( const bool signal = true );
	void					PermanentClear();
	//
	Character::Pointer 		FindDuplicates( Character::Pointer newChar );
	Character::Pointer		FindCharacter( const molib::moWCString& name );

	// Signals
	//
	VoidSignal				signal_changed()           { return f_changedSignal;          }
	VoidSignal				signal_cleared()           { return f_clearedSignal;          }
	CharSignal				signal_character_added()   { return f_characterAddedSignal;   }
	CharSignal				signal_character_removed() { return f_characterRemovedSignal; }

private:
	Character::List				f_chars;

	VoidSignal					f_changedSignal;
	VoidSignal					f_clearedSignal;
	CharSignal					f_characterAddedSignal;
	CharSignal					f_characterRemovedSignal;

	void 						Remove( Character::List::iterator iter, Combatant::Character::Pointer ch, const bool signal = true );
	Character::List::iterator	FindCharacterIter( const molib::moWCString& name );
};

}
// namespace UI

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

