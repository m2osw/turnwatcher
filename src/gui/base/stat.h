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
#include <iostream>
#include <memory>
#include <string>

// MOLIB
//
#include "mo/mo_props.h"

// LOCAL
//
#include "base/common.h"

#include <memory>

namespace Attribute
{

class Stat
{
public:
	typedef std::shared_ptr<Stat>	pointer_t;

	Stat();

	molib::mo_name_t	id() const						{ return f_id; }
	void				id( const molib::mo_name_t val );

	int					legacy_id() const				{ return f_legacyId; }
	void				legacy_id( int val )			{ f_legacyId = val; }

	int					legacy_type() const				{ return f_legacyType; }
	void				legacy_type( int val )			{ f_legacyType = val; }

	molib::mo_name_t	abilityId() const				{ return f_abilityId; }
	void				abilityId( const molib::mo_name_t val );

	Glib::ustring		name() const					{ return f_name; }
	void				name( const Glib::ustring	& val );

	int					dice() const					{ return f_dice; }
	void				dice( const int val );

	int					faces() const					{ return f_faces; }
	void				faces( const int val );

	int					modifier() const				{ return f_modifier; }
	void				modifier( const int val );

	bool				deleted() const					{ return f_deleted; }
	void				deleted( const bool val );

	Glib::ustring		accel() const					{ return f_accel; }
	void				accel( const Glib::ustring	& val );
	
	bool				showOnToolbar() const			{ return f_showOnToolbar; }
	void				showOnToolbar( const bool val );
	
	bool				showOnHUD() const				{ return f_showOnHUD; }
	void				showOnHUD( const bool val );	

	bool				showMonsterOnHUD() const		{ return f_showMonsterOnHUD; }
	void				showMonsterOnHUD( const bool val );

	// If true, then this stat cannot be altered. Examples: initiative, hit points, level
	//
	bool				internal() const				{ return f_internal; }
	void				internal( const bool val );

	// If true, then this is an ability
	//
	bool				ability() const					{ return f_ability; }
	void				ability( const bool val );
	
	// This determines the order, smallest to largest (starting at 0)
	//
	int					order() const					{ return f_order; }
	void				order( const int val );

	virtual void		Copy( const pointer_t copy );
	virtual void		Load( molib::moPropBagRef& propBag );
	virtual void		Save( molib::moPropBagRef& propBag );

	typedef sigc::signal<void>	StatSignal;
	StatSignal					SignalChanged()	{ return f_statChanged; }	// Stat changed

private:
	molib::moName		f_idName;
	molib::moName		f_abilityIdName;
	molib::moName		f_legacyIdName;
	molib::moName		f_legacyTypeName;
	molib::moName		f_nameName;
	molib::moName		f_diceName;
	molib::moName		f_facesName;
	molib::moName		f_modifierName;
	molib::moName		f_deletedName;
	molib::moName		f_accelName;
	molib::moName		f_showOnToolbarName;
	molib::moName		f_showOnHUDName;
	molib::moName		f_showMonsterOnHUDName;
	molib::moName		f_internalName;
	molib::moName		f_abilityName;
	molib::moName		f_orderName;
	//
	molib::moName		f_id;
	int					f_legacyId;
	int					f_legacyType;
	molib::moName		f_abilityId;
	Glib::ustring		f_name;
	int					f_dice;				// # of dice
	int					f_faces;			// # of faces on dice (2d10 -> f_dice = 2 and f_faces = 10)
	int					f_modifier;			// + modifier to rolls (2d10+3 -> f_dice = 2 and f_faces = 10; f_modifier = 3)
	bool				f_deleted;
	Glib::ustring		f_accel;			// accelerator key
	bool				f_showOnToolbar;	// Show on toolbar to allow the user to roll, otherwise text field
	bool				f_showOnHUD;		// Show on player HUD
	bool				f_showMonsterOnHUD;	// Show stat for monster on HUD (this allows the GM to control private stats)
	bool				f_internal;			// If true, then this stat cannot be altered.
	bool				f_ability;			// If true, then this is an ability stat (e.g. Str, Dex, etc.).
	int					f_order;			// Order of stats as they appear on the UI

	StatSignal			f_statChanged;
};


/*****************************************************************************************************/

class Value
{
public:
	typedef std::shared_ptr<Value>	pointer_t;

	Value();

	Stat::pointer_t	stat() const					{ return f_stat; }
	void			stat( Stat::pointer_t val )		{ f_stat = val; }

	int				mod() const						{ return f_mod; }
	void			mod( const int val ) 			{ f_mod = val;  }

	int				roll() const					{ return f_roll; }
	void			roll( const int val ) 			{ f_roll = val;  }

	Glib::ustring	notes()							{ return f_notes; }
	void			notes( const char* val )		{ f_notes = val;  }

	virtual int		total() const					{ return f_mod; }

	void			Copy( const pointer_t copy );
	bool			Load( molib::moPropBagRef& propBag );
	void			Save( molib::moPropBagRef& propBag );

private:
	molib::moName		f_statName;
	molib::moName		f_modName;
	molib::moName		f_rollName;
	molib::moName		f_notesName;

	Stat::pointer_t		f_stat;
	int					f_mod;		// Extra modifier
	int					f_roll;		// The last dice roll
	Glib::ustring		f_notes;	// Used instead of roll value

	void				LoadLegacy( molib::moPropBagRef& propBag );
};

}
// namespace Attribute


// vim: ts=4 sw=4 syntax=cpp.doxygen

