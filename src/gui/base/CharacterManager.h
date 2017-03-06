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
#include <map>
#include <stack>
#include <memory>

// Local includes
//
#include "base/character.h"
#include "base/transaction.h"
#include "base/LegacyCharacter.h"
#include "mo/mo_props.h"

namespace Combatant
{

class CharacterManager
{
public:
    typedef std::weak_ptr<CharacterManager> pointer_t;

	static pointer_t Instance();
	static void Release();
	~CharacterManager();

	// Public typedefs
	//
	typedef Character::pointer_t				char_pointer_t;
	typedef Character::list_t					char_list_t;
	typedef sigc::signal<void>					void_signal_t;
	typedef sigc::signal1<void,char_pointer_t>	char_signal_t;

	// Public methods
	//
	bool Load();
	bool Save();
	//
	void LoadDemoCharacters();
	void LoadCharacters	( molib::moPropBagRef& charBag
						, Transactions::TransactionGroup::pointer_t group = Transactions::TransactionGroup::pointer_t()
						, const bool emit_signals = true
						);
	void SaveCharacters( molib::moPropBagRef& charBag );

	const char_list_t &		GetCharacters() const	{ return f_chars; }
	//
	void					Insert( char_pointer_t ch, const bool signal = true );
	void 					Remove( char_pointer_t ch, const bool signal = true );
	void					Clear( const bool signal = true );
	void					PermanentClear();
	//
	char_pointer_t 			FindDuplicates( char_pointer_t newChar );
	char_pointer_t			FindCharacter( const QString& name );

	// Signals
	//
	void_signal_t			signal_changed()           { return f_changedSignal;          }
	void_signal_t			signal_cleared()           { return f_clearedSignal;          }
	char_signal_t			signal_character_added()   { return f_characterAddedSignal;   }
	char_signal_t			signal_character_removed() { return f_characterRemovedSignal; }

	// Legacy
	//
	void SetLegacyCharacters( const LegacyCharacter::list_t& lchars, Transactions::TransactionGroup::pointer_t group, const bool emit_signals );

private:
	typedef std::shared_ptr<CharacterManager> private_pointer_t;
	static private_pointer_t f_instance;

	molib::moName			f_propBagName;
	molib::moName			f_arrayBagName;
	char_list_t				f_chars;
	void_signal_t			f_changedSignal;
	void_signal_t			f_clearedSignal;
	char_signal_t			f_characterAddedSignal;
	char_signal_t			f_characterRemovedSignal;

	// Singleton only
	//
	CharacterManager();
    CharacterManager( const CharacterManager& );
    CharacterManager& operator =( const CharacterManager& );

	void 					Remove( char_list_t::iterator iter, char_pointer_t ch, const bool signal = true );
	char_list_t::iterator	FindCharacterIter( const QString& name );
};

}
// namespace UI

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

