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

#include "base/DuplicateRoll.h"
#include "base/DuplicateResolver.h"
#include "base/CharacterManager.h"
#include "base/StatManager.h"

using namespace molib;
using namespace Attribute;
using namespace Combatant;

namespace Initiative
{

DuplicateRoll::DuplicateRoll()
{
}

DuplicateRoll::~DuplicateRoll()
{
}

void DuplicateRoll::AddCharacter ( Character::pointer_t ch )
{
#if defined(DEBUG) && defined(TRACE)
	printf( "DuplicateRoll::AddCharacter(): Adding character %s: ", ch->name().c_str() );
#endif
	auto statMgr( StatManager::Instance().lock() );
	assert(statMgr);
	const mo_name_t initId( statMgr->initId() );

	if( !f_char && !f_dupResolver )
	{
		// If list is empty, we only want to add the character to the vector. Order
		// does not matter (duh!). This really is for place holding for the next
		// case below.
		//
#if defined(DEBUG) && defined(TRACE)
		printf( "[Resolver empty, adding sole character]\n" );
#endif
		f_char = ch;
	}
	else if( f_char && !f_dupResolver )
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "[One character entry, rolling tie breakers, creating new resolver, and adding both chars]\n" );
#endif
		// If one in the list, extract it and roll a tie breaker for it
		// and the new character. Resolve it, then add them both back to
		// the ordered vector. The resolver determines their order.
		//
		const int first_roll	= f_char->makeRoll( initId );
		const int second_roll	= ch    ->makeRoll( initId );
		f_dupResolver.reset( new DuplicateResolver );
		f_dupResolver->AddRoll( first_roll , f_char );
		f_dupResolver->AddRoll( second_roll, ch     );
	}
	else if( f_dupResolver )
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "[More than two entries in resolver; making tie-break roll and adding]\n" );
#endif
		// If more than 1, then roll the new tie breaker only, then add
		// the new roll/char to the resolver and reorder the vector.
		//
		const int tie_roll = ch->makeRoll( initId );
		f_dupResolver->AddRoll( tie_roll, ch );
	}
#if defined(DEBUG) && defined(TRACE)
	else
	{
		printf( "[ERROR! Nothing added!!!!]\n" );
	}
#endif
}


bool DuplicateRoll::FindCharacter( Character::pointer_t ch )
{
	bool char_found = false;

	if( f_char && !f_dupResolver )
	{
		const Glib::ustring fname( f_char->name() );
		const Glib::ustring cname( ch->name()     );
		char_found = (fname == cname);
#if defined(DEBUG) && defined(TRACE)
		if( char_found )
		{
			printf( "DuplicateRoll::FindCharacter(): Searching for character %s: ", cname.c_str() );
			printf( "[Match FOUND: %s]\n", fname.c_str() );
		}
#endif
	}
	else if( f_dupResolver )
	{
		char_found = f_dupResolver->FindCharacter( ch );
#if defined(DEBUG) && defined(TRACE)
		if( char_found )
		{
			printf( "DuplicateRoll::FindCharacter(): Searching for character %s: ", ch->name().c_str() );
			printf( "[Match FOUND: %s]\n", ch->name().c_str() );
		}
#endif
	}

	return char_found;
}


void DuplicateRoll::RemoveCharacter( Character::pointer_t ch )
{
	if( f_char && !f_dupResolver )
	{
		if( f_char->name() == ch->name() )
		{
#if defined(DEBUG) && defined(TRACE)
			printf( "DuplicateRoll::RemoveCharacter(): Searching for character %s: ", ch->name().c_str() );
			printf( "[Character found, and deleted]\n" );
#endif
			f_char = 0;
		}
	}
	else if( f_dupResolver )
	{
		f_dupResolver->RemoveCharacter( ch );

		if( f_dupResolver->IsEmpty() )
		{
			f_dupResolver = 0;
			f_char = 0;
#if defined(DEBUG) && defined(TRACE)
			printf( "DuplicateRoll::RemoveCharacter(): Searching for character %s: ", ch->name().c_str() );
			printf( "[Resolver is now empty, clearing out]\n" );
#endif
		}
	}
	else
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "DuplicateRoll::RemoveCharacter(): Searching for character %s: ", ch->name().c_str() );
		printf( "ERROR! Nothing in object!!!!!\n" );
#endif
	}
}


bool DuplicateRoll::IsEmpty() const
{
	bool is_empty = true;

	if( f_char )
	{
		is_empty = false;
	}

	if( f_dupResolver )
	{
		is_empty = false;
	}

	return is_empty;
}


void DuplicateRoll::GetOrderedVector( Character::list_t& charList )
{
	charList.clear();

	if( f_char && !f_dupResolver )
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "DuplicateRoll::GetOrderedVector(): getting sole character %s\n", f_char->name().c_str() );
#endif
		charList.push_back( f_char );
	}
	else if( f_dupResolver )
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "DuplicateRoll::GetOrderedVector(): getting resolver list\n" );
#endif
		f_dupResolver->GetOrderedVector( charList );
	}
#if defined(DEBUG) && defined(TRACE)
	else
	{
		printf( "DuplicateRoll::GetOrderedVector(): ERROR! No object created!\n" );
	}
#endif
}


void DuplicateRoll::Copy( DuplicateRoll::pointer_t dup_roll )
{
	f_char			= dup_roll->f_char;
	f_dupResolver	= NULL;

	if( dup_roll->f_dupResolver )
	{
		f_dupResolver.reset( new DuplicateResolver );
		f_dupResolver->Copy( dup_roll->f_dupResolver );
	}
}


void DuplicateRoll::Load( moPropBagRef& propBag )
{
	moPropStringRef	singleChar ( "CHARACTER" );
	singleChar.Link( propBag );
	if ( singleChar.HasProp() )
	{
		Glib::ustring			name( static_cast<moWCString>(singleChar).c_str() );
		Character::pointer_t	ch  ( CharacterManager::Instance().lock()->FindCharacter( name ) );
#if defined(DEBUG) && defined(TRACE)
		printf( "DuplicateRoll::Load() - searching for character %s, ", name.c_str() );
		if( ch )
		{
			printf( "found: %p\n", ch.get() );
		}
		else
		{
			printf( "not found--must have been deleted in last session!\n" );
		}
#endif
		if( ch )
		{
			f_char = ch;
		}
	}
	else
	{
		f_dupResolver.reset( new DuplicateResolver );
		f_dupResolver->Load( propBag );
	}
}


void DuplicateRoll::Save( moPropBagRef& propBag )
{
	if( f_char && !f_dupResolver )
	{
		moPropStringRef	singleChar ( "CHARACTER" );
		singleChar.NewProp();
		singleChar = f_char->name().c_str();
		propBag += singleChar;
	}
	else if( f_dupResolver )
	{
		f_dupResolver->Save( propBag );
	}
}


}
// namespace Initiative
//
// vim: ts=4 sw=4

