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

// STL
#include <exception>

// LOCAL
//
#include "base/stat.h"
#include "base/StatManager.h"

using namespace molib;

namespace Attribute
{

Stat::Stat()
	: f_idName               ( "STAT_ID"             )
	, f_abilityIdName        ( "ABILITY_ID"          )
	, f_legacyIdName         ( "ID"                  )
	, f_legacyTypeName       ( "TYPE"                )
	, f_nameName             ( "NAME"                )
	, f_diceName             ( "DICE"                )
	, f_facesName            ( "FACES"               )
	, f_modifierName         ( "MODIFIER"            )
	, f_deletedName          ( "DELETED"             )
	, f_accelName            ( "ACCEL"               )
	, f_showOnToolbarName    ( "SHOW_ON_TOOLBAR"     )
	, f_showOnHUDName        ( "SHOW_ON_HUD"         )
	, f_showMonsterOnHUDName ( "SHOW_MONSTER_ON_HUD" )
	, f_internalName         ( "IS_INTERNAL"         )
	, f_abilityName          ( "IS_ABILITY"          )
	, f_orderName            ( "ORDER"               )
	, f_id                   ( "UNNAMED"             )
	, f_abilityId            ( "UNNAMED"             )
	, f_name                 ( "Unnamed"             )
	, f_legacyId             ( -1                    )
	, f_legacyType           ( -1                    )
	, f_dice                 ( 1                     )
	, f_faces                ( 20                    )
	, f_modifier             ( 0                     )
	, f_deleted              ( false                 )
	, f_showOnToolbar        ( false                 )
	, f_showOnHUD            ( false                 )
	, f_showMonsterOnHUD     ( false                 )
	, f_internal             ( false                 )
	, f_ability              ( false                 )
	, f_order                ( -1                    )
{
}


void Stat::id               ( const mo_name_t      val ) { f_id               = val; f_statChanged.emit(); }
void Stat::abilityId        ( const mo_name_t      val ) { f_abilityId        = val; f_statChanged.emit(); }
void Stat::name             ( const Glib::ustring& val ) { f_name             = val; f_statChanged.emit(); }
void Stat::dice             ( const int            val ) { f_dice             = val; f_statChanged.emit(); }
void Stat::faces            ( const int            val ) { f_faces            = val; f_statChanged.emit(); }
void Stat::modifier         ( const int            val ) { f_modifier         = val; f_statChanged.emit(); }
void Stat::deleted          ( const bool           val ) { f_deleted          = val; f_statChanged.emit(); }
void Stat::accel            ( const Glib::ustring& val ) { f_accel            = val; f_statChanged.emit(); }
void Stat::showOnToolbar    ( const bool           val ) { f_showOnToolbar    = val; f_statChanged.emit(); }
void Stat::showOnHUD        ( const bool           val ) { f_showOnHUD        = val; f_statChanged.emit(); }
void Stat::showMonsterOnHUD ( const bool           val ) { f_showMonsterOnHUD = val; f_statChanged.emit(); }
void Stat::internal         ( const bool           val ) { f_internal         = val; f_statChanged.emit(); }
void Stat::ability          ( const bool           val ) { f_ability          = val; f_statChanged.emit(); }
void Stat::order            ( const int            val ) { f_order            = val; f_statChanged.emit(); }


void Stat::Copy( const pointer_t copy )
{
	f_id               = copy->f_id;
	f_legacyId         = copy->f_legacyId;
	f_legacyType       = copy->f_legacyType;
	f_abilityId        = copy->f_abilityId;
	f_name             = copy->f_name;
	f_dice             = copy->f_dice;
	f_faces            = copy->f_faces;
	f_modifier         = copy->f_modifier;
	f_deleted          = copy->f_deleted;
	f_accel            = copy->f_accel;
	f_showOnToolbar    = copy->f_showOnToolbar;
	f_showOnHUD        = copy->f_showOnHUD;
	f_showMonsterOnHUD = copy->f_showMonsterOnHUD;
	f_internal         = copy->f_internal;
	f_ability          = copy->f_ability;
	f_order            = copy->f_order;
}


void Stat::Load( moPropBagRef& propBag )
{
	moPropStringRef id               ( f_idName               );
	moPropStringRef abilityId        ( f_abilityIdName        );
	moPropIntRef    legacyId         ( f_legacyIdName         );
	moPropIntRef    legacyType       ( f_legacyTypeName       );
	moPropStringRef name             ( f_nameName             );
	moPropIntRef    dice             ( f_diceName             );
	moPropIntRef    faces            ( f_facesName            );
	moPropIntRef    modifier         ( f_modifierName         );
	moPropIntRef    deleted          ( f_deletedName          );
	moPropStringRef accel            ( f_accelName            );
	moPropIntRef    showOnToolbar    ( f_showOnToolbarName    );
	moPropIntRef    showOnHUD        ( f_showOnHUDName        );
	moPropIntRef    showMonsterOnHUD ( f_showMonsterOnHUDName );
	moPropIntRef    internal         ( f_internalName         );
	moPropIntRef    ability          ( f_abilityName          );
	moPropIntRef    order            ( f_orderName            );

	id       			.Link( propBag );
	abilityId			.Link( propBag );
	legacyId 			.Link( propBag );
	legacyType			.Link( propBag );
	name     			.Link( propBag );
	dice     			.Link( propBag );
	faces    			.Link( propBag );
	modifier 			.Link( propBag );
	deleted  			.Link( propBag );
	accel    			.Link( propBag );
	showOnToolbar		.Link( propBag );
	showOnHUD			.Link( propBag );
	showMonsterOnHUD	.Link( propBag );
	internal			.Link( propBag );
	ability				.Link( propBag );
	order				.Link( propBag );

	if( id.HasProp() )
	{
		f_id 			= moName( static_cast<moWCString>(id) );
	}
	else if( legacyId.HasProp() && legacyType.HasProp() )
	{
		f_legacyId 		= legacyId;
		f_legacyType	= legacyType;
	}
	else
	{
		throw moError( "Bad Stat data for object!" );
	}

	if( abilityId.HasProp() )
	{
		f_abilityId = moName( static_cast<moWCString>(abilityId) );
	}
	else
	{
		f_abilityId = moName( "UNNAMED" );
	}
	
	f_name     = static_cast<moWCString>(name).c_str();
	f_dice     = dice;
	f_faces    = faces;
	f_modifier = modifier;
	f_deleted  = deleted? true: false;
	//
	if( showMonsterOnHUD.HasProp() && showOnToolbar.HasProp() && showOnHUD.HasProp() )
	{
		f_showOnToolbar		= showOnToolbar? true: false;
		f_showOnHUD			= showOnHUD? true: false;
		f_showMonsterOnHUD	= showMonsterOnHUD? true: false;
	}
	//
	if( accel.HasProp() )	
	{
		// This is a bug in moProps--if a blank string, then we should have a property
		//
		f_accel = static_cast<moWCString>(accel).c_str();
	}

	if( internal.HasProp() ) f_internal = internal;
	if( ability.HasProp()  ) f_ability  = ability;
	if( order.HasProp()    ) f_order    = order;
	
	if( f_legacyId != -1 )
	{
		f_showMonsterOnHUD = f_showOnToolbar = f_showOnHUD = f_internal = f_ability = false;
		
		auto statMgr = StatManager::Instance().lock();
		assert(statMgr);
	
		// We must assign the custom stat value for later editing in the StatEditor.
		// The idea is that certain stats are fixed, immutable, but others, like
		// Spot/Listen/Will are custom, plus new stats are also.
		//
			 if( f_name == "Spot" 		) { f_id = statMgr->spotId();  	f_showOnToolbar = true; f_accel = "s";	}
		else if( f_name == "Listen"		) { f_id = statMgr->listenId(); f_showOnToolbar = true; f_accel = "l";	}
		else if( f_name == "Will"		) { f_id = statMgr->willId();	f_showOnToolbar = true; f_accel = "w";	}
		else if( f_name == "Init"  		) { f_id = statMgr->initId();	f_internal = true;						}
		else if( f_name == "Hit Points"	) { f_id = statMgr->hpId();		f_internal = true;						}
		else if( f_name == "Level" 		) { f_id = statMgr->levelId();	f_internal = true;						}

#ifdef DEBUG
		std::cerr	<< "Legacy Id: " 	 << f_legacyId 
					<< ", Legacy Type: " << f_legacyType
					<< ", ID name= " 	 << f_name.c_str()
					<< ", NEW ID=" 		 << moWCString(moName(f_id)).c_str()
					<< std::endl;
#endif
	}

	f_statChanged.emit();
}


void Stat::Save( moPropBagRef& propBag )
{
	// Do not save if deleted
	//
	if( f_deleted ) return;

	moPropStringRef id               ( f_idName               );
	moPropStringRef abilityId        ( f_abilityIdName        );
	moPropStringRef name             ( f_nameName             );
	moPropIntRef    dice             ( f_diceName             );
	moPropIntRef    faces            ( f_facesName            );
	moPropIntRef    modifier         ( f_modifierName         );
	moPropIntRef    deleted          ( f_deletedName          );
	moPropStringRef accel            ( f_accelName            );
	moPropIntRef    showMonsterOnHUD ( f_showMonsterOnHUDName );
	moPropIntRef    showOnToolbar    ( f_showOnToolbarName    );
	moPropIntRef    showOnHUD        ( f_showOnHUDName        );
	moPropIntRef    internal         ( f_internalName         );
	moPropIntRef    ability          ( f_abilityName          );
	moPropIntRef    order            ( f_orderName            );

	// Create the properties first
	//
	id        		.NewProp();
	abilityId 		.NewProp();
	name      		.NewProp();
	dice      		.NewProp();
	faces     		.NewProp();
	modifier  		.NewProp();
	deleted   		.NewProp();
	accel     		.NewProp();
	showMonsterOnHUD.NewProp();
	showOnToolbar	.NewProp();
	showOnHUD		.NewProp();
	internal		.NewProp();
	ability			.NewProp();
	order			.NewProp();
	//
	id        		= static_cast<moWCString>(moName(f_id));
	abilityId 		= static_cast<moWCString>(moName(f_abilityId));
	name      		= f_name.c_str();
	dice      		= f_dice;
	faces     		= f_faces;
	modifier  		= f_modifier;
	deleted   		= f_deleted? 1: 0;
	accel     		= f_accel.c_str();
	showMonsterOnHUD= f_showMonsterOnHUD;
	showOnToolbar	= f_showOnToolbar;
	showOnHUD		= f_showOnHUD;
	internal		= f_internal;
	ability			= f_ability;
	order			= f_order;

	propBag += id;
	propBag += abilityId;
	propBag += name;
	propBag += dice;
	propBag += faces;
	propBag += modifier;
	propBag += deleted;
	propBag += accel;
	propBag += showMonsterOnHUD;
	propBag += showOnToolbar;
	propBag += showOnHUD;
	propBag += internal;
	propBag += ability;
	propBag	+= order;
}


/*****************************************************************************************************/


Value::Value() :
	f_statName ("STATID"),
	f_modName  ("MODIFIER"),
	f_rollName ("ROLL"    ),
	f_notesName("NOTES"    ),
	f_mod      (0),
	f_roll	   (0)
{
}


void Value::Copy( const pointer_t copy )
{
	f_stat	= copy->f_stat;
	f_mod	= copy->f_mod;
	f_roll	= copy->f_roll;
	f_notes	= copy->f_notes;
}


/// \brief Loads info from a legacy property bag
//
// We search for id/type combination in the stat manager, since we are reading this from a legacy
// configuration file or importing from a legacy tw file.
//
/// \param propBag	Bag containing legacy properties to convert	
//
/// \sa Load
//
void Value::LoadLegacy( moPropBagRef& propBag )
{
	// Figure out legacy stuff...we have to go by id and type
	//
	moPropIntRef	legacy_id  ( "ID"   );
	moPropIntRef	legacy_type( "TYPE" );
	//
	legacy_id  .Link( propBag );
	legacy_type.Link( propBag );
	//
	if( !(legacy_id.HasProp() && legacy_type.HasProp()) )
	{
		// Can't go further--something is terribly wrong!
		//
		throw moError( "Cannot find legacy id and/or legacy value prop!" );
	}
	//
	auto statMgr = StatManager::Instance().lock();
	//
    f_stat = statMgr->GetLegacyStat( static_cast<int>(legacy_id), static_cast<int>(legacy_type) );
	//
	if( !f_stat )
	{
#ifdef DEBUG
        std::cerr << "Legacy id " << static_cast<int>(legacy_id) << ", type " << static_cast<int>(legacy_type)
				<< " was neither FOUND nor CREATED by stat manager!" << std::endl;
#endif
		throw moError( "Cannot find legacy id and/or legacy value prop in StatManager!" );
	}
	assert( f_stat );
}


/// \brief Loads info from a property bag
//
// We get out of the bag the stat, modifier and roll settings.
//
/// \param propBag	Bag containing properties to load
//
/// \sa LoadLegacy, Save
/// \return true if stat was found
//
bool Value::Load( moPropBagRef& propBag )
{
	moPropStringRef stat  ( f_statName  );
	moPropIntRef    mod   ( f_modName   );
	moPropIntRef    roll  ( f_rollName  );
	moPropStringRef notes ( f_notesName );
	//
	stat.Link( propBag );
	mod .Link( propBag );
	roll.Link( propBag );
	notes.Link( propBag );

	if( stat.HasProp() )
	{
		f_stat = StatManager::Instance().lock()->GetStat( static_cast<moName>(stat) );
		if( !f_stat )
		{
			return false;
		}
	}
	else
	{
		LoadLegacy( propBag );
	}
	//
	assert(f_stat);
	//
	if( mod.HasProp() )
	{
		f_mod = mod;
	}
	else
	{
		f_mod = 0;
	}
	//
	if( roll.HasProp() )
	{
		f_roll = roll;
	}
	else
	{
		f_roll = 0;
	}
	//
	if( notes.HasProp() )
	{
		f_notes = static_cast<moWCString>(notes).c_str();
	}
	else
	{
		f_notes = "";
	}

	return true;
}


/// \brief Save values to property bag for persistence
//
/// \param propBag	Bag containing properties to load
//
/// \sa Load
void Value::Save( moPropBagRef& propBag )
{
	moPropStringRef stat  ( f_statName  );
	moPropIntRef    mod   ( f_modName   );
	moPropIntRef    roll  ( f_rollName  );
	moPropStringRef notes ( f_notesName );
	//
	stat.NewProp();
	mod .NewProp();
	roll.NewProp();
	notes.NewProp();
	//
	stat  = static_cast<moWCString>(static_cast<moName>(f_stat->id()));
	mod   = f_mod;
	roll  = f_roll;
	notes = f_notes.c_str();
	//
	propBag += stat;
	propBag += mod;
	propBag += roll;
	propBag += notes;
}


}
// namespace Attribute


// vim: ts=4 sw=4 noet syntax=cpp.doxygen

