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




#include "CharacterManager.h"
#include "AppSettings.h"

namespace Combatant
{


CharacterManager::CharacterManager()
{
}


CharacterManager::~CharacterManager()
{
}


void CharacterManager::Insert( Combatant::Character::Pointer ch, const bool signal )
{
	ch->deleted( false );
	//
	if( FindDuplicates( ch ) == 0 )
	{
		f_chars.push_back( ch );
		//
		if( signal )
		{
			f_changedSignal.emit();
			f_characterAddedSignal.emit( ch );
		}
	}
}


void CharacterManager::Remove( Combatant::Character::Pointer ch, const bool signal )
{
	ch->deleted( true );
	//
	const auto found_iter = FindCharacterIter( ch->name() );
	//
	if( found_iter != f_chars.end() )
	{
		Remove( found_iter, ch, signal );
	}
}


void CharacterManager::Remove( Character::List::iterator iter, Combatant::Character::Pointer ch, const bool signal )
{
	f_chars.erase( iter );
	//
	if( signal )
	{
		f_changedSignal.emit();
		f_characterRemovedSignal.emit( ch );
	}
}


void CharacterManager::Clear( const bool signal )
{
	if( signal )
	{
		for( auto ch : f_chars )
		{
			if( ch )
			{
				// Emit signal to show that the character has been removed from the list
				//
				f_characterRemovedSignal.emit( ch );
			}
		}
	}

	// Alexis, you're right; that loop was waaaay to complicated. The point was to
	// emit signals for each character removed from the manager so other UI objects
	// could react properly. :-)
	//
	PermanentClear();

	if( signal )
	{
		f_changedSignal.emit();
		f_clearedSignal.emit();
	}
}


void CharacterManager::PermanentClear()
{
	f_chars.clear();
}


Combatant::Character::Pointer CharacterManager::FindDuplicates( Combatant::Character::Pointer newChar )
{
	Combatant::Character::Pointer dupChar;
	//
	for( auto ch : f_chars )
	{
		if( ch->name() == newChar->name() )
		{
			dupChar = ch;
			break;
		}
	}

	return dupChar;
}


Combatant::Character::Pointer CharacterManager::FindCharacter( const molib::moWCString& name )
{
	Combatant::Character::Pointer found_char;
	const auto found_iter = FindCharacterIter( name );
	if( f_chars.end() != found_iter )
	{
		found_char = *found_iter;
	}

	return found_char;
}


Combatant::Character::List::iterator CharacterManager::FindCharacterIter( const molib::moWCString& name )
{
	auto		iter       = f_chars.begin();
	const auto	end        = f_chars.end();
	auto		found_iter = end;
	for( ; iter != end; ++iter )
	{
		auto ch( *iter );
		if( ch->name() == name )
		{
			found_iter = iter;
			break;
		}
	}

	return found_iter;
}


}
// namespace UI


// vim: ts=4 sw=4 noet syntax=cpp.doxygen

