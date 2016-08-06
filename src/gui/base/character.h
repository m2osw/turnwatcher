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
#include <iostream>
#include <string>
#include <memory>

// GLIBmm
//
#include <glibmm.h>

// MOLIB
//
#include "mo/mo_props.h"

// LOCAL
//
#include "base/common.h"
#include "base/stat.h"
#include "base/LegacyCharacter.h"
#include "base/effect.h"

namespace Combatant
{

// Publically accessible character status
//
typedef enum { HNormal, Stabilized, Disabled, Dying, Dead }	Health;
typedef enum { Normal, Delayed, Readied }					Status;


class Character
{
public:
	typedef std::shared_ptr<Character> pointer_t;
	typedef std::vector<pointer_t>	   list_t;

	Character();
	virtual ~Character();

	void				Copy( pointer_t ch );
	void				Duplicate( pointer_t ch );
	void				Load( molib::moPropBagRef& propBag );
	void				Save( molib::moPropBagRef& propBag );
    void				Copy( LegacyCharacter::pointer_t lch );

	Glib::ustring		name() const		  					{ return f_name; }
	void				name( const Glib::ustring& val )		{ f_name = val;	f_signalChanged.emit();	}
	Glib::ustring		public_name() const		  				{ return f_publicName; }
	void				public_name( const Glib::ustring& val ) { f_publicName = val; f_signalChanged.emit(); }
	Glib::ustring		notes() const							{ return f_notes; }
	void				notes( const Glib::ustring& val )		{ f_notes = val; f_signalChanged.emit(); }
	bool 				monster() const							{ return f_monster; }
	void				monster( bool val )						{ f_monster = val; f_signalChanged.emit(); }

	Health	 			health();
	Status	 			status();
	void				status( const Status st );
	Glib::ustring		status_string();
	int					status_sort();

	int					HitDice() const				{ return f_hitDice; }
	void				HitDice( int val )			{ f_hitDice = val;  f_signalChanged.emit(); }

	Attribute::Value::pointer_t	getStat( const molib::mo_name_t id )	{ return GetStat( id ); }

	int					getMod ( const molib::mo_name_t id, const bool with_ability = false )
									{ return getMod( GetStat( id ), with_ability ); }
	int					getMod ( Attribute::Value::pointer_t stat, const bool with_ability = false );

	void				setMod ( const molib::mo_name_t id, const int mod )	{ setMod( GetStat( id ), mod ); }
	void				setMod ( Attribute::Value::pointer_t stat, const int mod );

	int					getRoll( const molib::mo_name_t id )						{ return getRoll( GetStat( id ) ); }
	int					getRoll( Attribute::Value::pointer_t stat );
	//
	void				setRoll( const molib::mo_name_t id, const int roll )		{ setRoll( GetStat( id ), roll ); }
	void				setRoll( Attribute::Value::pointer_t stat, const int roll );
	
	Glib::ustring	getNotes( const molib::mo_name_t id )						{ return getNotes( GetStat( id ) ); }
	Glib::ustring	getNotes( Attribute::Value::pointer_t stat );
	//
	void				setNotes( const molib::mo_name_t id, const char* notes )	{ setNotes( GetStat( id ), notes ); }
	void				setNotes( Attribute::Value::pointer_t stat, const char* notes );

	int					maxHP();
	int					baseHP() const				{ return f_baseHP;				}
	void				baseHP( int val )			{ f_baseHP = val;	f_signalChanged.emit();	}
	int					tempHP() const				{ return f_tempHP;				}
	void				tempHP( int val )			{ f_tempHP = val;	f_signalChanged.emit();	}
	int					hitDice() const				{ return f_hitDice;				}
	void				hitDice( int val )			{ f_hitDice = val;	f_signalChanged.emit();	}
	int					damage() const				{ return f_damage;				}
	void				damage( int hp )			{ f_damage = hp;	f_signalChanged.emit();	}
	bool				stabilized() const			{ return (bool) (int) f_stabilized; }
	void				stabilized( bool val )		{ f_stabilized = val;	f_signalChanged.emit();	}
	bool				justdropped() const			{ return (bool) (int) f_justdropped; }
	void				justdropped( bool val )		{ f_justdropped = val;	f_signalChanged.emit();	}
	int					hitpoints()					{ return maxHP() - f_damage; }

	bool				deleted() const				{ return f_deleted; }
	void				deleted( const bool val )	{ f_deleted = val; }

	int					makeRoll( const molib::mo_name_t id );
	void				makeAllRolls();
	int					getPosition() const;
	void				setPosition( const int pos );
	int					getSubPosition() const;
	void				setSubPosition( const int pos );
	int					getManualPos() const;
	void				setManualPos( const int pos );
	int					getForcePosition() const		{ return f_forcePosition; }
	void				setForcePosition( const int pos )	{ f_forcePosition = pos; }

	int                      addEffect   ( Effects::Effect::pointer_t 	effect  );
	Effects::Effect::pointer_t getEffect   ( const int					id  	);
	void                     getEffects  ( Effects::Effect::list_t&   	effects );
	void                     setEffects  ( const Effects::Effect::list_t& effects );
	void                     removeEffect( const int                    id      );

	typedef sigc::signal<void>	VoidSignal;
	VoidSignal			signal_changed() { return f_signalChanged; }

private:
	// These are maps of character stats. These can be ability scores, saves or skill checks.
	// It also holds the initiative roll. You can use the StatName enum here, or use a negative
	// number to represent a custom stat.
	//
	typedef std::map<molib::mo_name_t,Attribute::Value::pointer_t>		ValueMap;

	// Character stats
	//
	sigc::connection				f_statConnection;
	ValueMap						f_values;

	// Signals
	// 
	VoidSignal						f_signalChanged;

	// Other character information
	//
	Glib::ustring		f_name;			// name of character
	Glib::ustring		f_publicName;	// public name of character that shows up on the HUD window
	Glib::ustring		f_notes;		// Misc notes
	bool				f_monster;		// is this character a monster or pc?

	int					f_hitDice;		// Hitdice for the character
	int					f_baseHP;		// Base hitpoints the character has
	int					f_tempHP;
	int					f_damage;		// Current damage
	bool				f_stabilized;	// If dying, character may be stabilized
	bool				f_justdropped;	// If true, character just dropped below 0 hitpoints

	Status				f_status;		// normal, delayed or readied action
	int					f_position;		// the real initiative position (1 based, if 0, not sorted)
	int					f_subPosition;	// tie-breaker for similar initiatives.
	int					f_manualPos;	// if the character is moved manually in initiative order (delayed, readied or move)

	struct EffectEntry
	{
		bool						f_deleted;
		int							f_id;
		Effects::Effect::pointer_t	f_effect;

		EffectEntry() : f_deleted(true), f_id(-1) {}
		EffectEntry( const int id, Effects::Effect::pointer_t effect )
			: f_deleted(false), f_id(id), f_effect(effect) {}
	};

	int					f_forcePosition;
	bool				f_deleted;

	typedef std::vector<EffectEntry>	EffectEntries;
	EffectEntries		f_effects;

	void	LoadEffects( molib::moPropBagRef _effectsBag );
	void	SaveEffects( molib::moPropBagRef _effectsBag );

	void	CopyStats( pointer_t ch );
	void	LoadStats( molib::moPropBagRef& propBag );
	void	SaveStats( molib::moPropBagRef& propBag );
	void	AddStat();

	Attribute::Value::pointer_t	GetStat( const molib::mo_name_t id );
	Attribute::Value::pointer_t	CreateStat( Attribute::Stat::pointer_t stat );

	std::string				GetStatString( const std::string& health_str );
	int						GetHpPercent();

	// StatManager
	//
	void OnStatsChanged();
};

}
// namespace Combatant


// vim: ts=4 sw=4 syntax=cpp.doxygen

