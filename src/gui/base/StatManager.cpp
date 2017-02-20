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

// LOCAL
//
#include "base/StatManager.h"
#include "base/AppSettings.h"

#include <algorithm>

#include "mo/mo_application.h"
#include "mo/mo_props_xml.h"

using namespace molib;

namespace Attribute
{


StatManager::private_pointer_t StatManager::f_instance;


/// \brief Constructor
//
// Creates the names used to store values, plus initializes the stat list
// with default values.
//
/// \sa CreateDefaultStats
//
StatManager::StatManager()
	: f_initId      ("INIT_ID")
	, f_strId       ("STR_ID")
	, f_dexId       ("DEX_ID")
	, f_conId       ("CON_ID")
	, f_intId       ("INT_ID")
	, f_wisId       ("WIS_ID")
	, f_chaId       ("CHA_ID")
	, f_spotId      ("SPOT_ID")
	, f_listenId    ("LISTEN_ID")
	, f_willId      ("WILL_ID")
	, f_levelId     ("LEVEL_ID")
	, f_hpId        ("HITPOINTS_ID")
	, f_arrayName   ("STATS")
	, f_propBagName ("STATS")
{
	if( !Load() )
	{
		CreateDefaultStats();
	}
}


StatManager::~StatManager()
{
}


StatManager::pointer_t StatManager::Instance()
{
	if( !f_instance )
	{
		f_instance.reset( new StatManager );
	}
    return f_instance;
}


void StatManager::Release()
{
	f_instance.reset();
}


/// \brief Create default stat objects
//
// This method initializes the default number of stats we look upon as default stats.
// This is used to create "soft" columns and soon to handle base ability scores. Stats
// can be based on ability scores, their modifiers reflecting boosts (or penalties) from
// said ability scores.
//
void StatManager::CreateDefaultStats()
{
	f_statMap.clear();
    AddDefaultStat( gettext("Str"),        f_strId   , -1, false, true  );
    AddDefaultStat( gettext("Dex"),        f_dexId   , -1, false, true  );
    AddDefaultStat( gettext("Con"),        f_conId   , -1, false, true  );
    AddDefaultStat( gettext("Int"),        f_intId   , -1, false, true  );
    AddDefaultStat( gettext("Wis"),        f_wisId   , -1, false, true  );
    AddDefaultStat( gettext("Cha"),        f_chaId   , -1, false, true  );
    AddDefaultStat( gettext("Init"),       f_initId  , -1, false, true, false, NULL/*, f_dexId*/ );
    AddDefaultStat( gettext("Hit Points"), f_hpId    , -1, false, true, false, NULL/*, f_conId*/ );
    AddDefaultStat( gettext("Level"),      f_levelId , -1, false, true, false  );
	//
	AddDefaultColumns();
}


void StatManager::AddDefaultColumns()
{
#ifdef DARWIN
	const char*	spot_accel		= "<control> s";
	const char*	listen_accel	= "<control> l";
	const char*	will_accel		= "<control> w";
#else
	const char*	spot_accel		= "s";
	const char*	listen_accel	= "l";
	const char*	will_accel		= "w";
#endif

    AddDefaultStat( gettext("Spot"),   f_spotId   , 1, true, false, false, spot_accel   /*, f_wisId*/ );
    AddDefaultStat( gettext("Listen"), f_listenId , 2, true, false, false, listen_accel /*, f_wisId*/ );
    AddDefaultStat( gettext("Will"),   f_willId   , 3, true, false, false, will_accel   /*, f_wisId*/ );
}


/// \brief Add default stat to list
//
// This is a helper method used to create a default stat (for example, Spot and Listen).
// If the id already exists, then the method returns silently. This is so you can add stats
// that aren't already in the list while maintaining current stats. Helpful for legacy conversion.
//
void StatManager::AddDefaultStat(	const Glib::ustring& name, 
									const mo_name_t id,
									const int order,
									const bool on_UI,
									const bool internal_stat,
									const bool ability_stat,
	   								const char* accel,
									const mo_name_t ability_id )
{
	if( f_statMap.find(id) != f_statMap.end() )
	{
		// Already exists, no need to create
		//
		return;
	}

	Stat::pointer_t stat( new Stat );
    stat->name             ( name                  );
	stat->id               ( id                    );
	stat->order            ( order                 );
	stat->abilityId        ( ability_id            );
	stat->showOnToolbar    ( on_UI                 );
	stat->showOnHUD        ( false                 );
	stat->showMonsterOnHUD ( false                 );
	stat->internal         ( internal_stat         );
	stat->ability          ( ability_stat          );
    if( accel )
    {
        stat->accel( accel );
    }
	//
	stat->SignalChanged().connect( sigc::mem_fun( *this, &StatManager::OnStatChanged ) );
	//
	f_statMap[id] = stat;
}


/// \brief Add a new stat to the list
//
// Useful for when the user defines a new soft column. Other possibilities exist... :-)
//
/// \sa Stat
//
void StatManager::AddStat( const Glib::ustring& name, const mo_name_t id )
{
	Stat::pointer_t stat( new Stat );
	stat->name             ( name  );
	stat->id               ( id    );
	stat->showOnToolbar    ( false );
	stat->showOnHUD        ( false );
	stat->showMonsterOnHUD ( false );
	//
	stat->SignalChanged().connect( sigc::mem_fun( *this, &StatManager::OnStatChanged ) );
	f_statMap[stat->id()] = stat;
	//
    f_statsChanged.emit();
}


/// \brief Add an existing stat to the list
//
// Add a user-created stat to the internal list. The id() property must be defined correctly!
//
/// \sa Stat
//
void StatManager::AddStat( Stat::pointer_t stat )
{
	assert( stat );

	Stat::pointer_t new_stat( new Stat );
	new_stat->Copy( stat );
	new_stat->SignalChanged().connect( sigc::mem_fun( *this, &StatManager::OnStatChanged ) );
	f_statMap[new_stat->id()] = new_stat;
	//
	f_statsChanged.emit();
}


/// \brief Get a stat based on id (a moName)
/// \note No longer creates a stat if non-existent, so if you want a stat, you must explicitly create one using AddStat().
//
Stat::pointer_t StatManager::GetStat( const mo_name_t id )
{
	auto iter( f_statMap.find( id ) );
    if( iter != f_statMap.end() )
    {
        return iter->second;
    }

    return Stat::pointer_t();
}


void StatManager::DeleteStat( const mo_name_t id )
{
	Stat::pointer_t stat = f_statMap[id];
	if( stat )
	{
		f_statsChanged.emit();
		f_statMap.erase( id );
    }
}

Stat::pointer_t StatManager::Strength()
{
	return f_statMap[f_strId];
}

Stat::pointer_t StatManager::Dexterity()
{
	return f_statMap[f_dexId];
}

Stat::pointer_t StatManager::Constitution()
{
	return f_statMap[f_conId];
}

Stat::pointer_t StatManager::Intelligence()
{
	return f_statMap[f_intId];
}

Stat::pointer_t StatManager::Wisdom()
{
	return f_statMap[f_wisId];
}

Stat::pointer_t StatManager::Charisma()
{
	return f_statMap[f_chaId];
}

void StatManager::OnStatChanged()
{
	//f_statsChanged.emit();
}


const StatManager::StatMap&	StatManager::GetStats()
{
	return f_statMap;
}


/// \brief Get stat object based on id/type pair
//
// This is useful for converting legacy stats to current stat scheme.
// We search based on id/type pair for hit in stat map.
//
/// \param id	integer-based id to search for
/// \param type	integer-based id to search for (formerly an enumeration of Skill/Save/Misc/Ability).
//
/// \return Smart pointer to object or NULL
//
Stat::pointer_t StatManager::GetLegacyStat( const int id, const int type )
{
	Stat::pointer_t	ret;
	//
	for( auto statPair : f_statMap )
	{
		auto stat = statPair.second;
		if( (stat->legacy_id() == id) && (stat->legacy_type() == type) )
		{
			ret = stat;
			break;
		}
	}

	return ret;
}


bool StatManager::LoadStats( moPropBagRef& propBag )
{
	f_statMap.clear();

	moPropArrayRef	array( f_arrayName );
	array.Link( propBag );
	if( !array.HasProp() ) return false;

	unsigned long idx = 0;
	const unsigned long end = array.CountIndexes();
	//
	for( ; idx < end; ++idx )
	{
		const int 		item_no  ( array.ItemNoAtIndex( idx ) );
		moPropSPtr		prop_ptr ( array.Get( item_no ) );
		moName			name	 ( prop_ptr->GetName() );
		moPropBagRef	prop     ( name );

		prop.NewProp();
		prop.GetProperty()->Copy( *prop_ptr );
		
		try
		{
			Stat::pointer_t stat( new Stat );
			stat->Load( prop );
	
			const mo_name_t id = stat->id();
			//
			f_statMap[id] = stat;
		}
		catch( const moError& x )
		{
			// Forget this stat--we cannot find it!
			//
			std::cerr << "StatManager::Load(): Caught moError: " << x.Message() << std::endl;
			return false;
		}
	}

	return true;
}


/// \brief Load stats from a property bag
//
bool StatManager::Load()
{
	moPropBagRef propBag( f_propBagName );
	if( !Common::LoadBagFromFile( "stats.conf", propBag ) ) return false;

	const bool success = LoadStats( propBag );
	if( success ) f_statsChanged.emit();

	return success;
}


/// \brief Save stats to a property bag
//
bool StatManager::Save()
{
	moPropArrayRef	array( f_arrayName );
	array.NewProp();

	int array_idx = 0;
	for( auto statPair : f_statMap )
	{
		moPropBagRef	bag("STAT");
		bag.NewProp();
		auto stat( statPair.second );
		if( stat && !stat->deleted() )
		{
			// Only save if not deleted
			//
			stat->Save( bag );
			array.Set( array_idx, bag );
		}
		++array_idx;
	}

	moPropBagRef    propBag( f_propBagName );
	propBag.NewProp();
	propBag += array;

	return Common::SaveBagToFile( "stats.conf", propBag );
}


}
// namespace Attribute

// vim: ts=4 sw=4

