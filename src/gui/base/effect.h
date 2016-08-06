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
#include <vector>
#include <memory>

// molib
//
#include "mo/mo_props.h"
#include "mo/mo_string.h"

// LOCAL
//
#include "base/common.h"

namespace Combatant
{
	class Character;
}

namespace Effects
{

class Effect
{
public:
	typedef std::shared_ptr<Effect>	pointer_t;
	typedef std::vector<pointer_t>	list_t;

	Effect();

	Glib::ustring	name()	const					{ return f_name; }
	void			name( const Glib::ustring& val )		{ f_name = val;  }

	Glib::ustring	description() const				{ return f_description; }
	void			description( const Glib::ustring& val )	{ f_description = val;  }

	int				totalRounds() const				{ return f_totalRounds; }
	void			totalRounds( const int val )	{ f_totalRounds = val;  }

	int				roundsUsed() const				{ return f_roundsUsed; }
	void			roundsUsed( const int val )		{ f_roundsUsed = val;  }

	int				startIn() const					{ return f_startIn; }
	void			startIn( const int val )		{ f_startIn = val; }

	int				tempHP() const					{ return f_tempHP; }
	void			tempHP( const int val )			{ f_tempHP = val; }

	int				hpBoost() const					{ return f_hpBoost; }
	void			hpBoost( const int val )		{ f_hpBoost = val; }

	int				roundsLeft() const				{ return f_totalRounds - f_roundsUsed; }

	typedef enum { Spell, Ability, Other } Type;
	//
	Type			type() const					{ return f_type; }
	void			type( const Type val )			{ f_type = val;  }

	void            Load( molib::moPropBagRef propBag );
	void            Save( molib::moPropBagRef propBag );
	void			Copy( pointer_t effect );

	void			apply   ( std::shared_ptr<Combatant::Character> ch, const bool force = false );
	void			unapply ( std::shared_ptr<Combatant::Character> ch );
	
private:
	Glib::ustring	f_name;
	Glib::ustring	f_description;
	int				f_totalRounds;
	int				f_roundsUsed;
	int				f_startIn;	// Effect takes place when this decrements to zero
	int				f_tempHP;
	int				f_hpBoost;
	Type			f_type;
	bool			f_isActive;

	void			internalApply   ( std::shared_ptr<Combatant::Character> ch );
	void			internalUnapply ( std::shared_ptr<Combatant::Character> ch );
};

}
// namespace Effects



// vim: ts=4 sw=4 noet syntax=cpp.doxygen

