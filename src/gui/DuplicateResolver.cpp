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




#include "DuplicateResolver.h"
#include "SingletonManager.h"

namespace Initiative
{


DuplicateRoll::DuplicateResolver::DuplicateResolver()
{
}


DuplicateRoll::DuplicateResolver::~DuplicateResolver()
{
}


void DuplicateRoll::DuplicateResolver::Clear()
{
	f_rollMap.clear();
}


void DuplicateRoll::DuplicateResolver::AddRoll( const int roll, Combatant::Character::Pointer ch )
{
#if defined(DEBUG) && defined(TRACE)
	printf( "DuplicateRoll::DuplicateResolver::AddRoll(): Adding character %s\n", ch->name().c_str() );
#endif
	const int initId	= Application::Manager::Instance().lock()->GetStatMgr().lock()->initId();
	const int modified_roll	= (roll << 16) + (ch->getMod( initId ) & 0xFFFF);
	//
	if( f_rollMap.find(modified_roll) == f_rollMap.end() )
	{
		f_rollMap[modified_roll].reset( new DuplicateRoll );
	}

	DuplicateRoll::Pointer dup_roll = f_rollMap[modified_roll];
	//
	if( dup_roll->FindCharacter( ch ) )
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "DuplicateRoll::DuplicateResolver::AddRoll(): Character %s found! Removing...\n",
				ch->name().c_str() );
#endif
		dup_roll->RemoveCharacter( ch );
	}
	//
	dup_roll->AddCharacter( ch );
}


bool DuplicateRoll::DuplicateResolver::FindCharacter( Combatant::Character::Pointer ch )
{
	bool char_found = false;
	//
	for( auto pair : f_rollMap )
	{
		if( pair.second->FindCharacter( ch ) )
		{
			char_found = true;
			break;
		}
	}

	return char_found;
}


void DuplicateRoll::DuplicateResolver::RemoveCharacter( Combatant::Character::Pointer ch )
{
    std::vector<RollToDupMap::iterator> to_delete;
	//
    const auto end = f_rollMap.end();
    for( auto iter = f_rollMap.begin(); iter != end; ++iter )
	{
        DuplicateRoll::Pointer dupRoll( iter->second );
		assert(dupRoll);
		dupRoll->RemoveCharacter( ch );
		if( dupRoll->IsEmpty() )
		{
            to_delete.push_back( iter );
		}
	}
	//
    for( auto iter : to_delete )
	{
        f_rollMap.erase( iter );
	}
}


bool DuplicateRoll::DuplicateResolver::IsEmpty() const
{
	return f_rollMap.empty();
}


void DuplicateRoll::DuplicateResolver::Copy( Pointer resolver )
{
	assert(resolver);
	f_rollMap.clear();
	for( auto rollDupPair : resolver->f_rollMap )
	{
		DuplicateRoll::Pointer dup_roll( new DuplicateRoll );
		dup_roll->Copy( rollDupPair.second );
		f_rollMap[rollDupPair.first] = dup_roll;
	}
}


void DuplicateRoll::DuplicateResolver::GetOrderedVector( Combatant::Character::List& charList )
{
#if defined(DEBUG) && defined(TRACE)
	printf( "DuplicateRoll::DuplicateResolver::GetOrderedVector(): getting list:\n" );
#endif
	charList.clear();
	//
	for( auto rollDupPair : f_rollMap )
	{
		DuplicateRoll::Pointer				dup_roll = rollDupPair.second;
		Combatant::Character::List			sub_vect;
								dup_roll->GetOrderedVector( sub_vect );
		//Combatant::Character::List::iterator		sub_iter = sub_vect.begin();
		//const Combatant::Character::List::iterator	sub_end  = sub_vect.end();
		for( auto ch : sub_vect )
		//for( ; sub_iter != sub_end; ++sub_iter )
		{
			//Combatant::Character::Pointer ch = *sub_iter;
			if( ch )
			{
#if defined(DEBUG) && defined(TRACE)
				printf( "\tFound character in list %s\n", ch->name().c_str() );
#endif
				charList.push_back( ch );
			}
		}
	}
}


void DuplicateRoll::DuplicateResolver::Load( molib::moPropBagRef& mainPropBag )
{
	// Clear out everything
	//
	f_rollMap.clear();

	// Read sub-bag for the array
	//
	molib::moPropBagRef	subBag( "RESOLVER_BAG" );
	subBag.Link( mainPropBag );
	if( !subBag.HasProp() ) subBag.NewProp();

	// Read the resolver array
	//
	molib::moPropArrayRef	array("INIT_RESOLVER");
	array.Link( subBag );
	if( !array.HasProp() )	array.NewProp();
	//
	const int count = array.CountIndexes();
	//
	for( int idx = 0; idx < count; ++idx )
	{
		const int 		item_no  ( array.ItemNoAtIndex( idx ) );
		molib::moPropSPtr	prop_ptr ( array.Get( item_no ) );
		molib::moName		name	 ( prop_ptr->GetName() );
		molib::moPropBagRef	propBag(name);
		propBag.NewProp();
		propBag.GetProperty()->Copy( *prop_ptr );
		//
		molib::moPropIntRef rollProp( "SORT_ROLL" );
		rollProp.Link( propBag );
		const int		sortRoll = rollProp;
		//
		DuplicateRoll::Pointer	dupRoll( new DuplicateRoll );
		dupRoll->Load( propBag );
		//
		f_rollMap[sortRoll] = dupRoll;
	}
}


void DuplicateRoll::DuplicateResolver::Save( molib::moPropBagRef& mainPropBag )
{
	// Create array for resolver
	//
	molib::moPropArrayRef	array("INIT_RESOLVER");
	array.NewProp();

	// Populate the array with the sort value and DuplicateRoll
	//
	int array_index = 0;
	for( auto rollDupPair : f_rollMap )
	{
		const int		sortRoll = rollDupPair.first;
		DuplicateRoll::Pointer	dupRoll  = rollDupPair.second;
		//
		molib::moPropIntRef rollProp( "SORT_ROLL" );
		rollProp.NewProp();
		rollProp = sortRoll;
		//
		molib::moPropBagRef propBag( "DUP_ROLL" );
		propBag.NewProp();
		propBag += rollProp;
		dupRoll->Save( propBag );
		//
		array.Set( array_index, propBag );
		++array_index;
	}

	// Save array to resolver bag
	//
	molib::moPropBagRef	subBag( "RESOLVER_BAG" );
	subBag.NewProp();
	subBag += array;

	// Save the resolver bag into the main property bag
	//
	mainPropBag.Set( "RESOLVER_BAG", subBag );
}


}
// namespace Initiative
//
// vim: ts=8 sw=8

